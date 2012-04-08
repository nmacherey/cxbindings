/**
 * @file CXBindingsCppObjectsGenerator.h
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

#include "generators/CXBindingsCppObjectsGenerator.h"

/* Implements wxExtendedObject RTTI information */
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS( CXBindingsCppObjectsGenerator , CXBindingsGenerator );


CXBINDINGS_REGISTER_GENERATOR( CXBindingsCppObjectsGenerator, "CPPObjects" , "Generate CPP objects from CXBindings definition")  ;

CXBindingsCppObjectsGenerator::~CXBindingsCppObjectsGenerator()
{

}

bool CXBindingsCppObjectsGenerator::DoGenerateCode( CXBindingsGeneratorOptions& options )
{
	/* for each element in the info object of the grammar */
	CXBindings& grammar = m_interpreterInfo.grammar;
	CXBindingsInfo& info = grammar.GetInfo();
	CXBindingsArrayString genOrder = info.GetGeneration().GetObjects();
	CXBindingsArrayString templates = info.GetGeneration().GetTemplates();
	
	/* The first thing to do is to estabilsh the generation objects dependecy list
	 * So that object will be generated in the right order...
	 */
	CXBindingsArrayString dependencies = DoCreateDependencyList( options );

	/** @todo here generate object in their dependencies order is this really usefull .??? */
	for( unsigned int k = 0; k < dependencies.size() ; ++k ) {
		for( unsigned int i = 0; i < genOrder.size() ; ++i ){

			CXBindingsArrayGrammarObjectInfoMap& objectsInfoMap = m_interpreterInfo.objects;
			CXBindingsArrayGrammarObjectInfo& objectsInfo = objectsInfoMap[genOrder[i]];

			for( unsigned int j = 0; j < objectsInfo.size() ; ++j ) {
			        
				std::string realName = objectsInfo[j].properties["name"];
				realName = GetRealType( realName , options );

				if( realName == dependencies[k] )  {
					DoGenerateCodeFor( objectsInfo[j] , grammar , options );
					break;
				}
			}
		}
	}
	
	/** The last step is to generate the globals file and the other files for given objects */
	
	std::string hdrDir = options.genDir + "/include" ;
	std::string srcDir = options.genDir + "/src" ;
	CXBindingsMakeDirRecursively(hdrDir+"/") ;
	CXBindingsMakeDirRecursively(srcDir+"/") ;
	
	/* First generate globals file definitions */
	std::string global_file = GetMacro( "global_file")  ;
	std::string global_file_src = GetMacro( "global_file_src")  ;
	std::string filename = options.ns + "_globals" ;
	SetMacro( "filename" , filename) ;
	SetMacro( "globals_code" , m_globalInfo.headerPublicInfo) ;
	SetMacro( "globals_code_src" , m_globalInfo.srcInfo) ;
	
	DoReplaceMacros( global_file );
	DoReplaceMacros( global_file_src );
	SaveFile( hdrDir + "/" + filename + ".h"  , global_file );
	SaveFile( srcDir + "/" + filename + ".cpp"  , global_file_src) ;
	
	// Generate additional files
	CXBindingsArrayGrammarGeneratorFileAddin& addins = m_genfile->GetAddins();
	for( unsigned int i = 0; i < addins.size() ; ++i ) {
		std::string fname = addins[i].GetName();
		std::string header = addins[i].GetHeader();
		std::string source = addins[i].GetSource();
		
		if( !header.empty() ) {
			DoReplaceMacros( header );
			SaveFile( hdrDir + "/" + fname + ".h"  , header)  ;
		}
		if( !source.empty() ) {
			DoReplaceMacros( source );
			SaveFile( srcDir + "/" + fname + ".cpp"  , source) ;
		}
	}
	
	// Save generated objects code
	CXBindingsFileInfoMap::iterator it = m_objectInfos.begin();
	for( ; it != m_objectInfos.end() ; ++it )
	{
		if( m_objectFiles[it->first] !=filename && !m_objectFiles[it->first].empty() )
		{
			std::string srcFile = srcDir + "/" + m_objectFiles[it->first] + ".cpp" ;
			std::string hdrFile = hdrDir + "/" + m_objectFiles[it->first] + ".h" ;
		
			if( !it->second.srcInfo.empty() ) {
				SaveFile( srcFile , it->second.srcInfo );
			}
			
			SaveFile( hdrFile , it->second.headerPublicInfo );
		}
		
	}
	return false;
}

