/**
 * @file CXBindingsCppHandlersGenerator.h
 * @brief main file for generating CPP objects code in CXBindings
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "CXBindingsDefinitions.h"
#include "CXBindingsGlobals.h"
#include "CXBindingsException.h"
#include "CXBindingsExpressionParser.h"
#include "CXBindings.h"
#include "CXBindingsInterpreter.h"
#include "CXBindingsGeneratorFile.h"
#include "CXBindingsInterpreter.h"
#include "CXBindingsXmlReader.h"
#include "CXBindingsGeneratorFactory.h"

#include "generators/CXBindingsCppHandlersGenerator.h"

/* Implements CXBindingsObjectBase RTTI information */
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS( CXBindingsCppHandlersGenerator , CXBindingsGenerator );

CXBINDINGS_REGISTER_GENERATOR( CXBindingsCppHandlersGenerator, "CPPHandlers" , "Generate CPP XML Handlers from CXBindings definition")  ;

CXBindingsCppHandlersGenerator::~CXBindingsCppHandlersGenerator()
{

}

bool CXBindingsCppHandlersGenerator::DoGenerateCode( CXBindingsGeneratorOptions& options )
{
	/* for each element in the info object of the grammar */
	CXBindings& grammar = m_interpreterInfo.grammar;
	CXBindingsInfo& info = grammar.GetInfo();
	CXBindingsArrayString genOrder = info.GetGeneration().GetObjects();
	CXBindingsArrayString templates = info.GetGeneration().GetTemplates();
	
	/* The first thing to do is to estabilsh the generation objects dependecy list
	 * So that object will be generated in the right order...
	 */
	CXBindingsArrayString dependencies = DoCreateDependencyList(options);
	std::string filename = options.ns + "_globals" ;

	/** @todo here generate object in their dependencies order is this really usefull .??? */
	for( unsigned int k = 0; k < dependencies.size() ; ++k ) {
		for( unsigned int i = 0; i < genOrder.size() ; ++i ){

			CXBindingsArrayGrammarObjectInfoMap& objectsInfoMap = m_interpreterInfo.objects;
			CXBindingsArrayGrammarObjectInfo& objectsInfo = objectsInfoMap[genOrder[i]];

			for( unsigned int j = 0; j < objectsInfo.size() ; ++j ) {
				
				std::string realName = objectsInfo[j].properties["name" ];
				realName = GetRealType( realName , options );

				if( realName == dependencies[k] )  {
						DoGenerateCodeFor( objectsInfo[j] , grammar , options );
					break;
				}
			}
		}
	}
	
	/** The last step is to generate the globals file and the other files for given objects */
	
	std::string hdrDir = options.genDir + "/include/handlers" ;
	std::string srcDir = options.genDir + "/src/handlers" ;
	CXBindingsMakeDirRecursively(hdrDir+"/") ;
	CXBindingsMakeDirRecursively(srcDir+"/") ;
	
	hdrDir = options.genDir + "/include" ;
	srcDir = options.genDir + "/src" ;
	
	// Generate additional files
	CXBindingsArrayGrammarGeneratorFileAddin& addins = m_genfile->GetAddins();
	for( unsigned int i = 0; i < addins.size() ; ++i ) {
		std::string fname = addins[i].GetName();
		std::string header = addins[i].GetHeader();
		std::string source = addins[i].GetSource();
		
		if( !header.empty() ) {
			DoReplaceMacros( header );
			SaveFile( hdrDir + "/" + fname + ".h"  , header );
		}
		if( !source.empty() ) {
			DoReplaceMacros( source );
			SaveFile( srcDir + "/" + fname + ".cpp"  , source) ;
		}
	}
	
	hdrDir = options.genDir + "/include/handlers" ;
	srcDir = options.genDir + "/src/handlers" ;
	
	filename = options.ns + "_globals" ;
	CXBindingsHandlerFileInfoMap::iterator it = m_objectInfos.begin();
	for( ; it != m_objectInfos.end() ; ++it )
	{
		if( m_objectFiles[it->first]!=filename && !m_objectFiles[it->first].empty() )
		{
			std::string srcFile = srcDir + "/" + m_objectFiles[it->first] + "XmlHandler"  + ".cpp" ;
			std::string hdrFile = hdrDir + "/" + m_objectFiles[it->first] + "XmlHandler"  + ".h" ;
			
			SaveFile( srcFile , it->second.srcInfo );
			SaveFile( hdrFile , it->second.headerInfo );
		}
		
	}
	return false;
}

