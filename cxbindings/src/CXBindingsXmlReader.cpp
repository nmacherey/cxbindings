/**
 * @file CXBindingsXmlReader.h
 * @brief main file for managing xml reading stuff for CXBindings
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <stack>





#include "CXBindingsDefinitions.h"
#include "CXBindingsGlobals.h"
#include "CXBindingsException.h"
#include "CXBindingsExpressionParser.h"
#include "CXBindings.h"
#include "CXBindingsInterpreter.h"

#include "CXBindingsXmlReader.h"

IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsXmlHandler , CXBindingsObjectBase );

/** Global instance for the CXBindingsXmlReader */
template<> CXBindingsXmlReader* Singleton<CXBindingsXmlReader>::instance = 0;

/** Global isShutdown initialization for the CXBindingsXmlReader */
template<> bool  Singleton<CXBindingsXmlReader>::isShutdown = false;

CXBindingsXmlReader::CXBindingsXmlReader()
{

}

CXBindingsXmlReader::~CXBindingsXmlReader()
{
	m_registeredHandlers.clear();

	for( unsigned int i = 0; i < m_handlers.size(); ++i )
		delete m_handlers[i];

	m_handlers.clear();
}

CXBindingsObjectBase* CXBindingsXmlReader::LoadFile( const std::string& file )
{
	//wxLogMessage( "Opening File : ") + file  ;
	
	m_doc = xmlParseFile( cxcU2C(file) );
	
	if( m_doc == NULL )
		CXB_THROW("Error cannot open document!") ;
	
	m_root = xmlDocGetRootElement( m_doc );
	
	if( m_root == NULL )
		CXB_THROW("Error cannot get your document root !") ;
		
	std::string n_root = cxC2U((char*) m_root->name);
	//wxLogMessage( "Root read, name is : ") + n_root  ;

	if( n_root.empty() )
		CXB_THROW( "Error document root is empty")  ;

	/* Once we have extracted the root from the document, we have to 
   	 * find a handler which is able to return an object associated to
	 * this root.
	 */
	CXBindingsObjectBase* resource = CreateResFromNode( m_root , NULL , NULL );
	
	xmlFreeDoc(m_doc);

	return resource;
}

bool CXBindingsXmlReader::HasHandlerFor( xmlNode* node )
{
	for( unsigned int i = 0 ; i < m_handlers.size() ; ++i )
		if( m_handlers[i]->CanHandle( node ) )
			return true;

	return false;
}

CXBindingsObjectBase *CXBindingsXmlReader::DoCreateResFromNode(xmlNode *node, CXBindingsObjectBase *resource,CXBindingsXmlHandler *handlerToUse)
{
	if( handlerToUse != NULL )
		return handlerToUse->CreateResource( node , resource );

	CXBindingsObjectBase* result = NULL;
	for( unsigned int i = 0; i < m_handlers.size() ; ++i )
		if( m_handlers[i]->CanHandle( node ) ) {
			result = m_handlers[i]->CreateResource( node , resource );
			break;
		}

	return result;
}

xmlNode* CXBindingsXmlReader::DoWriteResource( CXBindingsObjectBase* resource , CXBindingsXmlHandler* handlerToUse )
{
	if( handlerToUse != NULL )
		return handlerToUse->WriteResource( resource );

	xmlNode* node = NULL;

	for( unsigned int i = 0; i < m_handlers.size() ; ++i )
		if( m_handlers[i]->CanHandle( resource ) ) {
			node = m_handlers[i]->WriteResource( resource );
			break;
		}

	return node;
}

CXBindingsArrayString CXBindingsXmlReader::GetList()
{
	CXBindingsArrayString ret;

	CXBindingsXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.begin();
	for( ; it != m_registeredHandlers.end() ; ++it )
		ret.push_back(it->first);

	return ret;
}

CXBindingsArrayString CXBindingsXmlReader::GetDescriptions()
{
	CXBindingsArrayString ret;

	CXBindingsXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.begin();
	for( ; it != m_registeredHandlers.end() ; ++it )
		ret.push_back(it->second.description);

	return ret;
}

