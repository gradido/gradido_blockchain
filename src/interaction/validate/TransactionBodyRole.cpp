#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoRedeemDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTimeoutDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_flags.hpp"
#include "loguru/loguru.hpp"

#include <regex>

using namespace magic_enum;

namespace gradido {
	using namespace data;
	namespace interaction {
		namespace validate {

			void TransactionBodyRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
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
						if (mBody.getVersionNumber() != GRADIDO_TRANSACTION_BODY_VERSION_STRING) {
							throw TransactionValidationInvalidInputException(
								"wrong version",
								"version_number",
								"string",
								GRADIDO_TRANSACTION_BODY_VERSION_STRING,
								mBody.getVersionNumber().data()
							);
						}
						// memo is only mandatory for transfer and creation transactions
						if (mBody.isDeferredTransfer() || mBody.isTransfer() || mBody.isCreation()) {
							auto &memos = mBody.getMemos();
							if (memos.empty()) {
								throw TransactionValidationInvalidInputException(
									"no memo",
									"memo",
									"EncryptedMemo",
									">= 1",
									0
								);
							}
							for (auto& memo : mBody.getMemos()) {
								if (MemoKeyType::PLAIN == memo.getKeyType()) {
									if (!isLikelyPlainText(memo.getMemo())) {
										LOG_F(ERROR, "plain memo don't seem to be plain!");
									}
									auto memoSize = memo.getMemo().size();
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
								else {
									if (isLikelyPlainText(memo.getMemo())) {
										LOG_F(ERROR, "Attention!! encrypted memo seem to be plain!");
									}
									auto memoSize = memo.getMemo().size() - 32;
									if (memoSize < 5 || memoSize > 450) {
										throw TransactionValidationInvalidInputException(
											"not in expected range [5;450]",
											"encryptedMemo",
											"bytes",
											">= 5 && <= 450",
											std::to_string(memoSize).data()
										);
									}
								}
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

					auto& specificRole = getSpecificTransactionRole(blockchain);
					if (!mBody.getOtherGroup().empty() && !recipientPreviousConfirmedTransaction && blockchain) {
						recipientPreviousConfirmedTransaction = 
							findBlockchain(blockchain->getProvider(), mBody.getOtherGroup(), __FUNCTION__)
							->findOne(blockchain::Filter::LAST_TRANSACTION)
							->getConfirmedTransaction();
					}
					specificRole.setConfirmedAt(mConfirmedAt);
					specificRole.run(type, blockchain, previousConfirmedTransaction, recipientPreviousConfirmedTransaction);
				}
				catch (TransactionValidationException& ex) {
					ex.setTransactionBody(mBody);
					throw;
				}
            }

			AbstractRole& TransactionBodyRole::getSpecificTransactionRole(std::shared_ptr<blockchain::Abstract> blockchain)
			{
				if (mSpecificTransactionRole) {
					return *mSpecificTransactionRole;
				}

				if (mBody.isTransfer()) {
					mSpecificTransactionRole = std::make_unique<GradidoTransferRole>(mBody.getTransfer(), mBody.getOtherGroup());
				} 
				else if (mBody.isCreation()) {
					mSpecificTransactionRole = std::make_unique<GradidoCreationRole>(mBody.getCreation());
					// check target date for creation transactions
					dynamic_cast<GradidoCreationRole*>(mSpecificTransactionRole.get())->validateTargetDate(mBody.getCreatedAt().getAsTimepoint());
				} 
				else if (mBody.isCommunityFriendsUpdate()) {
					// currently empty
					throw GradidoNotImplementedException("interaction::validate missing role for communityFriendsUpdate");
				} 
				else if (mBody.isRegisterAddress()) {
					mSpecificTransactionRole = std::make_unique<RegisterAddressRole>(mBody.getRegisterAddress());
				} 
				else if (mBody.isDeferredTransfer()) {
					mSpecificTransactionRole = std::make_unique<GradidoDeferredTransferRole>(mBody.getDeferredTransfer());
				}
				else if (mBody.isRedeemDeferredTransfer()) {
					auto redeemDeferredTransfer = mBody.getRedeemDeferredTransfer();
					if (blockchain) {
						// check if redeem is inside the timeout of deferred transfer
						auto deferredTransferEntry = blockchain->getTransactionForId(redeemDeferredTransfer->getDeferredTransferTransactionNr());
						if (!deferredTransferEntry) {
							throw GradidoBlockchainTransactionNotFoundException(
								"could'n find deferred transfer transaction for redeem deferred transfer"
							).setTransactionId(redeemDeferredTransfer->getDeferredTransferTransactionNr());
						}
						auto deferredTransferConfirmedAt = deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt().getAsTimepoint();
						auto deferredTransferBody = deferredTransferEntry->getTransactionBody();
						if (!deferredTransferBody || !deferredTransferBody->isDeferredTransfer()) {
							throw TransactionValidationInvalidInputException(
								"deferredTransferTransactionNr points to a invalid Transaction, wrong type",
								"deferred_transfer_transaction_nr",
								"uint64",
								"DeferredTransfer",
								enum_name(deferredTransferEntry->getTransactionBody()->getTransactionType()).data()
							);
						}
						auto deferredTransfer = deferredTransferEntry->getTransactionBody()->getDeferredTransfer();
						auto expectedMaxConfirmedAt = mBody.getCreatedAt().getAsTimepoint() + MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION;
						if (deferredTransferConfirmedAt + deferredTransfer->getTimeoutDuration().getAsDuration() < expectedMaxConfirmedAt) {
							std::string expected("< ");
							expected += DataTypeConverter::timePointToString(deferredTransferConfirmedAt + deferredTransfer->getTimeoutDuration().getAsDuration());
							throw TransactionValidationInvalidInputException(
								"redeem timeouted deferred transfer",
								"createdAt",
								"Timestamp",
								expected.data(),
								DataTypeConverter::timePointToString(expectedMaxConfirmedAt).data()
							);
						}
					}
					mSpecificTransactionRole = std::make_unique<GradidoRedeemDeferredTransferRole>(redeemDeferredTransfer);
				}
				else if (mBody.isTimeoutDeferredTransfer()) {
					mSpecificTransactionRole = std::make_unique<GradidoTimeoutDeferredTransferRole>(mBody.getTimeoutDeferredTransfer());
				}
				else if (mBody.isCommunityRoot()) {
					mSpecificTransactionRole = std::make_unique<CommunityRootRole>(mBody.getCommunityRoot());
				} 
				
				if (!mSpecificTransactionRole) {
					throw TransactionValidationException("body without specific transaction");
				}
				mSpecificTransactionRole->setCreatedAt(mBody.getCreatedAt());
				return *mSpecificTransactionRole;
			}
			
			bool TransactionBodyRole::isLikelyPlainText(const memory::Block& data)
			{
				// Regular expression pattern that matches printable UTF-8 characters,
				// including characters with diacritics and non-ASCII letters.
				std::regex utf8Regex("[\\x20-\\x7E\\xC2-\\xDF\\xE0-\\xEF\\xF0-\\xFF\\xA0-\\xFF]+");

				// Convert the memory block into a string
				std::string text(reinterpret_cast<const char*>(data.data()), data.size());

				// Check if the entire text matches the pattern for printable UTF-8 characters
				return std::regex_match(text, utf8Regex);
			}
		}
	}
}