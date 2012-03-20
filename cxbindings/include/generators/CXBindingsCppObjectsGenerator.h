/**
 * @file CXBindingsCppObjectsGenerator.h
 * @brief main file for generating CPP objects code in CXBindings
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSCPPOBJECTSGENERATOR_H
#define CXBINDINGSCPPOBJECTSGENERATOR_H

/** define an array of pairs */
typedef std::vector< std::pair< std::string , std::string > > CXBindingsArrayPropertyInfo;

/** previous definition */
struct CXBindingsFileInfo;

/** define an array of CXBindingsFileInfo */
typedef std::vector< std::pair< std::string , CXBindingsFileInfo > > CXBindingsArrayGrammarFileInfo;

/** @struct CXBindingsFileInfo
  * @brief main structure for storing file informations
  */
struct CXBindingsFileInfo
{
	/** Constructor */
	CXBindingsFileInfo( ){};

	/** copy constructor */
	CXBindingsFileInfo( const CXBindingsFileInfo& rhs ) :
		headerPublicInfo( rhs.headerPublicInfo ),
		headerProtectedInfo( rhs.headerProtectedInfo ),
		headerPrivateInfo( rhs.headerPrivateInfo ),
		srcInfo(rhs.srcInfo),
		properties(rhs.properties),
		dependencies(rhs.dependencies),
		bases(rhs.bases)
	{};

	/** assignment operator */
	CXBindingsFileInfo& operator=( const CXBindingsFileInfo& rhs )
	{
		headerPublicInfo = rhs.headerPublicInfo;
		headerProtectedInfo = rhs.headerProtectedInfo;
		headerPrivateInfo = rhs.headerPrivateInfo;
		srcInfo = rhs.srcInfo;
		properties = rhs.properties;
		dependencies = rhs.dependencies;
		bases = rhs.bases;

		return (*this);
	}

	/** euqality operator */
	bool operator==( const CXBindingsFileInfo& rhs ) const
	{
		return (
				headerPublicInfo == rhs.headerPublicInfo &&
				headerProtectedInfo == rhs.headerProtectedInfo &&
				headerPrivateInfo == rhs.headerPrivateInfo &&
				srcInfo == rhs.srcInfo &&
				properties == rhs.properties &&
				dependencies == rhs.dependencies &&
				bases == rhs.bases 
		       );
	}

	/** operator += */
	CXBindingsFileInfo& operator+=( const CXBindingsFileInfo& rhs )
	{
		headerPublicInfo += rhs.headerPublicInfo;
		headerProtectedInfo += rhs.headerProtectedInfo;
		headerPrivateInfo += rhs.headerPrivateInfo;
		srcInfo += rhs.srcInfo;

		for( unsigned int i = 0; i < rhs.properties.size() ; ++i )
			properties.push_back( rhs.properties[i] );
			
		for( unsigned int i = 0; i < rhs.dependencies.size() ; ++i )
			dependencies.push_back( rhs.dependencies[i] );

		for( unsigned int i = 0; i < rhs.bases.size() ; ++i )
			bases.push_back( rhs.bases[i] );

		return (*this);
	}

	std::string headerPublicInfo;
	std::string headerProtectedInfo;
	std::string headerPrivateInfo;
	std::string srcInfo;
	CXBindingsArrayPropertyInfo properties;
	CXBindingsArrayPropertyInfo dependencies;
	CXBindingsArrayGrammarFileInfo bases;
};

/** define a map of CXBindingsFileInfo in order to store objects generated informations
  * for use in derived objects
  */
typedef std::map< std::string , CXBindingsFileInfo > CXBindingsFileInfoMap;

/** define a structure for handling parameters macros */
struct CXBindingsFileParametersMacros
{
	/** default ctor */
	CXBindingsFileParametersMacros(){};

	/** copy ctor */
	CXBindingsFileParametersMacros(const CXBindingsFileParametersMacros& rhs):
		parameters_doc_list( rhs.parameters_doc_list ),
		parameters_ctor_list( rhs.parameters_ctor_list ),
		parameters_dtor_list( rhs.parameters_dtor_list ),
		parameters_ctor_list2( rhs.parameters_ctor_list2 ),
		parameters_init_list( rhs.parameters_init_list ),
		parameters_default_init_list( rhs.parameters_default_init_list ),
		parameters_copy_list( rhs.parameters_copy_list ),
		parameters_eqop_list( rhs.parameters_eqop_list ),
		parameters_eqeqop_list( rhs.parameters_eqeqop_list ),
		parameters_property_table(rhs.parameters_property_table)
	{};

	/** assignment operator */
	CXBindingsFileParametersMacros& operator=( const CXBindingsFileParametersMacros& rhs )
	{
		parameters_doc_list = rhs.parameters_doc_list ;
		parameters_ctor_list = rhs.parameters_ctor_list ;
		parameters_dtor_list = rhs.parameters_dtor_list;
		parameters_ctor_list2 = rhs.parameters_ctor_list2 ;
		parameters_init_list = rhs.parameters_init_list ;
		parameters_default_init_list = rhs.parameters_default_init_list;
		parameters_copy_list = rhs.parameters_copy_list ;
		parameters_eqop_list = rhs.parameters_eqop_list ;
		parameters_eqeqop_list = rhs.parameters_eqeqop_list;
		parameters_property_table = rhs.parameters_property_table;
		
		return (*this);
	};

