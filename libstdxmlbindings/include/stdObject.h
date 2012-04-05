
/**
*	@file stdObject.h

*	@author Nicolas Macherey (nm@graymat.fr)
*	@date 31-Janv-2009
*	@version 0.0.1
*
*/

#ifndef STD_OBJECT_H
#define STD_OBJECT_H

#include <iostream>
#include <ctype.h>
#include <limits>
#include <vector>

/*******************************************************************************
   DEFINES / TYPDEFS / ENUMS
*******************************************************************************/
#ifndef MIN
/*! Computes the minimum of \a a and \a b */
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
/*! Computes the maximum of \a a and \a b */
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#endif
/*! Defines UNDEF */
const double UNDEF = std::numeric_limits<double>::quiet_NaN();

/* class definitions for stdObject */
class stdClassInfo;
class stdHashTable;
class stdObjectRefData;
class stdObject;

typedef stdObject *(*stdObjectConstructorFn)(void);

/*******************************************************************************
   CLASS stdClassInfo DECLARATION
*******************************************************************************/
/**
*	@class stdClassInfo
*	@brief This class is the basic class for getting information about an element in a Std data file
*	@author Nicolas Macherey (nm@graymat.fr)
*	@date 08-Feb-2009
*	@version 0.0.1
*/

class stdClassInfo {
public:
	stdClassInfo( const char *className,
	              const stdClassInfo *baseInfo1,
	              const stdClassInfo *baseInfo2,
	              const stdClassInfo *baseInfo3,
	              int size,
	              stdObjectConstructorFn ctor )
		: m_className(className)
		, m_objectSize(size)
		, m_objectConstructor(ctor)
		, m_baseInfo1(baseInfo1)
		, m_baseInfo2(baseInfo2)
		, m_baseInfo3(baseInfo3)
		, m_next(sm_first) {
		sm_first = this;
		Register();
	}

	~stdClassInfo();

	stdObject *CreateObject() const {
		return m_objectConstructor ? (*m_objectConstructor)() : 0;
	}

	bool IsDynamic() const {
		return (NULL != m_objectConstructor);
	}

	const char *GetClassName() const {
		return m_className;
	}

	const char* GetBaseClassName1() const {
		return m_baseInfo1 ? m_baseInfo1->GetClassName() : NULL;
	}

	const char *GetBaseClassName2() const {
		return m_baseInfo2 ? m_baseInfo2->GetClassName() : NULL;
	}

	const char *GetBaseClassName3() const {
		return m_baseInfo3 ? m_baseInfo3->GetClassName() : NULL;
	}

	const stdClassInfo  *GetBaseClass1() const {
		return m_baseInfo1;
	}

	const stdClassInfo  *GetBaseClass2() const {
		return m_baseInfo2;
	}

	const stdClassInfo  *GetBaseClass3() const {
		return m_baseInfo3;
	}

	int                 GetSize() const {
		return m_objectSize;
	}

	stdObjectConstructorFn      GetConstructor() const {
		return m_objectConstructor;
	}

	static const stdClassInfo  *GetFirst() {
		return sm_first;
	}

	const stdClassInfo *GetNext() const {
		return m_next;
	}

	static stdClassInfo *FindClass(const char* className);

	bool IsKindOf(const stdClassInfo *info) const {
		return info != 0 &&
		       ( info == this ||
		         ( m_baseInfo1 && m_baseInfo1->IsKindOf(info) ) ||
		         ( m_baseInfo2 && m_baseInfo2->IsKindOf(info) ) ||
		         ( m_baseInfo3 && m_baseInfo3->IsKindOf(info) ) );
	}


public :

	const char *m_className;
	int m_objectSize;
	stdObjectConstructorFn m_objectConstructor;

	// Pointers to base stdClassInfo : set in InitializeClasses
	const stdClassInfo *m_baseInfo1;
	const stdClassInfo *m_baseInfo2;
	const stdClassInfo *m_baseInfo3;

	// class info object live in a linked list:
	// pointers to its head and the next element in it

