--*****************************************************************************
--*	Author:		Nicolas Macherey <nm@graymat.fr>
--*	Date:		21/02/2010
--*	Version:	1.00-beta
--* Copyright (C) 2010 Nicolas Macherey
--*
--* This program is licensed under the terms of the GNU GPL v3.0
--* please refer to License.txt to see the exact terms and conditions.
--*
--*	NOTES:
--*		- use the '/' slash for all paths.
--*****************************************************************************

-- ****************************************************************************
-- GLOBAL VARIABLES
-- ****************************************************************************
wxgrammar_include_dir = "../../include/wxgrammar"
--*****************************************************************************
-- Get the library version to be used with all libraries extension
	--*****************************************************************************
function GetLibraryVersion()
	local version = "10.5-beta"
	local version_win = "10_5_beta"

	return version,version_win
	end

	--*****************************************************************************
	-- Create Common build options for a package this will ensure that all our projects
	-- have excatly the same options.
	-- You have to call that method once for each premake you want to use !
	--*****************************************************************************
function CreateCommonBuildOptions( package , options , additional_flags )

	-- Unicode option
	if ( options["unicode"] ) then
	table.insert( package.defines, { "UNICODE", "_UNICODE" } )
	end

	if ( OS == "windows" ) then
	--******* WINDOWS SETUP ***********
	--*	Settings that are Windows specific.
	--*********************************
	table.insert( package.defines, "_USRDLL" )
	table.insert( package.defines, "USINGDLL" )
	table.insert( package.defines, "HAVE_VSNPRINTF" )
	table.insert( package.defines, "HAVE_CONFIG_H" )
	table.insert( package.defines, "WIN32" )
	table.insert( package.defines, "_WINDOWS")
	table.insert( package.defines, "__STDC__")	
	table.insert( package.defines, "__WIN32__")
	table.insert( package.defines, "HAVE_WIN32_THREADS")
	table.insert( package.defines, "HAVE_COMPILER_TLS")
	table.insert( package.defines, "_MBCS")
	table.insert( package.defines, "_CRT_SECURE_DEPRECATE")
	table.insert( package.defines, "_CRT_SECURE_NO_WARNINGS")
	table.insert( package.defines, "_CRT_NONSTDC_NO_DEPRECATE")
	table.insert( package.defines, "HAVE_CONFIG_H")
	else
	--******* LINUX SETUP *************
	--*	Settings that are Linux specific.
	--*********************************
	-- Set the build options.
	package.buildflags = { "extra-warnings" }

	if ( options["unicode"] ) then
	table.insert( package.buildflags, "unicode" )
	end

	if( not macosx ) then
	table.insert( package.defines, { "LINUX" , "unix" } )
	else
	table.insert( package.defines, { "MACOSX" , "unix" } )
	end
	end

	--Then we have to copy all informations to the targeted package directories
	for i,addition in ipairs(additional_flags) do 
	table.insert( package.defines , addition );
	end

	end

	--*****************************************************************************
	-- This function will create the build options that are specific to Release
	-- and debug oncfiguration. You juste have to give the release and debug name
	-- of your configuration in order to make it work properly !
	--*****************************************************************************
