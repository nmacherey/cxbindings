/**
 * @file CXBindings.h
 * @brief Helper class over boost::any object
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 12.3
 */
#ifndef CXBINDINGS_ANY_H
#define CXBINDINGS_ANY_H
#include <string>
#include <boost/any.hpp>
/**
 * @class CXBindignsAny
 * @brief This is the main class for implementing the any object over boost::any class,
 * Mainly this class adds some utility methods similar to the one in the wxAny class in wxWidgets
 * library
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 */
class CXBindingsAny
{

public:
    boost::any m_var;
	/* Constructors */
	/** Default constructor */
	CXBindingsAny();
	/** Copy constructor */
	CXBindingsAny(const CXBindingsAny& rhs);
	template<typename ValueType> CXBindingsAny(const ValueType &);
	CXBindingsAny & operator=(const CXBindingsAny &);
	template<typename ValueType> CXBindingsAny & operator=(const ValueType &);
	/* Additional helpers */
	/** Returns the content of the object as a long value */
	long GetLong();
	/** Returns the object's type as a string value */
	std::string GetType();
	
	std::string GetString();
	/** Returns the content of the object as a int value */
	int GetInt();
	/** Returns the content of the object as a double value */
	double GetDouble();
	/** Returns the content of the object as a float value */
	float GetFloat();
	/** Returns the content of the object as a boolean value */
	bool GetBool();
};


template<typename ValueType> CXBindingsAny::CXBindingsAny(const ValueType & value):m_var(value){}

template<typename ValueType> CXBindingsAny& CXBindingsAny::operator=(const ValueType & value) {
    boost::any(value).swap(m_var);
    return *this;
}

#endif //CXBINDINGS_ANY_H
