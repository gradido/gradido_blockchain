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
		 * @brief Iterates over a given range in ascending or descending order.
		 *
		 * This function applies a user-provided callback to each element within the specified iterator range.
		 * Iteration order is determined by the given @p searchDirection.
		 *
		 * The callback is expected to have the signature:
		 * @code
		 * bool callback(const ValueType& value);
		 * @endcode
		 *
		 * If the callback returns `false`, iteration stops early.
		 *
		 * @tparam Iterator Type of the range iterator (e.g. vector<T>::iterator, map<K,V>::iterator, etc.)
		 * @tparam Callback Callable type (e.g. lambda, functor, or std::function) accepting the dereferenced element.
		 * @param startIt Iterator pointing to the start of the range.
		 * @param endIt Iterator pointing to the end of the range.
		 * @param searchDirection Determines whether the range is iterated in ascending (ASC) or descending (DESC) order.
		 * @param callback Function or lambda that processes each element. Returning `false` stops the iteration.
		 *
		 * @throws GradidoUnhandledEnum if an invalid @p searchDirection value is provided.
		 *
		 * @note The function does not return any value. Control returns once the iteration completes or stops early.
		 */
		template<typename Iterator, typename Callback>
		void iterateRangeInOrder(
			const Iterator& startIt,
			const Iterator& endIt,
			SearchDirection searchDirection,
			Callback callback
		) {
			if (startIt == endIt) {
				return;
			}

			if (SearchDirection::ASC == searchDirection) {
				for (auto it = startIt; it != endIt; ++it) {
					if (!callback(*it)) {
						return;
					}
				}
			}
			else if (SearchDirection::DESC == searchDirection) {
				auto rBegin = std::make_reverse_iterator(endIt);
				auto rEnd = std::make_reverse_iterator(startIt);
				for (auto it = rBegin; it != rEnd; ++it) {
					if (!callback(*it)) {
						return;
					}
				}
			}
			else {
				throw GradidoUnhandledEnum("blockhain::inMemory::findAll", "SearchDirection", magic_enum::enum_name(searchDirection).data());
			}
			return;
		}

		/**
		 * @brief Iterates through a range of elements and collects those matching a filter, supporting custom result types and containers.
		 *
		 * This templated utility provides a generic mechanism to iterate over a contiguous or associative range of elements
		 * (defined by `Iterator`) and collect results into a container of type `ResultContainer`. The actual filtering
		 * logic is provided by `filterFunction`, which examines each element and can produce a transformed result of type `EntityType`.
		 *
		 * The function respects:
		 * - Filter pagination (`filter.pagination`),
		 * - Search direction (`filter.searchDirection`),
		 * - Early stopping via `FilterResult::STOP`.
		 *
		 * This design allows using different source container types (maps, vectors, etc.) and collecting different result types
		 * (full entries, IDs, summaries, pointers, etc.) into any container supporting `push_back()`.
		 *
		 * @tparam Iterator The iterator type pointing to the source container elements.
		 * @tparam ResultContainer The container type that stores the filtered results (must support push_back).
		 * @tparam EntityType The type of the results stored in `ResultContainer`. Defaults to `ResultContainer::value_type`.
		 *
		 * @param startIt Iterator pointing to the first element of the range (inclusive).
		 * @param endIt Iterator pointing past the last element of the range (exclusive).
		 * @param filter Filter object defining search direction, pagination, and other criteria.
		 * @param filterFunction Function that examines each element and optionally produces a result.
		 *        Signature: `FilterResult(const Iterator& it, EntityType& entity)`.
		 *        - Return `FilterResult::USE` to include the element in the results.
		 *        - Return `FilterResult::STOP` to stop iteration early.
		 *        - You can fill `entity` with the transformed result to be stored in the result container.
		 *
		 * @return ResultContainer A container filled with all elements or transformed results that match the filter.
		 *
		 * @throws GradidoUnhandledEnum If `filter.searchDirection` is invalid or unsupported.
		 *
		 * @note This function automatically handles ascending and descending iteration using `filter.searchDirection`.
		 *       Pagination and skip counts are applied automatically.
		 *
		 * @see Filter, FilterResult, SearchDirection
		 */
		template<
			typename Iterator, 
			typename ResultContainer, 
			typename EntityType = typename ResultContainer::value_type,
			typename FilterFunc
		>
		ResultContainer iterateRangeInOrderCollectFiltered(
			const Iterator& startIt,
			const Iterator& endIt,
			const Filter& filter,
			FilterFunc filterFunction
		) {
			ResultContainer results;
			if (startIt == endIt) {
				return results;
			}
			int paginationCursor = 0;
			iterateRangeInOrder(startIt, endIt, filter.searchDirection,
				[&](const Iterator::value_type& it) -> bool  // return false if finished
				{
					EntityType entity{};
					auto filterResult = filterFunction(it, entity);// filter.matches(entry, toFilter);
					if ((filterResult & FilterResult::USE) == FilterResult::USE) {
						if (paginationCursor >= filter.pagination.skipEntriesCount()) {
							results.push_back(entity);
							if (!filter.pagination.hasCapacityLeft(results.size())) {
								return false; // no capacity left, caller gets his requested result count
							}
						}
						paginationCursor++;
					}
					if ((filterResult & FilterResult::STOP) == FilterResult::STOP) {
						return false; // filter function has signaled stop
					}
					return true; // go on
				}
			);
			return results;
		}

		/**
		 * @brief Collects TransactionEntry pointers from a range of blockchain entries that match the specified filter criteria.
		 *
		 * This is a convenience wrapper around the generic `iterateRangeInOrder` template.
		 * It iterates over a range of elements of type `std::pair<Key, std::shared_ptr<const TransactionEntry>>`
		 * and collects all entries that satisfy the given `FilterCriteria` and `Filter`.
		 *
		 * @tparam Iterator The iterator type pointing to elements of type
		 *         `std::pair<const Key, std::shared_ptr<const TransactionEntry>>`.
		 *
		 * @param startIt Iterator pointing to the first element of the range (inclusive).
		 * @param endIt Iterator pointing past the last element of the range (exclusive).
		 * @param toFilter FilterCriteria indicating which aspects of each entry should be checked.
		 * @param filter Filter object defining search direction, pagination, and other criteria.
		 *
		 * @return TransactionEntries A vector of `std::shared_ptr<const TransactionEntry>` containing all
		 *         entries that matched the filter.
		 *
		 * @note The function automatically respects:
		 *       - `filter.searchDirection` (ascending or descending),
		 *       - Pagination rules in `filter.pagination`,
		 *       - Early stop if `FilterResult::STOP` is returned by the filter.
		 *
		 * @see iterateRangeInOrder, Filter, FilterCriteria, TransactionEntry
		 */
		template<typename Iterator>
		TransactionEntries findInRange(const Iterator& startIt, const Iterator& endIt, FilterCriteria toFilter, const Filter& filter)
		{
			using EntryType = typename Iterator::value_type;
			static_assert(
				std::is_same_v<EntryType, std::pair<const typename EntryType::first_type, std::shared_ptr<const TransactionEntry>>>,
				"Iterator must point to a pair<Key, std::shared_ptr<const TransactionEntry>>"
				);

			return iterateRangeInOrderCollectFiltered<Iterator, TransactionEntries>(
					startIt, 
					endIt, 
					filter,
				[toFilter, filter](const Iterator::value_type& it, std::shared_ptr<const TransactionEntry>& entity) 
				{
					entity = it.second;
					return filter.matches(it.second, toFilter);
				}
			);
		}
	}
}

#endif // __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_RANGE_UTILS_H