void CXBindingsCppObjectsGenerator::DoGenerateCodeFor( 
		CXBindingsObjectInfo& objectInfo , 
		CXBindings& grammar , 
		CXBindingsGeneratorOptions& options )
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
	
	SetMacro( "object" , objectName);
	SetMacro( "OBJECT" , upName) ;

	// @todo here the file is not preoprely set
	/* First set up some macros for the given file */
	// filename macros
	std::string filename = m_objectFiles[objectName];
	SetMacro( "filename" , filename) ;

	// STEP 1 : CHECK CHILD CONTAINERS AND RULES
	//wxLogMessage( "\t STEP 1 : Checking for existing rules in the child containers ... ")  ;
        CXBindingsArrayGrammarChildContainerInfo& ccInfo = objectInfo.childs;
	
	//std::string msg = std::string::Format( "\t\t Found %d childcontainers...") , ccInfo.size()  ;
	//wxLogMessage( msg );

	CXBindingsFileInfo objectFileInfo;
	
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

		CXBindingsFileInfo ruleContent = DoGenerateChildRuleCodeFor( childs[j] , grammar , options );
		objectFileInfo += ruleContent;
	}

	//wxLogMessage( "\t STEP 3 : create additional object macros for the given object if exists ... ")  ;

	// Append codes in the childs_header_public_code
	std::string childs_header_public_code = objectFileInfo.headerPublicInfo;
	SetMacro( "childs_header_public_code" , childs_header_public_code);

	std::string childs_header_private_code;
	childs_header_private_code = objectFileInfo.headerPrivateInfo;
	SetMacro( "childs_header_private_code" , childs_header_private_code) ;

	std::string childs_header_protected_code;
	childs_header_protected_code = objectFileInfo.headerProtectedInfo,
	SetMacro( "childs_header_protected_code" , childs_header_protected_code) ;

	std::string childs_src_code = objectFileInfo.srcInfo;
	SetMacro( "childs_src_code" , childs_src_code) ;

	// Finally build the missing macrods for doc comments and other parameters info
	CXBindingsFileParametersMacros params;
	
	/* once all parameters list have been generated, we need to create the macros of derived
	 * objects. This is very important in order to treat the inherit template from the various
	 * rules/
	 */
	std::string object_bases;
	std::string object_bases_default_ctor;
        std::string object_bases_init_list;
	std::string object_bases_copy_list;
	std::string object_bases_list;

	for( unsigned int i = 0; i < objectFileInfo.bases.size() ; ++i ) {
		std::string pName = objectFileInfo.bases[i].first;
		CXBindingsFileInfo& inf = objectFileInfo.bases[i].second;
		
		pName = GetRealType( pName , options );

		CXBindingsFileParametersMacros localp;
		localp.parameters_eqeqop_list = params.parameters_eqeqop_list;
		DoCreateParametersMacrosFor( inf , localp , options );

		params.parameters_doc_list += localp.parameters_doc_list;
		params.parameters_ctor_list += localp.parameters_ctor_list;
		params.parameters_eqop_list += localp.parameters_eqop_list;
		params.parameters_eqeqop_list = localp.parameters_eqeqop_list;

		//wxLogMessage( localp.parameters_doc_list );
		
		if( i != 0 ) {
			object_bases += " , public "  + pName;
			object_bases_list += " , "  + pName;
			object_bases_default_ctor += ",\n\t\t" + pName + "()" ;
			object_bases_copy_list += ",\n\t\t" + pName + "(rhs)" ;
			object_bases_init_list += ",\n\t\t" + pName + "(" + localp.parameters_ctor_list2 + ")" ;
		}
		else {
			object_bases += " public "  + pName;
			object_bases_list += pName;
			object_bases_default_ctor += "\t\t" + pName + "()" ;
			object_bases_copy_list += "\t\t" + pName + "(rhs)" ;
			object_bases_init_list += " \t\t" + pName + "(" + localp.parameters_ctor_list2 + ")" ;
		}
	}

	if( object_bases.empty() ) {
		object_bases = "public $(base_object)" ;
		object_bases_default_ctor = "\t\t$(base_object)()" ;
		object_bases_copy_list = "\t\t$(base_object)()" ;
		object_bases_list = "$(base_object)" ;
		object_bases_init_list = "\t\t$(base_object)()" ;
	}
	
	SetMacro( "object_bases" , object_bases ) ;
	SetMacro( "object_bases_list" , object_bases_list) ;
	SetMacro( "object_bases_default_ctor" , object_bases_default_ctor) ;
	SetMacro( "object_bases_init_list" , object_bases_init_list) ;
	SetMacro( "object_bases_copy_list" , object_bases_copy_list) ;
	
    DoCreateParametersMacrosFor( objectFileInfo , params , options );

    if( params.parameters_eqeqop_list.empty() )
        params.parameters_eqeqop_list = "\t\ttrue";

	SetMacro( "parameters_doc_list" , params.parameters_doc_list ) ;
	SetMacro( "parameters_ctor_list" , params.parameters_ctor_list ) ;
	SetMacro( "parameters_dtor_list" , params.parameters_dtor_list ) ;
	SetMacro( "parameters_init_list" , params.parameters_init_list ) ;
	SetMacro( "parameters_default_init_list" , params.parameters_default_init_list ) ;
	SetMacro( "parameters_copy_list" , params.parameters_copy_list ) ;
	SetMacro( "parameters_eqop_list" , params.parameters_eqop_list ) ;
	SetMacro( "parameters_eqeqop_list" , params.parameters_eqeqop_list ) ;
	SetMacro( "parameters_property_table" , params.parameters_property_table ) ;
	
	if( params.parameters_doc_list.empty() ){
		SetMacro( "object_ctors" , "$(object_simple_ctor_header)" )  ;
	} else {
		SetMacro( "object_ctors" , "$(object_complex_ctor_header)" )  ;
	}
	/* The next step is very important ! 
	 * First we have to establish all direct dependencies in the given property list
	 * and all dependencies of the given bases.
	 * Once this is done, we can get the dependency list and include list to put in the 
	 * source file*/
	
	std::string local_includes = "#include \"" + options.ns + "_globals"  + ".h\"\n" ;
	std::string object_imports;
	
	CXBindingsArrayString lc;
	CXBindingsArrayString oi;
	
	CXBindingsArrayString currObj;
	currObj.push_back(objectName);
	DoGetIncludesListFor(lc,oi,currObj,objectFileInfo,options);
	
	for( unsigned int i = 0; i < lc.size() ; ++i ) {
		local_includes += lc[i];
		object_imports += oi[i];
	}
	
	if( filename != options.ns + "_globals") 
	{
		object_imports += "class " + objectName + ";\n" ;
	}
	
	SetMacro( "local_includes" , local_includes) ;
	SetMacro( "object_imports" , object_imports) ;
	//wxLogMessage( "\t\t\t local_includes is :") + local_includes  ;
	//wxLogMessage( "\t\t\t object_imports is :") + object_imports  ;

		
	//wxLogMessage( "\t END STEP 3...")  ;

	// STEP 4 : CHECK EXISTING RULES IN THE GIVEN OBJECT
	//wxLogMessage( "\t STEP 4 : Checking for existing rules in the object ... ")  ;
	CXBindingsArrayGrammarRuleInfo& rules = objectInfo.rules;
	
	//msg = std::string::Format( "\t\t Found %d rules...") , rules.size()  ;
	//wxLogMessage( msg );

	CXBindingsFileInfo FinalInfo;

	for( unsigned int i = 0; i < rules.size() ; ++i ) {
	
		CXBindingsFileInfo ruleContent = DoGenerateRuleCodeFor( rules[i] , grammar , options );
		FinalInfo += ruleContent;
	}

	if( filename ==options.ns + "_globals")  
		m_globalInfo += objectFileInfo;
	
	objectFileInfo.headerPublicInfo = stdEmptyString;
	objectFileInfo.headerProtectedInfo = stdEmptyString;
	objectFileInfo.headerPrivateInfo = stdEmptyString;
	objectFileInfo.srcInfo = stdEmptyString;
		
	FinalInfo += objectFileInfo;
    //std::cout << objectName << std::endl;
	m_objectInfos[objectName] = FinalInfo;

	//wxLogMessage( "\t END STEP 4...")  ;
}

