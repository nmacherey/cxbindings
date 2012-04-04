/**
 * @file CXBindingsGeneratorFileAddinHandler.h
 * @brief main file for defining grammar generatorfilefile addin handler for a node
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSGENERATORFILEADDINHANDLER_H
#define CXBINDINGSGENERATORFILEADDINHANDLER_H

/**
  * @class CXBindingsGeneratorFileAddinHandler
  * @brief Main class for building an addin from an xml node
  * 
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsGeneratorFileAddinHandler : public CXBindingsXmlHandler
{
	DECLARE_ABSTRACT_CLASS_CXBINDINGS(CXBindingsGeneratorFileAddinHandler)
	
public :
	/** Default constructor */
	CXBindingsGeneratorFileAddinHandler();
	
    	/** This one is called from CreateResource after fileiables
    	  * were filled.
	  * Any handlers shall overload this metohd in order to complete the 
	  * generatorfilefile stuff.
	  */
    	virtual CXBindingsObjectBase *DoCreateResource();

	/** This is the method you have to overload to write a resource
	  * to anode simply create a new node and write info in
	  */
	virtual xmlNode* DoWriteResource();	

    	/** @return Returns true if it understands this node and can create
    	  * a resource from it, false otherwise.
	  */
    	virtual bool CanHandle(xmlNode *node);

	/** @return Returns true if it understands the given resource and
	  * can write it into an xmlNode 
	  */
	virtual bool CanHandle( CXBindingsObjectBase* resource );
};


#endif






