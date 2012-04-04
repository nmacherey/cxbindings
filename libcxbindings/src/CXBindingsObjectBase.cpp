/**
*	@file CXBindingsObjectBase.cpp
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 31-Janv-2009
*	@version 0.0.1
*/

/*******************************************************************************
   INCLUDES
*******************************************************************************/
#include <string.h>

#include "CXBindingsHashTable.h"
#include "CXBindingsObjectBase.h"


/*******************************************************************************
   CLASS CXBindingsClassInfo DEFINITION
*******************************************************************************/
CXBindingsClassInfo CXBindingsObjectBase::ms_classInfo( "CXBindingsObjectBase", 0, 0,0,
                                      (int) sizeof(CXBindingsObjectBase),
                                      (CXBindingsObjectBaseConstructorFn) 0 );

// restore optimizations
#if defined __VISUALC__ && __VISUALC__ >= 1300
#pragma optimize("", on)
#endif

CXBindingsClassInfo* CXBindingsClassInfo::sm_first = NULL;
CXBindingsHashTable* CXBindingsClassInfo::sm_classTable = NULL;

CXBindingsClassInfo::~CXBindingsClassInfo() {
	// remove this object from the linked list of all class infos: if we don't
	// do it, loading/unloading a DLL containing static wxClassInfo objects is
	// not going to work
	if ( this == sm_first ) {
		sm_first = m_next;
	} else {
		CXBindingsClassInfo *info = sm_first;
		while (info) {
			if ( info->m_next == this ) {
				info->m_next = m_next;
				break;
			}

			info = info->m_next;
		}
	}
	Unregister();
}

CXBindingsClassInfo *CXBindingsClassInfo::FindClass(const char* className) {
	if ( sm_classTable ) {
		return (CXBindingsClassInfo *)CXBindingsClassInfo::sm_classTable->Get(className);
	} else {
		for ( CXBindingsClassInfo *info = sm_first; info ; info = info->m_next ) {
			if ( strcmp(info->GetClassName(), className) == 0 )
				return info;
		}

		return NULL;
	}
}

// This function wasn't written to be reentrant but there is a possiblity of
// reentrance if something it does causes a shared lib to load and register
// classes. On Solaris this happens when the wxHashTable is newed, so the first
// part of the function has been modified to handle it, and a wxASSERT checks
// against reentrance in the remainder of the function.

void CXBindingsClassInfo::Register() {
	if ( !sm_classTable ) {
		CXBindingsHashTable *classTable = new CXBindingsHashTable(ktKEY_STRING);

		// check for reentrance
		if ( sm_classTable )
			delete classTable;
		else
			sm_classTable = classTable;
	}

	sm_classTable->Put(m_className, (CXBindingsObjectBase *)this);
}

void CXBindingsClassInfo::Unregister() {
	if ( sm_classTable ) {
		sm_classTable->Delete(m_className);
		if ( sm_classTable->GetCount() == 0 ) {
			delete sm_classTable;
			sm_classTable = NULL;
		}
	}
}

/*******************************************************************************
   CLASS CXBindingsObjectBase DEFINITION
*******************************************************************************/
void CXBindingsObjectBase::Ref(const CXBindingsObjectBase& clone) {
	// nothing to be done
	if (m_refData == clone.m_refData)
		return;

	// delete reference to old data
	UnRef();

	// reference new data
	if ( clone.m_refData ) {
		m_refData = clone.m_refData;
		++(m_refData->m_count);
	}
}

void CXBindingsObjectBase::UnRef() {
	if ( m_refData ) {
		if ( --m_refData->m_count == 0 )
			delete m_refData;
		m_refData = NULL;
	}
}

void CXBindingsObjectBase::AllocExclusive() {
	if ( !m_refData ) {
		m_refData = CreateRefData();
	} else if ( m_refData->GetRefCount() > 1 ) {
		// note that ref is not going to be destroyed in this case
		const CXBindingsObjectBaseRefData* ref = m_refData;
		UnRef();

		// ... so we can still access it
		m_refData = CloneRefData(ref);
	}
	//else: ref count is 1, we are exclusive owners of m_refData anyhow
}

/*******************************************************************************
   CLASS CXBindingsObjectBaseRefData DEFINITION
*******************************************************************************/
CXBindingsObjectBaseRefData *CXBindingsObjectBase::CreateRefData() const {
	return NULL;
}

CXBindingsObjectBaseRefData *
CXBindingsObjectBase::CloneRefData(const CXBindingsObjectBaseRefData * data) const {
	// if you use AllocExclusive() you must override this method
	return NULL;
}

CXBindingsClassInfo *CXBindingsObjectBase::GetClassInfo() const {
	return &CXBindingsObjectBase::ms_classInfo;
}

bool CXBindingsObjectBase::IsKindOf(CXBindingsClassInfo *info) const {
	CXBindingsClassInfo *thisInfo = GetClassInfo();
	return (thisInfo) ? thisInfo->IsKindOf(info) : false ;
}
