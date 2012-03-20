/**
 * @file CXBindingsPropertyHandler.cpp
 * @brief main file for defining grammar property handler for a node
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
#include "handlers/CXBindingsPropertyHandler.h"

/* implement wxWidgets RTTI propertyrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsPropertyHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsPropertyHandler , "Handler for building and writing property nodes from a wxgrammar.xml file")  ;

CXBindingsPropertyHandler::CXBindingsPropertyHandler()
{
}

bool CXBindingsPropertyHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "property")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsPropertyHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( property , CXBindingsProperty );

	std::string name = GetXmlAttr( m_node , "name")  ;
	
	if(name.empty())
		CXB_THROW( "Property with no name property")  ;
	
	property->SetName( name );
	
	bool required = GetXmlAttr( m_node , "required") =="true" ;
	property->SetRequired( required );

        std::string defaultValue = GetXmlAttr( m_node , "default")  ;
	
	property->SetDefaultValue( defaultValue );
		
	xmlNode* child = m_node->children;
	
	CXBindingsArrayGrammarCondition& conditions = property->GetConditions();
	
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
		}

		child = child->next;
	}

	return property;
}

xmlNode* CXBindingsPropertyHandler::DoWriteResource()
{
	CXBindingsProperty* property = dynamic_cast<CXBindingsProperty*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "property" );
	
	xmlNewProp( node , (const xmlChar*) "name" , cxxU2C( property->GetName() ) );
	std::string requiredStr = property->GetRequired() ? "true" :  "false"  ;

	xmlNewProp( node , (const xmlChar*) "required" , cxxU2C( requiredStr ) );	
	
	if( !property->GetDefaultValue().empty() )
		xmlNewProp( node , (const xmlChar*) "default" , cxxU2C( property->GetDefaultValue() ) );
	
	CXBindingsArrayGrammarCondition& conditions = property->GetConditions();

	for( unsigned int i = 0; i < conditions.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(conditions[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsPropertyHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsProperty")   
		return true;

	return false;
}




