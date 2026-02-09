#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/data/hiero/AccountId.h"
#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/MultithreadQueue.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Type.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/serialization/toJsonString.h"
#include "LoadFromBinary.h"

#include <fstream>
#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <deque>

using std::ifstream;
using std::list;
using std::atomic;
using std::thread;
using std::vector;
using std::mutex, std::lock_guard;
using std::make_shared, std::shared_ptr;

using hiero::TransactionId, hiero::AccountId;

using gradido::g_appContext;
using gradido::blockchain::InMemoryProvider;
using gradido::data::GradidoTransaction, gradido::data::ConstGradidoTransactionPtr;
using gradido::data::ConfirmedTransaction, gradido::data::ConstConfirmedTransactionPtr;
using gradido::data::LedgerAnchor;
using serialization::toJsonString;
using namespace gradido::interaction;

void LoadFromBinary::SetUp()
{

}

void LoadFromBinary::TearDown()
{

}

struct DataWorkSet
{
	DataWorkSet(memory::ConstBlockPtr data) : data(data) {}
	memory::ConstBlockPtr data;
	ConstGradidoTransactionPtr tx;
};

static void workerFunc(int index, vector<shared_ptr<DataWorkSet>> *workDataSet)
{
	Profiler timeUsed;
	int count = 0;
	for(auto& work: *workDataSet) {
		if (!work) break;
		deserialize::Context deserializer(work->data, deserialize::Type::GRADIDO_TRANSACTION);
		deserializer.run();
		work->tx = deserializer.getGradidoTransaction();
		count++;
	};	
	// printf("thread %d finished %d work sets in %s\n", index, count, timeUsed.string().data());
}
/*
TEST_F(LoadFromBinary, LoadDataFromBinaryMultithread)
{
	ifstream f("data.bin", ifstream::in | ifstream::binary);
	uint16_t transactionSize = 0;
	queue<shared_ptr<DataWorkSet>> mWork;	
	size_t transactionCount = 0;
	
	Profiler timeUsed;

	while (f.good()) {
		f.read((char*)&transactionSize, sizeof(uint16_t));
		auto buffer = make_shared<memory::Block>(transactionSize);
		f.read((char*)(buffer->data()), transactionSize);
		mWork.emplace(make_shared<DataWorkSet>(buffer));
	}
	transactionCount = mWork.size();
	printf("%s time to load %llu transactions from binary file\n", timeUsed.string().data(), mWork.size());
	timeUsed.reset();
	auto workerCount = thread::hardware_concurrency() / 2;
	std::vector<shared_ptr<thread>> mWorkerThread(workerCount, nullptr);
	size_t workPerThread = floor((double)mWork.size() / (double)workerCount);
	vector<vector<shared_ptr<DataWorkSet>>> mWorkDatasets(workerCount + 1, { workPerThread, nullptr });
	for (int i = 0; i < workerCount; i++) {
		for (int w = 0; w < workPerThread; w++) {
			if (mWork.empty()) break;
			mWorkDatasets[i][w].swap(mWork.front());
			mWork.pop();
		}
		mWorkerThread[i] = make_shared<thread>(workerFunc, i, &mWorkDatasets[i]);
	}
	size_t w = 0;
	while (!mWork.empty()) {
		mWorkDatasets[workerCount][w++].swap(mWork.front());
		mWork.pop();
	}

	printf("%s time for prepare worker and worker queues\n", timeUsed.string().data());
	timeUsed.reset();

	workerFunc(workerCount, &mWorkDatasets[workerCount]);

	for (int i = 0; i < workerCount; i++) {
		mWorkerThread[i]->join();
	}
	// printf("\n");
	printf("%s time to deserialize %llu transactions with %lu workers\n", 
		timeUsed.string().data(), transactionCount, workerCount
	);
	int zahl = 1;
}
// */
/*
TEST_F(LoadFromBinary, LoadDataFromBinarySingleThreaded)
{
	std::ifstream f("data.bin", ifstream::in | ifstream::binary);
	uint16_t transactionSize = 0;

	// list<ConstGradidoTransactionPtr> transactions;
	Profiler timeUsed;
	std::string communityId = "test";
	auto provider = InMemoryProvider::getInstance();
	auto blockchain = provider->findBlockchain("test");
	AccountId defaultHieroAccount(0, 0, 2);
	int count = 0;

	while (f.good()) {
		f.read((char*)&transactionSize, sizeof(uint16_t));
		auto buffer = make_shared<memory::Block>(transactionSize);
		f.read((char*)(buffer->data()), transactionSize);
		deserialize::Context deserializer(buffer, deserialize::Type::GRADIDO_TRANSACTION);
		deserializer.run();
		// transactions.emplace(transactions.end(), deserializer.getGradidoTransaction());
		// printf("\r%llu", transactions.size());
		auto tx = deserializer.getGradidoTransaction();
		auto createdAt = tx->getTransactionBody()->getCreatedAt();
		TransactionId transactionId(createdAt, defaultHieroAccount);
		serialize::Context serializer(transactionId);
		auto transactionIdRaw = serializer.run();
		blockchain->createAndAddConfirmedTransaction(tx, transactionIdRaw, createdAt);

		count++;
		if (timeUsed.seconds() > 10.0) break;
	}
	// printf("\n");
	printf("%s time to load and deserialize %d transactions from binary file\n", 
		timeUsed.string().data(), count
	);
}
// */
#include "gradido_protobuf_zig.h"
#include <vector>
using std::vector;

