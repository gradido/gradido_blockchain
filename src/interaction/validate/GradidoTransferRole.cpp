#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/GradidoTransfer.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/interaction/validate/Error.h"
#include "gradido_blockchain/interaction/validate/ErrorType.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Options.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

#include <cassert>
#include <memory>
#include <string_view>

using namespace magic_enum;
using std::shared_ptr;
using std::string_view, std::to_string;

namespace gradido {
	using blockchain::Filter;
	using data::AddressType, data::ConfirmedTransaction, data::CrossGroupType, data::TransactionType,  data::GradidoTransfer;
	using data::compact::ConfirmedGradidoTx;
	namespace interaction::validate {

		GradidoTransferRole::GradidoTransferRole(shared_ptr<const GradidoTransfer> gradidoTransfer)
			: mGradidoTransfer(gradidoTransfer), mCrossGroupType(data::CrossGroupType::LOCAL)
		{
			assert(gradidoTransfer);
			// prepare for signature check
			mMinSignatureCount = 1;
			mRequiredSignPublicKeys.push_back(gradidoTransfer->getSender().getPublicKey());
		}

		void GradidoTransferRole::run(Type type, ContextData& c)
		{
			TransferAmountRole transferAmountRole(mGradidoTransfer->getSender());
			transferAmountRole.setCrossGroupType(mCrossGroupType);
			transferAmountRole.run(type, c);
			auto& sender = mGradidoTransfer->getSender();

			if ((type & Type::SINGLE) == Type::SINGLE)
			{
				validateEd25519PublicKey(mGradidoTransfer->getRecipient(), "recipient");

				if (mGradidoTransfer->getRecipient()->isTheSame(sender.getPublicKey())) {
					throw TransactionValidationException("sender and recipient are the same");
				}
			}

			if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
				assert(c.senderBlockchain);
				if (!c.senderPreviousConfirmedTransaction) {
					throw BlockchainOrderException("transfer transaction not allowed as first transaction on sender blockchain");
				}
				if (!c.recipientPreviousConfirmedTransaction) {
					throw BlockchainOrderException("transfer transaction not allowed as first transaction on recipient blockchain");
				}
				validateAccount(c);
			}

			if ((type & Type::PREVIOUS) == Type::PREVIOUS)
			{
				if (!c.senderPreviousConfirmedTransaction) {
					throw BlockchainOrderException("transfer transaction not allowed as first transaction on blockchain");
				}
			}

			if ((type & Type::PREVIOUS_BALANCE) == Type::PREVIOUS_BALANCE)
			{
				validatePrevious(*c.senderPreviousConfirmedTransaction, c.senderBlockchain);
			}
		}

		void GradidoTransferRole::validatePrevious(
			const ConfirmedTransaction& previousConfirmedTransaction,
			shared_ptr<blockchain::Abstract> blockchain
		) {
			assert(blockchain);
			assert(mConfirmedAt.getSeconds());
			calculateAccountBalance::Context c(blockchain);
			auto& sender = mGradidoTransfer->getSender();
			auto finalBalance = c.fromEnd(
				sender.getPublicKey(),
				mConfirmedAt, // calculate decay after last transaction balance until confirmation date
				sender.getCoinCommunityIdIndex(),
				previousConfirmedTransaction.getId() // calculate until this transaction nr
			);

			if (sender.getAmount() > finalBalance + GradidoUnit::fromGradidoCent(100)) {
				throw InsufficientBalanceException("not enough Gradido Balance for send coins", sender.getAmount(), finalBalance);
			}
		}

