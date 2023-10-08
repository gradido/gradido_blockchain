#ifndef GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H
#define GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H

#include "proto/gradido/transaction_body.pb.h"

#include "DeferredTransfer.h"
#include "CommunityFriendsUpdate.h"
#include "RegisterAddress.h"
#include "TransactionCreation.h"
#include "TransactionTransfer.h"
#include "CommunityRoot.h"

#include "Poco/DateTime.h"

#include "../../lib/MultithreadContainer.h"

#define GRADIDO_PROTOCOL_VERSION "3.3"

namespace model {
	namespace gradido {
		/*!
		 *  \addtogroup Gradido-Protocol
 		 *  @{
 		 */

		//!  Contain the actuall transaction details.
		/*!

			<a href="https://github.com/gradido/gradido_protocol/blob/master/gradido/TransactionBody.proto" target="_blank">Protobuf: TransactionBody</a>
		*/
		class GRADIDOBLOCKCHAIN_EXPORT TransactionBody : public MultithreadContainer
		{
		public:
			virtual ~TransactionBody();

			void setCreatedAt(Poco::DateTime createdAt);
			[[deprecated("Replaced by setCreatedAt, changed name according to Gradido Apollo implementation")]]
			inline void setCreated(Poco::DateTime created) { setCreatedAt(created); }

			uint32_t getCreatedAtSeconds() const;
			[[deprecated("Replaced by getCreatedAtSeconds, changed name according to Gradido Apollo implementation")]]
			inline uint32_t getCreatedSeconds() const { return getCreatedAtSeconds(); }
			Poco::DateTime getCreatedAt() const;
			[[deprecated("Replaced by getCreatedAt, changed name according to Gradido Apollo implementation")]]
			inline Poco::DateTime getCreated() const { return getCreatedAt(); }

			static TransactionBody* load(const std::string& protoMessageBin, std::shared_ptr<ProtobufArenaMemory> arenaMemory);
			void upgradeToDeferredTransaction(Poco::Timestamp timeout);
			static TransactionBody* createGroupFriendsUpdate(bool colorFusion);
			static TransactionBody* createRegisterAddress(
				const MemoryBin* userPubkey,
				proto::gradido::RegisterAddress_AddressType type,
				const MemoryBin* nameHash,
				const MemoryBin* subaccountPubkey
			);
			static TransactionBody* createTransactionCreation(std::unique_ptr<proto::gradido::TransferAmount> transferAmount, Poco::DateTime targetDate);
			static TransactionBody* createTransactionTransfer(std::unique_ptr<proto::gradido::TransferAmount> transferAmount, const MemoryBin* recipientPubkey);
			static TransactionBody* createCommunityRoot(const MemoryBin* pubkey, const MemoryBin* gmwPubkey, const MemoryBin* aufPubkey);

			void updateToOutbound(const std::string& otherGroup);
			void updateToInbound(const std::string& otherGroup);

			inline TransactionType getTransactionType() const { return mTransactionType; }
			proto::gradido::TransactionBody_CrossGroupType getCrossGroupType() const;
			const std::string& getVersionNumber() const;
			const std::string& getOtherGroup() const;
			static const char* transactionTypeToString(TransactionType type);
			inline const char* getTransactionTypeString() const { return transactionTypeToString(mTransactionType);}
			std::string getMemo() const;
			void setMemo(const std::string& memo);

			std::string getGroupId(const IGradidoBlockchain* blockchain) const;

			inline bool isDeferredTransfer() const { return mTransactionType == TRANSACTION_DEFERRED_TRANSFER; }
			inline bool isGroupFriendsUpdate() const { return mTransactionType == TRANSACTION_GROUP_FRIENDS_UPDATE; }
			inline bool isRegisterAddress() const { return mTransactionType == TRANSACTION_REGISTER_ADDRESS; }
			inline bool isCreation() const { return mTransactionType == TRANSACTION_CREATION; }
			inline bool isTransfer() const  { return mTransactionType == TRANSACTION_TRANSFER; }
			inline bool isCommunityRoot() const { return mTransactionType == TRANSACTION_COMMUNITY_ROOT; }
			// cross group types
			inline bool isLocal() const { return getCrossGroupType() == proto::gradido::TransactionBody_CrossGroupType_LOCAL; }
			inline bool isInbound() const { return getCrossGroupType() == proto::gradido::TransactionBody_CrossGroupType_INBOUND; }
			inline bool isOutbound() const { return getCrossGroupType() == proto::gradido::TransactionBody_CrossGroupType_OUTBOUND; }
			inline bool isCross() const { return getCrossGroupType() == proto::gradido::TransactionBody_CrossGroupType_CROSS; }

			inline std::shared_ptr<ProtobufArenaMemory> getProtobufArena() { return mProtobufArenaMemory; }

			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const ConfirmedTransaction* parentConfirmedTransaction = nullptr
				) const;
			bool isBelongToUs(const TransactionBody* pairingTransaction) const;

			std::unique_ptr<std::string> getBodyBytes() const;
			const proto::gradido::TransactionBody* getBody() const { return mProtoTransactionBody; }

			const DeferredTransfer* getDeferredTransfer() const;
			const CommunityFriendsUpdate* getGroupFriendsUpdate() const;
			const RegisterAddress* getRegisterAddress() const;
			const TransactionCreation* getCreationTransaction() const;
			const TransactionTransfer* getTransferTransaction() const;
			const CommunityRoot* getCommunityRoot() const;

			const TransactionBase* getTransactionBase() const;

			std::string toDebugString() const;
		protected:
			TransactionBody();
			TransactionBody(std::shared_ptr<ProtobufArenaMemory> arenaMemory);

			void initSpecificTransaction();
			proto::gradido::TransactionBody* mProtoTransactionBody;
			TransactionBase* mTransactionSpecific;
			TransactionType mTransactionType;
			std::shared_ptr<ProtobufArenaMemory> mProtobufArenaMemory;
		};
		/*! @} End of Doxygen Groups*/
	}
}

#endif //GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H
