/**
 * @file CXBindingsGeneratorFileAddinHandler.cpp
 * @brief main file for defining grammar sfile handler for a node
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
#include "handlers/CXBindingsGeneratorFileAddinHandler.h"

/* implement wxWidgets RTTI sfilermations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsGeneratorFileAddinHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsGeneratorFileAddinHandler , "Handler for building and writing sfile nodes from a wxgrammar.xml file")  ;

CXBindingsGeneratorFileAddinHandler::CXBindingsGeneratorFileAddinHandler()
{
}

bool CXBindingsGeneratorFileAddinHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "addin")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsGeneratorFileAddinHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( sfile , CXBindingsGeneratorFileAddin );

	std::string name = GetXmlAttr( m_node , "name")  ;
	
	if(name.empty())
		CXB_THROW( "GeneratorFile with no name property")  ;
	
	sfile->SetName( name );
	xmlNode* child = m_node->children;	

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="header")  
		{
			std::string content = GetXmlContent(child);
			sfile->SetHeader(content);
		}
		else if( cname =="source")  
		{
			std::string content = GetXmlContent(child);
			sfile->SetSource(content);
		}
		child = child->next;
	}

	return sfile;
}

xmlNode* CXBindingsGeneratorFileAddinHandler::DoWriteResource()
{
	CXBindingsGeneratorFileAddin* sfile = dynamic_cast<CXBindingsGeneratorFileAddin*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "addin" );
	xmlNewProp( node , (const xmlChar*) "name" , cxxU2C( sfile->GetName() ) );
	
	if( !sfile->GetHeader().empty() )
		xmlNewChild(node,NULL,(const xmlChar*) "header" , cxxU2C(sfile->GetHeader()) );
		
	if( !sfile->GetSource().empty() )
		xmlNewChild(node,NULL,(const xmlChar*) "source" , cxxU2C(sfile->GetSource()) );
		
	return node;
}

bool CXBindingsGeneratorFileAddinHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsGeneratorFileAddin")   
		return true;

	return false;
}






