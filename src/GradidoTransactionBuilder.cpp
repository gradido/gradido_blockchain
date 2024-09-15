#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"

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
		if(!mGradidoTransaction) {
			throw GradidoTransactionBuilderException("gradido transaction is nullptr, was this changed to cross group transaction? Then please call buildOutbound.");
		}
		auto transaction = move(mGradidoTransaction);
		reset();
		return move(transaction);
	}

	std::unique_ptr<data::GradidoTransaction> GradidoTransactionBuilder::buildOutbound()
	{
		if(mSenderCommunity.empty() || mRecipientCommunity.empty()) {
			throw GradidoTransactionBuilderException("missing sender and/or recipient community for cross group transaction");
		}
	}

	std::unique_ptr<data::GradidoTransaction> GradidoTransactionBuilder::buildInbound()
	{
		if(mSenderCommunity.empty() || mRecipientCommunity.empty()) {
			throw GradidoTransactionBuilderException("missing sender and/or recipient community for cross group transaction");
		}
		if(!mGradidoTransaction->mParingMessageId || mGradidoTransaction->mParingMessageId->isEmpty()) {
			throw GradidoTransactionBuilderException("missing paring message id from outbound transaction for inbound transaction");
		}
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionBody(std::unique_ptr<data::TransactionBody> body)
	{
		interaction::serialize::Context context(*body.get());
		mGradidoTransaction->mBodyBytes = context.run();
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionBody(memory::ConstBlockPtr bodyBytes)
	{
		mGradidoTransaction->mBodyBytes = bodyBytes;
		return *this;
	}
	GradidoTransactionBuilder& GradidoTransactionBuilder::setSenderCommunity(const std::string& senderCommunity)
	{
		if(mGradidoTransaction->getSignatureMap().getSignaturePairs().size()) {
			throw GradidoTransactionBuilderException("please call setSenderCommunity before any call to addSignaturePair or sign");
		}
		mSenderCommunity = senderCommunity;
		initializeCrossCommunityTransaction();
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setRecipientCommunity(const std::string& recipientCommunity)
	{
		if(mGradidoTransaction->getSignatureMap().getSignaturePairs().size()) {
			throw GradidoTransactionBuilderException("please call setRecipientCommunity before any call to addSignaturePair or sign");
		}
		mRecipientCommunity = recipientCommunity;
		initializeCrossCommunityTransaction();
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

	GradidoTransactionBuilder& GradidoTransactionBuilder::setParentMessageId(memory::ConstBlockPtr paringMessageId)
	{
		mGradidoTransaction->mParingMessageId = paringMessageId;
		return *this;
	}
	bool GradidoTransactionBuilder::isCrossCommunityTransaction() const
	{
		// XOR
		if(mSenderCommunity.empty() != mRecipientCommunity.empty()) {
			throw GradidoTransactionBuilderException("please set both sender community and recipient community or none");
		}
		return !mSenderCommunity.empty();
	}
	void GradidoTransactionBuilder::initializeCrossCommunityTransaction()
	{
		if(!isCrossCommunityTransaction() || (mOutboundTransaction && mInboundTransaction && !mGradidoTransaction)) {
			return;
		}
		assert(mGradidoTransaction);
		// copy gradido transaction 
		auto bodyBytes = mGradidoTransaction->getBodyBytes();
		mOutboundTransaction = std::make_unique<GradidoTransaction>(*mGradidoTransaction);
		mGradidoTransaction.reset();
		mInboundTransaction = std::make_unique<GradidoTransaction>(*mOutboundTransaction);
		if(bodyBytes && !bodyBytes->isEmpty()) {
			updateTransactionBodiesForCrossCommunityTransactions(bodyBytes);
		}
	}

	void GradidoTransactionBuilder::updateTransactionBodiesForCrossCommunityTransactions(const memory::ConstBlockPtr& bodyBytes)
	{
		interaction::deserialize::Context bodyBytesDeserializer(bodyBytes, interaction::deserialize::Type::TRANSACTION_BODY);
		bodyBytesDeserializer.run();
		auto transactionBody = bodyBytesDeserializer.getTransactionBody();
		transactionBody->mType = CrossGroupType::OUTBOUND;

	}
}
