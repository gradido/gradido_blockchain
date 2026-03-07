#ifndef __GRADIDO_BLOCKCHAIN_TYPES_H
#define __GRADIDO_BLOCKCHAIN_TYPES_H

#include "gradido_blockchain/export.h"

#include <chrono>


typedef std::chrono::time_point<std::chrono::system_clock> Timepoint;
typedef std::chrono::system_clock::duration Duration;

namespace date {
  class year_month;
  class year_month_day;
}

GRADIDOBLOCKCHAIN_EXPORT date::year_month_day timepointAsYearMonthDay(Timepoint date);
GRADIDOBLOCKCHAIN_EXPORT date::year_month timepointAsYearMonth(Timepoint date);
GRADIDOBLOCKCHAIN_EXPORT Timepoint monthYearToTimepoint(const date::year_month& ym);

#endif