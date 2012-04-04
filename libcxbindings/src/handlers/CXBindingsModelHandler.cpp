/**
 * @file CXBindingsModelHandler.cpp
 * @brief main file for defining grammar model handler for a node
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
#include "handlers/CXBindingsModelHandler.h"

/* implement wxWidgets RTTI modelrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsModelHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsModelHandler , "Handler for building and writing model nodes from a wxgrammar.xml file")  ;

CXBindingsModelHandler::CXBindingsModelHandler()
{
}

bool CXBindingsModelHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "model")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsModelHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( model , CXBindingsModel );

	std::string name = GetXmlAttr( m_node , "type")  ;
	
	if(name.empty())
		CXB_THROW( "Model with no type property")  ;
	
	model->SetName(name);
	
	CXBindingsArrayGrammarChild& childs = model->GetChilds();
	CXBindingsArrayGrammarProperty& properties = model->GetProperties();
	CXBindingsArrayGrammarRule& rules = model->GetRules();
	CXBindingsArrayGrammarForEachChild& childRules = model->GetChildRules();
	CXBindingsArrayGrammarCategory& categories = model->GetCategories();
	CXBindingsArrayGrammarChildContainer& childcontainers = model->GetChildContainers();
	CXBindingsArrayGrammarInherits& inherits = model->GetModels();

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

	return model;
}

xmlNode* CXBindingsModelHandler::DoWriteResource()
{
	CXBindingsModel* model = dynamic_cast<CXBindingsModel*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "model" );
	
	xmlNewProp( node , (const xmlChar*) "type" , cxxU2C( model->GetName() ) );
	
	CXBindingsArrayGrammarChild& childs = model->GetChilds();

	for( unsigned int i = 0; i < childs.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(childs[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	CXBindingsArrayGrammarRule& rules = model->GetRules();

	for( unsigned int i = 0; i < rules.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(rules[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarProperty& properties = model->GetProperties();

	for( unsigned int i = 0; i < properties.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(properties[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}
	
	CXBindingsArrayGrammarForEachChild& childRules = model->GetChildRules();

	for( unsigned int i = 0; i < childRules.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(childRules[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarCategory& categories = model->GetCategories();

	for( unsigned int i = 0; i < categories.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(categories[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarChildContainer& childcontainers = model->GetChildContainers();

	for( unsigned int i = 0; i < childcontainers.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(childcontainers[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarInherits& inherits = model->GetModels();

	for( unsigned int i = 0; i < inherits.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(inherits[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	return node;
}

bool CXBindingsModelHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsModel")   
		return true;

	return false;
}









