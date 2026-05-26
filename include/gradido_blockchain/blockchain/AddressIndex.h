#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ADDRESS_INDEX_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ADDRESS_INDEX_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/ByteArray.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"
#include "gradido_blockchain_core/types/address.h"

#include <unordered_map>
#include <memory>
#include <vector>

namespace memory {
	class Block;
	using ConstBlockPtr = std::shared_ptr<const Block>;
}

namespace gradido {
	namespace data::compact {
		struct ConfirmedGradidoTx;
	}
	namespace blockchain {

		class TransactionEntry;

		// Not Threadsafe!
		class GRADIDOBLOCKCHAIN_EXPORT AddressIndex
		{
		public:
			AddressIndex(uint32_t communityIdIndex);
			~AddressIndex();

			void reset();

			//! public keys need to be already in publicKeyDictionary
			//! \return added entries count
			void addTransaction(const TransactionEntry& transactionEntry, const IDictionary<data::PublicKey>& publicKeyDictionary);
			void addTransaction(const data::compact::ConfirmedGradidoTx& compactTx);
			const std::vector<uint64_t>& getTransactionsNrs(data::compact::PublicKeyIndex publicKeyIndex) const;
			bool isExist(data::compact::PublicKeyIndex publicKeyIndex) const;
			grdt_address getAddressType(data::compact::PublicKeyIndex publicKeyIndex) const;
			std::vector<uint64_t> getgrdt_addressChangingTransactions(data::compact::PublicKeyIndex publicKeyIndex) const;
			//! \return 0 if not found, else return last transaction nr where the balance of the account was changed
			uint64_t lastBalanceChanged(data::compact::PublicKeyIndex publicKeyIndex) const;

		protected:
			bool addTransactionNrForIndex(uint32_t publicKeyIndex, uint64_t transactionNr, grdt_address addressType);
			//! \return false if no entry was found
			bool updateLastBalanceChangingTransactionNr(uint32_t publicKeyIndex, uint64_t transactionNr);

			struct AddressData 
			{
				AddressData() : addressType(GRDT_ADDRESS_NONE), lastBalanceChangingTransactionNr(0) {}
				grdt_address addressType;
				std::vector<uint64_t> transactionNrs;
				// TODO: expand by coin community idies
				uint64_t lastBalanceChangingTransactionNr;
			};

			// TODO: store all transaction nrs for public key which alter the type (RegisterAddress, CommunityRoot)
			std::unordered_map<uint32_t, AddressData> mIndexTransactionNrs;
			uint32_t mCommunityIdIndex;
		};
	}
}

#endif // __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ADDRESS_INDEX_H