/**
 * @file CXBindingsGeneratorFileVarHandler.cpp
 * @brief main file for defining grammar var handler for a node
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
#include "handlers/CXBindingsGeneratorFileVarHandler.h"

/* implement wxWidgets RTTI varrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsGeneratorFileVarHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsGeneratorFileVarHandler , "Handler for building and writing var nodes from a wxgrammar.xml file")  ;

CXBindingsGeneratorFileVarHandler::CXBindingsGeneratorFileVarHandler()
{
}

bool CXBindingsGeneratorFileVarHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "var")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsGeneratorFileVarHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( var , CXBindingsGeneratorFileVar );

	std::string name = GetXmlAttr( m_node , "name")  ;
	
	if(name.empty())
		CXB_THROW( "GeneratorFileVar with no name property")  ;
	
	var->SetName( name );
	
	std::string content = GetXmlContent( m_node );
	
	if(content.empty())
		CXB_THROW( "GeneratorFileVar with no content")  ;
	
	var->SetContent( content );

	return var;
}

xmlNode* CXBindingsGeneratorFileVarHandler::DoWriteResource()
{
	CXBindingsGeneratorFileVar* var = dynamic_cast<CXBindingsGeneratorFileVar*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "var" );

	xmlNewProp( node , (const xmlChar*) "name" , cxxU2C( var->GetName() ) );
	
	if( !var->GetContent().empty() )
		xmlNodeSetContent( node , cxxU2C( var->GetContent() ) );

	return node;
}

bool CXBindingsGeneratorFileVarHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsGeneratorFileVar")   
		return true;

	return false;
}