void CXBindingsCppHandlersGenerator::DoGenerateCodeFor( CXBindingsObjectInfo& objectInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{

	// STEP 0 : APPEND OBJECT PROPERTIES IN THE MACRO MAP
	CXBindingsStringStringMap& properties = objectInfo.properties;
	
	//wxLogMessage( "\t STEP O : Calculating Object preliminar informations : ") + properties["name" ]  ;
	
	//wxLogMessage( "\t\t STEP O.1 : Appending Object macro : ") + properties["name" ]  ;
	
	if( properties["name"].empty() ) 
		CXB_THROW( "Error object with no name cannot continue...")  ;

	std::string objectName = properties["name" ];
	objectName = GetRealType( objectName , options );
	
	std::string upName; 
	upName += objectName;
	to_upper( upName );
	
	SetMacro( "object" , objectName );
	SetMacro( "OBJECT" , upName );

	// @todo here the file is not preoprely set
	/* First set up some macros for the given file */
	// filename macros
	std::string filename = objectName + "XmlHandler" ;
	SetMacro( "filename" , filename) ;

	// STEP 1 : CHECK CHILD CONTAINERS AND RULES
	//wxLogMessage( "\t STEP 1 : Checking for existing rules in the child containers ... ")  ;
        CXBindingsArrayGrammarChildContainerInfo& ccInfo = objectInfo.childs;
	
	//std::string msg = std::string::Format( "\t\t Found %d childcontainers...") , ccInfo.size()  ;
	//wxLogMessage( msg );

	CXBindingsHandlerFileInfo objectFileInfo;
	
	for( unsigned int i = 0; i < ccInfo.size() ; ++i ) {
	 	objectFileInfo += DoGenerateChildContainerCodeFor( ccInfo[i] , grammar , options ); 	
	}
		
	//wxLogMessage( "\t END STEP 1")  ;

	// STEP 2 : CHECK CATEGORIES AND RULES
	//wxLogMessage( "\t STEP 2 : Checking for existing rules in the child categories ... ")  ;
        CXBindingsArrayGrammarCategoryInfo& catInfo = objectInfo.categories;
	
	//msg = std::string::Format( "\t\t Found %d categories...") , catInfo.size()  ;
	//wxLogMessage( msg );
	
	for( unsigned int i = 0; i < catInfo.size() ; ++i ) {
		objectFileInfo += DoGenerateCategoryCodeFor( catInfo[i] , grammar , options );
	}
		
	//wxLogMessage( "\t END STEP 2")  ;

	CXBindingsArrayGrammarChildInfo& childs = objectInfo.childInfos;
	for( unsigned int j = 0; j < childs.size() ; ++j ) {

		CXBindingsHandlerFileInfo ruleContent = DoGenerateChildRuleCodeFor( childs[j] , grammar , options );
		objectFileInfo += ruleContent;
	}

	//wxLogMessage( "\t STEP 3 : create additional object macros for the given object if exists ... ")  ;

	// Finally build the missing macrods for doc comments and other parameters info
	CXBindingsHandlerFileParametersMacros params;
	
	/* once all parameters list have been generated, we need to create the macros of derived
	 * objects. This is very important in order to treat the inherit template from the various
	 * rules/
	 */

	for( unsigned int i = 0; i < objectFileInfo.bases.size() ; ++i ) {
		std::string pName = objectFileInfo.bases[i].first;
		CXBindingsHandlerFileInfo& inf = objectFileInfo.bases[i].second;

		CXBindingsHandlerFileParametersMacros localp;
		DoCreateParametersMacrosFor( inf , localp , options );

		params += localp;
		//wxLogMessage( localp.property_readers );
	}
	
	DoCreateParametersMacrosFor( objectFileInfo , params , options );

	SetMacro( "objects_attribute_readers" , params.attribute_readers) ;
	SetMacro( "objects_attribute_writers" , params.attribute_writers) ;
	SetMacro( "objects_property_readers" , params.property_readers) ;
	SetMacro( "objects_property_writers" , params.property_writers) ;
	
	
	/* The next step is very important ! 
	 * First we have to establish all direct dependencies in the given property list
	 * and all dependencies of the given bases.
	 * Once this is done, we can get the dependency list and include list to put in the 
	 * source file*/
	
	std::string local_includes;
	std::string object_imports;
	
	CXBindingsArrayString lc;
	CXBindingsArrayString currObj;
	currObj.push_back(objectName);
	DoGetIncludesListFor(lc,currObj,objectFileInfo,options);
	
	for( unsigned int i = 0; i < lc.size() ; ++i ) {
		local_includes += lc[i];
	}
	
	SetMacro( "local_includes" , local_includes) ;
	//wxLogMessage( "\t\t\t local_includes is :") + local_includes  ;

		
	//wxLogMessage( "\t END STEP 3...")  ;

	// STEP 4 : CHECK EXISTING RULES IN THE GIVEN OBJECT
	//wxLogMessage( "\t STEP 4 : Checking for existing rules in the object ... ")  ;
	CXBindingsArrayGrammarRuleInfo& rules = objectInfo.rules;
	
	//msg = std::string::Format( "\t\t Found %d rules...") , rules.size()  ;
	//wxLogMessage( msg );

	CXBindingsHandlerFileInfo FinalInfo;
	
	if( filename ==options.ns + "_globals")   {
		//wxLogMessage( "Object is a typedef or an enumeration, no handler needs to be generated for.") ;
		return;
	}
	
	CXBindingsGeneratorFileTemplate* rTemplate = m_genfile->FindTemplate( "object")  ;

	if( rTemplate == NULL )
		CXB_THROW( "Error cannot find template object")  ;
	
	std::string objectNodeName = properties["name" ];
	boost::replace_all(objectNodeName,"Type","");

	SetMacro( "object_node_name" , objectNodeName) ;
	
	CXBindingsArrayGrammarGeneratorFileObjectPart& objects = rTemplate->GetObjects();

	for( unsigned int i = 0; i < objects.size() ; ++i ) {
		
		std::string content = objects[i].GetContent();
		DoReplaceMacros( content );

		if( objects[i].GetFile() =="header")   
		{
			FinalInfo.headerInfo += content;
		}
		else
			FinalInfo.srcInfo += content;
		
		//wxLogMessage( "\t\t\t Generated rule content is : ") + content  ;
	}
	
	FinalInfo += objectFileInfo;
	m_objectInfos[objectName] = FinalInfo;

	//wxLogMessage( "\t END STEP 4...")  ;
}

void CXBindingsCppHandlersGenerator::DoCreateParametersMacrosFor( CXBindingsHandlerFileInfo& file , CXBindingsHandlerFileParametersMacros& parameters  , CXBindingsGeneratorOptions& options)
{
	for( unsigned int i = 0; i < file.properties.size() ; ++i )
	{
		std::string pName = file.properties[i].first;
		std::string pType = file.properties[i].second;

		//wxLogMessage( pName + " - ") + pType  ;
		
		std::string nameExt = pName;
		nameExt = GetPropertyExtension( nameExt , options );
		
		SetMacro( "name_extension", nameExt) ;
		SetMacro( "name" , pName) ;
		SetMacro( "type" , pType) ;
		
		/* Here we have to get the types of each properties and if it's not an object
		 * get the "real" type of the given properties (i.e : a typedef or an enumeration)
		 */
		 
		/* first check if the type is known or not */
		CXBindingsStringStringMap::iterator it = m_types.find( pName );
		if( it != m_types.end() )
			pType = it->second;
			
		//wxLogMessage( pName + " - ") + pType  ;
		
		CXBindingsGeneratorFileTemplate* preader = NULL;
		CXBindingsGeneratorFileTemplate* pwriter = NULL;
		
		std::string trName = "property_reader_"  + pType;
		std::string twName = "property_writer_"  + pType;
		
		preader = m_genfile->FindTemplate( trName );
		pwriter = m_genfile->FindTemplate( twName );
			
		if( preader == NULL || pwriter == NULL )
			CXB_THROW( "Error cannot find property reader and writer template for object of type : " + pType );
			
		CXBindingsArrayGrammarGeneratorFileObjectPart& objects = preader->GetObjects();

		for( unsigned int j = 0; j < objects.size() ; ++j ) {
			
			std::string content = objects[j].GetContent();
			DoReplaceMacros( content );

			if( objects[j].GetFile() =="src")    {
				parameters.property_readers += content;
			}
		}
		
		CXBindingsArrayGrammarGeneratorFileObjectPart& objectsw = pwriter->GetObjects();

		for( unsigned int j = 0; j < objectsw.size() ; ++j ) {
			
			std::string content = objectsw[j].GetContent();
			DoReplaceMacros( content );

			if( objectsw[j].GetFile() =="src")    {
				parameters.property_writers += content;
			}
		}
	}
	
	for( unsigned int i = 0; i < file.attributes.size() ; ++i )
	{
		std::string pName = file.attributes[i].first;
		std::string pType = file.attributes[i].second;

		//wxLogMessage( pName + " - ") + pType  ;
		
		std::string nameExt = pName;
		nameExt = GetPropertyExtension( nameExt , options );
		
		SetMacro( "name_extension" , nameExt) ;
		SetMacro( "name" , pName) ;
		SetMacro( "type" , pType) ;
		
		/* Here we have to get the types of each attributes and if it's not an object
		 * get the "real" type of the given attributes (i.e : a typedef or an enumeration)
		 */
		 
		/* first check if the type is known or not */
		CXBindingsStringStringMap::iterator it = m_types.find( pName );
		if( it != m_types.end() )
			pType = it->second;
		
		//wxLogMessage( pName + " - ") + pType  ;
		
		CXBindingsGeneratorFileTemplate* preader = NULL;
		CXBindingsGeneratorFileTemplate* pwriter = NULL;
		
		std::string trName = "attribute_reader_"  + pType;
		std::string twName = "attribute_writer_"  + pType;
		
		preader = m_genfile->FindTemplate( trName );
		pwriter = m_genfile->FindTemplate( twName );
			
		if( preader == NULL || pwriter == NULL )
			CXB_THROW( "Error cannot find attribute reader and writer template for object of type : " + pType) ;
			
		CXBindingsArrayGrammarGeneratorFileObjectPart& objects = preader->GetObjects();

		for( unsigned int j = 0; j < objects.size() ; ++j ) {
			
			std::string content = objects[j].GetContent();
			DoReplaceMacros( content );

			if( objects[j].GetFile() =="src")    {
				parameters.attribute_readers += content;
			}
		}
		
		CXBindingsArrayGrammarGeneratorFileObjectPart& objectsw = pwriter->GetObjects();

		for( unsigned int j = 0; j < objectsw.size() ; ++j ) {
			
			std::string content = objectsw[j].GetContent();
			DoReplaceMacros( content );

			if( objectsw[j].GetFile() =="src")    {
				parameters.attribute_writers += content;
			}
		}
	}

}

CXBindingsHandlerFileInfo CXBindingsCppHandlersGenerator::DoGenerateRuleCodeFor( CXBindingsChildContainerInfo& childContainer , CXBindingsRuleInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsHandlerFileInfo res;

	//wxLogMessage( "\t\t Rule make is : ") + ruleInfo.make  ;
	//wxLogMessage( "\t\t Rule name is : ") + ruleInfo.name.content  ;
	//wxLogMessage( "\t\t Rule type is : ") + ruleInfo.type.content  ;

	CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
	std::string realType = ruleInfo.type.content;
	
	//wxLogMessage( "\t\t Real type is : ") + realType  ;
	
	if( ruleInfo.make =="object")  
		realType = ruleInfo.name.content;
	
	std::string typeTemplate = ruleInfo.type.stemplate;
	CXBindingsStringStringMap::iterator it = types.find( realType );

	std::string typeExt = GetRealType( realType  , options );
//	if( !IsKnownType( realType ) )
//		typeExt = GetRealType( realType  , options );
	
	if( it != types.end() ) {
		typeExt = it->second;
		//typeExt = realType;
	}
	
	//wxLogMessage( "\t\t Real type is : ") + realType  ;
	
	if( boost::contains(realType,":") )  
		realType = after_first(realType, ':')  ; 
	
	//wxLogMessage( "\t\t Real type is : ") + realType  ;
	
	std::string savedType = realType;
	// Find here type corrspondances
	std::string rType = FindRealTypeFor( realType );
	
	if( ruleInfo.make =="object")  
		rType = "object" ;
	
	if( ruleInfo.make =="child_enumerator")  
		rType = "enum" ;

	if( typeTemplate == "array")    {
		//realType = "std::vector< ") + realType + wxT(" >" ;
		rType = "array"  + rType;
	}
	
	//wxLogMessage( "Registering type for : ") + ruleInfo.name.content + " type is :") + realType + wxT(" ") + rType + wxT(" rule make is :"  + ruleInfo.make  ;
	m_types[ruleInfo.name.content] = rType;
	
	if( ruleInfo.make == "import")    {
		
		// We have to add here all informations about the object from which this one is derived
		// this information is contained in the realType variable

		/* for each element in the info object of the grammar */
		CXBindings& grammar = m_interpreterInfo.grammar;
		CXBindingsInfo& info = grammar.GetInfo();
		CXBindingsArrayString genOrder = info.GetGeneration().GetObjects();

		/* The first thing to do is to estabilsh the generation objects dependecy list
		 * So that object will be generated in the right order...
		 */
		CXBindingsArrayString dependencies;

		for( unsigned int i = 0; i < genOrder.size() ; ++i ){

			CXBindingsArrayGrammarObjectInfoMap& objectsInfoMap = m_interpreterInfo.objects;
			CXBindingsArrayGrammarObjectInfo& objectsInfo = objectsInfoMap[genOrder[i]];

			for( unsigned int j = 0; j < objectsInfo.size() ; ++j ) {

				std::string name = objectsInfo[j].properties["name" ];

				name = GetRealType( name , options );
				//wxLogMessage( "Checking bases for") + name + "-") + typeExt + wxT("-"  + realType  ;
				
				if( name == typeExt )  {
					std::pair< std::string , CXBindingsHandlerFileInfo> inf( name , m_objectInfos[name] );
					res.bases.push_back( inf );
					//wxLogMessage( "Adding bases for") + name  ;
					return res;
				}

			}
		}

		std::pair< std::string , CXBindingsHandlerFileInfo> inf( typeExt , m_objectInfos[typeExt] );
		res.bases.push_back( inf );
		return res;
	}
	if( ruleInfo.make == "property")    {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , typeExt );
		res.properties.push_back( mpair );
	}
	else if( ruleInfo.make == "attribute")    {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , typeExt );
		res.attributes.push_back( mpair );
	}
		
	if( ruleInfo.make == "property" || ruleInfo.make == "attribute" || ruleInfo.make == "variant_accessor" || ruleInfo.make == "variant_array_accessor")   {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , realType );
		res.dependencies.push_back( mpair );
	}

	return res;

}

