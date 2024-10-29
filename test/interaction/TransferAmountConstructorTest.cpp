#include "gtest/gtest.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

using namespace gradido;
using namespace data;

TEST(TransferAmountConstructorTest, NullptrPublicKey)
{
	EXPECT_THROW(
		TransferAmount(nullptr, "1000.00"),
		GradidoNullPointerException
	);
}

TEST(TransferAmountConstructorTest, EmptyPublicKey)
{
	EXPECT_THROW(
		TransferAmount(std::make_shared<memory::Block>(32), "1000.00"), 
		GradidoNodeInvalidDataException
	);
}

TEST(TransferAmountConstructorTest, InvalidPublicKey)
{
	EXPECT_THROW(
		TransferAmount(std::make_shared<memory::Block>(memory::Block::fromHex("9a3b4c5d6e7f8c9b0a", 18)), "1000.00"),
		Ed25519InvalidKeyException
	);
}