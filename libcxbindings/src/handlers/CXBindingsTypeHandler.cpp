/**
 * @file CXBindingsTypeHandler.cpp
 * @brief main file for defining grammar type handler for a node
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
#include "handlers/CXBindingsTypeHandler.h"

/* implement wxWidgets RTTI typermations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsTypeHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsTypeHandler , "Handler for building and writing type nodes from a wxgrammar.xml file")  ;

CXBindingsTypeHandler::CXBindingsTypeHandler()
{
}

bool CXBindingsTypeHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "type")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsTypeHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( type , CXBindingsType );

	std::string stemplate = GetXmlAttr( m_node , "template")  ;
	
	if(stemplate.empty())
		CXB_THROW( "Type with no template property")  ;
	
	type->SetTemplate(stemplate);
	
	CXBindingsArrayGrammarCondition& conditions = type->GetConditions();
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
			CXB_THROW( "Error type with no childs !")  ;

		type->SetContent(value);
	}
	
	//wxLogMessage( "End type : ")  ;

	return type;
}

xmlNode* CXBindingsTypeHandler::DoWriteResource()
{
	CXBindingsType* type = dynamic_cast<CXBindingsType*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "type" );
	
	xmlNewProp( node , (const xmlChar*) "template" , cxxU2C( type->GetTemplate() ) );
	
	CXBindingsArrayGrammarCondition& conditions = type->GetConditions();

	for( unsigned int i = 0; i < conditions.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(conditions[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsTypeHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsType")   
		return true;

	return false;
}