CXBindingsHandlerFileInfo CXBindingsCppHandlersGenerator::DoGenerateRuleCodeFor( CXBindingsRuleInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsHandlerFileInfo res;

	//wxLogMessage( "\t\t Rule make is : ") + ruleInfo.make  ;
	//wxLogMessage( "\t\t Rule name is : ") + ruleInfo.name.content  ;
	//wxLogMessage( "\t\t Rule type is : ") + ruleInfo.type.content  ;
	
	CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
	std::string realType = ruleInfo.type.content;
	
	//wxLogMessage( "\t\t Real type is : ") + realType  ;
	
	if( ruleInfo.make =="object")  
		realType = ruleInfo.name.content;
	
	std::string typeTemplate = ruleInfo.type.stemplate;
	CXBindingsStringStringMap::iterator it = types.find( realType );

	std::string typeExt = GetRealType( realType  , options );
//	if( !IsKnownType( realType ) )
//		typeExt = GetRealType( realType  , options );
//	
	if( it != types.end() ) {
		typeExt = it->second;
		//typeExt = realType;
	}
	
	//wxLogMessage( "\t\t Real type is : ") + realType  ;
	
	if( boost::contains(realType,":") )  
		realType = after_first(realType,':')  ; 
	
	//wxLogMessage( "\t\t Real type is : ") + realType  ;
	
	std::string savedType = realType;
	// Find here type corrspondances
	std::string rType = FindRealTypeFor( realType );
	
	if( ruleInfo.make =="object")  
		rType = "object" ;
	
	if( ruleInfo.make =="child_enumerator")  
		rType = "enum" ;

	if( typeTemplate == "array")    {
		//realType = "std::vector< ") + realType + wxT(" >" ;
		rType = "array"  + rType;
	}
	
	
	//wxLogMessage( "Registering type for : ") + ruleInfo.name.content + " type is :") + realType + wxT(" ") + rType + wxT(" rule make is :"  + ruleInfo.make  ;
	m_types[ruleInfo.name.content] = rType;

	if( ruleInfo.make == "import")    {
		
		// We have to add here all informations about the object from which this one is derived
		// this information is contained in the realType variable

		/* for each element in the info object of the grammar */
		CXBindings& grammar = m_interpreterInfo.grammar;
		CXBindingsInfo& info = grammar.GetInfo();
		CXBindingsArrayString genOrder = info.GetGeneration().GetObjects();

		/* The first thing to do is to estabilsh the generation objects dependecy list
		 * So that object will be generated in the right order...
		 */
		CXBindingsArrayString dependencies;

		for( unsigned int i = 0; i < genOrder.size() ; ++i ){

			CXBindingsArrayGrammarObjectInfoMap& objectsInfoMap = m_interpreterInfo.objects;
			CXBindingsArrayGrammarObjectInfo& objectsInfo = objectsInfoMap[genOrder[i]];

			for( unsigned int j = 0; j < objectsInfo.size() ; ++j ) {

				std::string name = objectsInfo[j].properties["name" ];

				name = GetRealType( name , options );
				//wxLogMessage( "Checking bases for") + name + "-") + typeExt + wxT("-"  + realType  ;
				if( name == typeExt )  {
					std::pair< std::string , CXBindingsHandlerFileInfo> inf( name , m_objectInfos[name] );
					res.bases.push_back( inf );
					//wxLogMessage( "Adding bases for") + name + "-") + typeExt + wxT("-"  + realType  ;
					return res;
				}

			}
		}

		std::pair< std::string , CXBindingsHandlerFileInfo> inf( typeExt , m_objectInfos[typeExt] );
		res.bases.push_back( inf );
		return res;
	}

	
	if( ruleInfo.make == "property")    {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , typeExt );
		res.properties.push_back( mpair );
	}
	else if( ruleInfo.make == "attribute")    {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , typeExt );
		res.attributes.push_back( mpair );
	}
	
	if( ruleInfo.make == "property" || ruleInfo.make == "attribute" || ruleInfo.make == "variant_accessor"  || ruleInfo.make == "variant_array_accessor")   {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , typeExt );
		res.dependencies.push_back( mpair );
	}

	return res;
	
}