void CXBindingsCppObjectsGenerator::DoCreateParametersMacrosFor( CXBindingsFileInfo& file , CXBindingsFileParametersMacros& parameters , CXBindingsGeneratorOptions& options )
{
	for( unsigned int i = 0; i < file.properties.size() ; ++i )
	{
		std::string pName = file.properties[i].first;
		std::string pType = file.properties[i].second;

		//wxLogMessage( pName + " - ") + pType  ;

		std::string nameExt = GetPropertyExtension( pName , options );
		
		SetMacro( "content_type" , pType) ;
		SetMacro( "content" , pName) ;
		SetMacro( "content_ext" , nameExt) ;
		std::string str,str2;
		
		str = GetMacro("ctor_var_composer") ;
		str2 = GetMacro( "ctor_var_composer2")  ;
		DoReplaceMacros(str);
		DoReplaceMacros(str2);
		
		if( parameters.parameters_copy_list.empty() && !str.empty() ) {
			parameters.parameters_ctor_list += str;
			parameters.parameters_ctor_list2 += str2;
		}
		else if( !str.empty() ) {
			parameters.parameters_ctor_list += " ,"  + str;
			parameters.parameters_ctor_list2 += " ,"  + str2;
		}
		
		str = GetMacro("var_eqop_composer") ;
		DoReplaceMacros(str);
		parameters.parameters_eqop_list += str;
		
		str = GetMacro("var_definit_composer") ;
		DoReplaceMacros(str);
		parameters.parameters_default_init_list += str;
		
		str = GetMacro("var_init_composer") ;
		DoReplaceMacros(str);
		parameters.parameters_init_list += str;
		
		str = GetMacro("var_copy_composer") ;
		DoReplaceMacros(str);
		parameters.parameters_copy_list += str;
		
		str = GetMacro("var_doc_composer") ;
		DoReplaceMacros(str);
		parameters.parameters_doc_list += str;
		
		str = GetMacro("var_property_composer") ;
		DoReplaceMacros(str);
		parameters.parameters_property_table += str;
		
		if( i < file.properties.size()-1 )
			parameters.parameters_property_table += "\n" ;
		
		str = GetMacro("var_eqeqop_composer") ;
		DoReplaceMacros(str);

		if( i == 0 && parameters.parameters_eqeqop_list.empty() ) {
			parameters.parameters_eqeqop_list += "\t"  + str;
		}
		else {
			parameters.parameters_eqeqop_list += " &&\n\t"  + str;
		}

	}

}

