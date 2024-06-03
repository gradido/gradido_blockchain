#include "gradido_blockchain/model/IGradidoBlockchain.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionTransfer.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/lib/Decay.h"
#include "gradido_blockchain/memory/MPFRBlock.h"

#include <sodium.h>


namespace model {
	namespace gradido {

		// ********************************************************************************************************************************

		TransactionTransfer::TransactionTransfer(const proto::gradido::GradidoTransfer& protoTransfer)
			: mProtoTransfer(protoTransfer)
		{

		}

		TransactionTransfer::~TransactionTransfer()
		{

		}

		int TransactionTransfer::prepare()
		{
			LOCK_RECURSIVE;

			if (mIsPrepared) return 0;

			mMinSignatureCount = 1;
			mRequiredSignPublicKeys.push_back(mProtoTransfer.sender().pubkey());

			mIsPrepared = true;

			return 0;
		}

		bool TransactionTransfer::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const ConfirmedTransaction* parentConfirmedTransaction/* = nullptr*/)
			const
		{
			LOCK_RECURSIVE;
			// cleanup after itself
			auto amount = memory::MPFRBlock::create();
			auto sender = mProtoTransfer.sender();

			if (mpfr_set_str(amount->getData(), sender.amount().data(), 10, gDefaultRound)) {
				throw TransactionValidationInvalidInputException("amount cannot be parsed to a number", "amount", "string");
			}

			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE)
			{
				auto recipient_pubkey = mProtoTransfer.recipient();
				//auto amount = sender.amount();

				if (!sender.amount().size()) {
					throw TransactionValidationInvalidInputException("amount is empty", "amount", "string");
				}
				else if (mpfr_cmp_si(amount->getData(), 0) <= 0) {
					throw TransactionValidationInvalidInputException("zero or negative amount", "amount", "string");
				}
				validate25519PublicKey(mProtoTransfer.recipient(), "recipient");
				validate25519PublicKey(sender.pubkey(), "sender");
				
				if (mProtoTransfer.recipient() == sender.pubkey()) {
					throw TransactionValidationException("sender and recipient are the same");
				}
			}

			if (blockchain) {
				if (getCoinCommunityId() == blockchain->getCommunityId()) {
					throw TransactionValidationInvalidInputException(
						"coinGroupId shouldn't be set if it is the same as blockchain group alias",
						"coinGroupId", "string or UUID"
					);
				}
			}

			if ((level & TRANSACTION_VALIDATION_SINGLE_PREVIOUS) == TRANSACTION_VALIDATION_SINGLE_PREVIOUS)
			{
				assert(blockchain);
				assert(parentConfirmedTransaction);
				auto finalBalanceTransaction = blockchain->calculateAddressBalance(
					getSenderPublicKey(), 
					getCoinCommunityId(),
					parentConfirmedTransaction->getConfirmedAtAsTimepoint(),
					parentConfirmedTransaction->getID() + 1
				);
				auto finalBalance = memory::MPFRBlock::create();
				mpfr_swap(finalBalanceTransaction, finalBalance->getData());

				if (mpfr_cmp(amount->getData(), finalBalance->getData()) > 0) {
					// if op1 > op2
					throw InsufficientBalanceException("not enough Gradido Balance for send coins", amount->getData(), finalBalance->getData());
				}
			}
			if ((level & TRANSACTION_VALIDATION_CONNECTED_GROUP) == TRANSACTION_VALIDATION_CONNECTED_GROUP) {
				assert(blockchain);
				// check if sender address was registered
				auto senderAddressType = blockchain->getAddressType(getSenderPublicKey());
				if (proto::gradido::RegisterAddress_AddressType::RegisterAddress_AddressType_NONE == senderAddressType) {
					throw WrongAddressTypeException("sender address not registered", senderAddressType, getSenderPublicKey());
				}
				// check if recipient address was registered
				auto recipientAddressType = blockchain->getAddressType(getRecipientPublicKey());
				if (proto::gradido::RegisterAddress_AddressType::RegisterAddress_AddressType_NONE == recipientAddressType) {
					throw WrongAddressTypeException("recipient address not registered", recipientAddressType, getRecipientPublicKey());
				}
			}

			return true;
		}

		std::vector<std::string_view> TransactionTransfer::getInvolvedAddresses() const
		{
			return { 
				mProtoTransfer.sender().pubkey(),
				mProtoTransfer.recipient() 
			};
		}

		bool TransactionTransfer::isInvolved(const std::string& pubkeyString) const
		{
			return
				mProtoTransfer.sender().pubkey() == pubkeyString ||
				mProtoTransfer.recipient() == pubkeyString
				;
		}

		const std::string& TransactionTransfer::getCoinCommunityId() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoTransfer.sender().community_id();
		}

		bool TransactionTransfer::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const TransactionTransfer*>(pairingTransaction);

			if (getCoinCommunityId() != pair->getCoinCommunityId() ||
				getAmount() != pair->getAmount() ||
				getSenderPublicKey() != pair->getSenderPublicKey() ||
				getRecipientPublicKey() != pair->getRecipientPublicKey()) {
				return false;
			}
			return true;
		}

		const std::string& TransactionTransfer::getAmount() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoTransfer.sender().amount();
		}
		const std::string& TransactionTransfer::getSenderPublicKey() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoTransfer.sender().pubkey();
		}
		const std::string& TransactionTransfer::getRecipientPublicKey() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoTransfer.recipient();
		}

		std::string TransactionTransfer::toDebugString() const
		{
			std::string result;
			result += "transfer: " + getAmount() + " GDD\n";
			result += "from: " + DataTypeConverter::binToHex(getSenderPublicKey()) + "\n";
			result += "to:   " + DataTypeConverter::binToHex(getRecipientPublicKey()) + "\n";
			return result;
		}

	}
}
