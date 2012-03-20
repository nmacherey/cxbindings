/**
 * @file CXBindingsXmlReader.h
 * @brief main file for reading an xml file
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSXMLFILE_H
#define CXBINDINGSXMLFILE_H

// These macros defines the CXBindings version number,
// taken from wxWidgets
#define CXBINDINGS_CURRENT_VERSION_MAJOR            1
#define CXBINDINGS_CURRENT_VERSION_MINOR            0
#define CXBINDINGS_CURRENT_VERSION_RELEASE          1
#define CXBINDINGS_CURRENT_VERSION_REVISION         0
#define CXBINDINGS_CURRENT_VERSION_STRING       "1.0.1.0" 

#define CXBINDINGS_CURRENT_VERSION \
                (CXBINDINGS_CURRENT_VERSION_MAJOR * 256*256*256 + \
                 CXBINDINGS_CURRENT_VERSION_MINOR * 256*256 + \
                 CXBINDINGS_CURRENT_VERSION_RELEASE * 256 + \
                 CXBINDINGS_CURRENT_VERSION_REVISION)


/* imports */
class CXBindingsXmlHandler;
class CXBindingsXmlReader;
class CXBindings;
class CXBindingsInterpreter;

/** define XmlHandler constructor methods for building generically any kind of handlers */
typedef CXBindingsXmlHandler* (*CXBindingsXmlHandlerConstructor)();

/** define XmlHandler destructor methods for freeing any created handler */
typedef void (*CXBindingsXmlHandlerDestructor)(CXBindingsXmlHandler*);

/** define an array of CXBindingsXmlHandler */
typedef std::vector< CXBindingsXmlHandler* > CXBindingsArrayGrammarXmlHandler;

/** @struct CXBindingsXmlHandlerRegistration
  * @brief main strcuture for storing registration information about the
  * construction and destrcution of a CXBindingsXmlHandler.
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
struct CXBindingsXmlHandlerRegistration
{
	/****************************************************************
	  * STRUCTURE ACCESSORS
	  **************************************************************/
	/** default constructor */
	CXBindingsXmlHandlerRegistration( ) : 
		ctor(0),dtor(0)
	{};

	/** copy constructor */
	CXBindingsXmlHandlerRegistration( const CXBindingsXmlHandlerRegistration& rhs ):
		name(rhs.name),
		description(rhs.description),
		ctor(rhs.ctor),
		dtor(rhs.dtor)
	{};

	/** operator= overload */
	CXBindingsXmlHandlerRegistration& operator=(const CXBindingsXmlHandlerRegistration& rhs)
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
	CXBindingsXmlHandlerConstructor ctor;
	CXBindingsXmlHandlerDestructor dtor;
};

/** now we just have to define a map of CXBindingsXmlHandlerRegistration in order to manage them in
  * the CXBindingsXmlReader.
  */
typedef std::map< std::string , CXBindingsXmlHandlerRegistration > CXBindingsXmlHandlerRegistrationMap;

