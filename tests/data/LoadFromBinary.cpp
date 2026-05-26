#include "gradido_blockchain_core/memory.h"
#include "gradido_blockchain_core/data/wire/confirmed_transaction.h"
#include "gradido_blockchain_core/data/wire/gradido_transaction.h"
#include "gradido_blockchain_core/data/wire/transaction_body.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/batch/signaturesVerify.h"
#include "gradido_blockchain/blockchain/batch/ThreadingPolicy.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/blockchain/TransactionsIndex.h"
#include "gradido_blockchain/data/adapter/uuid.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/data/hiero/AccountId.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/lib/MonotonicTimer.h"
#include "gradido_blockchain/lib/MultithreadQueue.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Type.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/serialization/toJsonString.h"
#include "LoadFromBinary.h"

#include <atomic>
#include <deque>
#include <filesystem>
#include <fstream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

using std::atomic;
using std::filesystem::file_size;
using std::ifstream;
using std::list;
using std::make_shared, std::shared_ptr;
using std::mutex, std::lock_guard;
using std::thread;
using std::vector;

using hiero::TransactionId, hiero::AccountId;

using gradido::g_appContext;
using gradido::blockchain::batch::verifySignatures, gradido::blockchain::batch::ThreadingPolicy;
using gradido::blockchain::Filter, gradido::blockchain::InMemoryProvider, gradido::blockchain::TransactionsIndex;
using gradido::data::adapter::uuidToString;
using gradido::data::GradidoTransaction, gradido::data::ConstGradidoTransactionPtr;
using gradido::data::ConfirmedTransaction, gradido::data::ConstConfirmedTransactionPtr;
using gradido::data::CrossGroupType;
using gradido::data::LedgerAnchor;
using gradido::data::Timestamp;
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
	MonotonicTimer timeUsed;
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
	
	MonotonicTimer timeUsed;

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
	MonotonicTimer timeUsed;
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
#include "magic_enum/magic_enum.hpp"

#include <cassert>
#include "gradido_blockchain/interaction/deserialize/Protopuf.h"

