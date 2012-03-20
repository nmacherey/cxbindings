/**
 * @file CXBindings.h
 * @brief main file for defining grammar components
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
 
#ifndef CXBINDINGS_H
#define CXBINDINGS_H
	
/** predefines */
class CXBindingsCondition;
class CXBindingsProperty;
class CXBindingsType;
class CXBindingsName;
class CXBindingsRule;
class CXBindingsForEachChild;
class CXBindingsChild;
class CXBindingsCategory;
class CXBindingsChildContainer;
class CXBindingsModel;
class CXBindingsInherits;
class CXBindingsObject;
class CXBindingsInfo;
class CXBindingsGeneration;
class CXBindings;
class CXBindingsSet;
class CXBindingsBaseCondition;

/** CXBindingsArrays declarations */
typedef std::vector<CXBindingsCondition> CXBindingsArrayGrammarCondition;
typedef std::vector<CXBindingsProperty> CXBindingsArrayGrammarProperty;
typedef std::vector<CXBindingsAny> CXBindingsArrayVariant;
typedef std::vector<CXBindingsRule> CXBindingsArrayGrammarRule;
typedef std::vector<CXBindingsModel> CXBindingsArrayGrammarModel;
typedef std::vector<CXBindingsChild> CXBindingsArrayGrammarChild;
typedef std::vector<CXBindingsForEachChild> CXBindingsArrayGrammarForEachChild;
typedef std::vector<CXBindingsCategory> CXBindingsArrayGrammarCategory;
typedef std::vector<CXBindingsChildContainer> CXBindingsArrayGrammarChildContainer;
typedef std::vector<CXBindingsModel> CXBindingsArrayGrammarModel;
typedef std::vector<CXBindingsObject> CXBindingsArrayGrammarObject;
typedef std::vector<CXBindingsInherits> CXBindingsArrayGrammarInherits;
typedef std::vector<CXBindingsSet> CXBindingsArrayGrammarSet;

typedef std::vector<int> CXBindingsArrayInt;
typedef std::vector<std::string> CXBindingsArrayString;


/************************************************************************************
 * ARRAYS DEFINITIONS
 ***********************************************************************************/

/**
 * @class CXBindingsCondition
 * @brief class for handling if nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsCondition : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsCondition)
public :

	/** Default ctor */
	CXBindingsCondition():
		CXBindingsObjectBase(),
		m_compiled(false)
		{};
		
	/** Copy ctor */
	CXBindingsCondition(const CXBindingsCondition& rhs):
		CXBindingsObjectBase(),
		m_condition(rhs.m_condition),
		m_content(rhs.m_content),
		m_compiled(rhs.m_compiled),
		m_conditionTree(rhs.m_conditionTree),
		m_contentTree(rhs.m_contentTree)
		{};
		
	/** Operator= overload */
	inline CXBindingsCondition& operator=(const CXBindingsCondition& rhs)
	{
		m_condition = rhs.m_condition;
		m_content = rhs.m_content;
		m_compiled = rhs.m_compiled;
		m_conditionTree = rhs.m_conditionTree;
		m_contentTree = rhs.m_contentTree;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsCondition& rhs) const
	{
		return (
				m_condition == rhs.m_condition &&
				m_content == rhs.m_content &&
				m_compiled == rhs.m_compiled &&
				m_conditionTree == rhs.m_conditionTree &&
				m_contentTree == rhs.m_contentTree
				);
	}
	
	/** setter and getter for condition */
	PROPERTY( std::string , Condition , condition )
	
	/** setter and getter for content */
	PROPERTY( std::string , Content , content )
	
	/** setter and getter for compliled */
	PROPERTY( bool , Compiled , compiled )
	
	/** setter and getter for conditionTree */
	PROPERTY( CXBindingsTree , ConditionTree , conditionTree )
	
	/** setter and getter for contentTree */
	PROPERTY( CXBindingsTree , ContentTree , contentTree )
	
	/****************************************************************************
	 * METHODS
	 ***************************************************************************/
	
	/** Compile the condition,
	 * this is mainly for building the condition tree and 
	 * the condition content tree.
	 * The condition parser will act on the condition string and
	 * build all condition's relative information.
	 */
	bool Compile();
	 
	/** Check if the condition Matches or not */
	bool Matches( xmlNode* node , CXBindings& grammar );
	
	/* evaluates the given tree node and give the result as a variant */
	CXBindingsAny EvaluateVar( CXBindingsTreeNode& item , xmlNode* node , CXBindings& grammar  );
	
	/** Get the match */
	std::string GetMatch( xmlNode* node , CXBindings& grammar );
	
