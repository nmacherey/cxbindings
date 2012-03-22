/**
 * @file CXBindingsExpressionParser.cpp
 * @brief main file for parsing a grammar defined in an xml document
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */

#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <stack>




#include "CXBindingsDefinitions.h"

#include "CXBindingsGlobals.h"
#include "CXBindingsException.h"
#include "CXBindingsExpressionParser.h"


IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsTreeNode,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsTree,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsExpressionParser,CXBindingsObjectBase);


std::string CXBindingsOperatorToString( CXBindingsOperator op )
{
	if( op < opVALUE )
		return CXBindingsOperators[op];
		
	return stdEmptyString;
}

CXBindingsOperator CXBindingsOperatorStringToOperator( const std::string& opStr )
{
	for( unsigned int i = 0; i < (unsigned int)CXBindingsNoOperators ; ++i )
		if( CXBindingsOperators[i] ==opStr) 
			return (CXBindingsOperator) i;
	
	return opNUMBER_OF_OP;
}

CXBindingsTree CXBindingsExpressionParser::Parse(const std::string& expression) 
{
	m_expr = expression;

	m_opStack.push(opEOS);
	/*wxLogMessage( "******************************************************")  ;
	wxLogMessage( "**** Parsing expression : ") + expression + " ****"   ;
	wxLogMessage( "******************************************************")  ;*/
	
	DoParse();
	
	// Extract the last stack element which will be the tree root 
	CXBindingsTreeNode root = m_stack.top();
	m_stack.pop();

	CXBindingsTree tree;
	tree.SetChild(root);
	
	/*wxLogMessage( "******************************************************")  ;
	wxLogMessage( "**** End Parsing expression : ") + expression + " ****"   ;
	wxLogMessage( "******************************************************")  ;*/
	
	m_opStack.pop();
	
	return tree;
}

CXBindingsOperator CXBindingsExpressionParser::GetToken(unsigned int& pos) {
	int sizFound = 0;
	std::string longest;

	for( unsigned int i = 0; i < (unsigned int)CXBindingsNoOperators ; ++i ) {
		int opSize = CXBindingsOperators[i].size();
		if( pos + opSize <= m_expr.size() ) {
			std::string temp = m_expr.substr(pos,opSize);
			if( temp ==CXBindingsOperators[i] && opSize > sizFound)  {
				sizFound = opSize;
				longest = temp;
			}
		}
	}

	pos += sizFound;
	return CXBindingsOperatorStringToOperator( longest );
}

bool CXBindingsExpressionParser::DoParse() {
	
	m_buffer.clear();
	m_opStack.push(opEOS);
	
	GetNextToken();
	
	while(1) 
	{
		if( m_currentOperator == opVALUE )
		{
			if( DoShift() ) 
				return true;
				
			continue;
		}

		//wxLogMessage( "Found Operator : ") + CXBindingsOperatorToString(m_currentOperator)  ;

		// do something to store the info
		//wxLogMessage( std::string::Format( "%d-%d-%d") , m_opStack.top() , m_currentOperator , parserTable[m_opStack.top()][m_currentOperator] )  ;

		switch (parserTable[m_opStack.top()][m_currentOperator]) {
		case paA : {
			
			if( m_stack.size() != 1 )
				CXB_THROW( "Value stack size is greather than 1 !")  ;
				
			return true;
		}
		case paR : {
			if( DoReduce() ) 
				return true;
			
			break;
		}
		case paS : {
			if( DoShift() ) 
				return true;
				
			break;
		}
		default :
			CXB_THROW( CXBindingsErroMsg[parserTable[m_opStack.top()][m_currentOperator]-paE1]);
		};
	}
	
	return false;
}

void CXBindingsExpressionParser::GetNextToken()
{
	unsigned int pos = 0;
	m_buffer.clear();
	
	CXBindingsOperator op;
	
	//wxLogMessage( "Expression is :") + m_expr  ;
	
	if( m_expr.empty() )
	{
		m_currentOperator = opEOS;
		return;
	}
	
	op = opNUMBER_OF_OP;
	
	while( op >= opVALUE && !m_expr.empty() ) 
	{
		unsigned int newPos = 0;
			
		op = GetToken(newPos);
		
		if( newPos == 0 )
		{
			if( m_expr[0] != ' ')  
			{
				m_buffer += m_expr[0];
				pos++;
				m_expr.erase(0,1);
			}
			else
				m_expr.erase(0,1);
		}
		else if( newPos > 0 && pos == 0 )
		{
			pos = newPos;
			m_expr.erase(0,pos);
			break;
		}
		else
			break;
	}
	
	m_lastOperator = m_currentOperator;
	
	if( !m_buffer.empty() )
		m_currentOperator = opVALUE;
	else
		m_currentOperator = op;
}

bool CXBindingsExpressionParser::DoShift() 
{	
	if( m_currentOperator == opVALUE )
	{
		CXBindingsTreeNode node;
		node.SetContent(m_buffer);
		node.SetType(opVALUE);
		
		m_stack.push(node);
		//wxLogMessage( "Pushed buffer on stack : ") + m_buffer  ;
	}
	else if( m_currentOperator < opVALUE )
	{
		//wxLogMessage( "Pushed operator on stack : ") + CXBindingsOperatorToString(m_currentOperator)  ;
		m_opStack.push(m_currentOperator);
	}
	
	GetNextToken();
	return false;
}

bool CXBindingsExpressionParser::DoReduce() 
{
	//wxLogMessage( "Inreduce : ") + CXBindingsOperatorToString(m_opStack.top())  ;
	switch(m_opStack.top())
	{
		case opHAS_PARENT :
		{
			if( m_stack.size() < 1 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 1 !")  ;
				
			CXBindingsTreeNode elem = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "hasParent")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem);
			
			m_stack.push(parent);
			break;
		}
		case opHAS_CHILD :
		{
			if( m_stack.size() < 1 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 1 !")  ;
				
			CXBindingsTreeNode elem = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "hasChild")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem);
			
			m_stack.push(parent);
			break;
		}
		case opDOLLAR:
		{
			if( m_stack.size() < 1 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 1 !")  ;
				
			CXBindingsTreeNode elem = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "$")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem);
			
			m_stack.push(parent);
			break;
		}
		case opAND :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "&&")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opOR :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "||")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opADD :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "+")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opSUB :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "-")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opMUL :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "*")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opDIV :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "-")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opEQUAL :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "=")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opEQUALEQUAL :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "==")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opNOTEQUAL :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "!=")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opLT :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "<")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opGT :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( ">")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opGTOREQUAL :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( ">=")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opLTOREQUAL :
		{
			if( m_stack.size() < 2 )
				CXB_THROW( "Stack error , your stack is too small shall be at least 2 !")  ;
				
			CXBindingsTreeNode elem1 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode elem2 = m_stack.top();
			m_stack.pop();
			
			CXBindingsTreeNode parent;
			parent.SetContent( "<=")  ;
			parent.SetType(m_opStack.top());
			
			CXBindingsArrayGrammarTreeNode& childs = parent.GetChilds();
			childs.push_back(elem1);
			childs.push_back(elem2);
			
			m_stack.push(parent);
			
			break;
		}
		case opRPARENTHESIS :
		{
			m_opStack.pop();
			break;
		}
		default :
			break;
	}
	m_opStack.pop();
	return false;
}
