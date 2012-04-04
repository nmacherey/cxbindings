/**
 * @file CXBindingsInfoHandler.cpp
 * @brief main file for defining grammar info handler for a node
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
#include "handlers/CXBindingsInfoHandler.h"

/* implement wxWidgets RTTI informations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsInfoHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsInfoHandler , "Handler for building and writing info nodes from a wxgrammar.xml file")  ;

CXBindingsInfoHandler::CXBindingsInfoHandler()
{
}

bool CXBindingsInfoHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "info")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsInfoHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( info , CXBindingsInfo );

	xmlNode* child = m_node->children;

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="generation")  
		{
			CXBindingsGeneration* generation = (CXBindingsGeneration*) CreateResFromNode(child,NULL);
			if( generation != NULL ) {
				info->SetGeneration(*generation);
				delete generation;
			}
		}
		
		child = child->next;
	}
	
	return info;
}

xmlNode* CXBindingsInfoHandler::DoWriteResource()
{
	CXBindingsInfo* info = dynamic_cast<CXBindingsInfo*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "info" );

	xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(info->GetGeneration()) );
	if( child != NULL )
		xmlAddChild( node , child );

	return node;
}

bool CXBindingsInfoHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsInfo")   
		return true;

	return false;
}