/** @class CXBindingsXmlReader
  * @brief Main class for reading an xml file in CXBindings and in generated files
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsXmlReader : public Singleton<CXBindingsXmlReader>
{
	friend class Singleton<CXBindingsXmlReader>;
	friend class CXBindingsXmlHandler;

public :
	/*******************************************************************************
    	*   ACCESSORS
    	*******************************************************************************/

    	/** Get the list of registered handlers in this manager
    	  * This will provide you with an array string of registered object names
    	  * but not with their description, if you need to get the list of handlers
	  * descriptions you should call CXBindingsXmlReader::GetDescriptions. Please
	  * note that this list might change at run time as this factory is made
	  * for adding and removing handlers dynamically.
    	  */
	CXBindingsArrayString GetList();

	/** Get all descriptions associated to each registered handler. Please
	  * note that this list might change at run time as this factory is made
	  * for adding and removing handlers dynamically.
	  */
	CXBindingsArrayString GetDescriptions();

	/** Register a handler in the factory.
	  * Each handler can only be registered once.
	  * Please see the CXBindingsXmlHandlerRegistrant object in order
	  * to know how to register a new handler in this factory.
	  * @param name handler name
	  * @param description handler's description
	  * @param ctor handler constructor function pointer
	  * @param dtor handler destructor function pointer
	  * @return false if the handler already exists in this factory !
	  */
	bool RegisterXmlHandler( const std::string& name,
			const std::string& description,
			CXBindingsXmlHandlerConstructor ctor,
			CXBindingsXmlHandlerDestructor dtor );

	/** Unregister indicator from that factory,
	  * Once you have call this method the handler will not be available
	  * in this factory.
	  */
	bool UnregisterXmlHandler( const std::string& name );

	/** Creates a handler accordingly to it's name
	  * You can call this method at any time if you want to create a handler
	  * from it's name only.
	  * @param name handlers name
	  * @param grammar CXBindings on which the handler will work
	  * @param interpreter CXBindingsInterpreter interpreter for generating the code
	  * @return the requested handler instance if exists NULL if it is not existing
	  */
	CXBindingsXmlHandler* CreateXmlHandler( const std::string& name );

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
	  * using CXBindingsxmlReader, you shall always use this method.
	  * It is taking in input the xml file path to read and returns a CXBindingsObjectBase
	  * in output.
	  * The CXBindingsObjectBase shall corresponds to your root node of your xml file.
	  */
	CXBindingsObjectBase* LoadFile( const std::string& file );

	/** This is a convenience function made for helping CXBindingsXmlHandler's to
	  * load resource from children of theirs but which they cannot handle.
	  */
	CXBindingsObjectBase *CreateResFromNode(xmlNode *node, CXBindingsObjectBase *resource = NULL, CXBindingsXmlHandler* handlerToUse = NULL)
	{
		return node ? DoCreateResFromNode(node, resource, handlerToUse)
			: NULL;
	}


	/** Check if the given xmlNode can be handled or not */
	bool HasHandlerFor( xmlNode* node );

	/** This method is used to write resource to into a node */
	xmlNode* WriteResource( CXBindingsObjectBase* resource , CXBindingsXmlHandler* handlerToUse = NULL )
	{
		return resource ? DoWriteResource(resource, handlerToUse)
			: NULL;
	}


private :
	/***********************************************************************************
	  * CONSTRUCTOR AND DESTRUCTOR
	  *********************************************************************************/
	/** Default constructor,
	  * You cannot build directly this class, this class is a global singleton 
	  * you have to use CXBindingsXmlReader::Get() to access the global instance 
	  * of this class.
	  */
	CXBindingsXmlReader();

	/** Default destructor
	  * You shall never have to destroy this class by your own unless you are an
	  * application developper. Anyway if you want to free the global instance of this manager
	  * properly, you have to call CXBindingsXmlReader::Free() to free it's global instance
	  */
	~CXBindingsXmlReader();

	/** Creates a CXBindingsObjectBase from a node */
	CXBindingsObjectBase *DoCreateResFromNode(xmlNode* node,
                                  CXBindingsObjectBase *resource,
                                  CXBindingsXmlHandler *handlerToUse = NULL);

	/** This method is used to write resource to into a node */
	xmlNode* DoWriteResource( CXBindingsObjectBase* resource , CXBindingsXmlHandler* handlerToUse = NULL );

	/** container of all registered handlers */
	CXBindingsXmlHandlerRegistrationMap m_registeredHandlers;
	
	/** container of all allocated handlers */
	CXBindingsArrayGrammarXmlHandler m_handlers;

	/** XML document */
	xmlDoc* m_doc;

	/** XML root node */
	xmlNode* m_root;
};

/**
  * @class CXBindingsXmlHandler
  * @brief Abstract class for defining a CXBindings xml handler
  * 
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsXmlHandler : public CXBindingsObjectBase
{
	DECLARE_ABSTRACT_CLASS_CXBINDINGS(CXBindingsXmlHandler)
	
	/** give CXBindingsXmlReader our private members access */
	friend class CXBindingsXmlReader;

