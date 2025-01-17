#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;
using namespace std;

namespace gradido {

	using namespace data;

	GradidoTransactionBuilder::GradidoTransactionBuilder() 
	  : mState(BuildingState::BUILDING_BODY), 
		mBody(make_unique<TransactionBody>(
			chrono::system_clock::now(), GRADIDO_TRANSACTION_BODY_VERSION_STRING, CrossGroupType::LOCAL
		)),
		mSpecificTransactionChoosen(false)
	{
		mBodyByteSignatureMaps.reserve(2);
	}

	GradidoTransactionBuilder::~GradidoTransactionBuilder()
	{

	}

	void GradidoTransactionBuilder::reset() 
	{
		mState = BuildingState::BUILDING_BODY;
		mBody = make_unique<data::TransactionBody>(chrono::system_clock::now(), GRADIDO_TRANSACTION_BODY_VERSION_STRING, CrossGroupType::LOCAL);
		mSenderCommunity.clear();
		mRecipientCommunity.clear();
		mBodyByteSignatureMaps.clear();
		mSpecificTransactionChoosen = false;
	}
	std::unique_ptr<data::GradidoTransaction> GradidoTransactionBuilder::build()
	{
		checkBuildState(BuildingState::LOCAL);
		assert(mBodyByteSignatureMaps.size());
		auto result = make_unique<data::GradidoTransaction>(
			mBodyByteSignatureMaps[0].signatureMap,
			mBodyByteSignatureMaps[0].bodyBytes,
			mParingMessageId
		);
		reset();
		return move(result);
	}

	std::unique_ptr<data::GradidoTransaction> GradidoTransactionBuilder::buildOutbound()
	{
		checkBuildState(BuildingState::CROSS_COMMUNITY);
		assert(mBodyByteSignatureMaps.size());
		auto result = make_unique<data::GradidoTransaction>(
			mBodyByteSignatureMaps[0].signatureMap,
			mBodyByteSignatureMaps[0].bodyBytes,
			mParingMessageId
		);
		reset();
		return move(result);
	}

	std::unique_ptr<data::GradidoTransaction> GradidoTransactionBuilder::buildInbound()
	{
		checkBuildState(BuildingState::CROSS_COMMUNITY);
		assert(mBodyByteSignatureMaps.size() > 1);
		if(!mParingMessageId || mParingMessageId->isEmpty()) {
			throw GradidoTransactionBuilderException("missing paring message id from outbound transaction for inbound transaction");
		}
		auto result = make_unique<data::GradidoTransaction>(
			mBodyByteSignatureMaps[1].signatureMap,
			mBodyByteSignatureMaps[1].bodyBytes,
			mParingMessageId
		);
		reset();
		return move(result);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setDeferredTransfer(
		data::GradidoTransfer transactionTransfer,
		DurationSeconds timeoutDuration
	) {
		checkBuildState(BuildingState::BUILDING_BODY);
		return setDeferredTransfer(
			std::make_unique<data::GradidoDeferredTransfer>(
				transactionTransfer,
				timeoutDuration
			)
		);
	}
	GradidoTransactionBuilder& GradidoTransactionBuilder::setDeferredTransfer(unique_ptr<GradidoDeferredTransfer> deferredTransfer)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mDeferredTransfer = move(deferredTransfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setCommunityFriendsUpdate(bool colorFusion)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setCommunityFriendsUpdate(
			std::make_unique<data::CommunityFriendsUpdate>(
				colorFusion
			)
		);
	}
	GradidoTransactionBuilder& GradidoTransactionBuilder::setCommunityFriendsUpdate(unique_ptr<CommunityFriendsUpdate> communityFriendsUpdate)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCommunityFriendsUpdate = std::move(communityFriendsUpdate);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setRegisterAddress(
		memory::ConstBlockPtr userPubkey,
		data::AddressType type,
		memory::ConstBlockPtr nameHash/* = nullptr*/,
		memory::ConstBlockPtr accountPubkey/* = nullptr*/
	)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setRegisterAddress(
			make_unique<data::RegisterAddress>(
				type,
				1,
				userPubkey,
				nameHash,
				accountPubkey
			)
		);
	};

	GradidoTransactionBuilder& GradidoTransactionBuilder::setRegisterAddress(unique_ptr<RegisterAddress> registerAddress)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mRegisterAddress = std::move(registerAddress);

