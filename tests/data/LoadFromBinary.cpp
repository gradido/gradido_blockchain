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

TEST_F(LoadFromBinary, LoadDataFromBinarySingleThreadedBuffered)
{
	std::ifstream f("data.bin", ifstream::in | ifstream::binary);
	uint16_t transactionSize = 0;

	// list<ConstGradidoTransactionPtr> transactions;
	Profiler timeUsed;
	std::string communityId = "gradido-akademie";
	auto communityIdIndex = g_appContext->getOrAddCommunityIdIndex(communityId);
	auto provider = InMemoryProvider::getInstance();
	auto blockchain = provider->findBlockchain("test");
	AccountId defaultHieroAccount(0, 0, 2);
	list<ConstConfirmedTransactionPtr> mTransactions;
	int count = 0;

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
			}
			// trigger body deserialization
			try {
				// printf("added: %s\n", toJsonString(*tx, true).data());
				//mTransactions.emplace_back(tx);
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
	}
	// printf("\n");
	printf("%s time to load and deserialize %d transactions from binary file\n",
		timeUsed.string().data(), count
	);
	return;
	timeUsed.reset();
	vector<grdw_confirmed_transaction> grdw_confirmed_txs;
	grdw_confirmed_txs.resize(count);
	printf("reserve memory for %d grdw_confirmed_transaction: %s\n", count, timeUsed.string().c_str());

	count = 0;
	uint8_t staticResultBuffer[1024];

	size_t maxAllocateConfirmed = 0;
	size_t maxBufferConfirmed = 0;
	size_t maxAllocateTransaction = 0;
	size_t maxBufferTransaction = 0;
	size_t maxAllocatedBody = 0;
	size_t maxBufferBody = 0;

	timeUsed.reset();
	count = 0;
	for (auto& tx : mTransactions) {
		auto blockchainCommunityIdIndex = tx->getGradidoTransaction()->getCommunityIdIndex();
		grdw_confirmed_transaction grdw_tx;
		tx->toGrdw(&grdw_tx, blockchainCommunityIdIndex);
		grdw_confirmed_transaction_encode(&grdw_tx, staticResultBuffer, 1024);
		grdw_confirmed_transaction_free_deep(&grdw_tx);
		++count;
	}
	printf("time for %d confirmed to grdw calls + encode + free: %s\n", count, timeUsed.string().c_str());

	timeUsed.reset();
	count = 0;
	for (auto& tx : mTransactions) {
		auto blockchainCommunityIdIndex = tx->getGradidoTransaction()->getCommunityIdIndex();
		tx->toGrdw(&grdw_confirmed_txs[count++], blockchainCommunityIdIndex);
	}
	printf("time for %d confirmed to grdw calls: %s\n", count, timeUsed.string().c_str());
	timeUsed.reset();
	for (auto& grdw_tx : grdw_confirmed_txs) {
		auto encodeResult = grdw_confirmed_transaction_encode(&grdw_tx, staticResultBuffer, 1024);
		if (encodeResult.allocator_used > maxAllocateConfirmed) {
			maxAllocateConfirmed = encodeResult.allocator_used;
		}
		if (encodeResult.written > maxBufferConfirmed) {
			maxBufferConfirmed = encodeResult.written;
		}
	}
	printf("time for %d grdw_confirmed_transaction_encode calls: %s\n", count, timeUsed.string().c_str());
	printf("confirmed stats: max allocated: %llu, max buffer: %llu\n", maxAllocateConfirmed, maxBufferConfirmed);

	timeUsed.reset();
	for (auto& grdw_tx : grdw_confirmed_txs) {
		auto encodeResult = grdw_gradido_transaction_encode(&grdw_tx.transaction, staticResultBuffer, 1024);
		if (encodeResult.allocator_used > maxAllocateTransaction) {
			maxAllocateTransaction = encodeResult.allocator_used;
		}
		if (encodeResult.written > maxBufferTransaction) {
			maxBufferTransaction = encodeResult.written;
		}
	}
	printf("time for %d grdw_gradido_transaction_encode calls: %s\n", count, timeUsed.string().c_str());
	printf("transaction stats: max allocated: %llu, max buffer: %llu\n", maxAllocateTransaction, maxBufferTransaction);
	timeUsed.reset();
	for (auto& grdw_tx : grdw_confirmed_txs) {
		grdw_confirmed_transaction_free_deep(&grdw_tx);
	}
	grdw_confirmed_txs.clear();
	printf("time for %d grdw_confirmed_transaction_free_deep calls: %s\n", count, timeUsed.string().c_str());
	
	timeUsed.reset();
	vector<grdw_transaction_body> grdw_bodys;
	grdw_bodys.resize(count);
	printf("reserve memory for %d grdw_transaction_body: %s\n", count, timeUsed.string().c_str());

	timeUsed.reset();
	count = 0;
	for (auto& tx : mTransactions) {
		tx->getGradidoTransaction()->getTransactionBody()->toGrdw(&grdw_bodys[count++]);
	}
	printf("time for %d body to grdw calls: %s\n", count, timeUsed.string().c_str());

	timeUsed.reset();
	for (auto& grdw_body : grdw_bodys) {
		auto encodeResult = grdw_transaction_body_encode(&grdw_body, staticResultBuffer, 1024);
		if (encodeResult.allocator_used > maxAllocatedBody) {
			maxAllocatedBody = encodeResult.allocator_used;
		}
		if (encodeResult.written > maxBufferBody) {
			maxBufferBody = encodeResult.written;
		}
	}
	printf("time for %d grdw_transaction_body_encode calls: %s\n", count, timeUsed.string().c_str());
	printf("body stats: max allocated: %llu, max buffer: %llu\n", maxAllocatedBody, maxBufferBody);

	timeUsed.reset();
	for (auto& grdw_body : grdw_bodys) {
		grdw_transaction_body_free_deep(&grdw_body);
	}
	grdw_bodys.clear();
	printf("time for %d grdw_transaction_body_free_deep calls: %s\n", count, timeUsed.string().c_str());
	
	int zahl = 1;
}
// */