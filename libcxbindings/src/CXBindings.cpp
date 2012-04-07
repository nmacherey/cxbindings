/**
 * @file CXBindings.cpp
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

/* Boost specific includes */
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

/* Local includes */
#include "CXBindingsDefinitions.h"
#include "CXBindingsGlobals.h"
#include "CXBindingsException.h"
#include "CXBindingsExpressionParser.h"
#include "CXBindingsAny.h"
#include "CXBindings.h"

/************************************************************************************
 * DYNAMIC CLASS IMPLEMENTATIONS
 ***********************************************************************************/
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsCondition,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsBaseCondition,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsSet,CXBindingsBaseCondition);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsProperty,CXBindingsBaseCondition);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsType,CXBindingsBaseCondition);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsVariable,CXBindingsBaseCondition);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsName,CXBindingsBaseCondition);

IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsRule,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsForEachChild,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsChild,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsCategory,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsChildContainer,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsModel,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsObject,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindings,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsInherits,CXBindingsObjectBase);

IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsGeneration,CXBindingsObjectBase);
IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(CXBindingsInfo,CXBindingsObjectBase);

/************************************************************************************
 * END OF DYNAMIC CLASS IMPLEMENTATIONS
 ***********************************************************************************/

/************************************************************************************
 * CXBindings METHODS IMPLEMENTATIONS
 ***********************************************************************************/

CXBindingsObject* CXBindings::FindObject( const std::string& name ) {
	for( unsigned int i = 0; i < m_objects.size() ; ++i )
		if( m_objects[i].GetName() ==name) 
			return &(m_objects[i]);

	return NULL;
}

CXBindingsModel* CXBindings::FindModel( const std::string& name ) {
	for( unsigned int i = 0; i < m_models.size() ; ++i )
		if( m_models[i].GetName() ==name) 
			return &(m_models[i]);

	return NULL;
}

CXBindingsSet* CXBindings::FindVariable( const std::string& name )
{
	for( unsigned int i = 0; i < m_vars.size() ; ++i )
		if( m_vars[i].GetVar() ==name) 
			return &(m_vars[i]);
	
	return NULL;
}

/************************************************************************************
 * CXBindingsCondition METHODS IMPLEMENTATIONS
 ***********************************************************************************/

bool CXBindingsCondition::Compile()
{
	////wxLogMessage("**** Compiling condition ") + m_condition + " ****"   ;
	
	if( m_compiled )
		return true;
		
	CXBindingsExpressionParser parser;
	m_conditionTree = parser.Parse(m_condition);
	
	CXBindingsExpressionParser parser2;
	m_contentTree = parser2.Parse(m_content);
	
	m_compiled = true;
	return true;
}

