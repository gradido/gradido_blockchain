#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
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

using gradido::blockchain::InMemoryProvider;
using gradido::data::GradidoTransaction, gradido::data::ConstGradidoTransactionPtr;
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
*/
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
*/

TEST_F(LoadFromBinary, LoadDataFromBinarySingleThreadedBuffered)
{
	std::ifstream f("data.bin", ifstream::in | ifstream::binary);
	uint16_t transactionSize = 0;

	// list<ConstGradidoTransactionPtr> transactions;
	Profiler timeUsed;
	std::string communityId = "test";
	auto provider = InMemoryProvider::getInstance();
	auto blockchain = provider->findBlockchain("test");
	AccountId defaultHieroAccount(0, 0, 2);
	list<ConstGradidoTransactionPtr> mTransactions;

	while (f.good()) {
		f.read((char*)&transactionSize, sizeof(uint16_t));
		auto buffer = make_shared<memory::Block>(transactionSize);
		f.read((char*)(buffer->data()), transactionSize);
		deserialize::Context deserializer(buffer, deserialize::Type::GRADIDO_TRANSACTION);
		deserializer.run();
		// transactions.emplace(transactions.end(), deserializer.getGradidoTransaction());
		// printf("\r%llu", transactions.size());
		auto tx = deserializer.getGradidoTransaction();
		// trigger body deserialization
		try {
			tx->getTransactionBody();
			// printf("added: %s\n", toJsonString(*tx, true).data());
			mTransactions.emplace_back(tx);
		}
		catch (...) {
			printf("cannot deserialize transaction body from: %s\n", toJsonString(*tx, true).data());
			printf("size: %u\n", transactionSize);
		}		
	}
	// printf("\n");
	printf("%s time to load and deserialize %d transactions from binary file\n",
		timeUsed.string().data(), mTransactions.size()
	);
	timeUsed.reset();
	int count = 0;
	for (auto& tx : mTransactions) {
		auto createdAt = tx->getTransactionBody()->getCreatedAt();
		TransactionId transactionId(createdAt, defaultHieroAccount);
		serialize::Context serializer(transactionId);
		auto transactionIdRaw = serializer.run();
		try {
			blockchain->createAndAddConfirmedTransaction(tx, transactionIdRaw, createdAt);
		}
		catch (GradidoBlockchainException& ex) {
			printf("\nexception: %s\n", ex.getFullString().data());
			int zahl = 1;
			// throw;
		}
		catch (std::exception& ex) {
			printf("\nex: %s\n", ex.what());
			int zahl = 2;
			// throw;
		}
		catch (...) {
			printf("\nunknow exceptions\n");
			// throw;
		}
		count++;
		printf("\rtransactions: %d", count);
		// if (timeUsed.seconds() > 30.0) break;
	}
	printf("\n");
	printf("%s time for adding %d transactions to blockchain\n", timeUsed.string().data(), count);
	int zahl = 1;
}
// */