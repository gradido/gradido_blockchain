#ifndef __GRADIDO_BLOCKCHAIN_V3_3_TRANSACTION_BODY_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_V3_3_TRANSACTION_BODY_BUILDER_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

#define GRADIDO_TRANSACTION_BODY_V3_3_VERSION_STRING "3.3"

namespace gradido {
	namespace v3_3 {
		class GRADIDOBLOCKCHAIN_EXPORT TransactionBodyBuilder
		{
		public:
			TransactionBodyBuilder();
			~TransactionBodyBuilder();
			void reset();
			std::unique_ptr<data::TransactionBody> build();

			TransactionBodyBuilder& setDeferredTransfer(
				memory::ConstBlockPtr senderPubkey,
				const std::string& amountGddCent,
				const std::string& communityId,
				memory::ConstBlockPtr recipientPubkey,
				Timepoint timeout
			);
			TransactionBodyBuilder& setCommunityFriendsUpdate(bool colorFusion);

			TransactionBodyBuilder& setRegisterAddress(
				memory::ConstBlockPtr userPubkey,
				data::AddressType type,
				memory::ConstBlockPtr nameHash = nullptr,
				memory::ConstBlockPtr accountPubkey = nullptr
			);

			TransactionBodyBuilder& setTransactionCreation(
				memory::ConstBlockPtr recipientPubkey,
				const std::string& amountGddCent,
				Timepoint targetDate
			);

			TransactionBodyBuilder& setTransactionTransfer(
				memory::ConstBlockPtr senderPubkey,
				const std::string& amountGddCent,
				const std::string& communityId,
				memory::ConstBlockPtr recipientPubkey
			);

			TransactionBodyBuilder& setCommunityRoot(
				memory::ConstBlockPtr pubkey,
				memory::ConstBlockPtr gmwPubkey,
				memory::ConstBlockPtr aufPubkey
			);

			//! if not called, time from calling TransactionBodyBuilder Constructor will be taken
			inline TransactionBodyBuilder& setCreatedAt(Timepoint createdAt) {
				mBody->createdAt = createdAt;
				return *this;
			}

			inline TransactionBodyBuilder& setMemo(std::string& memo) {
				mBody->memo = memo;
				return *this;
			}

			//! LOCAL is default value, if this function isn't called
			inline TransactionBodyBuilder& setCrossGroupType(data::CrossGroupType type) {
				mBody->type = type;
				return *this;
			}

			//! only needed for cross group transactions
			inline TransactionBodyBuilder& setOtherGroup(std::string&& otherGroup) {
				mBody->otherGroup = otherGroup;
				return *this;
			}
		protected:
			std::unique_ptr<data::TransactionBody> mBody;
			bool mSpecificTransactionChoosen;
		};

		// *******************  Exceptions ****************************
		class TransactionBodyBuilderException : public GradidoBlockchainException
		{
		public: 
			explicit TransactionBodyBuilderException(const char* what) noexcept
				: GradidoBlockchainException(what) {}

			std::string getFullString() const {
				return what();
			}
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_TRANSACTION_BODY_BUILDER_H