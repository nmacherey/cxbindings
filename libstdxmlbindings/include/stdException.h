
/**
*   @file stdException.h
*   @brief stdException is the basic class for std's error handling unit.
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
#ifndef STD_EXCEPTION_H
#define STD_EXCEPTION_H

/**
*	@class stdException
*	@brief The base std exception object.
*
*	In the std FrameWork you can manage errors handled by the different elements and objects by using the stdExcpetion object.
*	All errors and exeptions must use as base exception class stdException.
*
*	@author Nicolas Macherey (nm@graymat.fr)
*	@version 0.0.1
*	@date 03-Jan-2009
*/
class stdException
{
public:
    stdException(const std::string& msg, const std::string& file, int line);
    virtual ~stdException();

    /**
    *   @brief Display exception error message.
    */
    void ShowErrorMessage();
	
	std::string GetErrorMessage();

    // public members
    std::string m_message; ///< The exception's error message.
    std::string m_file; ///< The file where the exception was raised.
    int m_line; ///< The line in the file where the exception was raised.
};

#define STD_THROW(message) throw stdException(message, __FILE__, __LINE__)

#ifndef _DEBUG
#define STD_ASSERT(expr)
#else
// In unix we use kill to terminate the application, that makes gdb
// keep it alive which makes debugging easier.
// (thanks go to an unknown author)
#ifdef WIN32
#define DIE() exit(1)
#else
#include <csignal>
#define DIE() kill(0, SIGTERM)
#endif

#define BUFLEN 2048

#define STD_ASSERT_MESSAGE(expr) \
		char msg[BUFLEN]; \
        sprintf( msg , "Assertion failed in %s at %s:%d.\n\n%s\0" , __PRETTY_FUNCTION__ , __FILE__ , __LINE__ , #expr );

// non-fatal assertion
#define STD_ASSERT_NONFATAL(expr) \
        if (!(expr)) \
        { \
            STD_ASSERT_MESSAGE(expr); \
            std::cerr << "Assertion error " << msg << std::endl; \
        }

// fatal assertion
#define STD_ASSERT(expr) \
        if (!(expr)) \
        { \
            STD_ASSERT_MESSAGE(expr); \
            std::cerr << "Assertion error " << msg << std::endl; \
            DIE(); \
        }
#endif
#endif
		