CXBindingsHandlerFileInfo CXBindingsCppHandlersGenerator::DoGenerateChildRuleCodeFor( CXBindingsChildInfo& childInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsHandlerFileInfo ret;
	
	CXBindingsArrayGrammarRuleInfo& rules = childInfo.rules;

	for( unsigned int i = 0; i < rules.size() ; ++i ) {

		CXBindingsHandlerFileInfo res = DoGenerateRuleCodeFor( rules[i] , grammar , options );
		ret += res;
	}

	return ret;
}

CXBindingsHandlerFileInfo CXBindingsCppHandlersGenerator::DoGenerateForEachChildRuleCodeFor( CXBindingsChildContainerInfo& , CXBindingsForEachChildInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsHandlerFileInfo ret;
	
	CXBindingsRuleInfoMap& rules = ruleInfo.rules;
	
	CXBindingsRuleInfoMap::iterator it = rules.begin();

	for( ; it != rules.end() ; ++it ) {
		
		CXBindingsArrayGrammarRuleInfo& rInfo = it->second;

		for( unsigned int j = 0; j < rInfo.size() ; ++j ) {

			CXBindingsHandlerFileInfo res = DoGenerateRuleCodeFor( rInfo[j] , grammar , options );
			ret += res;
		}
	}

	return ret;
}

