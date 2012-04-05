
/**
 * @file stdXmlReader.h
 * @brief main file for reading an xml file
 * @author Nicolas Macherey (nm@graymat.fr)
 * @version 10.4
 */
#ifndef STD_XMLREADER_H
#define STD_XMLREADER_H

#include "stdSingleton.h"
#include "stdDefines.h"
#include "stdException.h"
#include "stdObject.h"

/* imports */
class stdXmlHandler;
class stdXmlReader;
class stdObject;

/** define XmlHandler constructor methods for building generically any kind of handlers */
typedef stdXmlHandler* (*stdXmlHandlerConstructor)();

/** define XmlHandler destructor methods for freeing any created handler */
typedef void (*stdXmlHandlerDestructor)(stdXmlHandler*);

/** define an array of stdXmlHandler */
typedef std::vector< stdXmlHandler* > stdArrayXmlHandler;

/** @struct stdXmlHandlerRegistration
  * @brief main strcuture for storing registration information about the
  * construction and destrcution of a stdXmlHandler.
  * @author Nicolas Macherey (nm@graymat.fr)
  * @date 8-May-2010
  */
struct stdXmlHandlerRegistration
{
	/****************************************************************
	  * STRUCTURE ACCESSORS
	  **************************************************************/
	/** default constructor */
	stdXmlHandlerRegistration( ) : 
		ctor(0),dtor(0)
	{};

	/** copy constructor */
	stdXmlHandlerRegistration( const stdXmlHandlerRegistration& rhs ):
		name(rhs.name),
		description(rhs.description),
		ctor(rhs.ctor),
		dtor(rhs.dtor)
	{};

	/** operator= overload */
	stdXmlHandlerRegistration& operator=(const stdXmlHandlerRegistration& rhs)
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
	stdXmlHandlerConstructor ctor;
	stdXmlHandlerDestructor dtor;
};

/** now we just have to define a map of stdXmlHandlerRegistration in order to manage them in
  * the stdXmlReader.
  */
typedef std::map< std::string , stdXmlHandlerRegistration > stdXmlHandlerRegistrationMap;

/** @class stdXmlReader
  * @brief Main class for reading an xml file in wxg and in generated files
  * @author Nicolas Macherey (nm@graymat.fr)
  * @date 8-May-2010
  */
class stdXmlReader : public stdSingleton<stdXmlReader>
{
	friend class stdSingleton<stdXmlReader>;
	friend class stdXmlHandler;

public :
	/*******************************************************************************
    	*   ACCESSORS
    	*******************************************************************************/

    	/** Get the list of registered handlers in this manager
    	  * This will provide you with an array string of registered object names
    	  * but not with their description, if you need to get the list of handlers
	  * descriptions you should call stdXmlReader::GetDescriptions. Please
	  * note that this list might change at run time as this factory is made
	  * for adding and removing handlers dynamically.
    	  */
	std::vector< std::string > GetList();

	/** Get all descriptions associated to each registered handler. Please
	  * note that this list might change at run time as this factory is made
	  * for adding and removing handlers dynamically.
	  */
	std::vector< std::string > GetDescriptions();

	/** Register a handler in the factory.
	  * Each handler can only be registered once.
	  * Please see the stdXmlHandlerRegistrant object in order
	  * to know how to register a new handler in this factory.
	  * @param name handler name
	  * @param description handler's description
	  * @param ctor handler constructor function pointer
	  * @param dtor handler destructor function pointer
	  * @return false if the handler already exists in this factory !
	  */
	bool RegisterXmlHandler( const std::string& name,
			const std::string& description,
			stdXmlHandlerConstructor ctor,
			stdXmlHandlerDestructor dtor );

	/** Unregister indicator from that factory,
	  * Once you have call this method the handler will not be available
	  * in this factory.
	  */
	bool UnregisterXmlHandler( const std::string& name );

	/** Creates a handler accordingly to it's name
	  * You can call this method at any time if you want to create a handler
	  * from it's name only.
	  * @param name handlers name
	  * @param g wxg on which the handler will work
	  * @param interpreter wxgInterpreter interpreter for generating the code
	  * @return the requested handler instance if exists NULL if it is not existing
	  */
	stdXmlHandler* CreateXmlHandler( const std::string& name );