function CreateCompilerFlags( package , release_name , debug_name )
	--******* WINDOWS SETUP ***********
	--*	Settings that are Windows specific.
	--*********************************
	table.insert( package.config[release_name].buildflags, "native-wchar" )
	table.insert( package.config[release_name].buildflags, "optimize-size" )
	table.insert( package.config[release_name].buildflags, "no-symbols" )
	table.insert( package.config[release_name].defines, "NDEBUG" )

	table.insert( package.config[debug_name].buildflags, "native-wchar" )
	table.insert( package.config[debug_name].buildflags, "no-edit-and-continue" )

	table.insert( package.config[debug_name].defines, "DEBUG" )
	table.insert( package.config[debug_name].defines, "_DEBUG" )
	--******* LINUX SETUP *************
	--*	Settings that are Linux specific.
	--*********************************
	if ( string.find( target or "", ".*-gcc" ) or target == "gnu" ) then
	table.insert( package.config[debug_name].buildoptions, "-O0" )
	table.insert( package.config[debug_name].buildoptions, "-g" )
	table.insert( package.config[debug_name].buildoptions, "-fno-strict-aliasing" )
	table.insert( package.config[debug_name].buildoptions, "-W" )
	if( not macosx ) then
	table.insert( package.config[debug_name].buildoptions, "-Ulinux" )
	end
	table.insert( package.config[debug_name].buildoptions, "-Uunix" )
	table.insert( package.config[debug_name].buildoptions, "-fmessage-length=0" )
	table.insert( package.config[debug_name].buildoptions, "-Winvalid-pch" )
	table.insert( package.config[debug_name].buildoptions, "-fexceptions" )
	table.insert( package.config[debug_name].buildoptions, "-fPIC" )

	table.insert( package.config[release_name].buildoptions, "-fno-strict-aliasing" )
	table.insert( package.config[release_name].buildoptions, "-W" )
	if( not macosx ) then
	table.insert( package.config[release_name].buildoptions, "-Ulinux" )
	end
	table.insert( package.config[release_name].buildoptions, "-Uunix" )
	table.insert( package.config[release_name].buildoptions, "-fmessage-length=0" )
	table.insert( package.config[release_name].buildoptions, "-Winvalid-pch" )
	table.insert( package.config[release_name].buildoptions, "-fexceptions" )
	table.insert( package.config[release_name].buildoptions, "-fPIC" )
	table.insert( package.config[release_name].buildoptions, "-O1" )
	table.insert( package.config[release_name].buildoptions, "-O2" )
	table.insert( package.config[release_name].buildoptions, "-O3" )
	table.insert( package.config[release_name].buildoptions, "-fexpensive-optimizations" )
	end
	end

	--*****************************************************************************
	-- The next function will simply add standard required libraries
	-- to the package linking options
	--*****************************************************************************
function InsertStandardLibraries( package )
	if ( OS == "windows" ) then
	table.insert( package.links , { "user32" , "gdi32" , "winspool" , "kernel32" , "comdlg32" , "advapi32" , "shell32" , "ole32" , "oleaut32" , "uuid" , "opengl32" , "winmm" } )
	else

	end
	end

	--*****************************************************************************
	-- This function will happend all wxwidgets defines to your project
	-- for both the given Release and Debug
	--*****************************************************************************
function InsertWxWidegtsDefines( package , release_name , debug_name , options )

	-- wxWidgets version
	local wx_ver = ""
	if( options["with-wx-28"] ) then
	if( windows ) then
	wx_ver = "28"
	else
	wx_ver = "2.8"
	end
	else
	if( windows ) then
	wx_ver = "29"
	else
	wx_ver = "2.9"
	end
	end

	-- Set debug flags
	if ( options["disable-wx-debug"] and ( not windows ) ) then
	debug_option = "--debug=no"
	else
	debug_option = "--debug=yes"
	end

	if ( OS == "windows" ) then
	--******* WINDOWS SETUP ***********
	--*	Settings that are Windows specific.
	--*********************************


	-- Set wxWidgets include paths
	if ( target == "cb-gcc" ) then
	table.insert( package.includepaths, "$(#WX.include)" )
	else
	table.insert( package.includepaths, "$(WXWIN)/include" )
	end

	if ( options["unicode"] ) then
	if ( target == "cb-gcc" ) then
	table.insert( package.config[debug_name].includepaths, "$(#WX.lib)/gcc_dll/mswud" )
	table.insert( package.config[release_name].includepaths, "$(#WX.lib)/gcc_dll/mswuh" )
	elseif ( target == "gnu" or target == "cl-gcc" ) then
	table.insert( package.config[debug_name].includepaths, "$(WXWIN)/lib/gcc_dll/mswud" )
	table.insert( package.config[release_name].includepaths, "$(WXWIN)/lib/gcc_dll/mswuh" )
	else
	table.insert( package.config[debug_name].includepaths, "$(WXWIN)/lib/vc_dll/mswud" )
	table.insert( package.config[release_name].includepaths, "$(WXWIN)/lib/vc_dll/mswuh" )
	end
	else
	if ( target == "cb-gcc" ) then
	table.insert( package.config[debug_name].includepaths, "$(#WX.lib)/gcc_dll/mswd" )
	table.insert( package.config[release_name].includepaths, "$(#WX.lib)/gcc_dll/mswh" )
	elseif ( target == "gnu" or target == "cl-gcc" ) then
	table.insert( package.config[debug_name].includepaths, "$(WXWIN)/lib/gcc_dll/mswd" )
	table.insert( package.config[release_name].includepaths, "$(WXWIN)/lib/gcc_dll/mswh" )
	else
	table.insert( package.config[debug_name].includepaths, "$(WXWIN)/lib/vc_dll/mswd" )
	table.insert( package.config[release_name].includepaths, "$(WXWIN)/lib/vc_dll/mswh" )
	end
	end

	-- Set the Windows defines.
	table.insert( package.defines, { "__WXMSW__" , "__WX__" , "WXUSINGDLL" } )
	else
	--******* LINUX SETUP *************
	--*	Settings that are Linux specific.
	--*********************************
	-- Ignore resource files in Linux.
	table.insert( package.excludes, matchrecursive( "*.rc" ) )

	-- Set wxWidgets build options.
	table.insert( package.config[debug_name].buildoptions, "`wx-config "..debug_option.." --cflags`" )
	table.insert( package.config[release_name].buildoptions, "`wx-config --cflags`" )
	end

	table.insert( package.config[debug_name].defines, { "__WXDEBUG__" } )

	end

	--*****************************************************************************
	-- This function will happend all wxwidgets linker flags to your project
	-- for both the given Release and Debug
	--*****************************************************************************
