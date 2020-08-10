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

	// argResultingAmountOfStrings = Amount of strings to populate vector with, 
	// if 0 then then the string will be seperated using argSeperator as many times as possible
	// if 1 then a vector containing and identical string to the one passed in will be returned
	inline const std::vector<std::string> stringToStrings(const std::string& argData, const std::string& argSeperator, const size_t argResultingAmountOfStrings = 0)
	{
		std::string data{ argData };
		std::vector<std::string> vectorToReturn;

		vectorToReturn.reserve(argResultingAmountOfStrings);
		size_t offset{ data.find(argSeperator) };

		for (size_t i = 0; i < argResultingAmountOfStrings-1; i++)
		{
			if (std::string::npos != offset)
			{
				vectorToReturn.emplace_back(data.substr(0, offset));
				data.erase(0, offset + 1);
				offset = data.find(argSeperator);
			}
			else
				break;
		}
		if ("" != data)
			vectorToReturn.emplace_back(data);

		return vectorToReturn;
	}

	inline const std::string replaceSubStrings(const std::vector<std::string>& argTargetReplacements, const std::string& argReplaceString, const std::string argReplacement)
	{
		std::string newString{ argReplaceString };

		for (const std::string& replace : argTargetReplacements)
		{
			size_t index{ 0 };
			index = newString.find(replace);
			while (std::string::npos != index)
			{
				newString.replace(index, replace.size(), argReplacement);
				index = newString.find(replace, index + replace.size());
			}
		}

		return newString;
	}

	inline const std::string stringToLowerCase(const std::string& argString)
	{
		std::string newString{ argString };

		std::transform(newString.begin(), newString.end(), newString.begin(),
			[](unsigned char c)
			{ return std::tolower(c); });

		return newString;
	}
}