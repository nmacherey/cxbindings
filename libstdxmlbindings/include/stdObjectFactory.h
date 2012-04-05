
/**
 * @file stdObjectFactory.h
 * @brief main file for managing extended objects, 
 * registering them and accessing their information
 * @author Nicolas Macherey (nm@graymat.fr)
 * @version 10.4
 */
#ifndef STDOBJECTFACTORY_H
#define STDOBJECTFACTORY_H

#include "stdSingleton.h"

/* imports */
class stdObject;
class stdObjectFactory;

/** define Generator constructor methods for building generically any kind of generators */
typedef stdObject* (*stdObjectConstructor)();

/** define Generator destructor methods for freeing any created generator */
typedef void (*stdObjectDestructor)(stdObject*);



/** @struct stdObjectRegistration
  * @brief main strcuture for storing registration information about the
  * construction and destrcution of a stdObject.
  * @author Nicolas Macherey (nm@graymat.fr)
  * @date 8-May-2010
  */
struct stdObjectRegistration
{
	/****************************************************************
	  * STRUCTURE ACCESSORS
	  **************************************************************/
	/** default constructor */
	stdObjectRegistration( ) : 
		ctor(0),dtor(0)
	{};

	/** copy constructor */
	stdObjectRegistration( const stdObjectRegistration& rhs ):
		name(rhs.name),
		description(rhs.description),
		ctor(rhs.ctor),
		dtor(rhs.dtor)
	{};

	/** operator= overload */
	stdObjectRegistration& operator=(const stdObjectRegistration& rhs)
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
	stdObjectConstructor ctor;
	stdObjectDestructor dtor;
};

/** now we just have to define a map of stdObjectRegistration in order to manage them in
  * the stdObjectFactory.
  */
typedef std::map< std::string , stdObjectRegistration > stdObjectRegistrationMap;

/** @class stdObjectFactory
  * @brief Main class for handling all available generator in wxGrammar
  *
  * @author Nicolas Macherey (nm@graymat.fr)
  * @date 8-May-2010
  */
class stdObjectFactory : public stdSingleton<stdObjectFactory>
{
	friend class stdSingleton<stdObjectFactory>;

public :
	/*******************************************************************************
    	*   ACCESSORS
    	*******************************************************************************/

    	/** Get the list of registered objects in this manager
    	  * This will provide you with an array string of registered object names
    	  * but not with their description, if you need to get the list of objects
	  * descriptions you should call stdObjectFactory::GetDescriptions.Please
	  * note that this list might change at run time as this factory is made
	  * for adding and removing objects dynamically.
    	  */
	std::vector< std::string > GetList();

	/** Get all descriptions associated to each registered object. Please
	  * note that this list might change at run time as this factory is made
	  * for adding and removing objects dynamically.
	  */
	std::vector< std::string > GetDescriptions();

	/** Register an object in the factory.
	  * Each object can only be registered once.
	  * Please see the stdObjectRegistrant object in order
	  * to know how to register a new object in this factory.
	  * @param name object name
	  * @param description object's description
	  * @param ctor object constructor function pointer
	  * @param dtor object destructor function pointer
	  * @return false if the object already exists in this factory !
	  */
	bool RegisterObject( const std::string& name,
			const std::string& description,
			stdObjectConstructor ctor,
			stdObjectDestructor dtor );

	/** Unregister object from that factory,
	  * Once you have call this method the object will not be available
	  * in this factory.
	  */
	bool UnregisterObject( const std::string& name );

	/** Creates an object accordingly to it's name
	  * You can call this method at any time if you want to create an object
	  * from it's name only.
	  * @param name object's name
	  * @return the requested object instance if exists NULL if it is not existing
	  */
	stdObject* CreateObject( const std::string& name );

	/** Check the existence of an object by it's name
	  * @param name object name to check the existance for
	  * @return false if the object is not registered in the factory
	  */
	bool Exists( const std::string& name );
	
	/** Find the description associated to the object name
	  * @param name object's name to find the description for
	  * @return a stdEmptyString if the object does not exists in the factory
	  */
	std::string GetDescription( const std::string& name );

private :
	/***********************************************************************************
	  * CONSTRUCTOR AND DESTRUCTOR
	  *********************************************************************************/
	/** Default constructor,
	  * You cannot build directly this class, this class is a global singleton 
	  * you have to use stdObjectFactory::Get() to access the global instance 
	  * of this class.
	  */
	stdObjectFactory();

	/** Default destructor
	  * You shall never have to destroy this class by your own unless you are an
	  * application developper. Anyway if you want to free the global instance of this manager
	  * properly, you have to call stdObjectFactory::Free() to free it's global instance
	  */
	~stdObjectFactory();

	/** container of all registered objects */
	stdObjectRegistrationMap m_objects;
};

/**
  * @class stdObjectRegistrant
  * @brief this main template allows you to register easily your stdObject in the stdObjectFactory
  * 
  * Assuming that your object has the following name : MyObject , an example could be :
  * @code 
  *	namespace {
  *		stdObjectRegistrant<MyObject> registrant( "MyObject" , "Description of my object");
  *	};
  * @endcode
  * This shall be done in the top of MyObject.cpp file.
  * I also wrote a helper in order to write it faster :
  * @code
  * stdREGISTER_OBJECT( MyObject , "Description of my object");
  * @endcode
  *
  * @author Nicolas Macherey (nm@graymat.fr)
  * @date 8-May-2010
  */
template<class T> class stdObjectRegistrant
{

public :
	stdObjectRegistrant( const std::string& name , const std::string& description ):
		m_name(name)
	{
		/* The main task is to call the wxGrammargeneratorFactory and to register the object in 
		 * using our two satic method that will be used to register in the factory */
		stdObjectFactory::Get()->RegisterObject( name , description , &Create , &Free );
	}
	
	~stdObjectRegistrant()
	{
		/* if the factory is down this means that the instance has already been removed from it */
		if(!stdObjectFactory::IsDown()) 
			stdObjectFactory::Get()->UnregisterObject( m_name );
	};
	
	/** The create method is made static, so that the program knows the adress of that method without the
	  * need of making a functor or something complicated.
	  * this method will correspond to the stdObjectConstructor pointer 
	  */
	static stdObject* Create( )
	{
		return new T( );
	}

	/** The free method is made static so that the program knows its adress. It corresponds to the 
	  * stdObjectDestructor function pointer
	  */
	static void Free( stdObject* object )
	{
		delete object;
	}

private :
	std::string m_name;
};

#define stdREGISTER_OBJECT( object , description ) \
	namespace ns##object { \
		stdObjectRegistrant<object> registrant( #object , description ); \
	};

#endif		
		