CXBindingsAny CXBindingsCondition::EvaluateVar( CXBindingsTreeNode& item , xmlNode* node  , CXBindings& grammar  )
{
	CXBindingsArrayGrammarTreeNode& childs = item.GetChilds();
	CXBindingsAny returned;
	////wxLogMessage( "Item is : ") + item.GetContent()  ;
	
	switch( item.GetType() )
	{
		case opAND :
		{
			bool res = 1;
			for(unsigned int i = 0; i < childs.size() ; ++i )
			{
				CXBindingsAny var =  EvaluateVar( childs[i] , node , grammar );
				
				bool test = var.GetBool();
				res = res && test;
			}
			
			returned = res;
			return returned;
			
			break;
		}
		case opOR :
		{
			bool res = 0;
			for(unsigned int i = 0; i < childs.size() ; ++i )
			{
				CXBindingsAny var =  EvaluateVar( childs[i] , node , grammar );
				
				bool test = var.GetBool();
				res = res || test;
			}
				
			returned = res;
			return returned;
			break;
		}
		case opHAS_PARENT :
		{
			if( childs.size() != 1 )
				CXB_THROW("hasParent expect only 1 child !") ;
				
			CXBindingsAny var =  EvaluateVar( childs[0] , node , grammar );
			if( var.m_var.type() != typeid(std::string)) {
					CXB_THROW("hasParent expected string argument in function") ;
            }
					
			std::string pname = var.GetString();
			if( node->parent == NULL )
				return false;
			
			std::string parentName = cxC2U( (char*) node->parent->name );
			
			returned = pname==parentName;
			return returned;
			break;
		}
		case opHAS_CHILD :
		{
			if( childs.size() != 1 )
				CXB_THROW("hasChild expect only 1 child !") ;
				
			CXBindingsAny var =  EvaluateVar( childs[0] , node , grammar );
			
			if( var.m_var.type() != typeid(std::string))
					CXB_THROW("hasChild expected string argument in function") ;
					
			std::string pname = var.GetString();
			
			xmlNode* child = node->children;
			while( child != NULL )
			{
				std::string cname = cxC2U( (char*) child->name );
				
				if( cname ==pname) 
				{
					returned = true;
					return returned;
				}
					
				child = child->next;
			}

			returned = false;
			return returned;
			break;
		}
		case opDOLLAR :
		{
			if( childs.size() != 1 )
				CXB_THROW("$ expect only 1 child !") ;
				
			CXBindingsAny var = EvaluateVar( childs[0] , node , grammar );
			
			if( var.m_var.type() != typeid(std::string))
					CXB_THROW("$ expected string argument in function") ;
					
			std::string pname = var.GetString();
			
			// Find in the grammar the CXBindingsSet that contains the information for the
			// Containing variable
			
			CXBindingsSet* variable = grammar.FindVariable(pname);
			
			if( variable == NULL )
				CXB_THROW("Cannot find your global variable : " + pname  );
				
			CXBindingsArrayGrammarCondition& conditions = variable->GetConditions();
			
			bool matchfound = false;
			std::string match;
			std::string content;
			
			for( unsigned int j = 0; j < conditions.size() ; ++j )
			{
				conditions[j].Compile();
				
				if( conditions[j].Matches(node,grammar) )
				{
					matchfound = true;
					match = conditions[j].GetMatch( node,grammar );
					
					////wxLogMessage( "\t\t\t Found condition match  : ") + match  ;
				}
			}
			
			if( matchfound )
				content = match;
			else
				content = variable->GetContent();
				
			if( content.empty() )
				CXB_THROW("Set is empty : " + pname );
				
			////wxLogMessage( "\t\t\t Global Variable content  : ") + content  ;
				
			CXB_THROW("$(VARIABLE) macros not supported yet") ;
				
			/** @todo get parent object here in order to know the variables informations */
			if( childs.size() != 1 )
				CXB_THROW("$(VARIABLE) wrong macro call") ;
					
			break;
		}
		case opEQUALEQUAL :
		{
			if( childs.size() != 2 )
				CXB_THROW("Operator == expected two arguments please fix !") ;
			
			CXBindingsAny var1 =  EvaluateVar( childs[0] , node , grammar );
			CXBindingsAny var2 =  EvaluateVar( childs[1] , node , grammar );

			////wxLogMessage( " Var 1 type : " ) + var1.GetType() + "-") + var1.GetString(   ;
			////wxLogMessage( " Var 2 type : " ) + var2.GetType() + "-") + var2.GetString(   ;
		    
			if( var1.m_var.type() == typeid(bool))  
				returned = (var1.GetBool() == var2.GetBool());
			else if( var1.m_var.type() == typeid(double))  
				returned = (var1.GetDouble() == var2.GetDouble());
			else if( var1.m_var.type() == typeid(long))  
				returned = (var1.GetLong() == var2.GetLong());
			else
				CXB_THROW("Operator Greather Than unsuported variable type !") ;
			return returned;

			break;
		}
		case opNOTEQUAL :
		{
			if( childs.size() != 2 )
				CXB_THROW("Operator != expected two arguments please fix !") ;
				
			CXBindingsAny var1 =  EvaluateVar( childs[0] , node , grammar );
			CXBindingsAny var2 =  EvaluateVar( childs[1] , node , grammar );
			
				
			if( var1.m_var.type() == typeid(bool))  
				returned = (var1.GetBool() != var2.GetBool());
			else if( var1.m_var.type() == typeid(double))  
				returned = (var1.GetDouble() != var2.GetDouble());
			else if( var1.m_var.type() == typeid(long))  
				returned = (var1.GetLong() != var2.GetLong());
			else
				CXB_THROW("Operator Greather Than unsuported variable type !") ;
			return returned;
			break;
		}
		case opGT :
		{
			if( childs.size() != 2 )
				CXB_THROW("Operator Greather Than expected two arguments please fix !") ;
				
			CXBindingsAny var1 =  EvaluateVar( childs[1] , node , grammar );
			CXBindingsAny var2 =  EvaluateVar( childs[0] , node , grammar );
			
				
			if( var1.m_var.type() == typeid(bool))  
				returned = (var1.GetBool() > var2.GetBool());
			else if( var1.m_var.type() == typeid(double))  
				returned = (var1.GetDouble() > var2.GetDouble());
			else if( var1.m_var.type() == typeid(long))  
				returned = (var1.GetLong() > var2.GetLong());
			else
				CXB_THROW("Operator Greather Than unsuported variable type !") ;
			
			return returned;
			break;
		}
		case opLT :
		{
			if( childs.size() != 2 )
				CXB_THROW("Operator Less Than expected two arguments please fix !") ;
				
			CXBindingsAny var1 =  EvaluateVar( childs[1] , node , grammar );
			CXBindingsAny var2 =  EvaluateVar( childs[0] , node , grammar );
			
				
			if( var1.m_var.type() == typeid(bool))  
				returned = (var1.GetBool() < var2.GetBool());
			else if( var1.m_var.type() == typeid(double))  
				returned = (var1.GetDouble() < var2.GetDouble());
			else if( var1.m_var.type() == typeid(long))  
				returned = (var1.GetLong() < var2.GetLong());
			else
				CXB_THROW("Operator Less Than unsuported variable type !") ;
				
			return returned;
			break;
		}
		case opGTOREQUAL :
		{
			if( childs.size() != 2 )
				CXB_THROW("Operator Greather Than Or Equal expected two arguments please fix !") ;
				
			CXBindingsAny var1 =  EvaluateVar( childs[1] , node , grammar );
			CXBindingsAny var2 =  EvaluateVar( childs[0] , node , grammar );
			
				
			if( var1.m_var.type() == typeid(bool))  
				returned = (var1.GetBool() >= var2.GetBool());
			else if( var1.m_var.type() == typeid(double))  
				returned = (var1.GetDouble() >= var2.GetDouble());
			else if( var1.m_var.type() == typeid(long))  
				returned = (var1.GetLong() >= var2.GetLong());
			else
				CXB_THROW("Operator Greather Than or Equal unsuported variable type !") ;
			
			return returned;
			break;
		}
		case opLTOREQUAL :
		{
			if( childs.size() != 2 )
				CXB_THROW("Operator Less Than or Equal expected two arguments please fix !") ;
				
			CXBindingsAny var1 =  EvaluateVar( childs[1] , node , grammar );
			CXBindingsAny var2 =  EvaluateVar( childs[0] , node , grammar );

			////wxLogMessage( " Var 1 type : " ) + var1.GetType() + "-") + var1.GetString(   ;
			////wxLogMessage( " Var 2 type : " ) + var2.GetType() + "-") + var2.GetString(   ;
			
				
			if( var1.m_var.type() == typeid(bool))  
				returned = (var1.GetBool() <= var2.GetBool());
			else if( var1.m_var.type() == typeid(double))  
				returned = (var1.GetDouble() <= var2.GetDouble());
			else if( var1.m_var.type() == typeid(long))  
				returned = (var1.GetLong() <= var2.GetLong());
			else
				CXB_THROW("Operator Less Than or Equal unsuported variable type !") ;
			
			std::string msg = "Condition is : " ;
			////wxLogMessage( msg + (returned.GetBool() ? "true") : "false")    ;
				
			return returned;
			break;
		}
		case opVALUE :
		{
			std::string value = item.GetContent();
			boost::replace_all ( value, "'", "" );
			//value.replace("'","" );
			
			/** check if it is a number */
			bool isInt = false;
			bool number = false;

			try
			    {
				boost::lexical_cast<double>(value);
				number = true;
			    }
			    catch(boost::bad_lexical_cast &)
			    {
				// if it throws, it's not a number.
			    }

			try
			    {
				boost::lexical_cast<long>(value);
				isInt = true;
			    }
			    catch(boost::bad_lexical_cast &)
			    {
				// if it throws, it's not a number.
			    }
			if( !number && !isInt )
			{
				// if the value is a string, we have to check if it is not a reference to an object property
				// or to a global variable specific to the node
				std::string content = value;
				xmlNode* cursor = node;
				std::string result;
				std::string buffer;

				while( content.size() > 0 )
				{
					if( content[0] == '{')  
					{
						std::string temp = after_first(content,'{') ;
						temp = before_first(temp,'}') ;
						content = after_first(content,'}') ;
						
						//wxLogMessage( temp + " ") + content  ;
						
						if( temp =="parent")  
						{
							if( cursor->parent == NULL )
								CXB_THROW("Requested parent but node has no parent !") ;
							
							cursor = cursor->parent;
							//wxLogMessage("SWITCHED TO PARENT")  ;
						}
						else
						{
							std::string attr = GetXmlAttr(cursor,temp);
							if( attr.empty() )
								CXB_THROW("Attribute not found : " + temp );
								
							result += attr;
						}
							
					}
					else if( content[0] == '[')  
					{
						std::string temp = after_first(content,'[') ;
						temp = before_first(temp,']') ;
						content = after_first(content,']') ;
						
						result += temp;
					}
					else
					{
						result += content[0];
						content.erase(0,1);
					}
				}

				////wxLogMessage( "Result : ") + result  ;
				if( result =="unbounded")  
					returned = (long) 100000;
                else if( result == "true" ) {
                    returned = true;
                }
                else if( result == "false" ) {
                    returned = false;
                }
				else
				{
					try
					    {
						long val = boost::lexical_cast<long>(result);
						returned = val;
					    }
					    catch(boost::bad_lexical_cast &)
					    {
						returned = result;
					    }
				}
			}
			else if( isInt )
			{
				long val =boost::lexical_cast<long>(value);
				returned = val; 
			}
			else
			{
				double val = boost::lexical_cast<double>(value);
				returned = val;
			}
			
			return returned;
			break;
		}
		default :
			std::string op = " Operator is : %d" + item.GetType()  ;
			CXB_THROW("Error unknown operator in condition cannot evaluate !" + op );
	}
	
	return returned;
};

