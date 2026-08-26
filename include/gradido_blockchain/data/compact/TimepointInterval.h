#ifndef GRADIDO_BLOCKCHAIN_DATA_COMPACT_TIMEPOINT_INTERVAL_H
#define GRADIDO_BLOCKCHAIN_DATA_COMPACT_TIMEPOINT_INTERVAL_H

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/lib/TimepointInterval.h"

#include "date/date.h"

namespace gradido::data::compact {
  struct TimepointInterval
  {
    date::month startMonth;
    date::month endMonth;
    date::year startYear;
    date::year endYear;
    operator ::TimepointInterval() const {
      return { 
        DataTypeConverter::monthYearToTimepoint({startYear, startMonth}), 
        DataTypeConverter::monthYearToTimepoint({endYear, endMonth})
    }
  };
}

#endif // GRADIDO_BLOCKCHAIN_DATA_COMPACT_TIMEPOINT_INTERVAL_H