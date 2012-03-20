/**
 * @file CXBindingsGeneratorFile.cpp
 * @brief main file for reading a wxgrammar_generator_file in xml format
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

#include "CXBindingsGeneratorFile.h"


/*********************************************************************************
  * IMPLEMENT WX RTTI DYNAMIC CLASSES
  *******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileVar , CXBindingsObjectBase );
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileAddin , CXBindingsObjectBase );
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileObjectPart , CXBindingsObjectBase );
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileTemplate , CXBindingsObjectBase );
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileTypeInfo , CXBindingsObjectBase );
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFile , CXBindingsObjectBase );


CXBindingsGeneratorFileTemplate* CXBindingsGeneratorFile::FindTemplate( const std::string& name )
{
	CXBindingsArrayGrammarGeneratorFileTemplate::iterator it = m_templates.begin();

	for( ; it != m_templates.end() ; ++it ) {

		if( it->GetName() == name ) 
			return &(*it);

	}

	return NULL;
}
