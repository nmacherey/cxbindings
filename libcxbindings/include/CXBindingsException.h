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

#define CXB_THROW(message) throw CXBindingsException(message, __FILE__, __LINE__)

#endif
