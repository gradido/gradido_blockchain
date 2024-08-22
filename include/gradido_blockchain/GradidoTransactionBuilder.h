#ifndef __GRADIDO_BLOCKCHAIN_GRADIDO_TRANSACTION_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_GRADIDO_TRANSACTION_BUILDER_H

#include "TransactionBodyBuilder.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	class GRADIDOBLOCKCHAIN_EXPORT GradidoTransactionBuilder
	{
	public:
		GradidoTransactionBuilder();
		~GradidoTransactionBuilder();

		void reset();
		std::unique_ptr<data::GradidoTransaction> build();
		GradidoTransactionBuilder& setTransactionBody(std::unique_ptr<data::TransactionBody> body);
		GradidoTransactionBuilder& addSignaturePair(memory::ConstBlockPtr publicKey, memory::ConstBlockPtr signature);
		GradidoTransactionBuilder& sign(std::shared_ptr<KeyPairEd25519> keyPair);
		GradidoTransactionBuilder& setParentMessageId(memory::ConstBlockPtr paringMessageId);

	protected:
		std::unique_ptr<data::GradidoTransaction> mGradidoTransaction;
	};

	// *******************  Exceptions ****************************
	class GradidoTransactionBuilderException : public GradidoBlockchainException
	{
	public:
		explicit GradidoTransactionBuilderException(const char* what) noexcept
			: GradidoBlockchainException(what) {}

		std::string getFullString() const {
			return what();
		}
	};
}

#endif //__GRADIDO_BLOCKCHAIN_GRADIDO_TRANSACTION_BUILDER_H