private :

	std::string m_condition;
	std::string m_content;
	
	bool m_compiled;
	CXBindingsTree m_conditionTree;
	CXBindingsTree m_contentTree;
};

inline bool operator==( const CXBindingsArrayInt& lhs , const CXBindingsArrayInt& rhs )
{
	if( lhs.size() != rhs.size() )
		return false;

	for( unsigned int i = 0; i < lhs.size() ; ++i )
		if( lhs[i] != rhs[i] )
			return false;

	return true;
}
/**
 * @class CXBindingsGeneration
 * @brief class for handling if generation nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsGeneration : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsGeneration)
	
public :

	/** Default ctor */
	CXBindingsGeneration():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsGeneration(const CXBindingsGeneration& rhs):
		CXBindingsObjectBase(),
		m_objects(rhs.m_objects),
		m_levels(rhs.m_levels),
		m_templates( rhs.m_templates)
		{};
		
	/** Operator= overload */
	inline CXBindingsGeneration& operator=(const CXBindingsGeneration& rhs)
	{
		m_objects = rhs.m_objects;
		m_levels = rhs.m_levels;
		m_templates = rhs.m_templates;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsGeneration& rhs) const
	{
		return (
				m_objects == rhs.m_objects &&
				m_levels == rhs.m_levels &&
				m_templates == rhs.m_templates 
				);
	}
	
	/** setter and getter for objects */
	PROPERTY( CXBindingsArrayString , Objects , objects )
	/** setter and getter for objects */
	PROPERTY( CXBindingsArrayInt , Levels , levels )
	/** setter and getter for templates */
	PROPERTY( CXBindingsArrayString , Templates , templates )
	
private :

	CXBindingsArrayString m_objects;
	CXBindingsArrayInt m_levels;
	CXBindingsArrayString m_templates;
};


/**
 * @class CXBindingsInfo
 * @brief class for handling if info nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsInfo : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsInfo)
	
public :

	/** Default ctor */
	CXBindingsInfo():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsInfo(const CXBindingsInfo& rhs):
		CXBindingsObjectBase(),
		m_generation(rhs.m_generation)
		{};
		
	/** Operator= overload */
	inline CXBindingsInfo& operator=(const CXBindingsInfo& rhs)
	{
		m_generation = rhs.m_generation;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsInfo& rhs) const
	{
		return (
				m_generation == rhs.m_generation
				);
	}
	
	/** setter and getter for generation */
	PROPERTY( CXBindingsGeneration , Generation , generation )
	
private :

	CXBindingsGeneration m_generation;
};

/**
 * @class CXBindingsBaseCondition
 * @brief class for handling nodes containing conditions in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsBaseCondition : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsBaseCondition)
	
public :
	
	/** Default ctor */
	CXBindingsBaseCondition():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsBaseCondition(const CXBindingsBaseCondition& rhs):
		CXBindingsObjectBase(),
		m_conditions(rhs.m_conditions)
		{};
		
	/** Operator= overload */
	inline CXBindingsBaseCondition& operator=(const CXBindingsBaseCondition& rhs)
	{
		m_conditions = rhs.m_conditions;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsBaseCondition& rhs) const
	{
		return (
				m_conditions == rhs.m_conditions
				);
	}
	
	/** setter and getter for conditions */
	PROPERTY( CXBindingsArrayGrammarCondition , Conditions , conditions )
	
protected :
	
	/** property name */
	CXBindingsArrayGrammarCondition 	m_conditions;
};

