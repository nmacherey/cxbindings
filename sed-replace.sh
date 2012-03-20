#!bin/bash

DIRECTORIES='cxbindings'
INCLUDES='include include/handlers include/generators src src/handlers src/generators'
WILDCARDS='*.h *.hh *.cpp *.c *.cxx *.cpp'
ADDITIONALFILES='defs.h Singleton.h'

# first copy all files from wxgrammar to cxbindings
rm -rf cxbindings/include 
rm -rf cxbindings/src

PWD=`pwd`

for DIR in $DIRECTORIES; do
	for INC in $INCLUDES; do
		wxpath="../wxgrammar/$INC"
		path="$DIR/$INC"
		
		mkdir -p $path
		cp -r $wxpath/wxGrammar* $path 2> /dev/null 
		cp -r $wxpath/std* $path 2> /dev/null

		# rename files
		rename 's/wxGrammar(.*)/CXBindings$1/' $path/* 2> /dev/null
		rename 's/stdObject(.*)/CXBindingsObjectBase$1/' $path/* 2> /dev/null
		rename 's/std(.*)/CXBindings$1/' $path/* 2> /dev/null
	done
done

for file in $ADDITIONALFILES; do
	cp ../wxgrammar/include/$file cxbindings/include/ 2> /dev/null
done

#Rename an additional file
mv cxbindings/include/defs.h cxbindings/include/CXBindingsDefinitions.h 2> /dev/null

#Move old generators to generators dir
cp cxbindings/include/*Generator.h cxbindings/include/generators/ 2> /dev/null
cp cxbindings/src/*Generator.cpp cxbindings/src/generators/ 2> /dev/null

rm cxbindings/include/*Generator.h 2> /dev/null
rm cxbindings/src/*Generator.cpp 2> /dev/null

# for each directories replace
# the stuff here is to replace all wx to std::string stuff with the proper expression
# same thing is achieve for wxGrammar stuff
for DIR in $DIRECTORIES; do
	for INC in $INCLUDES; do
		for CARD in $WILDCARDS; do
			path="$DIR/$INC/$CARD"
			sed -i 's/wxGrammarObject/CXBindingsObject/g' $path 2> /dev/null
			sed -i 's/wxGrammar/CXBindings/g' $path 2> /dev/null
			sed -i 's/WXGRAMMAR/CXBINDINGS/g' $path 2> /dev/null
			sed -i 's/nm@graymat.fr/nicolas.macherey@gmail.com/g' $path 2> /dev/null
			sed -i 's/wxString/std::string/g' $path 2> /dev/null
			sed -i 's/wxEmptyString/stdEmptyString/g' $path 2> /dev/null
			sed -i 's/wxArray/CXBindingsArray/g' $path 2> /dev/null
			sed -i 's/wxgStringStringMap/CXBindingsStringStringMap/g' $path 2> /dev/null
			sed -i 's/typedef\ wxObject\ CXBindingsBaseObject/#include <boost\/any.hpp>\n#include <vector>\n#include <string>\n#include "CXBindingsObjectBase.h"\n\nconst std::string stdEmptyString="";\ntypedef std::vector<std::string> CXBindingsArrayString/g' $path 2> /dev/null
			sed -i 's/wxObject/CXBindingsObjectBase/g' $path 2> /dev/null
			sed -i 's/wxGrammarBaseObject/CXBindingsObjectBase/g' $path 2> /dev/null
			sed -i 's/WXG/CXB/g' $path 2> /dev/null
			sed -i 's/STD/CXBINDINGS/g' $path 2> /dev/null
			sed -i 's/DECLARE_ABSTRACT_CLASS(/DECLARE_ABSTRACT_CLASS_CXBINDINGS(/g' $path 2> /dev/null
			sed -i 's/DECLARE_DYNAMIC_CLASS(/DECLARE_DYNAMIC_CLASS_CXBINDINGS(/g' $path 2> /dev/null
			sed -i 's/IMPLEMENT_ABSTRACT_CLASS(/IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS(/g' $path 2> /dev/null
			sed -i 's/IMPLEMENT_DYNAMIC_CLASS(/IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(/g' $path 2> /dev/null
			sed -i 's/STD/CXBINDINGS/g' $path 2> /dev/null
			sed -i 's/stdObject/CXBindingsObjectBase/g' $path 2> /dev/null
			sed -i 's/CXBindingsBaseObject/CXBindingsObjectBase/g' $path 2> /dev/null
			sed -i 's/stdClassInfo/CXBindingsClassInfo/g' $path 2> /dev/null
			sed -i 's/stdHashTable/CXBindingsHashTable/g' $path 2> /dev/null
			sed -i 's/defs\.h/CXBindingsDefinitions\.h/g' $path 2> /dev/null
			sed -i 's/wxT(\(.*\))/\1\ /g' $path 2> /dev/null
			sed -i 's/\.IsSameAs(\(.*\))/\ ==\1/g' $path 2> /dev/null
			sed -i 's/\.IsEmpty()/\.empty()/g' $path 2> /dev/null
			sed -i 's/\.GetCount()/\.size()/g' $path 2> /dev/null
			sed -i 's/wxC2U/cxC2U/g' $path 2> /dev/null
			sed -i 's/wxU2C/cxU2C/g' $path 2> /dev/null
			sed -i 's/wxxU2C/cxxU2C/g' $path 2> /dev/null
			sed -i 's/wxcU2C/cxcU2C/g' $path 2> /dev/null
			sed -i 's/wxUSE_UNICODE/USE_UNICODE/g' $path 2> /dev/null
			sed -i 's/WXDEBUG/DEBUG/g' $path 2> /dev/null
			sed -i 's/WXMSW/WINDOWS/g' $path 2> /dev/null
			sed -i 's/#include\ <wx\(.*\)>//g' $path 2> /dev/null
			sed -i 's/wxVariant/boost::any/g' $path 2> /dev/null
			sed -i 's/wxStaticCast(m_instance,classname)/static_cast<classname*>(m_instance)/g' $path 2> /dev/null
			sed -i 's/typedef std::vector<CXBindingsSet> CXBindingsArrayGrammarSet;/typedef std::vector<CXBindingsSet> CXBindingsArrayGrammarSet;\n\ntypedef std::vector<int> CXBindingsArrayInt;\ntypedef std::vector<std::string> CXBindingsArrayString;/g' $path 2> /dev/null
		done
	done
done
