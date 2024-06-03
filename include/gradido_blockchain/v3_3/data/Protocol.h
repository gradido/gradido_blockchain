#ifndef __GRADIDO_BLOCKCHAIN_v3_3_DATA_PROTOCOL_H
#define __GRADIDO_BLOCKCHAIN_v3_3_DATA_PROTOCOL_H

#include "AddressType.h"
#include "CrossGroupType.h"

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/data/PublicKey.h"
#include "gradido_blockchain/data/Signature.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"
#include "gradido_blockchain/lib/DecayDecimal.h"

#include <array>
#include <list>

namespace gradido {
	namespace common = data;
	namespace v3_3 {
		namespace data {

			//  ----------------  basic_types.proto -----------------------------------
			struct GRADIDOBLOCKCHAIN_EXPORT SignaturePair
			{
				// throw InvalidSizeException
				SignaturePair(memory::ConstMemoryBlockPtr pubkeyPtr, memory::ConstMemoryBlockPtr signaturePtr)
					: pubkey(pubkeyPtr), signature(signaturePtr) {}

				common::PublicKey pubkey;
				common::Signature signature;
			};

			struct GRADIDOBLOCKCHAIN_EXPORT SignatureMap 
			{
				SignatureMap() {}
				SignatureMap(SignaturePair firstSignaturePair)
					: signaturePairs({ firstSignaturePair }) {}

				inline void push(const SignaturePair& signaturePair) { signaturePairs.push_back(signaturePair); }
			
				std::list<SignaturePair> signaturePairs;
			};

			struct GRADIDOBLOCKCHAIN_EXPORT Timestamp
			{
			public:
				Timestamp(const Timepoint& date);
				Timestamp(int64_t _seconds, int32_t _nanos): seconds(_seconds), nanos(_nanos) {}

				operator Timepoint() const {
					int64_t microseconds = seconds * static_cast<int64_t>(1e6) + nanos / static_cast<int64_t>(1e3);
					return std::chrono::system_clock::time_point(std::chrono::microseconds(microseconds));
				};

				int64_t seconds;
				int32_t nanos;
			};

			struct GRADIDOBLOCKCHAIN_EXPORT TimestampSeconds
			{
			public:
				TimestampSeconds(const Timepoint& date);
				TimestampSeconds(int64_t _seconds) : seconds(_seconds) {}

				operator Timepoint() const {
					return std::chrono::system_clock::time_point(std::chrono::seconds{ seconds });
				};

				int64_t seconds;
			};

			struct GRADIDOBLOCKCHAIN_EXPORT TransferAmount
			{
				TransferAmount(
					memory::ConstMemoryBlockPtr recipientPubkeyPtr,
					const memory::StringCachedAlloc& amountString,
					const memory::StringCachedAlloc& communityId
				) : recipientPubkey(recipientPubkeyPtr), amount(amountString), communityId(communityId) {}

				common::PublicKey recipientPubkey;
				DecayDecimal amount;
				memory::StringCachedAlloc communityId;
			};

			//  ---------------- end   basic_types.proto   end -----------------------------------

			// community_friends_update.proto
			struct GRADIDOBLOCKCHAIN_EXPORT CommunityFriendsUpdate
			{
				CommunityFriendsUpdate(bool _colorFusion) : colorFusion(_colorFusion) {}

				bool colorFusion;
			};

			// community_root.proto
			struct GRADIDOBLOCKCHAIN_EXPORT CommunityRoot
			{
				// throw InvalidSizeException
				CommunityRoot(
					memory::ConstMemoryBlockPtr pubkeyPtr,
					memory::ConstMemoryBlockPtr gmwPubkeyPtr,
					memory::ConstMemoryBlockPtr aufPubkeyPtr
				) : pubkey(pubkeyPtr), gmwPubkey(gmwPubkeyPtr), aufPubkey(aufPubkeyPtr) {};
				
				common::PublicKey pubkey;
				common::PublicKey gmwPubkey;
				common::PublicKey aufPubkey;
			};

			// gradido_creation.proto
			struct GRADIDOBLOCKCHAIN_EXPORT GradidoCreation
			{
				GradidoCreation(const TransferAmount& _recipient, Timepoint _targetDate)
					: recipient(_recipient), targetDate(_targetDate) {}

				TransferAmount recipient;
				TimestampSeconds targetDate;
			};

			// gradido_transfer.proto
			struct GRADIDOBLOCKCHAIN_EXPORT GradidoTransfer
			{
				GradidoTransfer(const TransferAmount& _sender, memory::ConstMemoryBlockPtr recipientPtr)
					: sender(_sender), recipient(recipientPtr) {}

				TransferAmount sender;
				common::PublicKey recipient;
			};

