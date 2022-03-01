#include "gradido_blockchain/model/protobufWrapper/GlobalGroupAdd.h"
#include "gradido_blockchain/model/protobufWrapper/GradidoBlock.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"

#include "gradido_blockchain/model/IGradidoBlockchain.h"

#include "Poco/RegularExpression.h"


namespace model {
	namespace gradido {

		Poco::RegularExpression g_RegExGroupName("^[a-z0-9_ -]{3,255}");

		GlobalGroupAdd::GlobalGroupAdd(const proto::gradido::GlobalGroupAdd& protoGlobalGroupAdd)
			: mProtoGlobalGroupAdd(protoGlobalGroupAdd)
		{

		}

		GlobalGroupAdd::~GlobalGroupAdd()
		{

		}

		bool GlobalGroupAdd::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/
			) const
		{			
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				if (getGroupName().size() < 3) {
					throw TransactionValidationInvalidInputException("to short, at least 3", "group_name", "string");
				}
				if (getGroupName().size() > 255) {
					throw TransactionValidationInvalidInputException("to long, max 255", "group_name", "string");
				}
				if (!g_RegExGroupName.match(getGroupName())) {
					throw TransactionValidationInvalidInputException("invalid character, only ascii", "group_name", "string");

				}
				if (getGroupAlias().size() < 3) {
					throw TransactionValidationInvalidInputException("to short, at least 3", "group_alias", "string");
				}
				if (getGroupAlias().size() > 120) {
					throw TransactionValidationInvalidInputException("to long, max 255", "group_alias", "string");
				}
				if (!isValidGroupAlias(getGroupAlias())) {
					throw TransactionValidationInvalidInputException("invalid character, only ascii", "group_alias", "string");

				}
			}

			if ((level & TRANSACTION_VALIDATION_CONNECTED_GROUP) == TRANSACTION_VALIDATION_CONNECTED_GROUP)
			{
				assert(blockchain);
				auto transactions = blockchain->getAllTransactions();
				for (auto it = transactions.begin(); it != transactions.end(); it++) {
					auto gradidoBlock = std::make_unique<GradidoBlock>((*it)->getSerializedTransaction());
					auto transactionBody = gradidoBlock->getGradidoTransaction()->getTransactionBody();
					if (transactionBody->getTransactionType() != TRANSACTION_GLOBAL_GROUP_ADD) {
						throw InvalidTransactionTypeOnBlockchain("invalid transaction on global groups chain", transactionBody->getTransactionType());
					}
					auto groupAdd = transactionBody->getGlobalGroupAdd();
					if (groupAdd->getCoinColor() == getCoinColor()) {
						throw TransactionValidationInvalidInputException("already exist", "native_coin_color", "uint32");
					}
					if (groupAdd->getGroupAlias() == getGroupAlias()) {
						throw TransactionValidationInvalidInputException("already exist", "group_alias", "string");
					}
				}
			}
			return false;
		}

		bool GlobalGroupAdd::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			//auto pair = dynamic_cast<const GlobalGroupAdd*>(pairingTransaction);
			throw InvalidCrossGroupTransaction("try cross group transaction with ", TRANSACTION_GLOBAL_GROUP_ADD);
		}

		std::vector<MemoryBin*> GlobalGroupAdd::getInvolvedAddresses() const
		{
			return {};
		}
		uint32_t GlobalGroupAdd::getCoinColor() const 
		{ 
			return mProtoGlobalGroupAdd.native_coin_color(); 
		}
	}
}

