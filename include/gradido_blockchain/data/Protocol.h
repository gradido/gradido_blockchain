#ifndef __GRADIDO_BLOCKCHAIN_DATA_PROTOCOL_H
#define __GRADIDO_BLOCKCHAIN_DATA_PROTOCOL_H

#include "AddressType.h"
#include "TransactionType.h"
#include "CrossGroupType.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"
#include "gradido_blockchain/GradidoUnit.h"

#include <array>
#include <list>

namespace gradido {
	namespace data {

		//  ----------------  basic_types.proto -----------------------------------
		struct GRADIDOBLOCKCHAIN_EXPORT SignaturePair
		{
			// throw InvalidSizeException
			SignaturePair(memory::ConstBlockPtr pubkeyPtr, memory::ConstBlockPtr signaturePtr)
				: pubkey(pubkeyPtr), signature(signaturePtr) {}

			memory::ConstBlockPtr pubkey;
			memory::ConstBlockPtr signature;

			bool operator==(const SignaturePair& other) const {
				return pubkey->isTheSame(other.pubkey) && signature->isTheSame(other.signature);
			}
		};

		struct GRADIDOBLOCKCHAIN_EXPORT SignatureMap 
		{
			SignatureMap(size_t sizeHint = 1) { signaturePairs.reserve(sizeHint); }
			SignatureMap(SignaturePair firstSignaturePair, size_t sizeHint = 1)
				: SignatureMap(sizeHint) { push(firstSignaturePair); }

			//! attention! not thread safe
			void push(const SignaturePair& signaturePair);
			
			std::vector<SignaturePair> signaturePairs;
		};

		struct GRADIDOBLOCKCHAIN_EXPORT TimestampSeconds
		{
		public:
			TimestampSeconds() : seconds(0) {}
			TimestampSeconds(const Timepoint& date);
			TimestampSeconds(int64_t _seconds) : seconds(_seconds) {}

			operator Timepoint() const { return getAsTimepoint(); }
			Timepoint getAsTimepoint() const {
				return std::chrono::system_clock::time_point(std::chrono::seconds{ seconds });
			}
			bool operator==(const TimestampSeconds& other) const { return seconds == other.seconds; }
			bool operator!=(const TimestampSeconds& other) const { return seconds != other.seconds; }
			bool operator<(const TimestampSeconds& other) const { return seconds < other.seconds; }
			bool operator<=(const TimestampSeconds& other) const { return seconds <= other.seconds; }
			bool operator>(const TimestampSeconds& other) const { return seconds > other.seconds; }
			bool operator>=(const TimestampSeconds& other) const { return seconds >= other.seconds; }

			int64_t seconds;
		};

		struct GRADIDOBLOCKCHAIN_EXPORT Timestamp
		{
		public:
			Timestamp() : seconds(0), nanos(0) {}
			Timestamp(const Timepoint& date);
			Timestamp(int64_t _seconds, int32_t _nanos) : seconds(_seconds), nanos(_nanos) {}

			inline operator Timepoint() const { return getAsTimepoint(); }
			Timepoint getAsTimepoint() const {
				int64_t microseconds = seconds * static_cast<int64_t>(1e6) + nanos / static_cast<int64_t>(1e3);
				return std::chrono::system_clock::time_point(std::chrono::microseconds(microseconds));
			}
			inline bool operator==(const Timestamp& other) const { return seconds == other.seconds && nanos == other.nanos; }
			inline bool operator<(const Timestamp& other) const { return seconds < other.seconds || (seconds == other.seconds && nanos < other.nanos); }
			inline bool operator<(const TimestampSeconds& other) const { return seconds < other.seconds; }

			int64_t seconds;
			int32_t nanos;
		};

		struct GRADIDOBLOCKCHAIN_EXPORT TransferAmount
		{
			TransferAmount(
				memory::ConstBlockPtr pubkeyPtr,
				const std::string& amountString,
				const std::string& communityId = ""
			) : pubkey(pubkeyPtr), amount(amountString), communityId(communityId) {}

			inline bool operator==(const TransferAmount& other) const {
				return pubkey && pubkey->isTheSame(other.pubkey) && amount == other.amount && communityId == other.communityId;
			}

			memory::ConstBlockPtr pubkey;
			GradidoUnit amount;
			std::string communityId;				
		};

		//  ---------------- end   basic_types.proto   end -----------------------------------

		// community_friends_update.proto
		struct GRADIDOBLOCKCHAIN_EXPORT CommunityFriendsUpdate
		{
			CommunityFriendsUpdate(bool _colorFusion) : colorFusion(_colorFusion) {}

			inline bool operator==(const CommunityFriendsUpdate& other) const {
				return colorFusion == other.colorFusion;
			}

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
				
			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return { pubkey, gmwPubkey, aufPubkey }; }
			bool isInvolved(memory::ConstBlockPtr publicKey) const;
				
			memory::ConstBlockPtr pubkey;
			memory::ConstBlockPtr gmwPubkey;
			memory::ConstBlockPtr aufPubkey;				
		};

		// gradido_creation.proto
		struct GRADIDOBLOCKCHAIN_EXPORT GradidoCreation
		{
			GradidoCreation(const TransferAmount& _recipient, Timepoint _targetDate)
				: recipient(_recipient), targetDate(_targetDate) {}

			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return { recipient.pubkey }; }
			inline bool isInvolved(memory::ConstBlockPtr publicKey) const { return recipient.pubkey->isTheSame(publicKey); }

