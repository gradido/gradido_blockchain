#include "gradido_blockchain/model/protopuf/GradidoDeferredTransfer.h"

namespace model {
	namespace protopuf {
		GradidoDeferredTransfer::GradidoDeferredTransfer(const GradidoDeferredTransferMessage& data)
			: mParent(data["transfer"_f].value()), mTimeout(data["timeout"_f].value())
		{

		}

		GradidoDeferredTransfer::GradidoDeferredTransfer(const GradidoTransfer& transfer, Timepoint date)
			: mParent(transfer), mTimeout(date)
		{

		}

	}
}