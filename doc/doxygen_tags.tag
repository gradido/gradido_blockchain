<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.1" doxygen_gitid="ef9b20ac7f8a8621fcfc299f8bd0b80422390f4b">
  <compound kind="file">
    <name>TransactionBase.h</name>
    <path>F:/Gradido/gradido_blockchain_connector/dependencies/gradido_blockchain/include/gradido_blockchain/model/protobufWrapper/</path>
    <filename>_transaction_base_8h.html</filename>
    <class kind="class">model::gradido::TransactionBase</class>
    <namespace>model</namespace>
    <member kind="enumeration">
      <type></type>
      <name>TransactionType</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>ga63424b05ada401e320ed8c4473623a41</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_NONE</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41a7f6ce8cb9a8a22c017268a7a69f4fe07</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_CREATION</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41ae8b37c8c0f68019baef843c1fe486bef</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_TRANSFER</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41a3c755b89c3b6bfba5f24eee128b35503</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_GROUP_FRIENDS_UPDATE</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41a77e8ad86b3d2b7c8b2ede367d8248de0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_REGISTER_ADDRESS</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41ad4b904f6a035b1bd125bd550c0a9140b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_DEFERRED_TRANSFER</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41a0573bb0067ed10c3ff945b3b1ff9da26</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>TransactionValidationLevel</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>ga5a93fcdc94d643d95a6b670b2a924443</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_SINGLE</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a49f01ddec0f2d6ba82019c7bc21c6976</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_SINGLE_PREVIOUS</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a3594e89dac1b1dbe256275a645d6ccf9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_DATE_RANGE</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a04a15f9be4057756e66c28ad993a53c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_PAIRED</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a3ec32906ce798a445ffc3e0137275edd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_CONNECTED_GROUP</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443abf0fa3ca4cf18c8c5700e314f003f95b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_CONNECTED_BLOCKCHAIN</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a7ef43701d438775acf61a357284ad290</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>model::TransactionsManager::AccountInGroupNotFoundException</name>
    <filename>classmodel_1_1_transactions_manager_1_1_account_in_group_not_found_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
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
    <name>CachedDecimal</name>
    <filename>class_cached_decimal.html</filename>
    <base>Decimal</base>
  </compound>
  <compound kind="class">
    <name>CrossGroupTransactionBuilder</name>
    <filename>class_cross_group_transaction_builder.html</filename>
  </compound>
  <compound kind="class">
    <name>DecayDecimal</name>
    <filename>class_decay_decimal.html</filename>
    <base>Decimal</base>
    <member kind="function">
      <type></type>
      <name>Decimal</name>
      <anchorfile>class_decay_decimal.html</anchorfile>
      <anchor>a4eb311753379a77d4597a0aa4bcb4681</anchor>
      <arglist>(const std::string &amp;decimalString)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Decimal</name>
    <filename>class_decimal.html</filename>
    <member kind="function">
      <type></type>
      <name>Decimal</name>
      <anchorfile>class_decimal.html</anchorfile>
      <anchor>a4eb311753379a77d4597a0aa4bcb4681</anchor>
      <arglist>(const std::string &amp;decimalString)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SealedBoxes::DecryptException</name>
    <filename>class_sealed_boxes_1_1_decrypt_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>DecryptionFailedException</name>
    <filename>class_decryption_failed_exception.html</filename>
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
    <name>Ed25519DeriveException</name>
    <filename>class_ed25519_derive_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>Ed25519SignException</name>
    <filename>class_ed25519_sign_exception.html</filename>
    <base>GradidoBlockchainException</base>
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
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>mBodyDirty</name>
      <anchorfile>classmodel_1_1gradido_1_1_gradido_transaction.html</anchorfile>
      <anchor>a58aa28346c5ee07c13659b6193474cde</anchor>
      <arglist></arglist>
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
    <name>model::TransactionsManager::GroupNotFoundException</name>
    <filename>classmodel_1_1_transactions_manager_1_1_group_not_found_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="struct">
    <name>model::TransactionsManager::GroupTransactions</name>
    <filename>structmodel_1_1_transactions_manager_1_1_group_transactions.html</filename>
    <member kind="variable">
      <type>std::map&lt; std::string, TransactionList &gt;</type>
      <name>transactionsByPubkey</name>
      <anchorfile>structmodel_1_1_transactions_manager_1_1_group_transactions.html</anchorfile>
      <anchor>ad61e5ee3932faa91f8885612d7d55c6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::multimap&lt; Poco::DateTime, std::shared_ptr&lt; model::gradido::GradidoTransaction &gt; &gt;</type>
      <name>transactionsByReceived</name>
      <anchorfile>structmodel_1_1_transactions_manager_1_1_group_transactions.html</anchorfile>
      <anchor>af346f28c137999e1dfb39919bc8344ca</anchor>
      <arglist></arglist>
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
    <member kind="enumeration">
      <type></type>
      <name>FilterResult</name>
      <anchorfile>classmodel_1_1_i_gradido_blockchain.html</anchorfile>
      <anchor>af20b4dbd5c36c385c2b3dd98053ff9a8</anchor>
      <arglist></arglist>
      <enumvalue file="classmodel_1_1_i_gradido_blockchain.html" anchor="af20b4dbd5c36c385c2b3dd98053ff9a8a7dc33953b23388ad93a4db20e33d26e4">USE</enumvalue>
      <enumvalue file="classmodel_1_1_i_gradido_blockchain.html" anchor="af20b4dbd5c36c385c2b3dd98053ff9a8a152bb1f283844ecaab86a72e4877052c">DISMISS</enumvalue>
      <enumvalue file="classmodel_1_1_i_gradido_blockchain.html" anchor="af20b4dbd5c36c385c2b3dd98053ff9a8a615a46af313786fc4e349f34118be111">STOP</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>SearchDirection</name>
      <anchorfile>classmodel_1_1_i_gradido_blockchain.html</anchorfile>
      <anchor>ac7fe3ddbc6c0d1d3d704c1474de7d305</anchor>
      <arglist></arglist>
      <enumvalue file="classmodel_1_1_i_gradido_blockchain.html" anchor="ac7fe3ddbc6c0d1d3d704c1474de7d305ac6e421eaad140c1bc1a39980502df80c">ASC</enumvalue>
      <enumvalue file="classmodel_1_1_i_gradido_blockchain.html" anchor="ac7fe3ddbc6c0d1d3d704c1474de7d305a65a6d757dbb571ccc3af9706e9a5f607">DESC</enumvalue>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::vector&lt; Poco::SharedPtr&lt; TransactionEntry &gt; &gt;</type>
      <name>findTransactions</name>
      <anchorfile>classmodel_1_1_i_gradido_blockchain.html</anchorfile>
      <anchor>a4832861b1a95c2295c700b5a54edcedf</anchor>
      <arglist>(const std::string &amp;address)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::vector&lt; Poco::SharedPtr&lt; model::TransactionEntry &gt; &gt;</type>
      <name>findTransactions</name>
      <anchorfile>classmodel_1_1_i_gradido_blockchain.html</anchorfile>
      <anchor>af752c87f3f532bda650906119b456493</anchor>
      <arglist>(const std::string &amp;address, int month, int year)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>model::gradido::InsufficientBalanceException</name>
    <filename>classmodel_1_1gradido_1_1_insufficient_balance_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="class">
    <name>model::gradido::InvalidCreationException</name>
    <filename>classmodel_1_1gradido_1_1_invalid_creation_exception.html</filename>
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
      <anchor>a6bb407268dc34273a02b8e3e1f4c698f</anchor>
      <arglist>(MemoryBin *privateKey, MemoryBin *chainCode=nullptr)</arglist>
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
    <name>KeyPairEd25519Ex</name>
    <filename>class_key_pair_ed25519_ex.html</filename>
    <base>KeyPairEd25519</base>
    <member kind="function">
      <type></type>
      <name>KeyPairEd25519Ex</name>
      <anchorfile>class_key_pair_ed25519_ex.html</anchorfile>
      <anchor>afed20aa2252d3af3c000e0aff450b913</anchor>
      <arglist>(MemoryBin *privateKeyEx, MemoryBin *chainCode, int derivationIndex)</arglist>
    </member>
    <member kind="function">
      <type>MemoryBin *</type>
      <name>sign</name>
      <anchorfile>class_key_pair_ed25519_ex.html</anchorfile>
      <anchor>ad48ab69b6d953f1e7f2fe564f3977846</anchor>
      <arglist>(const unsigned char *message, size_t messageSize) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MapEnvironmentToConfig</name>
    <filename>class_map_environment_to_config.html</filename>
    <member kind="function">
      <type>std::string</type>
      <name>getString</name>
      <anchorfile>class_map_environment_to_config.html</anchorfile>
      <anchor>a4d606ace986ed95c358f25900d3a9c74</anchor>
      <arglist>(const std::string &amp;key, const std::string &amp;defaultValue) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getInt</name>
      <anchorfile>class_map_environment_to_config.html</anchorfile>
      <anchor>a9e7333ade24a8c6fbfa8aabc251a1999</anchor>
      <arglist>(const std::string &amp;key, int defaultValue) const</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>std::string</type>
      <name>mapKey</name>
      <anchorfile>class_map_environment_to_config.html</anchorfile>
      <anchor>a838bd8945b92ebfdccba3c867d8b3e4f</anchor>
      <arglist>(const std::string &amp;key) const</arglist>
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
    <name>model::TransactionsManager::MissingTransactionNrException</name>
    <filename>classmodel_1_1_transactions_manager_1_1_missing_transaction_nr_exception.html</filename>
    <base>GradidoBlockchainException</base>
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
    <name>ParseDecimalStringException</name>
    <filename>class_parse_decimal_string_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>ParseStringToMpfrException</name>
    <filename>class_parse_string_to_mpfr_exception.html</filename>
    <base>GradidoBlockchainException</base>
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
    <name>ProtobufArenaMemory</name>
    <filename>class_protobuf_arena_memory.html</filename>
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
    <name>RapidjsonInvalidMemberException</name>
    <filename>class_rapidjson_invalid_member_exception.html</filename>
    <base>RapidjsonMissingMemberException</base>
  </compound>
  <compound kind="class">
    <name>RapidjsonMissingMemberException</name>
    <filename>class_rapidjson_missing_member_exception.html</filename>
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
      <anchor>a77157ad85866bbe42fc8e7270f6de720</anchor>
      <arglist>(uint64_t transactionNr, uint8_t month, uint16_t year, std::string groupId)</arglist>
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
    <name>model::TransactionsCountException</name>
    <filename>classmodel_1_1_transactions_count_exception.html</filename>
    <base>GradidoBlockchainException</base>
  </compound>
  <compound kind="class">
    <name>model::TransactionsManager</name>
    <filename>classmodel_1_1_transactions_manager.html</filename>
    <base protection="protected">MultithreadContainer</base>
    <class kind="class">model::TransactionsManager::AccountInGroupNotFoundException</class>
    <class kind="class">model::TransactionsManager::GroupNotFoundException</class>
    <class kind="struct">model::TransactionsManager::GroupTransactions</class>
    <class kind="class">model::TransactionsManager::MissingTransactionNrException</class>
    <class kind="struct">model::TransactionsManager::UserBalance</class>
    <member kind="variable" protection="protected">
      <type>std::unordered_map&lt; std::string, GroupTransactions &gt;</type>
      <name>mAllTransactions</name>
      <anchorfile>classmodel_1_1_transactions_manager.html</anchorfile>
      <anchor>a32fdce7b5b01001a22b3c4c1743612cb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>model::TransactionsManagerBlockchain</name>
    <filename>classmodel_1_1_transactions_manager_blockchain.html</filename>
    <base>model::IGradidoBlockchain</base>
    <member kind="function">
      <type>std::vector&lt; Poco::SharedPtr&lt; TransactionEntry &gt; &gt;</type>
      <name>searchTransactions</name>
      <anchorfile>classmodel_1_1_transactions_manager_blockchain.html</anchorfile>
      <anchor>a937dfd48698696995b92f12446057b55</anchor>
      <arglist>(uint64_t startTransactionNr=0, std::function&lt; FilterResult(model::TransactionEntry *)&gt; filter=nullptr, SearchDirection order=SearchDirection::ASC)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Poco::SharedPtr&lt; TransactionEntry &gt; &gt;</type>
      <name>findTransactions</name>
      <anchorfile>classmodel_1_1_transactions_manager_blockchain.html</anchorfile>
      <anchor>aae3ac8194d9fb3a36a65251ab4e48289</anchor>
      <arglist>(const std::string &amp;address)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Poco::SharedPtr&lt; model::TransactionEntry &gt; &gt;</type>
      <name>findTransactions</name>
      <anchorfile>classmodel_1_1_transactions_manager_blockchain.html</anchorfile>
      <anchor>a7baf7bb80754941f56e41751eeff9eaa</anchor>
      <arglist>(const std::string &amp;address, int month, int year)</arglist>
    </member>
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
    <member kind="function">
      <type>rapidjson::Value</type>
      <name>getDetails</name>
      <anchorfile>classmodel_1_1gradido_1_1_transaction_validation_invalid_input_exception.html</anchorfile>
      <anchor>a4474c62d4f9b8392066f4709d756fd42</anchor>
      <arglist>(rapidjson::Document::AllocatorType &amp;alloc) const</arglist>
    </member>
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
  <compound kind="struct">
    <name>model::TransactionsManager::UserBalance</name>
    <filename>structmodel_1_1_transactions_manager_1_1_user_balance.html</filename>
  </compound>
  <compound kind="class">
    <name>model::gradido::WrongAddressTypeException</name>
    <filename>classmodel_1_1gradido_1_1_wrong_address_type_exception.html</filename>
    <base>model::gradido::TransactionValidationException</base>
  </compound>
  <compound kind="namespace">
    <name>model</name>
    <filename>namespacemodel.html</filename>
    <class kind="class">model::IGradidoBlockchain</class>
    <class kind="class">model::TransactionEntry</class>
    <class kind="class">model::TransactionsManager</class>
    <class kind="class">model::TransactionsManagerBlockchain</class>
    <class kind="class">model::TransactionsCountException</class>
  </compound>
  <compound kind="group">
    <name>Gradido-Protocol</name>
    <title>Gradido-Protocol</title>
    <filename>group___gradido-_protocol.html</filename>
    <class kind="class">model::gradido::GradidoBlock</class>
    <class kind="class">model::gradido::GradidoTransaction</class>
    <class kind="class">model::gradido::TransactionBody</class>
  </compound>
  <compound kind="group">
    <name>enums</name>
    <title>Enums</title>
    <filename>group__enums.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>TransactionType</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>ga63424b05ada401e320ed8c4473623a41</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_NONE</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41a7f6ce8cb9a8a22c017268a7a69f4fe07</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_CREATION</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41ae8b37c8c0f68019baef843c1fe486bef</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_TRANSFER</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41a3c755b89c3b6bfba5f24eee128b35503</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_GROUP_FRIENDS_UPDATE</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41a77e8ad86b3d2b7c8b2ede367d8248de0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_REGISTER_ADDRESS</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41ad4b904f6a035b1bd125bd550c0a9140b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_DEFERRED_TRANSFER</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga63424b05ada401e320ed8c4473623a41a0573bb0067ed10c3ff945b3b1ff9da26</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>TransactionValidationLevel</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>ga5a93fcdc94d643d95a6b670b2a924443</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_SINGLE</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a49f01ddec0f2d6ba82019c7bc21c6976</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_SINGLE_PREVIOUS</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a3594e89dac1b1dbe256275a645d6ccf9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_DATE_RANGE</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a04a15f9be4057756e66c28ad993a53c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_PAIRED</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a3ec32906ce798a445ffc3e0137275edd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_CONNECTED_GROUP</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443abf0fa3ca4cf18c8c5700e314f003f95b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TRANSACTION_VALIDATION_CONNECTED_BLOCKCHAIN</name>
      <anchorfile>group__enums.html</anchorfile>
      <anchor>gga5a93fcdc94d643d95a6b670b2a924443a7ef43701d438775acf61a357284ad290</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Gradido Blockchain</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md_README</docanchor>
  </compound>
</tagfile>
