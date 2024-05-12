
#pragma once

#include <string>

constexpr int count_formats(const std::string_view format)
{
	int count = 0;
	for (int i = 0; i < format.size(); ++i)
	{
		if (format[i] == '{' && format[i + 1] == '}')
		{
			++count;
			++i;
		}
	}
	return count;
}


template<typename DataType, int Index, typename... Args>
struct ProcessData;

template<int Index, typename... Args>
struct ProcessData<int, Index, Args...>
{
	void operator()(std::tuple<Args...>& tuple, const std::string_view data) const
	{
		std::get<Index>(tuple) = std::stoi(std::string(data));
	}
};

template<int Index, typename... Args>
struct ProcessData<float, Index, Args...>
{
	void operator()(std::tuple<Args...>& tuple, const std::string_view data) const
	{
		std::get<Index>(tuple) = std::stof(std::string(data));
	}
};

template<int Index, typename... Args>
struct ProcessData<double, Index, Args...>
{
	void operator()(std::tuple<Args...>& tuple, const std::string_view data) const
	{
		std::get<Index>(tuple) = std::stod(std::string(data));
	}
};

template<int Index, typename... Args>
struct ProcessData<std::string, Index, Args...>
{
	void operator()(std::tuple<Args...>& tuple, const std::string_view data) const
	{
		std::get<Index>(tuple) = std::string(data);
	}
};


template<int Index, int Max, typename... Args>
struct RecursiveFunction;

template<int Max, typename... Args>
struct RecursiveFunction<-1, Max, Args...>
{
	static void scan(std::tuple<Args...>& tuple, std::string_view& input, std::string_view& format) {}
};

template<int Index, int Max, typename... Args>
struct RecursiveFunction
{
	static void scan(std::tuple<Args...>& tuple, std::string_view& input, std::string_view& format)
	{
		if (format[0] != '{' || (format[0] == '{' && format[1] != '}'))
		{
			format.remove_prefix(1);
			input.remove_prefix(1);

			RecursiveFunction<Index, Max, Args...>::scan(tuple, input, format);
		}
		else
		{
			const std::size_t next = format.find('}') + 1;
			const std::string_view data = input.substr(0, input.find(format.at(next >= format.size() ? next - 1 : next)));

			using DataType = decltype(std::tuple_element_t<Max - Index - 1, std::tuple<Args...>>);
			ProcessData<DataType, Max - Index - 1, Args...>()(tuple, data);

			format.remove_prefix(2);
			if (data.size() + 1 <= input.size())
				input.remove_prefix(data.size());

			RecursiveFunction<Index - 1, Max, Args...>::scan(tuple, input, format);
		}
	}
};

template<int Num, typename... Args>
constexpr std::tuple<Args...> scan(std::string_view input, std::string_view format)
{
	std::tuple<Args...> tuple = {};
	RecursiveFunction<Num - 1, Num, Args...>::scan(tuple, input, format);
	return tuple;
}