void CXBindingsCppHandlersGenerator::DoGetIncludesListFor( CXBindingsArrayString& local_includes , CXBindingsArrayString currentObjects , CXBindingsHandlerFileInfo& file  , CXBindingsGeneratorOptions& options)
{
	for( unsigned int i = 0; i < file.bases.size() ; ++i ) {
		std::string pName = file.bases[i].first;
		
		CXBindingsArrayString::iterator it = std::find( currentObjects.begin(), currentObjects.end(), pName );
		if( it == currentObjects.end() ) {
			currentObjects.push_back(pName);
			DoGetIncludesListFor( local_includes , currentObjects , file.bases[i].second , options );
			it = std::find( currentObjects.begin(), currentObjects.end(), pName );
			currentObjects.erase(it);
		}
		
		std::string inc = "#include \"" + pName + ".h\"\n" ;
		it = std::find( local_includes.begin(), local_includes.end(), inc );
		if( it == local_includes.end() ) {
			local_includes.push_back("#include \"" + pName + ".h\"\n") ;
		}
	}
	
	for( unsigned int i = 0; i < file.dependencies.size() ; ++i )
	{
		if( m_objectFiles[file.dependencies[i].second] != (options.ns + "_globals") && !m_objectFiles[file.dependencies[i].second].empty() )  {
			std::string inc = "#include \"" + file.dependencies[i].second + ".h\"\n";

			CXBindingsArrayString::iterator it = std::find( local_includes.begin(), local_includes.end(), inc );
			if( it == local_includes.end() ) {
				
				it = std::find( local_includes.begin(), local_includes.end(), file.dependencies[i].first );
				if( it == local_includes.end() ) {
					currentObjects.push_back(file.dependencies[i].first);
					DoGetIncludesListFor( local_includes , currentObjects , m_objectInfos[file.dependencies[i].second] , options );
					it = std::find( currentObjects.begin(), currentObjects.end(), file.dependencies[i].first );
					currentObjects.erase(it);
				}
				
				local_includes.push_back( "#include \"" + file.dependencies[i].second + ".h\"\n"  ) ;
			}
		}
	}
}

