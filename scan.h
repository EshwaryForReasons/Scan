
#include <string>
#include <tuple>
#include <print>

namespace Scan
{
template <const std::size_t N>
class FormatString
{
public:

	constexpr FormatString() {}
	constexpr FormatString(const char(&str)[N]);

	constexpr std::string_view view() const;
	constexpr std::size_t size() const;
	constexpr char operator[](const std::size_t i) const;
	constexpr std::size_t find(const char c) const;
	constexpr std::size_t find(const char c, const std::size_t offset) const;

	char value[N]{};
};

template <const std::size_t N>
constexpr FormatString<N>::FormatString(const char(&str)[N])
{
	std::copy_n(str, N, value);
}

template <const std::size_t N>
constexpr char FormatString<N>::operator[](const std::size_t i) const
{
	return value[i];
}

template <const std::size_t N>
constexpr std::string_view FormatString<N>::view() const
{
	return std::string_view(value, value + N - 1);
}

template <const std::size_t N>
constexpr std::size_t FormatString<N>::size() const
{
	return N;
}

template <const std::size_t N>
constexpr std::size_t FormatString<N>::find(const char c) const
{
	return view().find(c);
}

template <const std::size_t N>
constexpr std::size_t FormatString<N>::find(const char c, const std::size_t offset) const
{
	return view().find(c, offset);
}


template <FormatString>
class FormatStringTag {};

template <FormatString S>
FormatStringTag<S> constexpr operator""_c() { return {}; }


#define DEFINE_SWITCH_TYPE_ON_SPECIFIER(Specifier, Type)						\
template<>																		\
struct SwitchTypeOnSpecifier<Specifier> { using type = std::tuple<Type>; };

template<const char Specifier>
struct SwitchTypeOnSpecifier;

DEFINE_SWITCH_TYPE_ON_SPECIFIER('i', int);
DEFINE_SWITCH_TYPE_ON_SPECIFIER('s', std::string);
DEFINE_SWITCH_TYPE_ON_SPECIFIER('f', float);
DEFINE_SWITCH_TYPE_ON_SPECIFIER('d', double);


template<const FormatString Format, const std::size_t Index, bool InSpecifier>
struct TupleMaker;

template<const FormatString Format, const std::size_t Index>
struct TupleMaker<Format, Index, true>
{
	using recurse = TupleMaker<Format, Index - 1, Format[Index] == '{'>::type;
	using current = SwitchTypeOnSpecifier<Format[Index]>::type;
	using type = decltype(std::tuple_cat(std::declval<recurse>(), std::declval<current>()));
};

template<const FormatString Format, const std::size_t Index>
struct TupleMaker<Format, Index, false>
{
	using recurse = TupleMaker<Format, Index - 1, Format[Index] == '}'>::type;
	using type = recurse;
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


#define DEFINE_PROCESS_DATA_ON_TYPE(Type, ProcessFunc)							\
template<int Index, typename Tuple>												\
struct ProcessData<Type, Index, Tuple>											\
{																				\
	void operator()(Tuple& tuple, const std::string_view data) const			\
	{																			\
		std::get<Index>(tuple) = ProcessFunc(std::string(data));				\
	}																			\
};

template<typename DataType, int Index, typename Tuple>
struct ProcessData;

DEFINE_PROCESS_DATA_ON_TYPE(int, std::stoi);
DEFINE_PROCESS_DATA_ON_TYPE(float, std::stof);
DEFINE_PROCESS_DATA_ON_TYPE(double, std::stod);
DEFINE_PROCESS_DATA_ON_TYPE(std::string, );


template<FormatString Format, typename Tuple, std::size_t TupleIndex, std::size_t FormatIndex>
struct RecursiveFunction;

template<FormatString Format, typename Tuple>
struct RecursiveFunction<Format, Tuple, -1, -1>
{
	static void scan(Tuple& tuple, std::string_view data) {}
};

template<FormatString Format, typename Tuple, std::size_t TupleIndex>
struct RecursiveFunction<Format, Tuple, TupleIndex, -1>
{
	static void scan(Tuple& tuple, std::string_view data) {}
};

template<FormatString Format, typename Tuple, std::size_t FormatIndex>
struct RecursiveFunction<Format, Tuple, -1, FormatIndex>
{
	static void scan(Tuple& tuple, std::string_view data) {}
};

template<FormatString Format, typename Tuple, std::size_t TupleIndex, std::size_t FormatIndex>
struct RecursiveFunction
{
	static void scan(Tuple& tuple, std::string_view input)
	{
		constexpr const std::size_t real_format_index = Format.size() - FormatIndex;
		constexpr const std::size_t next_format_index = Format.size() - (real_format_index + 1);
		const std::string_view fmv = Format.view().substr(real_format_index);
		if constexpr (Format[real_format_index] == '{' && Format[real_format_index + 2] == '}')
		{
			constexpr std::size_t next = Format.find('}', real_format_index) + 1;
			char next_or_final_ch = Format[real_format_index + 3];
			const std::string_view data = input.substr(0, input.find(next_or_final_ch));

			constexpr std::size_t tuple_size = std::tuple_size<Tuple>::value;
			using DataType = std::tuple_element<tuple_size - TupleIndex - 1, Tuple>::type;
			ProcessData<DataType, tuple_size - TupleIndex - 1, Tuple>()(tuple, data);

			input.remove_prefix(data.size());
			RecursiveFunction<Format, Tuple, TupleIndex - 1, next_format_index - 2>::scan(tuple, input);
		}
		else
		{
			input.remove_prefix(1);
			RecursiveFunction<Format, Tuple, TupleIndex, next_format_index>::scan(tuple, input);
		}
	}
};


template<FormatString Format>
constexpr TupleMaker<Format, Format.size() - 2, false>::type scan(FormatStringTag<Format>, std::string_view data)
{
	auto tuple = generate_tuple<Format>();
	constexpr std::size_t tuple_last_index = std::tuple_size<decltype(tuple)>::value - 1;
	RecursiveFunction<Format, decltype(tuple), tuple_last_index, Format.size()>::scan(tuple, data);
	return tuple;
}
}