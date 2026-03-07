#include "gradido_blockchain/interaction/calculateCreationSum/AbstractAlgoRole.h"
#include "gradido_blockchain/interaction/calculateCreationSum/Context.h"
#include "gradido_blockchain/interaction/calculateCreationSum/V01_ThreeMonthsAlgoRole.h"
#include "gradido_blockchain/interaction/calculateCreationSum/V02_TargetDateAlgoRole.h"

#include "date/date.h"
#include <magic_enum/magic_enum.hpp>

#include <chrono>

using namespace magic_enum;
using std::chrono::seconds, std::chrono::time_point_cast;

namespace gradido {
	namespace interaction {
		namespace calculateCreationSum {
			enum AlgoType : uint8_t {
				V01_Three_Months_Algo,
				V02_Target_Date_Algo
			};

			static AlgoType chooseAlgoType(Timepoint date)
			{
				auto secondsSinceEpoch = time_point_cast<seconds>(date).time_since_epoch().count();
				if (secondsSinceEpoch < 1588503608) {
					return AlgoType::V01_Three_Months_Algo;
				}
				else {
					return AlgoType::V02_Target_Date_Algo;
				}
			}

			Context::Context(Timepoint date, Timepoint targetDate, data::compact::PublicKeyIndex publicKey, uint64_t transactionNrMax/* = 0 */)
			{
				auto algoType = chooseAlgoType(date);
				switch (algoType) {
				case AlgoType::V01_Three_Months_Algo:
					mRole = std::make_unique<V01_ThreeMonthsAlgoRole>(date, targetDate, publicKey, transactionNrMax);
					break;
				case AlgoType::V02_Target_Date_Algo:
					mRole = std::make_unique<V02_TargetDateAlgoRole>(date, targetDate, publicKey, transactionNrMax);
					break;
				default:
					throw GradidoUnhandledEnum("Wrong algoType in calculate Creation sum", "AlgoType", enum_name(algoType).data());
				}
			}
			GradidoUnit calculateCreationSum(
				Timepoint date,
				Timepoint targetDate,
				data::compact::PublicKeyIndex publicKey,
				const blockchain::Abstract& blockchain,
				uint64_t transactionNrMax /* = 0 */
			)
			{
				auto algoType = chooseAlgoType(date);
				switch (algoType) {
				case AlgoType::V01_Three_Months_Algo:
					return v01_ThreeMonthsAlgo(timepointAsYearMonth(date), publicKey, blockchain, transactionNrMax);
				case AlgoType::V02_Target_Date_Algo:
					return v02_TargetDateAlgo(date, targetDate, publicKey, blockchain, transactionNrMax);
				default:
					throw GradidoUnhandledEnum("Wrong algoType in calculate Creation sum", "AlgoType", enum_name(algoType).data());
				}
			}

			GradidoUnit getLimit(Timepoint date)
			{
				auto algoType = chooseAlgoType(date);
				switch (algoType) {
				case AlgoType::V01_Three_Months_Algo: return GradidoUnit::fromGradidoCent(30000000);
				case AlgoType::V02_Target_Date_Algo: return GradidoUnit::fromGradidoCent(10000000);
				default: throw GradidoUnhandledEnum("Wrong algoType in calculate Creation sum", "AlgoType", enum_name(algoType).data());
				}
			}
		}
	}
}