function InsertWxWidegtsLibs( package , release_name , debug_name , options )

	-- wxWidgets version
	local wx_ver = ""
	if( options["with-wx-28"] ) then
	if( windows ) then
	wx_ver = "28"
	else
	wx_ver = "2.8"
	end
	else
	if( windows ) then
	wx_ver = "29"
	else
	wx_ver = "2.9"
	end
	end

	-- Set debug flags
	if ( options["disable-wx-debug"] and ( not windows ) ) then
	debug_option = "--debug=no"
	else
	debug_option = "--debug=yes"
	end

	if ( OS == "windows" ) then
	--******* WINDOWS SETUP ***********
	--*	Settings that are Windows specific.
	--*********************************

	if ( target == "cb-gcc" ) then
	table.insert( package.libpaths, "$(#WX.lib)/gcc_dll" )
	elseif ( target == "gnu" or target == "cl-gcc" ) then
	table.insert( package.libpaths, "$(WXWIN)/lib/gcc_dll" )
	else
	table.insert( package.libpaths, "$(WXWIN)/lib/vc_dll" )
	end

	-- Set wxWidgets libraries to link.
	if ( options["unicode"] ) then
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_richtext" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_xrc" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_aui" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_media" )
	table.insert( package.config[release_name].links, "wxbase".. wx_ver .. "uh_net" )
	table.insert( package.config[release_name].links, "wxbase".. wx_ver .. "uh_xml" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_adv" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_html" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_core" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_stc" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "uh_propgrid" )
	table.insert( package.config[release_name].links, "wxbase".. wx_ver .. "uh" )
	table.insert( package.config[release_name].links, "wxpngh" )
	table.insert( package.config[release_name].links, "wxjpegh" )
	table.insert( package.config[release_name].links, "wxtiffh" )
	table.insert( package.config[release_name].links, "wxregexuh" )
	table.insert( package.config[release_name].links, "wxzlibh" )
	table.insert( package.config[release_name].links, "wxexpath" )

	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_richtext" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_xrc" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_aui" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_media" )
	table.insert( package.config[debug_name].links, "wxbase".. wx_ver .. "ud_net" )
	table.insert( package.config[debug_name].links, "wxbase".. wx_ver .. "ud_xml" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_adv" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_html" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_core" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_stc" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "ud_propgrid" )
	table.insert( package.config[debug_name].links, "wxbase".. wx_ver .. "ud" )
	table.insert( package.config[debug_name].links, "wxpngd" )
	table.insert( package.config[debug_name].links, "wxjpegd" )
	table.insert( package.config[debug_name].links, "wxtiffd" )
	table.insert( package.config[debug_name].links, "wxregexud" )
	table.insert( package.config[debug_name].links, "wxzlibd" )
	table.insert( package.config[debug_name].links, "wxexpatd" )
	else
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_richtext" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_xrc" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_aui" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_media" )
	table.insert( package.config[release_name].links, "wxbase".. wx_ver .. "h_net" )
	table.insert( package.config[release_name].links, "wxbase".. wx_ver .. "h_xml" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_adv" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_html" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_core" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_stc" )
	table.insert( package.config[release_name].links, "wxmsw".. wx_ver .. "h_propgrid" )
	table.insert( package.config[release_name].links, "wxbase".. wx_ver .."h" )
	table.insert( package.config[release_name].links, "wxpngh" )
	table.insert( package.config[release_name].links, "wxjpegh" )
	table.insert( package.config[release_name].links, "wxtiffh" )
	table.insert( package.config[release_name].links, "wxregexh" )
	table.insert( package.config[release_name].links, "wxzlibh" )
	table.insert( package.config[release_name].links, "wxexpath" )

	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_richtext" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_xrc" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_aui" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_media" )
	table.insert( package.config[debug_name].links, "wxbase".. wx_ver .. "d_net" )
	table.insert( package.config[debug_name].links, "wxbase".. wx_ver .. "d_xml" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_adv" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_html" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_core" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_stc" )
	table.insert( package.config[debug_name].links, "wxmsw".. wx_ver .. "d_propgrid" )
	table.insert( package.config[debug_name].links, "wxbase".. wx_ver .. "d" )
	table.insert( package.config[debug_name].links, "wxpngd" )
	table.insert( package.config[debug_name].links, "wxjpegd" )
	table.insert( package.config[debug_name].links, "wxtiffd" )
	table.insert( package.config[debug_name].links, "wxregexd" )
	table.insert( package.config[debug_name].links, "wxzlibd" )
	table.insert( package.config[debug_name].links, "wxexpatd" )
	end
	else
	--******* LINUX SETUP *************
	--*	Settings that are Linux specific.
	--*********************************
	if( wx_ver == "2.9" ) then
	-- Set the wxWidgets link options.
	table.insert( package.config[debug_name].linkoptions, "`wx-config "..debug_option.." --static=no --libs all`" )
	table.insert( package.config[debug_name].linkoptions, "`xml2-config --libs`" )

	table.insert( package.config[release_name].linkoptions, "`wx-config --debug=no --static=no --libs all`" )
	table.insert( package.config[release_name].linkoptions, "`xml2-config --libs`" )
	else
	-- Set the wxWidgets link options.
	table.insert( package.config[debug_name].linkoptions, "`wx-config "..debug_option.." --libs`" )
	table.insert( package.config[debug_name].linkoptions, "`xml2-config --libs`" )

	table.insert( package.config[release_name].linkoptions, "`wx-config --debug=no --libs`" )
	table.insert( package.config[release_name].linkoptions, "`xml2-config --libs`" )
	end
	end

	end

	--*****************************************************************************