bool CXBindingsCondition::Matches( xmlNode* node  , CXBindings& grammar )
{
	if( !m_compiled )
		Compile();
		
	// once the expressions have been compiled, we can try
	// to evaluate them.
	CXBindingsAny res = EvaluateVar( m_conditionTree.GetChild() , node , grammar );
	if( res.m_var.type() != typeid(bool))  
		CXB_THROW( "Error conditions are expected to be boolean !")  ;
	
	/*if( res.GetBool() )
		//wxLogMessage("**** Condition ") + m_condition + " MATCHES IN NODE ****"   ;
	else
		//wxLogMessage("**** Condition ") + m_condition + " DOES NOT MATCH IN NODE ****"   ;*/
		
	return res.GetBool();
}

std::string CXBindingsCondition::GetMatch( xmlNode* node , CXBindings& grammar )
{
	if( Matches(node,grammar) )
	{
		std::string content = m_content;
		xmlNode* cursor = node;
		std::string result;
		std::string buffer;
		
		while( content.size() > 0 )
		{
			if( content[0] == '{')  
			{
						std::string temp = after_first(content,'{') ;
						temp = before_first(temp,'}') ;
						content = after_first(content,'}') ;
				
				////wxLogMessage( temp + " ") + content  ;
				
				if( temp =="parent")  
				{
					if( cursor->parent == NULL )
						CXB_THROW("Requested parent but node has no parent !") ;
					
					cursor = cursor->parent;
					////wxLogMessage("SWITCHED TO PARENT")  ;
				}
				else
				{
					std::string attr = GetXmlAttr(cursor,temp);
					if( attr.empty() )
						CXB_THROW("Attribute not found : " + temp );
						
					result += attr;
				}
					
			}
			else if( content[0] == '[')  
			{
						std::string temp = after_first(content,'[') ;
						temp = before_first(temp,']') ;
						content = after_first(content,']') ;
				
				result += temp;
			}
			else
			{
				result += content[0];
				content.erase(0,1);
			}
			
		}
		
		return result;
	}
	
	return stdEmptyString;
}