CXBindingsFileInfo CXBindingsCppObjectsGenerator::DoGenerateRuleCodeFor( CXBindingsChildContainerInfo& childContainer , CXBindingsRuleInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsFileInfo res;
	CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
	std::string realType = ruleInfo.type.content;
	std::string typeTemplate = ruleInfo.type.stemplate;
	CXBindingsStringStringMap::iterator it = types.find( realType );
	
	CXBindingsGeneratorFileTemplate* arrayTemplate = NULL;
	
	realType = GetRealType( realType , options );

	std::string typeExt = realType;
	if( !IsKnownType( realType ) )
		typeExt = GetRealType( realType  , options );
	
	if( it != types.end() ) {
		realType = it->second;
		typeExt = realType;
	}

	std::string savedType = typeExt;

	if( typeTemplate == "array")    {
		typeExt = "std::vector< " + typeExt + " >" ; 
		 arrayTemplate = m_genfile->FindTemplate( "array_addons")  ;
	}
	
	std::string nameExt = ruleInfo.name.content;
	nameExt = GetPropertyExtension( nameExt , options );
	SetMacro( "name_extension" , nameExt) ;
	
	SetMacro( "name" , ruleInfo.name.content ) ;
	SetMacro( "type" , typeExt ) ;
	SetMacro( "real_type" , savedType)  ;

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

				if( name == savedType )  {
					std::pair< std::string , CXBindingsFileInfo> inf( name , m_objectInfos[name] );
					res.bases.push_back( inf );

					return res;
				}

			}
		}

		std::pair< std::string , CXBindingsFileInfo> inf( savedType , m_objectInfos[savedType] );
		res.bases.push_back( inf );
		
		return res;
	}

	CXBindingsGeneratorFileTemplate* rTemplate = m_genfile->FindTemplate( ruleInfo.make );

	if( rTemplate == NULL )
		CXB_THROW( "Error cannot find template : " + ruleInfo.make) ;
	
	// manage the various teplates which can be found inside a child container
	if( ruleInfo.make == "child_enumerator")    {
		DoGenerateMacroForChildEnumerator( ruleInfo , childContainer.childs , grammar , options );
		std::string nameExt = GetRealType( ruleInfo.name.content , options );
		SetMacro( "name" , nameExt) ;
	}
	else if( ruleInfo.make == "typedef")    {
		std::string nameExt = GetRealType( ruleInfo.name.content , options );
		SetMacro( "name" , nameExt) ;
	}
	else if( ruleInfo.make == "property" || ruleInfo.make ==  "attribute" )     {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , typeExt );
		res.properties.push_back( mpair );
	}
	else
		CXB_THROW( "Error unknown child container template : " + ruleInfo.make ) ;
		
	if( ruleInfo.make == "property" || ruleInfo.make == "attribute" || ruleInfo.make == "variant_accessor"  || ruleInfo.make ==  "variant_array_accessor")  {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , savedType );
		res.dependencies.push_back( mpair );
	}

	CXBindingsArrayGrammarGeneratorFileObjectPart& objects = rTemplate->GetObjects();

	for( unsigned int i = 0; i < objects.size() ; ++i ) {
		
		std::string content = objects[i].GetContent();
		DoReplaceMacros( content );

		if( objects[i].GetFile() =="header")   
		{
			if( objects[i].GetPermission() == "private")   
				res.headerPrivateInfo += content;
			else if( objects[i].GetPermission() == "protected")   
				res.headerProtectedInfo += content;
			else
				res.headerPublicInfo += content;
		}
		else
			res.srcInfo += content;
	}
	
	if( arrayTemplate != NULL ) {
		CXBindingsArrayGrammarGeneratorFileObjectPart& objectsArray = arrayTemplate->GetObjects();

		for( unsigned int i = 0; i < objectsArray.size() ; ++i ) {
			
			std::string content = objectsArray[i].GetContent();
			DoReplaceMacros( content );

			if( objectsArray[i].GetFile() =="header")   
			{
				if( objectsArray[i].GetPermission() == "private")   
					res.headerPrivateInfo += content;
				else if( objectsArray[i].GetPermission() == "protected")   
					res.headerProtectedInfo += content;
				else
					res.headerPublicInfo += content;
			}
			else
				res.srcInfo += content;
		}
	}

	return res;

}