		mSpecificTransactionChoosen = true;
		return *this;
	}
	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionCreation(data::TransferAmount recipient, Timepoint targetDate)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setTransactionCreation(
			make_unique<data::GradidoCreation>(
				recipient,
				targetDate
			)
		);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionCreation(unique_ptr<GradidoCreation> creation) 
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}
		mBody->mCreation = std::move(creation);

		mSpecificTransactionChoosen = true;
		return *this;
	}


	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionTransfer(
		data::TransferAmount sender,
		memory::ConstBlockPtr recipientPubkey
	)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setTransactionTransfer(
			make_unique<data::GradidoTransfer>(
				sender,
				recipientPubkey
			)
		);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionTransfer(unique_ptr<GradidoTransfer> transfer)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mTransfer = std::move(transfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}


	GradidoTransactionBuilder& GradidoTransactionBuilder::setCommunityRoot(
		memory::ConstBlockPtr pubkey,
		memory::ConstBlockPtr gmwPubkey,
		memory::ConstBlockPtr aufPubkey
	) {
		checkBuildState(BuildingState::BUILDING_BODY);
		return setCommunityRoot(
			make_unique<data::CommunityRoot>(
				pubkey,
				gmwPubkey,
				aufPubkey
			)
		);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setCommunityRoot(unique_ptr<CommunityRoot> communityRoot)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCommunityRoot = std::move(communityRoot);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setRedeemDeferredTransfer(uint64_t deferredTransferTransactionNr, GradidoTransfer transactionTransfer)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setRedeemDeferredTransfer(
			std::make_unique<data::GradidoRedeemDeferredTransfer>(
				deferredTransferTransactionNr,
				transactionTransfer
			)
		);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setRedeemDeferredTransfer(std::unique_ptr<GradidoRedeemDeferredTransfer> redeemDeferredTransfer)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mRedeemDeferredTransfer = move(redeemDeferredTransfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTimeoutDeferredTransfer(uint64_t deferredTransferTransactionNr)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setTimeoutDeferredTransfer(
			std::make_unique<data::GradidoTimeoutDeferredTransfer>(
				deferredTransferTransactionNr
			)
		);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTimeoutDeferredTransfer(std::unique_ptr<GradidoTimeoutDeferredTransfer> timeoutDeferredTransfer)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mTimeoutDeferredTransfer = move(timeoutDeferredTransfer);
		// special case, because TimeoutDeferredTransfer didn't need singnatures
		switchBuildState();
		mSpecificTransactionChoosen = true;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setCreatedAt(Timepoint createdAt) {
		checkBuildState(BuildingState::BUILDING_BODY);
		mBody->mCreatedAt = data::Timestamp(createdAt);
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::addMemo(EncryptedMemo memo) {
		checkBuildState(BuildingState::BUILDING_BODY);
		mBody->mMemos.push_back(memo);
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setVersionNumber(std::string_view versionNumber) {
		checkBuildState(BuildingState::BUILDING_BODY);
		mBody->mVersionNumber = versionNumber;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionBody(std::unique_ptr<data::TransactionBody> body)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		mBody = std::move(body);
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionBody(memory::ConstBlockPtr bodyBytes)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		interaction::deserialize::Context deserializer(bodyBytes, interaction::deserialize::Type::TRANSACTION_BODY);
		deserializer.run();
		if (!deserializer.isTransactionBody()) {
			throw GradidoTransactionBuilderException("cannot deserialize TransactionBody");
		}
		mBody = std::make_unique<data::TransactionBody>(*deserializer.getTransactionBody());
		return *this;
	}
	GradidoTransactionBuilder& GradidoTransactionBuilder::setSenderCommunity(const std::string& senderCommunity)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		mSenderCommunity = senderCommunity;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setRecipientCommunity(const std::string& recipientCommunity)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		mRecipientCommunity = recipientCommunity;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::sign(std::shared_ptr<KeyPairEd25519> keyPair)
	{
		if (isBodyBuildingState()) {
			switchBuildState();
		}
		for (auto& bodyByteSignatureMap : mBodyByteSignatureMaps) {
			if (!bodyByteSignatureMap.bodyBytes || bodyByteSignatureMap.bodyBytes->isEmpty()) {
				throw GradidoTransactionBuilderException("system error, missing body bytes, please check Builder Implementation.");
			}
			auto signature = make_shared<memory::Block>(keyPair->sign(*bodyByteSignatureMap.bodyBytes));
			bodyByteSignatureMap.signatureMap.push(SignaturePair(keyPair->getPublicKey(), signature));
		}
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setParentMessageId(memory::ConstBlockPtr paringMessageId)
	{
		mParingMessageId = paringMessageId;
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
	
	void GradidoTransactionBuilder::checkBuildState(BuildingState expectedState) const
	{
		if (mState != expectedState) {
			std::string message;
			if (expectedState == BuildingState::BUILDING_BODY) {
				message = "please don't call this function after a call to addSignaturePair or sign";
			}
			else if(expectedState == BuildingState::LOCAL) {
				message = "please don't call this function on cross group transactions";
			}
			else {
				message = "please don't call this function on local transactions";
			}
			throw GradidoTransactionWrongBuildingStateBuilderException(
				"incorrect building state for this action",
				enum_name(expectedState),
				enum_name(mState)
			);
		}
	}

	void GradidoTransactionBuilder::switchBuildState()
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		// the context use a refernce to TransactionBody, so I can change it afterwards 
		interaction::serialize::Context serializer(*mBody);
		mBodyByteSignatureMaps.push_back(BodyBytesSignatureMap());
		if (isCrossCommunityTransaction()) {
			mState = BuildingState::CROSS_COMMUNITY;
			// prepare outbound body
			mBody->mType = CrossGroupType::OUTBOUND;
			mBody->mOtherGroup = mRecipientCommunity;
			mBodyByteSignatureMaps[0].bodyBytes = serializer.run();

			// prepare inbound body
			mBody->mType = CrossGroupType::INBOUND;
			mBody->mOtherGroup = mSenderCommunity;
			mBodyByteSignatureMaps.push_back(BodyBytesSignatureMap());
			mBodyByteSignatureMaps[1].bodyBytes = serializer.run();
		}
		else {
			mState = BuildingState::LOCAL;
			mBody->mType = CrossGroupType::LOCAL;
			mBodyByteSignatureMaps[0].bodyBytes = serializer.run();
		}
	}


	// ------------------------- exception implement ------------------------- 
	GradidoTransactionWrongBuildingStateBuilderException::GradidoTransactionWrongBuildingStateBuilderException(
		const char* what,
		std::string_view expectedBuildState,
		std::string_view actualBuildState
	) : GradidoTransactionBuilderException(what),
		mExpectedBuildState(expectedBuildState),
		mActualBuildState(actualBuildState)
	{

	}

	std::string GradidoTransactionWrongBuildingStateBuilderException::getFullString() const
	{
		std::string mResult = what();
		mResult += ", expected: " + mExpectedBuildState;
		mResult += ", actual: " + mActualBuildState;
		return mResult;
	}
}
