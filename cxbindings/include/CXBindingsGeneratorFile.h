/**
 * @file CXBindingsGeneratorFile.h
 * @brief main file for reading a wxgrammar_generator_file in xml format
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSGENERATORFILE_H
#define CXBINDINGSGENERATORFILE_H

/* imports */
class CXBindingsGeneratorFileVar;
class CXBindingsGeneratorFileAddin;
class CXBindingsGeneratorFileObjectPart;
class CXBindingsGeneratorFileTemplate;
class CXBindingsGeneratorFile;

/* arrays */
typedef std::vector< CXBindingsGeneratorFileVar > CXBindingsArrayGrammarGeneratorFileVar;
typedef std::vector< CXBindingsGeneratorFileAddin > CXBindingsArrayGrammarGeneratorFileAddin;
typedef std::vector< CXBindingsGeneratorFileObjectPart > CXBindingsArrayGrammarGeneratorFileObjectPart;
typedef std::vector< CXBindingsGeneratorFileTemplate > CXBindingsArrayGrammarGeneratorFileTemplate;

/** @class CXBindingsGeneratorFileVar
  * @brief main class for handling a variable from the CXBindingsGeneratorFile
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 9-May-2010
  */
class CXBindingsGeneratorFileVar : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileVar );

public :
	/** Default ctor */
	CXBindingsGeneratorFileVar():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsGeneratorFileVar(const CXBindingsGeneratorFileVar& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_content(rhs.m_content)
		{};
		
	/** Operator= overload */
	inline CXBindingsGeneratorFileVar& operator=(const CXBindingsGeneratorFileVar& rhs)
	{
		m_name = rhs.m_name;
		m_content = rhs.m_content;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsGeneratorFileVar& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_content == rhs.m_content 
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for content */
	PROPERTY( std::string , Content , content )
	
private :

	std::string m_name;
	std::string m_content;	
};

/** @class CXBindingsGeneratorFileAddin
  * @brief main class for handling an addin file from the CXBindingsGeneratorFile
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 9-May-2010
  */
class CXBindingsGeneratorFileAddin : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileAddin );

public :
	/** Default ctor */
	CXBindingsGeneratorFileAddin():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsGeneratorFileAddin(const CXBindingsGeneratorFileAddin& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_header(rhs.m_header),
		m_source(rhs.m_source)
		{};
		
	/** Operator= overload */
	inline CXBindingsGeneratorFileAddin& operator=(const CXBindingsGeneratorFileAddin& rhs)
	{
		m_name = rhs.m_name;
		m_header = rhs.m_header;
		m_source = rhs.m_source;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsGeneratorFileAddin& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_header == rhs.m_header &&
				m_source == rhs.m_source
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for header */
	PROPERTY( std::string , Header , header )
	
	/** setter and getter for source */
	PROPERTY( std::string , Source , source )
	
private :

	std::string m_name;
	std::string m_header;
	std::string m_source;
};

/** @class CXBindingsGeneratorFileObjectPart
  * @brief main class for handling a object node from the CXBindingsGeneratorFile
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 9-May-2010
  */
class CXBindingsGeneratorFileObjectPart : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileObjectPart );

public :
	/** Default ctor */
	CXBindingsGeneratorFileObjectPart():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsGeneratorFileObjectPart(const CXBindingsGeneratorFileObjectPart& rhs):
		CXBindingsObjectBase(),
		m_file(rhs.m_file),
		m_permission(rhs.m_permission),
		m_content(rhs.m_content)
		{};
		
	/** Operator= overload */
	inline CXBindingsGeneratorFileObjectPart& operator=(const CXBindingsGeneratorFileObjectPart& rhs)
	{
		m_file = rhs.m_file;
		m_permission = rhs.m_permission;
		m_content = rhs.m_content;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsGeneratorFileObjectPart& rhs) const
	{
		return (
				m_file == rhs.m_file &&
				m_permission == rhs.m_permission &&
				m_content == rhs.m_content
				);
	}
	
	/** setter and getter for file */
	PROPERTY( std::string , File , file )

	/** setter and getter for file */
	PROPERTY( std::string , Permission , permission )
	
	/** setter and getter for content */
	PROPERTY( std::string , Content , content )
	