	static stdClassInfo *sm_first;
	stdClassInfo *m_next;

private:
	// InitializeClasses() helper
	static stdClassInfo *GetBaseByName(const char *name);

	static stdHashTable *sm_classTable;

protected:
	// registers the class
	void Register();
	void Unregister();

};

// ----------------------------------------------------------------------------
// Dynamic class macros
// ----------------------------------------------------------------------------
#define DECLARE_NO_COPY_CLASS_STD(classname)        \
	private:                                    \
	classname(const classname&);            \
	classname& operator=(const classname&);

#define DECLARE_NO_ASSIGN_CLASS_STD(classname)      \
	private:                                    \
	classname& operator=(const classname&);

#define DECLARE_ABSTRACT_CLASS_STD(name)                                           \
	public:                                                                    \
	static stdClassInfo ms_classInfo;                                      \
	virtual stdClassInfo *GetClassInfo() const;

#define DECLARE_DYNAMIC_CLASS_STD_NO_ASSIGN_STD(name)                                  \
	DECLARE_NO_ASSIGN_CLASS_STD(name)                                              \
	DECLARE_DYNAMIC_CLASS_STD(name)

#define DECLARE_DYNAMIC_CLASS_STD_NO_COPY_STD(name)                                    \
	DECLARE_NO_COPY_CLASS_STD(name)                                                \
	DECLARE_DYNAMIC_CLASS_STD(name)

#define DECLARE_DYNAMIC_CLASS_STD(name)                                            \
	DECLARE_ABSTRACT_CLASS_STD(name)                                               \
	static stdObject* StdCreateObject();

#define DECLARE_CLASS_STD(name) DECLARE_DYNAMIC_CLASS_STD(name)

// common part of the macros below
#define IMPLEMENT_CLASS_STD_COMMON_STD(name, basename, baseclsinfo2, baseclsinfo3 , func) \
	stdClassInfo name::ms_classInfo( #name,                                    \
	                                 &basename::ms_classInfo,                                           \
	                                 baseclsinfo2,                                                      \
	                                 baseclsinfo3,                                                      \
	                                 (int) sizeof(name),                                                \
	                                 (stdObjectConstructorFn) func);                                    \
	\
	stdClassInfo *name::GetClassInfo() const                                   \
	{ return &name::ms_classInfo; }

#define IMPLEMENT_CLASS_STD_COMMON_STD1(name, basename, func)                          \
	IMPLEMENT_CLASS_STD_COMMON_STD(name, basename, NULL, NULL, func)

#define IMPLEMENT_CLASS_STD_COMMON_STD2(name, basename1, basename2, func)              \
	IMPLEMENT_CLASS_STD_COMMON_STD(name, basename1, &basename2::ms_classInfo, NULL, func)

#define IMPLEMENT_CLASS_STD_COMMON_STD3(name, basename1, basename2,basename3, func)    \
	IMPLEMENT_CLASS_STD_COMMON_STD(name, basename1, &basename2::ms_classInfo, &basename3::ms_classInfo, func)

// -----------------------------------
// for template classes
// -----------------------------------
#define DECLARE_ABSTRACT_TEMPLATE_CLASS_STD(EXPORT_MACRO, name)                    \
	public:                                                                    \
	EXPORT_MACRO static stdClassInfo ms_classInfo;                                      \
	EXPORT_MACRO virtual stdClassInfo *GetClassInfo() const;

#define DECLARE_DYNAMIC_TEMPLATE_CLASS_STD(EXPORT_MACRO, name)                     \
	DECLARE_ABSTRACT_TEMPLATE_CLASS_STD(EXPORT_MACRO, name)                         \
	EXPORT_MACRO static stdObject* StdCreateObject();

#define DECLARE_TEMPLATE_CLASS_STD(EXPORT_MACRO, name )                            \
	DECLARE_DYNAMIC_TEMPLATE_CLASS_STD(EXPORT_MACRO, name )