/**
 * @class CXBindingsSet
 * @brief class for handling set nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsSet : public CXBindingsBaseCondition
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsSet)
	
public :
	
	/** Default ctor */
	CXBindingsSet():
		CXBindingsBaseCondition()
		{};
		
	/** Copy ctor */
	CXBindingsSet(const CXBindingsSet& rhs):
		CXBindingsBaseCondition(rhs),
		m_var(rhs.m_var),
		m_content(rhs.m_content)
		{};
		
	/** Operator= overload */
	inline CXBindingsSet& operator=(const CXBindingsSet& rhs)
	{
		(* (CXBindingsBaseCondition*) this) = CXBindingsBaseCondition::operator=(rhs);
		m_var = rhs.m_var;
		m_content = rhs.m_content;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsSet& rhs) const
	{
		return (
				m_var == rhs.m_var &&
				m_content == rhs.m_content &&
				CXBindingsBaseCondition::operator==(rhs)
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Var , var )
	
	/** setter and getter for content */
	PROPERTY( std::string , Content , content )
	
private :
	
	/** property name */
	std::string 					m_var;
	std::string 					m_content;
};


/**
 * @class CXBindingsProperty
 * @brief class for handling property nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsProperty : public CXBindingsBaseCondition
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsProperty)
	
public :
	
	/** Default ctor */
	CXBindingsProperty():
		CXBindingsBaseCondition(),
		m_required(true)
		{};
		
	/** Copy ctor */
	CXBindingsProperty(const CXBindingsProperty& rhs):
		CXBindingsBaseCondition(rhs),
		m_name(rhs.m_name),
		m_required(rhs.m_required),
		m_defaultValue(rhs.m_defaultValue)
		{};
		
	/** Operator= overload */
	inline CXBindingsProperty& operator=(const CXBindingsProperty& rhs)
	{
		(* (CXBindingsBaseCondition*) this) = CXBindingsBaseCondition::operator=(rhs);
		m_name = rhs.m_name;
		m_required = rhs.m_required;
		m_defaultValue = rhs.m_defaultValue;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsProperty& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_required == rhs.m_required &&
				m_defaultValue == rhs.m_defaultValue &&
				CXBindingsBaseCondition::operator==(rhs)
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for required */
	PROPERTY( bool , Required , required )
	
	/** setter and getter for defaultValue */
	PROPERTY( std::string , DefaultValue , defaultValue )
	
private :
	
	/** property name */
	std::string 					m_name;
	bool 						m_required;
	std::string 					m_defaultValue;
};

/**
 * @class CXBindingsInherits
 * @brief class for handling property nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsInherits : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsInherits)
	
public :
	
	/** Default ctor */
	CXBindingsInherits():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsInherits(const CXBindingsInherits& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name)
		{};
		
	/** Operator= overload */
	inline CXBindingsInherits& operator=(const CXBindingsInherits& rhs)
	{
		m_name = rhs.m_name;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsInherits& rhs) const
	{
		return (
				m_name == rhs.m_name
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
private :
	
	/** property name */
	std::string 					m_name;
};

/**
 * @class CXBindingsType
 * @brief class for handling type nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsType : public CXBindingsBaseCondition
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsType)
	
public :

	/** Default ctor */
	CXBindingsType():
		CXBindingsBaseCondition()
		{};
		
	/** Copy ctor */
	CXBindingsType(const CXBindingsType& rhs):
		CXBindingsBaseCondition(rhs),
		m_stemplate(rhs.m_stemplate),
		m_content(rhs.m_content)
		{};
		
	/** Operator= overload */
	inline CXBindingsType& operator=(const CXBindingsType& rhs)
	{
		(* (CXBindingsBaseCondition*) this) = CXBindingsBaseCondition::operator=(rhs);
		m_stemplate = rhs.m_stemplate;
		m_content = rhs.m_content;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsType& rhs) const
	{
		return (
				m_stemplate == rhs.m_stemplate &&
				m_content == rhs.m_content &&
				CXBindingsBaseCondition::operator==(rhs)
				);
	}
	
	/** setter and getter for condition */
	PROPERTY( std::string , Template , stemplate )
	
	/** setter and getter for content */
	PROPERTY( std::string , Content , content )
	
	/** Get the content match */
	std::string GetContentMatch( xmlNode* node , CXBindings& grammar );

	/** Get the content match */
	std::string GetTemplateMatch( xmlNode* node , CXBindings& grammar );
private :

	std::string m_stemplate;
	std::string m_content;

};

