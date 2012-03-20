/**
 * @file CXBindingsGeneratorFileTemplateHandler.cpp
 * @brief main file for defining grammar stemplate handler for a node
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
#include "CXBindingsGeneratorFile.h"

#include "CXBindingsXmlReader.h"
#include "handlers/CXBindingsGeneratorFileTemplateHandler.h"

/* implement wxWidgets RTTI stemplatermations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsGeneratorFileTemplateHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsGeneratorFileTemplateHandler , "Handler for building and writing stemplate nodes from a wxgrammar.xml file")  ;

CXBindingsGeneratorFileTemplateHandler::CXBindingsGeneratorFileTemplateHandler()
{
}

bool CXBindingsGeneratorFileTemplateHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "template")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsGeneratorFileTemplateHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( stemplate , CXBindingsGeneratorFileTemplate );

	std::string name = GetXmlAttr( m_node , "name")  ;
	
	if(name.empty())
		CXB_THROW( "GeneratorFileTemplate with no name property")  ;
	
	stemplate->SetName( name );
	
	CXBindingsArrayGrammarGeneratorFileObjectPart& objects = stemplate->GetObjects();

	xmlNode* child = m_node->children;	

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="objectpart")  
		{
			CXBindingsGeneratorFileObjectPart* object = (CXBindingsGeneratorFileObjectPart*) CreateResFromNode(child,NULL);
			if( object != NULL ) {
				objects.push_back(*object);
				delete object;
			}
		}
		child = child->next;
	}

	return stemplate;
}

xmlNode* CXBindingsGeneratorFileTemplateHandler::DoWriteResource()
{
	CXBindingsGeneratorFileTemplate* stemplate = dynamic_cast<CXBindingsGeneratorFileTemplate*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "template" );

	xmlNewProp( node , (const xmlChar*) "name" , cxxU2C( stemplate->GetName() ) );
	
	CXBindingsArrayGrammarGeneratorFileObjectPart& objects = stemplate->GetObjects();

	for( unsigned int i = 0; i < objects.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(objects[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsGeneratorFileTemplateHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsGeneratorFileTemplate")   
		return true;

	return false;
}





