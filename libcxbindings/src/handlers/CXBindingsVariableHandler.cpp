/**
 * @file CXBindingsVariableHandler.cpp
 * @brief main file for defining grammar variable handler for a node
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
#include "handlers/CXBindingsVariableHandler.h"

/* implement wxWidgets RTTI variablermations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsVariableHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsVariableHandler , "Handler for building and writing variable nodes from a wxgrammar.xml file")  ;

CXBindingsVariableHandler::CXBindingsVariableHandler()
{
}

bool CXBindingsVariableHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "variable")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsVariableHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( variable , CXBindingsVariable );

	std::string stemplate = GetXmlAttr( m_node , "template")  ;
	
	variable->SetTemplate(stemplate);
	
	CXBindingsArrayGrammarCondition& conditions = variable->GetConditions();
	xmlNode* child = m_node->children;	
	bool foundsomething = false;

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="if")  
		{
			CXBindingsCondition* condition = (CXBindingsCondition*) CreateResFromNode(child,NULL);
			if( condition != NULL ) {
				conditions.push_back(*condition);
				delete condition;
			}
			foundsomething = true;
		}

		child = child->next;
	}

	if( !foundsomething )
	{
		std::string value = GetXmlContent( m_node );

		if( value.empty() )
			CXB_THROW( "Error variable with no childs !")  ;


		variable->SetContent(value);
	}

	return variable;
}

xmlNode* CXBindingsVariableHandler::DoWriteResource()
{
	CXBindingsVariable* variable = dynamic_cast<CXBindingsVariable*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "variable" );
	
	xmlNewProp( node , (const xmlChar*) "template" , cxxU2C( variable->GetTemplate() ) );
	
	CXBindingsArrayGrammarCondition& conditions = variable->GetConditions();

	for( unsigned int i = 0; i < conditions.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(conditions[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsVariableHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsVariable")   
		return true;

	return false;
}