std::string CXBindingsType::GetTemplateMatch( xmlNode* node , CXBindings& grammar )
{
	if( ! boost::algorithm::starts_with(m_stemplate,"$(") )  
		return m_stemplate;
	
	std::string pname = m_stemplate;

	boost::replace_all ( pname, "$", "" );
	boost::replace_all ( pname, ")", "" );
	boost::replace_all ( pname, "(", "" );

	// Find in the grammar the CXBindingsSet that contains the information for the
	// Containing variable

	CXBindingsSet* variable = grammar.FindVariable(pname);

	if( variable == NULL )
		CXB_THROW("Cannot find your global variable : " + pname );

	CXBindingsArrayGrammarCondition& conditions = variable->GetConditions();

	bool matchfound = false;
	std::string match;
	std::string content;

	for( unsigned int j = 0; j < conditions.size() ; ++j )
	{
		conditions[j].Compile();

		if( conditions[j].Matches(node,grammar) )
		{
			matchfound = true;
			match = conditions[j].GetMatch( node,grammar );

			////wxLogMessage( "\t\t\t Found condition match  : ") + match  ;
			break;
		}
	}

	if( matchfound )
		content = match;
	else
		content = variable->GetContent();

	if( content.empty() )
		CXB_THROW("Set is empty : " + pname );

	////wxLogMessage( "\t\t\t Global Variable content  : ") + content  ;
	return content;
}

