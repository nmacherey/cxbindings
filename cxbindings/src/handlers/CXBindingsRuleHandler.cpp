/**
 * @file CXBindingsRuleHandler.cpp
 * @brief main file for defining grammar rule handler for a node
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
#include "handlers/CXBindingsRuleHandler.h"

/* implement wxWidgets RTTI rulermations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsRuleHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsRuleHandler , "Handler for building and writing rule nodes from a wxgrammar.xml file")  ;

CXBindingsRuleHandler::CXBindingsRuleHandler()
{
}

bool CXBindingsRuleHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeRule = cxC2U( (char*) node->name );
	
	if( nodeRule == "rule")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsRuleHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( rule , CXBindingsRule );

	std::string make = GetXmlAttr( m_node , "make")  ;
	
	if(!make.empty())
		rule->SetMake(make);

	std::string accessors = GetXmlAttr( m_node , "accessors")  ;
	if( !accessors.empty() ){
		if( accessors =="true")  
			rule->SetAccessors(true);
		else
			rule->SetAccessors(false);
	}
	
	xmlNode* child = m_node->children;
	
	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="name")  
		{
			CXBindingsName* name = (CXBindingsName*) CreateResFromNode(child,NULL);
			if( name != NULL ) {
				rule->SetContentName(*name);
				delete name;
			}
		}
		else if( cname =="type")  
		{
			CXBindingsType* type = (CXBindingsType*) CreateResFromNode(child,NULL);
			if( type != NULL ) {
				rule->SetContentType(*type);
				delete type;
			}

		}
		
		else if( cname =="variable")  
		{
			CXBindingsVariable* var = (CXBindingsVariable*) CreateResFromNode(child,NULL);
			if( var != NULL ) {
				rule->SetContentVariable(*var);
				delete var;
			}

		}
		
		child = child->next;
	}
	
	return rule;
}

xmlNode* CXBindingsRuleHandler::DoWriteResource()
{
	CXBindingsRule* rule = dynamic_cast<CXBindingsRule*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "rule" );
	
	if( !rule->GetMake().empty() )
		xmlNewProp( node , (const xmlChar*) "make" , cxxU2C( rule->GetMake() ) );
	
	std::string accessorsStr;
	if( rule->GetAccessors() )
		accessorsStr = "true" ;
	else
		accessorsStr = "false" ;

	xmlNewProp( node , (const xmlChar*) "accessors" , cxxU2C( accessorsStr ) );

	xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(rule->GetContentName()) );
	if( child != NULL )
		xmlAddChild( node , child );
	
	child = CXBindingsXmlReader::Get()->WriteResource( &(rule->GetContentType()) );
	if( child != NULL )
		xmlAddChild( node , child );
		
	child = CXBindingsXmlReader::Get()->WriteResource( &(rule->GetContentVariable()) );
	if( child != NULL )
		xmlAddChild( node , child );
	
	return node;
}

bool CXBindingsRuleHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string classRule = resource->GetClassInfo()->GetClassName();
	
	if( classRule == "CXBindingsRule")   
		return true;

	return false;
}
