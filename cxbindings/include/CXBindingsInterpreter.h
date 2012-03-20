/**
 * @file CXBindingsInterpreter.h
 * @brief main file for generating c++ objects from a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#ifndef CXBINDINGSINTERPRETER_H
#define CXBINDINGSINTERPRETER_H

/** previous defines */
class CXBindingsCategoryInfo;
class CXBindingsChildContainerInfo;

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsCategoryInfo > CXBindingsArrayGrammarCategoryInfo;

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsChildContainerInfo > CXBindingsArrayGrammarChildContainerInfo;

/** Define a structure for handling CXBindingsName Informations extracted from nodes
 * Each CXBindingsName is different.
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsNameInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsNameInfo(){};
	
	/** copy constructor */
	CXBindingsNameInfo( const CXBindingsNameInfo& rhs ):
		name(rhs.name),
		stemplate(rhs.stemplate),
		content(rhs.content)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsNameInfo& operator=(const CXBindingsNameInfo& rhs)
	{
		name = rhs.name;
		stemplate = rhs.stemplate;
		content = rhs.content;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsNameInfo& rhs ) const 
	{
		return ( name == rhs.name &&
			 stemplate == rhs.stemplate &&
		         content == rhs.content	
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
	
	CXBindingsName name;			/*!< save the related CXBindingsName */
	std::string stemplate;			/*!< extracted template value for that rule name */
	std::string content;			/*! extracted content value for that rule name */
};

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsNameInfo > CXBindingsArrayGrammarNameInfo;

/** Define a structure for handling CXBindingsType Informations extracted from nodes
 * Each CXBindingsType is different.
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsTypeInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsTypeInfo(){};
	
	/** copy constructor */
	CXBindingsTypeInfo( const CXBindingsTypeInfo& rhs ):
		type(rhs.type),
		stemplate(rhs.stemplate),
		content(rhs.content)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsTypeInfo& operator=(const CXBindingsTypeInfo& rhs)
	{
		type = rhs.type;
		stemplate = rhs.stemplate;
		content = rhs.content;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsTypeInfo& rhs ) const 
	{
		return ( type == rhs.type &&
			 stemplate == rhs.stemplate &&
		         content == rhs.content	
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
	
	CXBindingsType type;			/*!< save the related CXBindingsType */
	std::string stemplate;			/*!< extracted template value for that rule type */
	std::string content;			/*! extracted content value for that rule type */
};

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsTypeInfo > CXBindingsArrayGrammarTypeInfo;

/** Define a structure for handling CXBindingsVariable Informations extracted from nodes
 * Each CXBindingsVariable is different.
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsVariableInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsVariableInfo(){};
	
	/** copy constructor */
	CXBindingsVariableInfo( const CXBindingsVariableInfo& rhs ):
		variable(rhs.variable),
		stemplate(rhs.stemplate),
		content(rhs.content)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsVariableInfo& operator=(const CXBindingsVariableInfo& rhs)
	{
		variable = rhs.variable;
		stemplate = rhs.stemplate;
		content = rhs.content;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsVariableInfo& rhs ) const 
	{
		return ( variable == rhs.variable &&
			 stemplate == rhs.stemplate &&
		         content == rhs.content	
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
	
	CXBindingsVariable variable;			/*!< save the related CXBindingsVariable */
	std::string stemplate;			/*!< extracted template value for that rule variable */
	std::string content;			/*! extracted content value for that rule variable */
};

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsVariableInfo > CXBindingsArrayGrammarVariableInfo;

/** Define a structure for handling CXBindingsRule Informations extracted from nodes
 * Each CXBindingsRule is different.
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsRuleInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsRuleInfo(){};
	
	/** copy constructor */
	CXBindingsRuleInfo( const CXBindingsRuleInfo& rhs ):
		rule(rhs.rule),
		name(rhs.name),
		type(rhs.type),
		variable(rhs.variable),
		make(rhs.make)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsRuleInfo& operator=(const CXBindingsRuleInfo& rhs)
	{
		rule = rhs.rule;
		name = rhs.name;
		type = rhs.type;
		make = rhs.make;
		variable = rhs.variable;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsRuleInfo& rhs ) const 
	{
		return ( rule == rhs.rule &&
			 name == rhs.name &&
		         type == rhs.type &&
			 variable == rhs.variable &&
			 make == rhs.make
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
	
	CXBindingsRule rule;			/*!< save the related CXBindingsRule */
	CXBindingsNameInfo name;			/*!< save the related rule name inforamtion */
	CXBindingsTypeInfo type;			/*!< save the related rule type information */
	CXBindingsVariableInfo variable;		/*!< save the related rule type information */
	std::string make;
};

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsRuleInfo > CXBindingsArrayGrammarRuleInfo;

/** define a map of rules */
typedef std::map< std::string , CXBindingsArrayGrammarRuleInfo > CXBindingsRuleInfoMap;

/** Define a structure for handling CXBindingsChild Informations extracted from nodes
 * Each CXBindingsChild is different and accordingly to the rule defined in the grammar,
 * we will have to store its extracted information somewhere. Here is the place to do
 * it !
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsChildInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsChildInfo(){};
	
	/** copy constructor */
	CXBindingsChildInfo( const CXBindingsChildInfo& rhs ):
		child(rhs.child),
		properties(rhs.properties),
		rules(rhs.rules)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsChildInfo& operator=(const CXBindingsChildInfo& rhs)
	{
		child = rhs.child;
		properties = rhs.properties;
		rules = rhs.rules;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsChildInfo& rhs ) const 
	{
		return ( child == rhs.child &&
			properties == rhs.properties &&
			rules == rhs.rules
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
	
	CXBindingsChild child;			/*!< save the related CXBindingsChild */
	CXBindingsStringStringMap properties;		/*!< extracted properties from the node */
	CXBindingsArrayGrammarRuleInfo rules;		/*!< extracted rules information */
};

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsChildInfo > CXBindingsArrayGrammarChildInfo;

/** Define a structure for handling information retained for any foreach
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsForEachChildInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsForEachChildInfo(){};
	
	/** copy constructor */
	CXBindingsForEachChildInfo( const CXBindingsForEachChildInfo& rhs ):
		foreach(rhs.foreach),
		rules(rhs.rules)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsForEachChildInfo& operator=(const CXBindingsForEachChildInfo& rhs)
	{
		foreach = rhs.foreach;
		rules = rhs.rules;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsForEachChildInfo& rhs ) const 
	{
		return ( foreach == rhs.foreach &&
			rules == rhs.rules
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
  	CXBindingsForEachChild foreach; 			/*!< save the foreach related to this information */	
	CXBindingsRuleInfoMap rules;
};

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsForEachChildInfo > CXBindingsArrayGrammarForEachChildInfo;

/** Define a structure for handling information retained for any child container  
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsChildContainerInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsChildContainerInfo(){};
	
	/** copy constructor */
	CXBindingsChildContainerInfo( const CXBindingsChildContainerInfo& rhs ):
		container(rhs.container),
		childs(rhs.childs),
		properties(rhs.properties),
		rules(rhs.rules),
		childRules( rhs.childRules ),
		categories( rhs.categories )
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsChildContainerInfo& operator=(const CXBindingsChildContainerInfo& rhs)
	{
		container = rhs.container;
		childs = rhs.childs;
		properties = rhs.properties;
		rules = rhs.rules;
		childRules = rhs.childRules;
		categories = rhs.categories;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsChildContainerInfo& rhs ) const 
	{
		return ( container == rhs.container &&
			childs == rhs.childs &&
			properties == rhs.properties &&
		        rules == rhs.rules && 
			childRules == rhs.childRules &&
			categories == rhs.categories	
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
  	CXBindingsChildContainer container; 		/*!< save the container related to this information */	
	CXBindingsArrayGrammarChildInfo childs;			/*!< save the related CXBindingsArrayGrammarChildInfo */
	CXBindingsStringStringMap properties;			/*!< extracted properties from the node */
	CXBindingsArrayGrammarRuleInfo rules;		/*!< extracted rules information */
	CXBindingsArrayGrammarForEachChildInfo childRules;
	CXBindingsArrayGrammarCategoryInfo categories;
};

/** Define a structure for handling information retained for any category
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsCategoryInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsCategoryInfo(){};
	
	/** copy constructor */
	CXBindingsCategoryInfo( const CXBindingsCategoryInfo& rhs ):
		category(rhs.category),
		childs(rhs.childs),
		containers(rhs.containers)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsCategoryInfo& operator=(const CXBindingsCategoryInfo& rhs)
	{
		category = rhs.category;
		childs = rhs.childs;
		containers = rhs.containers;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsCategoryInfo& rhs ) const 
	{
		return ( category == rhs.category &&
			childs == rhs.childs &&
			containers == rhs.containers
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
  	CXBindingsCategory category; 		/*!< save the category related to this information */	
	CXBindingsArrayGrammarChildInfo childs;			/*!< save the related CXBindingsArrayGrammarChildInfo */
	CXBindingsArrayGrammarChildContainerInfo containers;
};



/** Define a structure for handling information retained for any object
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsObjectInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsObjectInfo(){};
	
	/** copy constructor */
	CXBindingsObjectInfo( const CXBindingsObjectInfo& rhs ):
		object(rhs.object),
		categories(rhs.categories),
		childs(rhs.childs),
		properties(rhs.properties),
		rules(rhs.rules),
		childInfos(rhs.childInfos)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsObjectInfo& operator=(const CXBindingsObjectInfo& rhs)
	{
		object = rhs.object;
		categories = rhs.categories;
		childs = rhs.childs;
		properties = rhs.properties;
		rules = rhs.rules;
		childInfos = rhs.childInfos;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsObjectInfo& rhs ) const 
	{
		return ( object == rhs.object &&
			categories == rhs.categories &&
			childs == rhs.childs &&
			properties == rhs.properties &&
			rules == rhs.rules &&
		        childInfos == rhs.childInfos	
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
  	CXBindingsObject object; 			/*!< save the object related to this information */	
	CXBindingsArrayGrammarCategoryInfo categories;		/*!< save the related CXBindingsArrayGrammarCategoryInfo */
	CXBindingsArrayGrammarChildContainerInfo childs;	/*!< save the related CXBindingsArrayGrammarChildInfo */
	CXBindingsStringStringMap properties;			/*!< extracted properties from the node */
	CXBindingsArrayGrammarRuleInfo rules;		/*!< extracted rules information */
	CXBindingsArrayGrammarChildInfo childInfos;			/*!< save the related CXBindingsArrayGrammarChildInfo */
};

/** define an array of CXBindingsChildInfo */
typedef std::vector< CXBindingsObjectInfo > CXBindingsArrayGrammarObjectInfo;

/** define a map of CXBindingsArrayGrammarObjectInfo in order to associated the
  generated information to the proper object name in the grammar */
typedef std::map< std::string , CXBindingsArrayGrammarObjectInfo > CXBindingsArrayGrammarObjectInfoMap;

/** Define a structure for handling information retained for any grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @date 19-April-2010
 */
struct CXBindingsInterpreterInfo
{
	/*********************************************************************************
	* CONTRUCTORS
	*********************************************************************************/

	/** default constructor */
	CXBindingsInterpreterInfo(){};
	
	/** copy constructor */
	CXBindingsInterpreterInfo( const CXBindingsInterpreterInfo& rhs ):
		grammar(rhs.grammar),
		objects(rhs.objects)
	{};

	/*********************************************************************************
	* OVERLOADS
	*********************************************************************************/
	
	/** operator= overload */
	CXBindingsInterpreterInfo& operator=(const CXBindingsInterpreterInfo& rhs)
	{
		grammar = rhs.grammar;
		objects = rhs.objects;

		return (*this);
	}

	/** operator== overload */
	bool operator==( const CXBindingsInterpreterInfo& rhs ) const 
	{
		return ( grammar == rhs.grammar &&
			objects == rhs.objects
			);
	}
	
	/*********************************************************************************
	* PROPERTIES
	*********************************************************************************/
  	CXBindings grammar; 				/*!< save the grammar related to this information */	
	CXBindingsArrayGrammarObjectInfoMap objects;		/*!< save the related CXBindingsArrayGrammarChildInfo */
};


/**
 * @class CXBindingsInterpreter
 * @brief main class for generating cpp code from the grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsInterpreter : public CXBindingsObjectBase
{
public :

	/** default ctor */
	CXBindingsInterpreter(){};
	
	/** Generate the code associated to the grammar */
	CXBindingsInterpreterInfo DoExtractObjectsInformation( const std::string& file , CXBindings& grammar , const CXBindingsGeneratorOptions& options );

private :

	/** Generate the object code of the given node associated to the given grammar object */
	CXBindingsObjectInfo DoExtractNodeInformation( CXBindings& grammar , CXBindingsObject& object , xmlNode* node , const CXBindingsGeneratorOptions& options );
       
        /** Extract properties from a given node and check if all are matching or not
	 * This method is very important in the sense that "property" nodes can be used 
	 * in many different grammar elements.
	 * @param node parent node from which properties shall be extracted
 	 * @param properties CXBindingsArrayGrammarProperties that shall be extracted from the node
	 * @param grammar CXBindings object the properties belongs to
 	 * @return a map of extracted properties
	 * @note that this method will generate an exception if something is wrong with
	 * properties extraction 
         */
	CXBindingsStringStringMap DoExtractPropertiesFrom( xmlNode* node , CXBindingsArrayGrammarProperty& properties , CXBindings& grammar );

	/** Extract and generate child container informations for the given node
	 * This method is required for each object or model which accepts child containers
	 * it is very important for it's generation :D
	 * The difference between a category and a child container is that a category cannot contain
	 * properties and generation rules, they are just a group of information used in your file.
	 * @param node node to extract child containers from
	 * @param containers all containers which can be applied in the given node
	 * @param grammar general grammar container object
	 */
	CXBindingsArrayGrammarChildContainerInfo DoExtractChildContainersFrom( xmlNode* node , CXBindingsArrayGrammarChildContainer& containers , CXBindings& grammar );

	/** Extract and generate category informations for the given node
	 * This method is required for each object or model which accepts category nodes (i.e : not generated containers with no rules)
	 * it is very important for it's generation :D
	 * The difference between a category and a child container is that a category cannot contain
	 * properties and generation rules, they are just a group of information used in your file.
	 * @param node node to extract child containers from
	 * @param categories all categories which can be applied in the given node
	 * @param grammar general grammar container object
	 */
	CXBindingsArrayGrammarCategoryInfo DoExtractCategoriesFrom( xmlNode* node , CXBindingsArrayGrammarCategory& categories , CXBindings& grammar );

	/** Extract all childrens informations from the given node accordingly to the 
	 * given childrens.
   	 * This method is very important for all nodes which can handles childrens, a child node 
	 * can be used in various wxgrammar nodes.
	 * @param node parent node from which children informations shall be extracted
	 * @param childs CXBindingsArrayGrammarChild used to extract the information
	 * @param grammar general grammar container object
  	 * @return a CXBindingsArrayGrammarChildInfo containing the extracted information for each child
	 * @note this method may generate some exception if some errors happens.
	 */
	CXBindingsArrayGrammarChildInfo DoExtractChildrenInformationFrom( xmlNode* node , CXBindingsArrayGrammarChild& childs , CXBindings& grammar );

	/** Extract all rule information for a given node accordingly to the given rules
	  * This method is very important for all nodes subject to generation rules...
	  * Moreover it can be used in most of the CXBindings nodes !
	  * @param node parent node from which rule information shall be extracted
	  * @param rules CXBindingsArrayGrammarRule used to extract the information
	  * @param grammar general grammar container object
	  * @return a CXBindingsArrayGrammarRuleInfo containing extracted information for each rule
	  * @note that this method may generate some exception if something is wrong with the rule extraction
	  */
	CXBindingsArrayGrammarRuleInfo DoExtractRuleInformationFrom( xmlNode* node , CXBindingsArrayGrammarRule& rules , CXBindings& grammar );
	
	/** Extract all child rules information for a given node accordingly to the given child rules
	  * This method is very important for all nodes subject to generation child rules...
	  * Moreover it can be used in most of the CXBindings nodes !
	  * @param node parent node from which rule information shall be extracted
	  * @param rules CXBindingsArrayGrammarForEachChild used to extract the information
	  * @param grammar general grammar container object
	  * @return a CXBindingsArrayGrammarForEachChildInfo containing extracted information for each rule
	  * @note that this method may generate some exception if something is wrong with the rule extraction
	  */
	CXBindingsArrayGrammarForEachChildInfo DoExtractChildRuleInformationFrom( xmlNode* node , CXBindingsArrayGrammarForEachChild& rules , CXBindings& grammar );

	/** Append model properties to object 
	 * Any object node in wxgramma can hinerit from a model node, which defines
	 * common properties for this kind of objects.
	 * This method will simply apend all models to a given object before
	 * any generation stuff in order to have the entire information contained
	 * in the object.
	 * @param grammar general grammar container object
	 * @param object object to apend models in
	 * @param modelname name of the model to apend in the object.
	 * @note that this method may generate a CXBindingsException if something is
	 * wrong with the model or with the object.
	 */
	void AppendModelToObject( CXBindings& grammar , CXBindingsObject& object , const std::string& modelname );
	
	xmlDoc* m_doc;
	xmlNode* m_root;

};

#endif
