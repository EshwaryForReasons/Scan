
#pragma once

#include <string>
#include <tuple>

namespace scan
{
template<const char Specifier>
struct SwitchTypeOnSpecifier;

template<>
struct SwitchTypeOnSpecifier<'i'> { using type = std::tuple<int>; };

template<>
struct SwitchTypeOnSpecifier<'s'> { using type = std::tuple<std::string>; };

template<>
struct SwitchTypeOnSpecifier<'f'> { using type = std::tuple<float>; };

template<>
struct SwitchTypeOnSpecifier<'d'> { using type = std::tuple<double>; };
}