/**
 * @file CXBindingsGeneratorFactory.h
 * @brief main file for managing code generators for CXBindings
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
#include <algorithm>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/fstream.hpp> 
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
 
#include "CXBindingsDefinitions.h"
#include "CXBindingsGlobals.h"
#include "CXBindingsException.h"
#include "CXBindingsExpressionParser.h"
#include "CXBindingsAny.h"
#include "CXBindings.h"
#include "CXBindingsInterpreter.h"
#include "CXBindingsGeneratorFile.h"
#include "CXBindingsInterpreter.h"
#include "CXBindingsXmlReader.h"

#include "CXBindingsGeneratorFactory.h"


IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS( CXBindingsGenerator , CXBindingsObjectBase );

/** Global instance for the CXBindingsGeneratorFactory */
template<> CXBindingsGeneratorFactory* Singleton<CXBindingsGeneratorFactory>::instance = 0;

/** Global isShutdown initialization for the CXBindingsGeneratorFactory */
template<> bool  Singleton<CXBindingsGeneratorFactory>::isShutdown = false;

CXBindingsGeneratorFactory::CXBindingsGeneratorFactory()
{

}

CXBindingsGeneratorFactory::~CXBindingsGeneratorFactory()
{
	m_generators.clear();
}

CXBindingsArrayString CXBindingsGeneratorFactory::GetList()
{
	CXBindingsArrayString ret;

	CXBindingsGeneratorRegistrationMap::iterator it = m_generators.begin();
	for( ; it != m_generators.end() ; ++it )
		ret.push_back(it->first);

	return ret;
}

CXBindingsArrayString CXBindingsGeneratorFactory::GetDescriptions()
{
	CXBindingsArrayString ret;

	CXBindingsGeneratorRegistrationMap::iterator it = m_generators.begin();
	for( ; it != m_generators.end() ; ++it )
		ret.push_back(it->second.description);

	return ret;
}

bool CXBindingsGeneratorFactory::RegisterGenerator( const std::string& name,
		const std::string& description,
		CXBindingsGeneratorConstructor ctor,
		CXBindingsGeneratorDestructor dtor )
{
	CXBindingsGeneratorRegistrationMap::iterator it = m_generators.find(name);

	if( it != m_generators.end() )
		return false;

	CXBindingsGeneratorRegistration info;
	info.name = name;
	info.description = description;
	info.ctor = ctor;
	info.dtor = dtor;

	m_generators[name] = info;

	return true;
}

bool CXBindingsGeneratorFactory::UnregisterGenerator( const std::string& name )
{
	CXBindingsGeneratorRegistrationMap::iterator it = m_generators.find(name);

	if( it == m_generators.end() )
		return false;

	m_generators.erase(it);
	return true;
}

CXBindingsGenerator* CXBindingsGeneratorFactory::CreateGenerator( const std::string& name )
{
	CXBindingsGeneratorRegistrationMap::iterator it = m_generators.find(name);

	if( it == m_generators.end() )
		return NULL;

	CXBindingsGenerator* generator = it->second.ctor( );
	return generator;
}

bool CXBindingsGeneratorFactory::Exists( const std::string& name )
{
	CXBindingsGeneratorRegistrationMap::iterator it = m_generators.find(name);

	if( it == m_generators.end() )
		return false;

	return true;
}

std::string CXBindingsGeneratorFactory::GetDescription( const std::string& name )
{
	CXBindingsGeneratorRegistrationMap::iterator it = m_generators.find(name);

	if( it == m_generators.end() )
		return stdEmptyString;

	return it->second.description;
}

CXBindingsGenerator::~CXBindingsGenerator()
{
	if( m_grammar != NULL )
		delete m_grammar;

	if( m_genfile != NULL )
		delete m_genfile;
}

bool CXBindingsGenerator::GenerateCode( CXBindingsGeneratorOptions& options )
{
	SetDefaultMacros(options);

	m_grammar = (CXBindings*) CXBindingsXmlReader::Get()->LoadFile(options.grammarFile);

	if( m_grammar == NULL )
		CXB_THROW( "Error reading your grammar file : " + options.grammarFile );

	m_interpreterInfo = m_interpreter.DoExtractObjectsInformation(options.baseFile,*m_grammar,options);
	m_genfile = (CXBindingsGeneratorFile*) CXBindingsXmlReader::Get()->LoadFile(options.interpreterFile);
	
	if( m_genfile == NULL )
		CXB_THROW("Error reading your file :" + options.interpreterFile );

	// push all var nodes extracted from the genfile as macros for the Generator
	CXBindingsArrayGrammarGeneratorFileVar& vars = m_genfile->GetVariables( );
	for( unsigned int i = 0; i < vars.size() ; ++i )
		SetMacro( vars[i].GetName() , vars[i].GetContent() );


	CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
	m_baseTypes = types;

	return DoGenerateCode( options );
}


