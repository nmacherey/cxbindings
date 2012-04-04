/**
 * @file CXBindingsCppHandlersGenerator.h
 * @brief main file for generating CPP objects code in CXBindings
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSCPPHANDLERSGENERATOR_H
#define CXBINDINGSCPPHANDLERSGENERATOR_H

/** define an array of pairs */
typedef std::vector< std::pair< std::string , std::string > > CXBindingsArrayPropertyInfo;

/** previous definition */
struct CXBindingsHandlerFileInfo;

/** define an array of CXBindingsHandlerFileInfo */
typedef std::vector< std::pair< std::string , CXBindingsHandlerFileInfo > > CXBindingsArrayGrammarFileInfo;

/** @struct CXBindingsHandlerFileInfo
  * @brief main structure for storing file informations
  */
struct CXBindingsHandlerFileInfo
{
	/** Constructor */
	CXBindingsHandlerFileInfo( ){};

	/** copy constructor */
	CXBindingsHandlerFileInfo( const CXBindingsHandlerFileInfo& rhs ) :
		headerInfo( rhs.headerInfo ),
		srcInfo(rhs.srcInfo),
		properties(rhs.properties),
		dependencies(rhs.dependencies),
		bases(rhs.bases)
	{};

	/** assignment operator */
	CXBindingsHandlerFileInfo& operator=( const CXBindingsHandlerFileInfo& rhs )
	{
		headerInfo = rhs.headerInfo;
		srcInfo = rhs.srcInfo;
		properties = rhs.properties;
		attributes = rhs.attributes;
		dependencies = rhs.dependencies;
		bases = rhs.bases;

		return (*this);
	}

	/** euqality operator */
	bool operator==( const CXBindingsHandlerFileInfo& rhs ) const
	{
		return (
				headerInfo == rhs.headerInfo &&
				srcInfo == rhs.srcInfo &&
				properties == rhs.properties &&
				attributes == rhs.attributes &&
				dependencies == rhs.dependencies &&
				bases == rhs.bases 
		       );
	}

	/** operator += */
	CXBindingsHandlerFileInfo& operator+=( const CXBindingsHandlerFileInfo& rhs )
	{
		headerInfo += rhs.headerInfo;
		srcInfo += rhs.srcInfo;

		for( unsigned int i = 0; i < rhs.properties.size() ; ++i )
			properties.push_back( rhs.properties[i] );
			
		for( unsigned int i = 0; i < rhs.attributes.size() ; ++i )
			attributes.push_back( rhs.attributes[i] );
			
		for( unsigned int i = 0; i < rhs.dependencies.size() ; ++i )
			dependencies.push_back( rhs.dependencies[i] );

		for( unsigned int i = 0; i < rhs.bases.size() ; ++i )
			bases.push_back( rhs.bases[i] );

		return (*this);
	}

	std::string headerInfo;
	std::string srcInfo;
	CXBindingsArrayPropertyInfo properties;
	CXBindingsArrayPropertyInfo attributes;
	CXBindingsArrayPropertyInfo dependencies;
	CXBindingsArrayGrammarFileInfo bases;
};

/** define a map of CXBindingsHandlerFileInfo in order to store objects generated informations
  * for use in derived objects
  */
typedef std::map< std::string , CXBindingsHandlerFileInfo > CXBindingsHandlerFileInfoMap;

/** define a structure for handling parameters macros */
struct CXBindingsHandlerFileParametersMacros
{
	/** default ctor */
	CXBindingsHandlerFileParametersMacros(){};

	/** copy ctor */
	CXBindingsHandlerFileParametersMacros(const CXBindingsHandlerFileParametersMacros& rhs):
		attribute_readers( rhs.attribute_readers ),
		attribute_writers( rhs.attribute_writers ),
		property_readers( rhs.property_readers ),
		property_writers( rhs.property_writers )
	{};

	/** assignment operator */
	CXBindingsHandlerFileParametersMacros& operator=( const CXBindingsHandlerFileParametersMacros& rhs )
	{
		attribute_readers = rhs.attribute_readers ;
		attribute_writers = rhs.attribute_writers ;
		property_readers = rhs.property_readers ;
		property_writers = rhs.property_writers ;
		
		return (*this);
	};
	