private :

	std::string m_file;
	std::string m_permission;
	std::string m_content;	
};

/** @class CXBindingsGeneratorFileTypeInfo
  * @brief main class for handling a types_info nodes in the grammar generator file
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 9-May-2010
  */
class CXBindingsGeneratorFileTypeInfo : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileTypeInfo );

public :
	/** Default ctor */
	CXBindingsGeneratorFileTypeInfo():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsGeneratorFileTypeInfo(const CXBindingsGeneratorFileTypeInfo& rhs):
		CXBindingsObjectBase(),
		m_types(rhs.m_types)
		{};
		
	/** Operator= overload */
	inline CXBindingsGeneratorFileTypeInfo& operator=(const CXBindingsGeneratorFileTypeInfo& rhs)
	{
		m_types = rhs.m_types;
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsGeneratorFileTypeInfo& rhs) const
	{
		return (
				m_types == rhs.m_types 
				);
	}
	
	/** setter and getter for type */
	PROPERTY( CXBindingsStringStringMap , Types , types )

private :

	CXBindingsStringStringMap m_types;
};

/** @class CXBindingsGeneratorFileTemplate
  * @brief main class for handling a variable from the CXBindingsGeneratorFile
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 9-May-2010
  */
class CXBindingsGeneratorFileTemplate : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFileTemplate );

public :
	/** Default ctor */
	CXBindingsGeneratorFileTemplate():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsGeneratorFileTemplate(const CXBindingsGeneratorFileTemplate& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_objects(rhs.m_objects)
		{};
		
	/** Operator= overload */
	inline CXBindingsGeneratorFileTemplate& operator=(const CXBindingsGeneratorFileTemplate& rhs)
	{
		m_name = rhs.m_name;
		m_objects = rhs.m_objects;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsGeneratorFileTemplate& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_objects == rhs.m_objects 
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for objects */
	PROPERTY( CXBindingsArrayGrammarGeneratorFileObjectPart , Objects , objects )
	
private :

	std::string m_name;
	CXBindingsArrayGrammarGeneratorFileObjectPart m_objects;	
};

/** @class CXBindingsGeneratorFile
  * @brief main class for handling a variable from the CXBindingsGeneratorFile
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 9-May-2010
  */
class CXBindingsGeneratorFile : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS( CXBindingsGeneratorFile );

public :
	/** Default ctor */
	CXBindingsGeneratorFile():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsGeneratorFile(const CXBindingsGeneratorFile& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_vars(rhs.m_vars),
		m_addins(rhs.m_addins),
		m_templates(rhs.m_templates),
		m_typeInfo(rhs.m_typeInfo)
		{};
		
	/** Operator= overload */
	inline CXBindingsGeneratorFile& operator=(const CXBindingsGeneratorFile& rhs)
	{
		m_name = rhs.m_name;
		m_vars = rhs.m_vars;
		m_addins = rhs.m_addins;
		m_templates = rhs.m_templates;
		m_typeInfo = rhs.m_typeInfo;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsGeneratorFile& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_vars == rhs.m_vars &&
				m_addins == rhs.m_addins &&
				m_templates == rhs.m_templates &&
			        m_typeInfo == rhs.m_typeInfo	
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for variables */
	PROPERTY( CXBindingsArrayGrammarGeneratorFileVar , Variables , vars )
	
	/** setter and getter for addins */
	PROPERTY( CXBindingsArrayGrammarGeneratorFileAddin , Addins , addins )

	/** setter and getter for templates */
	PROPERTY( CXBindingsArrayGrammarGeneratorFileTemplate , Templates , templates )

	/** setter and getter for templates */
	PROPERTY( CXBindingsGeneratorFileTypeInfo , TypeInfo , typeInfo )
	
	/** Find the given template */
	CXBindingsGeneratorFileTemplate* FindTemplate( const std::string& name );
	
private :

	std::string m_name;
	CXBindingsArrayGrammarGeneratorFileVar m_vars;
	CXBindingsArrayGrammarGeneratorFileAddin m_addins;
	CXBindingsArrayGrammarGeneratorFileTemplate m_templates;
	CXBindingsGeneratorFileTypeInfo m_typeInfo;
};

#endif

