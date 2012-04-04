/**
 * @file CXBindingsCategoryHandler.cpp
 * @brief main file for defining grammar category handler for a node
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
#include "handlers/CXBindingsCategoryHandler.h"

/* implement wxWidgets RTTI categoryrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsCategoryHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsCategoryHandler , "Handler for building and writing category nodes from a wxgrammar.xml file")  ;

CXBindingsCategoryHandler::CXBindingsCategoryHandler()
{
}

bool CXBindingsCategoryHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "category")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsCategoryHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( category , CXBindingsCategory );

	std::string name = GetXmlAttr( m_node , "name")  ;
	
	if(name.empty())
		CXB_THROW( "Category with no name property")  ;
	
	category->SetName(name);
	
	std::string minOccurs = GetXmlAttr( m_node , "name")  ;
	long minOccs;

	if(minOccurs.empty())
		minOccs = 1;
	else {
		std::stringstream strm;
		strm << minOccs;
		strm >> minOccurs;
	}
		
	category->SetMinOccurs((int) minOccs);
	
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
		
	category->SetMaxOccurs((int) maxOccs);
	
	CXBindingsArrayGrammarChild& childs = category->GetChilds();
	CXBindingsArrayGrammarChildContainer& containers = category->GetChildContainers();

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

		else if( cname =="childcontainer")  
		{
			CXBindingsChildContainer* child_object = (CXBindingsChildContainer*) CreateResFromNode(child,NULL);
			if( child_object != NULL ) {
				containers.push_back(*child_object);
				delete child_object;
			}
		}

		child = child->next;
	}

	return category;
}

xmlNode* CXBindingsCategoryHandler::DoWriteResource()
{
	CXBindingsCategory* category = dynamic_cast<CXBindingsCategory*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "category" );
	
	xmlNewProp( node , (const xmlChar*) "name" , cxxU2C( category->GetName() ) );
	
	std::string minOccsStr;
	if( category->GetMinOccurs() == -1 )
		minOccsStr = "unbounded" ;
	else {
		std::stringstream strm;
		strm << category->GetMinOccurs() ;
		minOccsStr = strm.str();
	}

	xmlNewProp( node , (const xmlChar*) "minOccurs" , cxxU2C( minOccsStr ) );

	std::string maxOccsStr;
	if( category->GetMaxOccurs() == -1 )
		maxOccsStr = "unbounded" ;
	else{
		std::stringstream strm;
		strm << category->GetMaxOccurs() ;
		maxOccsStr = strm.str();
	}

	xmlNewProp( node , (const xmlChar*) "maxOccurs" , cxxU2C( maxOccsStr ) );
	
	CXBindingsArrayGrammarChild& childs = category->GetChilds();

	for( unsigned int i = 0; i < childs.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(childs[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	CXBindingsArrayGrammarChildContainer& containers = category->GetChildContainers();

	for( unsigned int i = 0; i < containers.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(containers[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsCategoryHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsCategory")   
		return true;

	return false;
}








