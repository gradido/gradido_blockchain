#include "gradido_blockchain/v3_3/interaction/validate/TransactionBodyRole.h"

#include "gradido_blockchain/v3_3/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"
#include "gradido_blockchain/v3_3/interaction/validate/GradidoCreationRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/RegisterAddressRole.h"

#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_flags.hpp"

using namespace magic_enum;

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {

				void TransactionBodyRole::run(
					Type type,
					const std::string& communityId,
					std::shared_ptr<AbstractBlockchainProvider> blockchainProvider,
					data::ConfirmedTransactionPtr previousConfirmedTransaction,
					data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				) {
					// when we don't know the confirmation date yet, we estimate
					// normally it should be maximal 2 minutes after createdAt if the system clock is correct
					if (!mConfirmedAt.seconds) {
						mConfirmedAt.seconds = mBody.createdAt.seconds + 120;
					}
					try {
						if ((type & Type::SINGLE) == Type::SINGLE) {
							if (mBody.versionNumber != data::GRADIDO_PROTOCOL_VERSION) {
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
							recipientPreviousConfirmedTransaction = blockchainProvider->findBlockchain(mBody.otherGroup)->getLastTransaction();
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
						if (mSpecificTransactionRole) {
							throw TransactionValidationException("TransactionBody has more than one Transaction Data Object");
						}
						mSpecificTransactionRole = std::make_unique<GradidoTransferRole>(*mBody.transfer.get(), mBody.otherGroup);
					}
					if (mBody.isCreation()) {
						if (mSpecificTransactionRole) {
							throw TransactionValidationException("TransactionBody has more than one Transaction Data Object");
						}
						mSpecificTransactionRole = std::make_unique<GradidoCreationRole>(*mBody.creation.get());
						// check target date for creation transactions
						dynamic_cast<GradidoCreationRole*>(mSpecificTransactionRole.get())->validateTargetDate(mBody.createdAt.getAsTimepoint());
					}
					if (mBody.isCommunityFriendsUpdate()) {
						if (mSpecificTransactionRole) {
							throw TransactionValidationException("TransactionBody has more than one Transaction Data Object");
						}
						// currently empty
						throw std::runtime_error("not implemented yet");
					}
					if (mBody.isRegisterAddress()) {
						if (mSpecificTransactionRole) {
							throw TransactionValidationException("TransactionBody has more than one Transaction Data Object");
						}
						mSpecificTransactionRole = std::make_unique<RegisterAddressRole>(*mBody.registerAddress.get());
					}
					if (mBody.isDeferredTransfer()) {
						if (mSpecificTransactionRole) {
							throw TransactionValidationException("TransactionBody has more than one Transaction Data Object");
						}
						auto deferredTransfer = mBody.deferredTransfer;
						if (mBody.createdAt.getAsTimepoint() >= deferredTransfer->timeout.getAsTimepoint()) {
							throw TransactionValidationInvalidInputException("already reached", "timeout", "Timestamp");
						}
						mSpecificTransactionRole = std::make_unique<GradidoDeferredTransferRole>(*mBody.deferredTransfer.get());
					}
					if (mBody.isCommunityRoot()) {
						if (mSpecificTransactionRole) {
							throw TransactionValidationException("TransactionBody has more than one Transaction Data Object");
						}
						mSpecificTransactionRole = std::make_unique<CommunityRootRole>(*mBody.communityRoot.get());
					}
					mSpecificTransactionRole = std::move(mSpecificTransactionRole);
					return *mSpecificTransactionRole;
				}
			}
		}
	}
}