
/**
 * @file stdXmlReader.h
 * @brief main file for managing xml reading stuff for wxg
 * @author Nicolas Macherey (nm@graymat.fr)
 * @version 10.4
 */
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stack>

#include "stdDefines.h"
#include "stdException.h"

#include "stdXmlReader.h"

IMPLEMENT_ABSTRACT_CLASS_STD( stdXmlHandler , stdObject );

/** Global instance for the stdXmlReader */
template<> stdXmlReader* stdSingleton<stdXmlReader>::instance = 0;

/** Global isShutdown initialization for the stdXmlReader */
template<> bool  stdSingleton<stdXmlReader>::isShutdown = false;

stdXmlReader::stdXmlReader()
{

}

stdXmlReader::~stdXmlReader()
{
	m_registeredHandlers.clear();

	for( unsigned int i = 0; i < m_handlers.size(); ++i )
		delete m_handlers[i];

	m_handlers.clear();
}

stdObject* stdXmlReader::LoadFile( const std::string& file )
{	
	m_doc = xmlParseFile( file.c_str() );
	
	if( m_doc == NULL )
		STD_THROW("Error cannot open document!");
	
	m_root = xmlDocGetRootElement( m_doc );
	
	if( m_root == NULL )
		STD_THROW("Error cannot get your document root !");
		
	std::string n_root = (char*) m_root->name;

	if( n_root.empty() )
		STD_THROW( "Error document root is empty" );

	/* Once we have extracted the root from the document, we have to 
   	 * find a handler which is able to return an object associated to
	 * this root.
	 */
	stdObject* resource = CreateResFromNode( m_root , stdEmptyString , NULL , NULL );
	
	xmlFreeDoc(m_doc);

	return resource;
}

void stdXmlReader::SaveFile( const std::string& file , stdObject* resource )
{
	m_doc = xmlNewDoc((const xmlChar*) "1.0");
	
	if( m_doc == NULL )
		STD_THROW("Error cannot open document!");
	
	m_root = WriteResource( resource );
	
	if( m_root == NULL )
		STD_THROW("Error cannot get your document root !");
		
	xmlDocSetRootElement(m_doc,m_root);
	xmlSaveFormatFileEnc( file.c_str() , m_doc , "UTF-8" , 1 );
	
	xmlFreeDoc(m_doc);
}

bool stdXmlReader::HasHandlerFor( xmlNode* node )
{
	for( unsigned int i = 0 ; i < m_handlers.size() ; ++i )
		if( m_handlers[i]->CanHandle( node ) )
			return true;

	return false;
}

stdObject *stdXmlReader::DoCreateResFromNode(xmlNode *node, const std::string& className , stdObject *resource,stdXmlHandler *handlerToUse)
{
	if( handlerToUse != NULL )
		return handlerToUse->CreateResource( node , className , resource );

	stdObject* result = NULL;
	stdXmlHandler *handler = NULL;

	for( unsigned int i = 0; i < m_handlers.size() ; ++i )
		if( m_handlers[i]->CanHandle( className ) ) {
			handler = m_handlers[i];
			break;
		}
	
	if( handler == NULL ) {
		for( unsigned int i = 0; i < m_handlers.size() ; ++i )
			if( m_handlers[i]->CanHandle( node ) ) {
				handler = m_handlers[i];
				break;
			}
	}
	
	if( handler != NULL )
		result = handler->CreateResource( node , className , resource );

	return result;
}

xmlNode* stdXmlReader::DoWriteResource( stdObject* resource, const char* nodename , stdXmlHandler* handlerToUse )
{
	if( handlerToUse != NULL )
		return handlerToUse->WriteResource( resource, nodename );

	xmlNode* node = NULL;

	for( unsigned int i = 0; i < m_handlers.size() ; ++i )
		if( m_handlers[i]->CanHandle( resource ) ) {
			node = m_handlers[i]->WriteResource( resource, nodename );
			break;
		}

	return node;
}

