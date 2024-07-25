#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"

#include "gradido_blockchain/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/const.h"

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
				data::ConstConfirmedTransactionPtr previousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				// when we don't know the confirmation date yet, we estimate
				// normally it should be maximal 2 minutes after createdAt if the system clock is correct
				if (!mConfirmedAt.seconds) {
					mConfirmedAt.seconds = mBody.createdAt.seconds + 120;
				}
				try {
					if ((type & Type::SINGLE) == Type::SINGLE) {
						if (mBody.versionNumber != GRADIDO_TRANSACTION_BODY_V3_3_VERSION_STRING) {
							throw TransactionValidationInvalidInputException("wrong version", "version_number", "string");
						}
						// memo is only mandatory for transfer and creation transactions
						if (mBody.isDeferredTransfer() || mBody.isTransfer() || mBody.isCreation()) {
							if (mBody.memo.size() < 5 || mBody.memo.size() > 450) {
								throw TransactionValidationInvalidInputException("not in expected range [5;450]", "memo", "string");
							}
						}
						auto otherGroup = mBody.otherGroup;
						if (!otherGroup.empty() && !isValidCommunityAlias(otherGroup)) {
							throw TransactionValidationInvalidInputException("invalid character, only ascii", "other_group", "string");
						}
					}

					auto& specificRole = getSpecificTransactionRole();
					if (!mBody.otherGroup.empty() && !recipientPreviousConfirmedTransaction) {
						recipientPreviousConfirmedTransaction = 
							blockchainProvider
							->findBlockchain(mBody.otherGroup)
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
					mSpecificTransactionRole = std::make_unique<GradidoTransferRole>(mBody.transfer, mBody.otherGroup);
				}
				else if (mBody.isCreation()) 
				{
					mSpecificTransactionRole = std::make_unique<GradidoCreationRole>(mBody.creation);
					// check target date for creation transactions
					dynamic_cast<GradidoCreationRole*>(mSpecificTransactionRole.get())->validateTargetDate(mBody.createdAt.getAsTimepoint());
				} 
				else if (mBody.isCommunityFriendsUpdate()) 
				{
					// currently empty
					throw std::runtime_error("not implemented yet");
				}
				else if (mBody.isRegisterAddress()) 
				{
					mSpecificTransactionRole = std::make_unique<RegisterAddressRole>(mBody.registerAddress);
				}
				else if (mBody.isDeferredTransfer()) 
				{
					auto deferredTransfer = mBody.deferredTransfer;
					if (mBody.createdAt.getAsTimepoint() >= deferredTransfer->timeout.getAsTimepoint()) {
						throw TransactionValidationInvalidInputException("already reached", "timeout", "Timestamp");
					}
					mSpecificTransactionRole = std::make_unique<GradidoDeferredTransferRole>(mBody.deferredTransfer);
				}
				else if (mBody.isCommunityRoot()) 
				{
					mSpecificTransactionRole = std::make_unique<CommunityRootRole>(mBody.communityRoot);
				}
				if (!mSpecificTransactionRole) {
					throw TransactionValidationException("body without specific transaction");
				}
				mSpecificTransactionRole->setCreatedAt(mBody.createdAt);
				return *mSpecificTransactionRole;
			}
		}
	}
}