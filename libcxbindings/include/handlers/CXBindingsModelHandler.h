/**
 * @file CXBindingsModelHandler.h
 * @brief main file for defining grammar model handler for a node
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSMODELHANDLER_H
#define CXBINDINGSMODELHANDLER_H

/**
  * @class CXBindingsModelHandler
  * @brief Main class for building a model from an xml node
  * 
  * @author Nicolas Macherey (nicolas.macherey@gmail.com)
  * @date 8-May-2010
  */
class CXBindingsModelHandler : public CXBindingsXmlHandler
{
	DECLARE_ABSTRACT_CLASS_CXBINDINGS(CXBindingsModelHandler)
	
public :
	/** Default constructor */
	CXBindingsModelHandler();
	
    	/** This one is called from CreateResource after variables
    	  * were filled.
	  * Any handlers shall overload this metohd in order to complete the 
	  * model stuff.
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