std::vector< std::string > stdXmlReader::GetList()
{
	std::vector< std::string > ret;

	stdXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.begin();
	for( ; it != m_registeredHandlers.end() ; ++it )
		ret.push_back(it->first);

	return ret;
}

std::vector< std::string > stdXmlReader::GetDescriptions()
{
	std::vector< std::string > ret;

	stdXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.begin();
	for( ; it != m_registeredHandlers.end() ; ++it )
		ret.push_back(it->second.description);

	return ret;
}

bool stdXmlReader::RegisterXmlHandler( const std::string& name,
		const std::string& description,
		stdXmlHandlerConstructor ctor,
		stdXmlHandlerDestructor dtor )
{
	stdXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it != m_registeredHandlers.end() )
		return false;

	stdXmlHandlerRegistration info;
	info.name = name;
	info.description = description;
	info.ctor = ctor;
	info.dtor = dtor;

	m_registeredHandlers[name] = info;
	
	stdXmlHandler* handler = CreateXmlHandler( name );
	m_handlers.push_back( handler );

	return true;
}

bool stdXmlReader::UnregisterXmlHandler( const std::string& name )
{
	stdXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it == m_registeredHandlers.end() )
		return false;
	
	RemoveHandlersOfClass( name );
	m_registeredHandlers.erase(it);
	return true;
}

stdXmlHandler* stdXmlReader::CreateXmlHandler( const std::string& name )
{
	stdXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it == m_registeredHandlers.end() )
		return NULL;

	stdXmlHandler* handler = it->second.ctor( );
	return handler;
}

bool stdXmlReader::Exists( const std::string& name )
{
	stdXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it == m_registeredHandlers.end() )
		return false;

	return true;
}

std::string stdXmlReader::GetDescription( const std::string& name )
{
	stdXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it == m_registeredHandlers.end() )
		return stdEmptyString;

	return it->second.description;
}


void stdXmlReader::RemoveHandlersOfClass( const std::string& classInfo )
{
	unsigned int i = 0;

	while( i < m_handlers.size() ) {
		std::string className = m_handlers[i]->GetClassInfo()->GetClassName();

		if( className.compare( classInfo ) == 0 ) {
			delete m_handlers[i];
			stdArrayXmlHandler::iterator it = m_handlers.begin() + i;
			m_handlers.erase(it);
		}
		else {
			++i;
		}
	}
}

stdXmlHandler::stdXmlHandler():
	stdObject(),
	m_node(NULL),
	m_instance(NULL)
{
}

stdObject *stdXmlHandler::CreateResource(xmlNode *node, const std::string& className , stdObject *instance)
{
    xmlNode *myNode = m_node;
    std::string myClass = m_class;
    stdObject *myInstance = m_instance;

    m_instance = instance;

    m_node = node;
    m_class = (char*) node->name;

    stdObject *returned = DoCreateResource();

    m_node = myNode;
    m_class = myClass;
    m_instance = myInstance;

    return returned;
}

xmlNode* stdXmlHandler::WriteResource( stdObject* resource, const char* nodename )
{
	stdObject* myInstance = m_instance;

	m_instance = resource;
    
    if( nodename )
        m_nodename = nodename;
    else
        m_nodename = "";

	xmlNode* node = DoWriteResource( );

	m_instance = myInstance;
	return node;
}

void stdXmlHandler::CreateChildren(bool this_hnd_only)
{
	xmlNode* child = m_node->children;
	while( child != NULL ) {

		if( stdXmlReader::Get()->HasHandlerFor( child ) )	
			stdXmlReader::Get()->DoCreateResFromNode(child, stdEmptyString, NULL, this_hnd_only ? this : NULL);
		
		child = child->next;
	}
}


void stdXmlHandler::CreateChildrenPrivately(xmlNode *rootnode)
{
	xmlNode *root;
	if (rootnode == NULL) root = m_node; else root = rootnode;
	xmlNode *n = root->children;

	while (n != NULL)
	{
		if (CanHandle(n))
		{
			CreateResource(n,stdEmptyString, NULL);
		}
		n = n->next;
    }
}
		