	std::string parameters_doc_list;
	std::string parameters_ctor_list;
	std::string parameters_dtor_list;
	std::string parameters_ctor_list2;
	std::string parameters_init_list;
	std::string parameters_default_init_list;
	std::string parameters_copy_list;
	std::string parameters_eqop_list;
	std::string parameters_eqeqop_list;
	std::string parameters_property_table;
};

/**
  * @class CXBindingsCppObjectsGenerator
  * @brief Main class for generating CPP objects code in CXBindings
  * 
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsCppObjectsGenerator : public CXBindingsGenerator
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsCppObjectsGenerator)

public :
	
	/** Default constructor */
	CXBindingsCppObjectsGenerator():
		CXBindingsGenerator()
	{};

	/** destructor */
	virtual ~CXBindingsCppObjectsGenerator();
	
	/** This is the method to overload 
	 * This method is taking the CXBindingsCppObjectsGeneratorOptions in input containing all
	 * generation options associated to your grammar.
	 * Any code generator for CXBindings shall overload this method in order to proceed to 
	 * the code generation stuff.
	 */
	virtual bool DoGenerateCode( CXBindingsGeneratorOptions& options );

private :
	
	/** This method will generate the code for the given grammar 
	  * in the global scope, the global scope generator is a global file
	  * named YourNamespaceGlobals.h and .cpp in which all global definitions
	  * shall be written.
	  * @param objectInfo the CXBindingsObjectInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  */
	void DoGenerateCodeFor( CXBindingsObjectInfo& objectInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	/** This method generates the code of the given grammar rule and returns 
	  * its related string .
	  * @param ruleInfo the CXBindingsRuleInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  * @return a CXBindingsFileInfo containing the code generated by the rule
	  */
	CXBindingsFileInfo DoGenerateRuleCodeFor( CXBindingsRuleInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );	
	
	/** This method generates the code of the given grammar rule and returns 
	  * its related string .
	  */
	CXBindingsFileInfo DoGenerateRuleCodeFor( CXBindingsChildContainerInfo& childContainer , CXBindingsRuleInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );
	
	/** This method generates the code of the given grammar rule and returns 
	  * its related string .
	  * @param childContainer CXBindingsChildContainerInfo as given in output of the CXBindingsInterpreter
	  * @param ruleInfo the CXBindingsForEachChildInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  * @return a CXBindingsFileInfo containing the code generated by the rule
	  */
	CXBindingsFileInfo DoGenerateForEachChildRuleCodeFor( CXBindingsChildContainerInfo& childContainer , CXBindingsForEachChildInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );
	
	/** This method generates the code of the given grammar rule and returns 
	  * its related string .
	  * @param childInfo CXBindingsChildInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  * @return a CXBindingsFileInfo containing the code generated by the rule
	  */
	CXBindingsFileInfo DoGenerateChildRuleCodeFor( CXBindingsChildInfo& childInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	/** This method will generate macros for child_enumerator template 
	  * @param ruleInfo CXBindingsRuleInfo as given in output of the CXBindingsInterpreter
	  * @param childs CXBindingsArrayGrammarChildInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  */
	void DoGenerateMacroForChildEnumerator( CXBindingsRuleInfo& ruleInfo , CXBindingsArrayGrammarChildInfo& childs , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	/** Build the parameters list 
	  * @param file CXBindingsFileInfo containing all related code for the given object
	  * @param parameters [out] output structure containing basic macros for the parameters list
	  */
	void DoCreateParametersMacrosFor( CXBindingsFileInfo& file , CXBindingsFileParametersMacros& parameters , CXBindingsGeneratorOptions& options );
	
	/** Get the property list for all childrens 
	  * @param local_includes [out] array string containing includes for the given file
	  * @param object_imports [out] array string containing dependecies class definitions for use in the header
	  * @param file file info to generate the include list for
	  * @param options generator options
	  */
	void DoGetIncludesListFor( CXBindingsArrayString& local_includes , CXBindingsArrayString& object_imports  ,CXBindingsArrayString& currentObjects , CXBindingsFileInfo& file  , CXBindingsGeneratorOptions& options);

	/** Generate code for the given childcontainer 
	  */
	CXBindingsFileInfo DoGenerateChildContainerCodeFor( CXBindingsChildContainerInfo& containerInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	/** Generate code for the given category 
	 */
	CXBindingsFileInfo DoGenerateCategoryCodeFor( CXBindingsCategoryInfo& categoryInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

private :

	CXBindingsFileInfoMap m_objectInfos;
	CXBindingsFileInfo m_globalInfo;
};


#endif
