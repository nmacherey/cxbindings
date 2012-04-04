/**
 * @file CXBindingsGeneratorFileHandler.cpp
 * @brief main file for defining grammar sfile handler for a node
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
#include "handlers/CXBindingsGeneratorFileHandler.h"

/* implement wxWidgets RTTI sfilermations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsGeneratorFileHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsGeneratorFileHandler , "Handler for building and writing sfile nodes from a wxgrammar.xml file")  ;

CXBindingsGeneratorFileHandler::CXBindingsGeneratorFileHandler()
{
}

bool CXBindingsGeneratorFileHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "wxgrammar_generator_file")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsGeneratorFileHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( sfile , CXBindingsGeneratorFile );

	std::string name = GetXmlAttr( m_node , "lang")  ;
	
	if(name.empty())
		CXB_THROW( "GeneratorFile with no lang property")  ;
	
	sfile->SetName( name );
	
	CXBindingsArrayGrammarGeneratorFileVar& vars = sfile->GetVariables();
	CXBindingsArrayGrammarGeneratorFileAddin& addins = sfile->GetAddins();
	CXBindingsArrayGrammarGeneratorFileTemplate& templates = sfile->GetTemplates();

	xmlNode* child = m_node->children;	

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="var")  
		{
			CXBindingsGeneratorFileVar* object = (CXBindingsGeneratorFileVar*) CreateResFromNode(child,NULL);
			if( object != NULL ) {
				vars.push_back(*object);
				delete object;
			}
		}
		else if( cname =="template")  
		{
			CXBindingsGeneratorFileTemplate* object = (CXBindingsGeneratorFileTemplate*) CreateResFromNode(child,NULL);
			if( object != NULL ) {
				templates.push_back(*object);
				delete object;
			}
		}
		else if( cname =="types_info")  
		{
			CXBindingsGeneratorFileTypeInfo* object = (CXBindingsGeneratorFileTypeInfo*) CreateResFromNode(child,NULL);
			if( object != NULL ) {
				sfile->SetTypeInfo(*object);
				delete object;
			}
		}
		else if( cname =="addin")  
		{
			CXBindingsGeneratorFileAddin* addin = (CXBindingsGeneratorFileAddin*) CreateResFromNode(child,NULL);
			if( addin != NULL ) {
				addins.push_back(*addin);
				delete addin;
			}
		}
		child = child->next;
	}

	return sfile;
}

xmlNode* CXBindingsGeneratorFileHandler::DoWriteResource()
{
	CXBindingsGeneratorFile* sfile = dynamic_cast<CXBindingsGeneratorFile*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "wxgrammar_generator_file" );

	xmlNewProp( node , (const xmlChar*) "lang" , cxxU2C( sfile->GetName() ) );
	
	CXBindingsArrayGrammarGeneratorFileVar& vars = sfile->GetVariables();

	for( unsigned int i = 0; i < vars.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(vars[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}
	
	CXBindingsArrayGrammarGeneratorFileAddin& addins = sfile->GetAddins();

	for( unsigned int i = 0; i < addins.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(addins[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}
	
	CXBindingsArrayGrammarGeneratorFileTemplate& templates = sfile->GetTemplates();

	for( unsigned int i = 0; i < templates.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(templates[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}
	
	xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(sfile->GetTypeInfo()) );
	if( child != NULL )
		xmlAddChild( node , child );
		
	return node;
}

bool CXBindingsGeneratorFileHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsGeneratorFile")   
		return true;

	return false;
}






