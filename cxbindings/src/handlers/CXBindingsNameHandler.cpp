/**
 * @file CXBindingsNameHandler.cpp
 * @brief main file for defining grammar name handler for a node
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
#include "handlers/CXBindingsNameHandler.h"

/* implement wxWidgets RTTI namermations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsNameHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsNameHandler , "Handler for building and writing name nodes from a wxgrammar.xml file")  ;

CXBindingsNameHandler::CXBindingsNameHandler()
{
}

bool CXBindingsNameHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "name")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsNameHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( name , CXBindingsName );

	std::string stemplate = GetXmlAttr( m_node , "template")  ;
	
	if(stemplate.empty())
		CXB_THROW( "Name with no template property")  ;
	
	name->SetTemplate(stemplate);
	
	CXBindingsArrayGrammarCondition& conditions = name->GetConditions();
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
			CXB_THROW( "Error name with no childs !")  ;

		name->SetContent(value);
	}

	return name;
}

xmlNode* CXBindingsNameHandler::DoWriteResource()
{
	CXBindingsName* name = dynamic_cast<CXBindingsName*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "name" );
	
	xmlNewProp( node , (const xmlChar*) "template" , cxxU2C( name->GetTemplate() ) );
	
	CXBindingsArrayGrammarCondition& conditions = name->GetConditions();

	for( unsigned int i = 0; i < conditions.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(conditions[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsNameHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsName")   
		return true;

	return false;
}






