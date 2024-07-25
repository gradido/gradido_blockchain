
#include "gradido_blockchain/model/protopuf/GradidoCreation.h"

namespace model {
	namespace protopuf {
		GradidoCreation::GradidoCreation(const GradidoCreationMessage& data)
			: mRecipient(data["recipient"_f].value()), mTargetDate(data["target_date"_f].value())
		{

		}

		GradidoCreation::GradidoCreation(const TransferAmount& recipient, Timepoint targetDate)
			: mRecipient(recipient), mTargetDate(targetDate)
		{

		}
	}
}
// 