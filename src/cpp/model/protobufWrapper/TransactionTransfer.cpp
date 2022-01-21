#include "TransactionTransfer.h"
#include "TransactionValidationExceptions.h"

#include <sodium.h>

namespace model {
	namespace gradido {

		// ********************************************************************************************************************************

		TransactionTransfer::TransactionTransfer(const std::string& memo, const proto::gradido::GradidoTransfer& protoTransfer)
			: TransactionBase(memo), mProtoTransfer(protoTransfer)
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
			auto pubkey_copy = mm->getFreeMemory(crypto_sign_PUBLICKEYBYTES);
			memcpy(*pubkey_copy, sender_pubkey.data(), crypto_sign_PUBLICKEYBYTES);
			mRequiredSignPublicKeys.push_back(pubkey_copy);

			mIsPrepared = true;

			return 0;
		}

		bool TransactionTransfer::validate()
		{
			LOCK_RECURSIVE;
			
			auto sender = mProtoTransfer.sender();
			auto recipient_pubkey = mProtoTransfer.recipient();
			
			auto amount = sender.amount();
			auto mm = MemoryManager::getInstance();
			if (0 == amount) {
				throw TransactionValidationInvalidInputException("amount is empty", "amount", "number")
					.setTransactionType(TRANSACTION_TRANSFER).setMemo(mMemo);
			}
			else if (amount < 0) {
				throw TransactionValidationInvalidInputException("negative amount", "amount", "number")
					.setTransactionType(TRANSACTION_TRANSFER).setMemo(mMemo);
			}
			if (recipient_pubkey.size() != crypto_sign_PUBLICKEYBYTES) {
				throw TransactionValidationInvalidInputException("invalid size", "recipient", "public key")
					.setTransactionType(TRANSACTION_TRANSFER).setMemo(mMemo);
			}
			if (sender.pubkey().size() != crypto_sign_PUBLICKEYBYTES) {
				throw TransactionValidationInvalidInputException("invalid size", "sender", "public key")
					.setTransactionType(TRANSACTION_TRANSFER).setMemo(mMemo);
			}
			if (0 == memcmp(sender.pubkey().data(), recipient_pubkey.data(), crypto_sign_PUBLICKEYBYTES)) {
				throw TransactionValidationException("sender and recipient are the same")
					  .setTransactionType(TRANSACTION_TRANSFER).setMemo(mMemo);
			}
			auto empty = mm->getFreeMemory(crypto_sign_PUBLICKEYBYTES);
			memset(*empty, 0, crypto_sign_PUBLICKEYBYTES);
			if (0 == memcmp(sender.pubkey().data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
				mm->releaseMemory(empty);
				throw TransactionValidationInvalidInputException("empty", "sender", "public key")
					.setTransactionType(TRANSACTION_TRANSFER).setMemo(mMemo);
			}
			if (0 == memcmp(recipient_pubkey.data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
				mm->releaseMemory(empty);
				throw TransactionValidationInvalidInputException("empty", "recipient", "public key")
					.setTransactionType(TRANSACTION_TRANSFER).setMemo(mMemo);
			}
			mm->releaseMemory(empty);
			if (mMemo.size() < 5 || mMemo.size() > 150) {
				throw TransactionValidationInvalidInputException("not in expected range [5;150]", "memo", "string")
					.setTransactionType(TRANSACTION_TRANSFER).setMemo(mMemo);
			}
			return true;
		}
	}
}
