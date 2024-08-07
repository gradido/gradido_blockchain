#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

using namespace std;

namespace gradido {

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
		mGradidoTransaction->bodyBytes = context.run();
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::addSignaturePair(memory::ConstBlockPtr publicKey, memory::ConstBlockPtr signature)
	{
		mGradidoTransaction->signatureMap.push(SignaturePair(publicKey, signature));
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::sign(std::shared_ptr<KeyPairEd25519> keyPair)
	{
		if (!mGradidoTransaction->bodyBytes) {
			throw GradidoTransactionBuilderException("please call setTransactionBody before");
		}
		auto signature = make_shared<memory::Block>(keyPair->sign(*mGradidoTransaction->bodyBytes));
		addSignaturePair(keyPair->getPublicKey(), signature);
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setParentMessageId(memory::ConstBlockPtr paringMessageId)
	{
		mGradidoTransaction->paringMessageId = paringMessageId;
		return *this;
	}
}