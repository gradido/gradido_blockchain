#ifndef __GRADIDO_BLOCKCHAIN_LIB_STRING_VIEW_COMPARE_H
#define __GRADIDO_BLOCKCHAIN_LIB_STRING_VIEW_COMPARE_H

#include <string>
#include <string_view>

// Comparison operator for std::set and std::map to compare std::string and std::string_view
struct StringViewCompare {
	using is_transparent = void;

	bool operator()(const std::string& lhs, const std::string& rhs) const {
		return lhs < rhs;
	}

	bool operator()(const std::string_view& lhs, const std::string& rhs) const {
		return lhs < rhs;
	}

	bool operator()(const std::string& lhs, const std::string_view& rhs) const {
		return lhs < rhs;
	}

	bool operator()(const std::string_view& lhs, const std::string_view& rhs) const {
		return lhs < rhs;
	}
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_STRING_VIEW_COMPARE_H