bool CXBindingsXmlReader::RegisterXmlHandler( const std::string& name,
		const std::string& description,
		CXBindingsXmlHandlerConstructor ctor,
		CXBindingsXmlHandlerDestructor dtor )
{
	CXBindingsXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it != m_registeredHandlers.end() )
		return false;

	CXBindingsXmlHandlerRegistration info;
	info.name = name;
	info.description = description;
	info.ctor = ctor;
	info.dtor = dtor;

	m_registeredHandlers[name] = info;
	
	CXBindingsXmlHandler* handler = CreateXmlHandler( name );
	m_handlers.push_back( handler );

	return true;
}

bool CXBindingsXmlReader::UnregisterXmlHandler( const std::string& name )
{
	CXBindingsXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it == m_registeredHandlers.end() )
		return false;
	
	RemoveHandlersOfClass( name );
	m_registeredHandlers.erase(it);
	return true;
}

CXBindingsXmlHandler* CXBindingsXmlReader::CreateXmlHandler( const std::string& name )
{
	CXBindingsXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it == m_registeredHandlers.end() )
		return NULL;

	CXBindingsXmlHandler* handler = it->second.ctor( );
	return handler;
}

bool CXBindingsXmlReader::Exists( const std::string& name )
{
	CXBindingsXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it == m_registeredHandlers.end() )
		return false;

	return true;
}

std::string CXBindingsXmlReader::GetDescription( const std::string& name )
{
	CXBindingsXmlHandlerRegistrationMap::iterator it = m_registeredHandlers.find(name);

	if( it == m_registeredHandlers.end() )
		return stdEmptyString;

	return it->second.description;
}


void CXBindingsXmlReader::RemoveHandlersOfClass( const std::string& classInfo )
{
	unsigned int i = 0;

	while( i < m_handlers.size() ) {
		std::string className = m_handlers[i]->GetClassInfo()->GetClassName();

		if( className == classInfo )  {
			delete m_handlers[i];
			CXBindingsArrayGrammarXmlHandler::iterator it = m_handlers.begin() + i;
			m_handlers.erase(it);
		}
		else {
			++i;
		}
	}
}

CXBindingsXmlHandler::CXBindingsXmlHandler():
	CXBindingsObjectBase(),
	m_node(NULL),
	m_instance(NULL)
{
}

CXBindingsObjectBase *CXBindingsXmlHandler::CreateResource(xmlNode *node, CXBindingsObjectBase *instance)
{
    xmlNode *myNode = m_node;
    std::string myClass = m_class;
    CXBindingsObjectBase *myInstance = m_instance;

    m_instance = instance;

    m_node = node;
    m_class = cxC2U( (char*) node->name );

    CXBindingsObjectBase *returned = DoCreateResource();

    m_node = myNode;
    m_class = myClass;
    m_instance = myInstance;

    return returned;
}

xmlNode* CXBindingsXmlHandler::WriteResource( CXBindingsObjectBase* resource )
{
	CXBindingsObjectBase* myInstance = m_instance;

	m_instance = resource;

	xmlNode* node = DoWriteResource( );

	m_instance = myInstance;
	return node;
}

void CXBindingsXmlHandler::CreateChildren(bool this_hnd_only)
{
	xmlNode* child = m_node->children;
	while( child != NULL ) {

		if( CXBindingsXmlReader::Get()->HasHandlerFor( child ) )	
			CXBindingsXmlReader::Get()->DoCreateResFromNode(child, NULL, this_hnd_only ? this : NULL);
		
		child = child->next;
	}
}


void CXBindingsXmlHandler::CreateChildrenPrivately(xmlNode *rootnode)
{
	xmlNode *root;
	if (rootnode == NULL) root = m_node; else root = rootnode;
	xmlNode *n = root->children;

	while (n != NULL)
	{
		if (CanHandle(n))
		{
			CreateResource(n, NULL);
		}
		n = n->next;
    }
}

