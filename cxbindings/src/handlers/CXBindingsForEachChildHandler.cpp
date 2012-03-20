/**
 * @file CXBindingsForEachChildHandler.cpp
 * @brief main file for defining grammar foreachchild handler for a node
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
#include "handlers/CXBindingsForEachChildHandler.h"

/* implement wxWidgets RTTI foreachchildrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsForEachChildHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsForEachChildHandler , "Handler for building and writing foreachchild nodes from a wxgrammar.xml file")  ;

CXBindingsForEachChildHandler::CXBindingsForEachChildHandler()
{
}

bool CXBindingsForEachChildHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "foreach_child")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsForEachChildHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( foreachchild , CXBindingsForEachChild );

	std::string of = GetXmlAttr( m_node , "of")  ;
	
	if(of.empty())
		CXB_THROW( "ForEachChild with no of property")  ;
	
	foreachchild->SetOf(of);
	
	CXBindingsArrayGrammarRule& rules = foreachchild->GetRules();
	xmlNode* child = m_node->children;	

	while( child != NULL )
	{
		std::string cname = cxC2U( (char*) child->name );
		
		if( cname =="rule")  
		{
			CXBindingsRule* rule = (CXBindingsRule*) CreateResFromNode(child,NULL);
			if( rule != NULL ) {
				rules.push_back(*rule);
				delete rule;
			}
		}

		child = child->next;
	}

	return foreachchild;
}

xmlNode* CXBindingsForEachChildHandler::DoWriteResource()
{
	CXBindingsForEachChild* foreachchild = dynamic_cast<CXBindingsForEachChild*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "foreach_child" );
	
	xmlNewProp( node , (const xmlChar*) "of" , cxxU2C( foreachchild->GetOf() ) );
	
	CXBindingsArrayGrammarRule& rules = foreachchild->GetRules();

	for( unsigned int i = 0; i < rules.size() ; ++i )
	{
		xmlNode* child = CXBindingsXmlReader::Get()->WriteResource( &(rules[i]) );
		if( child != NULL )
			xmlAddChild( node , child );
	}

	return node;
}

bool CXBindingsForEachChildHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsForEachChild")   
		return true;

	return false;
}






