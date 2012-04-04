/**
 * @file CXBindingsConditionHandler.h
 * @brief main file for defining grammar condition handler for a node
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
#include "handlers/CXBindingsConditionHandler.h"

/* implement wxWidgets RTTI informations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsConditionHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsConditionHandler , "Handler for building and writing condition nodes from a wxgrammar.xml file")  ;

CXBindingsConditionHandler::CXBindingsConditionHandler()
{
}

bool CXBindingsConditionHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "if")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsConditionHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( condition , CXBindingsCondition );
	
	std::string cond = GetXmlAttr( m_node , "cond")  ;
	
	if(cond.empty())
		CXB_THROW( "Condition with no cond property")  ;
	
	condition->SetCondition( cond );

	std::string value = GetXmlContent( m_node );

	if( value.empty() )
		CXB_THROW( "Error condition with no childs !")  ;

	//wxLogMessage( "\t => condition content is : ") + value  ;
	condition->SetContent(value);

	return condition;
}

xmlNode* CXBindingsConditionHandler::DoWriteResource()
{
	CXBindingsCondition* condition = dynamic_cast<CXBindingsCondition*>(m_instance);
	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "if" );

	xmlNewProp( node , (const xmlChar*) "cond" , cxxU2C(condition->GetCondition() ) );
	xmlNodeSetContent( node , cxxU2C( condition->GetContent() ) );

	return node;
}

bool CXBindingsConditionHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsCondition")   
		return true;

	return false;
}

