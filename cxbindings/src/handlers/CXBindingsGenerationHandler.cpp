/**
 * @file CXBindingsGenerationHandler.cpp
 * @brief main file for defining grammar generation handler for a node
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */

#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <stdio.h>
#include <iostream>
#include <sstream>
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
#include "handlers/CXBindingsGenerationHandler.h"

/* implement wxWidgets RTTI informations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsGenerationHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsGenerationHandler , "Handler for building and writing generation nodes from a wxgrammar.xml file")  ;

CXBindingsGenerationHandler::CXBindingsGenerationHandler()
{
}

bool CXBindingsGenerationHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "generation")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsGenerationHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( generation , CXBindingsGeneration );

	xmlNode* child = m_node->children;

	CXBindingsArrayString& objects = generation->GetObjects();
	CXBindingsArrayInt& levels = generation->GetLevels();
	CXBindingsArrayString& templates = generation->GetTemplates();

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="generate")  
		{
			std::string elem = GetXmlAttr(child,"object") ;
			
			if( !elem.empty() )
			{
				objects.push_back(elem);
				//wxLogMessage( "Found generate node : ") + elem  ;
			}
			else {
				//wxLogMessage( "Warning generate node with no objects !")  ;
			}

			std::string level = GetXmlAttr(child , "level") ;
			if( level.empty() || level == "all")   
				levels.push_back(-1);
			else
				levels.push_back(1);
			
			std::string templateStr = GetXmlAttr(child , "template") ;
			if( templateStr.empty() )
				templates.push_back("global_scope") ;
			else
				templates.push_back(templateStr);
		}
		
		child = child->next;
	}
	
	return generation;
}

xmlNode* CXBindingsGenerationHandler::DoWriteResource()
{
	CXBindingsGeneration* generation = dynamic_cast<CXBindingsGeneration*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "generation" );
	
	CXBindingsArrayString& objects = generation->GetObjects();
	CXBindingsArrayInt& levels = generation->GetLevels();
	CXBindingsArrayString& templates = generation->GetTemplates();
	
	for( unsigned int i = 0 ; i < objects.size() ; ++i ) {
		xmlNode* child = xmlNewNode( NULL , (const xmlChar*) "generate" );
		
		xmlNewProp( child , (const xmlChar*) "object" , cxxU2C(objects[i]) );
		
		std::string levelStr;
		if( levels[i] == -1 )
			levelStr = "all" ;
		else {
			std::stringstream strm;
			strm << levels[i];
			levelStr = strm.str();
		}

		xmlNewProp( child , (const xmlChar*) "level" , cxxU2C(levelStr) );
		xmlNewProp( child , (const xmlChar*) "template" , cxxU2C(templates[i]) );

		xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsGenerationHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsGeneration")   
		return true;

	return false;
}