public :
	/** Default constructor */
	CXBindingsXmlHandler();
	
	/** Destructor */
	virtual ~CXBindingsXmlHandler()
	{};

   	/** Creates an object any kind of CXBindingsObjectBase that can be created using
	  * a CXBindingsXmlHandler.
	  * Any handler provides CXBindingsXmlReader with a set of methods to
	  * notify if the given resource can be created from a node or not.
	  * the system is quite similar to the wxWidgets XRC system excepts
	  * that any kind of nodes can be handled and not necessarily objects
	  * node from your xml file.
	  * Any way from that information the handler shall be able to create
	  * it's resource from a node. This is the place to do it !
	  */
    	CXBindingsObjectBase *CreateResource(xmlNode *node, CXBindingsObjectBase *instance);

	/** Write the given resource in the given parent node
	  * This method is used to write an xml document using the
	  * CXBindingsXmlReader system.
	  * The system simply ask the handler to write the given object in
	  * a new node, this node will then be added in the parent properly.
	  * each handler which is handling child nodes will have to add the
	  * returned node if not NULL to its xml tree.
	  */
	xmlNode* WriteResource( CXBindingsObjectBase* resource );

    	/** This one is called from CreateResource after variables
    	  * were filled.
	  * Any handlers shall overload this metohd in order to complete the 
	  * generation stuff.
	  */
    	virtual CXBindingsObjectBase *DoCreateResource() = 0;

	/** This is the method you have to overload to write a resource
	  * to anode simply create a new node and write info in
	  */
	virtual xmlNode* DoWriteResource() = 0;

    	/** @return Returns true if it understands this node and can create
    	  * a resource from it, false otherwise.
	  */
    	virtual bool CanHandle(xmlNode *node) = 0;

	/** @return Returns true if it understands the given resource and
	  * can write it into an xmlNode 
	  */
	virtual bool CanHandle( CXBindingsObjectBase* resource ) = 0;

protected:
    	// Variables (filled by CreateResource)
    	xmlNode *m_node;
    	std::string m_class;
    	CXBindingsObjectBase *m_instance;

protected:
	/** Creates children. */
	void CreateChildren(bool this_hnd_only = false);

	/** Helper function. */
	void CreateChildrenPrivately(xmlNode *rootnode = NULL);

	/** Creates a resource from a node. */
	CXBindingsObjectBase *CreateResFromNode(xmlNode *node,
			CXBindingsObjectBase *instance = NULL)
	{ return CXBindingsXmlReader::Get()->CreateResFromNode(node, instance); }		
};

/** define a macro for helping creating the instance inside a wxXmlHandler :D */
#define CXBINDINGS_MAKE_INSTANCE(variable, classname) \
   classname *variable = NULL; \
   if (m_instance) \
       variable = static_cast<classname*>(m_instance); \
   if (!variable) \
       variable = new classname;

/**
  * @class CXBindingsXmlHandlerRegistrant
  * @brief this main template allows you to register easily your CXBindingsXmlHandler in the CXBindingsXmlReader
  * 
  * Assuming that your handler has the following name : MyXmlHandler , an example could be :
  * @code 
  *	namespace {
  *		CXBindingsXmlHandlerRegistrant<MyXmlHandler> registrant( "MyXmlHandler") , wxT("Description of my handler" ;
  *	};
  * @endcode
  * This shall be done in the top of MyXmlHandler.cpp file.
  * I also wrote a helper in order to write it faster :
  * @code
  * CXBINDINGS_REGISTER_XMLHANDLER( MyXmlHandler , wxtT("Description of my XmlHandler") );
  * @endcode
  *
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
template<class T> class CXBindingsXmlHandlerRegistrant
{

public :
	CXBindingsXmlHandlerRegistrant( const std::string& name , const std::string& description ):
		m_name(name)
	{
		/* The main task is to call the CXBindingsXmlReader and to register the handler in 
		 * using our two satic method that will be used to register in the factory */
		CXBindingsXmlReader::Get()->RegisterXmlHandler( name , description , &Create , &Free );
	}
	
	~CXBindingsXmlHandlerRegistrant()
	{
		/* if the factory is down this means that the instance has already been removed from it */
		if(!CXBindingsXmlReader::IsDown()) 
			CXBindingsXmlReader::Get()->UnregisterXmlHandler( m_name );
	};
	
	/** The create method is made static, so that the program knows the adress of that method without the
	  * need of making a functor or something complicated.
	  * this method will correspond to the CXBindingsXmlHandlerConstructor pointer 
	  */
	static CXBindingsXmlHandler* Create( )
	{
		return new T( );
	}

	/** The free method is made static so that the program knows its adress. It corresponds to the 
	  * CXBindingsXmlHandlerDestructor function pointer
	  */
	static void Free( CXBindingsXmlHandler* handler )
	{
		delete handler;
	}

private :
	std::string m_name;
};

#define CXBINDINGS_REGISTER_XMLHANDLER( object , description ) \
	namespace ns##object { \
		CXBindingsXmlHandlerRegistrant<object> registrant( #object , description  ); \
	};

#endif

