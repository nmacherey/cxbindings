/**
 * @file CXBindingsGlobals.cpp
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <stack>


#include <boost/filesystem.hpp>


#include "CXBindingsGlobals.h"


bool CXBindingsMakeDirRecursively(const std::string& full_path, int perms)
{
    boost::filesystem::create_directories( full_path );
    return true;
}

bool CXBindingsMakeDir(const std::string& full_path, int perms )
{
   
    boost::filesystem::create_directory( full_path );
    return true;
}
