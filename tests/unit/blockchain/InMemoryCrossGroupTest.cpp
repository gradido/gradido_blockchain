#include "../interaction/const.h"
#include "InMemoryCrossGroupTest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/blockchain/InMemory.h"
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

using gradido::blockchain::InMemory;
using gradido::blockchain::InMemoryProvider;
using gradido::data::EncryptedMemo, gradido::data::LedgerAnchor;
using gradido::GradidoTransactionBuilder;

#define VERSION_STRING "3.5"
static EncryptedMemo memo("dummy memo");
const hiero::AccountId hieroAccount(0, 0, 121);

void InMemoryCrossGroupTest::SetUp()
{
	InMemoryTest::SetUp();
	mCommunityId2 = "SecondCommunity";
	auto createdAt = generateNewCreatedAt();
	mBlockchain = InMemoryProvider::getInstance()->findBlockchain(mCommunityId);

	GradidoTransactionBuilder builder;
	builder
		.setCreatedAt(mLastCreatedAt)
		.setVersionNumber(VERSION_STRING)
		.setCommunityRoot(
			g_KeyPairs[0]->getPublicKey(),
			g_KeyPairs[1]->getPublicKey(),
			g_KeyPairs[2]->getPublicKey()
		)
		.sign(g_KeyPairs[0])
		;
	mBlockchain->createAndAddConfirmedTransaction(builder.build(), LedgerAnchor({ mLastCreatedAt, hieroAccount }), mLastCreatedAt);
}

void InMemoryCrossGroupTest::TearDown()
{
	InMemoryTest::TearDown();
}