-- This function will create all rpath options (ONLY FOR UNIX LIKE SYSTEMS)
	--*****************************************************************************
function CreateRPathOptions( package , release_name , debug_name )
	if ( OS == "windows" ) then

	else
	if ( string.find( target or "", ".*-gcc" ) or target == "gnu" ) then
	if ( macosx ) then
	table.insert( package.config[release_name].linkoptions, "-Wl,-L../lib" )
	table.insert( package.config[debug_name].linkoptions, "-Wl,-L../lib" )
	elseif ( not windows ) then
	if ( options["rpath"] ) then
	-- need to upgrade premake before this works
	-- table.insert( package.linkoptions, "-Wl,-rpath," .. options[rpath] )
	table.insert( package.config[release_name].linkoptions, "-Wl,-rpath,/usr/lib" )
	table.insert( package.config[debug_name].linkoptions, "-Wl,-rpath,/usr/lib" )
	else
	if ( target == "cb-gcc" ) then
	table.insert( package.config[release_name].linkoptions, "-Wl,-rpath,$``ORIGIN/../lib" )
	table.insert( package.config[debug_name].linkoptions, "-Wl,-rpath,$``ORIGIN/../lib" )
	else
	table.insert( package.config[release_name].linkoptions, "-Wl,-rpath,$$``ORIGIN/../lib" )
	table.insert( package.config[debug_name].linkoptions, "-Wl,-rpath,$$``ORIGIN/../lib" )
	end
	end
	end

	end
	end

	end

	--*****************************************************************************
	-- This function will happend all xmlincludes to your project 
	-- ONLY FOR UNIX LIKE SYSTEMS AS FOR WINDOWS THE PACKAGE IS PROVIDED WIH
	-- RHEIA
	--*****************************************************************************
function CreateXml2Defines( pakage , release_name , debug_name )
	if ( OS == "windows" ) then

	else
	table.insert( package.config[debug_name].buildoptions, "`xml2-config --cflags`" )
	table.insert( package.config[release_name].buildoptions, "`xml2-config --cflags`" )
	end
	end

	--*****************************************************************************
	-- This function will happend all xml2 library flags to your project 
	-- ONLY FOR UNIX LIKE SYSTEMS AS FOR WINDOWS THE PACKAGE IS PROVIDED WIH
	-- RHEIA
	--*****************************************************************************
