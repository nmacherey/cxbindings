/**
 * @file CXBindingsChildHandler.cpp
 * @brief main file for defining grammar child handler for a node
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
#include "handlers/CXBindingsChildHandler.h"

/* implement wxWidgets RTTI childrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsChildHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsChildHandler , "Handler for building and writing child nodes from a wxgrammar.xml file")  ;

CXBindingsChildHandler::CXBindingsChildHandler()
{
}

bool CXBindingsChildHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "child")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsChildHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( child , CXBindingsChild );

	std::string name = GetXmlAttr( m_node , "name")  ;
	
	if(name.empty())
		CXB_THROW( "Child with no name property")  ;
	
	child->SetName(name);
	
	std::string minOccurs = GetXmlAttr( m_node , "name")  ;
	long minOccs;

	if(minOccurs.empty())
		minOccs = 1;
	else{
		std::stringstream strm;
		strm << minOccs;
		strm >> minOccurs;
	}
		
	child->SetMinOccurs((int) minOccs);
	
	std::string maxOccurs = GetXmlAttr( m_node , "name")  ;
	long maxOccs;

	if(maxOccurs.empty())
		maxOccs = 1;
	else if( maxOccurs == "unbounded")   
		maxOccs = -1;
	else{
		std::stringstream strm;
		strm << maxOccs;
		strm >> maxOccurs;
	}
		
	child->SetMaxOccurs((int) maxOccs);
	
	CXBindingsArrayGrammarProperty& properties = child->GetProperties();
	CXBindingsArrayGrammarRule& rules = child->GetRules();
	xmlNode* childnode = m_node->children;	

	while( childnode != NULL )
	{
		std::string cname = cxC2U( (char*) childnode->name );
		
		if( cname =="rule")  
		{
			CXBindingsRule* rule = (CXBindingsRule*) CreateResFromNode(childnode,NULL);
			if( rule != NULL ) {
				rules.push_back(*rule);
				delete rule;
			}
		}
		else if( cname =="property")  
		{
			CXBindingsProperty* property = (CXBindingsProperty*) CreateResFromNode(childnode,NULL);
			if( property != NULL ) {
				properties.push_back(*property);
				delete property;
			}
		}

		childnode = childnode->next;
	}

	return child;
}

xmlNode* CXBindingsChildHandler::DoWriteResource()
{
	CXBindingsChild* child = dynamic_cast<CXBindingsChild*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "child" );
	
	xmlNewProp( node , (const xmlChar*) "name" , cxxU2C( child->GetName() ) );
	
	std::string minOccsStr;
	if( child->GetMinOccurs() == -1 )
		minOccsStr = "unbounded" ;
	else {
		std::stringstream strm;
		strm << child->GetMinOccurs() ;
		minOccsStr = strm.str();
	}

	xmlNewProp( node , (const xmlChar*) "minOccurs" , cxxU2C( minOccsStr ) );

	std::string maxOccsStr;
	if( child->GetMaxOccurs() == -1 )
		maxOccsStr = "unbounded" ;
	else{
		std::stringstream strm;
		strm << child->GetMaxOccurs() ;
		maxOccsStr = strm.str();
	}

	xmlNewProp( node , (const xmlChar*) "maxOccurs" , cxxU2C( maxOccsStr ) );
	
	CXBindingsArrayGrammarRule& rules = child->GetRules();

	for( unsigned int i = 0; i < rules.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(rules[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarProperty& properties = child->GetProperties();

	for( unsigned int i = 0; i < properties.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(properties[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}


	return node;
}

bool CXBindingsChildHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsChild")   
		return true;

	return false;
}







