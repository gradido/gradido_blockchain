#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H

#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoUnit.h"

namespace gradido {
	namespace data {
		class TransactionBody;
	}
	namespace interaction {
		namespace calculateAccountBalance {
			class AbstractRole
			{
			public:
				AbstractRole(std::shared_ptr<const data::TransactionBody> body)
					: mBody(body) {}
				virtual ~AbstractRole() {}

				//! how much this transaction will add to the account balance
				virtual GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const 
				{
					if (getRecipient()->isTheSame(accountPublicKey)) {
						return getTransferAmount().getAmount();
					}
					return 0.0;
				}
				//! how much this transaction will reduce the account balance
				virtual GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const 
				{
					if (getTransferAmount().getPublicKey()->isTheSame(accountPublicKey)) {
						return getTransferAmount().getAmount();
					}
					return 0.0;
				}
				virtual const data::TransferAmount& getTransferAmount() const {
					return mBody->getTransferAmount();
				}
				virtual memory::ConstBlockPtr getRecipient() const = 0;
				virtual memory::ConstBlockPtr getSender() const {
					return getTransferAmount().getPublicKey();
				};
				
			protected:
				std::shared_ptr<const data::TransactionBody> mBody;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H