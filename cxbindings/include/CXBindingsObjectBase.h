/**
*	@file CXBindingsObjectBase.h

*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 31-Janv-2009
*	@version 0.0.1
*
*/

#ifndef CXBINDINGS_OBJECT_H
#define CXBINDINGS_OBJECT_H

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

/* class definitions for CXBindingsObjectBase */
class CXBindingsClassInfo;
class CXBindingsHashTable;
class CXBindingsObjectBaseRefData;
class CXBindingsObjectBase;

typedef CXBindingsObjectBase *(*CXBindingsObjectBaseConstructorFn)(void);

/*******************************************************************************
   CLASS CXBindingsClassInfo DECLARATION
*******************************************************************************/
/**
*	@class CXBindingsClassInfo
*	@brief This class is the basic class for getting information about an element in a Std data file
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 08-Feb-2009
*	@version 0.0.1
*/

class CXBindingsClassInfo {
public:
	CXBindingsClassInfo( const char *className,
	              const CXBindingsClassInfo *baseInfo1,
	              const CXBindingsClassInfo *baseInfo2,
	              const CXBindingsClassInfo *baseInfo3,
	              int size,
	              CXBindingsObjectBaseConstructorFn ctor )
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

	~CXBindingsClassInfo();

	CXBindingsObjectBase *CreateObject() const {
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

	const CXBindingsClassInfo  *GetBaseClass1() const {
		return m_baseInfo1;
	}

	const CXBindingsClassInfo  *GetBaseClass2() const {
		return m_baseInfo2;
	}

	const CXBindingsClassInfo  *GetBaseClass3() const {
		return m_baseInfo3;
	}

	int                 GetSize() const {
		return m_objectSize;
	}

	CXBindingsObjectBaseConstructorFn      GetConstructor() const {
		return m_objectConstructor;
	}

	static const CXBindingsClassInfo  *GetFirst() {
		return sm_first;
	}

	const CXBindingsClassInfo *GetNext() const {
		return m_next;
	}

	static CXBindingsClassInfo *FindClass(const char* className);

	bool IsKindOf(const CXBindingsClassInfo *info) const {
		return info != 0 &&
		       ( info == this ||
		         ( m_baseInfo1 && m_baseInfo1->IsKindOf(info) ) ||
		         ( m_baseInfo2 && m_baseInfo2->IsKindOf(info) ) ||
		         ( m_baseInfo3 && m_baseInfo3->IsKindOf(info) ) );
	}


public :

	const char *m_className;
	int m_objectSize;
	CXBindingsObjectBaseConstructorFn m_objectConstructor;

	// Pointers to base CXBindingsClassInfo : set in InitializeClasses
	const CXBindingsClassInfo *m_baseInfo1;
	const CXBindingsClassInfo *m_baseInfo2;
	const CXBindingsClassInfo *m_baseInfo3;

	// class info object live in a linked list:
	// pointers to its head and the next element in it

	static CXBindingsClassInfo *sm_first;
	CXBindingsClassInfo *m_next;

private:
	// InitializeClasses() helper
	static CXBindingsClassInfo *GetBaseByName(const char *name);

	static CXBindingsHashTable *sm_classTable;

protected:
	// registers the class
	void Register();
	void Unregister();

};

// ----------------------------------------------------------------------------
// Dynamic class macros
// ----------------------------------------------------------------------------
#define DECLARE_NO_COPY_CLASS_CXBINDINGS(classname)        \
	private:                                    \
	classname(const classname&);            \
	classname& operator=(const classname&);

#define DECLARE_NO_ASSIGN_CLASS_CXBINDINGS(classname)      \
	private:                                    \
	classname& operator=(const classname&);

#define DECLARE_ABSTRACT_CLASS_CXBINDINGS(name)                                           \
	public:                                                                    \
	static CXBindingsClassInfo ms_classInfo;                                      \
	virtual CXBindingsClassInfo *GetClassInfo() const;

#define DECLARE_DYNAMIC_CLASS_CXBINDINGS_NO_ASSIGN_CXBINDINGS(name)                                  \
	DECLARE_NO_ASSIGN_CLASS_CXBINDINGS(name)                                              \
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(name)

#define DECLARE_DYNAMIC_CLASS_CXBINDINGS_NO_COPY_CXBINDINGS(name)                                    \
	DECLARE_NO_COPY_CLASS_CXBINDINGS(name)                                                \
	DECLARE_DYNAMIC_CLASS_CXBINDINGS(name)

#define DECLARE_DYNAMIC_CLASS_CXBINDINGS(name)                                            \
	DECLARE_ABSTRACT_CLASS_CXBINDINGS(name)                                               \
	static CXBindingsObjectBase* StdCreateObject();

#define DECLARE_CLASS_CXBINDINGS(name) DECLARE_DYNAMIC_CLASS_CXBINDINGS(name)

// common part of the macros below
#define IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS(name, basename, baseclsinfo2, baseclsinfo3 , func) \
	CXBindingsClassInfo name::ms_classInfo( #name,                                    \
	                                 &basename::ms_classInfo,                                           \
	                                 baseclsinfo2,                                                      \
	                                 baseclsinfo3,                                                      \
	                                 (int) sizeof(name),                                                \
	                                 (CXBindingsObjectBaseConstructorFn) func);                                    \
	\
	CXBindingsClassInfo *name::GetClassInfo() const                                   \
	{ return &name::ms_classInfo; }

#define IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS1(name, basename, func)                          \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS(name, basename, NULL, NULL, func)

#define IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS2(name, basename1, basename2, func)              \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS(name, basename1, &basename2::ms_classInfo, NULL, func)

#define IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS3(name, basename1, basename2,basename3, func)    \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS(name, basename1, &basename2::ms_classInfo, &basename3::ms_classInfo, func)

// -----------------------------------
// for template classes
// -----------------------------------
#define DECLARE_ABSTRACT_TEMPLATE_CLASS_CXBINDINGS(EXPORT_MACRO, name)                    \
	public:                                                                    \
	EXPORT_MACRO static CXBindingsClassInfo ms_classInfo;                                      \
	EXPORT_MACRO virtual CXBindingsClassInfo *GetClassInfo() const;

#define DECLARE_DYNAMIC_TEMPLATE_CLASS_CXBINDINGS(EXPORT_MACRO, name)                     \
	DECLARE_ABSTRACT_TEMPLATE_CLASS_CXBINDINGS(EXPORT_MACRO, name)                         \
	EXPORT_MACRO static CXBindingsObjectBase* StdCreateObject();

#define DECLARE_TEMPLATE_CLASS_CXBINDINGS(EXPORT_MACRO, name )                            \
	DECLARE_DYNAMIC_TEMPLATE_CLASS_CXBINDINGS(EXPORT_MACRO, name )

#define IMPLEMENT_TEMPLATE_CLASS_CXBINDINGS( name , basename )                            \
	template<> CXBindingsObjectBase* name::StdCreateObject()                      \
	{ return new name; }                                               \
	template<> CXBindingsClassInfo name::ms_classInfo( #name,                 \
	        &basename::ms_classInfo,                                           \
	        NULL,                                                              \
	        NULL,                                                              \
	        (int) sizeof(name),                                                \
	        (CXBindingsObjectBaseConstructorFn) name::StdCreateObject );                  \
	\
	template<> CXBindingsClassInfo *name::GetClassInfo() const                \
	{ return &name::ms_classInfo; }

#define IMPLEMENT_TEMPLATE_CLASS_CXBINDINGS2( name , basename1, basename2)                \
	template<> name::CXBindingsObjectBase* name::StdCreateObject()                \
	{ return new name; }                                               \
	template<> CXBindingsClassInfo name::ms_classInfo( #name,                 \
	        &basename1::ms_classInfo,                                          \
	        &basename2::ms_classInfo,                                          \
	        NULL,                                                              \
	        (int) sizeof(name),                                                \
	        (CXBindingsObjectBaseConstructorFn) name::StdCreateObject );                  \
	\
	template<> CXBindingsClassInfo *name::GetClassInfo() const                \
	{ return &name::ms_classInfo; }

#define IMPLEMENT_TEMPLATE_CLASS_CXBINDINGS3( name , basename1, basename2, basename3)     \
	template<> CXBindingsObjectBase* name::StdCreateObject()                      \
	{ return new name; } \
	template<> CXBindingsClassInfo name::ms_classInfo( #name,                 \
	        &basename1::ms_classInfo,                                          \
	        &basename2::ms_classInfo,                                          \
	        &basename3::ms_classInfo,                                          \
	        (int) sizeof(name),                                                \
	        (CXBindingsObjectBaseConstructorFn) name::StdCreateObject );                  \
	\
	template<> CXBindingsClassInfo *name::GetClassInfo() const                \
	{ return &name::ms_classInfo; }

// -----------------------------------
// for concrete classes
// -----------------------------------

// Single inheritance with one base class
#define IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS(name, basename)                                \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS1(name, basename, name::StdCreateObject)             \
	CXBindingsObjectBase* name::StdCreateObject()                                         \
	{ return new name; }

// Multiple inheritance with two base classes
#define IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS2(name, basename1, basename2)                   \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS2(name, basename1, basename2,                        \
	                                name::StdCreateObject)                           \
	CXBindingsObjectBase* name::StdCreateObject()                                         \
	{ return new name; }

