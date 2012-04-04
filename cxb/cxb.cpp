/**
 * @file cxb.cpp
 * @brief Main file for running cxbindings from the command line
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 12.04
 */
#include <iostream>
#include <string>
#include <vector>
#include <stack>

#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>

#include "CXBindingsGlobals.h"
#include "CXBindingsException.h"
#include "CXBindingsExpressionParser.h"
#include "CXBindings.h"
#include "CXBindingsGeneratorFile.h"
#include "CXBindingsInterpreter.h"
#include "CXBindingsXmlReader.h"
#include "CXBindingsGeneratorFactory.h"

namespace po = boost::program_options;


int main(int argc, char **argv, char **ppenv) {
    /** Read program options from command line */
    po::options_description desc("CXBindigns program options");
    desc.add_options()
        ("help,h",po::value<std::string>(), "display this message")
        ("file,f",po::value<std::string>(), "File to generate C++ from")
        ("interpreter,i",po::value<std::string>(), "Interpreter/Grammar XML file to use")
        ("namespace,n",po::value<std::string>(), "Namespace prefix to your objects to use")
        ("export-macro",po::value<std::string>(), "Export macro to use for your objects (default: DLLEXPORT)")
        ("generator,g",po::value<std::string>(), "Generator Plugin to use (CPPObjects, CPPHandlers)")
        ("generator-templates,t",po::value<std::string>(), "Generator Plugin Templates file")
        ("output,o",po::value<std::string>(), "Output Directory")
    ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    std::string interpreter;
    std::string ns;
    std::string file;
    std::string exportMacro;
    std::string generator;
    std::string templates;
    std::string output;

    if (vm.count("help") || argc == 1) {
            std::cout << desc << "\n";
            return 1;
    }

    if( !vm.count("interpreter") ) {
        std::cout << "Error missing interpreter file..." << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    
    if( !vm.count("file") ) {
        std::cout << "Error missing file to generate objects from..." << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    
    if( !vm.count("namespace") ) {
        std::cout << "Error missing namespace..." << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }

    if( !vm.count("export-macro") ) {
        std::cout << "Error missing export macro using DLLEXPORT as default..." << std::endl;
        exportMacro = "DLLEXPORT";
    } else {
        exportMacro = vm["export-macro"].as<std::string>();
    }
    
    if( !vm.count("generator") ) {
        std::cout << "Error missing generator..." << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    
    if( !vm.count("generator-templates") ) {
        std::cout << "Error missing generator templates file..." << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    
    if( !vm.count("output") ) {
        std::cout << "Error missing output directory..." << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    
    interpreter = vm["interpreter"].as<std::string>();
    file = vm["file"].as<std::string>();
    ns = vm["namespace"].as<std::string>();
    generator = vm["generator"].as<std::string>();
    templates = vm["generator-templates"].as<std::string>();
    output = vm["output"].as<std::string>();

    CXBindingsGeneratorOptions gen_options;     
    gen_options.genDir = output;
    gen_options.exportMacro = exportMacro;
    gen_options.genName = ns;
    gen_options.ns = ns;
    gen_options.baseFile = file;
    gen_options.grammarFile = interpreter;
    gen_options.interpreterFile = templates;
    

    CXBindingsGenerator* cxb = CXBindingsGeneratorFactory::Get()->CreateGenerator( generator );
    
    if( !cxb ) {
        std::cout << "Error cannot find your generator plugin... " << generator << std::endl;
        std::cout << "The list of available generators is :" << std::endl;
        CXBindingsGeneratorFactory* factory = CXBindingsGeneratorFactory::Get();
        CXBindingsArrayString list = factory->GetList();
        for( unsigned int i = 0; i < list.size() ; ++i )
            std::cout << list[i] << std::endl;
    }
    
    try{
        cxb->GenerateCode(gen_options);
        if( cxb != NULL )
            delete cxb;
    } catch( CXBindingsException& e ) {
        std::cerr << e.GetErrorMessage() << std::endl;
        return 1;
    }

    return 0;
}

