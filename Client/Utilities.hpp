#pragma once
#include <vector>
#include <string>
#include <functional>

namespace TNAP
{
	template<typename Type>
	inline const std::vector<Type> stringToVector(const std::string& argData, const std::string& argSeperator, const std::function<Type(const std::string&)>& argFunction, const size_t argSize = 0)
	{
		std::string data{ argData };
		std::vector<Type> vectorToReturn;
		vectorToReturn.reserve(argSize);
		size_t offset{ data.find(argSeperator) };
		while (std::string::npos != offset)
		{
			vectorToReturn.emplace_back(argFunction(data.substr(0, offset)));
			data.erase(0, offset + 1);
			offset = data.find(argSeperator);
		}
		if ("" != data)
			vectorToReturn.emplace_back(argFunction(data));
		return vectorToReturn;
	}
}