#define IMPLEMENT_DYNAMIC_CLASS_CXBINDINGS3(name, basename1, basename2, basename3)        \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS3(name, basename1, basename2, basename2,             \
	                                name::StdCreateObject)                           \
	CXBindingsObjectBase* name::StdCreateObject()                                         \
	{ return new name; }

// -----------------------------------
// for abstract classes
// -----------------------------------

// Single inheritance with one base class

#define IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS(name, basename)                               \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS1(name, basename, NULL)

// Multiple inheritance with two base classes

#define IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS2(name, basename1, basename2)                  \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS2(name, basename1, basename2, NULL)

#define IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS3(name, basename1, basename2, basename3)       \
	IMPLEMENT_CLASS_CXBINDINGS_COMMON_CXBINDINGS3(name, basename1, basename2, basename3, NULL)

#define IMPLEMENT_CLASS_CXBINDINGS IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS
#define IMPLEMENT_CLASS_CXBINDINGS2 IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS2
#define IMPLEMENT_CLASS_CXBINDINGS3 IMPLEMENT_ABSTRACT_CLASS_CXBINDINGS3


#define CXBINDINGSCLASSINFO(name) (&name::ms_classInfo)

#define IS_OF_KIND(obj, className) obj->IsKindOf(&className::ms_classInfo)

