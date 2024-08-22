#include "gradido_blockchain/TransactionBodyBuilder.h"
#include "gradido_blockchain/const.h"

using namespace std;

namespace gradido {
	using namespace data;

	TransactionBodyBuilder::TransactionBodyBuilder()
		: mBody(make_unique<data::TransactionBody>(
			"", chrono::system_clock::now(), GRADIDO_TRANSACTION_BODY_V3_3_VERSION_STRING, CrossGroupType::LOCAL
		)), mSpecificTransactionChoosen(false)
	{
		mBody->mCreatedAt = Timestamp(chrono::system_clock::now());
	}

	TransactionBodyBuilder::~TransactionBodyBuilder()
	{
	}

	void TransactionBodyBuilder::reset()
	{
		mBody = make_unique<data::TransactionBody>("", chrono::system_clock::now(), GRADIDO_TRANSACTION_BODY_V3_3_VERSION_STRING, CrossGroupType::LOCAL);
		mSpecificTransactionChoosen = false;
	}

	std::unique_ptr<TransactionBody> TransactionBodyBuilder::build()
	{
		if (!mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("missing specific transaction (GradidoTransfer, GradidoCreation or other");
		}

		auto body = move(mBody);
		reset();
		return move(body);
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setDeferredTransfer(
		memory::ConstBlockPtr senderPubkey,
		const string& amountGddCent,
		const string& communityId,
		memory::ConstBlockPtr recipientPubkey,
		Timepoint timeout
	) {
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mDeferredTransfer = make_shared<GradidoDeferredTransfer>(
			GradidoTransfer(
				TransferAmount(
					senderPubkey,
					amountGddCent,
					communityId
				), recipientPubkey
			), timeout
		);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setDeferredTransfer(
		std::unique_ptr<data::GradidoTransfer> transactionTransfer,
		Timepoint timeout
	)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}
		mBody->mDeferredTransfer = make_shared<GradidoDeferredTransfer>(
			*transactionTransfer,
			timeout
		);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setDeferredTransfer(std::unique_ptr<data::GradidoDeferredTransfer> deferredTransfer)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mDeferredTransfer = std::move(deferredTransfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setCommunityFriendsUpdate(bool colorFusion)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCommunityFriendsUpdate = make_shared<CommunityFriendsUpdate>(colorFusion);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setCommunityFriendsUpdate(std::unique_ptr<data::CommunityFriendsUpdate> communityFriendsUpdate)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCommunityFriendsUpdate = std::move(communityFriendsUpdate);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setRegisterAddress(
		memory::ConstBlockPtr userPubkey,
		data::AddressType type,
		memory::ConstBlockPtr nameHash/* = nullptr*/,
		memory::ConstBlockPtr accountPubkey/* = nullptr*/
	) {
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mRegisterAddress = make_shared<RegisterAddress>(
			type, 1, userPubkey, nameHash, accountPubkey
		);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setRegisterAddress(std::unique_ptr<data::RegisterAddress> registerAddress) 
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mRegisterAddress = std::move(registerAddress);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionCreation(
		memory::ConstBlockPtr recipientPubkey,
		const std::string& amountGddCent,
		Timepoint targetDate
	) {
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		// const TransferAmount & _recipient, Timepoint _targetDate
		mBody->mCreation = make_shared<GradidoCreation>(
			TransferAmount(recipientPubkey, amountGddCent), targetDate
		);

		mSpecificTransactionChoosen = true;
		return *this;
	}
	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionCreation(
		const TransferAmount& recipient,
		Timepoint targetDate
	) {
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCreation = make_shared<GradidoCreation>(recipient, targetDate);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionCreation(std::unique_ptr<data::GradidoCreation> creation) {
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}
		mBody->mCreation = std::move(creation);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionTransfer(
		memory::ConstBlockPtr senderPubkey,
		const std::string& amountGddCent,
		const std::string& communityId,
		memory::ConstBlockPtr recipientPubkey
	) {
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mTransfer = make_shared<GradidoTransfer>(
			TransferAmount(
				senderPubkey,
				amountGddCent,
				communityId
			), recipientPubkey
		);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionTransfer(std::unique_ptr<data::GradidoTransfer> transfer)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mTransfer = std::move(transfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setCommunityRoot(
		memory::ConstBlockPtr pubkey,
		memory::ConstBlockPtr gmwPubkey,
		memory::ConstBlockPtr aufPubkey
	) {
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCommunityRoot = make_shared<CommunityRoot>(
			pubkey, gmwPubkey, aufPubkey
		);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setCommunityRoot(std::unique_ptr<data::CommunityRoot> communityRoot)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCommunityRoot = std::move(communityRoot);

		mSpecificTransactionChoosen = true;
		return *this;
	}
}