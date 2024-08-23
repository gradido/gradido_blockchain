#include "gradido_blockchain/TransactionBodyBuilder.h"
#include "gradido_blockchain/const.h"

using namespace std;
using namespace memory;

namespace gradido {
	using namespace data;

	TransactionBodyBuilder::TransactionBodyBuilder()
		: mBody(make_unique<TransactionBody>(
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
			throw TransactionBodyBuilderException("missing specific transaction (GradidoTransfer, GradidoCreation or other)");
		}

		auto body = move(mBody);
		reset();
		return move(body);
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setDeferredTransfer(unique_ptr<GradidoDeferredTransfer> deferredTransfer)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mDeferredTransfer = move(deferredTransfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setCommunityFriendsUpdate(unique_ptr<CommunityFriendsUpdate> communityFriendsUpdate)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCommunityFriendsUpdate = std::move(communityFriendsUpdate);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setRegisterAddress(unique_ptr<RegisterAddress> registerAddress) 
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mRegisterAddress = std::move(registerAddress);

		mSpecificTransactionChoosen = true;
		return *this;
	}


	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionCreation(unique_ptr<GradidoCreation> creation) {
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}
		mBody->mCreation = std::move(creation);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionTransfer(unique_ptr<GradidoTransfer> transfer)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mTransfer = std::move(transfer);

		mSpecificTransactionChoosen = true;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setCommunityRoot(unique_ptr<CommunityRoot> communityRoot)
	{
		if (mSpecificTransactionChoosen) {
			throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
		}

		mBody->mCommunityRoot = std::move(communityRoot);

		mSpecificTransactionChoosen = true;
		return *this;
	}
}