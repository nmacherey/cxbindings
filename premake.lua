project.name = "CXBindings"

-- Configureations. All I am doing is reordering them so that
-- it defaults to a Release build.
project.configs = { "DLLRelease", "DLLDebug" }

-- Package options
addoption( "unicode", "Use the Unicode character set" )

if ( not windows ) then
	addoption("rpath", "Specify the rpath for the compiled binary")
end

--dopackage( "scripts/wxgrammarsqlite3" )
dopackage( "scripts/cxbindings" )
