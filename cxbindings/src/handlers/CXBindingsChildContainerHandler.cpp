/**
 * @file CXBindingsChildContainerHandler.cpp
 * @brief main file for defining grammar childcontainer handler for a node
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
#include "handlers/CXBindingsChildContainerHandler.h"

/* implement wxWidgets RTTI childcontainerrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsChildContainerHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsChildContainerHandler , "Handler for building and writing childcontainer nodes from a wxgrammar.xml file")  ;

CXBindingsChildContainerHandler::CXBindingsChildContainerHandler()
{
}

bool CXBindingsChildContainerHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "childcontainer")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsChildContainerHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( childcontainer , CXBindingsChildContainer );

	std::string name = GetXmlAttr( m_node , "name")  ;
	
	if(name.empty())
		CXB_THROW( "ChildContainer with no name property")  ;
	
	childcontainer->SetName(name);
	
	std::string minOccurs = GetXmlAttr( m_node , "name")  ;
	long minOccs;

	if(minOccurs.empty())
		minOccs = 1;
	else {
		std::stringstream strm;
		strm << minOccs;
		strm >> minOccurs;;
	}
		
	childcontainer->SetMinOccurs((int) minOccs);
	
	std::string maxOccurs = GetXmlAttr( m_node , "name")  ;
	long maxOccs;

	if(maxOccurs.empty())
		maxOccs = 1;
	else if( maxOccurs == "unbounded")   
		maxOccs = -1;
	else {
		std::stringstream strm;
		strm << maxOccs;
		strm >> maxOccurs;;
	}
		
	childcontainer->SetMaxOccurs((int) maxOccs);
	
	CXBindingsArrayGrammarChild& childs = childcontainer->GetChilds();
	CXBindingsArrayGrammarProperty& properties = childcontainer->GetProperties();
	CXBindingsArrayGrammarRule& rules = childcontainer->GetRules();
	CXBindingsArrayGrammarForEachChild& childRules = childcontainer->GetChildRules();
	CXBindingsArrayGrammarCategory& categories = childcontainer->GetCategories();

	xmlNode* child = m_node->children;	

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="child")  
		{
			CXBindingsChild* child_object = (CXBindingsChild*) CreateResFromNode(child,NULL);
			if( child_object != NULL ) {
				childs.push_back(*child_object);
				delete child_object;
			}
		}
		else if( cname =="rule")  
		{
			CXBindingsRule* rule = (CXBindingsRule*) CreateResFromNode(child,NULL);
			if( rule != NULL ) {
				rules.push_back(*rule);
				delete rule;
			}
		}
		else if( cname =="property")  
		{
			CXBindingsProperty* property = (CXBindingsProperty*) CreateResFromNode(child,NULL);
			if( property != NULL ) {
				properties.push_back(*property);
				delete property;
			}
		}
		else if( cname =="foreach_child")  
		{
			CXBindingsForEachChild* childrule = (CXBindingsForEachChild*) CreateResFromNode(child,NULL);
			if( childrule != NULL ) {
				childRules.push_back(*childrule);
				delete childrule;
			}
		}
		else if( cname =="category")  
		{
			CXBindingsCategory* category = (CXBindingsCategory*) CreateResFromNode(child,NULL);
			if( category != NULL ) {
				categories.push_back(*category);
				delete category;
			}
		}


		child = child->next;
	}

	return childcontainer;
}

xmlNode* CXBindingsChildContainerHandler::DoWriteResource()
{
	CXBindingsChildContainer* childcontainer = dynamic_cast<CXBindingsChildContainer*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "childcontainer" );
	
	xmlNewProp( node , (const xmlChar*) "name" , cxxU2C( childcontainer->GetName() ) );
	
	std::string minOccsStr;
	if( childcontainer->GetMinOccurs() == -1 )
		minOccsStr = "unbounded" ;
	else{
		std::stringstream strm;
		strm << childcontainer->GetMinOccurs() ;
		minOccsStr = strm.str();
	}

	xmlNewProp( node , (const xmlChar*) "minOccurs" , cxxU2C( minOccsStr ) );

	std::string maxOccsStr;
	if( childcontainer->GetMaxOccurs() == -1 )
		maxOccsStr = "unbounded" ;
	else{
		std::stringstream strm;
		strm << childcontainer->GetMaxOccurs() ;
		maxOccsStr = strm.str();
	}

	xmlNewProp( node , (const xmlChar*) "maxOccurs" , cxxU2C( maxOccsStr ) );
	
	CXBindingsArrayGrammarChild& childs = childcontainer->GetChilds();

	for( unsigned int i = 0; i < childs.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(childs[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	CXBindingsArrayGrammarRule& rules = childcontainer->GetRules();

	for( unsigned int i = 0; i < rules.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(rules[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarProperty& properties = childcontainer->GetProperties();

	for( unsigned int i = 0; i < properties.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(properties[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}
	
	CXBindingsArrayGrammarForEachChild& childRules = childcontainer->GetChildRules();

	for( unsigned int i = 0; i < childRules.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(childRules[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarCategory& categories = childcontainer->GetCategories();

	for( unsigned int i = 0; i < categories.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(categories[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}
	return node;
}

bool CXBindingsChildContainerHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsChildContainer")   
		return true;

	return false;
}









