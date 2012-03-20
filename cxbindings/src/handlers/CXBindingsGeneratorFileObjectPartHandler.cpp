/**
 * @file CXBindingsGeneratorFileObjectPartHandler.cpp
 * @brief main file for defining grammar objectpart handler for a node
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
#include "handlers/CXBindingsGeneratorFileObjectPartHandler.h"

/* implement wxWidgets RTTI objectpartrmations */
IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsGeneratorFileObjectPartHandler , CXBindingsXmlHandler );

/* Register this handler in the CXBindingsXmlReader */
CXBINDINGS_REGISTER_XMLHANDLER( CXBindingsGeneratorFileObjectPartHandler , "Handler for building and writing objectpart nodes from a wxgrammar.xml file")  ;

CXBindingsGeneratorFileObjectPartHandler::CXBindingsGeneratorFileObjectPartHandler()
{
}

bool CXBindingsGeneratorFileObjectPartHandler::CanHandle(xmlNode *node)
{
	if( node == NULL )
		return false;

	std::string nodeName = cxC2U( (char*) node->name );
	
	if( nodeName == "objectpart")   
		return true;

	return false;
}

CXBindingsObjectBase *CXBindingsGeneratorFileObjectPartHandler::DoCreateResource()
{
	CXBINDINGS_MAKE_INSTANCE( objectpart , CXBindingsGeneratorFileObjectPart );

	std::string file = GetXmlAttr( m_node , "file")  ;
	
	if(file.empty())
		CXB_THROW( "GeneratorFileObjectPart with no file property")  ;
	
	objectpart->SetFile( file );
	
	std::string permission = GetXmlAttr( m_node , "permission")  ;
	
	if(!permission.empty() ) {
		//wxLogMessage( "\t objectpart permission is : ") + permission  ;
		objectpart->SetPermission( permission );
	}
	
	std::string content = GetXmlContent( m_node );
	
	if(content.empty())
		CXB_THROW( "GeneratorFileObjectPart with no content")  ;
	
	objectpart->SetContent( content );

	return objectpart;
}

xmlNode* CXBindingsGeneratorFileObjectPartHandler::DoWriteResource()
{
	CXBindingsGeneratorFileObjectPart* objectpart = dynamic_cast<CXBindingsGeneratorFileObjectPart*>(m_instance);

	xmlNode* node = xmlNewNode( NULL , (const xmlChar*) "objectpart" );

	xmlNewProp( node , (const xmlChar*) "file" , cxxU2C( objectpart->GetFile() ) );
	
	if( !objectpart->GetPermission().empty() )
		xmlNewProp( node , (const xmlChar*) "permission" , cxxU2C( objectpart->GetPermission() ) );

	if( !objectpart->GetContent().empty() )
		xmlNodeSetContent( node , cxxU2C( objectpart->GetContent() ) );

	return node;
}

bool CXBindingsGeneratorFileObjectPartHandler::CanHandle(CXBindingsObjectBase *resource)
{
	if( resource == NULL )
		return false;

	std::string className = resource->GetClassInfo()->GetClassName();
	
	if( className == "CXBindingsGeneratorFileObjectPart")   
		return true;

	return false;
}