	/** Check the existence of a handler by it's name
	  * @param name handler name to check the existance for
	  * @return false if the handler is not registered in the factory
	  */
	bool Exists( const std::string& name );
	
	/** Find the description associated to the handler name
	  * @param name handler's name to find the description for
	  * @return a stdEmptyString if the handler does not exists in the factory
	  */
	std::string GetDescription( const std::string& name );

	/** Remove all handlers associated to the given class from
	  * the global array
	  * @param classInfo class info of the handlers to remove
	  */
	void RemoveHandlersOfClass( const std::string& classInfo );

	/** This is the main stuff of that manager. when you need to read an xml file
	  * using wxgxmlReader, you shall always use this method.
	  * It is taking in input the xml file path to read and returns a stdObject
	  * in output.
	  * The stdObject shall corresponds to your root node of your xml file.
	  */
	stdObject* LoadFile( const std::string& file );
	
	/** Save the resource in the given file 
	 * @param file file in which the resource shall be saved 
	 * @param resource resrouce to save in the file 
	 */
	void SaveFile( const std::string& file , stdObject* resource );

	/** This is a convenience function made for helping stdXmlHandler's to
	  * load resource from children of theirs but which they cannot handle.
	  */
	stdObject *CreateResFromNode(xmlNode *node , const std::string& className, stdObject *resource = NULL, stdXmlHandler* handlerToUse = NULL)
	{
		return node ? DoCreateResFromNode(node , className , resource, handlerToUse)
			: NULL;
	}


	/** Check if the given xmlNode can be handled or not */
	bool HasHandlerFor( xmlNode* node );

	/** This method is used to write resource to into a node */
	xmlNode* WriteResource( stdObject* resource, const char* nodename = NULL , stdXmlHandler* handlerToUse = NULL )
	{
		return resource ? DoWriteResource(resource, nodename, handlerToUse)
			: NULL;
	}


private :
	/***********************************************************************************
	  * CONSTRUCTOR AND DESTRUCTOR
	  *********************************************************************************/
	/** Default constructor,
	  * You cannot build directly this class, this class is a global singleton 
	  * you have to use stdXmlReader::Get() to access the global instance 
	  * of this class.
	  */
	stdXmlReader();

	/** Default destructor
	  * You shall never have to destroy this class by your own unless you are an
	  * application developper. Anyway if you want to free the global instance of this manager
	  * properly, you have to call stdXmlReader::Free() to free it's global instance
	  */
	~stdXmlReader();

	/** Creates a stdObject from a node */
	stdObject *DoCreateResFromNode(xmlNode* node,
				  const std::string& className,
                                  stdObject *resource,
                                  stdXmlHandler *handlerToUse = NULL);

	/** This method is used to write resource to into a node */
	xmlNode* DoWriteResource( stdObject* resource, const char* nodename = NULL , stdXmlHandler* handlerToUse = NULL );

	/** container of all registered handlers */
	stdXmlHandlerRegistrationMap m_registeredHandlers;
	
	/** container of all allocated handlers */
	stdArrayXmlHandler m_handlers;

	/** XML document */
	xmlDoc* m_doc;

	/** XML root node */
	xmlNode* m_root;
};

/**
  * @class stdXmlHandler
  * @brief Abstract class for defining a wxg xml handler
  * 
  * @author Nicolas Macherey (nm@graymat.fr)
  * @date 8-May-2010
  */
class stdXmlHandler : public stdObject
{
	DECLARE_ABSTRACT_CLASS_STD(stdXmlHandler)
	
	/** give stdXmlReader our private members access */
	friend class stdXmlReader;

public :
	/** Default constructor */
	stdXmlHandler();
	
	/** Destructor */
	virtual ~stdXmlHandler()
	{};

   	/** Creates an object any kind of stdObject that can be created using
	  * a stdXmlHandler.
	  * Any handler provides stdXmlReader with a set of methods to
	  * notify if the given resource can be created from a node or not.
	  * the system is quite similar to the wxWidgets XRC system excepts
	  * that any kind of nodes can be handled and not necessarily objects
	  * node from your xml file.
	  * Any way from that information the handler shall be able to create
	  * it's resource from a node. This is the place to do it !
	  */
    	stdObject *CreateResource(xmlNode *node, const std::string& className, stdObject *instance);

