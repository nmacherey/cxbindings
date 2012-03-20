/**
 * @file CXBindingsGeneratorFileObjectPartHandler.h
 * @brief main file for defining grammar generatorfileobjectpart handler for a node
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSGENERATORFILEOBJECTPARTHANDLER_H
#define CXBINDINGSGENERATORFILEOBJECTPARTHANDLER_H

/**
  * @class CXBindingsGeneratorFileObjectPartHandler
  * @brief Main class for building a generatorfileobjectpart from an xml node
  * 
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsGeneratorFileObjectPartHandler : public CXBindingsXmlHandler
{
	DECLARE_ABSTRACT_CLASS_CXBINDINGS(CXBindingsGeneratorFileObjectPartHandler)
	
public :
	/** Default constructor */
	CXBindingsGeneratorFileObjectPartHandler();
	
    	/** This one is called from CreateResource after objectpartiables
    	  * were filled.
	  * Any handlers shall overload this metohd in order to complete the 
	  * generatorfileobjectpart stuff.
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




