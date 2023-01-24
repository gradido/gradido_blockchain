#include "TestAuthenticatedEncryption.h"

#include "gradido_blockchain/crypto/SecretKeyCryptography.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"

void TestAuthenticatedEncryption::SetUp()
{
	CryptoConfig::g_CryptoAppSecret = DataTypeConverter::hexToBin("21ffbbc616fe");
	CryptoConfig::g_ServerCryptoKey = DataTypeConverter::hexToBin("a51ef8ac7ef1abf162fb7a65261acd7a");
}

void TestAuthenticatedEncryption::TearDown()
{
	auto mm = MemoryManager::getInstance();
	mm->releaseMemory(CryptoConfig::g_CryptoAppSecret);
	mm->releaseMemory(CryptoConfig::g_ServerCryptoKey);
}

TEST_F(TestAuthenticatedEncryption, encryptDecryptTest) {
	SecretKeyCryptography authenticated_encryption;
	EXPECT_FALSE(authenticated_encryption.hasKey());
	EXPECT_EQ(authenticated_encryption.getKeyHashed(), 0);

	EXPECT_EQ(authenticated_encryption.createKey("max.musterman@gmail.com", "r3an7d_spassw"), SecretKeyCryptography::AUTH_CREATE_ENCRYPTION_KEY_SUCCEED);
	//printf("create key duration: %s\n", time_used.string().data());

	EXPECT_TRUE(authenticated_encryption.hasKey());

	std::string test_message = "Dies ist eine Test Message zur Encryption";
	auto mm = MemoryManager::getInstance();
	auto test_message_bin = mm->getMemory(test_message.size());
	MemoryBin* encrypted_message = nullptr;
	memcpy(*test_message_bin, test_message.data(), test_message.size());

	EXPECT_EQ(authenticated_encryption.encrypt(test_message_bin, &encrypted_message), SecretKeyCryptography::AUTH_ENCRYPT_OK);
	//printf("encrypt message duration: %s\n", time_used.string().data());

	MemoryBin* decrypted_message = nullptr;
	EXPECT_EQ(authenticated_encryption.decrypt(encrypted_message, &decrypted_message), SecretKeyCryptography::AUTH_DECRYPT_OK);
	//printf("decrypt message duration: %s\n", time_used.string().data());

	EXPECT_EQ(std::string((const char*)*decrypted_message, decrypted_message->size()), test_message);
	mm->releaseMemory(decrypted_message);
//	*/
}