std::string CXBindingsType::GetContentMatch( xmlNode* node , CXBindings& )
{
	std::string content = m_content;
	xmlNode* cursor = node;
	std::string result;
	std::string buffer;

	while( content.size() > 0 )
	{
		if( content[0] == '{')  
		{
						std::string temp = after_first(content,'{') ;
						temp = before_first(temp,'}') ;
						content = after_first(content,'}') ;

			////wxLogMessage( temp + " " + content );

			if( temp =="parent")  
			{
				if( cursor->parent == NULL )
					CXB_THROW("Requested parent but node has no parent !") ;

				cursor = cursor->parent;
				////wxLogMessage("SWITCHED TO PARENT")  ;
			}
			else
			{
				std::string attr = GetXmlAttr(cursor,temp);
				if( attr.empty() )
					CXB_THROW("Attribute not found : " + temp );

				result += attr;
			}

		}
		else if( content[0] == '[')  
		{
						std::string temp = after_first(content,'[') ;
						temp = before_first(temp,']') ;
						content = after_first(content,']') ;

			result += temp;
		}
		else
		{
			result += content[0];
			content.erase(0,1);
		}

	}

	return result;
}

std::string CXBindingsVariable::GetTemplateMatch( xmlNode* node , CXBindings& grammar )
{
	if( !boost::algorithm::starts_with(m_stemplate,"$(") )  
		return m_stemplate;
	
	std::string pname = m_stemplate;

	boost::replace_all ( pname, "$", "" );
	boost::replace_all ( pname, ")", "" );
	boost::replace_all ( pname, "(", "" );

	// Find in the grammar the CXBindingsSet that contains the information for the
	// Containing variable

	CXBindingsSet* variable = grammar.FindVariable(pname);

	if( variable == NULL )
		CXB_THROW("Cannot find your global variable : " + pname );

	CXBindingsArrayGrammarCondition& conditions = variable->GetConditions();

	bool matchfound = false;
	std::string match;
	std::string content;

	for( unsigned int j = 0; j < conditions.size() ; ++j )
	{
		conditions[j].Compile();

		if( conditions[j].Matches(node,grammar) )
		{
			matchfound = true;
			match = conditions[j].GetMatch( node,grammar );

			////wxLogMessage( "\t\t\t Found condition match  : ") + match  ;
			break;
		}
	}

	if( matchfound )
		content = match;
	else
		content = variable->GetContent();

	if( content.empty() )
		CXB_THROW("Set is empty : " + pname );

	////wxLogMessage( "\t\t\t Global Variable content  : ") + content  ;
	return content;
}

std::string CXBindingsVariable::GetContentMatch( xmlNode* node , CXBindings& )
{
	std::string content = m_content;
	xmlNode* cursor = node;
	std::string result;
	std::string buffer;

	while( content.size() > 0 )
	{
		if( content[0] == '{')  
		{
						std::string temp = after_first(content,'{') ;
						temp = before_first(temp,'}') ;
						content = after_first(content,'}') ;

			////wxLogMessage( temp + " ") + content  ;

			if( temp =="parent")  
			{
				if( cursor->parent == NULL )
					CXB_THROW("Requested parent but node has no parent !") ;

				cursor = cursor->parent;
				////wxLogMessage("SWITCHED TO PARENT")  ;
			}
			else
			{
				std::string attr = GetXmlAttr(cursor,temp);
				if( attr.empty() )
					CXB_THROW("Attribute not found : " + temp );

				result += attr;
			}

		}
		else if( content[0] == '[')  
		{
						std::string temp = after_first(content,'[') ;
						temp = before_first(temp,']') ;
						content = after_first(content,']') ;

			result += temp;
		}
		else
		{
			result += content[0];
			content.erase(0,1);
		}

	}

	return result;
}