#define IMPLEMENT_TEMPLATE_CLASS_STD( name , basename )                            \
	template<> stdObject* name::StdCreateObject()                      \
	{ return new name; }                                               \
	template<> stdClassInfo name::ms_classInfo( #name,                 \
	        &basename::ms_classInfo,                                           \
	        NULL,                                                              \
	        NULL,                                                              \
	        (int) sizeof(name),                                                \
	        (stdObjectConstructorFn) name::StdCreateObject );                  \
	\
	template<> stdClassInfo *name::GetClassInfo() const                \
	{ return &name::ms_classInfo; }

#define IMPLEMENT_TEMPLATE_CLASS_STD2( name , basename1, basename2)                \
	template<> name::stdObject* name::StdCreateObject()                \
	{ return new name; }                                               \
	template<> stdClassInfo name::ms_classInfo( #name,                 \
	        &basename1::ms_classInfo,                                          \
	        &basename2::ms_classInfo,                                          \
	        NULL,                                                              \
	        (int) sizeof(name),                                                \
	        (stdObjectConstructorFn) name::StdCreateObject );                  \
	\
	template<> stdClassInfo *name::GetClassInfo() const                \
	{ return &name::ms_classInfo; }

#define IMPLEMENT_TEMPLATE_CLASS_STD3( name , basename1, basename2, basename3)     \
	template<> stdObject* name::StdCreateObject()                      \
	{ return new name; } \
	template<> stdClassInfo name::ms_classInfo( #name,                 \
	        &basename1::ms_classInfo,                                          \
	        &basename2::ms_classInfo,                                          \
	        &basename3::ms_classInfo,                                          \
	        (int) sizeof(name),                                                \
	        (stdObjectConstructorFn) name::StdCreateObject );                  \
	\
	template<> stdClassInfo *name::GetClassInfo() const                \
	{ return &name::ms_classInfo; }

// -----------------------------------
// for concrete classes
// -----------------------------------

// Single inheritance with one base class
#define IMPLEMENT_DYNAMIC_CLASS_STD(name, basename)                                \
	IMPLEMENT_CLASS_STD_COMMON_STD1(name, basename, name::StdCreateObject)             \
	stdObject* name::StdCreateObject()                                         \
	{ return new name; }

// Multiple inheritance with two base classes
#define IMPLEMENT_DYNAMIC_CLASS_STD2(name, basename1, basename2)                   \
	IMPLEMENT_CLASS_STD_COMMON_STD2(name, basename1, basename2,                        \
	                                name::StdCreateObject)                           \
	stdObject* name::StdCreateObject()                                         \
	{ return new name; }

#define IMPLEMENT_DYNAMIC_CLASS_STD3(name, basename1, basename2, basename3)        \
	IMPLEMENT_CLASS_STD_COMMON_STD3(name, basename1, basename2, basename2,             \
	                                name::StdCreateObject)                           \
	stdObject* name::StdCreateObject()                                         \
	{ return new name; }

// -----------------------------------
// for abstract classes
// -----------------------------------

// Single inheritance with one base class

#define IMPLEMENT_ABSTRACT_CLASS_STD(name, basename)                               \
	IMPLEMENT_CLASS_STD_COMMON_STD1(name, basename, NULL)

// Multiple inheritance with two base classes

#define IMPLEMENT_ABSTRACT_CLASS_STD2(name, basename1, basename2)                  \
	IMPLEMENT_CLASS_STD_COMMON_STD2(name, basename1, basename2, NULL)

#define IMPLEMENT_ABSTRACT_CLASS_STD3(name, basename1, basename2, basename3)       \
	IMPLEMENT_CLASS_STD_COMMON_STD3(name, basename1, basename2, basename3, NULL)

#define IMPLEMENT_CLASS_STD IMPLEMENT_ABSTRACT_CLASS_STD
#define IMPLEMENT_CLASS_STD2 IMPLEMENT_ABSTRACT_CLASS_STD2
#define IMPLEMENT_CLASS_STD3 IMPLEMENT_ABSTRACT_CLASS_STD3


