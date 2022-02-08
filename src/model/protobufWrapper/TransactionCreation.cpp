#include "gradido_blockchain/model/protobufWrapper/TransactionCreation.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "Poco/DateTimeFormatter.h"

#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"

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

		std::string TransactionCreation::getTargetDateString()
		{
			// proto format is seconds, poco timestamp format is microseconds
			Poco::Timestamp pocoStamp(mProtoCreation.target_date().seconds() * 1000 * 1000);
			//Poco::DateTime(pocoStamp);
			return Poco::DateTimeFormatter::format(pocoStamp, "%d. %b %y");
		}

		bool TransactionCreation::validate(TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, IGradidoBlockchain* blockchain/* = nullptr*/) const
		{
			auto target_date = Poco::DateTime(DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date()));
			// TODO: check with created date not with current clock date
			auto now = Poco::DateTime();
			auto mm = MemoryManager::getInstance();
			//  2021-09-01 02:00:00 | 2021-12-04 01:22:14
			if (target_date.year() == now.year()) 
			{
				if (target_date.month() + 2 < now.month()) {
					throw TransactionValidationInvalidInputException("year is the same, target date month is more than 2 month in past", "target_date", "date time");
				}
				if (target_date.month() > now.month()) {
					throw TransactionValidationInvalidInputException("year is the same, target date month is in future", "target_date", "date time");
				}
			}
			else if(target_date.year() > now.year()) 
			{
				throw TransactionValidationInvalidInputException("target date year is in future", "target_date", "date time");
			}
			else if(target_date.year() +1 < now.year())
			{
				throw TransactionValidationInvalidInputException("target date year is in past", "target_date", "date time");
			}
			else 
			{
				// target_date.year +1 == now.year
				if (target_date.month() + 2 < now.month() + 12) {
					throw TransactionValidationInvalidInputException("target date is more than 2 month in past", "target_date", "date time");
				}
			}
			auto amount = mProtoCreation.recipient().amount();
			if (amount > 1000 * 10000) {
				throw TransactionValidationInvalidInputException("creation amount to high, max 1000 per month", "amount", "integer");
			}
			if (amount < 10000) {
				throw TransactionValidationInvalidInputException("creation amount to low, min 1 GDD", "amount", "integer");
			}
			
			if (mProtoCreation.recipient().pubkey().size() != crypto_sign_PUBLICKEYBYTES) {
				throw TransactionValidationInvalidInputException("invalid size", "recipient pubkey", "public key");
			}			
			auto empty = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
			memset(*empty, 0, crypto_sign_PUBLICKEYBYTES);
			if (0 == memcmp(mProtoCreation.recipient().pubkey().data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
				throw TransactionValidationInvalidInputException("empty", "recipient pubkey", "public key");
			}
			
			return true;
		}


	}
}

