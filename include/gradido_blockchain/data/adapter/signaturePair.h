#ifndef GRADIDO_BLOCKCHAIN_DATA_ADAPTER_SIGNATUREPAIR_H
#define GRADIDO_BLOCKCHAIN_DATA_ADAPTER_SIGNATUREPAIR_H

#include "gradido_blockchain/data/SignaturePair.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_protobuf_zig.h"

namespace gradido::data::adapter {
  inline SignaturePair fromGrdw(grdw_signature_pair* signaturePair) {
    return {
      std::make_shared<const memory::Block>(32, signaturePair->public_key),
      std::make_shared<const memory::Block>(64, signaturePair->signature)
    };
  }
  inline grdw_signature_pair toGrdw(const SignaturePair& signaturePair) {
    grdw_signature_pair result;
    assert(signaturePair.getPublicKey()->size() == 32 && signaturePair.getSignature()->size() == 64);
    memcpy(result.public_key, signaturePair.getPublicKey()->data(), 32);
    memcpy(result.signature, signaturePair.getSignature()->data(), 64);
    return result;
  }
}
#endif // GRADIDO_BLOCKCHAIN_DATA_ADAPTER_SIGNATUREPAIR_H