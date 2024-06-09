#include "gradido_blockchain/v3_3/TransactionBodyBuilder.h"

using namespace std;

namespace gradido {
	namespace v3_3 {

		using namespace data;

		TransactionBodyBuilder::TransactionBodyBuilder()
			: mBody(make_unique<data::TransactionBody>(
				"", chrono::system_clock::now(), GRADIDO_TRANSACTION_BODY_V3_3_VERSION_STRING, CrossGroupType::LOCAL
			)), mSpecificTransactionChoosen(false)
		{
			mBody->createdAt = Timestamp(chrono::system_clock::now());
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

			mBody->deferredTransfer = make_shared<GradidoDeferredTransfer>(
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

		TransactionBodyBuilder& TransactionBodyBuilder::setCommunityFriendsUpdate(bool colorFusion)
		{
			if (mSpecificTransactionChoosen) {
				throw TransactionBodyBuilderException("specific transaction already choosen, only one is possible!");
			}

			mBody->communityFriendsUpdate = make_shared<CommunityFriendsUpdate>(colorFusion);

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

			mBody->registerAddress = make_shared<RegisterAddress>(
				type, 1, userPubkey, nameHash, accountPubkey
			);

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
			mBody->creation = make_shared<GradidoCreation>(
				TransferAmount(recipientPubkey, amountGddCent), targetDate
			);

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

			mBody->transfer = make_shared<GradidoTransfer>(
				TransferAmount(
					senderPubkey,
					amountGddCent,
					communityId
				), recipientPubkey
			);

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

			mBody->communityRoot = make_shared<CommunityRoot>(
				pubkey, gmwPubkey, aufPubkey
			);

			mSpecificTransactionChoosen = true;
			return *this;
		}
	}
}