std::string CXBindingsCppHandlersGenerator::FindRealTypeFor( const std::string& type )
{
	std::string realType;
	std::string newType = type;
	std::string oldType;

	if( IsBaseType( type ) ) {
		newType = GetBaseType(type);
		oldType = type;
	} else {
		CXBindingsStringStringMap::iterator it = m_types.find( type );
		
		while( it != m_types.end() && newType != oldType)  {
			oldType = newType;
			newType = it->second;
			it = m_types.find( newType );
		}
	}
	
	//wxLogMessage( "oldtype is :") + type + ", new type is : "  + newType  ;
	
	if( newType =="std::string" || newType =="std::string" || newType == "string")   
		newType = "string" ;
	else if( newType =="wxDateTime")  
		newType = "datetime" ;
	else if( newType =="wxTime")  
		newType = "time" ;
	else if( newType =="wxDate")  
		newType = "date" ;
	else if( newType =="wxYear")  
		newType = "year" ;
	else if( newType =="wxYearMonth")  
		newType = "month" ;
	else if( newType =="wxMonth")  
		newType = "month" ;
	else if( newType =="wxMonthDay")  
		newType = "monthday" ;
	else if( newType =="wxDay")  
		newType = "day" ;
	else if( newType =="wxDuration")  
		newType = "duration" ;
	else if( newType =="wxAny")  
		newType = "any" ;
	else if( newType =="unsigned int" || newType == "uint" )   
		newType = "uint" ;
	else if( newType =="unsigned long" || newType == "ulong")   
		newType = "ulong" ;
	else if( newType =="bool")  
		newType = "bool" ;
	else if( newType =="long")  
		newType = "long" ;
	else if( newType =="int")  
		newType = "int" ;
	else if( newType =="integer")  
		newType = "int" ;
	else if( newType =="float")  
		newType = "float" ;
	else if( newType =="double")  
		newType = "double" ;
	else if( boost::contains(type,"Enum"))  
		newType = "enum" ;
	else
		newType = "object" ;
		
	
		
	//wxLogMessage( "oldtype is :") + type + ", new type is : "  + newType  ;
	
	realType = newType;
	return realType;
}