#include <cassert>
#include "gradido_blockchain/interaction/deserialize/Protopuf.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_protobuf_zig.h"
// using gradido::interaction::deserialize::ConfirmedTransactionMessage, gradido::interaction::deserialize::ConfirmedTransactionIndexMessage;
// using gradido::interaction::deserialize::TransactionBodyMessage;
using namespace gradido::interaction::deserialize;

// gradido_transaction.proto
using GradidoTransactionSmallMessage = message<
	message_field<"sig_map", 1, SignatureMapMessage>,
	// bytes_field<"body_bytes", 2>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
	message_field<"pairing_ledger_anchor", 3, LedgerAnchorMessage>//, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
>;

using ConfirmedTransactionSmallMessage = message<
	uint64_field<"id", 1>,
	message_field<"transaction", 2, GradidoTransactionSmallMessage>,
	message_field<"confirmed_at", 3, TimestampMessage>,
	message_field<"ledger_anchor", 6, LedgerAnchorMessage>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
	message_field<"account_balances", 7, AccountBalanceMessage, repeated>,//, singular, std::vector<StringCachedAlloc>>
	enum_field<"balance_derivation", 8, gradido::data::BalanceDerivationType>
>;

TEST_F(LoadFromBinary, toFromProtobuf)
{
	std::ifstream f("data.bin", ifstream::in | ifstream::binary);
	uint16_t transactionSize = 0;
	std::string communityId = "gradido-akademie";
	auto communityIdIndex = g_appContext->getOrAddCommunityIdIndex(communityId);
	gradido::blockchain::InMemoryProvider::getInstance()->findBlockchain(communityIdIndex);

	Profiler timeUsed;
	std::string communityId = "gradido-akademie";
	auto communityIdIndex = g_appContext->getOrAddCommunityIdIndex(communityId);
	// std::deque<std::shared_ptr<memory::Block>> transactionBins;
	std::deque<gradido::data::compact::ConfirmedGradidoTx> transactions;
	printf("message size: %llu\n", sizeof(ConfirmedTransactionMessage));
	//const size_t bufferSize = 800;
	//std::array<uint8_t, bufferSize> buffer;
	size_t count = 0;
	int biggestTransactionSize = 0;
	int biggestBodyBytesSize = 0;
	int biggestZigMemorySize = 0;
	int encodeZigMemorySize = 0;
	int coundEncodingErrors = 0;
	int errorMemoSize[2] = { 0,0 };
	uint8_t transactionBuffer[1024];
	uint8_t encodingBuffer[1024];

	while (f.good()) {
		f.read((char*)&transactionSize, sizeof(uint16_t));
		if (transactionSize > 10000) break;
		// if (transactionSize > bufferSize) {
		// 	printf("buffer overflow, transaction size: %u\n", transactionSize);
		// 	break;
		// }
		if (transactionSize > biggestTransactionSize) {
			biggestTransactionSize = transactionSize;
		}
		f.read((char*)(transactionBuffer), transactionSize);
		
		auto bufferSpan = std::span<std::byte>{reinterpret_cast<std::byte*>(transactionBuffer), transactionSize};
		try {
			
			// 102 ms with 74k Transactions
			grdw_confirmed_transaction tx;
			auto confirmedDecodingMemSize = grdw_confirmed_transaction_decode(&tx, transactionBuffer, transactionSize);
			if (confirmedDecodingMemSize < 0) {
				throw GradidoNodeInvalidDataException("confirmed transaction decode failed!");
			}
			if (confirmedDecodingMemSize > biggestZigMemorySize) {
				biggestZigMemorySize = confirmedDecodingMemSize;
			}
			if (tx.transaction.body_bytes_size > biggestBodyBytesSize) {
				biggestBodyBytesSize = tx.transaction.body_bytes_size;
			}
			grdw_transaction_body body;
			auto bodyDecodingMemSize = grdw_transaction_body_decode(&body, tx.transaction.body_bytes, tx.transaction.body_bytes_size);
			if (bodyDecodingMemSize < 0) {
				throw GradidoNodeInvalidDataException("transaction body decode failed!");
			}
			if (bodyDecodingMemSize > biggestZigMemorySize) {
				biggestZigMemorySize = bodyDecodingMemSize;
			}
			memset(encodingBuffer, 0, 1024);
			auto encodeResult = grdw_transaction_body_encode(&body, encodingBuffer, 1024);
			if (encodeResult < 0) {
				deserialize::Context des(make_shared<const Block>(tx.transaction.body_bytes_size, tx.transaction.body_bytes), deserialize::Type::TRANSACTION_BODY);
				des.run(communityIdIndex);
				if (des.isTransactionBody()) {
					printf("%s\n", serialization::toJsonString(*des.getTransactionBody(), true).c_str());
				}				
				if (body.memos_count > 0) {
					auto& memoSize = body.memos[0].memo_size;
					if (!errorMemoSize[0] || memoSize < errorMemoSize[0]) {
						errorMemoSize[0] = memoSize;
					}
					if (memoSize > errorMemoSize[1]) {
						errorMemoSize[1] = memoSize;
					}
				}
				// throw GradidoNodeInvalidDataException("transaction body encode failed!");
				coundEncodingErrors++;
			}
			else {
				size_t bodyBytesSize = encodeResult;
				while (!encodingBuffer[--bodyBytesSize]);
				if (bodyBytesSize > encodeZigMemorySize) {
					encodeZigMemorySize = bodyBytesSize;
				}
			}
			
			// if (memcmp(buffer, tx.transaction.body_bytes, tx.transaction.body_bytes_size) != 0) {
				// throw GradidoNodeInvalidDataException("transaction body serialize isn't byte perfect");
			// }

			/*deserialize::Context des(make_shared<const Block>(tx.transaction.body_bytes_size, tx.transaction.body_bytes), deserialize::Type::TRANSACTION_BODY);
			des.run(communityIdIndex);
			auto legacyBody = des.getTransactionBody();
			*/
			/* if (body.transaction_type == GRDW_TRANSACTION_TYPE_CREATION) {
				printf("legacy body:\n%s\n", Block(tx.transaction.body_bytes_size, tx.transaction.body_bytes).convertToHex().c_str());
				printf("body:\n%s\n", Block(1024, encodingBuffer).convertToHex().c_str());
				break;
			}*/
			
			grdw_confirmed_transaction_free_deep(&tx);
			grdw_transaction_body_free_deep(&body);
			
			//
			
			//printf("version c: %s", tx.version_number);
			/*
			auto result = message_coder<ConfirmedTransactionMessage>::decode(bufferSpan);
			assert(result.has_value());
			auto& [confirmedTransaction, bufferEnd2] = *result;
			
			const auto& gradidoTransaction = confirmedTransaction["transaction"_f].value();
			auto bodyBytes = gradidoTransaction["body_bytes"_f].value();
			auto bodyBytesSpan = std::span<std::byte>{ reinterpret_cast<std::byte*>(bodyBytes.data()), bodyBytes.size() };

			
			// auto bodyBytesSpan = std::span<std::byte>{ reinterpret_cast<std::byte*>(tx.transaction.body_bytes), tx.transaction.body_bytes_size };
			
			// auto bodyResultLegacy = message_coder<TransactionBodyMessage>::decode(bodyBytesSpanLegacy);
			auto bodyResult = message_coder<TransactionBodyMessage>::decode(bodyBytesSpan);
			assert(bodyResult.has_value());
			// assert(bodyResultLegacy.has_value());
			auto& [transactionBody, bufferEnd3] = *bodyResult;
			// auto& [transactionBodyLegacy, bufferEnd4] = *bodyResultLegacy;
			
			
			//transactions.emplace_back();
			// transactions.emplace_back(std::move(confirmedTransaction));
			//
			// auto result2 = message_coder<ConfirmedTransactionMessage>::encode(confirmedTransaction, bufferSpan);
			// printf("after encode:\n");
			// assert(result2.has_value());
			//*/
			++count;
			// break;
			
		}
		catch (std::exception& ex) {
			printf("exception: %s on count: %d\n", ex.what(), count);
			throw;
		}
		// break;
	}
	printf("error memo sizes from %d to %d\n", errorMemoSize[0], errorMemoSize[1]);
	printf("biggest transaction size: %d\n", biggestTransactionSize);
	printf("biggest body bytes size: %d\n", biggestBodyBytesSize);
	printf("biggest zig memory size: %d\n", biggestZigMemorySize);
	printf("biggest zig memory size for encoding: %d\n", encodeZigMemorySize);
	printf("encoding errors: %d\n", coundEncodingErrors);
	printf("%s for deserialize and loading: %llu into memory\n", timeUsed.string().c_str(), count);
	timeUsed.reset();
}
// */
/*
TEST_F(LoadFromBinary, LoadDataFromBinaryDeserializeSerialize)
{
	Profiler timeUsed;
	uint16_t transactionSize = 0;
	
	std::ifstream f("data.bin", ifstream::in | ifstream::binary);
	
	// list<ConstGradidoTransactionPtr> transactions;
	timeUsed.reset();
	std::string communityId = "gradido-akademie";
	auto provider = InMemoryProvider::getInstance();
	auto blockchain = provider->findBlockchain(communityId);
	auto communityIdIndex = g_appContext->getCommunityIds().getIndexForData(communityId).value();
	AccountId defaultHieroAccount(0, 0, 2);
	vector<ConstConfirmedTransactionPtr> mTransactions;
	uint8_t staticInputBuffer[1024];
	grdu_memory alloc;
	int count = 0;

	char readFromFileStaticBuffer[1024];
	size_t confirmeDeserializeMaxAllocatorUsed = 0;
	size_t confirmedDeserializeMaxInputBuffer = 0;

	timeUsed.reset();
	grdu_memory_init_static(&alloc, staticInputBuffer, 1024);
	while (f.good()) {
		f.read((char*)&transactionSize, sizeof(uint16_t));
		f.read(readFromFileStaticBuffer, transactionSize);
		alloc.last_index = 0;
		
		grdw_confirmed_transaction tx{};
		auto result = grdw_confirmed_transaction_decode(&alloc, &tx, (uint8_t*)readFromFileStaticBuffer, transactionSize);
		if (alloc.last_index > confirmedDeserializeMaxInputBuffer) {
			confirmedDeserializeMaxInputBuffer = alloc.last_index;
		}
		if (result.allocator_used > confirmeDeserializeMaxAllocatorUsed) {
			confirmeDeserializeMaxAllocatorUsed = result.allocator_used;
		}
		++count;
		try {
			// ConfirmedTransaction::fromGrdw(&tx, communityIdIndex);
		}
		catch (...) {
			break;
		}
		
	}
	printf("%s time for simply deserialize confirmed tx with static buffer\n", timeUsed.string().c_str());
	printf(
		"deserialize stats: %llu max zig intern allocator used, %llu max input buffer usage\n", 
		confirmeDeserializeMaxAllocatorUsed, confirmedDeserializeMaxInputBuffer
	);
	mTransactions.reserve(count);

	count = 0;
	f.clear();
	f.seekg(0);
	timeUsed.reset();

	while (f.good()) {
		f.read((char*)&transactionSize, sizeof(uint16_t));
		auto buffer = make_shared<memory::Block>(transactionSize);
		f.read((char*)(buffer->data()), transactionSize);
		deserialize::Context deserializer(buffer, deserialize::Type::CONFIRMED_TRANSACTION);
		try {
			deserializer.run(communityIdIndex);
			if (deserializer.getType() == deserialize::Type::UNKNOWN) {
				printf("unknown type, size: %u\n", transactionSize);
				break;
			}
			// transactions.emplace(transactions.end(), deserializer.getGradidoTransaction());
			// printf("\r%llu", transactions.size());
			auto tx = deserializer.getConfirmedTransaction();
			if (!tx || deserializer.getType() == deserialize::Type::UNKNOWN) {
				break;
			}
			if (count > 10000) {
				// printf("%u: %s\n\n", transactionSize, serialization::toJsonString(*tx, true).data());
			tx->getGradidoTransaction()->getTransactionBody();
#ifdef _DEBUG
			if (count > 8000) {
				break;
				//printf("%u: %s\n\n", transactionSize, serialization::toJsonString(*tx, true).data());
			}
#endif
			// trigger body deserialization
			try {
				// printf("added: %s\n", toJsonString(*tx, true).data());
				mTransactions.emplace_back(tx);
			}
			catch (...) {
				printf("cannot deserialize transaction body from: %s\n", toJsonString(*tx, true).data());
				printf("size: %u\n", transactionSize);
			}
		}
		catch (...) {
			printf("error on transaction deserialize: %u\n", transactionSize);
		}
		count++;
	//	if (count > 10100) break;
		// if (count > 8000) break;
	}
	// printf("\n");
	printf("%s time to load and deserialize %d transactions from binary file into fat Object and put into vector\n",
		timeUsed.string().data(), count
	);
	// return;
	timeUsed.reset();	

	uint8_t staticResultBuffer[1024];
	
	
	size_t maxAllocateConfirmed = 0;
	size_t maxBufferConfirmed = 0;
	size_t maxAllocateTransaction = 0;
	size_t maxBufferTransaction = 0;
	size_t maxAllocatedBody = 0;
	size_t maxBufferBody = 0;
	size_t maxInputBuffer = 0;

	timeUsed.reset();
	for (auto& tx : mTransactions) {
		auto blockchainCommunityIdIndex = tx->getGradidoTransaction()->getCommunityIdIndex();
		grdw_confirmed_transaction grdw_tx;
		grdu_memory_init_static(&alloc, staticInputBuffer, 1024);
		tx->toGrdw(&alloc, &grdw_tx, blockchainCommunityIdIndex);
		if (alloc.last_index > maxInputBuffer) {
			maxInputBuffer = alloc.last_index;
		}
		auto encodeResult = grdw_confirmed_transaction_encode(&grdw_tx, staticResultBuffer, 1024);
		if (encodeResult.allocator_used > maxAllocateConfirmed) {
			maxAllocateConfirmed = encodeResult.allocator_used;
		}
		if (encodeResult.written > maxBufferConfirmed) {
			maxBufferConfirmed = encodeResult.written;
		}

	timeUsed.reset();
	count = 0;
	auto inMemoryBlockchain = static_cast<gradido::blockchain::InMemory*>(blockchain.get());
	for (auto& tx : mTransactions) {
		if (!tx) break;
		const auto& body = tx->getGradidoTransaction()->getTransactionBody();
		if (!body) {
			break;
		}
		serialize::Context c(*tx);
		c.run();
		count++;
	}
	printf("\n");
	printf("%s time for serialize %d transactions\n", timeUsed.string().data(), count);
	int zahl = 1;
}
// */
/*
TEST_F(LoadFromBinary, LoadDataFromBinarySingleThreadedBuffered)
{
	Profiler timeUsed;
	uint16_t transactionSize = 0;
	std::vector<memory::ConstBlockPtr> herzlichRawTxs;
	{
		std::ifstream herzlicht("herzlicht.dat", ifstream::in | ifstream::binary);
		while (herzlicht.good()) {
			herzlicht.read((char*)&transactionSize, sizeof(uint16_t));
			if (transactionSize > 800) break;
			auto buffer = make_shared<memory::Block>(transactionSize);
			herzlicht.read((char*)(buffer->data()), transactionSize);
			herzlichRawTxs.emplace_back(buffer);
		}
	}
	std::vector<memory::ConstBlockPtr> wekingheimRawTxs;
	{
		std::ifstream wekingheim("wekingheim.dat", ifstream::in | ifstream::binary);
		while (wekingheim.good()) {
			wekingheim.read((char*)&transactionSize, sizeof(uint16_t));
			if (transactionSize > 800) break;
			auto buffer = make_shared<memory::Block>(transactionSize);
			wekingheim.read((char*)(buffer->data()), transactionSize);
			herzlichRawTxs.emplace_back(buffer);
		}
	}
	printf("%s to load wekingheim and herzlicht raws\n", timeUsed.string().c_str());

	// std::ifstream f("data.bin", ifstream::in | ifstream::binary);
	std::ifstream academy("gradido_akademie.dat", ifstream::in | ifstream::binary);
	
	// list<ConstGradidoTransactionPtr> transactions;
	timeUsed.reset();
	std::string communityId = "gradido-akademie";
	auto provider = InMemoryProvider::getInstance();
	auto blockchain = provider->findBlockchain(communityId);
	auto communityIdIndex = g_appContext->getCommunityIds().getIndexForData(communityId).value();
	AccountId defaultHieroAccount(0, 0, 2);
	list<ConstConfirmedTransactionPtr> mTransactions;
	int count = 0;

	while (academy.good()) {
		academy.read((char*)&transactionSize, sizeof(uint16_t));
		auto buffer = make_shared<memory::Block>(transactionSize);
		academy.read((char*)(buffer->data()), transactionSize);
		deserialize::Context deserializer(buffer, deserialize::Type::CONFIRMED_TRANSACTION);
		try {
			deserializer.run(communityIdIndex);
			if (deserializer.getType() == deserialize::Type::UNKNOWN) {
				printf("unknown type, size: %u\n", transactionSize);
				break;
			}
			// transactions.emplace(transactions.end(), deserializer.getGradidoTransaction());
			// printf("\r%llu", transactions.size());
			auto tx = deserializer.getConfirmedTransaction();
			if (!tx && deserializer.getType() == deserialize::Type::UNKNOWN) {
				break;
			}
			tx->getGradidoTransaction()->getTransactionBody();
#ifdef _DEBUG
			if (count > 8000) {
				break;
				//printf("%u: %s\n\n", transactionSize, serialization::toJsonString(*tx, true).data());
			}
#endif
			// trigger body deserialization
			try {
				// printf("added: %s\n", toJsonString(*tx, true).data());
				mTransactions.emplace_back(tx);
			}
			catch (...) {
				printf("cannot deserialize transaction body from: %s\n", toJsonString(*tx, true).data());
				printf("size: %u\n", transactionSize);
			}
		}
		catch (...) {
			printf("error on transaction deserialize: %u\n", transactionSize);
		}
		count++;
		// if (count > 8000) break;
	}
	// printf("\n");
	printf("%s time to load and deserialize %d transactions from binary file\n",
		timeUsed.string().data(), mTransactions.size()
	);
	
	timeUsed.reset();
	count = 0;
	auto inMemoryBlockchain = static_cast<gradido::blockchain::InMemory*>(blockchain.get());
	for (auto& tx : mTransactions) {
		if (!tx) break;
		const auto& body = tx->getGradidoTransaction()->getTransactionBody();
		if (!body) {
			break;
		}
		auto createdAt = body->getCreatedAt();
		TransactionId transactionId(createdAt, defaultHieroAccount);
		if (body->isTimeoutDeferredTransfer()) {
			// printf("skip timeout deferred transfer\n");
			continue;
		}
		try {
			// inMemoryBlockchain->createAndAddConfirmedTransactionExtern(tx->getGradidoTransaction(), LedgerAnchor(transactionId), tx->getAccountBalances());
			// printf("%llu: %s\n\n", tx->getId(), serialization::toJsonString(*tx, true).data());
		}
		catch (GradidoBlockchainException& ex) {
			printf("\nexception: %s\n", ex.getFullString().data());
			printf("createdAt: %ld %d, nr: %llu\n", createdAt.getSeconds(), createdAt.getNanos(), tx->getId());
			int zahl = 1;
			throw;
		}
		catch (std::exception& ex) {
			printf("\nex: %s\n", ex.what());
			int zahl = 2;
			throw;
		}
		catch (...) {
			printf("\nunknow exceptions\n");
			throw;
		}
		count++;
		// if (count > 100) break;
		// printf("\rtransactions: %d", count);
		// if (timeUsed.seconds() > 30.0) break;
	}
	printf("time for confirmed transaction toGrdw and encode: %s\n", timeUsed.string().c_str());
	printf(
		"confirmed stats: max allocated (zig intern): %llu, max buffer (output): %llu, max input buffer (toGrdw): %llu\n",
		maxAllocateConfirmed, maxBufferConfirmed, maxInputBuffer
	);

	timeUsed.reset();
	maxInputBuffer = 0;
	for (auto& tx : mTransactions) {
		grdw_gradido_transaction grdw_tx;
		grdu_memory_init_static(&alloc, staticInputBuffer, 1024);
		auto blockchainCommunityIdIndex = tx->getGradidoTransaction()->getCommunityIdIndex();
		tx->getGradidoTransaction()->toGrdw(&alloc, &grdw_tx, blockchainCommunityIdIndex);
		if (alloc.last_index > maxInputBuffer) {
			maxInputBuffer = alloc.last_index;
		}
		auto encodeResult = grdw_gradido_transaction_encode(&grdw_tx, staticResultBuffer, 1024);
		if (encodeResult.allocator_used > maxAllocateTransaction) {
			maxAllocateTransaction = encodeResult.allocator_used;
		}
		if (encodeResult.written > maxBufferTransaction) {
			maxBufferTransaction = encodeResult.written;
		}
	}
	printf("time for gradido transaction toGrdw and encode: %s\n", timeUsed.string().c_str());
	printf(
		"transaction stats: max allocated (zig intern): %llu, max buffer (output): %llu, max input buffer (toGrdw): %llu\n",
		maxAllocateTransaction, maxBufferTransaction, maxInputBuffer
	);
	timeUsed.reset();
	maxInputBuffer = 0;
	for (auto& tx : mTransactions) {
		grdw_transaction_body grdw_body;
		grdu_memory_init_static(&alloc, staticInputBuffer, 1024);
		tx->getGradidoTransaction()->getTransactionBody()->toGrdw(&alloc, &grdw_body);
		if (alloc.last_index > maxInputBuffer) {
			maxInputBuffer = alloc.last_index;
		}
		auto encodeResult = grdw_transaction_body_encode(&grdw_body, staticResultBuffer, 1024);
		if (encodeResult.allocator_used > maxAllocatedBody) {
			maxAllocatedBody = encodeResult.allocator_used;
		}
		if (encodeResult.written > maxBufferBody) {
			maxBufferBody = encodeResult.written;
		}
	}
	printf("time for transaction body toGrdw and encode: %s\n", timeUsed.string().c_str());
	printf(
		"body stats:max allocated (zig intern): %llu, max buffer (output): %llu, max input buffer (toGrdw): %llu\n",
		maxAllocatedBody, maxBufferBody, maxInputBuffer
	);
	
	int zahl = 1;
}
// */