/**
 * @file CXBindingsInheritsHandler.cpp
 * @brief main file for defining grammar inherits handler for a node
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
#include "handlers/CXBindingsInheritsHandler.h"

/* implement wxWidgets RTTI informations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsInheritsHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsInheritsHandler , "Handler for building and writing inherits nodes from a wxgrammar.xml file")  ;

CXBindingsInheritsHandler::CXBindingsInheritsHandler()
{
}

bool CXBindingsInheritsHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "inherits")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsInheritsHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( inherits , CXBindingsInherits );

	std::string model = GetXmlAttr( m_node , "model")  ;
	
	if(model.empty())
		CXB_THROW( "inherits with no of property")  ;
	
	inherits->SetName(model);
	
	return inherits;
}

xmlNode* CXBindingsInheritsHandler::DoWriteResource()
{
	CXBindingsInherits* inherits = dynamic_cast<CXBindingsInherits*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "inherits" );
	xmlNewProp( node , (const xmlChar*) "model" , cxxU2C( inherits->GetName() ) );

	return node;
}

bool CXBindingsInheritsHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsInherits")   
		return true;

	return false;
}


