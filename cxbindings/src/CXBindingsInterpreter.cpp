/**
 * @file CXBindingsInterpreter.h
 * @brief main file for generating c++ objects from a grammar
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 * @version 10.4
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <stack>





#include "CXBindingsDefinitions.h"
#include "CXBindingsGlobals.h"
#include "CXBindingsException.h"
#include "CXBindingsExpressionParser.h"
#include "CXBindings.h"
#include "CXBindingsInterpreter.h"

namespace
{
	typedef std::vector< xmlNode* > xmlNodeArray;

	void GetNodeList( xmlNodeArray& nodes, xmlNode* parent , const std::string& name , int level )
	{
		xmlNode* child = parent->children;
		while( child != NULL )
		{
			std::string cname = cxC2U( (char*) child->name );

			if( cname ==name) 
				nodes.push_back(child);

			if(level != 1)
				GetNodeList( nodes, child , name , level );

			child = child->next;
		}
	}
}

CXBindingsInterpreterInfo CXBindingsInterpreter::DoExtractObjectsInformation( const std::string& file , CXBindings& grammar , const CXBindingsGeneratorOptions& options )
{
	////wxLogMessage( "Generating object code for your grammar : ") + grammar.GetName()  ;
	////wxLogMessage( "Generating object code for your file : ") + file  ;

	CXBindingsInterpreterInfo result;
	result.grammar = grammar;

	/* Init libxml */     
	xmlInitParser();

	/* Load XML document */
	m_doc = xmlParseFile(cxcU2C(file));

	if (m_doc == NULL)
		CXB_THROW( "Error cannot open your document !")  ;

	/* Get the document root */
	m_root = xmlDocGetRootElement(m_doc);

	////wxLogMessage( "Xml Document file as root : ") + cxC2U( (char*) m_root->name )  ;

	/* for each element in the info object of the grammar */
	CXBindingsInfo& info = grammar.GetInfo();
	CXBindingsArrayString genOrder = info.GetGeneration().GetObjects();
	CXBindingsArrayInt levels = info.GetGeneration().GetLevels();

	for( unsigned int i = 0; i < genOrder.size() ; ++i )
	{
		////wxLogMessage( "\t Begin generation for : ") + genOrder[i]  ;

		/* Get all nodes of the given type */
		xmlNodeArray m_nodes;
		GetNodeList( m_nodes , m_root , genOrder[i] , levels[i] );

		/* Display the total number of nodes founded */
		//wxLogMessage( std::string::Format( "\t\t Founded %d node of type : ") , m_nodes.size() ) + genOrder[i]  ;

		/* Get the grammar object associated */
		CXBindingsObject* obj = grammar.FindObject(genOrder[i]);

		if( obj == NULL )
			CXB_THROW( "Error object not registered in the grammar !")  ;

		/* First if the object has a model, simply happend the model properties to the 
		 * object properties, Hence, we can only work on the CXBindingsObject */

		CXBindingsArrayGrammarInherits& models = obj->GetModels();
		for( unsigned int j = 0; j < models.size() ; ++j )
		{
			//wxLogMessage( "\t\t\t Appending model : ") + models[j].GetName() + " properties to object ") + obj->GetName(   ;
			AppendModelToObject( grammar , *obj , models[j].GetName() );
		}

		CXBindingsArrayGrammarObjectInfo objInfos;

		/* Now for each node we have to generate the object code */
		for( unsigned int j = 0; j < m_nodes.size() ; ++j )
			objInfos.push_back( DoExtractNodeInformation(grammar,*obj,m_nodes[j],options) );

		result.objects[genOrder[i]] = objInfos;
		//wxLogMessage( "\t End generation for : ") + genOrder[i]  ;
	}

	/* Cleanup */
	xmlFreeDoc(m_doc);
	xmlCleanupParser();
	xmlMemoryDump();

	return result;
}

