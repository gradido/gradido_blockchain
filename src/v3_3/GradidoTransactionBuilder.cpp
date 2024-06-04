#include "gradido_blockchain/v3_3/GradidoTransactionBuilder.h"
#include "gradido_blockchain/v3_3/interaction/serialize/Context.h"

using namespace std;

namespace gradido {
	namespace v3_3 {

		using namespace data;

		GradidoTransactionBuilder::GradidoTransactionBuilder()
			: mGradidoTransaction(make_unique<GradidoTransaction>())
		{

		}

		GradidoTransactionBuilder::~GradidoTransactionBuilder()
		{

		}

		void GradidoTransactionBuilder::reset() 
		{
			mGradidoTransaction = make_unique<GradidoTransaction>();
		}
		std::unique_ptr<data::GradidoTransaction> GradidoTransactionBuilder::build()
		{
			auto transaction = move(mGradidoTransaction);
			reset();
			return move(transaction);
		}

		GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionBody(std::unique_ptr<data::TransactionBody> body)
		{
			interaction::serialize::Context context(*body.get());
			mGradidoTransaction->mBodyBytes = context.run();
			return *this;
		}

		GradidoTransactionBuilder& GradidoTransactionBuilder::addSignaturePair(memory::ConstBlockPtr publicKey, memory::ConstBlockPtr signature)
		{
			mGradidoTransaction->mSignatureMap.push(SignaturePair(publicKey, signature));
			return *this;
		}

		GradidoTransactionBuilder& GradidoTransactionBuilder::sign(std::shared_ptr<KeyPairEd25519> keyPair)
		{
			if (!mGradidoTransaction->mBodyBytes) {
				throw GradidoTransactionBuilderException("please call setTransactionBody before");
			}
			auto signature = make_shared<memory::Block>(keyPair->sign(*mGradidoTransaction->mBodyBytes));
			addSignaturePair(keyPair->getPublicKey(), signature);
			return *this;
		}

		GradidoTransactionBuilder& GradidoTransactionBuilder::setParentMessageId(memory::ConstBlockPtr parentMessageId)
		{
			mGradidoTransaction->mParentMessageId = parentMessageId;
			return *this;
		}
	}
}