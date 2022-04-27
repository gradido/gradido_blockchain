#include "gradido_blockchain/model/IGradidoBlockchain.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionTransfer.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/lib/Decay.h"

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

			auto sender_pubkey = mProtoTransfer.sender().pubkey();

			mMinSignatureCount = 1;
			auto mm = MemoryManager::getInstance();
			auto pubkey_copy = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
			memcpy(*pubkey_copy, sender_pubkey.data(), crypto_sign_PUBLICKEYBYTES);
			mRequiredSignPublicKeys.push_back(pubkey_copy);

			mIsPrepared = true;

			return 0;
		}

		bool TransactionTransfer::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, 
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/)
			const
		{
			LOCK_RECURSIVE;
			auto mm = MemoryManager::getInstance();
			// cleanup after itself
			auto amount = MathMemory::create();
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
				
				if (recipient_pubkey.size() != crypto_sign_PUBLICKEYBYTES) {
					throw TransactionValidationInvalidInputException("invalid size", "recipient", "public key");
				}
				if (sender.pubkey().size() != crypto_sign_PUBLICKEYBYTES) {
					throw TransactionValidationInvalidInputException("invalid size", "sender", "public key");
				}
				if (0 == memcmp(sender.pubkey().data(), recipient_pubkey.data(), crypto_sign_PUBLICKEYBYTES)) {
					throw TransactionValidationException("sender and recipient are the same");
				}
				auto empty = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
				memset(*empty, 0, crypto_sign_PUBLICKEYBYTES);
				if (0 == memcmp(sender.pubkey().data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
					mm->releaseMemory(empty);
					throw TransactionValidationInvalidInputException("empty", "sender", "public key");
				}
				if (0 == memcmp(recipient_pubkey.data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
					mm->releaseMemory(empty);
					throw TransactionValidationInvalidInputException("empty", "recipient", "public key");
				}
				mm->releaseMemory(empty);
			}

			if ((level & TRANSACTION_VALIDATION_SINGLE_PREVIOUS) == TRANSACTION_VALIDATION_SINGLE_PREVIOUS)
			{
				assert(blockchain);
				assert(parentGradidoBlock);
				auto finalBalanceTransaction = blockchain->calculateAddressBalance(getSenderPublicKeyString(), getCoinColor(), parentGradidoBlock->getReceivedAsTimestamp());
				auto finalBalance = MathMemory::create();
				mpfr_swap(finalBalanceTransaction, finalBalance->getData());
				mm->releaseMathMemory(finalBalanceTransaction);
				std::string amountString, balanceString;
				amountToString(&amountString, amount->getData());
				amountToString(&balanceString, finalBalance->getData());
				printf("amount: %s, balance: %s\n", amountString.data(), balanceString.data());
				if (mpfr_cmp(amount->getData(), finalBalance->getData()) > 0) {
					// if op1 > op2
					throw InsufficientBalanceException("not enough Gradido Balance for send coins", amount->getData(), finalBalance->getData());
				}
				mm->releaseMathMemory(finalBalanceTransaction);
			}
			
			return true;
		}

		std::vector<MemoryBin*> TransactionTransfer::getInvolvedAddresses() const
		{
			auto mm = MemoryManager::getInstance();
			auto senderPubkeySize = mProtoTransfer.sender().pubkey().size();
			auto senderPubkey = mm->getMemory(senderPubkeySize);
			memcpy(*senderPubkey, mProtoTransfer.sender().pubkey().data(), senderPubkeySize);

			auto recipientPubkeySize = mProtoTransfer.recipient().size();
			auto recipientPubkey = mm->getMemory(recipientPubkeySize);
			memcpy(*recipientPubkey, mProtoTransfer.recipient().data(), recipientPubkeySize);
			return { senderPubkey, recipientPubkey };
		}

		uint32_t TransactionTransfer::getCoinColor() const
		{
			return mProtoTransfer.sender().coin_color();
		}

		bool TransactionTransfer::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const TransactionTransfer*>(pairingTransaction);
			auto mm = MemoryManager::getInstance();
			bool belongToUs = true;

			if (getCoinColor() != pair->getCoinColor()) {
				belongToUs = false;
			}
			if (getAmount() != pair->getAmount()) {
				belongToUs = false;
			}
			auto senderPubkey = getSenderPublicKey();
			auto senderPubkeyPair = pair->getSenderPublicKey();
			if (!senderPubkey->isSame(senderPubkeyPair)) {
				belongToUs = false;
			}
			mm->releaseMemory(senderPubkey); mm->releaseMemory(senderPubkeyPair);

			auto recipientPubkey = getRecipientPublicKey();
			auto recipientPubkeyPair = pair->getRecipientPublicKey();
			if (!recipientPubkey->isSame(recipientPubkeyPair)) {
				belongToUs = false;
			}
			mm->releaseMemory(recipientPubkey); mm->releaseMemory(recipientPubkeyPair);
			return belongToUs;
		}

		MemoryBin* TransactionTransfer::getSenderPublicKey() const
		{
			auto bin = MemoryManager::getInstance()->getMemory(mProtoTransfer.sender().pubkey().size());
			bin->copyFromProtoBytes(mProtoTransfer.sender().pubkey());
			return bin;
		}
		MemoryBin* TransactionTransfer::getRecipientPublicKey() const
		{
			auto bin = MemoryManager::getInstance()->getMemory(mProtoTransfer.recipient().size());
			bin->copyFromProtoBytes(mProtoTransfer.recipient());
			return bin;
		}
	}
}
