#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ADDRESS_INDEX_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ADDRESS_INDEX_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/AddressType.h"

#include <unordered_map>

class Dictionary;

namespace gradido {
	namespace blockchain {

		class TransactionEntry;

		// Not Threadsafe!
		class GRADIDOBLOCKCHAIN_EXPORT AddressIndex
		{
		public:
			AddressIndex();
			~AddressIndex();

			void reset();

			//! public keys need to be already in publicKeyDictionary
			//! \return added entries count
			void addTransaction(const TransactionEntry& transactionEntry, const Dictionary& publicKeyDictionary);
			const std::vector<uint64_t>& getTransactionsNrs(uint32_t publicKeyIndex) const;
			bool isExist(uint32_t publicKeyIndex) const;
			data::AddressType getAddressType(uint32_t publicKeyIndex) const;
			//! \return 0 if not found, else return last transaction nr where the balance of the account was changed
			uint64_t lastBalanceChanged(uint32_t publicKeyIndex) const;

		protected:
			bool addTransactionNrForIndex(uint32_t publicKeyIndex, uint64_t transactionNr, data::AddressType addressType);
			//! \return false if no entry was found
			bool updateLastBalanceChangingTransactionNr(uint32_t publicKeyIndex, uint64_t transactionNr);

			struct AddressData 
			{
				AddressData() : addressType(data::AddressType::NONE), lastBalanceChangingTransactionNr(0) {}
				data::AddressType addressType;
				std::vector<uint64_t> transactionNrs;
				// TODO: expand by coin community idies
				uint64_t lastBalanceChangingTransactionNr;
			};

			// store all transaction nrs for public key which alter the type (RegisterAddress, CommunityRoot)
			std::unordered_map<uint32_t, AddressData> mIndexTransactionNrs;
		};
	}
}

#endif // __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ADDRESS_INDEX_H