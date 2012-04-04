/**
 * @file CXBindings.cpp
 * @brief Helper class over boost::any object
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 12.3
 */
#include <algorithm>
#include "CXBindingsAny.h"

CXBindingsAny::CXBindingsAny() {
}

CXBindingsAny::CXBindingsAny(const CXBindingsAny& rhs){

}
CXBindingsAny& CXBindingsAny::operator=(const CXBindingsAny & rhs) {
    m_var = rhs.m_var;
    return (*this);
}

long CXBindingsAny::GetLong() {
    return boost::any_cast<long> (m_var); 
}

int CXBindingsAny::GetInt() {
    return boost::any_cast<int> (m_var);
}

std::string CXBindingsAny::GetString() {
    return boost::any_cast<std::string>(m_var);
}


double CXBindingsAny::GetDouble() {
    return boost::any_cast<double> (m_var);
}

float CXBindingsAny::GetFloat() {
    return boost::any_cast<float> (m_var);
}

bool CXBindingsAny::GetBool() {
    return boost::any_cast<bool> (m_var);
}

std::string CXBindingsAny::GetType() {
    try {
        boost::any_cast<std::string>(m_var);
        return "string";
    } catch(const boost::bad_any_cast &) {}
    
    return m_var.type().name();
}