	/** assignment operator */
	CXBindingsHandlerFileParametersMacros& operator+=( const CXBindingsHandlerFileParametersMacros& rhs )
	{
		attribute_readers += rhs.attribute_readers ;
		attribute_writers += rhs.attribute_writers ;
		property_readers += rhs.property_readers ;
		property_writers += rhs.property_writers ;
		
		return (*this);
	};

	std::string attribute_readers;
	std::string attribute_writers;
	std::string property_readers;
	std::string property_writers;
};

/**
  * @class CXBindingsCppHandlersGenerator
  * @brief Main class for generating CPP xml Handlers code in CXBindings
  * 
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsCppHandlersGenerator : public CXBindingsGenerator
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsCppHandlersGenerator)

public :
	
	/** Default constructor */
	CXBindingsCppHandlersGenerator():
		CXBindingsGenerator()
	{};

	/** destructor */
	virtual ~CXBindingsCppHandlersGenerator();
	
	/** This is the method to overload 
	 * This method is taking the CXBindingsCppHandlersGeneratorOptions in input containing all
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
	  * @return a CXBindingsHandlerFileInfo containing the code generated by the rule
	  */
	CXBindingsHandlerFileInfo DoGenerateRuleCodeFor( CXBindingsRuleInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );	
	
	/** This method generates the code of the given grammar rule and returns 
	  * its related string .
	  */
	CXBindingsHandlerFileInfo DoGenerateRuleCodeFor( CXBindingsChildContainerInfo& childContainer , CXBindingsRuleInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );
	
	/** This method generates the code of the given grammar rule and returns 
	  * its related string .
	  * @param childContainer CXBindingsChildContainerInfo as given in output of the CXBindingsInterpreter
	  * @param ruleInfo the CXBindingsForEachChildInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  * @return a CXBindingsHandlerFileInfo containing the code generated by the rule
	  */
	CXBindingsHandlerFileInfo DoGenerateForEachChildRuleCodeFor( CXBindingsChildContainerInfo& childContainer , CXBindingsForEachChildInfo& ruleInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );
	
	/** This method generates the code of the given grammar rule and returns 
	  * its related string .
	  * @param childInfo CXBindingsChildInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  * @return a CXBindingsHandlerFileInfo containing the code generated by the rule
	  */
	CXBindingsHandlerFileInfo DoGenerateChildRuleCodeFor( CXBindingsChildInfo& childInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	/** Build the parameters list 
	  * @param file CXBindingsHandlerFileInfo containing all related code for the given object
	  * @param parameters [out] output structure containing basic macros for the parameters list
	  */
	void DoCreateParametersMacrosFor( CXBindingsHandlerFileInfo& file , CXBindingsHandlerFileParametersMacros& parameters , CXBindingsGeneratorOptions& options );
	
	/** Get the property list for all childrens 
	  * @param local_includes [out] array string containing includes for the given file
	  * @param file file info to generate the include list for
	  * @param options generator options
	  */
	void DoGetIncludesListFor( CXBindingsArrayString& local_includes , CXBindingsArrayString currentObjects , CXBindingsHandlerFileInfo& file  , CXBindingsGeneratorOptions& options);
	
	/** This method is made for finding real type for a given type
	 * @param type type to find the real type for
	 * @return the type's associated real type
	 */
	std::string FindRealTypeFor( const std::string& type );

	/** Generate code for the given childcontainer 
	  */
	CXBindingsHandlerFileInfo DoGenerateChildContainerCodeFor( CXBindingsChildContainerInfo& containerInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	/** Generate code for the given category 
	 */
	CXBindingsHandlerFileInfo DoGenerateCategoryCodeFor( CXBindingsCategoryInfo& categoryInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

private :

	CXBindingsHandlerFileInfoMap m_objectInfos;
	CXBindingsHandlerFileInfo m_globalInfo;
	CXBindingsStringStringMap m_types;
};


#endif
