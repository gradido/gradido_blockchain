#include "gradido_blockchain/model/protobufWrapper/TransactionTransfer.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"

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

		bool TransactionTransfer::validate(TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, IGradidoBlockchain* blockchain/* = nullptr*/) const
		{
			LOCK_RECURSIVE;
			
			auto sender = mProtoTransfer.sender();
			auto recipient_pubkey = mProtoTransfer.recipient();
			
			auto amount = sender.amount();
			auto mm = MemoryManager::getInstance();
			if (0 == amount) {
				throw TransactionValidationInvalidInputException("amount is empty", "amount", "integer");
			}
			else if (amount < 0) {
				throw TransactionValidationInvalidInputException("negative amount", "amount", "integer");
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
	}
}
