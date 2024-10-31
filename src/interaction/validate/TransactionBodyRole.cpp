#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"


#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_flags.hpp"

using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace validate {

			void TransactionBodyRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				std::shared_ptr<const data::ConfirmedTransaction> previousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				// when we don't know the confirmation date yet, we estimate
				// normally it should be maximal 2 minutes after createdAt if the system clock is correct
				if (!mConfirmedAt.getSeconds()) {
					mConfirmedAt = mBody.getCreatedAt().getSeconds() + 120;
				}
				try {
					if ((type & Type::SINGLE) == Type::SINGLE) {
						if (mBody.getVersionNumber() != GRADIDO_TRANSACTION_BODY_V3_3_VERSION_STRING) {
							throw TransactionValidationInvalidInputException(
								"wrong version",
								"version_number",
								"string",
								GRADIDO_TRANSACTION_BODY_V3_3_VERSION_STRING,
								mBody.getVersionNumber().data()
							);
						}
						// memo is only mandatory for transfer and creation transactions
						if (mBody.isDeferredTransfer() || mBody.isTransfer() || mBody.isCreation()) {
							auto memoSize = mBody.getMemo().size();
							if (memoSize < 5 || memoSize > 450) {
								throw TransactionValidationInvalidInputException(
									"not in expected range [5;450]",
									"memo",
									"string",
									">= 5 && <= 450",
									std::to_string(memoSize).data()
								);
							}
						}
						auto& otherGroup = mBody.getOtherGroup();
						if (!otherGroup.empty() && !isValidCommunityAlias(otherGroup)) {
							throw TransactionValidationInvalidInputException(
								"invalid character, only lowercase english latin letter, numbers and -",
								"other_group", 
								"string",
								mCommunityIdRegexString.data(),
								otherGroup.data()
							);
						}
					}

					auto& specificRole = getSpecificTransactionRole();
					if (!mBody.getOtherGroup().empty() && !recipientPreviousConfirmedTransaction && blockchainProvider) {
						recipientPreviousConfirmedTransaction = 
							findBlockchain(blockchainProvider, mBody.getOtherGroup(), __FUNCTION__)
							->findOne(blockchain::Filter::LAST_TRANSACTION)
							->getConfirmedTransaction();
					}
					specificRole.setConfirmedAt(mConfirmedAt);
					specificRole.run(type, communityId, blockchainProvider, previousConfirmedTransaction, recipientPreviousConfirmedTransaction);
				}
				catch (TransactionValidationException& ex) {
					ex.setTransactionBody(mBody);
					throw;
				}
            }

			AbstractRole& TransactionBodyRole::getSpecificTransactionRole()
			{
				if (mSpecificTransactionRole) {
					return *mSpecificTransactionRole;
				}
				if (mBody.isTransfer()) {
					mSpecificTransactionRole = std::make_unique<GradidoTransferRole>(mBody.getTransfer(), mBody.getOtherGroup());
				}
				else if (mBody.isCreation()) 
				{
					mSpecificTransactionRole = std::make_unique<GradidoCreationRole>(mBody.getCreation());
					// check target date for creation transactions
					dynamic_cast<GradidoCreationRole*>(mSpecificTransactionRole.get())->validateTargetDate(mBody.getCreatedAt().getAsTimepoint());
				} 
				else if (mBody.isCommunityFriendsUpdate()) 
				{
					// currently empty
					throw std::runtime_error("not implemented yet");
				}
				else if (mBody.isRegisterAddress()) 
				{
					mSpecificTransactionRole = std::make_unique<RegisterAddressRole>(mBody.getRegisterAddress());
				}
				else if (mBody.isDeferredTransfer()) 
				{
					auto deferredTransfer = mBody.getDeferredTransfer();
					if (mBody.getCreatedAt().getAsTimepoint() >= deferredTransfer->getTimeout().getAsTimepoint()) {
						std::string expected = "> " + DataTypeConverter::timePointToString(mBody.getCreatedAt().getAsTimepoint());
						throw TransactionValidationInvalidInputException(
							"already reached", 
							"timeout", 
							"TimestampSeconds",
							expected.data(),
							DataTypeConverter::timePointToString(deferredTransfer->getTimeout().getAsTimepoint()).data()
						);
					}
					mSpecificTransactionRole = std::make_unique<GradidoDeferredTransferRole>(mBody.getDeferredTransfer());
				}
				else if (mBody.isCommunityRoot()) 
				{
					mSpecificTransactionRole = std::make_unique<CommunityRootRole>(mBody.getCommunityRoot());
				}
				if (!mSpecificTransactionRole) {
					throw TransactionValidationException("body without specific transaction");
				}
				mSpecificTransactionRole->setCreatedAt(mBody.getCreatedAt());
				return *mSpecificTransactionRole;
			}
		}
	}
}