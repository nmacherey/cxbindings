/**
 * @file CXBindingsExpressionParser.h
 * @brief main file for parsing a grammar expression defined in an xml document
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
 
#ifndef CXBINDINGS_EXPRESSION_PARSER_H
#define CXBINDINGS_EXPRESSION_PARSER_H

/** definitions */
class CXBindingsTreeNode;
class CXBindingsTree;

/** arrays */
typedef std::vector<CXBindingsTreeNode> CXBindingsArrayGrammarTreeNode;

enum CXBindingsTreeContent
{
	tcVALUE = 0,
	tcCONCAT,
	tcFUNCTION,
	tcOBJECT_PROPERTY,
	tcNUM
};

/** enumerates the parser actions */
enum CXBindingsParserAction
{
	paS = 0,
	paR,
	paA,
	paE1,
	paE2,
	paE3,
	paE4,
	paE5,
	paE6
};

/* enumerates operators */
enum CXBindingsOperator
{
	opADD = 0,
	opSUB,
	opMUL,
	opDIV,
	opAND,
	opOR,
	opHAS_PARENT, // hasParent(str)
	opHAS_CHILD,  // hasChild(str)
	opLPARENTHESIS,
	opRPARENTHESIS,
	opDOLLAR,
	opEQUAL,
	opEQUALEQUAL,
	opNOTEQUAL,
	opGT,
	opLT,
	opGTOREQUAL,
	opLTOREQUAL,
	opEOS,
	/* keep always that one last :D */
	opVALUE,
	opNUMBER_OF_OP
};

const std::string CXBindingsOperators []= {
	"+" ,
	"-" ,
	"*" ,
	"/" ,
	"&&" ,
	"||" ,
	"hasParent" ,
	"hasChild" ,
	"(" ,
	")" ,
	"$" ,
	"=" ,
	"==" ,
	"!=" ,
	">" ,
	"<" ,
	">=" ,
	"<=" ,
	"End of String" 
};

const int CXBindingsNoOperators = sizeof(CXBindingsOperators)/sizeof(std::string);

extern std::string CXBindingsOperatorToString( CXBindingsOperator op );
extern CXBindingsOperator CXBindingsOperatorStringToOperator( const std::string& opStr );

const CXBindingsParserAction parserTable[opNUMBER_OF_OP][opNUMBER_OF_OP] = {
	//  +  	  -    *     /     &&    ||   hp  ,  hc     (      )    $     =    ==    !=     >     <     >=    <=    \0
/* +  */ { paR, paR , paS , paS , paR , paR , paS ,  paS , paS , paR , paS , paR , paR , paR , paR , paR , paR , paR , paR },
/* -  */ { paR, paR , paS , paS , paR , paR , paS ,  paS , paS , paR , paS , paR , paR , paR , paR , paR , paR , paR , paR },
/* *  */ { paR, paR , paR , paR , paR , paR , paS ,  paS , paS , paR , paS , paR , paR , paR , paR , paR , paR , paR , paR },
/* /  */ { paR, paR , paR , paR , paR , paR , paS ,  paS , paS , paR , paS , paR , paR , paR , paR , paR , paR , paR , paR },
/* && */ { paR, paR , paS , paS , paR , paR , paS ,  paS , paS , paR , paS , paS , paS , paS , paS , paS , paS , paS , paR },
/* || */ { paR, paR , paS , paS , paR , paR , paS ,  paS , paS , paR , paS , paS , paS , paS , paS , paS , paS , paS , paR },
/* hp */ {paE4,paE4 ,paE4 ,paE4 , paR , paR , paR ,  paR , paS , paR ,paE4 ,paE4 , paR , paR ,paE4 ,paE4 ,paE4 ,paE4 , paR },
/* hc */ {paE4,paE4 ,paE4 ,paE4 , paR , paR ,paE4 , paE4 , paS , paR ,paE4 ,paE4 , paR , paR ,paE4 ,paE4 ,paE4 ,paE4 , paR },
/* (  */ { paS, paS , paS , paS , paS , paS , paS ,  paS , paS , paS , paS ,paE1 ,paE1 ,paE1 ,paE1 ,paE1 ,paE1 ,paE1 ,paR },
/* )  */ { paR, paR , paR , paR , paR , paR ,paE3 , paE3 ,paE2 , paR , paR , paR , paR , paR , paR , paR , paR , paR , paR },
/* $  */ {paE5,paE5 ,paE5 ,paE5 ,paE5 ,paE5 ,paE5 , paE5 , paS , paR ,paE5 ,paE5 ,paE5 ,paE5 ,paE5 ,paE5 ,paE5 ,paE5 ,paR },
/* =  */ { paS, paS , paS , paS , paR , paR , paS , paS , paS , paS , paS  ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paR },
/* == */ { paS, paS , paS , paS , paR , paR , paS , paS , paS , paS , paS  ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paR },
/* != */ { paS, paS , paS , paS , paR , paR , paS , paS , paS , paS , paS  ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paR },
/* >  */ { paS, paS , paS , paS , paR , paR , paS , paS , paS , paS , paS  ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paR },
/* <  */ { paS, paS , paS , paS , paR , paR , paS , paS , paS , paS , paS  ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paR },
/* >= */ { paS, paS , paS , paS , paR , paR , paS , paS , paS , paS , paS  ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paR },
/* <= */ { paS, paS , paS , paS , paR , paR , paS , paS , paS , paS , paS  ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paE6 ,paR },
/* \0 */ { paS, paS , paS , paS , paS , paS , paS , paS , paS ,paE3 , paS  , paS , paS , paS , paS , paS , paS , paS ,paA }
};