			TransferAmount recipient;
			TimestampSeconds targetDate;
		};

		// gradido_transfer.proto
		struct GRADIDOBLOCKCHAIN_EXPORT GradidoTransfer
		{
			GradidoTransfer(const TransferAmount& _sender, memory::ConstBlockPtr recipientPtr)
				: sender(_sender), recipient(recipientPtr) {}

			inline bool operator==(const GradidoTransfer& other) const {
				return sender == other.sender && recipient && recipient->isTheSame(other.recipient);
			}

			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return { sender.pubkey, recipient }; }
			bool isInvolved(memory::ConstBlockPtr publicKey) const;

			TransferAmount sender;
			memory::ConstBlockPtr recipient;		
		};

		struct GRADIDOBLOCKCHAIN_EXPORT GradidoDeferredTransfer
		{
			GradidoDeferredTransfer(const GradidoTransfer& _transfer, Timepoint _timeout)
				: transfer(_transfer), timeout(_timeout) {}

			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return transfer.getInvolvedAddresses(); }
			inline bool isInvolved(memory::ConstBlockPtr publicKey) const { return transfer.isInvolved(publicKey); }
			memory::ConstBlockPtr getSenderPublicKey() const { return transfer.sender.pubkey; }
			memory::ConstBlockPtr getRecipientPublicKey() const { return transfer.recipient; }

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

			inline bool operator==(const RegisterAddress& other) const {
				return
					userPubkey && userPubkey->isTheSame(other.userPubkey) &&
					addressType == other.addressType &&
					nameHash && nameHash->isTheSame(other.nameHash) &&
					accountPubkey && accountPubkey->isTheSame(other.accountPubkey) &&
					derivationIndex == other.derivationIndex
				;
			}

			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			bool isInvolved(memory::ConstBlockPtr publicKey) const;

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
			data::TransactionType getTransactionType() const;

			bool isPairing(const TransactionBody& other) const;
			bool isInvolved(memory::ConstBlockPtr publicKey) const;
			//! return nullptr, if transaction type doesn't have a transfer amount
			const TransferAmount* getTransferAmount() const;

			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;

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

		typedef std::shared_ptr<const TransactionBody> ConstTransactionBodyPtr;

		// gradido_transaction.proto
		struct GRADIDOBLOCKCHAIN_EXPORT GradidoTransaction
		{
			GradidoTransaction() {}
			GradidoTransaction(
				const SignatureMap& signatureMap,
				memory::ConstBlockPtr bodyBytes,
				memory::ConstBlockPtr paringMessageId = nullptr
			) : signatureMap(signatureMap), bodyBytes(bodyBytes), paringMessageId(paringMessageId) {}

			// copy constructor
			GradidoTransaction(const GradidoTransaction& other)
				: GradidoTransaction(other.signatureMap, other.bodyBytes, other.paringMessageId) {}

			SignatureMap			signatureMap;
			memory::ConstBlockPtr	bodyBytes;
			memory::ConstBlockPtr	paringMessageId;

			//! will deserialize just once and cache the result
			ConstTransactionBodyPtr getTransactionBody() const;
			//! will deserialize transactionBody if not cached
			bool isPairing(const GradidoTransaction& other) const;
			bool isInvolved(memory::ConstBlockPtr publicKey) const;
			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			//! will serialize just once and cache the result
			memory::ConstBlockPtr getSerializedTransaction() const;
			//! return hash for clearly identify the transaction
			//! normally the first signature, but if not exist return hash from bodyBytes
			memory::ConstBlockPtr getFingerprint() const;
		protected:
			mutable ConstTransactionBodyPtr mTransactionBody;
			mutable std::mutex mTransactionBodyMutex;
			mutable memory::ConstBlockPtr mSerializedTransaction;
			mutable std::mutex mSerializedTransactionMutex;
		};

		typedef std::shared_ptr<const GradidoTransaction> ConstGradidoTransactionPtr;

		// confirmed_transaction.proto
		struct GRADIDOBLOCKCHAIN_EXPORT ConfirmedTransaction
		{
			ConfirmedTransaction(
				uint64_t id,
				std::shared_ptr<const GradidoTransaction> gradidoTransaction,
				Timepoint confirmedAt,
				const std::string& versionNumber,
				memory::ConstBlockPtr runningHash,
				memory::ConstBlockPtr messageId,
				const std::string& accountBalanceString
			) : id(id),
				gradidoTransaction(std::move(gradidoTransaction)),
				confirmedAt(confirmedAt),
				versionNumber(versionNumber),
				runningHash(runningHash),
				messageId(messageId),
				accountBalance(accountBalanceString) {}

			memory::Block calculateRunningHash(std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction = nullptr) const;

			uint64_t                    				id;
			std::shared_ptr<const data::GradidoTransaction> gradidoTransaction;
			TimestampSeconds							confirmedAt;
			std::string   								versionNumber;
			memory::ConstBlockPtr 						runningHash;
			memory::ConstBlockPtr 						messageId;
			GradidoUnit   								accountBalance;
		};

		typedef std::shared_ptr<ConfirmedTransaction> ConfirmedTransactionPtr;
		typedef std::shared_ptr<const ConfirmedTransaction> ConstConfirmedTransactionPtr;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_PROTOCOL_H