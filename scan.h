
#pragma once

#include "format_string.h"
#include "type_specifier_switch.h"
#include "data_processor.h"
#include "tuple_maker.h"

#include <string>
#include <tuple>
#include <ranges>
#include <vector>
#include <algorithm>
#include <array>

namespace scan
{
template<typename Tuple, std::size_t TupleIndex>
struct ArrayToTuple;

template<typename Tuple>
struct ArrayToTuple<Tuple, -1>
{
	static void array_to_tuple(Tuple& tuple, const std::array<std::string_view, std::tuple_size<Tuple>::value>& data) {}
};

template<typename Tuple, std::size_t TupleIndex>
struct ArrayToTuple
{
	static void array_to_tuple(Tuple& tuple, const std::array<std::string_view, std::tuple_size<Tuple>::value>& data)
	{
		ProcessData<Tuple, std::tuple_element<TupleIndex, Tuple>::type, TupleIndex>::process(tuple, data[TupleIndex]);
		ArrayToTuple<Tuple, TupleIndex - 1>::array_to_tuple(tuple, data);
	}
};


template<FormatString Format, std::size_t Index>
consteval bool leads_to_specifier_for_counting()
{
	return Format[Index] == '}' && Format[Index - 1] != '}';
}

template<const FormatString Format, const std::size_t FormatIndex, bool InSpecifier>
struct GetNumSpecifiers;

template<const FormatString Format, const std::size_t FormatIndex>
struct GetNumSpecifiers<Format, FormatIndex, true>
{
	static constexpr int get_num_specifiers()
	{
		return 1 + GetNumSpecifiers<Format, FormatIndex - 1, leads_to_specifier_for_counting<Format, FormatIndex - 1>()>::get_num_specifiers();
	}
};

template<const FormatString Format, const std::size_t FormatIndex>
struct GetNumSpecifiers<Format, FormatIndex, false>
{
	static constexpr int get_num_specifiers()
	{
		return 0 + GetNumSpecifiers<Format, FormatIndex - 1, leads_to_specifier_for_counting<Format, FormatIndex - 1>()>::get_num_specifiers();
	}
};

template<const FormatString Format>
struct GetNumSpecifiers<Format, 0, false>
{
	static constexpr int get_num_specifiers() {return 0;}
};


template<FormatString Format>
consteval int get_num_specifiers()
{
	return GetNumSpecifiers<Format, Format.size() - 1, false>::get_num_specifiers();
}

template<FormatString Format>
consteval bool is_specifier(const std::size_t i)
{
	const bool b_contains_boundaries = Format[i] == '{' || Format[i + 2] == '}';
	if (!b_contains_boundaries || i + 3 > Format.size())
		return false;

	char f = Format[i + 1];
	return Format[i + 1] == 'i'
		|| Format[i + 1] == 'f'
		|| Format[i + 1] == 'd'
		|| Format[i + 1] == 's'
		|| Format[i + 1] == '-';
}

template<FormatString Format, std::size_t NumSpecifiers>
consteval std::array<char, NumSpecifiers> get_specifiers_in_order()
{
	std::array<char, NumSpecifiers> specifiers_in_order;
	for (std::size_t fi = 0, i = 0; fi < Format.size(), i < NumSpecifiers; ++fi)
	{
		if (is_specifier<Format>(fi))
		{
			specifiers_in_order[i++] = Format[fi + 1];
		}
	}
	return specifiers_in_order;
}

template<FormatString Format, std::size_t NumSpecifiers>
consteval std::array<std::string_view, NumSpecifiers + 1> get_interim_data()
{
	std::array<std::string_view, NumSpecifiers + 1> interim_data;
	constexpr std::string_view format_view = Format.view();

	std::size_t start = 0;
	std::size_t fi = 0;
	for (std::size_t i = 0; fi < Format.size() - 2; ++fi)
	{
		if (is_specifier<Format>(fi))
		{
			interim_data[i++] = format_view.substr(start, fi - start);
			start = fi + 3;
			fi += 2;
		}
	}

	interim_data.back() = format_view.substr(start, fi - start);
	return interim_data;
}

template<FormatString Format>
constexpr auto scan(std::string_view data)
{
	auto tuple = generate_tuple<Format>();
	constexpr std::size_t tuple_size = std::tuple_size<decltype(tuple)>::value;

	//Including ignore specifiers
	constexpr auto num_specifiers = get_num_specifiers<Format>();
	constexpr auto specifiers_in_order = get_specifiers_in_order<Format, num_specifiers>();
	constexpr auto interim_data = get_interim_data<Format, num_specifiers>();

	std::array<std::tuple<std::size_t, std::size_t>, num_specifiers> data_coords;
	for (std::size_t i = 1; i < interim_data.size(); ++i)
	{
		const std::size_t previous_end = i > 1 ? std::get<1>(data_coords[i - 2]) : 0;
		const std::size_t begin = data.find(interim_data[i - 1], previous_end) + interim_data[i - 1].size();
		const std::size_t end = interim_data[i].empty() ? data.size() : data.find(interim_data[i], begin);
		data_coords[i - 1] = { begin, end };
	}

	std::array<std::string_view, tuple_size> data_views;
	for (std::size_t i = 0, num_ignored = 0; i < data_coords.size(); ++i, ++num_ignored)
	{
		const auto& [begin, end] = data_coords[i];
		if (specifiers_in_order[i] != '-')
			data_views[i - num_ignored--] = data.substr(begin, end - begin);
	}

	ArrayToTuple<decltype(tuple), tuple_size - 1>::array_to_tuple(tuple, data_views);
	return tuple;
}

template<FormatString Format>
constexpr auto scan(FormatStringTag<Format>, std::string_view data)
{
	return scan<Format>(data);
}
}