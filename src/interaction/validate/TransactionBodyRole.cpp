#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/MemoKeyType.h"
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
#include "gradido_blockchain/memory/Block.h"

#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_flags.hpp"
#include "loguru/loguru.hpp"

#include <memory>
#include <string>
#include <regex>

using namespace magic_enum;
using memory::Block;
using std::shared_ptr, std::make_unique;
using std::string;
using std::regex, std::regex_match;

namespace gradido {
	using blockchain::Filter;
	using data::CrossGroupType, data::MemoKeyType;

	namespace interaction {
		namespace validate {

			void TransactionBodyRole::run(Type type, ContextData& c)
			{
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
						if (mBody.getType() != CrossGroupType::LOCAL && !mBody.getOtherCommunityIdIndex().has_value()) {
							throw TransactionValidationInvalidInputException(
								"missing other community id index for cross group transaction",
								"other_community_id_index",
								"std::optional<uint32>",
								"has value",
								"null"
							);
						}
					}

					auto& specificRole = getSpecificTransactionRole();
					if (mBody.getOtherCommunityIdIndex().has_value() && c.senderBlockchain) {
						auto otherBlockchain = findBlockchain(c.senderBlockchain->getProvider(), mBody.getOtherCommunityIdIndex().value(), __FUNCTION__);

						if (mBody.getType() == CrossGroupType::OUTBOUND) {
							c.recipientBlockchain = otherBlockchain;
						}
						else if (mBody.getType() == CrossGroupType::INBOUND) {
							c.recipientBlockchain = c.senderBlockchain;
							c.senderBlockchain = otherBlockchain;
						}
						else {
							throw GradidoNodeInvalidDataException("Invalid branch, CrossGroupType::CROSS not implemented yet");
						}
						auto lastRecipientEntry = c.recipientBlockchain->findOne(Filter::LAST_TRANSACTION);
						if (!lastRecipientEntry) {
							throw GradidoNodeInvalidDataException("missing last transaction of other community id");
						}
						c.recipientPreviousConfirmedTransaction = lastRecipientEntry->getConfirmedTransaction();
					}

					specificRole.run(type, c);
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
					auto specificRole = make_unique<GradidoTransferRole>(mBody.getTransfer());
					specificRole->setCrossGroupType(mBody.getType());
					mSpecificTransactionRole = std::move(specificRole);
				}
				else if (mBody.isCreation()) {
					mSpecificTransactionRole = make_unique<GradidoCreationRole>(mBody.getCreation());
					// check target date for creation transactions
					dynamic_cast<GradidoCreationRole*>(mSpecificTransactionRole.get())->validateTargetDate(mBody.getCreatedAt().getAsTimepoint());
				}
				else if (mBody.isCommunityFriendsUpdate()) {
					// currently empty
					throw GradidoNotImplementedException("interaction::validate missing role for communityFriendsUpdate");
				}
				else if (mBody.isRegisterAddress()) {
					mSpecificTransactionRole = make_unique<RegisterAddressRole>(mBody.getRegisterAddress().value());
				}
				else if (mBody.isDeferredTransfer()) {
					mSpecificTransactionRole = make_unique<GradidoDeferredTransferRole>(mBody.getDeferredTransfer());
				}
				else if (mBody.isRedeemDeferredTransfer()) {
					auto specificRole = make_unique<GradidoRedeemDeferredTransferRole>(mBody.getRedeemDeferredTransfer());
					specificRole->setCrossGroupType(mBody.getType());
					mSpecificTransactionRole = std::move(specificRole);
				}
				else if (mBody.isTimeoutDeferredTransfer()) {
					mSpecificTransactionRole = make_unique<GradidoTimeoutDeferredTransferRole>(mBody.getTimeoutDeferredTransfer());
				}
				else if (mBody.isCommunityRoot()) {
					mSpecificTransactionRole = make_unique<CommunityRootRole>(mBody.getCommunityRoot().value());
				}

				if (!mSpecificTransactionRole) {
					throw TransactionValidationException("body without specific transaction");
				}
				mSpecificTransactionRole->setCreatedAt(mBody.getCreatedAt());
				mSpecificTransactionRole->setConfirmedAt(mConfirmedAt);
				return *mSpecificTransactionRole;
			}

			bool TransactionBodyRole::isLikelyPlainText(const Block& data)
			{
				// Regular expression pattern that matches printable UTF-8 characters,
				// including characters with diacritics and non-ASCII letters.
				regex utf8Regex("[\\x20-\\x7E\\xC2-\\xDF\\xE0-\\xEF\\xF0-\\xFF\\xA0-\\xFF]+");

				// Convert the memory block into a string
				string text(reinterpret_cast<const char*>(data.data()), data.size());

				// Check if the entire text matches the pattern for printable UTF-8 characters
				return regex_match(text, utf8Regex);
			}
		}
	}
}