	/** Write the given resource in the given parent node
	  * This method is used to write an xml document using the
	  * stdXmlReader system.
	  * The system simply ask the handler to write the given object in
	  * a new node, this node will then be added in the parent properly.
	  * each handler which is handling child nodes will have to add the
	  * returned node if not NULL to its xml tree.
	  */
	xmlNode* WriteResource( stdObject* resource, const char* nodename = NULL );

    	/** This one is called from CreateResource after variables
    	  * were filled.
	  * Any handlers shall overload this metohd in order to complete the 
	  * generation stuff.
	  */
    	virtual stdObject *DoCreateResource() = 0;

	/** This is the method you have to overload to write a resource
	  * to anode simply create a new node and write info in
	  */
	virtual xmlNode* DoWriteResource() = 0;

    	/** @return Returns true if it understands this node and can create
    	  * a resource from it, false otherwise.
	  */
    	virtual bool CanHandle(xmlNode *node) = 0;

	/** @return Returns true if it understands this node and can create
    	  * a resource from it, false otherwise.
	  */
    	virtual bool CanHandle(const std::string& className) = 0;

	/** @return Returns true if it understands the given resource and
	  * can write it into an xmlNode 
	  */
	virtual bool CanHandle( stdObject* resource ) = 0;

protected:
    	// Variables (filled by CreateResource)
    	xmlNode *m_node;
    	std::string m_class;
        std::string m_nodename;
    	stdObject *m_instance;

protected:
	/** Creates children. */
	void CreateChildren(bool this_hnd_only = false);

	/** Helper function. */
	void CreateChildrenPrivately(xmlNode *rootnode = NULL);

	/** Creates a resource from a node. */
	stdObject *CreateResFromNode(xmlNode *node,
			const std::string& className,
			stdObject *instance = NULL)
	{ return stdXmlReader::Get()->CreateResFromNode(node, className, instance); }		
};

/** define a macro for helping creating the instance inside a wxXmlHandler :D */
#define stdMAKE_INSTANCE(variable, classname) \
   classname *variable = NULL; \
   if (m_instance) \
       variable = static_cast<classname*>(m_instance); \
   if (!variable) \
       variable = new classname;

/**
  * @class stdXmlHandlerRegistrant
  * @brief this main template allows you to register easily your stdXmlHandler in the stdXmlReader
  * 
  * Assuming that your handler has the following name : MyXmlHandler , an example could be :
  * @code 
  *	namespace {
  *		stdXmlHandlerRegistrant<MyXmlHandler> registrant( "MyXmlHandler" , "Description of my handler";
  *	};
  * @endcode
  * This shall be done in the top of MyXmlHandler.cpp file.
  * I also wrote a helper in order to write it faster :
  * @code
  * stdREGISTER_XMLHANDLER( MyXmlHandler , "Description of my XmlHandler" );
  * @endcode
  *
  * @author Nicolas Macherey (nm@graymat.fr)
  * @date 8-May-2010
  */
template<class T> class stdXmlHandlerRegistrant
{

public :
	stdXmlHandlerRegistrant( const std::string& name , const std::string& description ):
		m_name(name)
	{
		/* The main task is to call the stdXmlReader and to register the handler in 
		 * using our two satic method that will be used to register in the factory */
		stdXmlReader::Get()->RegisterXmlHandler( name , description , &Create , &Free );
	}
	
	~stdXmlHandlerRegistrant()
	{
		/* if the factory is down this means that the instance has already been removed from it */
		if(!stdXmlReader::IsDown()) 
			stdXmlReader::Get()->UnregisterXmlHandler( m_name );
	};
	
	/** The create method is made static, so that the program knows the adress of that method without the
	  * need of making a functor or something complicated.
	  * this method will correspond to the stdXmlHandlerConstructor pointer 
	  */
	static stdXmlHandler* Create( )
	{
		return new T( );
	}

	/** The free method is made static so that the program knows its adress. It corresponds to the 
	  * stdXmlHandlerDestructor function pointer
	  */
	static void Free( stdXmlHandler* handler )
	{
		delete handler;
	}

private :
	std::string m_name;
};

#define stdREGISTER_XMLHANDLER( object , description ) \
	namespace ns##object { \
		stdXmlHandlerRegistrant<object> registrant( #object , description ); \
	};

#endif
		