bool CXBindingsGenerator::MacroExists( const std::string& name )
{
	CXBindingsStringStringMap::iterator it = m_macros.find( name );

	if( it == m_macros.end() )
		return false;

	return true;
}

std::string CXBindingsGenerator::GetMacro( const std::string& name )
{
	CXBindingsStringStringMap::iterator it = m_macros.find( name );

	if( it == m_macros.end() )
		return stdEmptyString;

	return it->second;
}

void CXBindingsGenerator::SetMacro( const std::string& name , const std::string& value )
{
	m_macros[name] = value;
}

void CXBindingsGenerator::AppendToMacro( const std::string& name , const std::string& value )
{
	m_macros[name] += value;
}

void CXBindingsGenerator::ClearMacros()
{
	m_macros.clear();
}

void CXBindingsGenerator::SetDefaultMacros(CXBindingsGeneratorOptions& options)
{
	m_macros["generationdir"] = options.genDir;
	m_macros["generationname"] = options.genName;
	m_macros["ns"] = options.ns;
	m_macros["exportmacro"] = options.exportMacro;

	boost::gregorian::date date = boost::gregorian::day_clock::local_day();
    	std::string sDate =  boost::gregorian::to_simple_string(date)   ;
	m_macros["date"] = sDate;
}

// FIXME: I need to unit test this class in order to validate the new regex system
int CXBindingsGenerator::DoReplaceMacros( std::string& str )
{
	boost::regex re( "\\$\\(([a-z_A-Z0-9]*)\\)" );
	
	std::string text=str;
	int notfound = 0;
	boost::cmatch what;
	str = text;
    
    boost::sregex_token_iterator iter(text.begin(), text.end(), re, 0);
    boost::sregex_token_iterator end;
    for( ; iter != end; ++iter ) {        
		std::string macroName = *iter;
        if( macroName[0] == '$' ){
            std::string temp;
            for( unsigned int i = 2; i < macroName.size()-1 ; ++i )
                temp += macroName[i];

            macroName = temp;
        }
		std::string macroValue = GetMacro( macroName );
		if( !MacroExists(macroName) )
			CXB_THROW( "Error missing macro (preventing infinity loops): "+ macroName );
		else {
			boost::replace_all(str,  "$(" + macroName + ")" , macroValue )  ;
        }
	}
	
	return notfound;
}

void CXBindingsGenerator::SaveFile( const std::string& file , const std::string& content )
{
	boost::filesystem::path p(file.c_str());
	 boost::filesystem::ofstream ofs(p); 
	ofs << content;
}

void CXBindingsGenerator::DoCreateObjectDependencyList( CXBindingsArrayString& dependencyList , CXBindingsObjectInfo& objectInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	// Object dependencies are defined by their childs, child containers and categories.
	// Dependencies names are extracted from the related rule information for each object.
	
	CXBindingsStringStringMap& properties = objectInfo.properties;

	if( properties["name"].empty()  )
		CXB_THROW( "Error object with no name cannot continue...")  ;

	//wxLogMessage( "\t Calculating object dependencies for object ") + properties["name" ]  ;

	std::string realName = properties["name" ];
	realName = GetRealType( realName  , options );
	
	CXBindingsArrayString::iterator it = std::find( dependencyList.begin() , dependencyList.end(), realName );
		
	if( it != dependencyList.end() )
		return;

	// STEP 1 : CHECK CHILD CONTAINERS AND RULES
	//wxLogMessage( "\t STEP 1 : Checking for existing child containers dependencies ... ")  ;
        CXBindingsArrayGrammarChildContainerInfo& ccInfo = objectInfo.childs;
	
	//std::string msg = std::string::Format( "\t\t Found %d childcontainers...") , ccInfo.size()  ;
	//wxLogMessage( msg );
	
	for( unsigned int i = 0; i < ccInfo.size() ; ++i ) 
		DoCreateContainerDependencyList( dependencyList , realName , ccInfo[i] , grammar , options );
		
	//wxLogMessage( "\t END STEP 1")  ;
	
	// STEP 2 : CHECK CATEGORIES AND RULES
	//wxLogMessage( "\t STEP 2 : Checking for existing dependencies in the child categories ... ")  ;
        CXBindingsArrayGrammarCategoryInfo& catInfo = objectInfo.categories;
	
	//msg = std::string::Format( "\t\t Found %d categories...") , catInfo.size()  ;
	//wxLogMessage( msg );
	
	for( unsigned int i = 0; i < catInfo.size() ; ++i ) 
		DoCreateCategoryDependencyList( dependencyList , realName , catInfo[i] , grammar , options );
	
	CXBindingsArrayGrammarChildInfo& childs = objectInfo.childInfos;
	for( unsigned int j = 0; j < childs.size() ; ++j ) {
		DoCreateChildDependencyList( dependencyList , realName , childs[j] , grammar , options );
	}
		
	//wxLogMessage( "\t END STEP 2")  ;

	// Add me in the dependencyList

	dependencyList.push_back( realName );

}

