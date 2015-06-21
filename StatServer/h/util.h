#pragma once

#include <string>
#include <boost\asio\streambuf.hpp>

const std::string QuitCommand("QUIT");
const std::string DataCommand("DATA");
const std::string StatCommand("STAT");

std::string to_string(boost::asio::streambuf &buf) 
{
       std::ostringstream out;
       out << &buf;
       return out.str();
}

std::string toUpper(const std::string& s)
{
	std::string retStr(s);
	std::transform( s.begin(), s.end(), retStr.begin(), std::toupper );
	return retStr;
}

std::string removeEndLSymbol(const std::string& s)
{
	std::string retStr(s);
	char lastSymbol = retStr.back();
	if ( (lastSymbol == '\n') || (lastSymbol == '\0') )
		retStr.pop_back();
	return retStr;
}

bool equalCommands(const std::string& command1, const std::string& command2)
{
	std::string upCom1(toUpper(removeEndLSymbol(command1)));
	std::string upCom2(toUpper(removeEndLSymbol(command2)));
	return upCom1 == upCom2;
}