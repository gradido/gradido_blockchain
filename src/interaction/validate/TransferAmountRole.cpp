#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			void TransferAmountRole::run(Type type, ContextData& c) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					auto& coinCommunityId = mTransferAmount.getCommunityId();
					if (!coinCommunityId.empty() && !isValidCommunityAlias(coinCommunityId)) {
						throw TransactionValidationInvalidInputException(
								"invalid character, only lowercase english latin letter, numbers and -",
								"community_id",
								"string",
								COMMUNITY_ID_REGEX_STRING,
								coinCommunityId.data()
							);
					}

					if (c.senderBlockchain) {
						auto communityId = c.senderBlockchain->getCommunityId();
						if (!communityId.empty() && coinCommunityId == communityId) {
							std::string expected = "!= " + std::string(communityId);
							throw TransactionValidationInvalidInputException(
								"coin communityId shouldn't be set if it is the same as blockchain communityId",
								"community_id",
								"string",
								expected.data(),
								coinCommunityId.data()
							);
						}
					}
					if (mTransferAmount.getAmount() <= GradidoUnit::zero()) {
						throw TransactionValidationInvalidInputException("zero or negative amount", "amount", "string");
					}
					validateEd25519PublicKey(mTransferAmount.getPublicKey(), "sender");
				}
			}
		}
	}
}