CXBindingsArrayString CXBindingsGenerator::DoCreateDependencyList( CXBindingsGeneratorOptions& options )
{
	/* for each element in the info object of the grammar */
	CXBindings& grammar = m_interpreterInfo.grammar;
	CXBindingsInfo& info = grammar.GetInfo();
	CXBindingsArrayString genOrder = info.GetGeneration().GetObjects();
	CXBindingsArrayString templates = info.GetGeneration().GetTemplates();
	
	/* The first thing to do is to estabilsh the generation objects dependecy list
	 * So that object will be generated in the right order...
	 */
	CXBindingsArrayString dependencies;
	
	for( unsigned int i = 0; i < genOrder.size() ; ++i ){

		CXBindingsArrayGrammarObjectInfoMap& objectsInfoMap = m_interpreterInfo.objects;
		CXBindingsArrayGrammarObjectInfo& objectsInfo = objectsInfoMap[genOrder[i]];

		for( unsigned int j = 0; j < objectsInfo.size() ; ++j ) {
			DoCreateObjectDependencyList( dependencies , objectsInfo[j] , grammar , options );
			
			std::string name = objectsInfo[j].properties["name" ];

			name = GetRealType( name , options );

			if( templates[i] == "global_scope")   
				m_objectFiles[name] = options.ns + "_globals" ;
			else
				m_objectFiles[name] = name;
		}
	}
	
	return dependencies;
}

void CXBindingsGenerator::DoCreateChildDependencyList(CXBindingsArrayString& dependencyList , const std::string& parentType , CXBindingsChildInfo& childInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsStringStringMap& properties = childInfo.properties;

	std::string type = properties["type" ];

	//wxLogMessage( "\t Type is : ") + type  ;

	if( type.empty() )
		return;

	type = GetRealType( type , options );

	CXBindingsArrayString::iterator it = std::find( dependencyList.begin() , dependencyList.end(), type );
	if( it != dependencyList.end() || type ==parentType) 
		return;
	

//	CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
//	CXBindingsStringStringMap::iterator it = types.find( type );
//
//	if( it != types.end() )
//		return;
	
	//wxLogMessage( "\t Type (2) is : ") + type  ;
	
	// else find the objectInfo related to this type in the CXBindingsInterpreterInfo */
	CXBindingsInfo& info = grammar.GetInfo();
	CXBindingsArrayString genOrder = info.GetGeneration().GetObjects();
	CXBindingsArrayString templates = info.GetGeneration().GetTemplates();

	for( unsigned int i = 0; i < genOrder.size() ; ++i ){

		CXBindingsArrayGrammarObjectInfoMap& objectsInfoMap = m_interpreterInfo.objects;
		CXBindingsArrayGrammarObjectInfo& objectsInfo = objectsInfoMap[genOrder[i]];

		for( unsigned int j = 0; j < objectsInfo.size() ; ++j ) {
			std::string objName = objectsInfo[j].properties["name" ];

			objName = GetRealType( objName , options );

			if( objName == type )  {
				DoCreateObjectDependencyList( dependencyList , objectsInfo[j] , grammar , options );
				return;
			}
		}
		
	}

	dependencyList.push_back( type );
}


