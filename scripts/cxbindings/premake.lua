--*****************************************************************************
--*	Author:		Nicolas Macherey <nm@graymat.fr>
--*	Date:		21/02/2010
--*	Version:	1.00-beta
--* Copyright (C) 2010 Nicolas Macherey
--*
--* This program is licensed under the terms of the Rheia User License Agreement
--* please refer to License.txt to see the exact terms and conditions.
--*
--*	NOTES:
--*		- use the '/' slash for all paths.
--*****************************************************************************

-- Include the helper functions for all libraries
dofile ("../premake_package_template.lua")

--******* Initial Setup ************
--* Most of the setting are set here.
--**********************************

-- Set the name of your package.
package.name = "cxbindings"
-- Set this if you want a different name for your target than the package's name.
local targetName = "cxbindings"
-- Common setup
package.language = "c++"
-- Set the kind of package you want to create.
--		Options: exe | winexe | lib | dll
package.kind = "dll"

-- Get the library version, this si mainly for uniformization
-- of all libraries so we are sure that they have the same
-- version number for all
local version,version_win = GetLibraryVersion()

-- Set the files to include.
package.files = matchfiles("../../".. targetName .. "/src/*.cpp","../../".. targetName .. "/src/handlers/*.cpp","../../".. targetName .. "/src/generators/*.cpp","../../".. targetName .. "/include/*.h","../../".. targetName .. "/include/handlers/*.h","../../".. targetName .. "/include/generators/*.h")

-- Set the include paths.
package.includepaths = { "../../".. targetName .."/include" }

--******* GENAERAL SETUP **********
--*	Settings that are not dependant
--*	on the operating system.
--*********************************
-- Set up all object dirs for putting objects at the right place
SetUpObjectDirectories( pakage , options ,  "DLLRelease" , "DLLDebug" )

-- Set the default targetName if none is specified.
if ( string.len( targetName ) == 0 ) then
	targetName = package.name
end

package.config["DLLRelease"].target = targetName
package.config["DLLDebug"].target = targetName..""

SetUpDefaultPaths( pakage , "DLLRelease" , "DLLDebug" )
CreateTargetDefinitionsLibrary( package , targetName , "DLLRelease" , "DLLDebug" )

CreateLibraryPostBuildStepFor( package , "DLLRelease" )
CreateLibraryPostBuildStepFor( package , "DLLDebug" )

-- table.insert( package.links , "cxbindingssqlite3" )

-- Make the global definitions for that package
CreateCommonBuildOptions( package , options , {"CXBINDINGS_MAKINGDLL","CXBINDINGS_USEDLL"} )
CreateCompilerFlags( package , "DLLRelease" , "DLLDebug" )
InsertStandardLibraries( package )
CreateRPathOptions( package , "DLLRelease" , "DLLDebug")
CreateXml2Defines( package , "DLLRelease" , "DLLDebug" )
CreateXml2Flags( package , "DLLRelease" , "DLLDebug" )


