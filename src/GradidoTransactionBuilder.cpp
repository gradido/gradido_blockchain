#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/CommunityFriendsUpdate.h"
#include "gradido_blockchain/data/CommunityRoot.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/DurationSeconds.h"
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/data/GradidoCreation.h"
#include "gradido_blockchain/data/GradidoDeferredTransfer.h"
#include "gradido_blockchain/data/GradidoRedeemDeferredTransfer.h"
#include "gradido_blockchain/data/GradidoTimeoutDeferredTransfer.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/data/GradidoTransfer.h"
#include "gradido_blockchain/data/RegisterAddress.h"
#include "gradido_blockchain/data/SignaturePair.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/memory/Block.h"


#include "magic_enum/magic_enum.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <string_view>

using namespace magic_enum;
using std::chrono::system_clock;
using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;
using std::string;
using std::string_view;

using memory::Block;
using memory::ConstBlockPtr;

namespace gradido {
	using data::AddressType;
	using data::DurationSeconds;
	using data::CommunityFriendsUpdate;
	using data::CommunityRoot;
	using data::CrossGroupType;
	using data::EncryptedMemo;
	using data::GradidoCreation;
	using data::GradidoDeferredTransfer;
	using data::GradidoRedeemDeferredTransfer;
	using data::GradidoTimeoutDeferredTransfer;
	using data::GradidoTransaction;
	using data::GradidoTransfer;
	using data::RegisterAddress;
	using data::SignaturePair;
	using data::Timestamp;
	using data::TransactionBody;
	using data::TransferAmount;

