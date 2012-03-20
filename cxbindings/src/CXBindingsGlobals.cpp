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





#include "CXBindingsGlobals.h"


bool CXBindingsMakeDirRecursively(const std::string& full_path, int perms)
{
    if (wxDirExists(full_path)) // early out
        return true;

    CXBindingsArrayString dirs;
    std::string currdir;

    {
        wxFileName tmp(full_path);
        currdir = tmp.GetVolume() + tmp.GetVolumeSeparator() + wxFILE_SEP_PATH;
        dirs = tmp.GetDirs();
    }
    for (size_t i = 0; i < dirs.size(); ++i)
    {
        currdir << dirs[i];
        if (!wxDirExists(currdir) && !wxMkdir(currdir, perms))
            return false;
        currdir << wxFILE_SEP_PATH;
    }
    return true;
}

bool CXBindingsMakeDir(const std::string& full_path, int perms )
{
    if (wxDirExists(full_path))
        return true;

    if (!wxMkdir(full_path, perms))
        return false;

    return true;
}