function CreateXml2Flags( pakage , release_name , debug_name )
	if ( OS == "windows" ) then

	else
	table.insert( package.config[debug_name].linkoptions, "`xml2-config --libs`" )
	table.insert( package.config[release_name].linkoptions, "`xml2-config --libs`" )
	end
	end

	--*****************************************************************************
	-- This function will create the default target name for both debug and release
	-- for your project if your are building a plugin
	--*****************************************************************************
function CreateTargetDefinitionsLibrary( pakage , targetname , release_name , debug_name )

	package.config[release_name].target = targetname
	package.config[debug_name].target = targetname

	if( windows ) then
	package.config[release_name].targetextension =  "dll" 
	package.config[debug_name].targetextension =  "dll"

	package.config[release_name].libdir = "../../devel/".. release_name .."/lib"
	package.config[release_name].bindir = "../../devel/".. release_name .."/bin"
	package.config[debug_name].libdir = "../../devel/".. debug_name .."/lib"
	package.config[debug_name].bindir = "../../devel/".. debug_name .."/bin"
	else
	package.config[release_name].targetprefix =  "lib" 
	package.config[debug_name].targetprefix =  "lib"

	if( not macosx ) then
	package.config[release_name].targetextension =  "so" 
	package.config[debug_name].targetextension =  "so"
	else
	package.config[release_name].targetextension =  "dylib" 
	package.config[debug_name].targetextension =  "dylib"
	end

	package.config[release_name].libdir = "../../devel/".. release_name .."/lib"
	package.config[release_name].bindir = "../../devel/".. release_name .."/lib"
	package.config[debug_name].libdir = "../../devel/".. debug_name .."/lib"
	package.config[debug_name].bindir = "../../devel/".. debug_name .."/lib"
	end
	end

function CreateTargetDefinitionsExe( pakage , targetname , release_name , debug_name )

	package.config[release_name].target = targetname
	package.config[debug_name].target = targetname

	if( windows ) then
	package.config[release_name].libdir = "../../devel/".. release_name .."/lib"
	package.config[release_name].bindir = "../../devel/".. release_name .."/bin"
	package.config[debug_name].libdir = "../../devel/".. debug_name .."/lib"
	package.config[debug_name].bindir = "../../devel/".. debug_name .."/bin"
	else
	package.config[release_name].libdir = "../../devel/".. release_name .."/lib"
	package.config[release_name].bindir = "../../devel/".. release_name .."/bin"
	package.config[debug_name].libdir = "../../devel/".. debug_name .."/lib"
	package.config[debug_name].bindir = "../../devel/".. debug_name .."/bin"
	end
	end


	--*****************************************************************************
	-- This function will happend the default path common to all project for post
	-- build steps generation
	--*****************************************************************************
function SetUpDefaultPaths( pakage , release_name , debug_name )
	package.config[release_name].libpaths = { "../../devel/".. release_name .."/lib" }
	package.config[debug_name].libpaths = { "../../devel/".. debug_name .."/lib" }
	end

	--*****************************************************************************
	-- This function will create all postbuild steps for your package if required
	-- this is very usefull
	--*****************************************************************************
function CreateLibraryPostBuildStepFor( package , config_name )
	local CP = "";
	if( macosx ) then
	CP="cp -r "
	else
	CP="cp -ru "
	end
	local MKDIR=""

	if( windows ) then
	MKDIR = "gmkdir -p "
	else
	MKDIR = "mkdir -p "
	end

	table.insert( package.config[config_name] , MKDIR .. "../../devel/".. config_name .."/include/" .. package.config[config_name].target )
	table.insert( package.config[config_name] , CP .. "../../include/" .. package.name.. " ../../devel/".. config_name .."/include" )
	end

	-- Get the package share dir
function GetShareDir( config_name )
	local shareDir=""
	if( not windows ) then
	shareDir  = "../../devel/"..config_name.."/share"
	else
	shareDir  = "../../devel/"..config_name.."/bin/share"
	end
	return shareDir
	end

	-- Get the library name 
function GetLibraryName( generic_name )
	local library_name = ""

	if( not windows ) then
	library_name = "lib" .. generic_name .. ".so"
	else
	library_name = generic_name .. ".dll"
	end
	return library_name
	end

	--*****************************************************************************
	-- This function creates the object directory
	--*****************************************************************************
function SetUpObjectDirectories( pakage , options , release_name , debug_name )

	-- Set object output directory.
	if ( options["unicode"] ) then
	package.config[debug_name].objdir = ".objsud"
	package.config[release_name].objdir = ".objsu"
	else
	package.config[debug_name].objdir = ".objsd"
	package.config[release_name].objdir = ".objs"
	end

	end