/**
 * @class CXBindingsVariable
 * @brief class for handling type nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsVariable : public CXBindingsBaseCondition
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsVariable)
	
public :

	/** Default ctor */
	CXBindingsVariable():
		CXBindingsBaseCondition()
		{};
		
	/** Copy ctor */
	CXBindingsVariable(const CXBindingsVariable& rhs):
		CXBindingsBaseCondition(rhs),
		m_stemplate(rhs.m_stemplate),
		m_content(rhs.m_content)
		{};
		
	/** Operator= overload */
	inline CXBindingsVariable& operator=(const CXBindingsVariable& rhs)
	{
		(* (CXBindingsBaseCondition*) this) = CXBindingsBaseCondition::operator=(rhs);
		m_stemplate = rhs.m_stemplate;
		m_content = rhs.m_content;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsVariable& rhs) const
	{
		return (
				m_stemplate == rhs.m_stemplate &&
				m_content == rhs.m_content &&
				CXBindingsBaseCondition::operator==(rhs)
				);
	}
	
	/** setter and getter for condition */
	PROPERTY( std::string , Template , stemplate )
	
	/** setter and getter for content */
	PROPERTY( std::string , Content , content )
	
	/** Get the content match */
	std::string GetContentMatch( xmlNode* node , CXBindings& grammar );

	/** Get the content match */
	std::string GetTemplateMatch( xmlNode* node , CXBindings& grammar );
private :

	std::string m_stemplate;
	std::string m_content;

};

/**
 * @class CXBindingsName
 * @brief class for handling name nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsName : public CXBindingsBaseCondition
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsName)
	
public :

	/** Default ctor */
	CXBindingsName():
		CXBindingsBaseCondition()
		{};
		
	/** Copy ctor */
	CXBindingsName(const CXBindingsName& rhs):
		CXBindingsBaseCondition(rhs),
		m_stemplate(rhs.m_stemplate),
		m_content(rhs.m_content)
		{};
		
	/** Operator= overload */
	inline CXBindingsName& operator=(const CXBindingsName& rhs)
	{
		(* (CXBindingsBaseCondition*) this) = CXBindingsBaseCondition::operator=(rhs);
		m_stemplate = rhs.m_stemplate;
		m_content = rhs.m_content;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsName& rhs) const
	{
		return (
				m_stemplate == rhs.m_stemplate &&
				m_content == rhs.m_content &&
				CXBindingsBaseCondition::operator==(rhs)
				);
	}
	
	/** setter and getter for condition */
	PROPERTY( std::string , Template , stemplate )
	
	/** setter and getter for content */
	PROPERTY( std::string , Content , content )
	
	/** Get the content match */
	std::string GetContentMatch( xmlNode* node , CXBindings& grammar );

	/** Get the content match */
	std::string GetTemplateMatch( xmlNode* node , CXBindings& grammar );

private :

	std::string m_stemplate;
	std::string m_content;
};

