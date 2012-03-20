/**
 * @file CXBindingsSetHandler.cpp
 * @brief main file for defining grammar set handler for a node
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
#include "handlers/CXBindingsSetHandler.h"

/* implement wxWidgets RTTI setrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsSetHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsSetHandler , "Handler for building and writing set nodes from a wxgrammar.xml file")  ;

CXBindingsSetHandler::CXBindingsSetHandler()
{
}

bool CXBindingsSetHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "set")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsSetHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( set , CXBindingsSet );

	std::string var = GetXmlAttr( m_node , "var")  ;
	
	if(var.empty())
		CXB_THROW( "Set with no var property")  ;
	
	set->SetVar( var );
	
	xmlNode* child = m_node->children;
	
	CXBindingsArrayGrammarCondition& conditions = set->GetConditions();
	
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
			CXB_THROW( "Error set with no childs !")  ;

		set->SetContent(value);
	}

	return set;
}

xmlNode* CXBindingsSetHandler::DoWriteResource()
{
	CXBindingsSet* set = dynamic_cast<CXBindingsSet*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "set" );
	CXBindingsArrayGrammarCondition& conditions = set->GetConditions();

	xmlNewProp( node , (const xmlChar*) "var" , cxxU2C( set->GetVar() ) );

	for( unsigned int i = 0; i < conditions.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(conditions[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	if( !set->GetContent().empty() )
		xmlNodeSetContent( node , cxxU2C( set->GetContent() ) );

	return node;
}

bool CXBindingsSetHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsSet")   
		return true;

	return false;
}



