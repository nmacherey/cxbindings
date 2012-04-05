
/**
*	@file stdObject.cpp
*	@author Nicolas Macherey (nm@graymat.fr)
*	@date 31-Janv-2009
*	@version 0.0.1
*/

/*******************************************************************************
   INCLUDES
*******************************************************************************/
#include <string.h>

#include "stdHashTable.h"
#include "stdObject.h"


/*******************************************************************************
   CLASS stdClassInfo DEFINITION
*******************************************************************************/
stdClassInfo stdObject::ms_classInfo( "stdObject", 0, 0,0,
                                      (int) sizeof(stdObject),
                                      (stdObjectConstructorFn) 0 );

// restore optimizations
#if defined __VISUALC__ && __VISUALC__ >= 1300
#pragma optimize("", on)
#endif

stdClassInfo* stdClassInfo::sm_first = NULL;
stdHashTable* stdClassInfo::sm_classTable = NULL;

stdClassInfo::~stdClassInfo() {
	// remove this object from the linked list of all class infos: if we don't
	// do it, loading/unloading a DLL containing static wxClassInfo objects is
	// not going to work
	if ( this == sm_first ) {
		sm_first = m_next;
	} else {
		stdClassInfo *info = sm_first;
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

stdClassInfo *stdClassInfo::FindClass(const char* className) {
	if ( sm_classTable ) {
		return (stdClassInfo *)stdClassInfo::sm_classTable->Get(className);
	} else {
		for ( stdClassInfo *info = sm_first; info ; info = info->m_next ) {
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

void stdClassInfo::Register() {
	if ( !sm_classTable ) {
		stdHashTable *classTable = new stdHashTable(ktKEY_STRING);

		// check for reentrance
		if ( sm_classTable )
			delete classTable;
		else
			sm_classTable = classTable;
	}

	sm_classTable->Put(m_className, (stdObject *)this);
}

void stdClassInfo::Unregister() {
	if ( sm_classTable ) {
		sm_classTable->Delete(m_className);
		if ( sm_classTable->GetCount() == 0 ) {
			delete sm_classTable;
			sm_classTable = NULL;
		}
	}
}

/*******************************************************************************
   CLASS stdObject DEFINITION
*******************************************************************************/
void stdObject::Ref(const stdObject& clone) {
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

void stdObject::UnRef() {
	if ( m_refData ) {
		if ( --m_refData->m_count == 0 )
			delete m_refData;
		m_refData = NULL;
	}
}

void stdObject::AllocExclusive() {
	if ( !m_refData ) {
		m_refData = CreateRefData();
	} else if ( m_refData->GetRefCount() > 1 ) {
		// note that ref is not going to be destroyed in this case
		const stdObjectRefData* ref = m_refData;
		UnRef();

		// ... so we can still access it
		m_refData = CloneRefData(ref);
	}
	//else: ref count is 1, we are exclusive owners of m_refData anyhow
}

/*******************************************************************************
   CLASS stdObjectRefData DEFINITION
*******************************************************************************/
stdObjectRefData *stdObject::CreateRefData() const {
	return NULL;
}

stdObjectRefData *
stdObject::CloneRefData(const stdObjectRefData * data) const {
	// if you use AllocExclusive() you must override this method
	return NULL;
}

stdClassInfo *stdObject::GetClassInfo() const {
	return &stdObject::ms_classInfo;
}

bool stdObject::IsKindOf(stdClassInfo *info) const {
	stdClassInfo *thisInfo = GetClassInfo();
	return (thisInfo) ? thisInfo->IsKindOf(info) : false ;
}


/** Implement the wxWidgets RTTI dynamic calss information */
IMPLEMENT_ABSTRACT_CLASS_STD( stdEnumeration , stdObject )
	
		