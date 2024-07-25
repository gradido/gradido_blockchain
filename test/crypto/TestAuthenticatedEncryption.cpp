#include "TestAuthenticatedEncryption.h"

#include "gradido_blockchain/crypto/SecretKeyCryptography.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"

void TestAuthenticatedEncryption::SetUp()
{
	CryptoConfig::g_CryptoAppSecret = std::make_shared<memory::Block>(memory::Block::fromHex("21ffbbc616fe"));
	CryptoConfig::g_ServerCryptoKey = std::make_shared<memory::Block>(memory::Block::fromHex("a51ef8ac7ef1abf162fb7a65261acd7a"));
}

void TestAuthenticatedEncryption::TearDown()
{
}

TEST_F(TestAuthenticatedEncryption, encryptDecryptTest) {
	SecretKeyCryptography authenticated_encryption;
	EXPECT_FALSE(authenticated_encryption.hasKey());
	EXPECT_EQ(authenticated_encryption.getKeyHashed(), 0);

	EXPECT_NO_THROW(authenticated_encryption.createKey("max.musterman@gmail.com", "r3an7d_spassw"));
	//printf("create key duration: %s\n", time_used.string().data());

	EXPECT_TRUE(authenticated_encryption.hasKey());

	std::string test_message = "Dies ist eine Test Message zur Encryption";
	memory::Block test_message_bin(test_message.size());
	memcpy(test_message_bin, test_message.data(), test_message.size());

	auto encrypted_message = authenticated_encryption.encrypt(test_message_bin);
	EXPECT_TRUE(encrypted_message);
	//printf("encrypt message duration: %s\n", time_used.string().data());
	
	auto decrypted_message = authenticated_encryption.decrypt(encrypted_message);
	EXPECT_TRUE(decrypted_message);
	//printf("decrypt message duration: %s\n", time_used.string().data());

	EXPECT_EQ(decrypted_message.copyAsString(), test_message);
//	*/
}
