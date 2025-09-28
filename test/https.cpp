#ifdef USE_HTTPS
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "gtest/gtest.h"

#include "gradido_blockchain/http/HttpRequest.h"
#include "gradido_blockchain/http/RequestExceptions.h"

TEST(HTTPS, HTTPS_ENABLED) {
    // httplib::SSLClient cli("google.com", 443);
    httplib::SSLClient cli("testnet.mirrornode.hedera.com", 443);
    
#ifdef CACERT_PEM_PATH
    const char* cacert = CACERT_PEM_PATH;
    cli.set_ca_cert_path(cacert);
    printf("Using CA certs from: %s\n", cacert);
#endif

    cli.set_read_timeout(5);  
    cli.set_write_timeout(5);

    auto res = cli.Get("api/v1/topics/0.0.6892589/messages?limit=25&order=asc");
    ASSERT_TRUE(res) << "HTTPS request failed: " << httplib::to_string(res.error());
    printf("response body: %s\n", res->body.data());
}

TEST(HTTPS, HttpRequest)
{
    // httplib::SSLClient cli("google.com", 443);
    HttpRequest request("https://testnet.mirrornode.hedera.com");
    try {
        auto res = request.GET("/api/v1/topics/0.0.6892589/messages?limit=25&order=asc");
        printf("response body: %s\n", res.data());
        ASSERT_FALSE(res.empty()) << "HTTPS request failed: " << res;
    } catch (const HttplibRequestException& e) {
        // FAIL() << "Exception during HTTPS request: " << e.getFullString();
        ASSERT_EQ(e.getStatus(), 301);
    }
}

#endif // USE_HTTPS