CXBindingsObjectInfo CXBindingsInterpreter::DoExtractNodeInformation( CXBindings& grammar , CXBindingsObject& object , xmlNode* node , const CXBindingsGeneratorOptions&  )
{
	CXBindingsObjectInfo objectInfo;

	//wxLogMessage( "\t\t Generating Object generation informations for object : ") + object.GetName()  ;

	/* First check if the node has the corresponding properties
	 * Assuming that each property is unique we can create a map of 
	 * properties with name and value.
	 * Properties are attributes of the given node. If the property 
	 * is required and is not available, we shall generate an exception
	 * so that the user will know what is the problem. */
	CXBindingsArrayGrammarProperty&	properties = object.GetProperties();
	CXBindingsStringStringMap pValues = DoExtractPropertiesFrom( node , properties , grammar );


	CXBindingsArrayGrammarChild& childs = object.GetChilds();
	CXBindingsArrayGrammarChildInfo childinfos = DoExtractChildrenInformationFrom( node , childs , grammar );

	/* Once we've found all properties, we have to check if the node contains
	 * child containers. Which means that the node can contain childrens and
	 * a specific action has to be done accordinly to each childs */
	CXBindingsArrayGrammarChildContainer& containers = object.GetChildContainers();
	CXBindingsArrayGrammarChildContainerInfo childContainersInfo = DoExtractChildContainersFrom( node , containers , grammar );	

	/* Once we've found all containers, we have to check if the node contains
	 * categories. Which means that the node can contain childrens */
	CXBindingsArrayGrammarCategory& categories = object.GetCategories();
	CXBindingsArrayGrammarCategoryInfo categoriesInfo = DoExtractCategoriesFrom( node , categories , grammar );	

	/* Extract all available rules informations */
	CXBindingsArrayGrammarRule& rules = object.GetRules();
	CXBindingsArrayGrammarRuleInfo rInfos = DoExtractRuleInformationFrom( node , rules , grammar );

	/* Finally store the extracted grammar information in the returned information
	 * structure.
	 */	
	objectInfo.properties = pValues;
	objectInfo.childs = childContainersInfo;
	objectInfo.categories = categoriesInfo;
	objectInfo.object = object;
	objectInfo.rules = rInfos;
	objectInfo.childInfos = childinfos;

	//wxLogMessage( "\t\t End Object generation information for object : ") + object.GetName()  ;
	return objectInfo;
}

void CXBindingsInterpreter::AppendModelToObject( CXBindings& grammar , CXBindingsObject& object , const std::string& modelname )
{
	CXBindingsModel* model = grammar.FindModel(modelname);
	if( model == NULL )
		CXB_THROW( "Error cannot find object model in model list : " + modelname ) ;

	CXBindingsArrayGrammarInherits& models = model->GetModels();
	for( unsigned int i = 0; i < models.size() ; ++i )
	{
		//wxLogMessage( "\t\t\t Appending model : ") + models[i].GetName() + " properties to object ") + object.GetName(   ;
		AppendModelToObject( grammar , object , models[i].GetName() );
	}

	CXBindingsArrayGrammarChild&			m_childs = model->GetChilds();
	CXBindingsArrayGrammarProperty&			m_properties = model->GetProperties();
	CXBindingsArrayGrammarRule&			m_rules = model->GetRules();
	CXBindingsArrayGrammarForEachChild& 		m_childRules = model->GetChildRules();
	CXBindingsArrayGrammarCategory&		 	m_categories = model->GetCategories();
	CXBindingsArrayGrammarChildContainer& 		m_childContainers = model->GetChildContainers();

	CXBindingsArrayGrammarChild&			o_childs = object.GetChilds();
	CXBindingsArrayGrammarProperty&			o_properties = object.GetProperties();
	CXBindingsArrayGrammarRule&			o_rules = object.GetRules();
	CXBindingsArrayGrammarForEachChild& 		o_childRules = object.GetChildRules();
	CXBindingsArrayGrammarCategory& 		o_categories = object.GetCategories();
	CXBindingsArrayGrammarChildContainer& 		o_childContainers = object.GetChildContainers();

	//wxLogMessage( "\t\t\t Appending model childs : ") + std::string::Format("%d"),m_childs.size()) + wxT(" to object ") + object.GetName(   ;
	for( unsigned int i = 0; i < m_childs.size() ; ++i )
		o_childs.push_back(m_childs[i]);

	//wxLogMessage( "\t\t\t Appending model properties : ") + std::string::Format("%d"),m_properties.size()) + wxT(" to object ") + object.GetName(   ;
	for( unsigned int i = 0; i < m_properties.size() ; ++i )
		o_properties.push_back(m_properties[i]);

	//wxLogMessage( "\t\t\t Appending model rules : ") + std::string::Format("%d"),m_rules.size()) + wxT(" to object ") + object.GetName(   ;
	for( unsigned int i = 0; i < m_rules.size() ; ++i )
		o_rules.push_back(m_rules[i]);

	//wxLogMessage( "\t\t\t Appending model childRules : ") + std::string::Format("%d"),m_childRules.size()) + wxT(" to object ") + object.GetName(   ;
	for( unsigned int i = 0; i < m_childRules.size() ; ++i )
		o_childRules.push_back(m_childRules[i]);

	//wxLogMessage( "\t\t\t Appending model categories : ") + std::string::Format("%d"),m_categories.size()) + wxT(" to object ") + object.GetName(   ;
	for( unsigned int i = 0; i < m_categories.size() ; ++i )
		o_categories.push_back(m_categories[i]);

	//wxLogMessage( "\t\t\t Appending model childContainers : ") + std::string::Format("%d"),m_childContainers.size()) + wxT(" to object ") + object.GetName(   ;
	for( unsigned int i = 0; i < m_childContainers.size() ; ++i )
		o_childContainers.push_back(m_childContainers[i]);
}