CXBindingsFileInfo CXBindingsCppObjectsGenerator::DoGenerateRuleCodeFor( CXBindingsRuleInfo& ruleInfo , CXBindings& , CXBindingsGeneratorOptions& options )
{
	CXBindingsFileInfo res;

	//wxLogMessage( "\t\t Rule make is : ") + ruleInfo.make  ;
	//wxLogMessage( "\t\t Rule name is : ") + ruleInfo.name.content  ;
	//wxLogMessage( "\t\t Rule type is : ") + ruleInfo.type.content  ;
	
	
		
	CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
	std::string realType = ruleInfo.type.content;
	std::string typeTemplate = ruleInfo.type.stemplate;
	
	CXBindingsGeneratorFileTemplate* arrayTemplate = NULL;
	
	CXBindingsStringStringMap::iterator it = types.find( realType );

	realType = GetRealType( realType  , options );
		
	std::string typeExt = realType;
	if( !IsKnownType( realType ) )
		typeExt = GetRealType( realType , options );
	
	if( it != types.end() ) {
		realType = it->second;
		typeExt = realType;
	}

	std::string savedType = typeExt;
	
	if( typeTemplate == "array")    {
		typeExt = "std::vector< " + typeExt + " >" ; 
		 arrayTemplate = m_genfile->FindTemplate( "array_addons")  ;
	}
	

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

				if( name == savedType )  {
					std::pair< std::string , CXBindingsFileInfo> inf( name , m_objectInfos[name] );
					res.bases.push_back( inf );

					return res;
				}

			}
		}

		std::pair< std::string , CXBindingsFileInfo> inf( savedType , m_objectInfos[savedType] );
		res.bases.push_back( inf );
		
		return res;
	}

	std::string nameExt = ruleInfo.name.content;
	nameExt = GetPropertyExtension( nameExt , options );
	SetMacro( "name_extension" , nameExt)  ;
	
	SetMacro( "name" , ruleInfo.name.content)  ;
	SetMacro( "type" , typeExt)  ;
	SetMacro( "real_type" , savedType)  ;
	SetMacro( "variable" , ruleInfo.variable.content)  ;

	CXBindingsGeneratorFileTemplate* rTemplate = m_genfile->FindTemplate( ruleInfo.make );

	if( rTemplate == NULL )
		CXB_THROW( "Error cannot find template : " + ruleInfo.make)  ;
	
	if( ruleInfo.make == "property" || ruleInfo.make ==  "attribute" )    {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , typeExt );
		res.properties.push_back( mpair );
	}
	
	if( ruleInfo.make == "property" || ruleInfo.make == "attribute" || ruleInfo.make == "variant_accessor"  || ruleInfo.make == "variant_array_accessor")  {
		std::pair< std::string , std::string > mpair( ruleInfo.name.content , savedType );
		res.dependencies.push_back( mpair );
	}

	CXBindingsArrayGrammarGeneratorFileObjectPart& objects = rTemplate->GetObjects();

	for( unsigned int i = 0; i < objects.size() ; ++i ) {
		
		std::string content = objects[i].GetContent();
		DoReplaceMacros( content );

		if( objects[i].GetFile() =="header")   
		{
			if( objects[i].GetPermission() == "private")   
				res.headerPrivateInfo += content;
			else if( objects[i].GetPermission() == "protected")   
				res.headerProtectedInfo += content;
			else
				res.headerPublicInfo += content;
		}
		else
			res.srcInfo += content;
	}
	
	if( arrayTemplate != NULL ) {
		CXBindingsArrayGrammarGeneratorFileObjectPart& objectsArray = arrayTemplate->GetObjects();

		for( unsigned int i = 0; i < objectsArray.size() ; ++i ) {
			
			std::string content = objectsArray[i].GetContent();
			DoReplaceMacros( content );

			if( objectsArray[i].GetFile() =="header")   
			{
				if( objectsArray[i].GetPermission() == "private")   
					res.headerPrivateInfo += content;
				else if( objectsArray[i].GetPermission() == "protected")   
					res.headerProtectedInfo += content;
				else
					res.headerPublicInfo += content;
			}
			else
				res.srcInfo += content;
		}
	}

	return res;
	
}