/*******************************************************************************
   CLASS CXBindingsObjectBaseRefData DECLARATION
*******************************************************************************/
/*!
@class CXBindingsObjectBaseRefData
@brief ref counted data meant to be stored in CXBindingsObjectBase
*/

class CXBindingsObjectBaseRefData {
	friend class CXBindingsObjectBase;

public:
	CXBindingsObjectBaseRefData() : m_count(1) { }
	virtual ~CXBindingsObjectBaseRefData() { }

	int GetRefCount() const {
		return m_count;
	}

private:
	int m_count;
};

/*******************************************************************************
   CLASS CXBindingsObjectBase DECLARATION
*******************************************************************************/
/**
*	@class CXBindingsObjectBase
*	@brief This class is the basic class for reading/writing Std specific object from/to a file
*	This class is used as a base class for defining CXBINDINGS Data read/wittrent from/to a file.
*	Any returned information shall be based on this object. And any StdXmlHandler's must
*	handle this type of object.
*
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 08-Feb-2009
*	@version 0.0.1
*/
class CXBindingsObjectBase {
	DECLARE_ABSTRACT_CLASS_CXBINDINGS(CXBindingsObjectBase)

public:
	CXBindingsObjectBase() {
		m_refData = NULL;
	}
	virtual ~CXBindingsObjectBase() {
		UnRef();
	}

	CXBindingsObjectBase(const CXBindingsObjectBase& other) {
		m_refData = other.m_refData;
		if (m_refData)
			m_refData->m_count++;
	}

	CXBindingsObjectBase& operator=(const CXBindingsObjectBase& other) {
		if ( this != &other ) {
			Ref(other);
		}
		return *this;
	}

	bool IsKindOf(CXBindingsClassInfo *info) const;

	// get/set
	CXBindingsObjectBaseRefData *GetRefData() const {
		return m_refData;
	}
	void SetRefData(CXBindingsObjectBaseRefData *data) {
		m_refData = data;
	}

	// make a 'clone' of the object
	void Ref(const CXBindingsObjectBase& clone);

	// destroy a reference
	void UnRef();

	// Make sure this object has only one reference
	void UnShare() {
		AllocExclusive();
	}

	// check if this object references the same data as the other one
	bool IsSameAs(const CXBindingsObjectBase& o) const {
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
	virtual CXBindingsObjectBaseRefData *CreateRefData() const;

	// create a new m_refData initialized with the given one
	virtual CXBindingsObjectBaseRefData *CloneRefData(const CXBindingsObjectBaseRefData *data) const;

	CXBindingsObjectBaseRefData *m_refData;
};

#endif
