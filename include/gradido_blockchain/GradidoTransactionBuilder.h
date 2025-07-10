#ifndef __GRADIDO_BLOCKCHAIN_GRADIDO_TRANSACTION_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_GRADIDO_TRANSACTION_BUILDER_H

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
		GradidoTransactionBuilder& setDeferredTransfer(data::GradidoTransfer transactionTransfer, data::DurationSeconds timeoutDuration);
		GradidoTransactionBuilder& setDeferredTransfer(std::unique_ptr<data::GradidoDeferredTransfer> deferredTransfer);

		GradidoTransactionBuilder& setCommunityFriendsUpdate(bool colorFusion);
		GradidoTransactionBuilder& setCommunityFriendsUpdate(std::unique_ptr<data::CommunityFriendsUpdate> communityFriendsUpdate);

		GradidoTransactionBuilder& setRegisterAddress(
			memory::ConstBlockPtr userPubkey,
			data::AddressType type,
			memory::ConstBlockPtr nameHash = nullptr,
			memory::ConstBlockPtr accountPubkey = nullptr
		);
		GradidoTransactionBuilder& setRegisterAddress(std::unique_ptr<data::RegisterAddress> registerAddress);

		GradidoTransactionBuilder& setTransactionCreation(const data::TransferAmount& recipient, Timepoint targetDate);
		GradidoTransactionBuilder& setTransactionCreation(std::unique_ptr<data::GradidoCreation> creation);

		GradidoTransactionBuilder& setTransactionTransfer(const data::TransferAmount& sender, memory::ConstBlockPtr recipientPubkey);
		GradidoTransactionBuilder& setTransactionTransfer(std::unique_ptr<data::GradidoTransfer> transfer);

		GradidoTransactionBuilder& setCommunityRoot(
			memory::ConstBlockPtr pubkey,
			memory::ConstBlockPtr gmwPubkey,
			memory::ConstBlockPtr aufPubkey
		);
		GradidoTransactionBuilder& setCommunityRoot(std::unique_ptr<data::CommunityRoot> communityRoot);

		GradidoTransactionBuilder& setRedeemDeferredTransfer(uint64_t deferredTransferTransactionNr, data::GradidoTransfer transactionTransfer);
		GradidoTransactionBuilder& setRedeemDeferredTransfer(std::unique_ptr<data::GradidoRedeemDeferredTransfer> redeemDeferredTransfer);
		GradidoTransactionBuilder& setTimeoutDeferredTransfer(uint64_t deferredTransferTransactionNr);
		GradidoTransactionBuilder& setTimeoutDeferredTransfer(std::unique_ptr<data::GradidoTimeoutDeferredTransfer> timeoutDeferredTransfer);

		//! if not called, time from calling GradidoTransactionBuilder Constructor will be taken
		//! \param createAt timestamp when transaction where created
		GradidoTransactionBuilder& setCreatedAt(Timepoint createdAt);
		GradidoTransactionBuilder& addMemo(const data::EncryptedMemo& memo);
		GradidoTransactionBuilder& setVersionNumber(std::string_view versionNumber);
		//! \param body will be moved
		GradidoTransactionBuilder& setTransactionBody(std::unique_ptr<data::TransactionBody> body);

		GradidoTransactionBuilder& setTransactionBody(memory::ConstBlockPtr bodyBytes);
		//! set sender community for cross community transaction
		//! Outbound transaction goes onto sender community blockchain and mark the starting point of transaction
		GradidoTransactionBuilder& setSenderCommunity(const std::string& senderCommunity);
		//! set recipient community for cross community transaction
		//! Inbound transaction goes onto recipient community blockchain and mark the end point of transaction
		GradidoTransactionBuilder& setRecipientCommunity(const std::string& recipientCommunity);
		GradidoTransactionBuilder& sign(std::shared_ptr<KeyPairEd25519> keyPair);
		//! \param paringMessageId usually only for cross community transactions, the iota message id of outbound transaction
		GradidoTransactionBuilder& setParentMessageId(memory::ConstBlockPtr paringMessageId);

		bool isCrossCommunityTransaction() const;
		inline bool isBodyBuildingState() const { return mState == BuildingState::BUILDING_BODY; }
		inline bool isCrossCommunityBuildingState() const { return mState == BuildingState::CROSS_COMMUNITY; }
		inline bool isLocalCommunityBuildingState() const { return mState == BuildingState::LOCAL; }

	protected:

		enum class BuildingState {
			BUILDING_BODY,
			LOCAL,
			CROSS_COMMUNITY
		};

		struct BodyBytesSignatureMap {
			memory::ConstBlockPtr bodyBytes;
			data::SignatureMap signatureMap;
		};

		// throw an exception if current state isn't expected state
		void checkBuildState(BuildingState expectedState) const;
		// switch from build_body to local or cross community, depending if sender and recipient community are set
		// fill mBodyByteSignatureMaps with body bytes, make ready for receiving sign calls
		void switchBuildState();

		BuildingState mState;
		std::unique_ptr<data::TransactionBody> mBody;
		std::string mSenderCommunity;
		std::string mRecipientCommunity;
		// for local transaction it contain only one entry
		// for cross group transaction it contain outbound on index = 0 and inbound on index = 1
		std::vector<BodyBytesSignatureMap> mBodyByteSignatureMaps;

		bool mSpecificTransactionChoosen;
		memory::ConstBlockPtr mParingMessageId;
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

	class GradidoTransactionWrongBuildingStateBuilderException : public GradidoTransactionBuilderException
	{
	public:
		explicit GradidoTransactionWrongBuildingStateBuilderException(
			const char* what,
			std::string_view expectedBuildState,
			std::string_view actualBuildState
		);
		std::string getFullString() const;

	protected:
		std::string mExpectedBuildState;
		std::string mActualBuildState;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_GRADIDO_TRANSACTION_BUILDER_H