void CXBindingsCppObjectsGenerator::DoGenerateMacroForChildEnumerator( CXBindingsRuleInfo& ruleInfo , CXBindingsArrayGrammarChildInfo& childs , CXBindings& , CXBindingsGeneratorOptions& )
{
	/** @todo manage here enumaration for types different from a string */
	if( childs.size() > 0 ) {
		std::string child_enum_list;
		std::string child_string_enum_list;

		for( unsigned int i = 0; i < childs.size() ; ++i ) {
			if( i == 0 )
				child_enum_list += "\t$(ns)" + childs[i].properties[ruleInfo.variable.content] + " = 0,\n" ;
			else
				child_enum_list += "\t$(ns)" + childs[i].properties[ruleInfo.variable.content] + ",\n" ;


			std::string str = GetMacro("string_composer") ;
			SetMacro("content",childs[i].properties[ruleInfo.variable.content]) ;
			DoReplaceMacros( str );
			
			if( i < childs.size()-1 )
				child_string_enum_list += "\t"  + str + ",\n" ;
			else
				child_string_enum_list += "\t"   + str;
		}

		SetMacro( "child_enum_list" , child_enum_list ) ;
		SetMacro( "child_string_enum_list" , child_string_enum_list ) ;
	}

}

CXBindingsFileInfo CXBindingsCppObjectsGenerator::DoGenerateChildRuleCodeFor( CXBindingsChildInfo& childInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsFileInfo ret;
	
	CXBindingsArrayGrammarRuleInfo& rules = childInfo.rules;

	for( unsigned int i = 0; i < rules.size() ; ++i ) {

		CXBindingsFileInfo res = DoGenerateRuleCodeFor( rules[i] , grammar , options );
		ret += res;
	}

	return ret;
}

CXBindingsFileInfo CXBindingsCppObjectsGenerator::DoGenerateForEachChildRuleCodeFor( CXBindingsChildContainerInfo& , CXBindingsForEachChildInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsFileInfo ret;
	
	CXBindingsRuleInfoMap& rules = ruleInfo.rules;
	
	CXBindingsRuleInfoMap::iterator it = rules.begin();

	for( ; it != rules.end() ; ++it ) {
		
		CXBindingsArrayGrammarRuleInfo& rInfo = it->second;

		for( unsigned int j = 0; j < rInfo.size() ; ++j ) {

			CXBindingsFileInfo res = DoGenerateRuleCodeFor( rInfo[j] , grammar , options );
			ret += res;
		}
	}

	return ret;
}

