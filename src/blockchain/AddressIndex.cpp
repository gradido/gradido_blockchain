#include "gradido_blockchain/blockchain/AddressIndex.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include "loguru/loguru.hpp"

#include <vector>

using std::make_pair, std::vector, std::move;
using memory::ConstBlockPtr;

namespace gradido {
	using data::adapter::toPublicKey;
	using data::compact::ConfirmedGradidoTx;
	using data::AddressType;

	namespace blockchain {

		AddressIndex::AddressIndex(uint32_t communityIdIndex)
			: mCommunityIdIndex(communityIdIndex)
		{

		}

		AddressIndex::~AddressIndex()
		{

		}

		void AddressIndex::reset()
		{
			mIndexTransactionNrs.clear();
		}

		void AddressIndex::addTransaction(const TransactionEntry& transactionEntry, const IDictionary<PublicKey>& publicKeyDictionary)
		{
			const auto& body = transactionEntry.getConfirmedTransaction()->getGradidoTransaction()->getTransactionBody();
			uint64_t txNr = transactionEntry.getTransactionNr();

			auto getPublicKeyIndex = [&](const ConstBlockPtr pubKeyPtr) -> uint32_t {
				assert(pubKeyPtr);
				auto index = publicKeyDictionary.getIndexForData(toPublicKey(pubKeyPtr));
				if (!index) {
					throw DictionaryMissingEntryException("AddressIndex: missing index of public key in Dictionary", pubKeyPtr->convertToHex());
				}
				return index;
			};

			if (body->isCommunityRoot()) 
			{
				auto communityRoot = body->getCommunityRoot().value();
				if (!addTransactionNrForIndex(communityRoot.aufPublicKeyIndex, txNr, AddressType::COMMUNITY_AUF)) {
					LOG_F(WARNING, "couldn't add Community Auf Key to Address Indices");
				}
				if (!addTransactionNrForIndex(communityRoot.gmwPublicKeyIndex, txNr, AddressType::COMMUNITY_GMW)) {
					LOG_F(WARNING, "couldn't add Community GMW Key to Address Indices");
				}
			} 
			else if (body->isRegisterAddress()) 
			{
				const auto& registerAddress = body->getRegisterAddress();

				if (!addTransactionNrForIndex(registerAddress->userPublicKeyIndex, txNr, registerAddress->addressType)) {
					LOG_F(WARNING, "couldn't add register user Key to Address Indices");
				}

				if (!addTransactionNrForIndex(registerAddress->accountPublicKeyIndex, txNr, registerAddress->addressType)) {
					LOG_F(WARNING, "couldn't add register address Key to Address Indices");
				}				
			}
			else if (body->isDeferredTransfer()) 
			{
				const auto& deferredTransfer = body->getDeferredTransfer();
				if (!addTransactionNrForIndex(getPublicKeyIndex(deferredTransfer->getRecipientPublicKey()), txNr, AddressType::DEFERRED_TRANSFER)) {
					LOG_F(WARNING, "couldn't add deferred address Key to Address Indices");
				}
			}
			const auto& accountBalances = transactionEntry.getConfirmedTransaction()->getAccountBalances();
			for (const auto& accountBalance : accountBalances) {
				updateLastBalanceChangingTransactionNr(getPublicKeyIndex(accountBalance.getPublicKey()), txNr);
			}
		}

