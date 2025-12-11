#include "gradido_blockchain/data/hiero/AccountId.h"
#include "gtest/gtest.h"

using namespace hiero;

TEST(HieroAccountIdTest, Constructor) 
{
	auto accountId = AccountId(0, 0, 2);
	EXPECT_EQ(accountId.getShardNum(), 0);
	EXPECT_EQ(accountId.getRealmNum(), 0);
	EXPECT_EQ(accountId.getAccountNum(), 2);
	EXPECT_TRUE(accountId.getAlias().isEmpty());

	accountId = AccountId(12711, -21782, 19231);
	EXPECT_EQ(accountId.getShardNum(), 12711);
	EXPECT_EQ(accountId.getRealmNum(), -21782);
	EXPECT_EQ(accountId.getAccountNum(), 19231);
	EXPECT_TRUE(accountId.getAlias().isEmpty());

	// move
	auto alias = memory::Block("testalias");
	accountId = AccountId(0, 0, std::move(alias));
	EXPECT_FALSE(accountId.getAccountNum());
	EXPECT_EQ(accountId.getAlias().copyAsString(), "testalias");
	EXPECT_FALSE(alias.data());

	// copy
	alias = memory::Block("testalias");
	accountId = AccountId(0, 0, alias);
	EXPECT_FALSE(accountId.getAccountNum());
	EXPECT_EQ(accountId.getAlias().copyAsString(), "testalias");
	EXPECT_TRUE(alias.data());
	EXPECT_FALSE(alias.data() == accountId.getAlias().data());
}

TEST(HieroAccountIdTest, toString)
{
	auto accountId = AccountId(0, 0, 12121);
	EXPECT_EQ(accountId.toString(), "0.0.12121");

	accountId = AccountId(0, -2, 12132);
	EXPECT_EQ(accountId.toString(), "0.-2.12132");
}