void CXBindingsGenerator::DoCreateCategoryDependencyList( CXBindingsArrayString& dependencyList  , const std::string& realName, CXBindingsCategoryInfo& categoryInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsArrayGrammarChildContainerInfo& containers = categoryInfo.containers;

	for( unsigned int j = 0 ; j < containers.size() ; ++j )
		DoCreateContainerDependencyList( dependencyList , realName , containers[j] , grammar , options );
	
	CXBindingsArrayGrammarChildInfo& childs = categoryInfo.childs;

	for( unsigned int j = 0; j < childs.size() ; ++j ) {
		DoCreateChildDependencyList( dependencyList , realName , childs[j] , grammar , options );
	}
}

void CXBindingsGenerator::DoCreateContainerDependencyList( CXBindingsArrayString& dependencyList  , const std::string& realName, CXBindingsChildContainerInfo& containerInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options )
{
	CXBindingsArrayGrammarRuleInfo& crules = containerInfo.rules;

	for( unsigned int j = 0; j < crules.size() ; ++j ) {
		std::string type = crules[j].type.content;
		std::string make = crules[j].make;

		type = GetRealType( type , options );
		CXBindingsArrayString::iterator it = std::find( dependencyList.begin() , dependencyList.end(), type );	
		if( (make == "import"|| make == "child_enumerator" || make == "typedef" ) && it != dependencyList.end() )   {
			dependencyList.push_back( type );
		}
	}
	
	CXBindingsArrayGrammarCategoryInfo& categories = containerInfo.categories;

	for( unsigned int i = 0; i < categories.size() ; ++i )
		DoCreateCategoryDependencyList( dependencyList , realName , categories[i] , grammar , options );

	CXBindingsArrayGrammarChildInfo& childs = containerInfo.childs;

	for( unsigned int j = 0; j < childs.size() ; ++j ) {
		DoCreateChildDependencyList( dependencyList , realName , childs[j] , grammar , options );
	}
}


std::string CXBindingsGenerator::GetObjectName( const std::string& originalName , CXBindingsGeneratorOptions& options )
{
	std::string ret = originalName;
	ret[0] = to_upper_char( ret[0] );

	return ret;
}

std::string CXBindingsGenerator::GetPropertyExtension( const std::string& originalName , CXBindingsGeneratorOptions& options )
{
	std::string ret = originalName;
	ret[0] = to_upper_char( ret[0] );

	return ret;
}
	
bool CXBindingsGenerator::IsKnownType( const std::string& type )
{
	CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
	CXBindingsStringStringMap::iterator it = types.find( type );

	if( it != types.end() ){
		return true;
	}

	it = types.begin();
	for( ; it != types.end() ; ++it )
		if( it->second == type ) 
			return true;

	return false;
}

bool CXBindingsGenerator::IsBaseType( const std::string& type )
{
	CXBindingsStringStringMap& types = m_baseTypes;
	CXBindingsStringStringMap::iterator it = types.find( type );

	if( it != types.end() ){
		return true;
	}

	it = types.begin();
	for( ; it != types.end() ; ++it )
		if( it->second == type ) 
			return true;

	return false;
}

std::string CXBindingsGenerator::GetBaseType( const std::string& type ) {
	CXBindingsStringStringMap& types = m_baseTypes;
	CXBindingsStringStringMap::iterator it = types.find( type );

	if( it != types.end() ){
		return it->second;
	}

	it = types.begin();
	for( ; it != types.end() ; ++it )
		if( it->second == type ) 
			return it->second;

	return stdEmptyString;
}

std::string CXBindingsGenerator::GetRealType( const std::string& type , CXBindingsGeneratorOptions& options )
{
	std::string realType = type;
	std::string ns;
	
	if( IsKnownType( type ) ) {
		CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
		CXBindingsStringStringMap::iterator it = types.find( type );
		
		if( it == types.end() ) {
			return type;
		}

		return it->second;
	}

	if( boost::algorithm::contains(realType,":") )   {
		ns = before_first( realType , ':' );
		realType = after_first( realType, ':' );
	}
	
	if( ns.empty() )
		ns = options.ns;

	realType = GetObjectName( realType , options );
	realType = ns + realType;

	CXBindingsStringStringMap& types = m_genfile->GetTypeInfo().GetTypes();
	types[type] = realType;
	
	//wxLogMessage( type + " - ") + realType  ;

	return realType;
}
