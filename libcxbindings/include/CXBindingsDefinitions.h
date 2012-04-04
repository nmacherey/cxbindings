/**
 * @file CXBindingsDefinitions.h
 * @brief usefull macros
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */

#ifndef CXBINDINGS_DEFS_H
#define CXBINDINGS_DEFS_H

#include <vector>
#include <string>
#include "CXBindingsObjectBase.h"

const std::string stdEmptyString="";
typedef std::vector<std::string> CXBindingsArrayString;

/** simple define for writting quickly get and set methods */
#define PROPERTY( type , Name , name  ) \
	inline void Set##Name( const type& name ) {m_##name = name;} \
	inline type& Get##Name( ) { return m_##name;} 
	
	
#endif