/**
 * @class CXBindingsRule
 * @brief class for handling rule nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsRule : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsRule)
	
public :
	
	/** Default ctor */
	CXBindingsRule():
		CXBindingsObjectBase(),
		m_accessors(false)
		{};
		
	/** Copy ctor */
	CXBindingsRule(const CXBindingsRule& rhs):
		CXBindingsObjectBase(),
		m_make(rhs.m_make),
		m_accessors(rhs.m_accessors),
		m_contentName(rhs.m_contentName),
		m_contentType(rhs.m_contentType),
		m_contentVariable(rhs.m_contentVariable)
		{};
		
	/** Operator= overload */
	inline CXBindingsRule& operator=(const CXBindingsRule& rhs)
	{
		m_make = rhs.m_make;
		m_accessors = rhs.m_accessors;
		m_contentName = rhs.m_contentName;
		m_contentType = rhs.m_contentType;
		m_contentVariable = rhs.m_contentVariable;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsRule& rhs) const
	{
		return (
				m_make == rhs.m_make &&
				m_accessors == rhs.m_accessors &&
				m_contentName == rhs.m_contentName && 
				m_contentType == rhs.m_contentType &&
				m_contentVariable == rhs.m_contentVariable
				);
	}
	
	/** setter and getter for make */
	PROPERTY( std::string , Make , make )
	
	/** setter and getter for accessors */
	PROPERTY( bool , Accessors , accessors )
	
	/** setter and getter for contentName */
	PROPERTY( CXBindingsName , ContentName , contentName )
	
	/** setter and getter for contentType */
	PROPERTY( CXBindingsType , ContentType , contentType )
	
	/** setter and getter for contentType */
	PROPERTY( CXBindingsVariable , ContentVariable , contentVariable )

	/** get the make match */
	std::string GetMakeMatch( xmlNode* node , CXBindings& grammar );
	
private :
	
	std::string 					m_make;
	bool 						m_accessors;
	CXBindingsName 					m_contentName;
	CXBindingsType 					m_contentType;
	CXBindingsVariable 				m_contentVariable;
};

/**
 * @class CXBindingsForEachChild
 * @brief class for handling foreach_child nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsForEachChild : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsForEachChild)
	
public :

	/** Default ctor */
	CXBindingsForEachChild():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsForEachChild(const CXBindingsForEachChild& rhs):
		CXBindingsObjectBase(),
		m_of(rhs.m_of),
		m_rules(rhs.m_rules)
		{};
		
	/** Operator= overload */
	inline CXBindingsForEachChild& operator=(const CXBindingsForEachChild& rhs)
	{
		m_of = rhs.m_of;
		m_rules = rhs.m_rules;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsForEachChild& rhs) const
	{
		return (
				m_of == rhs.m_of &&
				m_rules == rhs.m_rules
				);
	}
	
	/** setter and getter for of */
	PROPERTY( std::string , Of , of )
	
	/** setter and getter for rules */
	PROPERTY( CXBindingsArrayGrammarRule , Rules , rules )
	
private :
	
	std::string 				m_of;
	CXBindingsArrayGrammarRule 		m_rules;
};

