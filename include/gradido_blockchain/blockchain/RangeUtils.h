#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_RANGE_UTILS_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_RANGE_UTILS_H

#include "TransactionEntry.h"
#include "Filter.h"
#include "SearchDirection.h"
#include "FilterResult.h"
#include "FilterCriteria.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "magic_enum/magic_enum.hpp"

namespace gradido {
    namespace blockchain {
		/**
		 * @brief Iterates through a range of transaction entries and collects those matching the given filter.
		 *
		 * This templated helper function is used to search within a continuous range of blockchain transactions,
		 * such as those stored in memory, on disk, or retrieved remotely.
		 * The function applies the specified @p filter and @p toFilter criteria to each transaction entry,
		 * respecting pagination, search direction, and early stop conditions.
		 *
		 * The search direction is determined by @p filter.searchDirection and can be ascending (ASC)
		 * or descending (DESC). Iteration stops automatically if the filter requests it via
		 * FilterResult::STOP or if the pagination capacity is reached.
		 *
		 * Typical usage is within blockchain storage implementations (e.g. InMemory, FileBased, Remote)
		 * that need a common iteration and filtering mechanism.
		 *
		 * @tparam Iterator Iterator type pointing to a container of elements of type
		 *         <tt>std::pair<Key, std::shared_ptr<const TransactionEntry>></tt>.
		 * @param startIt Iterator pointing to the first element of the range (inclusive).
		 * @param endIt Iterator pointing past the last element of the range (exclusive).
		 * @param toFilter FilterCriteria indicating which aspects of each entry should be checked.
		 * @param filter Filter object defining direction, pagination, and match rules.
		 * @param communityId 
		 *
		 * @return TransactionEntries A list of transaction entries matching the given filter criteria.
		 *
		 * @throws GradidoUnhandledEnum If the search direction in @p filter is invalid or unsupported.
		 *
		 * @note This function stops iteration early if either:
		 *       - FilterResult::STOP is returned by the filter, or
		 *       - the pagination capacity defined by the filter is reached.
		 *
		 * @see Filter, FilterCriteria, TransactionEntry, SearchDirection, FilterResult
		 */
        template<typename Iterator>
        TransactionEntries findInRange(const Iterator& startIt, const Iterator& endIt, FilterCriteria toFilter, const Filter& filter) 
		{
            using EntryType = typename Iterator::value_type;
            static_assert(
				std::is_same_v<EntryType, std::pair<const typename EntryType::first_type, std::shared_ptr<const TransactionEntry>>>,
				"Iterator must point to a pair<Key, std::shared_ptr<const TransactionEntry>>"
            );

			TransactionEntries transactionEntries;
			if (startIt == endIt) {
				return transactionEntries;
			}
			int paginationCursor = 0;
			// return false if finished
			auto handleEntry = [&](const auto& entry) -> bool {
				auto result = filter.matches(entry, toFilter);
				if ((result & FilterResult::USE) == FilterResult::USE) {
					if (paginationCursor >= filter.pagination.skipEntriesCount()) {
						transactionEntries.push_back(entry);
						if (!filter.pagination.hasCapacityLeft(transactionEntries.size())) {
							return false; // no capacity left, caller gets his requested result count
						}
					}
					paginationCursor++;
				}
				if ((result & FilterResult::STOP) == FilterResult::STOP) {
					return false; // filter function has signaled stop
				}
				return true;
				};

			if (SearchDirection::ASC == filter.searchDirection) {
				for (auto it = startIt; it != endIt; ++it) {
					if (!handleEntry(it->second)) {
						return transactionEntries;
					}
				}
			}
			else if (SearchDirection::DESC == filter.searchDirection) {
				auto it = endIt;
				do {
					--it;
					if (!handleEntry(it->second)) {
						return transactionEntries;
					}
				} while (it != startIt);
			}
			else {
				throw GradidoUnhandledEnum("blockhain::inMemory::findAll", "SearchDirection", magic_enum::enum_name(filter.searchDirection).data());
			}
			return transactionEntries;
        }
    }
}

#endif // __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_RANGE_UTILS_H
