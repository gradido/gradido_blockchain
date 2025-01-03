#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {

            class TransactionBodyRole: public AbstractRole 
            {
            public:
				TransactionBodyRole(const data::TransactionBody& body) : mBody(body) {}
				void run(
					Type type,
					std::shared_ptr<blockchain::Abstract> blockchain,
					std::shared_ptr<const data::ConfirmedTransaction> previousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
				);

				inline void checkRequiredSignatures(
					const data::SignatureMap& signatureMap,
					std::shared_ptr<blockchain::Abstract> blockchain
				) { 
					getSpecificTransactionRole(blockchain)
						.checkRequiredSignatures(signatureMap, blockchain);
				}
            protected:
				AbstractRole& getSpecificTransactionRole(std::shared_ptr<blockchain::Abstract> blockchain);

				/**
				 * @brief Checks if the given memory block is likely plain text.
				 *
				 * This function uses a regular expression to check if the contents of the
				 * given memory block consist of printable characters, including characters
				 * from various European languages (with diacritics such as ä, ö, ü, ß),
				 * and characters from multiple scripts in UTF-8 encoding. It attempts to
				 * distinguish between human-readable plain text and binary data that could
				 * indicate encryption.
				 * 
				 * written from chatgpt 4o mini
				 *
				 * @param data The memory block to be checked.
				 * @return true if the data looks like plain text, false if it seems binary or encrypted.
				 */
				bool isLikelyPlainText(const memory::Block& data);

				std::unique_ptr<AbstractRole> mSpecificTransactionRole;
				const data::TransactionBody& mBody;
            };
		}
	}
}



#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H