using namespace gradido::interaction::deserialize;
using namespace magic_enum;
/*

TEST_F(LoadFromBinary, toFromProtobuf)
{
	std::ifstream f("data.bin", ifstream::in | ifstream::binary);
	uint16_t transactionSize = 0;
	std::string communityId = "gradido-akademie";
	auto communityIdIndex = g_appContext->getOrAddCommunityIdIndex(communityId);
	gradido::blockchain::InMemoryProvider::getInstance()->findBlockchain(communityIdIndex);

	MonotonicTimer timeUsed;
	// std::deque<std::shared_ptr<memory::Block>> transactionBins;
	std::deque<gradido::data::compact::ConfirmedGradidoTx> transactions;
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
	uint8_t encodingBuffer[2048];
	uint8_t inputBuffer[2048];
	grdu_memory alloc;
	grdu_memory_init_static(&alloc, inputBuffer, 2048);

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
			alloc.last_index = 0;
			auto decodeResult = grdw_confirmed_transaction_decode(&alloc, &tx, transactionBuffer, transactionSize);
			if (GRDW_ENCODING_ERROR_SUCCESS != decodeResult.state) {
				printf("confirmed transaction decode error: %s\n", enum_name(decodeResult.state).data());
				throw GradidoNodeInvalidDataException("confirmed transaction decode failed!");
			}
			if (decodeResult.allocator_used > biggestZigMemorySize) {
				biggestZigMemorySize = decodeResult.allocator_used;
			}
			if (tx.transaction.body_bytes_size > biggestBodyBytesSize) {
				biggestBodyBytesSize = tx.transaction.body_bytes_size;
			}
			grdw_transaction_body body;
			alloc.last_index = 0;
			decodeResult = grdw_transaction_body_decode(&alloc , &body, tx.transaction.body_bytes, tx.transaction.body_bytes_size);
			if (GRDW_ENCODING_ERROR_SUCCESS != decodeResult.state) {
				printf("transaction body decode error: %s\n", enum_name(decodeResult.state).data());
				throw GradidoNodeInvalidDataException("transaction body decode failed!");
			}
			if (decodeResult.allocator_used > biggestZigMemorySize) {
				biggestZigMemorySize = decodeResult.allocator_used;
			}
			
			auto encodeResult = grdw_transaction_body_encode(&body, encodingBuffer, 2048);
			if (GRDW_ENCODING_ERROR_SUCCESS != encodeResult.state) {
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
				size_t bodyBytesSize = encodeResult.written;
				if (bodyBytesSize > encodeZigMemorySize) {
					encodeZigMemorySize = bodyBytesSize;
				}
			}
			
			// if (memcmp(buffer, tx.transaction.body_bytes, tx.transaction.body_bytes_size) != 0) {
				// throw GradidoNodeInvalidDataException("transaction body serialize isn't byte perfect");
			// }

			deserialize::Context des(make_shared<const Block>(tx.transaction.body_bytes_size, tx.transaction.body_bytes), deserialize::Type::TRANSACTION_BODY);
			des.run(communityIdIndex);
			auto legacyBody = des.getTransactionBody();
			
			 if (body.transaction_type == GRDW_TRANSACTION_TYPE_CREATION) {
				printf("legacy body:\n%s\n", Block(tx.transaction.body_bytes_size, tx.transaction.body_bytes).convertToHex().c_str());
				printf("body:\n%s\n", Block(1024, encodingBuffer).convertToHex().c_str());
				break;
			}
					
			
			//
			
			//printf("version c: %s", tx.version_number);
			
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
			//
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
using gradido::data::compact::ConfirmedGradidoTx;
using std::shared_ptr;
using namespace gradido;

struct DataSet {
	const char* communityId;
	const char* fileName;
	queue<shared_ptr<const ConfirmedTransaction>> transactions;
	queue<shared_ptr<const ConfirmedGradidoTx>> compactTransactions;
	shared_ptr<blockchain::InMemory> blockchain;
};

TEST_F(LoadFromBinary, LoadAndConfirm)
{
	MonotonicTimer timeUsed;
	MonotonicTimer timeUsedAll;
	MonotonicTimer timeSinceLastPrint;
	DataSet communities[] = {
		{ .communityId = "e70da33e-5976-4767-bade-aa4e4fa1c01a", .fileName = "gradido_akademie.dat" }
	};
	const int communityCount = 1;
	char readFromFileStaticBuffer[1024];
	uint8_t staticInputBuffer[4096];
	grd_memory alloc;
	grd_memory_init_arena_static(&alloc, staticInputBuffer, 4096);
	grdw_confirmed_transaction tx{};
	grdw_transaction_body body{};

	auto provider = InMemoryProvider::getInstance();
	for (int i = 0; i < communityCount; ++i) {
		// init all blockchains and dictionaries
		provider->findBlockchain(communities[i].communityId);
	}
	// load from file, deserialize, create object
	for (uint32_t i = 0; i < communityCount; ++i) {
		timeUsed.reset();
		auto& com = communities[i];
		com.blockchain = reinterpret_pointer_cast<blockchain::InMemory>(provider->findBlockchain(com.communityId));
		ASSERT_EQ(com.blockchain->getCommunityIdIndex(), i+1);
		ifstream f(com.fileName, ifstream::in | ifstream::binary);
		auto fileSize = file_size(com.fileName);
		uint16_t txSize = 0;
		uint32_t readed = 0;
		uint32_t count = 0;
		while (f.good()) 
		{
			f.read((char*)&txSize, sizeof(uint16_t));
			readed += sizeof(uint16_t);
			f.read(readFromFileStaticBuffer, txSize);
			readed += txSize;

			alloc.last_index = 0;
			grd_memory_block src = { .data = (uint8_t*)readFromFileStaticBuffer, .size = txSize };
			auto decodeResult = grdw_confirmed_transaction_decode(&tx, &src, &alloc);
			ASSERT_EQ(decodeResult, GRD_SUCCESS);
			decodeResult = grdw_transaction_body_decode(&body, &tx.transaction.body_bytes, &alloc);
			ASSERT_EQ(decodeResult, GRD_SUCCESS);

			if (GRDT_LEDGER_ANCHOR_NODE_TRIGGER_TRANSACTION_ID != tx.ledger_anchor.type) {
				// com.transactions.push(ConfirmedTransaction::fromGrdw(&tx, i+1));
				
				/*auto compact = make_shared<ConfirmedGradidoTx>(ConfirmedGradidoTx::fromGrdwConfirmedTransaction(&tx, i+1));
				alloc.last_index = 0;
				decodeResult = grdw_transaction_body_decode(&alloc, &body, tx.transaction.body_bytes, tx.transaction.body_bytes_size);
				ASSERT_EQ(decodeResult.state, GRDW_ENCODING_ERROR_SUCCESS);
				compact->fillFromGrdwTransactionBody(&body);
				
				// com.compactTransactions.push(compact);
				// */
			}

			if (readed + 32 >= fileSize) {
				break;
			}
			++count;
		}
		printf("%s for loading %u confirmed transactions for %s\n", timeUsed.string().c_str(), count, com.communityId);
	}
	return;

	timeUsed.reset();
	timeSinceLastPrint.reset();
	uint32_t count = 0;
	while (communities[0].transactions.size() || communities[1].transactions.size() || communities[2].transactions.size()) {
		Timestamp oldest = { 0,0 };
		int32_t next = -1;
		for (int i = 0; i < communityCount; ++i) {
			if (communities[i].transactions.empty()) {
				continue;
			}
			auto confirmedAt = communities[i].transactions.front()->getConfirmedAt();
			if (confirmedAt < oldest || oldest.empty()) {
				oldest = confirmedAt;
				next = i;
			}
			else if (confirmedAt == oldest && i != next) {
				auto bodyThis = communities[i].transactions.front()->getGradidoTransaction()->getTransactionBody();
				auto bodyNext = communities[next].transactions.front()->getGradidoTransaction()->getTransactionBody();
				if (bodyThis->getType() == CrossGroupType::OUTBOUND && bodyNext->getType() == CrossGroupType::INBOUND) {
					next = i;
				}
			}
		}
		if (-1 == next) {
			break;
		}
		const auto& confirmedTx = communities[next].transactions.front();
		auto tx = communities[next].transactions.front()->getGradidoTransaction();
		if (tx->getTransactionBody()->getType() == CrossGroupType::INBOUND) {
			int zahl = 0;
		}
		ASSERT_NO_THROW(
			communities[next].blockchain->createAndAddConfirmedTransactionExternFast(
				tx, confirmedTx->getLedgerAnchor(), confirmedTx->getAccountBalances()
			)
		);
		communities[next].transactions.pop();
		++count;
		/*if (timeSinceLastPrint.millis() > 100) {
			timeSinceLastPrint.reset();
			printf("\r%u", count);
		}*/
		// if (count > 5000) break;
	}
	// printf("\r%u\n", count);
	printf("%s for deserailize body and confirm %u transactions\n", timeUsed.string().c_str(), count);
	timeUsed.reset();
	// bulk verify
	for (int i = 0; i < communityCount; ++i) {
		verifySignatures(Filter::ALL_TRANSACTIONS, communities[i].communityId, ThreadingPolicy::AllExceptOne);
	}
	printf("%s for bulk verify all\n", timeUsed.string().c_str());

	// test request cross group balance
	Filter f = Filter::ALL_TRANSACTIONS;
	// f.coinCommunityIdIndex = 0;
	auto txs = communities[1].blockchain->findAll(f);
	printf("found txs: %d\n", txs.size());
	for (const auto& tx : txs) {
		auto coin = g_appContext->getCommunityIds().getDataForIndexOrThrow(tx->getConfirmedTransaction()->getAccountBalances()[0].getCoinCommunityIdIndex());
		printf("coin color: %s\n", uuidToString(coin).c_str());
	}

	int zahl = 0;
	printf("%s time for all\n", timeUsedAll.string().c_str());
}
/*
TEST_F(LoadFromBinary, LoadDataFromBinaryDeserializeSerialize)
{
	MonotonicTimer timeUsed;
	uint16_t transactionSize = 0;
	uint16_t maxTransactionSize = 0;
	
	std::ifstream f("data.bin", ifstream::in | ifstream::binary);
	
	// list<ConstGradidoTransactionPtr> transactions;
	timeUsed.reset();
	std::string communityIds[] = {
		"gradido-akademie",
		"herzlicht",
		"wekingheim"
	};
	std::string communityId = communityIds[0];
	auto provider = InMemoryProvider::getInstance();
	auto blockchain = provider->findBlockchain(communityId);
	provider->findBlockchain(communityIds[1]);
	provider->findBlockchain(communityIds[2]);
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
	std::deque<ConfirmedGradidoTx> mCompactTxs;
	TransactionsIndex txIndex;
	
	grdw_confirmed_transaction tx{};
	grdw_transaction_body body{};

	while (f.good()) {
		f.read((char*)&transactionSize, sizeof(uint16_t));
		if (transactionSize < 48787 && transactionSize > maxTransactionSize) {
			maxTransactionSize = transactionSize;
		}
		f.read(readFromFileStaticBuffer, transactionSize);
		alloc.last_index = 0;
		
		try {			
			auto result = grdw_confirmed_transaction_decode(&alloc, &tx, (uint8_t*)readFromFileStaticBuffer, transactionSize);
			if (GRDW_ENCODING_ERROR_SUCCESS != result.state) {
				printf("%d error on encoding tx: %d\n", result.state, count);
				break;
			}
			if (alloc.last_index > confirmedDeserializeMaxInputBuffer) {
				confirmedDeserializeMaxInputBuffer = alloc.last_index;
			}
			if (result.allocator_used > confirmeDeserializeMaxAllocatorUsed) {
				confirmeDeserializeMaxAllocatorUsed = result.allocator_used;
			}
			auto confirmedTx = ConfirmedGradidoTx::fromGrdwConfirmedTransaction(&tx, communityIdIndex, false);
			alloc.last_index = 0;
			
			auto bodyResult = grdw_transaction_body_decode(&alloc, &body, tx.transaction.body_bytes, tx.transaction.body_bytes_size);
			if (GRDW_ENCODING_ERROR_SUCCESS != bodyResult.state) {
				printf("%d error on encoding tx body: %d\n", bodyResult.state, count);
				break;
			}
			confirmedTx.fillFromGrdwTransactionBody(&body);
			txIndex.addIndicesForTransaction(&tx.transaction, confirmedTx);
			++count;		
			// mCompactTxs.emplace_back(std::move(confirmedTx));
		}
		catch (GradidoBlockchainException& ex) {
			printf("exception in fromGrdw: %s\n", ex.getFullString().c_str());
			break;
		}
		
	}
	printf("%s time for deserialize confirmed tx and body, add to deque as compact confirmed tx and initalize transaction index\n", timeUsed.string().c_str());
	printf(
		"deserialize stats: %llu max zig intern allocator used, %llu max input buffer usage\n", 
		confirmeDeserializeMaxAllocatorUsed, confirmedDeserializeMaxInputBuffer
	);
	printf("max transaction size: %u\n", maxTransactionSize);
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
			}
			// trigger body deserialization
			try {
				// printf("added: %s\n", toJsonString(*tx, true).data());
				mTransactions.emplace_back(tx);
			}
			catch (...) {
				printf("cannot deserialize transaction body from: %s\n", toJsonString(*tx, true).data());
				printf("size: %u\n", transactionSize);
			}
		} catch (...) {
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
	MonotonicTimer timeUsed;
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