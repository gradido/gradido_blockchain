/*!
*
* \author: einhornimmond
*
* \date: 25.10.19
*
* \brief: Creation Transaction
*/
#ifndef GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_CREATION_INCLUDE
#define GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_CREATION_INCLUDE

#pragma warning(disable:4800)

#include "TransactionBase.h"
#include "proto/gradido/gradido_creation.pb.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include <chrono>

namespace model {
	namespace gradido {

		class GRADIDOBLOCKCHAIN_EXPORT TransactionCreation : public TransactionBase
		{
		public:
			TransactionCreation(const proto::gradido::GradidoCreation& protoCreation);
			~TransactionCreation();

			int prepare();
			//! TODO: check created sum in the last 3 month if 1.000 per month isn't exceed
			//! maybe ask node server and hope the answer came fast
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const ConfirmedTransaction* parentConfirmedTransaction = nullptr
			) const;

			bool validateTargetDate(uint64_t receivedSeconds) const;
			static mpfr_ptr calculateCreationSum(
				const std::string& address,
				int month,
				int year, 
				std::chrono::time_point<std::chrono::system_clock> received,
				IGradidoBlockchain* blockchain
			);
			// use only for transactions before Sun May 03 2020 11:00:08 GMT+0000!
			static mpfr_ptr calculateCreationSumLegacy(
				const std::string& address,
				std::chrono::time_point<std::chrono::system_clock> received,
				IGradidoBlockchain* blockchain
			);

			std::vector<MemoryBin*> getInvolvedAddresses() const;
			bool isInvolved(const std::string pubkeyString) const;
			const std::string& getCoinCommunityId() const;
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			const std::string& getAmount() const;
			MemoryBin* getRecipientPublicKey() const;
			const std::string& getRecipientPublicKeyString() const;

			std::chrono::time_point<std::chrono::system_clock> getTargetDate() const;

			std::string toDebugString() const;

			static unsigned getTargetDateReceivedDistanceMonth(std::chrono::time_point<std::chrono::system_clock> received);

			enum class CreationMaxAlgoVersion : short
			{
				v01_THREE_MONTHS_3000_GDD,
				v02_ONE_MONTH_1000_GDD_TARGET_DATE
			};
			static CreationMaxAlgoVersion getCorrectCreationMaxAlgo(std::chrono::time_point<std::chrono::system_clock> date);
		protected:
			const proto::gradido::GradidoCreation& mProtoCreation;
		};
	}
}


#endif //GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_CREATION_INCLUDE
