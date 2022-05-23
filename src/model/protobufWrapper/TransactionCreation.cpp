#include "gradido_blockchain/model/protobufWrapper/TransactionCreation.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/lib/Decay.h"
#include "Poco/DateTimeFormatter.h"

#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/GradidoBlock.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"

#include <sodium.h>


namespace model {
	namespace gradido {

		TransactionCreation::TransactionCreation(const proto::gradido::GradidoCreation& protoCreation)
			: mProtoCreation(protoCreation)
		{
		}

		TransactionCreation::~TransactionCreation()
		{

		}

		int TransactionCreation::prepare()
		{
			if (mIsPrepared) return 0;

			if (!mProtoCreation.has_recipient()) {
				throw TransactionValidationInvalidInputException("not exist", "recipient", "TransferAmount");
			}
			auto recipient_amount = mProtoCreation.recipient();

			auto receiverPublic = recipient_amount.pubkey();
			if (receiverPublic.size() != crypto_sign_PUBLICKEYBYTES) {
				throw TransactionValidationInvalidInputException("invalid size", "pubkey", "public key");
			}

			//
			mMinSignatureCount = 1;
			auto mm = MemoryManager::getInstance();
			auto pubkey_copy = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
			memcpy(*pubkey_copy, receiverPublic.data(), crypto_sign_PUBLICKEYBYTES);
			mForbiddenSignPublicKeys.push_back(pubkey_copy);

			mIsPrepared = true;
			return 0;
		}

		std::string TransactionCreation::getTargetDateString() const
		{
			// proto format is seconds, poco timestamp format is microseconds
			Poco::Timestamp pocoStamp(mProtoCreation.target_date().seconds() * 1000 * 1000);
			//Poco::DateTime(pocoStamp);
			return Poco::DateTimeFormatter::format(pocoStamp, "%d. %b %y");
		}

		Poco::DateTime TransactionCreation::getTargetDate() const
		{
			return DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date());
		}

		bool TransactionCreation::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				auto mm = MemoryManager::getInstance();
				mpfr_ptr amount = mm->getMathMemory();
				if (mpfr_set_str(amount, mProtoCreation.recipient().amount().data(), 10, gDefaultRound)) {
					throw TransactionValidationInvalidInputException("amount cannot be parsed to a number", "amount", "string");
				}
				if(mpfr_cmp_si(amount, 1000) > 0) {
					throw TransactionValidationInvalidInputException("creation amount to high, max 1000 per month", "amount", "string");
				}
				if (mpfr_cmp_si(amount, 1) < 0) {
					throw TransactionValidationInvalidInputException("creation amount to low, min 1 GDD", "amount", "string");
				}
				mm->releaseMathMemory(amount);
				if (mProtoCreation.recipient().pubkey().size() != crypto_sign_PUBLICKEYBYTES) {
					throw TransactionValidationInvalidInputException("invalid size", "recipient pubkey", "public key");
				}
				auto empty = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
				memset(*empty, 0, crypto_sign_PUBLICKEYBYTES);
				if (0 == memcmp(mProtoCreation.recipient().pubkey().data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
					throw TransactionValidationInvalidInputException("empty", "recipient pubkey", "public key");
				}
			}

			if (blockchain) {
				if (getCoinGroupId() == blockchain->getGroupId()) {
					throw TransactionValidationInvalidInputException(
						"coinGroupId shouldn't be set if it is the same as blockchain group alias",
						"coinGroupId", "string or UUID"
					);
				}
			}

			if ((level & TRANSACTION_VALIDATION_DATE_RANGE) == TRANSACTION_VALIDATION_DATE_RANGE)
			{
				Poco::DateTime targetDate = Poco::Timestamp(mProtoCreation.target_date().seconds() * Poco::Timestamp::resolution());
				assert(blockchain);
				assert(parentGradidoBlock);

				auto pubkey = mProtoCreation.recipient().pubkey();
				auto mm = MemoryManager::getInstance();
				auto amount = mm->getMathMemory();
				mpfr_ptr sum = mm->getMathMemory();
				if (mpfr_set_str(amount, mProtoCreation.recipient().amount().data(), 10, gDefaultRound)) {
					throw TransactionValidationInvalidInputException("amount cannot be parsed to a number", "amount", "string");
				}

				blockchain->calculateCreationSum(pubkey, targetDate.month(), targetDate.year(), parentGradidoBlock->getReceivedAsTimestamp(), sum);
				mpfr_add(sum, sum, amount, gDefaultRound);

				auto id = parentGradidoBlock->getID();
				int lastId = 0;
				if (blockchain->getLastTransaction()) {
					lastId = blockchain->getLastTransaction()->getID();
				}
				if (id <= lastId) {
					// this transaction was already added to blockchain and therefor also added in calculateCreationSum
					mpfr_sub(sum, sum, amount, gDefaultRound);
				}
				// TODO: replace with variable, state transaction for group
				if (mpfr_cmp_si(sum, 1000) > 0) {
					throw TransactionValidationInvalidInputException("creation more than 1.000 GDD per month not allowed", "amount");
				}
				mm->releaseMathMemory(sum);
				mm->releaseMathMemory(amount);
			}

