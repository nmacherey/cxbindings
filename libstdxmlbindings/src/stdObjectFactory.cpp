
/**
 * @file stdObjectFactory.cpp
 * @brief main file for managing extended objects
 * @author Nicolas Macherey (nm@graymat.fr)
 * @version 10.4
 */
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>

#include "stdSingleton.h"
#include "stdDefines.h"
#include "stdObject.h"
#include "stdObjectFactory.h"

/** Global instance for the stdObjectFactory */
template<> stdObjectFactory* stdSingleton<stdObjectFactory>::instance = 0;

/** Global isShutdown initialization for the stdObjectFactory */
template<> bool  stdSingleton<stdObjectFactory>::isShutdown = false;

stdObjectFactory::stdObjectFactory()
{

}

stdObjectFactory::~stdObjectFactory()
{
	m_objects.clear();
}

std::vector< std::string > stdObjectFactory::GetList()
{
	std::vector< std::string > ret;

	stdObjectRegistrationMap::iterator it = m_objects.begin();
	for( ; it != m_objects.end() ; ++it )
		ret.push_back(it->first);

	return ret;
}

std::vector< std::string > stdObjectFactory::GetDescriptions()
{
	std::vector< std::string > ret;

	stdObjectRegistrationMap::iterator it = m_objects.begin();
	for( ; it != m_objects.end() ; ++it )
		ret.push_back(it->second.description);

	return ret;
}

bool stdObjectFactory::RegisterObject( const std::string& name,
		const std::string& description,
		stdObjectConstructor ctor,
		stdObjectDestructor dtor )
{
	stdObjectRegistrationMap::iterator it = m_objects.find(name);

	if( it != m_objects.end() )
		return false;

	stdObjectRegistration info;
	info.name = name;
	info.description = description;
	info.ctor = ctor;
	info.dtor = dtor;

	m_objects[name] = info;

	return true;
}

bool stdObjectFactory::UnregisterObject( const std::string& name )
{
	stdObjectRegistrationMap::iterator it = m_objects.find(name);

	if( it == m_objects.end() )
		return false;

	m_objects.erase(it);
	return true;
}

stdObject* stdObjectFactory::CreateObject( const std::string& name )
{
	stdObjectRegistrationMap::iterator it = m_objects.find(name);

	if( it == m_objects.end() )
		return NULL;

	stdObject* object = it->second.ctor( );
	return object;
}

bool stdObjectFactory::Exists( const std::string& name )
{
	stdObjectRegistrationMap::iterator it = m_objects.find(name);

	if( it == m_objects.end() )
		return false;

	return true;
}

std::string stdObjectFactory::GetDescription( const std::string& name )
{
	stdObjectRegistrationMap::iterator it = m_objects.find(name);

	if( it == m_objects.end() )
		return stdEmptyString;

	return it->second.description;
}			
		