		void AddressIndex::addTransaction(const data::compact::ConfirmedGradidoTx& compactTx)
		{
			uint64_t txNr = compactTx.txNr;
			if (compactTx.isCommunityRoot()) {
				const auto& communityRoot = compactTx.specific.communityRoot;
				if (!addTransactionNrForIndex(communityRoot.aufPublicKeyIndex, txNr, AddressType::COMMUNITY_AUF)) {
					LOG_F(WARNING, "couldn't add Community Auf Key to Address Indices");
				}
				if (!addTransactionNrForIndex(communityRoot.gmwPublicKeyIndex, txNr, AddressType::COMMUNITY_GMW)) {
					LOG_F(WARNING, "couldn't add Community GMW Key to Address Indices");
				}
			}
			else if (compactTx.isRegisterAddress())
			{
				const auto& registerAddress = compactTx.specific.registerAddress;

				if (!addTransactionNrForIndex(registerAddress.userPublicKeyIndex, txNr, registerAddress.addressType)) {
					LOG_F(WARNING, "couldn't add register user Key to Address Indices");
				}

				if (!addTransactionNrForIndex(registerAddress.accountPublicKeyIndex, txNr, registerAddress.addressType)) {
					LOG_F(WARNING, "couldn't add register address Key to Address Indices");
				}
			}
			else if (compactTx.isDeferredTransfer()) {
				const auto& deferredTransfer = compactTx.specific.deferredTransfer;
				if (!addTransactionNrForIndex(deferredTransfer.recipientPublicKeyIndex, txNr, AddressType::DEFERRED_TRANSFER)) {
					LOG_F(WARNING, "couldn't add deferred address Key to Address Indices");
				}
			}
			const auto& accountBalances = compactTx.accountBalances;
			for (int i = 0; i < compactTx.accountBalanceCount; i++) {
				updateLastBalanceChangingTransactionNr(accountBalances[i].publicKeyIndex, txNr);
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

		const vector<uint64_t>& AddressIndex::getTransactionsNrs(data::compact::PublicKeyIndex publicKeyIndex) const
		{
			if (publicKeyIndex.communityIdIndex != mCommunityIdIndex) {
				throw GradidoNodeInvalidDataException("dont't call AddressIndex::getTransactionsNrs with foreign publicKey");
			}
			auto it = mIndexTransactionNrs.find(publicKeyIndex.publicKeyIndex);
			if (it == mIndexTransactionNrs.end()) {
				throw GradidoNodeInvalidDataException("publicKeyIndex not found, please call isPublicKeyIndexExist before");
			}
			return it->second.transactionNrs;
		}

		AddressType AddressIndex::getAddressType(data::compact::PublicKeyIndex publicKeyIndex) const
		{
			if (publicKeyIndex.communityIdIndex != mCommunityIdIndex) {
				throw GradidoNodeInvalidDataException("dont't call AddressIndex::getAddressType with foreign publicKey");
			}
			auto it = mIndexTransactionNrs.find(publicKeyIndex.publicKeyIndex);
			if (it == mIndexTransactionNrs.end()) {
				return AddressType::NONE;
			}
			return it->second.addressType;
		}

		vector<uint64_t> AddressIndex::getAddressTypeChangingTransactions(data::compact::PublicKeyIndex publicKeyIndex) const
		{
			if (publicKeyIndex.communityIdIndex != mCommunityIdIndex) {
				throw GradidoNodeInvalidDataException("dont't call AddressIndex::getAddressTypeChangingTransactions with foreign publicKey");
			}
			auto it = mIndexTransactionNrs.find(publicKeyIndex.publicKeyIndex);
			if (it == mIndexTransactionNrs.end()) {
				return {};
			}
			return it->second.transactionNrs;
		}


		uint64_t AddressIndex::lastBalanceChanged(data::compact::PublicKeyIndex publicKeyIndex) const
		{
			if (publicKeyIndex.communityIdIndex != mCommunityIdIndex) {
				throw GradidoNodeInvalidDataException("dont't call AddressIndex::lastBalanceChanged with foreign publicKey");
			}
			auto it = mIndexTransactionNrs.find(publicKeyIndex.publicKeyIndex);
			if (it == mIndexTransactionNrs.end()) {
				return 0;
			}
			return it->second.lastBalanceChangingTransactionNr;
		}

		bool AddressIndex::isExist(data::compact::PublicKeyIndex publicKeyIndex) const
		{
			if (publicKeyIndex.communityIdIndex != mCommunityIdIndex) {
				throw GradidoNodeInvalidDataException("dont't call AddressIndex::isExist with foreign publicKey");
			}
			auto it = mIndexTransactionNrs.find(publicKeyIndex.publicKeyIndex);
			return it != mIndexTransactionNrs.end();
		}
	}
}