#define STDCLASSINFO(name) (&name::ms_classInfo)

#define IS_OF_KIND(obj, className) obj->IsKindOf(&className::ms_classInfo)

/*******************************************************************************
   CLASS stdObjectRefData DECLARATION
*******************************************************************************/
/*!
@class stdObjectRefData
@brief ref counted data meant to be stored in stdObject
*/

class stdObjectRefData {
	friend class stdObject;

public:
	stdObjectRefData() : m_count(1) { }
	virtual ~stdObjectRefData() { }

	int GetRefCount() const {
		return m_count;
	}

private:
	int m_count;
};

/*******************************************************************************
   CLASS stdObject DECLARATION
*******************************************************************************/
/**
*	@class stdObject
*	@brief This class is the basic class for reading/writing Std specific object from/to a file
*	This class is used as a base class for defining STD Data read/wittrent from/to a file.
*	Any returned information shall be based on this object. And any StdXmlHandler's must
*	handle this type of object.
*
*	@author Nicolas Macherey (nm@graymat.fr)
*	@date 08-Feb-2009
*	@version 0.0.1
*/
class stdObject {
	DECLARE_ABSTRACT_CLASS_STD(stdObject)

public:
	stdObject() {
		m_refData = NULL;
	}
	virtual ~stdObject() {
		UnRef();
	}

	stdObject(const stdObject& other) {
		m_refData = other.m_refData;
		if (m_refData)
			m_refData->m_count++;
	}

	stdObject& operator=(const stdObject& other) {
		if ( this != &other ) {
			Ref(other);
		}
		return *this;
	}

	bool IsKindOf(stdClassInfo *info) const;

	// get/set
	stdObjectRefData *GetRefData() const {
		return m_refData;
	}
	void SetRefData(stdObjectRefData *data) {
		m_refData = data;
	}

	// make a 'clone' of the object
	void Ref(const stdObject& clone);

	// destroy a reference
	void UnRef();

	// Make sure this object has only one reference
	void UnShare() {
		AllocExclusive();
	}

	// check if this object references the same data as the other one
	bool IsSameAs(const stdObject& o) const {
		return m_refData == o.m_refData;
	}

protected:
	// ensure that our data is not shared with anybody else: if we have no
	// data, it is created using CreateRefData() below, if we have shared data
	// it is copied using CloneRefData(), otherwise nothing is done
	void AllocExclusive();

	// both methods must be implemented if AllocExclusive() is used, not pure
	// virtual only because of the backwards compatibility reasons

	// create a new m_refData
	virtual stdObjectRefData *CreateRefData() const;

	// create a new m_refData initialized with the given one
	virtual stdObjectRefData *CloneRefData(const stdObjectRefData *data) const;

	stdObjectRefData *m_refData;
};


/** @class stdEnumeration
  * @brief Main class from which any enumeration class shall derive from
  * @author Nicolas Macherey (nm@graymat.fr) http://www.rheia-framework.com
  * @date 2012-Apr-05
  */
class stdEnumeration : public stdObject
{
	DECLARE_ABSTRACT_CLASS_STD( stdEnumeration )
	
public :
	
	/** Get values all childrens must overload this method */
	virtual const std::string* GetValues() const = 0;
	
	/** Get enumerated as a wxArrayInt */
	virtual std::vector<unsigned int> GetFlags() const = 0;
	
	/** Get the total number of enumerated values */
	virtual unsigned int GetCount() const = 0;
	
	/** Get the value from the flag */
	virtual const std::string& GetFromFlag( unsigned int value ) const = 0;
	
	/** get the falg from value */
	virtual unsigned int GetUIntFromValue( const std::string& value ) const = 0;
	
	/** get the flag */
	virtual unsigned int GetUIntFlag() const = 0;
	
	/** get the value */
	virtual const std::string& GetValue() const = 0;
	
	/** Set the value */
	virtual void SetFlag( unsigned int value ) = 0;
	
	/** Set the value */
	virtual void SetValue( const std::string& value ) = 0;

};
	

#endif
		