		void GradidoTransferRole::validateAccount(ContextData& c)
		{
			assert(c.senderBlockchain);
			assert(c.recipientBlockchain);
			Filter filter;
			filter.involvedPublicKey = mGradidoTransfer->getSender().getPublicKey();
			filter.maxTransactionNr = c.senderPreviousConfirmedTransaction->getId();

			// check if sender address was registered
			auto senderAddressType = c.senderBlockchain->getAddressType(filter);
			if (AddressType::NONE == senderAddressType) {
				throw WrongAddressTypeException(
					"sender address not registered",
					senderAddressType,
					mGradidoTransfer->getSender().getPublicKey(),
					c.senderBlockchain->getCommunityIdIndex()
				);
			}
			if (AddressType::DEFERRED_TRANSFER == senderAddressType) {
				throw WrongAddressTypeException(
					"sender address is deferred transfer, please use redeemDeferredTransferTransaction for that",
					senderAddressType,
					mGradidoTransfer->getSender().getPublicKey(),
					c.senderBlockchain->getCommunityIdIndex()
				);
			}

			// check if recipient address was registered
			filter.involvedPublicKey = mGradidoTransfer->getRecipient();
			filter.maxTransactionNr = c.recipientPreviousConfirmedTransaction->getId();
			auto recipientAddressType = c.recipientBlockchain->getAddressType(filter);
			if (AddressType::NONE == recipientAddressType) {
				throw WrongAddressTypeException(
					"recipient address not registered", 
					recipientAddressType, 
					mGradidoTransfer->getRecipient(),
					c.recipientBlockchain->getCommunityIdIndex()
				);
			}
			if (AddressType::DEFERRED_TRANSFER == recipientAddressType) {
				throw WrongAddressTypeException(
					"recipient cannot be a deferred transfer address",
					recipientAddressType,
					mGradidoTransfer->getRecipient(),
					c.recipientBlockchain->getCommunityIdIndex()
				);
			}
		}
	
