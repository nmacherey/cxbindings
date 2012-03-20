/**
 * @file CXBindingsGeneratorFileTypeInfoHandler.cpp
 * @brief main file for defining grammar stypeinfo handler for a node
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
#include "CXBindingsGeneratorFile.h"

#include "CXBindingsXmlReader.h"
#include "handlers/CXBindingsGeneratorFileTypeInfoHandler.h"

/* implement wxWidgets RTTI stypeinformations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsGeneratorFileTypeInfoHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsGeneratorFileTypeInfoHandler , "Handler for building and writing stypeinfo nodes from a wxgrammar.xml file")  ;

CXBindingsGeneratorFileTypeInfoHandler::CXBindingsGeneratorFileTypeInfoHandler()
{
}

bool CXBindingsGeneratorFileTypeInfoHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "types_info")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsGeneratorFileTypeInfoHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( stypeinfo , CXBindingsGeneratorFileTypeInfo );
	
	CXBindingsStringStringMap& types = stypeinfo->GetTypes();

	xmlNode* child = m_node->children;	

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="typeinfo")  
		{
			std::string source = GetXmlAttr(child,"source") ;
			std::string dest = GetXmlAttr(child,"destination") ;
			
			if( source.empty() )
				CXB_THROW( "Error type info with an empty source !")  ;
				
			if( dest.empty() )
				CXB_THROW( "Error type info with an empty destination !")  ;
				
			types[source] = dest;

		}
		child = child->next;
	}

	return stypeinfo;
}

xmlNode* CXBindingsGeneratorFileTypeInfoHandler::DoWriteResource()
{
	CXBindingsGeneratorFileTypeInfo* stypeinfo = dynamic_cast<CXBindingsGeneratorFileTypeInfo*>(m_instance);
	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "types_info" );
	
	CXBindingsStringStringMap& types = stypeinfo->GetTypes();
	CXBindingsStringStringMap::iterator it = types.begin();

	for( ; it != types.end() ; ++it )
	{
		xmlNode* child = xmlNewNode( NULL , (const xmlChar*) "typeinfo");
		
		xmlNewProp( child , (const xmlChar*) "source" , cxxU2C( it->first) );
		xmlNewProp( child , (const xmlChar*) "destination" , cxxU2C( it->second) );
		
		xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsGeneratorFileTypeInfoHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsGeneratorFileTypeInfo")   
		return true;

	return false;
}





