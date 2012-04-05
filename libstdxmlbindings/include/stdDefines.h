
/**
 * @file stdDefines.h
 * @author Nicolas Macherey (nm@graymat.fr)
 * @version 10.4
 */
#ifndef STD_DEFINES_H
#define STD_DEFINES_H

#include <string>
#include <set>

const std::string stdEmptyString = "";
#define stdNOT_FOUND -1

#define stdx2C( str ) reinterpret_cast<const xmlChar*>( str.c_str() )
#define stdc2C( str ) str.c_str()

/** Get the attribute content in the given node
  * @param node node to get the named attribute in
  * @param name attribute name to get
  * @return a stdEmptyString if the attribute does not exists
  */
inline std::string GetXmlAttr( xmlNode* node , const std::string& name )
{
	xmlChar* attr = xmlGetProp( node , stdx2C(name) );

	if( attr == NULL )
		return stdEmptyString;

	std::string ret = (char*) attr;
	xmlFree(attr);

	return ret;
}

/** Get the content of the node as a std::string 
  * @param node node to get the content for
  */
inline std::string GetXmlContent( xmlNode* node )
{
	xmlChar* content = xmlNodeGetContent( node );

	if( content == NULL )
		return stdEmptyString;

	std::string ret = (char*) content;
	xmlFree(content);

	return ret;
}
	
#endif		
		