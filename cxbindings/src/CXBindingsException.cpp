/**
*   @file CXBindingsException.cpp
*   @brief CXBindingsExecption is the basic class for CXBindings's error handling unit.
*
*   Two macros are defined to help error handling in CXBindings:
*   CXB_THROW() and CXB_ASSERT().
*
*   @note CXB_ASSERT() does nothing in release mode. For debug mode, compile with -D__DEBUG__
*
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@version 0.0.1
*	@date 03-Jan-2009
*/
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <iostream>
#include <sstream>
#include <map>


 // for wxSafeShowMessage()


#include "CXBindingsGlobals.h"
#include "CXBindingsException.h"

CXBindingsException::CXBindingsException(const std::string& msg, const std::string& file, int line)
  : Message(msg),
    File(file),
    Line(line)
{}

CXBindingsException::~CXBindingsException()
{}


void CXBindingsException::ShowErrorMessage(bool safe)
{

    std::stringstream gccvers;
#ifdef __GNUC__
    gccvers << "gcc " << __GNUC__ <<__GNUC_MINOR__ << __GNUC_PATCHLEVEL__;
#endif

    std::string title = "Exception";
     std::stringstream err;
    err << "An exception has been raised!\n\n" <<
                 "The application encountered an error at " << File << ", on line " << Line << ".\n" <<
                 "The error message is:\n\n" << Message << " \n\n" <<
                 "CXBindings Version revision " << 1123 <<
                 "build: " << gccvers.str() << " " << __DATE__ << " " <<  __TIME__;

	std::cerr << err.str() << std::endl;
}

std::string CXBindingsException::GetErrorMessage()
{
	std::stringstream gccvers;
#ifdef __GNUC__
    gccvers << "gcc " << __GNUC__ <<__GNUC_MINOR__ << __GNUC_PATCHLEVEL__;
#endif

    std::string title = "Exception";
     std::stringstream err;
    err << "An exception has been raised!\n\n" <<
                 "The application encountered an error at " << File << ", on line " << Line << ".\n" <<
                 "The error message is:\n\n" << Message << " \n\n" <<
                 "CXBindings Version revision " << 1123 <<
                 "build: " << gccvers.str() << " " << __DATE__ << " " <<  __TIME__;
			   
	return err.str();
}