void CXBindingsCppObjectsGenerator::DoGetIncludesListFor( CXBindingsArrayString& local_includes , CXBindingsArrayString& object_imports ,CXBindingsArrayString& currentObjects , CXBindingsFileInfo& file  , CXBindingsGeneratorOptions& options)
{
	for( unsigned int i = 0; i < file.bases.size() ; ++i ) {
		std::string pName = file.bases[i].first;
		
		CXBindingsArrayString::iterator it = std::find( currentObjects.begin(), currentObjects.end(), pName );
		if( it == currentObjects.end() ) {
			currentObjects.push_back(pName);
			DoGetIncludesListFor( local_includes, object_imports , currentObjects , file.bases[i].second , options );
			it = std::find( currentObjects.begin(), currentObjects.end(), pName );
			currentObjects.erase(it);
		}
		
		std::string inc = "#include \"" + pName + ".h\"\n" ;
		it = std::find( local_includes.begin(), local_includes.end(), inc );
		if( it == local_includes.end() ) {
			local_includes.push_back("#include \"" + pName + ".h\"\n") ;
			pName = GetRealType(pName,options);
			object_imports.push_back("class " + pName + ";\n") ;
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
					
                    CXBindingsFileInfoMap::iterator oi = m_objectInfos.find(file.dependencies[i].second);
                    if( oi != m_objectInfos.end() ) 
                        DoGetIncludesListFor( local_includes, object_imports , currentObjects , oi->second , options );
					
                    it = std::find( currentObjects.begin(), currentObjects.end(), file.dependencies[i].first );
                    if( it != currentObjects.end() )
    					currentObjects.erase(it);
				}
				
				local_includes.push_back( "#include \"" + file.dependencies[i].second + ".h\"\n"  ) ;
                std::string dependency = GetRealType( file.dependencies[i].second, options );
                object_imports.push_back( "class " + dependency + ";\n");
			}
		}
	}
}


CXBindingsFileInfo CXBindingsCppObjectsGenerator::DoGenerateChildContainerCodeFor( CXBindingsChildContainerInfo& containerInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsFileInfo objectFileInfo;

	CXBindingsArrayGrammarRuleInfo& crules = containerInfo.rules;

	for( unsigned int j = 0; j < crules.size() ; ++j ) {

		CXBindingsFileInfo ruleContent = DoGenerateRuleCodeFor( containerInfo, crules[j] , grammar , options );
		objectFileInfo += ruleContent;
	}

	CXBindingsArrayGrammarForEachChildInfo& childRules = containerInfo.childRules;

	for( unsigned int j = 0; j < childRules.size() ; ++j ) {

		CXBindingsFileInfo ruleContent = DoGenerateForEachChildRuleCodeFor( containerInfo , childRules[j] , grammar , options );
		objectFileInfo += ruleContent;
	}
	
	CXBindingsArrayGrammarCategoryInfo& categories = containerInfo.categories;

	for( unsigned int j = 0; j < categories.size() ; ++j ) {

		CXBindingsFileInfo ruleContent = DoGenerateCategoryCodeFor( categories[j] , grammar , options );
		objectFileInfo += ruleContent;
	}
	
	CXBindingsArrayGrammarChildInfo& childs = containerInfo.childs;

	for( unsigned int j = 0; j < childs.size() ; ++j ) {

		CXBindingsFileInfo ruleContent = DoGenerateChildRuleCodeFor( childs[j] , grammar , options );
		objectFileInfo += ruleContent;
	}

	return objectFileInfo;
}


CXBindingsFileInfo CXBindingsCppObjectsGenerator::DoGenerateCategoryCodeFor( CXBindingsCategoryInfo& categoryInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsFileInfo objectFileInfo;

	CXBindingsArrayGrammarChildContainerInfo& containers = categoryInfo.containers;

	for( unsigned int j = 0; j < containers.size() ; ++j ) {

		CXBindingsFileInfo ruleContent = DoGenerateChildContainerCodeFor( containers[j] , grammar , options );
		objectFileInfo += ruleContent;
	}

	CXBindingsArrayGrammarChildInfo& childs = categoryInfo.childs;

	for( unsigned int j = 0; j < childs.size() ; ++j ) {

		CXBindingsFileInfo ruleContent = DoGenerateChildRuleCodeFor( childs[j] , grammar , options );
		objectFileInfo += ruleContent;
	}
	
	return objectFileInfo;
}
