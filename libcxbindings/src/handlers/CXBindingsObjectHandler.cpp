/**
 * @file CXBindingsObjectHandler.cpp
 * @brief main file for defining grammar object handler for a node
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
#include "handlers/CXBindingsObjectHandler.h"

/* implement wxWidgets RTTI objectrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsObjectHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsObjectHandler , "Handler for building and writing object nodes from a wxgrammar.xml file")  ;

CXBindingsObjectHandler::CXBindingsObjectHandler()
{
}

bool CXBindingsObjectHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "object")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsObjectHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( object , CXBindingsObject );

	std::string name = GetXmlAttr( m_node , "type")  ;
	
	if(name.empty())
		CXB_THROW( "Object with no type property")  ;
	
	object->SetName(name);
	
	CXBindingsArrayGrammarChild& childs = object->GetChilds();
	CXBindingsArrayGrammarProperty& properties = object->GetProperties();
	CXBindingsArrayGrammarRule& rules = object->GetRules();
	CXBindingsArrayGrammarForEachChild& childRules = object->GetChildRules();
	CXBindingsArrayGrammarCategory& categories = object->GetCategories();
	CXBindingsArrayGrammarChildContainer& childcontainers = object->GetChildContainers();
	CXBindingsArrayGrammarInherits& inherits = object->GetModels();

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
		else if( cname =="childcontainer")  
		{
			CXBindingsChildContainer* container = (CXBindingsChildContainer*) CreateResFromNode(child,NULL);
			if( container != NULL ) {
				childcontainers.push_back(*container);
				delete container;
			}
		}
		else if( cname =="inherits")  
		{
			CXBindingsInherits* inherit = (CXBindingsInherits*) CreateResFromNode(child,NULL);
			if( inherit != NULL ) {
				inherits.push_back(*inherit);
				delete inherit;
			}
		}


		child = child->next;
	}

	return object;
}

xmlNode* CXBindingsObjectHandler::DoWriteResource()
{
	CXBindingsObject* object = dynamic_cast<CXBindingsObject*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "object" );
	
	xmlNewProp( node , (const xmlChar*) "type" , cxxU2C( object->GetName() ) );
	
	CXBindingsArrayGrammarChild& childs = object->GetChilds();

	for( unsigned int i = 0; i < childs.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(childs[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	CXBindingsArrayGrammarRule& rules = object->GetRules();

	for( unsigned int i = 0; i < rules.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(rules[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarProperty& properties = object->GetProperties();

	for( unsigned int i = 0; i < properties.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(properties[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}
	
	CXBindingsArrayGrammarForEachChild& childRules = object->GetChildRules();

	for( unsigned int i = 0; i < childRules.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(childRules[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarCategory& categories = object->GetCategories();

	for( unsigned int i = 0; i < categories.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(categories[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarChildContainer& childcontainers = object->GetChildContainers();

	for( unsigned int i = 0; i < childcontainers.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(childcontainers[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarInherits& inherits = object->GetModels();

	for( unsigned int i = 0; i < inherits.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(inherits[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	return node;
}

bool CXBindingsObjectHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsObject")   
		return true;

	return false;
}