/**
 * @class CXBindingsChild
 * @brief class for handling child nodes in a grammar 
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsChild : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsChild)
	
public :

	/** Default ctor */
	CXBindingsChild():
		CXBindingsObjectBase(),
		m_minOccurs(0),
		m_maxOccurs(-1)
		{};
		
	/** Copy ctor */
	CXBindingsChild(const CXBindingsChild& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_minOccurs(rhs.m_minOccurs),
		m_maxOccurs(rhs.m_maxOccurs),
		m_properties(rhs.m_properties),
		m_rules(rhs.m_rules)
		{};
		
	/** Operator= overload */
	inline CXBindingsChild& operator=(const CXBindingsChild& rhs)
	{
		m_name = rhs.m_name;
		m_minOccurs = rhs.m_minOccurs;
		m_maxOccurs = rhs.m_maxOccurs;
		m_properties = rhs.m_properties;
		m_rules = rhs.m_rules;
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsChild& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_minOccurs == rhs.m_minOccurs &&
				m_maxOccurs == rhs.m_maxOccurs &&
				m_properties == rhs.m_properties &&
				m_rules == rhs.m_rules 
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for minOccurs */
	PROPERTY( int , MinOccurs , minOccurs )
	
	/** setter and getter for maxOccurs */
	PROPERTY( int , MaxOccurs , maxOccurs )
	
	/** setter and getter for properties */
	PROPERTY( CXBindingsArrayGrammarProperty , Properties , properties )
	
	/** setter and getter for rules */
	PROPERTY( CXBindingsArrayGrammarRule , Rules , rules )
	
private :
	
	std::string 				m_name;
	int 					m_minOccurs;
	int 					m_maxOccurs;
	CXBindingsArrayGrammarProperty 	m_properties;
	CXBindingsArrayGrammarRule 		m_rules;
};

/**
 * @class CXBindingsCategory
 * @brief class for handling category nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsCategory : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsCategory)
	
public :

	/** Default ctor */
	CXBindingsCategory():
		CXBindingsObjectBase(),
		m_minOccurs(0),
		m_maxOccurs(-1)
		{};
		
	/** Copy ctor */
	CXBindingsCategory(const CXBindingsCategory& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_minOccurs(rhs.m_minOccurs),
		m_maxOccurs(rhs.m_maxOccurs),
		m_childs(rhs.m_childs),
		m_containers(rhs.m_containers)
		{};
		
	/** Operator= overload */
	inline CXBindingsCategory& operator=(const CXBindingsCategory& rhs)
	{
		m_name = rhs.m_name;
		m_minOccurs = rhs.m_minOccurs;
		m_maxOccurs = rhs.m_maxOccurs;
		m_childs = rhs.m_childs;
		m_containers = rhs.m_containers;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsCategory& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_minOccurs == rhs.m_minOccurs &&
				m_maxOccurs == rhs.m_maxOccurs &&
				m_childs == rhs.m_childs &&
			        m_containers == rhs.m_containers	
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for minOccurs */
	PROPERTY( int , MinOccurs , minOccurs )
	
	/** setter and getter for maxOccurs */
	PROPERTY( int , MaxOccurs , maxOccurs )
	
	/** setter and getter for childs */
	PROPERTY( CXBindingsArrayGrammarChild , Childs , childs )

	/** setter and getter for containers */
	PROPERTY( CXBindingsArrayGrammarChildContainer , ChildContainers , containers )
	
private :

	std::string 				m_name;
	int 					m_minOccurs;
	int 					m_maxOccurs;
	CXBindingsArrayGrammarChild		m_childs;
	CXBindingsArrayGrammarChildContainer 	m_containers;
};


/**
 * @class CXBindingsChildContainer
 * @brief class for handling childContainer nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsChildContainer : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsChildContainer)
	
public :

	/** Default ctor */
	CXBindingsChildContainer():
		CXBindingsObjectBase(),
		m_minOccurs(0),
		m_maxOccurs(-1)
		{};
		
	/** Copy ctor */
	CXBindingsChildContainer(const CXBindingsChildContainer& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_minOccurs(rhs.m_minOccurs),
		m_maxOccurs(rhs.m_maxOccurs),
		m_childs(rhs.m_childs),
		m_properties(rhs.m_properties),
		m_rules(rhs.m_rules),
		m_childRules(rhs.m_childRules),
		m_categories(rhs.m_categories)
		{};
		
	/** Operator= overload */
	inline CXBindingsChildContainer& operator=(const CXBindingsChildContainer& rhs)
	{
		m_name = rhs.m_name;
		m_minOccurs = rhs.m_minOccurs;
		m_maxOccurs = rhs.m_maxOccurs;
		m_childs = rhs.m_childs;
		m_properties = rhs.m_properties;
		m_rules = rhs.m_rules;
		m_childRules = rhs.m_childRules;
		m_categories = rhs.m_categories;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsChildContainer& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_minOccurs == rhs.m_minOccurs &&
				m_maxOccurs == rhs.m_maxOccurs &&
				m_childs == rhs.m_childs &&
				m_properties == rhs.m_properties &&
				m_rules == rhs.m_rules &&
				m_childRules == rhs.m_childRules &&
			        m_categories == rhs.m_categories	
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for minOccurs */
	PROPERTY( int , MinOccurs , minOccurs )
	
	/** setter and getter for maxOccurs */
	PROPERTY( int , MaxOccurs , maxOccurs )
	
	/** setter and getter for childs */
	PROPERTY( CXBindingsArrayGrammarChild , Childs , childs )
	
	/** setter and getter for properties */
	PROPERTY( CXBindingsArrayGrammarProperty , Properties , properties )
	
	/** setter and getter for rules */
	PROPERTY( CXBindingsArrayGrammarRule , Rules , rules )
	
	/** setter and getter for childRules */
	PROPERTY( CXBindingsArrayGrammarForEachChild , ChildRules , childRules )

	/** setter and getter for categories */
	PROPERTY( CXBindingsArrayGrammarCategory , Categories , categories )
	
private :

	std::string 					m_name;
	int 						m_minOccurs;
	int 						m_maxOccurs;
	CXBindingsArrayGrammarChild			m_childs;
	CXBindingsArrayGrammarProperty		m_properties;
	CXBindingsArrayGrammarRule			m_rules;
	CXBindingsArrayGrammarForEachChild 	m_childRules;
	CXBindingsArrayGrammarCategory 		m_categories;
};

/**
 * @class CXBindingsModel
 * @brief class for handling Model nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsModel : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsModel)
	
public :

	/** Default ctor */
	CXBindingsModel():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsModel(const CXBindingsModel& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_childs(rhs.m_childs),
		m_properties(rhs.m_properties),
		m_rules(rhs.m_rules),
		m_childRules(rhs.m_childRules),
		m_categories(rhs.m_categories),
		m_childContainers(rhs.m_childContainers),
		m_models(rhs.m_models)
		{};
		
	/** Operator= overload */
	inline CXBindingsModel& operator=(const CXBindingsModel& rhs)
	{
		m_name = rhs.m_name;
		m_childs = rhs.m_childs;
		m_properties = rhs.m_properties;
		m_rules = rhs.m_rules;
		m_childRules = rhs.m_childRules;
		m_categories = rhs.m_categories;
		m_childContainers = rhs.m_childContainers;
		m_models = rhs.m_models;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsModel& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_childs == rhs.m_childs &&
				m_properties == rhs.m_properties &&
				m_rules == rhs.m_rules &&
				m_childRules == rhs.m_childRules &&
				m_categories == rhs.m_categories &&
				m_childContainers == rhs.m_childContainers &&
				m_models == rhs.m_models
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for childs */
	PROPERTY( CXBindingsArrayGrammarChild , Childs , childs )
	
	/** setter and getter for properties */
	PROPERTY( CXBindingsArrayGrammarProperty , Properties , properties )
	
	/** setter and getter for rules */
	PROPERTY( CXBindingsArrayGrammarRule , Rules , rules )
	
	/** setter and getter for childRules */
	PROPERTY( CXBindingsArrayGrammarForEachChild , ChildRules , childRules )
	
	/** setter and getter for categories */
	PROPERTY( CXBindingsArrayGrammarCategory , Categories , categories )
	
	/** setter and getter for childContainers */
	PROPERTY( CXBindingsArrayGrammarChildContainer , ChildContainers , childContainers )
	
	/** setter and getter for models */
	PROPERTY( CXBindingsArrayGrammarInherits , Models , models )
	
private :
		
	std::string 					m_name;
	CXBindingsArrayGrammarChild			m_childs;
	CXBindingsArrayGrammarProperty		m_properties;
	CXBindingsArrayGrammarRule			m_rules;
	CXBindingsArrayGrammarForEachChild 	m_childRules;
	CXBindingsArrayGrammarCategory 		m_categories;
	CXBindingsArrayGrammarChildContainer m_childContainers;
	CXBindingsArrayGrammarInherits 		m_models;
};


/**
 * @class CXBindingsObject
 * @brief class for handling object nodes in a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsObject : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsObject)
	
public :

	/** Default ctor */
	CXBindingsObject():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindingsObject(const CXBindingsObject& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_childs(rhs.m_childs),
		m_properties(rhs.m_properties),
		m_rules(rhs.m_rules),
		m_childRules(rhs.m_childRules),
		m_categories(rhs.m_categories),
		m_childContainers(rhs.m_childContainers),
		m_models(rhs.m_models)
		{};
		
	/** Operator= overload */
	inline CXBindingsObject& operator=(const CXBindingsObject& rhs)
	{
		m_name = rhs.m_name;
		m_childs = rhs.m_childs;
		m_properties = rhs.m_properties;
		m_rules = rhs.m_rules;
		m_childRules = rhs.m_childRules;
		m_categories = rhs.m_categories;
		m_childContainers = rhs.m_childContainers;
		m_models = rhs.m_models;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsObject& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_childs == rhs.m_childs &&
				m_properties == rhs.m_properties &&
				m_rules == rhs.m_rules &&
				m_childRules == rhs.m_childRules &&
				m_categories == rhs.m_categories &&
				m_childContainers == rhs.m_childContainers &&
				m_models == rhs.m_models 
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for childs */
	PROPERTY( CXBindingsArrayGrammarChild , Childs , childs )
	
	/** setter and getter for properties */
	PROPERTY( CXBindingsArrayGrammarProperty , Properties , properties )
	
	/** setter and getter for rules */
	PROPERTY( CXBindingsArrayGrammarRule , Rules , rules )
	
	/** setter and getter for childRules */
	PROPERTY( CXBindingsArrayGrammarForEachChild , ChildRules , childRules )
	
	/** setter and getter for categories */
	PROPERTY( CXBindingsArrayGrammarCategory , Categories , categories )
	
	/** setter and getter for childContainers */
	PROPERTY( CXBindingsArrayGrammarChildContainer , ChildContainers , childContainers )
	
	/** setter and getter for models */
	PROPERTY( CXBindingsArrayGrammarInherits , Models , models )
	
private :
		
	std::string 					m_name;
	CXBindingsArrayGrammarChild			m_childs;
	CXBindingsArrayGrammarProperty		m_properties;
	CXBindingsArrayGrammarRule			m_rules;
	CXBindingsArrayGrammarForEachChild 	m_childRules;
	CXBindingsArrayGrammarCategory 		m_categories;
	CXBindingsArrayGrammarChildContainer m_childContainers;
	CXBindingsArrayGrammarInherits 		m_models;
};



/**
 * @class CXBindings
 * @brief class for handling the whole grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindings : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindings)
	
public :

	/** Default ctor */
	CXBindings():
		CXBindingsObjectBase()
		{};
		
	/** Copy ctor */
	CXBindings(const CXBindings& rhs):
		CXBindingsObjectBase(),
		m_name(rhs.m_name),
		m_models(rhs.m_models),
		m_objects(rhs.m_objects),
		m_info(rhs.m_info),
		m_vars(rhs.m_vars)
		{};
		
	/** Operator= overload */
	inline CXBindings& operator=(const CXBindings& rhs)
	{
		m_name = rhs.m_name;
		m_models = rhs.m_models;
		m_objects = rhs.m_objects;
		m_info = rhs.m_info;
		m_vars = rhs.m_vars;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindings& rhs) const
	{
		return (
				m_name == rhs.m_name &&
				m_objects == rhs.m_objects &&
				m_models == rhs.m_models &&
				m_info == rhs.m_info &&
				m_vars == rhs.m_vars
				);
	}
	
	/** setter and getter for name */
	PROPERTY( std::string , Name , name )
	
	/** setter and getter for models */
	PROPERTY( CXBindingsArrayGrammarModel , Models , models )
	
	/** setter and getter for objects */
	PROPERTY( CXBindingsArrayGrammarObject , Objects , objects )
	
	/** setter and getter for info */
	PROPERTY( CXBindingsInfo , Info , info )
	
	/** setter and getter for vars */
	PROPERTY( CXBindingsArrayGrammarSet , Vars , vars )
	
	/*********************************************************************
	 * SPECIAL ACCESSORS 
	 ********************************************************************/
	 
	/** Find an object in the array given it's name */
	CXBindingsObject* FindObject( const std::string& name );
	
	/** Find an object in the array given it's name */
	CXBindingsModel* FindModel( const std::string& name );
	
	/** Find a variable accordingly to it's name */
	CXBindingsSet* FindVariable( const std::string& name );
	
private :
		
	std::string 					m_name;
	CXBindingsArrayGrammarModel 		m_models;
	CXBindingsArrayGrammarObject 		m_objects;
	CXBindingsInfo				m_info;
	CXBindingsArrayGrammarSet			m_vars;
};

#endif