CXBindingsHandlerFileInfo CXBindingsCppHandlersGenerator::DoGenerateChildContainerCodeFor( CXBindingsChildContainerInfo& containerInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsHandlerFileInfo objectFileInfo;

	CXBindingsArrayGrammarRuleInfo& crules = containerInfo.rules;

	for( unsigned int j = 0; j < crules.size() ; ++j ) {

		CXBindingsHandlerFileInfo ruleContent = DoGenerateRuleCodeFor( containerInfo, crules[j] , grammar , options );
		objectFileInfo += ruleContent;
	}

	CXBindingsArrayGrammarForEachChildInfo& childRules = containerInfo.childRules;

	for( unsigned int j = 0; j < childRules.size() ; ++j ) {

		CXBindingsHandlerFileInfo ruleContent = DoGenerateForEachChildRuleCodeFor( containerInfo , childRules[j] , grammar , options );
		objectFileInfo += ruleContent;
	}
	
	CXBindingsArrayGrammarCategoryInfo& categories = containerInfo.categories;

	for( unsigned int j = 0; j < categories.size() ; ++j ) {

		CXBindingsHandlerFileInfo ruleContent = DoGenerateCategoryCodeFor( categories[j] , grammar , options );
		objectFileInfo += ruleContent;
	}
	
	CXBindingsArrayGrammarChildInfo& childs = containerInfo.childs;

	for( unsigned int j = 0; j < childs.size() ; ++j ) {

		CXBindingsHandlerFileInfo ruleContent = DoGenerateChildRuleCodeFor( childs[j] , grammar , options );
		objectFileInfo += ruleContent;
	}

	return objectFileInfo;
}


CXBindingsHandlerFileInfo CXBindingsCppHandlersGenerator::DoGenerateCategoryCodeFor( CXBindingsCategoryInfo& categoryInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsHandlerFileInfo objectFileInfo;

	CXBindingsArrayGrammarChildContainerInfo& containers = categoryInfo.containers;

	for( unsigned int j = 0; j < containers.size() ; ++j ) {

		CXBindingsHandlerFileInfo ruleContent = DoGenerateChildContainerCodeFor( containers[j] , grammar , options );
		objectFileInfo += ruleContent;
	}

	CXBindingsArrayGrammarChildInfo& childs = categoryInfo.childs;

	for( unsigned int j = 0; j < childs.size() ; ++j ) {

		CXBindingsHandlerFileInfo ruleContent = DoGenerateChildRuleCodeFor( childs[j] , grammar , options );
		objectFileInfo += ruleContent;
	}
	
	return objectFileInfo;
}
