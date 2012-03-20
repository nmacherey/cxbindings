/**
 * @file CXBindingsGeneratorFactory.h
 * @brief main file for managing code generators for CXBindings
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSGENERATORFACTORY_H
#define CXBINDINGSGENERATORFACTORY_H

#include <string>
#include <boost/algorithm/string.hpp>


/* imports */
class CXBindingsGenerator;
class CXBindingsGeneratorFactory;
class CXBindings;
class CXBindingsInterpreter;

/** define Generator constructor methods for building generically any kind of generators */
typedef CXBindingsGenerator* (*CXBindingsGeneratorConstructor)();

/** define Generator destructor methods for freeing any created generator */
typedef void (*CXBindingsGeneratorDestructor)(CXBindingsGenerator*);



/** @struct CXBindingsGeneratorRegistration
  * @brief main strcuture for storing registration information about the
  * construction and destrcution of a CXBindingsGenerator.
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
struct CXBindingsGeneratorRegistration
{
	/****************************************************************
	  * STRUCTURE ACCESSORS
	  **************************************************************/
	/** default constructor */
	CXBindingsGeneratorRegistration( ) : 
		ctor(0),dtor(0)
	{};

	/** copy constructor */
	CXBindingsGeneratorRegistration( const CXBindingsGeneratorRegistration& rhs ):
		name(rhs.name),
		description(rhs.description),
		ctor(rhs.ctor),
		dtor(rhs.dtor)
	{};

	/** operator= overload */
	CXBindingsGeneratorRegistration& operator=(const CXBindingsGeneratorRegistration& rhs)
	{
		name = rhs.name;
		description = rhs.description;
		ctor = rhs.ctor;
		dtor = rhs.dtor;
		return (*this);
	}


	/****************************************************************
	  * PROPERTIES
	  **************************************************************/
	std::string name;
	std::string description;
	CXBindingsGeneratorConstructor ctor;
	CXBindingsGeneratorDestructor dtor;
};

/** now we just have to define a map of CXBindingsGeneratorRegistration in order to manage them in
  * the CXBindingsGeneratorFactory.
  */
typedef std::map< std::string , CXBindingsGeneratorRegistration > CXBindingsGeneratorRegistrationMap;



/** @class CXBindingsGeneratorFactory
  * @brief Main class for handling all available generator in CXBindings
  *
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsGeneratorFactory : public Singleton<CXBindingsGeneratorFactory>
{
	friend class Singleton<CXBindingsGeneratorFactory>;

public :
	/*******************************************************************************
    	*   ACCESSORS
    	*******************************************************************************/

    	/** Get the list of registered generators in this manager
    	  * This will provide you with an array string of registered object names
    	  * but not with their description, if you need to get the list of generators
	  * descriptions you should call CXBindingsGeneratorFactory::GetDescriptions.Please
	  * note that this list might change at run time as this factory is made
	  * for adding and removing generators dynamically.
    	  */
	CXBindingsArrayString GetList();

	/** Get all descriptions associated to each registered generator. Please
	  * note that this list might change at run time as this factory is made
	  * for adding and removing generators dynamically.
	  */
	CXBindingsArrayString GetDescriptions();

	/** Register a generator in the factory.
	  * Each generator can only be registered once.
	  * Please see the CXBindingsGeneratorRegistrant object in order
	  * to know how to register a new generator in this factory.
	  * @param name generator name
	  * @param description generator's description
	  * @param ctor generator constructor function pointer
	  * @param dtor generator destructor function pointer
	  * @return false if the generator already exists in this factory !
	  */
	bool RegisterGenerator( const std::string& name,
			const std::string& description,
			CXBindingsGeneratorConstructor ctor,
			CXBindingsGeneratorDestructor dtor );

	/** Unregister indicator from that factory,
	  * Once you have call this method the generator will not be available
	  * in this factory.
	  */
	bool UnregisterGenerator( const std::string& name );

	/** Creates a generator accordingly to it's name
	  * You can call this method at any time if you want to create a generator
	  * from it's name only.
	  * @param name generators name
	  * @return the requested generator instance if exists NULL if it is not existing
	  */
	CXBindingsGenerator* CreateGenerator( const std::string& name );

	/** Check the existence of a generator by it's name
	  * @param name generator name to check the existance for
	  * @return false if the generator is not registered in the factory
	  */
	bool Exists( const std::string& name );
	
	/** Find the description associated to the generator name
	  * @param name generator's name to find the description for
	  * @return a stdEmptyString if the generator does not exists in the factory
	  */
	std::string GetDescription( const std::string& name );