const std::string CXBindingsErroMsg [] = {
	"Error missing right parethesis !" ,
	"Error missing operator !" ,
	"Error unbalanced right parethesis !" ,
	"Error invalid function argument !" ,
	"Error invalid variable call !" ,
	"Error invalid operator !" 
};

/**
 * @class CXBindingsTreeNode
 * @brief main class for handling a tree element for condition parsing
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsTreeNode : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsTreeNode)

public :
	
	/** default constructor */
	CXBindingsTreeNode() : 
		CXBindingsObjectBase()
		{};
	
	/** Copy ctor */
	CXBindingsTreeNode(const CXBindingsTreeNode& rhs):
		CXBindingsObjectBase(),
		m_content(rhs.m_content),
		m_type(rhs.m_type),
		m_childs(rhs.m_childs)
		{};
		
	/** Operator= overload */
	inline CXBindingsTreeNode& operator=(const CXBindingsTreeNode& rhs)
	{
		m_content = rhs.m_content;
		m_type = rhs.m_type;
		m_childs = rhs.m_childs;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsTreeNode& rhs) const
	{
		return (
				m_content == rhs.m_content &&
				m_type == rhs.m_type &&
				m_childs == rhs.m_childs
				);
	}
	
	/** setter and getter for content */
	PROPERTY( std::string , Content , content )
	
	/** setter and getter for type */
	PROPERTY( CXBindingsOperator , Type , type )
	
	/** setter and getter for childs */
	PROPERTY( CXBindingsArrayGrammarTreeNode , Childs , childs )
	
private :
	
	std::string m_content;
	CXBindingsOperator m_type;
	CXBindingsArrayGrammarTreeNode m_childs;
};

/**
 * @class CXBindingsTree
 * @brief main class for handling a tree for condition parsing
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsTree : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsTree)

public :
	
	/** default constructor */
	CXBindingsTree() : 
		CXBindingsObjectBase()
		{};
	
	/** Copy ctor */
	CXBindingsTree(const CXBindingsTree& rhs):
		CXBindingsObjectBase(),
		m_child(rhs.m_child)
		{};
		
	/** Operator= overload */
	inline CXBindingsTree& operator=(const CXBindingsTree& rhs)
	{
		m_child = rhs.m_child;
		
		return (*this);
	}
	
	/** Operator== overload */
	inline bool operator==(const CXBindingsTree& rhs) const
	{
		return (
				m_child == rhs.m_child
				);
	}
	
	/** setter and getter for childs */
	PROPERTY( CXBindingsTreeNode , Child , child )
	
	/** Simple method to make sure that the tree is cleared */
	void Clear() { CXBindingsTreeNode child; m_child = child; }
	
private :

	CXBindingsTreeNode m_child;
};


/**
 * @class CXBindingsExpressionParser
 * @brief main class for parsing grammar expressions in CXBindings
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
class CXBindingsExpressionParser : public CXBindingsObjectBase
{
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(CXBindingsExpressionParser)
public :
	
	/** Parse the given expression */
	CXBindingsTree Parse(const std::string& expression);
	
private :
	
	CXBindingsOperator GetToken(unsigned int& pos);
	void GetNextToken();
	
	bool DoParse();
	bool DoShift();
	bool DoReduce();
	
	std::string m_expr;
	std::string m_buffer;
	
	CXBindingsOperator m_currentOperator;
	CXBindingsOperator m_lastOperator;
	unsigned int m_pos;
	
	std::stack<CXBindingsTreeNode> m_stack;
	std::stack<CXBindingsOperator> m_opStack;
};

#endif
