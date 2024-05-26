
#pragma once

#include <string>
#include <tuple>

namespace scan
{
template<typename Tuple, typename DataType, std::size_t Index>
struct ProcessData;

template<typename Tuple, std::size_t Index>
struct ProcessData<Tuple, int, Index>
{
	static void process(Tuple& tuple, const std::string_view data)
	{
		std::get<Index>(tuple) = std::stoi(std::string(data));
	}
};

template<typename Tuple, std::size_t Index>
struct ProcessData<Tuple, float, Index>
{
	static void process(Tuple& tuple, const std::string_view data)
	{
		std::get<Index>(tuple) = std::stof(std::string(data));
	}
};

template<typename Tuple, std::size_t Index>
struct ProcessData<Tuple, double, Index>
{
	static void process(Tuple& tuple, const std::string_view data)
	{
		std::get<Index>(tuple) = std::stod(std::string(data));
	}
};

template<typename Tuple, std::size_t Index>
struct ProcessData<Tuple, std::string, Index>
{
	static void process(Tuple& tuple, const std::string_view data)
	{
		std::get<Index>(tuple) = std::string(data);
	}
};
}