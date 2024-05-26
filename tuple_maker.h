
#pragma once

#include "format_string.h"

namespace scan
{
template<FormatString Format, std::size_t Index>
constexpr bool leads_to_specifier()
{
	return Format[Index] == '{' && Format[Index + 1] != '{' && Format[Index + 1] != '-';
}

template<FormatString Format, std::size_t Index>
constexpr bool leads_from_specifier()
{
	return Format[Index] == '}' && Format[Index - 1] != '}' && Format[Index - 1] != '-';
}

template<const FormatString Format, const std::size_t Index, bool InSpecifier>
struct TupleMaker;

template<const FormatString Format, const std::size_t Index>
struct TupleMaker<Format, Index, true>
{
	using recurse = TupleMaker<Format, Index - 1, leads_to_specifier<Format, Index>()>::type;
	using current = SwitchTypeOnSpecifier<Format[Index]>::type;
	using type = decltype(std::tuple_cat(std::declval<recurse>(), std::declval<current>()));
};

template<const FormatString Format, const std::size_t Index>
struct TupleMaker<Format, Index, false>
{
	using type = TupleMaker<Format, Index - 1, leads_from_specifier<Format, Index>()>::type;
};

template<const FormatString Format>
struct TupleMaker<Format, -1, false>
{
	using type = std::tuple<>;
};

template<FormatString Format>
constexpr TupleMaker<Format, Format.size() - 2, false>::type generate_tuple()
{
	return {};
}
}