std::string CXBindingsName::GetTemplateMatch( xmlNode* node , CXBindings& grammar )
{
	if( !boost::algorithm::starts_with(m_stemplate,"$(") )  
		return m_stemplate;
	
	std::string pname = m_stemplate;

	boost::replace_all ( pname, "$", "" );
	boost::replace_all ( pname, ")", "" );
	boost::replace_all ( pname, "(", "" );

	// Find in the grammar the CXBindingsSet that contains the information for the
	// Containing variable

	CXBindingsSet* variable = grammar.FindVariable(pname);

	if( variable == NULL )
		CXB_THROW("Cannot find your global variable : " + pname );

	CXBindingsArrayGrammarCondition& conditions = variable->GetConditions();

	bool matchfound = false;
	std::string match;
	std::string content;

	for( unsigned int j = 0; j < conditions.size() ; ++j )
	{
		conditions[j].Compile();

		if( conditions[j].Matches(node,grammar) )
		{
			matchfound = true;
			match = conditions[j].GetMatch( node,grammar );

			////wxLogMessage( "\t\t\t Found condition match  : ") + match  ;
			break;
		}
	}

	if( matchfound )
		content = match;
	else
		content = variable->GetContent();

	if( content.empty() )
		CXB_THROW("Set is empty : " + pname );

	////wxLogMessage( "\t\t\t Global Variable content  : ") + content  ;
	return content;
}

std::string CXBindingsName::GetContentMatch( xmlNode* node , CXBindings& )
{
	std::string content = m_content;
	xmlNode* cursor = node;
	std::string result;
	std::string buffer;

	while( content.size() > 0 )
	{
		if( content[0] == '{')  
		{
						std::string temp = after_first(content,'{') ;
						temp = before_first(temp,'}') ;
						content = after_first(content,'}') ;

			////wxLogMessage( temp + " " + content  );

			if( temp =="parent")  
			{
				if( cursor->parent == NULL )
					CXB_THROW("Requested parent but node has no parent !") ;

				cursor = cursor->parent;
				////wxLogMessage("SWITCHED TO PARENT")  ;
			}
			else
			{
				std::string attr = GetXmlAttr(cursor,temp);
				if( attr.empty() )
					CXB_THROW("Attribute not found : " + temp );

				result += attr;
			}

		}
		else if( content[0] == '[')  
		{
						std::string temp = after_first(content,'[') ;
						temp = before_first(temp,']') ;
						content = after_first(content,']') ;

			result += temp;
		}
		else
		{
			result += content[0];
			content.erase(0,1);
		}

	}

	return result;
}

std::string CXBindingsRule::GetMakeMatch( xmlNode* node , CXBindings& grammar )
{
	if( !boost::algorithm::starts_with(m_make,"$(") )  
		return m_make;
	
	std::string pname = m_make;

	boost::replace_all ( pname, "$", "" );
	boost::replace_all ( pname, ")", "" );
	boost::replace_all ( pname, "(", "" );

	// Find in the grammar the CXBindingsSet that contains the information for the
	// Containing variable

	CXBindingsSet* variable = grammar.FindVariable(pname);

	if( variable == NULL )
		CXB_THROW("Cannot find your global variable : " + pname  );

	CXBindingsArrayGrammarCondition& conditions = variable->GetConditions();

	bool matchfound = false;
	std::string match;
	std::string content;

	for( unsigned int j = 0; j < conditions.size() ; ++j )
	{
		conditions[j].Compile();

		if( conditions[j].Matches(node,grammar) )
		{
			matchfound = true;
			match = conditions[j].GetMatch( node,grammar );

			////wxLogMessage( "\t\t\t Found condition match  : ") + match  ;
			break;
		}
	}

	if( matchfound )
		content = match;
	else
		content = variable->GetContent();

	if( content.empty() )
		CXB_THROW("Set is empty : " + pname  );

	////wxLogMessage( "\t\t\t Global Variable content  : ") + content  ;
	return content;
}