/* property extraction method */
CXBindingsStringStringMap CXBindingsInterpreter::DoExtractPropertiesFrom( xmlNode* node , CXBindingsArrayGrammarProperty& properties , CXBindings& grammar )
{
	//wxLogMessage( "\t\t Extracting properties")  ;
	CXBindingsStringStringMap pValues;

	for( unsigned int i = 0; i < properties.size() ; ++i )
	{
		//wxLogMessage( "\t\t\t Extracting property : ") + properties[i].GetName()  ;

		CXBindingsArrayGrammarCondition& conditions = properties[i].GetConditions();
		std::string conditionMatch;

		bool matchfound = false;
		std::string match;

		for( unsigned int j = 0; j < conditions.size() ; ++j )
		{
			conditions[j].Compile();

			if( conditions[j].Matches(node,grammar) )
			{
				matchfound = true;
				match = conditions[j].GetMatch( node,grammar );

				//wxLogMessage( "\t\t\t Found condition match  : ") + match  ;
				break;
			}
		}

		std::string value = GetXmlAttr( node , properties[i].GetName() );
		//wxLogMessage( "\t\t\t Default value is  : ") + properties[i].GetDefaultValue()  ;
		
		/* One of the most important thing here is to fill the "missing" properties, missing in the
		 * the sence of grammar usage required fields. If not the system will generate some exceptions
		 * and notify that some properties are missing.
		 * It shall be the only place to do it !
		 */
		if( ((value.empty() && properties[i].GetRequired() ) &&
					(value.empty() && properties[i].GetDefaultValue().empty() ) ) &&
				match.empty()
		  )
			CXB_THROW( "Error cannot find required property : " + properties[i].GetName()  );
		else if( value.empty() && match.empty() )
		{
			xmlNewProp( node , cxxU2C( properties[i].GetName() ) , cxxU2C( properties[i].GetDefaultValue() ) ); 
			value = properties[i].GetDefaultValue();
		}
		else if( value.empty() && !match.empty() )
		{
			xmlNewProp( node , cxxU2C( properties[i].GetName() ) , cxxU2C( match ) );
			value = match;
		}

		//wxLogMessage( "\t\t\t Found property : ") + properties[i].GetName() + " , it's value is : "  + value  ;
		pValues[properties[i].GetName()] = value;
	}

	//wxLogMessage( "\t\t End Extracting properties")  ;
	return pValues;
}

