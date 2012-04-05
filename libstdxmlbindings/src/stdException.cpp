
/**
*   @file stdException.cpp
*   @brief stdExecption is the basic class for std's error handling unit.
*
*   Two macros are defined to help error handling in std:
*   STD_THROW() and STD_ASSERT().
*
*   @note STD_ASSERT() does nothing in release mode. For debug mode, compile with -D__WXDEBUG__
*
*	@author Nicolas Macherey (nm@graymat.fr)
*	@version 0.0.1
*	@date 03-Jan-2009
*/
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <iostream>
#include <sstream>
#include <map>

#include "stdDefines.h"
#include "stdException.h"

stdException::stdException(const std::string& msg, const std::string& file, int line)
  : m_message(msg),
    m_file(file),
    m_line(line)
{}

stdException::~stdException()
{}


void stdException::ShowErrorMessage()
{
    std::cerr << "Exception : " << "An exception has been raised!" << std::endl << std::endl;
	std::cerr << "The library encountered an error in file : " << m_file << ", on line : " << m_line << "." << std::endl;
	std::cerr << "The error message is : " << std::endl << std::endl;
	std::cerr << m_message << std::endl;
	std::cerr << "Library build is : " << __DATE__ << " " << __TIME__ << std::endl;
}

std::string stdException::GetErrorMessage()
{
	std::stringstream strm;
	
	strm << "Exception : " << "An exception has been raised!" << std::endl << std::endl;
	strm << "The library encountered an error in file : " << m_file << ", on line : " << m_line << "." << std::endl;
	strm << "The error message is : " << std::endl << std::endl;
	strm << m_message << std::endl;
	strm << "Library build is : " << __DATE__ << " " << __TIME__ << std::endl;
	std::string ret;
	strm >> ret;
	
	return ret;
}		
		