			struct GRADIDOBLOCKCHAIN_EXPORT GradidoDeferredTransfer
			{
				GradidoDeferredTransfer(const GradidoTransfer& _transfer, Timepoint _timeout)
					: transfer(_transfer), timeout(_timeout) {}

				GradidoTransfer transfer;
				TimestampSeconds timeout;
			};

			// register_address.proto
			struct GRADIDOBLOCKCHAIN_EXPORT RegisterAddress
			{
				RegisterAddress(
					memory::ConstMemoryBlockPtr userPubkeyPtr,
					AddressType _addressType,
					uint32_t _derivationIndex = 1,
					memory::ConstMemoryBlockPtr nameHashPtr = nullptr,
					memory::ConstMemoryBlockPtr accountPubkeyPtr = nullptr
				) : userPubkey(userPubkeyPtr), addressType(_addressType), nameHash(nameHashPtr),
					accountPubkey(accountPubkeyPtr), derivationIndex(_derivationIndex) {}

				common::PublicKey			userPubkey;
				AddressType					addressType;
				memory::ConstMemoryBlockPtr	nameHash;
				common::PublicKey			accountPubkey;
				uint32_t					derivationIndex;
			};

			// transaction_body.proto
			struct GRADIDOBLOCKCHAIN_EXPORT TransactionBody
			{
				TransactionBody(
					const memory::StringCachedAlloc& _memo,
					Timepoint _createdAt,
					const memory::StringCachedAlloc& _versionNumber,
					CrossGroupType _type = CrossGroupType::LOCAL,
					const memory::StringCachedAlloc& _otherGroup = ""
				) : memo(_memo), createdAt(_createdAt), versionNumber(_versionNumber), type(_type), otherGroup(_otherGroup) {};

				inline bool isTransfer() const { return static_cast<bool>(transfer); }
				inline bool isCreation() const { return static_cast<bool>(creation); }
				inline bool isCommunityFriendsUpdate() const { return static_cast<bool>(communityFriendsUpdate); }
				inline bool isRegisterAddress() const { return static_cast<bool>(registerAddress); }
				inline bool isDeferredTransfer() const { return static_cast<bool>(deferredTransfer); }
				inline bool isCommunityRoot() const { return static_cast<bool>(communityRoot); }

				memory::StringCachedAlloc				memo;
				Timestamp								createdAt;
				memory::StringCachedAlloc				versionNumber;
				CrossGroupType							type;
				memory::StringCachedAlloc				otherGroup;

				std::shared_ptr<GradidoTransfer>        transfer;
				std::shared_ptr<GradidoCreation>        creation;
				std::shared_ptr<CommunityFriendsUpdate> communityFriendsUpdate;
				std::shared_ptr<RegisterAddress>        registerAddress;
				std::shared_ptr<GradidoDeferredTransfer> deferredTransfer;
				std::shared_ptr<CommunityRoot>          communityRoot;

			};

			// gradido_transaction.proto
			struct GRADIDOBLOCKCHAIN_EXPORT GradidoTransaction
			{
				GradidoTransaction(
					const SignatureMap& signatureMap,
					memory::ConstMemoryBlockPtr bodyBytes,
					memory::ConstMemoryBlockPtr parentMessageId = nullptr
				) : mSignatureMap(signatureMap), mBodyBytes(bodyBytes), mParentMessageId(parentMessageId) {}

				SignatureMap				mSignatureMap;
				memory::ConstMemoryBlockPtr	mBodyBytes;
				memory::ConstMemoryBlockPtr	mParentMessageId;
			};

			// confirmed_transaction.proto
			struct GRADIDOBLOCKCHAIN_EXPORT ConfirmedTransaction
			{
			public:
				ConfirmedTransaction(
					uint64_t id,
					const GradidoTransaction& gradidoTransaction,
					Timepoint confirmedAt,
					const memory::StringCachedAlloc& versionNumber,
					memory::ConstMemoryBlockPtr runningHash,
					memory::ConstMemoryBlockPtr messageId,
					const memory::StringCachedAlloc& accountBalance
				) : mId(id),
					mGradidoTransaction(gradidoTransaction),
					mConfirmedAt(confirmedAt),
					mVersionNumber(versionNumber),
					mRunningHash(runningHash),
					mMessageId(messageId),
					mAccountBalance(accountBalance) {}

				uint64_t                    mId;
				GradidoTransaction          mGradidoTransaction;
				TimestampSeconds            mConfirmedAt;
				memory::StringCachedAlloc   mVersionNumber;
				memory::ConstMemoryBlockPtr mRunningHash;
				memory::ConstMemoryBlockPtr mMessageId;
				memory::StringCachedAlloc   mAccountBalance;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_DATA_PROTOCOL_H