CXBindingsArrayGrammarCategoryInfo CXBindingsInterpreter::DoExtractCategoriesFrom( xmlNode* node , CXBindingsArrayGrammarCategory& categories , CXBindings& grammar )
{
	CXBindingsArrayGrammarCategoryInfo result;

	xmlNode* child = node->children;
	//wxLogMessage( "\t\t Checking if node has matching categories")  ;

	while( child != NULL )
	{
		std::string cName = cxC2U( (char*) child->name );

		// Check if a container has the corresponding child;
		int category_index = -1;

		for( unsigned int i = 0; i < categories.size() ; ++i )
			if( categories[i].GetName() == cName ) 
			{
				category_index = i;
				break;
			}

		// If the container has been found treat it as it shall be
		if( category_index != -1 )
		{
			CXBindingsCategory& category = categories[category_index];
			//wxLogMessage( "\t\t\t Found child category : ") + category.GetName()  ;

			/* Once properties have been extracted from the child category, we have to extract 
			 * childrens if they exists from the nodes child category !
			 * childrens can have the property "minOccurs" and "maxOccurs" which will means
			 * that they are required or not !
			 * before generating each child category information accordingly the the rule it must
			 * contains all childrens must be extracted.
			 */
			CXBindingsArrayGrammarChild& childs = category.GetChilds();
			CXBindingsArrayGrammarChildInfo childinfos = DoExtractChildrenInformationFrom( child , childs , grammar );

			/* Once we've found all properties, we have to check if the node contains
			 * child containers. Which means that the node can contain childrens and
			 * a specific action has to be done accordinly to each childs */
			CXBindingsArrayGrammarChildContainer& containers = category.GetChildContainers();
			CXBindingsArrayGrammarChildContainerInfo childContainersInfo = DoExtractChildContainersFrom( child , containers , grammar );	
			/* Finally store the result in the returned array */
			CXBindingsCategoryInfo categoryInfo;
			categoryInfo.childs = childinfos;
			categoryInfo.category = category;
			categoryInfo.containers = childContainersInfo;

			result.push_back( categoryInfo );
		}

		child = child->next;
	}
	//wxLogMessage( "\t\t End Checking if node has matching child containers")  ;	
	return result;
}

CXBindingsArrayGrammarChildContainerInfo CXBindingsInterpreter::DoExtractChildContainersFrom( xmlNode* node , CXBindingsArrayGrammarChildContainer& containers , CXBindings& grammar )
{
	CXBindingsArrayGrammarChildContainerInfo result;

	xmlNode* child = node->children;
	//wxLogMessage( "\t\t Checking if node has matching child containers")  ;

	while( child != NULL )
	{
		std::string cName = cxC2U( (char*) child->name );

		// Check if a container has the corresponding child;
		int container_index = -1;

		for( unsigned int i = 0; i < containers.size() ; ++i )
			if( containers[i].GetName() == cName ) 
			{
				container_index = i;
				break;
			}

		// If the container has been found treat it as it shall be
		if( container_index != -1 )
		{
			CXBindingsChildContainer& container = containers[container_index];
			//wxLogMessage( "\t\t\t Found child container : ") + container.GetName()  ;

			//wxLogMessage( "\t\t\t Extracting properties for child container : ") + container.GetName()  ;
			CXBindingsArrayGrammarProperty&	properties = container.GetProperties();
			CXBindingsArrayGrammarRule& rules = container.GetRules();
			CXBindingsArrayGrammarForEachChild& childRules = container.GetChildRules();

			CXBindingsStringStringMap pValues = DoExtractPropertiesFrom( child , properties , grammar );
			CXBindingsArrayGrammarRuleInfo rInfos = DoExtractRuleInformationFrom( child , rules , grammar );

			/* Once properties have been extracted from the child container, we have to extract 
			 * childrens if they exists from the nodes child container !
			 * childrens can have the property "minOccurs" and "maxOccurs" which will means
			 * that they are required or not !
			 * before generating each child container information accordingly the the rule it must
			 * contains all childrens must be extracted.
			 */
			CXBindingsArrayGrammarChild& childs = container.GetChilds();
			CXBindingsArrayGrammarChildInfo childinfos = DoExtractChildrenInformationFrom( child , childs , grammar );
			CXBindingsArrayGrammarForEachChildInfo cInfos = DoExtractChildRuleInformationFrom( child , childRules , grammar );

			/* Once we've found all containers, we have to check if the node contains
			 * categories. Which means that the node can contain childrens */
			CXBindingsArrayGrammarCategory& categories = container.GetCategories();
			CXBindingsArrayGrammarCategoryInfo categoriesInfo = DoExtractCategoriesFrom( child , categories , grammar );	
			/* Finally store the result in the returned array */
			CXBindingsChildContainerInfo childContainerInfo;
			childContainerInfo.properties = pValues;
			childContainerInfo.childs = childinfos;
			childContainerInfo.container = container;
			childContainerInfo.rules = rInfos;
			childContainerInfo.childRules = cInfos;
			childContainerInfo.categories = categoriesInfo;

			result.push_back( childContainerInfo );
		}

		child = child->next;
	}
	//wxLogMessage( "\t\t End Checking if node has matching child containers")  ;	
	return result;
}

