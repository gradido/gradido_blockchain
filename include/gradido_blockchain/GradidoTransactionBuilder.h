#ifndef __GRADIDO_BLOCKCHAIN_GRADIDO_TRANSACTION_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_GRADIDO_TRANSACTION_BUILDER_H

#include "TransactionBodyBuilder.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	/*!
	 * @author einhornimmond
	 * 
	 * for easy creating of gradido transaction
	 * for one blockchain or also for cross community transactions
	 * <b>Cross Community Transaction</b>
	 * - you need to set sender and recipient community
	 * - after that you can call addSignaturePair or sign as often as needed for the transaction
	 * - call buildOutbound and send transaction to iota
	 * - note iota message id
	 * - call setParentMessageId with iota message id of outbound transaction
	 * - call buildInbound and send transaction to iota
	 */
	class GRADIDOBLOCKCHAIN_EXPORT GradidoTransactionBuilder
	{
	public:
		GradidoTransactionBuilder();
		~GradidoTransactionBuilder();

		void reset();
		//! build local transaction with exist only on one blockchain
		std::unique_ptr<data::GradidoTransaction> build();
		//! build outbound transaction for cross community transaction 
		//! should be used before buildInbound, send to iota, get message id
		std::unique_ptr<data::GradidoTransaction> buildOutbound();
		//! build inbound transaction for cross community transaction
		//! parent message id must be set before with outbound transaction iota message id
		std::unique_ptr<data::GradidoTransaction> buildInbound();
		GradidoTransactionBuilder& setTransactionBody(std::unique_ptr<data::TransactionBody> body);
		GradidoTransactionBuilder& setTransactionBody(memory::ConstBlockPtr bodyBytes);
		//! set sender community for cross community transaction
		//! Outbound transaction goes onto sender community blockchain and mark the starting point of transaction
		GradidoTransactionBuilder& setSenderCommunity(const std::string& senderCommunity);
		//! set recipient community for cross community transaction
		//! Inbound transaction goes onto recipient community blockchain and mark the end point of transaction
		GradidoTransactionBuilder& setRecipientCommunity(const std::string& recipientCommunity);
		GradidoTransactionBuilder& addSignaturePair(memory::ConstBlockPtr publicKey, memory::ConstBlockPtr signature);
		GradidoTransactionBuilder& sign(std::shared_ptr<KeyPairEd25519> keyPair);
		//! \param paringMessageId usually only for cross community transactions, the iota message id of outbound transaction
		GradidoTransactionBuilder& setParentMessageId(memory::ConstBlockPtr paringMessageId);

		bool isCrossCommunityTransaction() const;

	protected:
		void initializeCrossCommunityTransaction();
		void updateTransactionBodiesForCrossCommunityTransactions(const memory::ConstBlockPtr& bodyBytes);

		std::unique_ptr<data::GradidoTransaction> mGradidoTransaction;
		std::unique_ptr<data::GradidoTransaction> mOutboundTransaction;
		std::unique_ptr<data::GradidoTransaction> mInboundTransaction;
		std::string mSenderCommunity;
		std::string mRecipientCommunity;
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