	GradidoTransactionBuilder::GradidoTransactionBuilder() 
	  : mState(BuildingState::BUILDING_BODY), 
		mBody(make_unique<TransactionBody>(
			system_clock::now(), GRADIDO_TRANSACTION_BODY_VERSION_STRING, CrossGroupType::LOCAL
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
		mBody = make_unique<data::TransactionBody>(system_clock::now(), GRADIDO_TRANSACTION_BODY_VERSION_STRING, CrossGroupType::LOCAL);
		mSenderCommunity.clear();
		mRecipientCommunity.clear();
		mBodyByteSignatureMaps.clear();
		mSpecificTransactionChoosen = false;
	}
	unique_ptr<GradidoTransaction> GradidoTransactionBuilder::build()
	{
		checkBuildState(BuildingState::LOCAL);
		assert(mBodyByteSignatureMaps.size());
		auto result = make_unique<GradidoTransaction>(
			mBodyByteSignatureMaps[0].signatureMap,
			mBodyByteSignatureMaps[0].bodyBytes,
			mParingMessageId
		);
		reset();
		return std::move(result);
	}

	unique_ptr<GradidoTransaction> GradidoTransactionBuilder::buildOutbound()
	{
		checkBuildState(BuildingState::CROSS_COMMUNITY);
		assert(mBodyByteSignatureMaps.size());
		auto result = make_unique<GradidoTransaction>(
			mBodyByteSignatureMaps[0].signatureMap,
			mBodyByteSignatureMaps[0].bodyBytes,
			mParingMessageId
		);
		reset();
		return std::move(result);
	}

	unique_ptr<GradidoTransaction> GradidoTransactionBuilder::buildInbound()
	{
		checkBuildState(BuildingState::CROSS_COMMUNITY);
		assert(mBodyByteSignatureMaps.size() > 1);
		if(!mParingMessageId || mParingMessageId->isEmpty()) {
			throw GradidoTransactionBuilderException("missing paring message id from outbound transaction for inbound transaction");
		}
		auto result = make_unique<GradidoTransaction>(
			mBodyByteSignatureMaps[1].signatureMap,
			mBodyByteSignatureMaps[1].bodyBytes,
			mParingMessageId
		);
		reset();
		return std::move(result);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setDeferredTransfer(
		GradidoTransfer transactionTransfer,
		DurationSeconds timeoutDuration
	) {
		checkBuildState(BuildingState::BUILDING_BODY);
		return setDeferredTransfer(
			std::make_unique<GradidoDeferredTransfer>(
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

		mBody->mDeferredTransfer = std::move(deferredTransfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setCommunityFriendsUpdate(bool colorFusion)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setCommunityFriendsUpdate(
			std::make_unique<CommunityFriendsUpdate>(
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
		ConstBlockPtr userPubkey,
		AddressType type,
		ConstBlockPtr nameHash/* = nullptr*/,
		ConstBlockPtr accountPubkey/* = nullptr*/
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
	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionCreation(const TransferAmount& recipient, Timepoint targetDate)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setTransactionCreation(
			make_unique<GradidoCreation>(
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
		const TransferAmount& sender,
		ConstBlockPtr recipientPubkey
	)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setTransactionTransfer(
			make_unique<GradidoTransfer>(
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
		ConstBlockPtr pubkey,
		ConstBlockPtr gmwPubkey,
		ConstBlockPtr aufPubkey
	) {
		checkBuildState(BuildingState::BUILDING_BODY);
		return setCommunityRoot(
			make_unique<CommunityRoot>(
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
			make_unique<GradidoRedeemDeferredTransfer>(
				deferredTransferTransactionNr,
				transactionTransfer
			)
		);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setRedeemDeferredTransfer(unique_ptr<GradidoRedeemDeferredTransfer> redeemDeferredTransfer)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mRedeemDeferredTransfer = std::move(redeemDeferredTransfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTimeoutDeferredTransfer(uint64_t deferredTransferTransactionNr)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		return setTimeoutDeferredTransfer(
			make_unique<GradidoTimeoutDeferredTransfer>(
				deferredTransferTransactionNr
			)
		);
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTimeoutDeferredTransfer(unique_ptr<GradidoTimeoutDeferredTransfer> timeoutDeferredTransfer)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		if (mSpecificTransactionChoosen) {
			throw GradidoTransactionBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mTimeoutDeferredTransfer = std::move(timeoutDeferredTransfer);
		// special case, because TimeoutDeferredTransfer didn't need singnatures
		switchBuildState();
		mSpecificTransactionChoosen = true;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setCreatedAt(Timepoint createdAt) {
		checkBuildState(BuildingState::BUILDING_BODY);
		mBody->mCreatedAt = Timestamp(createdAt);
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::addMemo(const EncryptedMemo& memo) {
		checkBuildState(BuildingState::BUILDING_BODY);
		mBody->mMemos.push_back(memo);
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setVersionNumber(string_view versionNumber) {
		checkBuildState(BuildingState::BUILDING_BODY);
		mBody->mVersionNumber = versionNumber;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionBody(unique_ptr<TransactionBody> body)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		mBody = std::move(body);
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setTransactionBody(ConstBlockPtr bodyBytes)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		interaction::deserialize::Context deserializer(bodyBytes, gradido::interaction::deserialize::Type::TRANSACTION_BODY);
		deserializer.run();
		if (!deserializer.isTransactionBody()) {
			throw GradidoTransactionBuilderException("cannot deserialize TransactionBody");
		}
		mBody = make_unique<data::TransactionBody>(*deserializer.getTransactionBody());
		return *this;
	}
	GradidoTransactionBuilder& GradidoTransactionBuilder::setSenderCommunity(const string& senderCommunity)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		mSenderCommunity = senderCommunity;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setRecipientCommunity(const string& recipientCommunity)
	{
		checkBuildState(BuildingState::BUILDING_BODY);
		mRecipientCommunity = recipientCommunity;
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::sign(shared_ptr<KeyPairEd25519> keyPair)
	{
		if (isBodyBuildingState()) {
			switchBuildState();
		}
		for (auto& bodyByteSignatureMap : mBodyByteSignatureMaps) {
			if (!bodyByteSignatureMap.bodyBytes || bodyByteSignatureMap.bodyBytes->isEmpty()) {
				throw GradidoTransactionBuilderException("system error, missing body bytes, please check Builder Implementation.");
			}
			auto signature = make_shared<Block>(keyPair->sign(*bodyByteSignatureMap.bodyBytes));
			bodyByteSignatureMap.signatureMap.push(SignaturePair(keyPair->getPublicKey(), signature));
		}
		return *this;
	}

	GradidoTransactionBuilder& GradidoTransactionBuilder::setParentMessageId(ConstBlockPtr paringMessageId)
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
		string_view expectedBuildState,
		string_view actualBuildState
	) : GradidoTransactionBuilderException(what),
		mExpectedBuildState(expectedBuildState),
		mActualBuildState(actualBuildState)
	{

	}

	std::string GradidoTransactionWrongBuildingStateBuilderException::getFullString() const
	{
		string mResult = what();
		mResult += ", expected: " + mExpectedBuildState;
		mResult += ", actual: " + mActualBuildState;
		return mResult;
	}
}
