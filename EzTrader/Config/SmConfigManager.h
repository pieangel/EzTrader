#pragma once
#include <string>
class SmConfigManager 
{
public:
	SmConfigManager();
	~SmConfigManager();

	static std::string GetApplicationPath();
};

