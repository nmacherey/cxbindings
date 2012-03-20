/**
*   @file CXBindingsException.h
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
#ifndef CXBindingsException_h_
#define CXBindingsException_h_

/**
*	@class CXBindingsException
*	@brief The base CXBindings exception object.
*
*	In the CXBindings FrameWork you can manage errors handled by the different elements and objects by using the CXBindingsExcpetion object.
*	All errors and exeptions must use as base exception class CXBindingsException.
*
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@version 0.0.1
*	@date 03-Jan-2009
*/
class CXBindingsException
{
public:
    CXBindingsException(const std::string& msg, const std::string& file, int line);
    virtual ~CXBindingsException();

    /**
    *   @brief Display exception error message.
    *   @param safe If true, wxSafeShowMessage will be used to display the error,
    *             else a normal message box will be used.
    */
    void ShowErrorMessage(bool safe = true);
	
	std::string GetErrorMessage();

    // public members
    std::string Message; ///< The exception's error message.
    std::string File; ///< The file where the exception was raised.
    int Line; ///< The line in the file where the exception was raised.
};

#if USE_UNICODE
#define CXB_THROW(message) throw CXBindingsException(message, cxC2U(__FILE__), __LINE__)
#else
#define CXB_THROW(message) throw CXBindingsException(message, __FILE__, __LINE__)
#endif

#ifndef __DEBUG__
#define CXB_ASSERT(expr)
#else
// In unix we use kill to terminate the application, that makes gdb
// keep it alive which makes debugging easier.
// (thanks go to an unknown author)
#ifdef __WINDOWS__
#define DIE() exit(1)
#else
#include <csignal>
#define DIE() kill(0, SIGTERM)
#endif

#if USE_UNICODE
#define CXB_ASSERT_MESSAGE(expr) \
            std::string err; \
            err.Printf(_T("Assertion failed in %s at %s:%d.\n\n%s"), cxC2U(__PRETTY_FUNCTION__).c_str(), cxC2U(__FILE__).c_str(), __LINE__, cxC2U(#expr).c_str());
#else
#define CXB_ASSERT_MESSAGE(expr) \
            std::string err; \
            err.Printf(_T("Assertion failed in %s at %s:%d.\n\n%s"), __PRETTY_FUNCTION__, __FILE__, __LINE__, #expr);
#endif

// non-fatal assertion
#define CXB_ASSERT_NONFATAL(expr) \
        if (!(expr)) \
        { \
            CXB_ASSERT_MESSAGE(expr); \
            wxSafeShowMessage(_T("Assertion error"), err); \
        }

// fatal assertion
#define CXB_ASSERT(expr) \
        if (!(expr)) \
        { \
            CXB_ASSERT_MESSAGE(expr); \
            wxSafeShowMessage(_T("Fatal assertion error"), err); \
            DIE(); \
        }
#endif

#endif