CXBindingsArrayGrammarChildInfo CXBindingsInterpreter::DoExtractChildrenInformationFrom( xmlNode* node , CXBindingsArrayGrammarChild& childs , CXBindings& grammar )
{
	CXBindingsArrayGrammarChildInfo infos;

	for( unsigned int i = 0; i < childs.size() ; ++i )
	{
		CXBindingsArrayGrammarProperty& cprops = childs[i].GetProperties();
		CXBindingsArrayGrammarRule& crules = childs[i].GetRules();

		std::string name = childs[i].GetName();

		/* Now for each children of the child container we have to parse
		 * children nodes if exists and get their properties in order 
		 * to proceed to the generation of the child container type 
		 * accordingly to it's containing rule.
		 */
		xmlNode* child = node->children;
		while( child != NULL )
		{
			std::string sName = cxC2U( (char*) child->name );
			if( sName == name )  {
				CXBindingsStringStringMap cpValues = DoExtractPropertiesFrom( child , cprops , grammar );
				CXBindingsArrayGrammarRuleInfo rInfos = DoExtractRuleInformationFrom( child , crules , grammar );

				/* Now just store the child infos into the returned array */
				CXBindingsChildInfo info;
				info.child = childs[i];
				info.properties = cpValues;
				info.rules = rInfos;

				infos.push_back( info );	
			}
			child = child->next;					
		}
	}
	return infos;
}

CXBindingsArrayGrammarRuleInfo CXBindingsInterpreter::DoExtractRuleInformationFrom( xmlNode* node , CXBindingsArrayGrammarRule& rules , CXBindings& grammar )
{
	CXBindingsArrayGrammarRuleInfo infos;

	for( unsigned int i = 0; i < rules.size() ; ++i ) {

		CXBindingsName& rName = rules[i].GetContentName();
		CXBindingsType& rType = rules[i].GetContentType();
		CXBindingsVariable& rVariable = rules[i].GetContentVariable();

		CXBindingsNameInfo iName;
		iName.name = rName;
		iName.stemplate = rName.GetTemplateMatch( node , grammar );
		iName.content = rName.GetContentMatch( node , grammar );

		CXBindingsTypeInfo iType;
		iType.type = rType;
		iType.stemplate = rType.GetTemplateMatch( node , grammar );
		iType.content = rType.GetContentMatch( node , grammar );
		
		CXBindingsVariableInfo iVariable;
		iVariable.variable = rVariable;
		iVariable.stemplate = rVariable.GetTemplateMatch( node , grammar );
		iVariable.content = rVariable.GetContentMatch( node , grammar );

		CXBindingsRuleInfo info;
		info.rule = rules[i];
		info.name = iName;
		info.type = iType;
		info.variable = iVariable;
		info.make = rules[i].GetMakeMatch( node , grammar );

		infos.push_back( info );

	}

	return infos;
}

CXBindingsArrayGrammarForEachChildInfo CXBindingsInterpreter::DoExtractChildRuleInformationFrom( xmlNode* node , CXBindingsArrayGrammarForEachChild& rules , CXBindings& grammar )
{
	CXBindingsArrayGrammarForEachChildInfo infos;

	for( unsigned int i = 0; i < rules.size() ; ++i ) {

		CXBindingsArrayGrammarRule& crules = rules[i].GetRules();
		xmlNode* child = node->children;
		std::string childOf = rules[i].GetOf();
		
		CXBindingsForEachChildInfo info;
		
		while(child != NULL) {
			std::string cName = cxC2U( (char*) child->name );

			if( cName == childOf )  {
				CXBindingsArrayGrammarRuleInfo rInfo = DoExtractRuleInformationFrom( child , crules , grammar );
				std::string name = GetXmlAttr( child , "name")  ;

				if( name.empty() )
					CXB_THROW( "Error child as no name property")  ;

				info.rules[name] = rInfo;
			}

			child = child->next;
		}
		

		info.foreach = rules[i];
		infos.push_back( info );
	}

	return infos;
}