			if ((level & TRANSACTION_VALIDATION_CONNECTED_GROUP) == TRANSACTION_VALIDATION_CONNECTED_GROUP) {
				assert(blockchain);
				auto addressType = blockchain->getAddressType(getRecipientPublicKeyString());
				if (addressType != proto::gradido::RegisterAddress_AddressType_HUMAN) {
					throw WrongAddressTypeException("wrong address type for creation", addressType);
				}
			}

			return true;
		}

		bool TransactionCreation::validateTargetDate(uint64_t receivedSeconds) const
		{
			auto target_date = Poco::DateTime(DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date()));
			auto received = Poco::DateTime(Poco::Timestamp(receivedSeconds * Poco::Timestamp::resolution()));
			//  2021-09-01 02:00:00 | 2021-12-04 01:22:14
			if (target_date.year() == received.year())
			{
				if (target_date.month() + 2 < received.month()) {
					throw TransactionValidationInvalidInputException("year is the same, target date month is more than 2 month in past", "target_date", "date time");
				}
				if (target_date.month() > received.month()) {
					throw TransactionValidationInvalidInputException("year is the same, target date month is in future", "target_date", "date time");
				}
			}
			else if (target_date.year() > received.year())
			{
				throw TransactionValidationInvalidInputException("target date year is in future", "target_date", "date time");
			}
			else if (target_date.year() + 1 < received.year())
			{
				throw TransactionValidationInvalidInputException("target date year is in past", "target_date", "date time");
			}
			else
			{
				// target_date.year +1 == now.year
				if (target_date.month() + 2 < received.month() + 12) {
					throw TransactionValidationInvalidInputException("target date is more than 2 month in past", "target_date", "date time");
				}
			}
			return true;
		}

		std::vector<MemoryBin*> TransactionCreation::getInvolvedAddresses() const
		{
			auto mm = MemoryManager::getInstance();
			auto recipientPubkeySize = mProtoCreation.recipient().pubkey().size();
			auto recipientPubkey = mm->getMemory(recipientPubkeySize);
			memcpy(*recipientPubkey, mProtoCreation.recipient().pubkey().data(), recipientPubkeySize);
			return { recipientPubkey };
		}

		const std::string& TransactionCreation::getCoinGroupId() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().group_id();
		}

		bool TransactionCreation::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const TransactionCreation*>(pairingTransaction);
			bool belongToUs = true;
			auto mm = MemoryManager::getInstance();
			if (getTargetDate() != pair->getTargetDate()) {
				belongToUs = false;
			}
			if (getAmount() != pair->getAmount()) {
				belongToUs = false;
			}

			if (getCoinGroupId() != pair->getCoinGroupId()) {
				belongToUs = false;
			}

			auto recipient = getRecipientPublicKey();
			auto pairRecipient = pair->getRecipientPublicKey();
			if (!recipient->isSame(pairRecipient)) {
				belongToUs = false;
			}
			mm->releaseMemory(recipient); mm->releaseMemory(pairRecipient);
			return belongToUs;
		}

		const std::string& TransactionCreation::getAmount() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().amount();
		}


		MemoryBin* TransactionCreation::getRecipientPublicKey() const
		{
			auto bin = MemoryManager::getInstance()->getMemory(mProtoCreation.recipient().pubkey().size());
			bin->copyFromProtoBytes(mProtoCreation.recipient().pubkey());
			return bin;
		}

		const std::string& TransactionCreation::getRecipientPublicKeyString() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().pubkey();
		}

		std::string TransactionCreation::toDebugString() const
		{
			std::string result;
			auto targetDateString = Poco::DateTimeFormatter::format(getTargetDate(), Poco::DateTimeFormat::SORTABLE_FORMAT);
			result += "creation: " + getAmount() + " GDD, target: " + targetDateString + "\n";
			result += "to:   " + DataTypeConverter::binToHex(getRecipientPublicKeyString()) + "\n";
			return std::move(result);
		}

	}
}