private :
	/***********************************************************************************
	  * CONSTRUCTOR AND DESTRUCTOR
	  *********************************************************************************/
	/** Default constructor,
	  * You cannot build directly this class, this class is a global singleton 
	  * you have to use CXBindingsGeneratorFactory::Get() to access the global instance 
	  * of this class.
	  */
	CXBindingsGeneratorFactory();

	/** Default destructor
	  * You shall never have to destroy this class by your own unless you are an
	  * application developper. Anyway if you want to free the global instance of this manager
	  * properly, you have to call CXBindingsGeneratorFactory::Free() to free it's global instance
	  */
	~CXBindingsGeneratorFactory();

	/** container of all registered generators */
	CXBindingsGeneratorRegistrationMap m_generators;
};

/**
  * @class CXBindingsGenerator
  * @brief Abstract class for defining a CXBindings code generator
  * 
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsGenerator : public CXBindingsObjectBase
{
	DECLARE_ABSTRACT_CLASS_CXBINDINGS(CXBindingsGenerator)

public :
	
	/** Default constructor */
	CXBindingsGenerator():
		CXBindingsObjectBase(),
		m_grammar(NULL),
		m_genfile(NULL)
	{};

	/** Default destructor */
	virtual ~CXBindingsGenerator();

	 /** Set some default starting parameters before generating the code and
	   * generate the code associated to this generator.
	   * This method will make a call to DoGenerateCode( options )
	   */
	bool GenerateCode( CXBindingsGeneratorOptions& options );

	/** This is the method to overload 
	 * This method is taking the CXBindingsGeneratorOptions in input containing all
	 * generation options associated to your grammar.
	 * Any code generator for CXBindings shall overload this method in order to proceed to 
	 * the code generation stuff.
	 */
	virtual bool DoGenerateCode( CXBindingsGeneratorOptions& options ) = 0;

	/********************************************************************************************
	  * ACCESSORS
	  ******************************************************************************************/
	/** Get the interpreter info contained in this generator */
	const CXBindingsInterpreterInfo& GetInterpreterInfo() {return m_interpreterInfo;};

	/** Set the interpreter info contained in this generator */
	void SetInterpreterInfo( const CXBindingsInterpreterInfo& info ) {m_interpreterInfo = info;};
	
	/********************************************************************************************
	  * UTILITY FUNCTIONS
	  ******************************************************************************************/
	
	/** tranform the given string to upper case */
	inline void to_upper( std::string& s )
	{
		boost::to_upper(s);	
	}
	
	/**  tansform the given char to upper case if it is not the case */
	inline char to_upper_char( char q )
	{
		char ret = q;
		if (q >= 'a' && q <= 'z')
			ret = q - 32;

		return ret;
	}

	/** Accordingly to future options for naming shame
	  * Get the object name as read from the xml file and
	  * return the object name accordingly to the naming shame
	  * as there are no options in the gui actually for that, it will
	  * put an Upper case on the first object letter
	  */
	std::string GetObjectName( const std::string& originalName  , CXBindingsGeneratorOptions& options );

	/** Accodingly to future options in the naming shame
	  * Get the property setter and getter extension
	  */
	std::string GetPropertyExtension( const std::string& porpertyName , CXBindingsGeneratorOptions& options );

	/** Check if the given type is a nown type or not
	  */
	bool IsKnownType( const std::string& type );
	bool IsBaseType( const std::string& type );
	
	/** Get the name or type with namespace removal */
	std::string GetRealType( const std::string& type , CXBindingsGeneratorOptions& options );
	
	/** Get the base type */
	std::string GetBaseType( const std::string& type );

	/********************************************************************************************
	  * MACROS MANAGEMENT
	  * Macros are variables which can be found in any xml file under the form : $(mymacro)
	  * Any generator is able to manage its specific file macros 
	  * at the very begining some basic macros are defined in the Generator :
	  * generationdir : which corresponds to the selected generation directory in CXBindingsGeneratorOptions
	  * ns : which corresponds to the global namespace
	  * exportmacro : which corresponds to the selected export macros
	  * date : which corresponds to the current date
	  ******************************************************************************************/
	
	/** Check if a macro exists accordingly to its name */
	bool MacroExists( const std::string& name );

	/** Get the value of the given macro */
	std::string GetMacro( const std::string& name );

	/** Set the value of the given macro
	  * If the macro exists it's value will be replaced
	  * if not, the macro will be added to the macro list
	  */
	void SetMacro( const std::string& name , const std::string& value );

	/** Append the given information to the macro */
	void AppendToMacro( const std::string& name , const std::string& value );

	/** Clear all available macros */
	void ClearMacros();

	/** SetUp the default macros as describe above */
	void SetDefaultMacros(CXBindingsGeneratorOptions& options);

	/** Replace macros in the given string 
	 * here all macros in the will be replaced, the input string is in
	 * ouput the new string with all it's macros replaced.
	 * If a macro was missing, the tool will return the number of
	 * macros he wasn't able to replace.
	 * @note Please note that this tool is using regular expressions to find the macros 
	 * available in the string.
	 */
	int DoReplaceMacros( std::string& str );
	
	/** The next method is used to save the given string into a file 
	 * you can call it when ever you need
	 */
	void SaveFile( const std::string& file , const std::string& content );
	
	/** This method will get all dependencies for the given object and fils the object files
	 * accordingly so that after a call to this method you willl be able to have a proper dependency
	 * list for the given grammar.
	 * @param options the generator options if required
	 * @return the full dependency list
	 */
	CXBindingsArrayString DoCreateDependencyList( CXBindingsGeneratorOptions& options );
	
	/** This method will establish the object dependency list in order to build the proper includes
	  * for generating the object.
	  * This is very important in order to know how to generate the objects. Basically you can have two
	  * type of dependency :
	  * 	+ base class : which means that the object will be derived from that one
	  * 	+ property class : whic means that the object embeds the dependency as a one of it's attribute
	  * @param dependencyList [out] array string of extracted dependencies ordered by most independent object
	  * @param objectInfo the CXBindingsObjectInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  */
	void DoCreateObjectDependencyList( CXBindingsArrayString& dependencyList , CXBindingsObjectInfo& objectInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	/** This method will create the childrens dependency list, 
	  * This is only for childrens properties of a given object .
	  * @param dependencyList [out] array string of extracted dependencies ordered by most independent object
	  * @param childInfo the CXBindingsChildInfo as given in output of the CXBindingsInterpreter
	  * @param grammar the grammar as output of the CXBindingsXmlReader for the grammar file
	  * @param options generator options
	  */
	void DoCreateChildDependencyList( CXBindingsArrayString& dependencyList , const std::string& parentType , CXBindingsChildInfo& childInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	void DoCreateCategoryDependencyList( CXBindingsArrayString& dependencyList , const std::string& realName , CXBindingsCategoryInfo& categoryInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

	void DoCreateContainerDependencyList( CXBindingsArrayString& dependencyList , const std::string& realName , CXBindingsChildContainerInfo& containerInfo , CXBindings& grammar , CXBindingsGeneratorOptions& options );

protected :

	/** extracted grammar from options */
	CXBindings* m_grammar;
	
	/** interpreter */
	CXBindingsInterpreter m_interpreter;

	/** generator file */
	CXBindingsGeneratorFile* m_genfile;	

	/** generator information */
	CXBindingsInterpreterInfo m_interpreterInfo;

	/** define the macro map */
	CXBindingsStringStringMap m_macros;
	
	/** define objects generated file names */
	CXBindingsStringStringMap m_objectFiles;
	
	CXBindingsStringStringMap m_baseTypes;
};


/**
  * @class CXBindingsGeneratorRegistrant
  * @brief this main template allows you to register easily your CXBindingsgenerator in the CXBindingsGeneratorFactory
  * 
  * Assuming that your generator has the following name : MyGenerator , an example could be :
  * @code 
  *	namespace {
  *		CXBindingsgeneratorRegistrant<MyGenerator> registrant( "MyGenerator") , wxT("Description of my generator" ;
  *	};
  * @endcode
  * This shall be done in the top of MyGenerator.cpp file.
  * I also wrote a helper in order to write it faster :
  * @code
  * CXBINDINGS_REGISTER_GENERATOR( MyGenerator , wxtT("Description of my Generator") );
  * @endcode
  *
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
template<class T> class CXBindingsGeneratorRegistrant
{

public :
	CXBindingsGeneratorRegistrant( const std::string& name , const std::string& description ):
		m_name(name)
	{
		/* The main task is to call the CXBindingsgeneratorFactory and to register the generator in 
		 * using our two satic method that will be used to register in the factory */
		CXBindingsGeneratorFactory::Get()->RegisterGenerator( name , description , &Create , &Free );
	}
	
	~CXBindingsGeneratorRegistrant()
	{
		/* if the factory is down this means that the instance has already been removed from it */
		if(!CXBindingsGeneratorFactory::IsDown()) 
			CXBindingsGeneratorFactory::Get()->UnregisterGenerator( m_name );
	};
	
	/** The create method is made static, so that the program knows the adress of that method without the
	  * need of making a functor or something complicated.
	  * this method will correspond to the CXBindingsGeneratorConstructor pointer 
	  */
	static CXBindingsGenerator* Create( )
	{
		return new T( );
	}

	/** The free method is made static so that the program knows its adress. It corresponds to the 
	  * CXBindingsGeneratorDestructor function pointer
	  */
	static void Free( CXBindingsGenerator* generator )
	{
		delete generator;
	}

private :
	std::string m_name;
};

#define CXBINDINGS_REGISTER_GENERATOR( object , description ) \
	namespace ns##object { \
		CXBindingsGeneratorRegistrant<object> registrant( #object) , description  ; \
	};

#endif
