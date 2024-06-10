#ifndef __GRADIDO_BLOCKCHAIN_v3_3_DATA_PROTOCOL_H
#define __GRADIDO_BLOCKCHAIN_v3_3_DATA_PROTOCOL_H

#include "AddressType.h"
#include "CrossGroupType.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"
#include "gradido_blockchain/lib/DecayDecimal.h"

#include <array>
#include <list>

namespace gradido {
	namespace v3_3 {
		namespace data {

			//  ----------------  basic_types.proto -----------------------------------
			struct GRADIDOBLOCKCHAIN_EXPORT SignaturePair
			{
				// throw InvalidSizeException
				SignaturePair(memory::ConstBlockPtr pubkeyPtr, memory::ConstBlockPtr signaturePtr)
					: pubkey(pubkeyPtr), signature(signaturePtr) {}

				memory::ConstBlockPtr pubkey;
				memory::ConstBlockPtr signature;
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
				Timestamp() : seconds(0), nanos(0) {}
				Timestamp(const Timepoint& date);
				Timestamp(int64_t _seconds, int32_t _nanos): seconds(_seconds), nanos(_nanos) {}

				operator Timepoint() const {
					int64_t microseconds = seconds * static_cast<int64_t>(1e6) + nanos / static_cast<int64_t>(1e3);
					return std::chrono::system_clock::time_point(std::chrono::microseconds(microseconds));
				};
				bool operator==(const Timestamp& other) const { return seconds == other.seconds && nanos == other.nanos; }

				int64_t seconds;
				int32_t nanos;
			};

			struct GRADIDOBLOCKCHAIN_EXPORT TimestampSeconds
			{
			public:
				TimestampSeconds() : seconds(0) {}
				TimestampSeconds(const Timepoint& date);
				TimestampSeconds(int64_t _seconds) : seconds(_seconds) {}

				operator Timepoint() const {
					return std::chrono::system_clock::time_point(std::chrono::seconds{ seconds });
				};
				bool operator==(const TimestampSeconds& other) const { return seconds == other.seconds; }

				int64_t seconds;
			};

			struct GRADIDOBLOCKCHAIN_EXPORT TransferAmount
			{
				TransferAmount(
					memory::ConstBlockPtr pubkeyPtr,
					const std::string& amountString,
					const std::string& communityId = ""
				) : pubkey(pubkeyPtr), amount(amountString), communityId(communityId) {}

				memory::ConstBlockPtr pubkey;
				DecayDecimal amount;
				std::string communityId;
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
					memory::ConstBlockPtr pubkeyPtr,
					memory::ConstBlockPtr gmwPubkeyPtr,
					memory::ConstBlockPtr aufPubkeyPtr
				) : pubkey(pubkeyPtr), gmwPubkey(gmwPubkeyPtr), aufPubkey(aufPubkeyPtr) {};
				
				memory::ConstBlockPtr pubkey;
				memory::ConstBlockPtr gmwPubkey;
				memory::ConstBlockPtr aufPubkey;
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
				GradidoTransfer(const TransferAmount& _sender, memory::ConstBlockPtr recipientPtr)
					: sender(_sender), recipient(recipientPtr) {}

				TransferAmount sender;
				memory::ConstBlockPtr recipient;
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
					AddressType _addressType,
					uint32_t _derivationIndex = 1,
					memory::ConstBlockPtr userPubkeyPtr = nullptr,
					memory::ConstBlockPtr nameHashPtr = nullptr,
					memory::ConstBlockPtr accountPubkeyPtr = nullptr
				) : userPubkey(userPubkeyPtr), addressType(_addressType), nameHash(nameHashPtr),
					accountPubkey(accountPubkeyPtr), derivationIndex(_derivationIndex) {}

				memory::ConstBlockPtr	userPubkey;
				AddressType				addressType;
				memory::ConstBlockPtr	nameHash;
				memory::ConstBlockPtr   accountPubkey;
				uint32_t				derivationIndex;
			};

			// transaction_body.proto
			struct GRADIDOBLOCKCHAIN_EXPORT TransactionBody
			{
				TransactionBody(): type(CrossGroupType::LOCAL) {}
				TransactionBody(
					const std::string& _memo,
					Timepoint _createdAt,
					const std::string& _versionNumber,
					CrossGroupType _type = CrossGroupType::LOCAL,
					const std::string& _otherGroup = ""
				) : memo(_memo), createdAt(_createdAt), versionNumber(_versionNumber), type(_type), otherGroup(_otherGroup) {};

				inline bool isTransfer() const { return static_cast<bool>(transfer); }
				inline bool isCreation() const { return static_cast<bool>(creation); }
				inline bool isCommunityFriendsUpdate() const { return static_cast<bool>(communityFriendsUpdate); }
				inline bool isRegisterAddress() const { return static_cast<bool>(registerAddress); }
				inline bool isDeferredTransfer() const { return static_cast<bool>(deferredTransfer); }
				inline bool isCommunityRoot() const { return static_cast<bool>(communityRoot); }

				std::string								memo;
				Timestamp								createdAt;
				std::string								versionNumber;
				CrossGroupType							type;
				std::string								otherGroup;

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
				GradidoTransaction() {}
				GradidoTransaction(
					const SignatureMap& signatureMap,
					memory::ConstBlockPtr bodyBytes,
					memory::ConstBlockPtr parentMessageId = nullptr
				) : signatureMap(signatureMap), bodyBytes(bodyBytes), parentMessageId(parentMessageId) {}

				SignatureMap			signatureMap;
				memory::ConstBlockPtr	bodyBytes;
				memory::ConstBlockPtr	parentMessageId;
			};

			// confirmed_transaction.proto
			struct GRADIDOBLOCKCHAIN_EXPORT ConfirmedTransaction
			{
				ConfirmedTransaction(
					uint64_t id,
					std::unique_ptr<data::GradidoTransaction> gradidoTransaction,
					Timepoint confirmedAt,
					const std::string& versionNumber,
					memory::ConstBlockPtr runningHash,
					memory::ConstBlockPtr messageId,
					DecayDecimal accountBalance
				) : id(id),
					gradidoTransaction(std::move(gradidoTransaction)),
					confirmedAt(confirmedAt),
					versionNumber(versionNumber),
					runningHash(runningHash),
					messageId(messageId),
					accountBalance(accountBalance) {}

				memory::Block calculateRunningHash(std::shared_ptr<ConfirmedTransaction> previousConfirmedTransaction = nullptr);

				uint64_t                    				id;
				std::unique_ptr<data::GradidoTransaction>   gradidoTransaction;
				TimestampSeconds							confirmedAt;
				std::string   								versionNumber;
				memory::ConstBlockPtr 						runningHash;
				memory::ConstBlockPtr 						messageId;
				DecayDecimal   								accountBalance;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_DATA_PROTOCOL_H