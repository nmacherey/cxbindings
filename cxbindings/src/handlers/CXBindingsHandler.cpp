/**
 * @file CXBindingsHandler.cpp
 * @brief main file for defining grammar grammar handler for a node
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
#include "handlers/CXBindingsHandler.h"

/* implement wxWidgets RTTI grammarrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsHandler , "Handler for building and writing grammar nodes from a wxgrammar.xml file")  ;

CXBindingsHandler::CXBindingsHandler()
{
}

bool CXBindingsHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "grammar")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( grammar , CXBindings );

	std::string name = GetXmlAttr( m_node , "type")  ;
	
	if(name.empty())
		CXB_THROW( "grammar with no name property")  ;
	
	grammar->SetName(name);
	
	CXBindingsArrayGrammarModel& models = grammar->GetModels();
	CXBindingsArrayGrammarObject& objects = grammar->GetObjects();
	CXBindingsArrayGrammarSet& vars = grammar->GetVars();

	xmlNode* child = m_node->children;	

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="model")  
		{
			CXBindingsModel* model = (CXBindingsModel*) CreateResFromNode(child,NULL);
			if( model != NULL ) {
				models.push_back(*model);
				delete model;
			}
		}
		else if( cname =="object")  
		{
			CXBindingsObject* object = (CXBindingsObject*) CreateResFromNode(child,NULL);
			if( object != NULL ) {
				objects.push_back(*object);
				delete object;
			}
		}
		else if( cname =="set")  
		{
			CXBindingsSet* var = (CXBindingsSet*) CreateResFromNode(child,NULL);
			if( var != NULL ) {
				vars.push_back(*var);
				delete var;
			}
		}
		else if( cname =="info")  
		{
			CXBindingsInfo* info = (CXBindingsInfo*) CreateResFromNode(child,NULL);
			if( info != NULL ) {
				grammar->SetInfo(*info);
				delete info;
			}
		}
		
		child = child->next;
	}

	return grammar;
}

xmlNode* CXBindingsHandler::DoWriteResource()
{
	CXBindings* grammar = dynamic_cast<CXBindings*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "grammar" );
	
	xmlNewProp( node , (const xmlChar*) "type" , cxxU2C( grammar->GetName() ) );
	
	CXBindingsArrayGrammarModel& models = grammar->GetModels();

	for( unsigned int i = 0; i < models.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(models[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	CXBindingsArrayGrammarObject& objects = grammar->GetObjects();

	for( unsigned int i = 0; i < objects.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(objects[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}

	CXBindingsArrayGrammarSet& vars = grammar->GetVars();

	for( unsigned int i = 0; i < vars.size() ; ++i )
	{
		xmlNode* childnode = CXBindingsXmlReader::Get()->WriteResource( &(vars[i]) );
		if( childnode != NULL )
			xmlAddChild( node , childnode );
	}
  
	xmlNode* childnode =  CXBindingsXmlReader::Get()->WriteResource( &(grammar->GetInfo()) );
	if( childnode != NULL )
		xmlAddChild( node , childnode );


	return node;
}

bool CXBindingsHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindings")   
		return true;

	return false;
}











