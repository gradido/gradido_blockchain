#include "gtest/gtest.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "../KeyPairs.h"
#include "const.h"

using namespace gradido;
using namespace data;
using namespace interaction;

TEST(ValidateTest, TransactionBody)
{
	TransactionBody body("memo", createdAt, VERSION_STRING);
	validate::Context c(body);
	EXPECT_THROW(c.run(), validate::TransactionValidationException);
}

/*
TEST(SerializeTest, RegisterAddressBody) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.registerAddress = make_shared<RegisterAddress>(
		AddressType::COMMUNITY_HUMAN,
		1,
		g_KeyPairs[3].publicKey,
		nullptr,
		g_KeyPairs[4].publicKey
	);
	ASSERT_TRUE(body.isRegisterAddress());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	ASSERT_EQ(serialized->convertToBase64(),
		"CgASCAiAzLn/BRAAGgMzLjMgAEpICiAllxqg50IhRNzCRIh+Ke8WDVR5sSGemBfKbs44sJ83wBABIiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VygB"
	);

}

TEST(SerializeTest, GradidoCreationBody) {
	TransactionBody body("Deine erste Schoepfung ;)", createdAt, VERSION_STRING);
	body.creation = make_shared<GradidoCreation>(
		TransferAmount(g_KeyPairs[4].publicKey, "1000.00"),
		TimestampSeconds(1609459000)
	);
	ASSERT_TRUE(body.isCreation());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(),
		"ChlEZWluZSBlcnN0ZSBTY2hvZXBmdW5nIDspEggIgMy5/wUQABoDMy4zIAA6NAoqCiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VxIEMTAwMBoAGgYIuMq5/wU="
	);

}

TEST(SerializeTest, GradidoTransferBody) {
	TransactionBody body("Ich teile mit dir", createdAt, VERSION_STRING);
	body.transfer = make_shared<GradidoTransfer>(
		TransferAmount(g_KeyPairs[4].publicKey, "500.55"),
		g_KeyPairs[5].publicKey
	);
	ASSERT_TRUE(body.isTransfer());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(),
		"ChFJY2ggdGVpbGUgbWl0IGRpchIICIDMuf8FEAAaAzMuMyAAMlAKLAogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSBjUwMC41NRoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7Fdg=="
	);
}

TEST(SerializeTest, GradidoDeferredTransferBody) {
	TransactionBody body("Link zum einloesen", createdAt, VERSION_STRING);
	body.deferredTransfer = make_shared<GradidoDeferredTransfer>(
		GradidoTransfer(
			TransferAmount(g_KeyPairs[4].publicKey, "555.55"),
			g_KeyPairs[5].publicKey
		), TimestampSeconds(1609465000)
	);
	ASSERT_TRUE(body.isDeferredTransfer());
	serialize::Context c(body);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());

	ASSERT_EQ(serialized->convertToBase64(),
		"ChJMaW5rIHp1bSBlaW5sb2VzZW4SCAiAzLn/BRAAGgMzLjMgAFJaClAKLAogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSBjU1NS41NRoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7FdhIGCKj5uf8F"
	);
}

TEST(SerializeTest, CommunityFriendsUpdateBody) {
	TransactionBody body("", createdAt, VERSION_STRING);
	body.communityFriendsUpdate = make_shared<CommunityFriendsUpdate>(true);
	ASSERT_TRUE(body.isCommunityFriendsUpdate());
	serialize::Context c(body);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(),
		"CgASCAiAzLn/BRAAGgMzLjMgAEICCAE="
	);
}

TEST(SerializeTest, GradidoTransaction) {
	GradidoTransaction transaction;
	auto bodyBytes = make_shared<memory::Block>(
		"''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson "
	);
	auto sign = make_shared<memory::Block>(crypto_sign_BYTES);
	auto& keyPair = g_KeyPairs[3];
	unsigned long long actualSignLength = 0;
	crypto_sign_detached(*sign, &actualSignLength, *bodyBytes, bodyBytes->size(), *keyPair.privateKey);
	ASSERT_EQ(actualSignLength, crypto_sign_BYTES);
	//printf("signature: %s\n", sign->convertToHex().data());
	transaction.signatureMap.push({ keyPair.publicKey, sign });
	transaction.bodyBytes = bodyBytes;

	serialize::Context c(transaction);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(),
		"CmYKZAogJZcaoOdCIUTcwkSIfinvFg1UebEhnpgXym7OOLCfN8ASQNnYDRZmlKkhvEiea/cRiqwr2bExLlkQxcbwjDYXy3nC3Ggin7Jous8li9gLfQpnUX9FzhrbqOiMEvQqVisNHQUSigEnJ1RvIGJlIHlvdXJzZWxmIGluIGEgd29ybGQgdGhhdCBpcyBjb25zdGFudGx5IHRyeWluZyB0byBtYWtlIHlvdSBzb21ldGhpbmcgZWxzZSBpcyB0aGUgZ3JlYXRlc3QgYWNjb21wbGlzaG1lbnQuJycKIC0gUmFscGggV2FsZG8gRW1lcnNvbiA="
	);
}

TEST(SerializeTest, SignatureMap) {
	memory::Block message(
		"Human nature is a complex interplay of light and shadow, where our greatest strengths often emerge from our deepest vulnerabilities. To be human is to strive for connection, seek meaning, and continuously evolve through both triumphs and trials."
	);
	auto sign = make_shared<memory::Block>(64);
	unsigned long long actualSignLength = 0;
	SignatureMap signatureMap;
	for (int i = 0; i < 2; i++) {
		crypto_sign_detached(*sign, &actualSignLength, message, message.size(), *g_KeyPairs[i].privateKey);
		signatureMap.push({ g_KeyPairs[i].publicKey, g_KeyPairs[i].privateKey });
	}

	serialize::Context c(signatureMap);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(),
		"CmQKIGQ8Q4d2/CY0+viH34SFue1YBynCCZ4A5NTVPNdGJqDWEkAVfmpfqv7OHLnp5DqPU7RQngLBjAal/jGbcwsNtPh022Q8Q4d2/CY0+viH34SFue1YBynCCZ4A5NTVPNdGJqDWCmQKIFH5sejZhHY63U2QzGQi8f1KCcZ3uY5LGYwFW8KUIPWOEkDC/SCM7JsboMSTRqH1iIFiX0RaMV/YuMUtk1q+KyoReVH5sejZhHY63U2QzGQi8f1KCcZ3uY5LGYwFW8KUIPWO"
	);
}

TEST(SerializeTest, MinimalConfirmedTransaction) {

	ConfirmedTransaction confirmedTransaction(
		7,
		std::make_unique<GradidoTransaction>(),
		confirmedAt,
		VERSION_STRING,
		make_shared<memory::Block>(crypto_generichash_BYTES),
		make_shared<memory::Block>(32),
		"179.00"
	);
	serialize::Context c(confirmedTransaction);
	auto serialized = c.run();
	//printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	//printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(),
		"CAcSAgoAGgYIwvK5/wUiAzMuMyogAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOgMxNzk="
	);
}

TEST(SerializeTest, CompleteConfirmedTransaction) {
	std::string memo("Danke fuer dein Sein!");
	TransactionBodyBuilder bodyBuilder;
	auto transactionBody = bodyBuilder
		.setTransactionTransfer(
			g_KeyPairs[4].publicKey, // sender
			"100.251621",
			"",
			g_KeyPairs[5].publicKey // recipient
		)
		.setCreatedAt(createdAt)
		.setMemo(memo)
		.build();

	GradidoTransactionBuilder builder;
	auto keyPair = make_shared<KeyPairEd25519>(g_KeyPairs[0].publicKey, g_KeyPairs[0].privateKey);
	auto gradidoTransaction = builder
		.setTransactionBody(std::move(transactionBody))
		.sign(keyPair)
		.build();

	ConfirmedTransaction confirmedTransaction(
		7,
		std::move(gradidoTransaction),
		confirmedAt,
		VERSION_STRING,
		nullptr,
		make_shared<memory::Block>(32),
		"899.748379"
	);
	confirmedTransaction.runningHash = std::make_shared<memory::Block>(confirmedTransaction.calculateRunningHash());
	serialize::Context c(confirmedTransaction);
	auto serialized = c.run();
	// printf("serialized size: %d, serialized in base64: %s\n", serialized->size(), DataTypeConverter::binToBase64(*serialized).data());
	// printf("hex: %s\n", serialized->convertToHex().data());
	ASSERT_EQ(serialized->convertToBase64(),
		"CAcS5gEKZgpkCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hJABLGV+LKPuwm777Y98sSKCJjyT764T0LkK9JMAlH8McVfE4a58QJDRkRVVw8s+97770Ecu41wN699/2dl2bMyCRJ8ChVEYW5rZSBmdWVyIGRlaW4gU2VpbiESCAiAzLn/BRAAGgMzLjMgADJSCi4KIIqMkyk8uX6HhBeNqK5YgUT3yYL0ZYv9NRAaHitHnD5XEggxMDAuMjUxNhoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7FdhoGCMLyuf8FIgMzLjMqIIECZhgxQyc+ARHQ/855AyzGFnuR8jjwBqgBYRWxbkANMiAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADoIODk5Ljc0ODQ="
	);
}
*/