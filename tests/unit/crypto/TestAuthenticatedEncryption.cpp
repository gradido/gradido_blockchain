#include "TestAuthenticatedEncryption.h"

#include "gradido_blockchain/crypto/SecretKeyCryptography.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"

void TestAuthenticatedEncryption::SetUp()
{
	CryptoConfig::g_CryptoAppSecret = std::make_shared<memory::Block>(
		memory::Block::fromHex("21ffbbc616fe", 12)
	);
	CryptoConfig::g_ServerCryptoKey = std::make_shared<memory::Block>(
		memory::Block::fromHex("a51ef8ac7ef1abf162fb7a65261acd7a", 32)
	);
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
	EXPECT_EQ(
		encrypted_message.convertToHex(), 
		"46844ac9906132f8c40d8165e6d426a84b7e507175b327ba8bea9789470e1b61501c3b29fe62071ed76a97f433b70e69ffccded2f089c6ccc2"
	);
	
	auto decrypted_message = authenticated_encryption.decrypt(encrypted_message);
	EXPECT_TRUE(decrypted_message);
	//printf("decrypt message duration: %s\n", time_used.string().data());

	EXPECT_EQ(decrypted_message.copyAsString(), test_message);
//	*/
}
