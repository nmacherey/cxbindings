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
    std::string gccvers;
#ifdef __GNUC__
    gccvers.Printf(_T("gcc %d.%d.%d"), __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

    std::string title = _("Exception");
    std::string err;
    err.Printf(_("An exception has been raised!\n\n"
                 "The application encountered an error at %s, on line %d.\n"
                 "The error message is:\n\n%s\n\n"
                 "CXBindings Version revision %d (%s, "
                 "build: %s %s)"),
               File.c_str(), Line, Message.c_str(),
               1123, gccvers.c_str(),
               __DATE__), __TIME__  ;

	if( safe )
		std::cerr <<  cxcU2C( title )  << " : " << cxcU2C( err ) << std::endl;
	else
		wxMessageBox( err , "CXBindings exception catched !") , wxICON_ERROR  ;
}

std::string CXBindingsException::GetErrorMessage()
{
	std::string gccvers;
#ifdef __GNUC__
    gccvers.Printf(_T("gcc %d.%d.%d"), __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

    std::string title = _("Exception");
    std::string err;
    err.Printf(_("An exception has been raised!\n\n"
                 "The application encountered an error at %s, on line %d.\n"
                 "The error message is:\n\n%s\n\n"
                 "CXBindings Version revision %d (%s, "
                 "build: %s %s)"),
               File.c_str(), Line, Message.c_str(),
               1123, gccvers.c_str(),
               __DATE__), __TIME__  ;
			   
	return err;
}
