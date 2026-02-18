
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/batch/signaturesVerify.h"
#include "gradido_blockchain/blockchain/batch/ThreadingPolicy.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/Pagination.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"

#include "sodium.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using std::atomic;
using std::condition_variable;
using std::mutex, std::unique_lock;
using std::queue;
using std::ref, std::thread, std::this_thread::sleep_for;
using std::vector;

namespace gradido::blockchain::batch {

  void worker(
    condition_variable& cvMasterWorker,
    condition_variable& cvWorkerMaster,
    mutex& mtx,
    queue<ConstTransactionEntryPtr>& transactionQueue,
    vector<uint64_t>& invalidTxNrs,
    atomic<bool>& done
  )
  {
    vector<ConstTransactionEntryPtr> batch;
    batch.reserve(500);
    while(true) {
      unique_lock<mutex> lock(mtx);
      if (transactionQueue.empty())
      {
        lock.unlock();
        cvWorkerMaster.notify_one();
        lock.lock();
        cvMasterWorker.wait(lock, [&transactionQueue, &done]() {
          return done || !transactionQueue.empty();
          }
        );
      }

      if (done && transactionQueue.empty()) {
        break;
      }
      do {
        batch.push_back(transactionQueue.front());
        transactionQueue.pop();
      } while (!transactionQueue.empty() && batch.size() < 500);
      lock.unlock();
      for(const auto& tx : batch)
      {
        const auto& gradidoTx = tx->getConfirmedTransaction()->getGradidoTransaction();
        const auto& signaturePairs = gradidoTx->getSignatureMap().getSignaturePairs();
        const auto& bodyBytes = gradidoTx->getBodyBytes();
        for (const auto& signaturePair : signaturePairs) {
          if (0 != crypto_sign_verify_detached(
            signaturePair.getSignature()->data(),
            bodyBytes->data(),
            bodyBytes->size(),
            signaturePair.getPublicKey()->data()
          )) {
            lock.lock();
            invalidTxNrs.emplace_back(tx->getTransactionNr());
            lock.unlock();
            break;
          }
        }
      }
      batch.clear();
    }
  }

  vector<uint64_t> verifySignatures(const Filter& filter, const std::string& communityId, ThreadingPolicy policy)
  {
    auto threadCount = resolveThreadCount(policy);
    auto communityIdIndex = g_appContext->getCommunityIds().getIndexForData(communityId);
    if (!communityIdIndex) {
      throw DictionaryMissingEntryException("communityId not found", communityId);
    }
    auto blockchain = g_appContext->getCommunityContext(*communityIdIndex).getBlockchain();
    vector<uint64_t> invalidTxNrs;
    queue<ConstTransactionEntryPtr> transactionQueue;
    const size_t queueSize = 500 * (threadCount + 1);
    condition_variable cvMasterWorker, cvWorkerMaster;
    mutex mtx;
    atomic<bool> done(false);
    vector<thread> threads;
    threads.reserve(threadCount);
    Pagination pagination = filter.pagination;
    if (!pagination.size) {
      pagination.size = 100;
    }
    if (!pagination.page) {
      pagination.page = 1;
    }

    for(size_t i = 0; i < threadCount; i++) {
      threads.emplace_back(
        worker,
        ref(cvMasterWorker),
        ref(cvWorkerMaster),
        ref(mtx),
        ref(transactionQueue),
        ref(invalidTxNrs),
        ref(done)
      );
    }
    while(!done) {
      unique_lock<mutex> lock(mtx);
      while (transactionQueue.size() < queueSize) {
        lock.unlock();
        Filter filterCopy = filter;
        filterCopy.pagination = pagination;
        auto transactions = blockchain->findAll(filterCopy);
        if (transactions.empty()) {
          done = true;
          lock.lock();
          break;
        }
        lock.lock();
        for (const auto& transaction : transactions) {
          transactionQueue.push(transaction);
        }
        lock.unlock();
        cvMasterWorker.notify_one();
        pagination.page++;
        lock.lock();
      }
      lock.unlock();
      cvMasterWorker.notify_all();
      if (!done) {
        lock.lock();
        cvWorkerMaster.wait(lock, [&] {
          return transactionQueue.size() < queueSize;
          }
        );
      }
    }

    for(auto& thread : threads) {
      thread.join();
    }

    return invalidTxNrs;
  }
}