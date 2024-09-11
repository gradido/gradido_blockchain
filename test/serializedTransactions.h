#ifndef __GRADIDO_BLOCKCHAIN_TEST_INTERACTION_SERIALIZED_TRANSACTIONS_H
#define __GRADIDO_BLOCKCHAIN_TEST_INTERACTION_SERIALIZED_TRANSACTIONS_H

// contain only createAt and version string
constexpr auto emptyTransactionBodyBase64 = "CgASCAiAzLn/BRAAGgMzLjMgAA==";
// contain only createAt, version string and memo
constexpr auto emptyTransactionBodyMemoBase64 =  "CgRtZW1vEggIgMy5/wUQABoDMy4zIAA=";

constexpr auto communityRootTransactionBase64 =
"CgASCAiAzLn/BRAAGgMzLjMgAFpmCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hIgUfmx6NmEdjrdTZDMZCLx/UoJxne5jksZjAVbwpQg9Y4aILuZSh1i57PKe586fpKo7WESpT92xJWVtRfMdFClC3pa";
constexpr auto registeAddressTransactionBase64 =
"CgASCAiAzLn/BRAAGgMzLjMgAEpICiAllxqg50IhRNzCRIh+Ke8WDVR5sSGemBfKbs44sJ83wBABIiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VygB";
constexpr auto creationTransactionBase64 =
"ChlEZWluZSBlcnN0ZSBTY2hvZXBmdW5nIDspEggIgMy5/wUQABoDMy4zIAA6NAoqCiCKjJMpPLl+h4QXjaiuWIFE98mC9GWL/TUQGh4rR5w+VxIEMTAwMBoAGgYIuMq5/wU=";
constexpr auto transferTransactionBase64 =
"ChFJY2ggdGVpbGUgbWl0IGRpchIICIDMuf8FEAAaAzMuMyAAMlAKLAogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSBjUwMC41NRoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7Fdg==";
constexpr auto deferredTransferTransactionBase64 =
"ChJMaW5rIHp1bSBlaW5sb2VzZW4SCAiAzLn/BRAAGgMzLjMgAFJaClAKLAogioyTKTy5foeEF42orliBRPfJgvRli/01EBoeK0ecPlcSBjU1NS41NRoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7FdhIGCKj5uf8F";
constexpr auto communityFriendsUpdateBase64 = "CgASCAiAzLn/BRAAGgMzLjMgAEICCAE=";

constexpr auto invalidBodyTestPayload = "''To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.''\n - Ralph Waldo Emerson ";
// contain invalidBodyTestPayload as body bytes
// signed with g_KeyPairs[3]
constexpr auto gradidoTransactionSignedInvalidBody = 
"CmYKZAogJZcaoOdCIUTcwkSIfinvFg1UebEhnpgXym7OOLCfN8ASQNnYDRZmlKkhvEiea/cRiqwr2bExLlkQxcbwjDYXy3nC3Ggin7Jous8li9gLfQpnUX9FzhrbqOiMEvQqVisNHQUSigEnJ1RvIGJlIHlvdXJzZWxmIGluIGEgd29ybGQgdGhhdCBpcyBjb25zdGFudGx5IHRyeWluZyB0byBtYWtlIHlvdSBzb21ldGhpbmcgZWxzZSBpcyB0aGUgZ3JlYXRlc3QgYWNjb21wbGlzaG1lbnQuJycKIC0gUmFscGggV2FsZG8gRW1lcnNvbiA=";
// not containe a gradido transaction, is only a empty wrapper for tests
constexpr auto minimalConfirmedTransaction = "CAcSAgoAGgYIwvK5/wUiAzMuMyogAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOgMxNzk=";
// contain a transfer transaction
constexpr auto completeConfirmedTransaction = 
"CAcS5gEKZgpkCiBkPEOHdvwmNPr4h9+EhbntWAcpwgmeAOTU1TzXRiag1hJABLGV+LKPuwm777Y98sSKCJjyT764T0LkK9JMAlH8McVfE4a58QJDRkRVVw8s+97770Ecu41wN699/2dl2bMyCRJ8ChVEYW5rZSBmdWVyIGRlaW4gU2VpbiESCAiAzLn/BRAAGgMzLjMgADJSCi4KIIqMkyk8uX6HhBeNqK5YgUT3yYL0ZYv9NRAaHitHnD5XEggxMDAuMjUxNhoAEiDRqVgkyEhZACebkqYBdfxnb4kUxh1zmcZsLQy2+p7FdhoGCMLyuf8FIgMzLjMqIIgnAMXVrMc4Gp2khh7bkK27q8fWQoaf3VcrNF5WZdhaMiAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADoIODk5Ljc0ODQ=";
#endif //__GRADIDO_BLOCKCHAIN_TEST_INTERACTION_SERIALIZED_TRANSACTIONS_H
