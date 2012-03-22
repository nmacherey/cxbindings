/**
 * @file CXBindingsGlobals.h
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSGLOBALS_H
#define CXBINDINGSGLOBALS_H


#include <set>
#include <string>

#include "CXBindingsDefinitions.h"
#include "Singleton.h"


	inline std::string after_first( const std::string& str, char c) {
		std::string ret;
		bool found = false;

		for( unsigned int i = 0; i < str.size() ; ++i ) {
			if( found )
				ret += str[i];
			else if( str[i] == c )
				found = true;
		}
		return ret;
	}

	inline std::string before_first( const std::string& str, char c) {
		std::string ret;

		for( unsigned int i = 0; i < str.size() ; ++i ) {
			if( str[i] == c )
				break;
			
			ret += str[i];
		}
		return ret;
	}

/// Return @c str as a proper unicode-compatible string
inline std::string cxC2U(const char* str)
{
    return std::string(str);
}

/// Return @c str as a proper unicode-compatible string
inline std::string cxU2C(const std::string& str)
{
return str;
}

#define cxxU2C( str ) reinterpret_cast<const xmlChar*>( cxU2C( str ).c_str() )
#define cxcU2C( str ) cxU2C( str ).c_str()


/*! Function used for creating a directory with wxMkdir */
extern bool CXBindingsMakeDir(const std::string& full_path, int perms = 0775 );
extern bool CXBindingsMakeDirRecursively(const std::string& full_path, int perms = 0775 );

/** 
 * @struct CXBindingsGeneratorOptions
 * @brief Code generator options gathered into a structure so function prototypes do not change
 * if we change the information contained in that structure
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsGeneratorOptions
{
	/*********************************************************************************
	  * CONSTRUCTORS
	  *******************************************************************************/
	/** default constructor */
	CXBindingsGeneratorOptions() {};
	
	/** copy constructor */
	CXBindingsGeneratorOptions( const CXBindingsGeneratorOptions& rhs ):
		genDir( rhs.genDir ),
		exportMacro( rhs.exportMacro ),
		genName( rhs.genName ),
		ns( rhs.ns ),
		grammarFile(rhs.grammarFile),
		interpreterFile(rhs.interpreterFile),
		baseFile(rhs.baseFile)
	{}

	/** operator= overload */
	CXBindingsGeneratorOptions& operator=( const CXBindingsGeneratorOptions& rhs )
	{
		genDir = rhs.genDir;
		exportMacro = rhs.exportMacro;
		genName = rhs.genName;
		ns = rhs.ns;
		
		grammarFile = rhs.grammarFile;
		interpreterFile = rhs.interpreterFile;
		baseFile = rhs.baseFile;

		return (*this);
	}

	std::string genDir;
	std::string exportMacro;
	std::string genName;
	std::string ns;
	std::string grammarFile;
	std::string interpreterFile;
	std::string baseFile;
};

/** define a map of strings */
typedef std::map< std::string , std::string > CXBindingsStringStringMap;

/** Get the attribute content in the given node
  * @param node node to get the named attribute in
  * @param name attribute name to get
  * @return a stdEmptyString if the attribute does not exists
  */
inline static std::string GetXmlAttr( xmlNode* node , const std::string& name )
{
	xmlChar* attr = xmlGetProp( node , cxxU2C(name) );

	if( attr == NULL )
		return stdEmptyString;

	std::string ret = cxC2U( (char*) attr );
	xmlFree(attr);

	return ret;
}

/** Get the content of the node as a std::string 
  * @param node node to get the content for
  */
inline static std::string GetXmlContent( xmlNode* node )
{
	xmlChar* content = xmlNodeGetContent( node );

	if( content == NULL )
		return stdEmptyString;

	std::string ret = cxC2U( (char*) content );
	xmlFree(content);

	return ret;
}
	
#endif
