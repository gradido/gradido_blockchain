<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="class">
    <name>model::gradido::AddressAlreadyExistException</name>
    <filename>classmodel_1_1gradido_1_1_address_already_exist_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="class">
    <name>AuthenticatedEncryption</name>
    <filename>class_authenticated_encryption.html</filename>
    <member kind="function">
      <type>int</type>
      <name>precalculateSharedSecret</name>
      <anchorfile>class_authenticated_encryption.html</anchorfile>
      <anchor>ab91b5dbdbebe8882780734d7da5e4bf4</anchor>
      <arglist>(AuthenticatedEncryption *recipiantKey)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BlockchainOrderException</name>
    <filename>class_blockchain_order_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>CrossGroupTransactionBuilder</name>
    <filename>class_cross_group_transaction_builder.html</filename>
  </compound>
  <compound kind="class">
    <name>SealedBoxes::DecryptException</name>
    <filename>class_sealed_boxes_1_1_decrypt_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::DeferredTransfer</name>
    <filename>classmodel_1_1gradido_1_1_deferred_transfer.html</filename>
    <base>model::gradido::TransactionTransfer</base>
  </compound>
  <compound kind="class">
    <name>DRHashList</name>
    <filename>class_d_r_hash_list.html</filename>
    <base>DRStaticHashList</base>
  </compound>
  <compound kind="class">
    <name>DRHashListItem</name>
    <filename>class_d_r_hash_list_item.html</filename>
  </compound>
  <compound kind="class">
    <name>DRMemoryList</name>
    <filename>class_d_r_memory_list.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>DRStaticHashList</name>
    <filename>class_d_r_static_hash_list.html</filename>
  </compound>
  <compound kind="class">
    <name>Ed25519SignException</name>
    <filename>class_ed25519_sign_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::GlobalGroupAdd</name>
    <filename>classmodel_1_1gradido_1_1_global_group_add.html</filename>
    <base>model::gradido::TransactionBase</base>
    <member kind="function">
      <type>int</type>
      <name>prepare</name>
      <anchorfile>classmodel_1_1gradido_1_1_global_group_add.html</anchorfile>
      <anchor>ae9f4823ab45013857d6330edca030944</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; MemoryBin * &gt;</type>
      <name>getInvolvedAddresses</name>
      <anchorfile>classmodel_1_1gradido_1_1_global_group_add.html</anchorfile>
      <anchor>a38d81728c7976bad17ba3b9635963806</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>model::gradido::GradidoBlock</name>
    <filename>classmodel_1_1gradido_1_1_gradido_block.html</filename>
    <base>MultithreadContainer</base>
    <member kind="function">
      <type>std::string</type>
      <name>toJson</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_block.html</anchorfile>
      <anchor>a806e098e7f19aaa52c8d71157c16d5db</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>validate</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_block.html</anchorfile>
      <anchor>a4d18042739e28221f8f725551d27d3de</anchor>
      <arglist>(TransactionValidationLevel level=TRANSACTION_VALIDATION_SINGLE, IGradidoBlockchain *blockchain=nullptr, IGradidoBlockchain *otherBlockchain=nullptr) const</arglist>
    </member>
    <member kind="function">
      <type>MemoryBin *</type>
      <name>getMessageId</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_block.html</anchorfile>
      <anchor>a0621762ae399a5d081417fc0d6be8951</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>MemoryBin *</type>
      <name>calculateTxHash</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_block.html</anchorfile>
      <anchor>a46071b5a981be66189b98b251a1677db</anchor>
      <arglist>(const GradidoBlock *previousBlock) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>calculateFinalGDD</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_block.html</anchorfile>
      <anchor>abe91beca2d46a82445be04a10016229c</anchor>
      <arglist>(IGradidoBlockchain *blockchain)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GradidoBlockchainException</name>
    <filename>class_gradido_blockchain_exception.html</filename>
  </compound>
  <compound kind="class">
    <name>GradidoBlockchainTransactionAlreadyExistException</name>
    <filename>class_gradido_blockchain_transaction_already_exist_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>GradidoBlockchainTransactionNotFoundException</name>
    <filename>class_gradido_blockchain_transaction_not_found_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>GradidoInvalidBase64Exception</name>
    <filename>class_gradido_invalid_base64_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>GradidoInvalidEnumException</name>
    <filename>class_gradido_invalid_enum_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::GradidoTransaction</name>
    <filename>classmodel_1_1gradido_1_1_gradido_transaction.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>validate</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_transaction.html</anchorfile>
      <anchor>a42823b6ea4481b7b084d318e7ae54d5a</anchor>
      <arglist>(TransactionValidationLevel level=TRANSACTION_VALIDATION_SINGLE, IGradidoBlockchain *blockchain=nullptr, const GradidoBlock *parentGradidoBlock=nullptr, IGradidoBlockchain *otherBlockchain=nullptr) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateBodyBytes</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_transaction.html</anchorfile>
      <anchor>ad378fef62c33651f1d5f93a98cf1bb08</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>MemoryBin *</type>
      <name>getParentMessageId</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_transaction.html</anchorfile>
      <anchor>acd8f6c9b697653f10ba274698f728020</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GradidoUnknownEnumException</name>
    <filename>class_gradido_unknown_enum_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>GraphQLRequest</name>
    <filename>class_graph_q_l_request.html</filename>
    <base>JsonRequest</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::GroupFriendsUpdate</name>
    <filename>classmodel_1_1gradido_1_1_group_friends_update.html</filename>
    <base>model::gradido::TransactionBase</base>
    <member kind="function">
      <type>std::vector&lt; MemoryBin * &gt;</type>
      <name>getInvolvedAddresses</name>
      <anchorfile>classmodel_1_1gradido_1_1_group_friends_update.html</anchorfile>
      <anchor>a24886711163d95e12b8e745629f9d2d1</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>HandleRequestException</name>
    <filename>class_handle_request_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="struct">
    <name>Mnemonic::HashCollisionWords</name>
    <filename>struct_mnemonic_1_1_hash_collision_words.html</filename>
  </compound>
  <compound kind="class">
    <name>HttpRequest</name>
    <filename>class_http_request.html</filename>
  </compound>
  <compound kind="class">
    <name>model::IGradidoBlockchain</name>
    <filename>classmodel_1_1_i_gradido_blockchain.html</filename>
  </compound>
  <compound kind="class">
    <name>model::gradido::InsufficientBalanceException</name>
    <filename>classmodel_1_1gradido_1_1_insufficient_balance_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="class">
    <name>InvalidCrossGroupTransaction</name>
    <filename>class_invalid_cross_group_transaction.html</filename>
    <base>InvalidTransactionTypeOnBlockchain</base>
  </compound>
  <compound kind="class">
    <name>DataTypeConverter::InvalidHexException</name>
    <filename>class_data_type_converter_1_1_invalid_hex_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>InvalidTransactionTypeOnBlockchain</name>
    <filename>class_invalid_transaction_type_on_blockchain.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>IotaPostRequestException</name>
    <filename>class_iota_post_request_exception.html</filename>
    <base>IotaRequestException</base>
  </compound>
  <compound kind="class">
    <name>IotaRequest</name>
    <filename>class_iota_request.html</filename>
    <base>JsonRequest</base>
    <member kind="function">
      <type>std::string</type>
      <name>sendMessage</name>
      <anchorfile>class_iota_request.html</anchorfile>
      <anchor>af4ab89bb1ce2fa816e38de61df78166c</anchor>
      <arglist>(const std::string &amp;indexHex, const std::string &amp;messageHex)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>getMessageMilestoneId</name>
      <anchorfile>class_iota_request.html</anchorfile>
      <anchor>a240c272a419ed75cee7fdb28ced057d9</anchor>
      <arglist>(const std::string &amp;messageIdHex)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; MemoryBin * &gt;</type>
      <name>findByIndex</name>
      <anchorfile>class_iota_request.html</anchorfile>
      <anchor>a6c806d2fc1a0cb113ef10e6797fcd408</anchor>
      <arglist>(const std::string &amp;index)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>defaultExceptionHandler</name>
      <anchorfile>class_iota_request.html</anchorfile>
      <anchor>a69413a8df5b8bc36ec31e7ae416b7ae3</anchor>
      <arglist>(Poco::Logger &amp;errorLog, bool terminate=true)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>IotaRequestException</name>
    <filename>class_iota_request_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>JsonCakePHPRequest</name>
    <filename>class_json_cake_p_h_p_request.html</filename>
    <base>JsonRequest</base>
  </compound>
  <compound kind="class">
    <name>JsonRequest</name>
    <filename>class_json_request.html</filename>
    <base>HttpRequest</base>
    <member kind="function">
      <type>rapidjson::Document</type>
      <name>postRequest</name>
      <anchorfile>class_json_request.html</anchorfile>
      <anchor>ae051214d475d100b954d88852209bcfa</anchor>
      <arglist>(rapidjson::Value &amp;payload)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>JsonRequestHandler</name>
    <filename>class_json_request_handler.html</filename>
  </compound>
  <compound kind="class">
    <name>JsonRequestHandlerJwt</name>
    <filename>class_json_request_handler_jwt.html</filename>
    <base>JsonRequestHandler</base>
  </compound>
  <compound kind="class">
    <name>JsonRPCException</name>
    <filename>class_json_r_p_c_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>JsonRPCRequest</name>
    <filename>class_json_r_p_c_request.html</filename>
    <base>JsonRequest</base>
  </compound>
  <compound kind="class">
    <name>JwtTokenException</name>
    <filename>class_jwt_token_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>KeyPairEd25519</name>
    <filename>class_key_pair_ed25519.html</filename>
    <member kind="function">
      <type></type>
      <name>KeyPairEd25519</name>
      <anchorfile>class_key_pair_ed25519.html</anchorfile>
      <anchor>a813233e90602282ae943a40de3e16d01</anchor>
      <arglist>(MemoryBin *privateKey)</arglist>
    </member>
    <member kind="function">
      <type>MemoryBin *</type>
      <name>sign</name>
      <anchorfile>class_key_pair_ed25519.html</anchorfile>
      <anchor>a2ca30219eb03bf1f534c857e1318e9af</anchor>
      <arglist>(const MemoryBin *message) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>verify</name>
      <anchorfile>class_key_pair_ed25519.html</anchorfile>
      <anchor>a507ecc0cc31ddb6b628ccef5f24994e7</anchor>
      <arglist>(const std::string &amp;message, const std::string &amp;signature) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>isTheSame</name>
      <anchorfile>class_key_pair_ed25519.html</anchorfile>
      <anchor>a48e7c3f19ee5eb357be846c16a1dcb0a</anchor>
      <arglist>(const MemoryBin *privkey) const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::unique_ptr&lt; KeyPairEd25519 &gt;</type>
      <name>create</name>
      <anchorfile>class_key_pair_ed25519.html</anchorfile>
      <anchor>adbf559ed7546131b77c43be2ec01b43b</anchor>
      <arglist>(const std::shared_ptr&lt; Passphrase &gt; passphrase)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MathMemory</name>
    <filename>class_math_memory.html</filename>
  </compound>
  <compound kind="class">
    <name>MemoryBin</name>
    <filename>class_memory_bin.html</filename>
    <member kind="function">
      <type>int</type>
      <name>convertFromHex</name>
      <anchorfile>class_memory_bin.html</anchorfile>
      <anchor>a3c0780b439bd9fd1a97bdafb62834b83</anchor>
      <arglist>(const std::string &amp;hex)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MemoryManager</name>
    <filename>class_memory_manager.html</filename>
  </compound>
  <compound kind="class">
    <name>MemoryManagerException</name>
    <filename>class_memory_manager_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>MemoryPageStack</name>
    <filename>class_memory_page_stack.html</filename>
    <base protection="protected">MultithreadContainer</base>
  </compound>
  <compound kind="class">
    <name>CryptoConfig::MissingKeyException</name>
    <filename>class_crypto_config_1_1_missing_key_exception.html</filename>
    <base protection="private">GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>Mnemonic</name>
    <filename>class_mnemonic.html</filename>
    <class kind="struct">Mnemonic::HashCollisionWords</class>
  </compound>
  <compound kind="class">
    <name>MnemonicException</name>
    <filename>class_mnemonic_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>MultithreadContainer</name>
    <filename>class_multithread_container.html</filename>
  </compound>
  <compound kind="class">
    <name>MultithreadContainerLockTimeoutException</name>
    <filename>class_multithread_container_lock_timeout_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>MultithreadQueue</name>
    <filename>class_multithread_queue.html</filename>
    <templarg></templarg>
    <base protection="protected">MultithreadContainer</base>
    <member kind="function">
      <type>bool</type>
      <name>pop</name>
      <anchorfile>class_multithread_queue.html</anchorfile>
      <anchor>a07fd863668fc6c0547c87f34cf138750</anchor>
      <arglist>(ResourceType &amp;val)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>iota::NodeInfo</name>
    <filename>structiota_1_1_node_info.html</filename>
  </compound>
  <compound kind="class">
    <name>model::gradido::PairingTransactionNotMatchException</name>
    <filename>classmodel_1_1gradido_1_1_pairing_transaction_not_match_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="class">
    <name>Passphrase</name>
    <filename>class_passphrase.html</filename>
    <member kind="function">
      <type>std::shared_ptr&lt; Passphrase &gt;</type>
      <name>transform</name>
      <anchorfile>class_passphrase.html</anchorfile>
      <anchor>af895aeeb6ec48b27b5ce1edea3d41142</anchor>
      <arglist>(const Mnemonic *targetWordSource)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>createClearPassphrase</name>
      <anchorfile>class_passphrase.html</anchorfile>
      <anchor>a86a1450283f05cab7649b9a83a5c4ffe</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>checkIfValid</name>
      <anchorfile>class_passphrase.html</anchorfile>
      <anchor>a96b7a9b39ebcc1288009df8861ccfd0f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>createWordIndices</name>
      <anchorfile>class_passphrase.html</anchorfile>
      <anchor>a83111775e556b675031bc083f37f710f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>getString</name>
      <anchorfile>class_passphrase.html</anchorfile>
      <anchor>ae6631f4eaf0e0a980c2fc1aeb4bd7abf</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::shared_ptr&lt; Passphrase &gt;</type>
      <name>generate</name>
      <anchorfile>class_passphrase.html</anchorfile>
      <anchor>a2bb5c291e84a5be867fd64eada4402b3</anchor>
      <arglist>(const Mnemonic *wordSource)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::string</type>
      <name>filter</name>
      <anchorfile>class_passphrase.html</anchorfile>
      <anchor>a19ce9534fe8b6d4c4d9e767e5ecca073</anchor>
      <arglist>(const std::string &amp;passphrase)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PassphraseEmptyWordSourceException</name>
    <filename>class_passphrase_empty_word_source_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>PocoNetException</name>
    <filename>class_poco_net_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>Profiler</name>
    <filename>class_profiler.html</filename>
  </compound>
  <compound kind="class">
    <name>ProtobufJsonSerializationException</name>
    <filename>class_protobuf_json_serialization_exception.html</filename>
    <base>ProtobufSerializationException</base>
  </compound>
  <compound kind="class">
    <name>ProtobufParseException</name>
    <filename>class_protobuf_parse_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>ProtobufSerializationException</name>
    <filename>class_protobuf_serialization_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>RapidjsonParseErrorException</name>
    <filename>class_rapidjson_parse_error_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::RegisterAddress</name>
    <filename>classmodel_1_1gradido_1_1_register_address.html</filename>
    <base>model::gradido::TransactionBase</base>
    <member kind="function">
      <type>int</type>
      <name>prepare</name>
      <anchorfile>classmodel_1_1gradido_1_1_register_address.html</anchorfile>
      <anchor>a49e280360dd82da8ba1a7dd239ff72d8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; MemoryBin * &gt;</type>
      <name>getInvolvedAddresses</name>
      <anchorfile>classmodel_1_1gradido_1_1_register_address.html</anchorfile>
      <anchor>abf98b586ef860e01dc78bd741910d148</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RequestEmptyResponseException</name>
    <filename>class_request_empty_response_exception.html</filename>
    <base>RequestException</base>
  </compound>
  <compound kind="class">
    <name>RequestException</name>
    <filename>class_request_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>RequestResponseCakePHPException</name>
    <filename>class_request_response_cake_p_h_p_exception.html</filename>
    <base>RequestResponseErrorException</base>
  </compound>
  <compound kind="class">
    <name>RequestResponseErrorException</name>
    <filename>class_request_response_error_exception.html</filename>
    <base>RequestException</base>
  </compound>
  <compound kind="class">
    <name>RequestResponseInvalidJsonException</name>
    <filename>class_request_response_invalid_json_exception.html</filename>
    <base>RequestException</base>
    <member kind="function">
      <type>bool</type>
      <name>printToFile</name>
      <anchorfile>class_request_response_invalid_json_exception.html</anchorfile>
      <anchor>ad6d997e611552a93fceb300af66033d4</anchor>
      <arglist>(const char *prefix, const char *ending=&quot;.txt&quot;) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SecretKeyCryptography</name>
    <filename>class_secret_key_cryptography.html</filename>
    <member kind="function">
      <type></type>
      <name>SecretKeyCryptography</name>
      <anchorfile>class_secret_key_cryptography.html</anchorfile>
      <anchor>a12e896fd174654c6c88a138c1c3bff71</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SecretKeyCryptography</name>
      <anchorfile>class_secret_key_cryptography.html</anchorfile>
      <anchor>a2deb531eceaa77d9f6bdb454b0cd0008</anchor>
      <arglist>(unsigned long long opslimit, size_t memlimit, int algo)</arglist>
    </member>
    <member kind="function">
      <type>ResultType</type>
      <name>createKey</name>
      <anchorfile>class_secret_key_cryptography.html</anchorfile>
      <anchor>a0a13ce0ff327b5cd2f803a3579ab3ccc</anchor>
      <arglist>(const std::string &amp;salt_parameter, const std::string &amp;passwd)</arglist>
    </member>
    <member kind="function">
      <type>ResultType</type>
      <name>decrypt</name>
      <anchorfile>class_secret_key_cryptography.html</anchorfile>
      <anchor>a5306dd15a2892e8ffb8879b72fdd44ee</anchor>
      <arglist>(const std::vector&lt; unsigned char &gt; &amp;encryptedMessage, MemoryBin **message) const</arglist>
    </member>
    <member kind="function">
      <type>ResultType</type>
      <name>decrypt</name>
      <anchorfile>class_secret_key_cryptography.html</anchorfile>
      <anchor>a1e4933ad34a7560080fbf49deffee3ba</anchor>
      <arglist>(const unsigned char *encryptedMessage, size_t encryptedMessageSize, MemoryBin **message) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionBase</name>
    <filename>classmodel_1_1gradido_1_1_transaction_base.html</filename>
    <base>MultithreadContainer</base>
    <member kind="function" virtualness="virtual">
      <type>virtual int</type>
      <name>prepare</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_base.html</anchorfile>
      <anchor>afa1f59c0726d85b0403ea56e7e9afaa7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::vector&lt; MemoryBin * &gt;</type>
      <name>getInvolvedAddresses</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_base.html</anchorfile>
      <anchor>a7ba3d9c63f04d3fcb609f2c461d111b9</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>checkRequiredSignatures</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_base.html</anchorfile>
      <anchor>a1b132888e8277372ee6ec9e90924fdb5</anchor>
      <arglist>(const proto::gradido::SignatureMap *sig_map) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isPublicKeyRequired</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_base.html</anchorfile>
      <anchor>a9f7dcad0ceb9881acff77b5bef5868d8</anchor>
      <arglist>(const unsigned char *pubkey) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isPublicKeyForbidden</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_base.html</anchorfile>
      <anchor>a5c7030b08d3078d547a19f6383bcc08a</anchor>
      <arglist>(const unsigned char *pubkey) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionBody</name>
    <filename>classmodel_1_1gradido_1_1_transaction_body.html</filename>
    <base>MultithreadContainer</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionCreation</name>
    <filename>classmodel_1_1gradido_1_1_transaction_creation.html</filename>
    <base>model::gradido::TransactionBase</base>
    <member kind="function">
      <type>int</type>
      <name>prepare</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_creation.html</anchorfile>
      <anchor>a7763225d40e1e89e0cff5f1186e73cd0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>validate</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_creation.html</anchorfile>
      <anchor>ab370ea2e1bafeb69dce5fc59c6625394</anchor>
      <arglist>(TransactionValidationLevel level=TRANSACTION_VALIDATION_SINGLE, IGradidoBlockchain *blockchain=nullptr, const GradidoBlock *parentGradidoBlock=nullptr) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; MemoryBin * &gt;</type>
      <name>getInvolvedAddresses</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_creation.html</anchorfile>
      <anchor>af83441d805c38a2a0a161564dbfb4e73</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>model::TransactionEntry</name>
    <filename>classmodel_1_1_transaction_entry.html</filename>
    <member kind="function">
      <type></type>
      <name>TransactionEntry</name>
      <anchorfile>classmodel_1_1_transaction_entry.html</anchorfile>
      <anchor>a6229e4482960a4469c65a1711162c01c</anchor>
      <arglist>(std::unique_ptr&lt; std::string &gt; _serializedTransaction)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TransactionEntry</name>
      <anchorfile>classmodel_1_1_transaction_entry.html</anchorfile>
      <anchor>a7ba798c66070d6bc85a1f5f3b6acfbf2</anchor>
      <arglist>(uint64_t transactionNr, uint8_t month, uint16_t year, uint32_t coinColor)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classmodel_1_1_transaction_entry.html</anchorfile>
      <anchor>aeb00c6d48e385cac4e1b2c95ba93e1b4</anchor>
      <arglist>(const TransactionEntry &amp;b)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TransactionFactory</name>
    <filename>class_transaction_factory.html</filename>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionTransfer</name>
    <filename>classmodel_1_1gradido_1_1_transaction_transfer.html</filename>
    <base>model::gradido::TransactionBase</base>
    <member kind="function">
      <type>int</type>
      <name>prepare</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_transfer.html</anchorfile>
      <anchor>affbf9dbbb3b44f1994fdb6126a297a6d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; MemoryBin * &gt;</type>
      <name>getInvolvedAddresses</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_transfer.html</anchorfile>
      <anchor>aafeb1cb2575e2b3f5033cd425dc28c6a</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionValidationException</name>
    <filename>classmodel_1_1gradido_1_1_transaction_validation_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionValidationForbiddenSignException</name>
    <filename>classmodel_1_1gradido_1_1_transaction_validation_forbidden_sign_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionValidationInvalidInputException</name>
    <filename>classmodel_1_1gradido_1_1_transaction_validation_invalid_input_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionValidationInvalidSignatureException</name>
    <filename>classmodel_1_1gradido_1_1_transaction_validation_invalid_signature_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionValidationMissingSignException</name>
    <filename>classmodel_1_1gradido_1_1_transaction_validation_missing_sign_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::TransactionValidationRequiredSignMissingException</name>
    <filename>classmodel_1_1gradido_1_1_transaction_validation_required_sign_missing_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="namespace">
    <name>model</name>
    <filename>namespacemodel.html</filename>
    <class kind="class">model::IGradidoBlockchain</class>
    <class kind="class">model::TransactionEntry</class>
  </compound>
  <compound kind="group">
    <name>Gradido-Protocol</name>
    <title>Gradido-Protocol</title>
    <filename>group___gradido-_protocol.html</filename>
    <class kind="class">model::gradido::GradidoBlock</class>
    <class kind="class">model::gradido::GradidoTransaction</class>
    <class kind="class">model::gradido::TransactionBody</class>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Gradido Blockchain</title>
    <filename>index</filename>
  </compound>
</tagfile>
