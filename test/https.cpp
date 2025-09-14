#ifdef USE_HTTPS
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "gtest/gtest.h"

TEST(HTTPS, HTTPS_ENABLED) {
    const char* cacert = CACERT_PEM_PATH;

    httplib::SSLClient cli("google.com", 443);
    cli.set_ca_cert_path(cacert);
    cli.set_read_timeout(5);  
    cli.set_write_timeout(5);

    auto res = cli.Get("/");
    ASSERT_TRUE(res) << "HTTPS request failed: " << (res ? res->status : -1);
}

#endif // USE_HTTPS