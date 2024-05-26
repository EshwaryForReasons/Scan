
#pragma once

#include <string>

namespace scan
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
FormatStringTag<S> constexpr operator""_f() { return {}; }
}

using scan::operator""_f;