// Copyright 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cstdio>
#include "gtest/gtest.h"
#include "gradido_blockchain/AppContext.h"
#include "KeyPairs.h"
#include "gradido_blockchain/crypto/CryptoConfig.h"
#include "gradido_blockchain/lib/Dictionary.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/version.h"

#include <memory>
#include <string>

#if GTEST_OS_ESP8266 || GTEST_OS_ESP32
#if GTEST_OS_ESP8266
extern "C" {
#endif
	void setup() {
	  generateKeyPairs();
		testing::InitGoogleTest();
	}

	void loop() { RUN_ALL_TESTS(); }

#if GTEST_OS_ESP8266
}
#endif

#else

using gradido::AppContext, gradido::g_appContext;
using std::string;
using std::make_unique;
using memory::ConstBlockPtr, memory::ConstBlockPtrHash; memory::ConstBlockPtrEqual;

// GTEST_API_ 
int main(int argc, char** argv) {
	g_appContext = make_unique<AppContext>(make_unique<ThreadsafeRuntimeDictionary<std::string>>("communityIdDictionary"));
	try {
		g_appContext->addCommunityId("test-community");
	}
	catch (std::exception& ex) {
		printf("ex: %s\n", ex.what());
	}

	testing::InitGoogleTest(&argc, argv);
	generateKeyPairs();
	CryptoConfig::g_ServerCryptoKey = std::make_shared<memory::Block>(memory::Block::fromHex("153afcd54ef316e45cd3e5ed4567cd21", 32));
	//printf("Running main() from %s\n", __FILE__);
	//
	
	printf("Gradido Blockchain Version: %s\n", GRADIDO_BLOCKCHAIN_VERSION);
	auto result = RUN_ALL_TESTS();
	return result;
}

#endif

