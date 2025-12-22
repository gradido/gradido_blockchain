#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/blockchain/AddressIndex.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/lib/Dictionary.h"
#include "gradido_blockchain/memory/Block.h"

#include "loguru/loguru.hpp"

#include <vector>

using std::make_pair, std::vector, std::move;
using memory::ConstBlockPtr;

namespace gradido {

	using data::AddressType;

	namespace blockchain {

		AddressIndex::AddressIndex()
		{

		}

		AddressIndex::~AddressIndex()
		{

		}

		void AddressIndex::reset()
		{
			mIndexTransactionNrs.clear();
		}

		void AddressIndex::addTransaction(const TransactionEntry& transactionEntry, const Dictionary& publicKeyDictionary)
		{
			const auto& body = transactionEntry.getConfirmedTransaction()->getGradidoTransaction()->getTransactionBody();
			uint64_t txNr = transactionEntry.getTransactionNr();

			auto addKey = [&](const ConstBlockPtr& pubKeyPtr, AddressType type) -> bool {			
				assert(pubKeyPtr);
				return addTransactionNrForIndex(
					publicKeyDictionary.getIndexForBinary(*pubKeyPtr),
					txNr,
					type
				);
			};

			auto updateBalanceChangingTx = [&](const ConstBlockPtr& pubKeyPtr) -> bool {
				assert(pubKeyPtr);
				return updateLastBalanceChangingTransactionNr(
					publicKeyDictionary.getIndexForBinary(*pubKeyPtr),
					txNr
				);
			};

			if (body->isCommunityRoot()) 
			{
				const auto& communityRoot = body->getCommunityRoot();
					
				if (!addKey(communityRoot->getAufPubkey(), AddressType::COMMUNITY_AUF)) {
					LOG_F(WARNING, "couldn't add Community Auf Key to Address Indices");
				}
				if (!addKey(communityRoot->getGmwPubkey(), AddressType::COMMUNITY_GMW)) {
					LOG_F(WARNING, "couldn't add Community GMW Key to Address Indices");
				}
			} 
			else if (body->isRegisterAddress()) 
			{
				const auto& registerAddress = body->getRegisterAddress();

				if (!addKey(registerAddress->getUserPublicKey(), registerAddress->getAddressType())) {
					LOG_F(WARNING, "couldn't add register user Key to Address Indices");
				}

				if (!addKey(registerAddress->getAccountPublicKey(), registerAddress->getAddressType())) {
					LOG_F(WARNING, "couldn't add register address Key to Address Indices");
				}				
			}
			else if (body->isDeferredTransfer()) 
			{
				const auto& deferredTransfer = body->getDeferredTransfer();
				if (!addKey(deferredTransfer->getRecipientPublicKey(), AddressType::DEFERRED_TRANSFER)) {
					LOG_F(WARNING, "couldn't add deferred address Key to Address Indices");
				}
			}
			const auto& accountBalances = transactionEntry.getConfirmedTransaction()->getAccountBalances();
			for (const auto& accountBalance : accountBalances) {
				updateBalanceChangingTx(accountBalance.getPublicKey());
			}

		}

		bool AddressIndex::addTransactionNrForIndex(uint32_t publicKeyIndex, uint64_t transactionNr, data::AddressType addressType)
		{
			auto it = mIndexTransactionNrs.find(publicKeyIndex);
			if (it == mIndexTransactionNrs.end()) {
				AddressData data;
				data.addressType = addressType;
				data.transactionNrs.reserve(1); // as long no moving occure, every address should only have one transaction nr
				data.transactionNrs.emplace_back(transactionNr);
				if (!mIndexTransactionNrs.emplace(make_pair(publicKeyIndex, move(data))).second) {
					throw GradidoNodeInvalidDataException("error inserting new publicKeyIndex-transactionNr pair, maybe already exist?");
				}
				return true;
			}
			else {
				if (addressType != it->second.addressType) {
					throw GradidoNodeInvalidDataException("AddressIndex: address type changed, currently not expected!");
				}
				for (auto nr : it->second.transactionNrs) {
					if (nr == transactionNr) {
						return false;
					}
				}
				it->second.transactionNrs.push_back(transactionNr);
				return true;
			}
		}

		bool AddressIndex::updateLastBalanceChangingTransactionNr(uint32_t publicKeyIndex, uint64_t transactionNr)
		{
			auto it = mIndexTransactionNrs.find(publicKeyIndex);
			if (it != mIndexTransactionNrs.end()) {
				it->second.lastBalanceChangingTransactionNr = transactionNr;
				return true;
			}
			return false;
		}

		const vector<uint64_t>& AddressIndex::getTransactionsNrs(uint32_t publicKeyIndex) const
		{
			auto it = mIndexTransactionNrs.find(publicKeyIndex);
			if (it == mIndexTransactionNrs.end()) {
				throw GradidoNodeInvalidDataException("publicKeyIndex not found, please call isPublicKeyIndexExist before");
			}
			return it->second.transactionNrs;
		}

		AddressType AddressIndex::getAddressType(uint32_t publicKeyIndex) const
		{
			auto it = mIndexTransactionNrs.find(publicKeyIndex);
			if (it == mIndexTransactionNrs.end()) {
				return AddressType::NONE;
			}
			return it->second.addressType;
		}

		uint64_t AddressIndex::lastBalanceChanged(uint32_t publicKeyIndex) const
		{
			auto it = mIndexTransactionNrs.find(publicKeyIndex);
			if (it == mIndexTransactionNrs.end()) {
				return 0;
			}
			return it->second.lastBalanceChangingTransactionNr;
		}

		bool AddressIndex::isExist(uint32_t publicKeyIndex) const
		{
			auto it = mIndexTransactionNrs.find(publicKeyIndex);
			return it != mIndexTransactionNrs.end();
		}
	}
}