		static Error validateSingleTransferCommon(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			if (!tx.isTransfer() && !tx.isDeferredTransfer() && !tx.isRedeemDeferredTransfer() && !tx.isTimeoutDeferredTransfer()) {
				throw GradidoNodeInvalidDataException("called validateSingleTransferCommon with wrong typed tx");
			}

			const auto& publicKeyIdDict = appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain()->getPublicKeyDictionary();
			if (!appContext.hasPublicKey(tx.getSender())) {
				return { .type = ErrorType::Invalid_Dictionary_Index, .message = "couldn't find sender public key in Dictionary" };
			}
			if (!appContext.hasPublicKey(tx.getRecipient())) {
				return { .type = ErrorType::Invalid_Dictionary_Index, .message = "couldn't find recipient public key in Dictionary" };
			}
			auto amount = tx.getAmount();
			if (amount < GradidoUnit::fromGradidoCent(100)) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "transfer amount to low, min 0.01 GDD (100 GDD Cent)",
					.actual = amount.toString(),
					.expected = ">= 100"
				};
			}
			if (tx.hasColdData()) {
				const auto& coldData = tx.coldData.get();
				if (!tx.isTimeoutDeferredTransfer()) {
					if (coldData->signatureMap.size() != 1) {
						return {
							.type = ErrorType::Invalid_Field,
							.message = "unexpected signature count",
							.actual = to_string(coldData->signatureMap.size()),
							.expected = "1"
						};
					}
					auto sender = tx.getSender();
					const auto& senderBlockchain = appContext.getCommunityContext(sender.communityIdIndex).getBlockchain();
					auto signerPublicKeyIndex = senderBlockchain->getPublicKeyDictionary().getIndexForData(coldData->signatureMap[0].first);
					if (signerPublicKeyIndex != sender.publicKeyIndex) {
						return {
							.type = ErrorType::Invalid_Field,
							.message = "transfer must be signed from sender public key"
						};
					}
				}
				else {
					if (coldData->signatureMap.size()) {
						return {
							.type = ErrorType::Field_Value_Conflict,
							.message = "timeout deferred transfer hasn't any signature"
						};
					}
				}
			}

			if (tx.isConfirmedTx()) {
				uint32_t coinCommunityIdIndex = tx.accountBalances[0].coinCommunityIdIndex;
				for (int i = 1; i < tx.accountBalanceCount; ++i) {
					if (tx.accountBalances[i].coinCommunityIdIndex != coinCommunityIdIndex) {
						return {
							.type = ErrorType::Field_Value_Conflict,
							.message = "coin community id on multiple account balances are different, coin exchange doesn't supported"
						};
					}
				}
			}
			return { .type = ErrorType::Success };
		}

		static Error validateSingleTransfer(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			auto result = validateSingleTransferCommon(tx, appContext, options);
			if (ErrorType::Success != result.type) {
				return result;
			}
			if (!tx.isTransfer()) {
				throw GradidoNodeInvalidDataException("called validateSingleTransfer with not transfer typed tx");
			}
			if (tx.isConfirmedTx()) {
				if (tx.txNr < 2) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "Transfer must be tx nr 2 at least to have community root and user registered first",
						.actual = to_string(tx.txNr),
						.expected = ">= 2"
					};
				}
				// Local community tx
				if (!tx.isCrossCommunityTx()) {
					if (tx.accountBalanceCount != 2) {
						return {
						.type = ErrorType::Invalid_Field,
						.message = "unexpected account balances, expect balance of sender and recipient on local transfer tx",
						.actual = to_string(tx.accountBalanceCount),
						.expected = "2"
						};
					}
				}
				// Cross community tx
				else {
					if (tx.accountBalanceCount != 1) {
						return {
							.type = ErrorType::Invalid_Field,
							.message = "unexpected account balances, expect balance of sender or recipient on cross community transfer tx",
							.actual = to_string(tx.accountBalanceCount),
							.expected = "1"
						};
					}
				}
			}
			return { .type = ErrorType::Success };
		}

		static Error validateSingleTransferRedeemDeferred(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			auto result = validateSingleTransferCommon(tx, appContext, options);
			if (ErrorType::Success != result.type) {
				return result;
			}
			if (!tx.isRedeemDeferredTransfer()) {
				throw GradidoNodeInvalidDataException("called validateSingleTransfer with not redeem deferred transfer typed tx");
			}
			if (tx.isConfirmedTx()) {
				if (tx.txNr < 2) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "Redeem Deferred Transfer must be tx nr 2 at least to have community root and user registered first",
						.actual = to_string(tx.txNr),
						.expected = ">= 2"
					};
				}
				// Local community tx
				if (!tx.isCrossCommunityTx()) {
					if (tx.accountBalanceCount < 2 || tx.accountBalanceCount > 3) {
						return {
						.type = ErrorType::Invalid_Field,
						.message = "unexpected account balances, expect balance of sender and recipient and maybe change on local redeem deferred transfer tx",
						.actual = to_string(tx.accountBalanceCount),
						.expected = "2|3"
						};
					}
				}
				// Cross community tx
				else if (CrossGroupType::INBOUND == tx.crossGroupType) {
					if (tx.accountBalanceCount != 1) {
						return {
							.type = ErrorType::Field_Value_Conflict,
							.message = "unexpected account balances, expect balance of recipient on inbound cross community redeem deferred transfer tx",
							.actual = to_string(tx.accountBalanceCount),
							.expected = "1"
						};
					}
				}
				else if (CrossGroupType::OUTBOUND == tx.crossGroupType) {
					if (tx.accountBalanceCount < 1 || tx.accountBalanceCount > 2) {
						return {
							.type = ErrorType::Field_Value_Conflict,
							.message = "unexpected account balances, expect balance of recipient and maybe change on outbound cross community redeem deferred transfer tx",
							.actual = to_string(tx.accountBalanceCount),
							.expected = "1|2"
						};
					}
				}
				else {
					return {
						.type = ErrorType::Field_Value_Conflict,
						.message = "Redeem Deferred Transfer cannot be Cross Community Type Cross"
					};
				}
			}
			return { .type = ErrorType::Success };
		}

		static Error validateSingleTransferDeferredTimeout(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			auto result = validateSingleTransferCommon(tx, appContext, options);
			if (ErrorType::Success != result.type) {
				return result;
			}
			if (!tx.isTimeoutDeferredTransfer()) {
				throw GradidoNodeInvalidDataException("called validateSingleTransferDeferredTimeout with not timeout deferred transfer typed tx");
			}
			if (tx.isConfirmedTx()) {
				if (tx.txNr < 5) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "Timeout Redeem Deferred Transfer must be tx nr 5 at least to have community root, user registered, receive transfer and deferred transfer first",
						.actual = to_string(tx.txNr),
						.expected = ">= 5"
					};
				}
				// Local community tx
				if (!tx.isCrossCommunityTx()) {
					if (tx.accountBalanceCount != 2) {
						return {
						.type = ErrorType::Invalid_Field,
						.message = "unexpected account balances, expect balance of sender (deferred address) and recipient (original sender) on local timeout deferred transfer tx",
						.actual = to_string(tx.accountBalanceCount),
						.expected = "2"
						};
					}
				}
				// Cross community tx
				else {
					return {
						.type = ErrorType::Field_Value_Conflict,
						.message = "Timeout Redeem Deferred Transfer cannot be a Cross Community Transaction"
					};
				}
			}
			return { .type = ErrorType::Success };
		}

		// founding transaction link
		static Error validateSingleTransferDeferred(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			auto result = validateSingleTransferCommon(tx, appContext, options);
			if (ErrorType::Success != result.type) {
				return result;
			}
			if (!tx.isDeferredTransfer()) {
				throw GradidoNodeInvalidDataException("called validateSingleTransferDeferred with not deferred transfer typed tx");
			}
			if (tx.isConfirmedTx()) {
				if (tx.txNr < 4) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "Deferred Transfer must be tx nr 4 at least to have community root, user registered and receive transfer first",
						.actual = to_string(tx.txNr),
						.expected = ">= 4"
					};
				}
				// Local community tx
				if (!tx.isCrossCommunityTx()) {
					if (tx.accountBalanceCount != 2) {
						return {
						.type = ErrorType::Invalid_Field,
						.message = "unexpected account balances, expect balance of sender and recipient (deferred tx address) on local deferred transfer tx",
						.actual = to_string(tx.accountBalanceCount),
						.expected = "2"
						};
					}
				}
				// Cross community tx
				else {
					return {
						.type = ErrorType::Field_Value_Conflict,
						.message = "Deferred Transfer cannot be a Cross Community Transaction"
					};
				}
			}
			return { .type = ErrorType::Success };
		}

		Error validateGradidoTransfer(
			const data::compact::ConfirmedGradidoTx& tx,
			const AppContext& appContext,
			Options options
		) {
			if (!appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain().get()) {
				return {
					.type = ErrorType::Missing_Blockchain,
					.message = "missing blockchain for community " + to_string(tx.txCommunityIdIndex)
				};
			}
			Error result;
			if ((Type::SINGLE & options.type) == Type::SINGLE) {
				switch (tx.transactionType) {
				case TransactionType::TRANSFER:
					result = validateSingleTransfer(tx, appContext, options);
					break;
				case TransactionType::DEFERRED_TRANSFER:
					result = validateSingleTransferDeferred(tx, appContext, options);
					break;
				case TransactionType::REDEEM_DEFERRED_TRANSFER:
					result = validateSingleTransferRedeemDeferred(tx, appContext, options);
					break;
				case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
					result = validateSingleTransferDeferredTimeout(tx, appContext, options);
					break;
				default:
					throw GradidoUnhandledEnum(
						"this isn't a transfer transaction type",
						"TransactionType",
						enum_name(tx.transactionType).data()
					);
				}
				if (ErrorType::Success != result.type) {
					return result;
				}
			}
			return {
				.type = ErrorType::Success
			};
		}
	}
}