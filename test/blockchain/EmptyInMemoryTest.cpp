#include "EmptyInMemoryTest.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"

using namespace std; 
using namespace gradido;
using namespace data;
using namespace blockchain;
using namespace interaction;

void EmptyInMemoryTest::SetUp()
{

}

void EmptyInMemoryTest::TearDown()
{
	InMemoryProvider::getInstance()->clear();
}


TEST_F(EmptyInMemoryTest, AddCommunityRootAsFirst) {
	std::shared_ptr<InMemory> blockchain = dynamic_pointer_cast<InMemory>(
		InMemoryProvider::getInstance()->findBlockchain("testCommunity")
	);
	auto transaction = make_shared<GradidoTransaction>();
	// keyPair 0 is public of this community root Transaction
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	sign(*transaction, g_KeyPairs[0]);
	
	EXPECT_TRUE(blockchain->addGradidoTransaction(transaction));
}

TEST_F(EmptyInMemoryTest, InvalidMissingSignature) {
	std::shared_ptr<InMemory> blockchain = dynamic_pointer_cast<InMemory>(
		InMemoryProvider::getInstance()->findBlockchain("testCommunity")
	);
	auto transaction = make_shared<GradidoTransaction>();
	// keyPair 0 is public of this community root Transaction
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa"
	));
	//sign(*transaction, g_KeyPairs[0]);

	EXPECT_THROW(blockchain->addGradidoTransaction(transaction), validate::TransactionValidationMissingSignException);
}

TEST_F(EmptyInMemoryTest, InvalidRegisterAddressAsFirst) {
	std::shared_ptr<InMemory> blockchain = dynamic_pointer_cast<InMemory>(
		InMemoryProvider::getInstance()->findBlockchain("testCommunity")
	);
	auto transaction = make_shared<GradidoTransaction>();
	
	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"CgASCAiAzLn/BRAAGgMzLjMgAEpICiAllxqg50IhRNzCRIh+Ke8WDVR5sSGemBfKbs44sJ83wBABIiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VygB"
	));
	sign(*transaction, g_KeyPairs[0]);
	sign(*transaction, g_KeyPairs[4]);

	EXPECT_THROW(blockchain->addGradidoTransaction(transaction), BlockchainOrderException);
}
TEST_F(EmptyInMemoryTest, InvalidGradidoCreationAsFirst) {
	std::shared_ptr<InMemory> blockchain = dynamic_pointer_cast<InMemory>(
		InMemoryProvider::getInstance()->findBlockchain("testCommunity")
	);
	auto transaction = make_shared<GradidoTransaction>();

	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"ChlEZWluZSBlcnN0ZSBTY2hvZXBmdW5nIDspEggIgMy5/wUQABoDMy4zIAA6NAoqCiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VxIEMTAwMBoAGgYIuMq5/wU="
	));
	sign(*transaction, g_KeyPairs[3]);

	EXPECT_THROW(blockchain->addGradidoTransaction(transaction), validate::WrongAddressTypeException);
}

TEST_F(EmptyInMemoryTest, InvalidGradidoTransferAsFirst) {
	std::shared_ptr<InMemory> blockchain = dynamic_pointer_cast<InMemory>(
		InMemoryProvider::getInstance()->findBlockchain("testCommunity")
	);
	auto transaction = make_shared<GradidoTransaction>();

	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"ChFJY2ggdGVpbGUgbWl0IGRpchIICIDMuf8FEAAaAzMuMyAAMlAKLAogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSBjUwMC41NRoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7Fdg=="
	));
	sign(*transaction, g_KeyPairs[4]);

	EXPECT_THROW(blockchain->addGradidoTransaction(transaction), InvalidGradidoTransaction);
}


TEST_F(EmptyInMemoryTest, InvalidGradidoDeferredTransferAsFirst) {
	std::shared_ptr<InMemory> blockchain = dynamic_pointer_cast<InMemory>(
		InMemoryProvider::getInstance()->findBlockchain("testCommunity")
	);
	auto transaction = make_shared<GradidoTransaction>();

	transaction->bodyBytes = make_shared<memory::Block>(memory::Block::fromBase64(
		"ChJMaW5rIHp1bSBlaW5sb2VzZW4SCAiAzLn/BRAAGgMzLjMgAFJaClAKLAogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSBjU1NS41NRoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7FdhIGCKj5uf8F"
	));
	sign(*transaction, g_KeyPairs[4]);

	EXPECT_THROW(blockchain->addGradidoTransaction(transaction), InvalidGradidoTransaction);
}