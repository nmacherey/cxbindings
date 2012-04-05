
/*!
* @file stdHashTable.cpp
* @author Nicolas Macherey (nm@graymat.fr)
* @date February 2009
* @brief File containing classes definitions for a HashTable
*/

/*******************************************************************************
   INCLUDES
*******************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "stdHashTable.h"
#include "stdObject.h"


/*******************************************************************************
   CLASS stdHashTableBase_Node DEFINITION
*******************************************************************************/
stdHashTableBase_Node::stdHashTableBase_Node( long key, void* value,
        stdHashTableBase* table )
	: m_value( value ), m_hashPtr( table ) {
	m_key.integer = key;
}

stdHashTableBase_Node::stdHashTableBase_Node( const char* key, void* value,
        stdHashTableBase* table )
	: m_value( value ), m_hashPtr( table ) {
	m_key.string = strcpy( new char[strlen( key ) + 1], key );
}

stdHashTableBase_Node::~stdHashTableBase_Node() {
	if( m_hashPtr ) m_hashPtr->DoRemoveNode( this );
}

/*******************************************************************************
   CLASS stdHashTableBase DEFINITION
*******************************************************************************/
stdHashTableBase::stdHashTableBase()
	: m_size( 0 ), m_count( 0 ), m_table( NULL ), m_keyType( ktKEY_NONE ),
	  m_deleteContents( false ) {
}

void stdHashTableBase::Create( stdKeyType keyType, size_t size ) {
	m_keyType = keyType;
	m_size = size;
	m_table = new stdHashTableBase_Node*[ m_size ];

	for( size_t i = 0; i < m_size; ++i )
		m_table[i] = NULL;
}

void stdHashTableBase::Clear() {
	for( size_t i = 0; i < m_size; ++i ) {
		Node* end = m_table[i];

		if( end == NULL )
			continue;

		Node *curr, *next = end->GetNext();

		do {
			curr = next;
			next = curr->GetNext();

			DoDestroyNode( curr );

			delete curr;
		} while( curr != end );

		m_table[i] = NULL;
	}

	m_count = 0;
}

void stdHashTableBase::DoRemoveNode( stdHashTableBase_Node* node ) {
	size_t bucket = ( m_keyType == ktKEY_INTEGER ?
	                  node->m_key.integer        :
	                  MakeKey( node->m_key.string ) ) % m_size;

	if( node->GetNext() == node ) {
		// single-node chain (common case)
		m_table[bucket] = NULL;
	} else {
		Node *start = m_table[bucket], *curr;
		Node* prev = start;

		for( curr = prev->GetNext(); curr != node;
		        prev = curr, curr = curr->GetNext() ) ;

		DoUnlinkNode( bucket, node, prev );
	}

	DoDestroyNode( node );
}

void stdHashTableBase::DoDestroyNode( stdHashTableBase_Node* node ) {
	// if it is called from DoRemoveNode, node has already been
	// removed, from other places it does not matter
	node->m_hashPtr = NULL;

	if( m_keyType == ktKEY_STRING )
		delete[] node->m_key.string;
	if( m_deleteContents )
		DoDeleteContents( node );
}

void stdHashTableBase::Destroy() {
	Clear();

	delete[] m_table;

	m_table = NULL;
	m_size = 0;
}

void stdHashTableBase::DoInsertNode( size_t bucket, stdHashTableBase_Node* node ) {
	if( m_table[bucket] == NULL ) {
		m_table[bucket] = node->m_next = node;
	} else {
		Node *prev = m_table[bucket];
		Node *next = prev->m_next;

		prev->m_next = node;
		node->m_next = next;
		m_table[bucket] = node;
	}

	++m_count;
}

void stdHashTableBase::DoPut( long key, long hash, void* data ) {
	size_t bucket = size_t(hash) % m_size;
	Node* node = new stdHashTableBase_Node( key, data, this );

	DoInsertNode( bucket, node );
}

void stdHashTableBase::DoPut( const char* key, long hash, void* data ) {
	size_t bucket = size_t(hash) % m_size;
	Node* node = new stdHashTableBase_Node( key, data, this );

	DoInsertNode( bucket, node );
}

void* stdHashTableBase::DoGet( long key, long hash ) const {

	size_t bucket = size_t(hash) % m_size;

	if( m_table[bucket] == NULL )
		return NULL;

	Node *first = m_table[bucket]->GetNext(),
	      *curr = first;

	do {
		if( curr->m_key.integer == key )
			return curr->m_value;

		curr = curr->GetNext();
	} while( curr != first );

	return NULL;
}

void* stdHashTableBase::DoGet( const char* key, long hash ) const {

	size_t bucket = size_t(hash) % m_size;

	if( m_table[bucket] == NULL )
		return NULL;

	Node *first = m_table[bucket]->GetNext(),
	      *curr = first;

	do {
		if( strcmp( curr->m_key.string, key ) == 0 )
			return curr->m_value;

		curr = curr->GetNext();
	} while( curr != first );

	return NULL;
}

void stdHashTableBase::DoUnlinkNode( size_t bucket, stdHashTableBase_Node* node,
                                     stdHashTableBase_Node* prev ) {
	if( node == m_table[bucket] )
		m_table[bucket] = prev;

	if( prev == node && prev == node->GetNext() )
		m_table[bucket] = NULL;
	else
		prev->m_next = node->m_next;

	DoDestroyNode( node );
	--m_count;
}

void* stdHashTableBase::DoDelete( long key, long hash ) {

	size_t bucket = size_t(hash) % m_size;

	if( m_table[bucket] == NULL )
		return NULL;

	Node *first = m_table[bucket]->GetNext(),
	      *curr = first,
	       *prev = m_table[bucket];

	do {
		if( curr->m_key.integer == key ) {
			void* retval = curr->m_value;
			curr->m_value = NULL;

			DoUnlinkNode( bucket, curr, prev );
			delete curr;

			return retval;
		}

		prev = curr;
		curr = curr->GetNext();
	} while( curr != first );

	return NULL;
}

void* stdHashTableBase::DoDelete( const char* key, long hash ) {

	size_t bucket = size_t(hash) % m_size;

	if( m_table[bucket] == NULL )
		return NULL;

	Node *first = m_table[bucket]->GetNext(),
	      *curr = first,
	       *prev = m_table[bucket];

	do {
		if( strcmp( curr->m_key.string, key ) == 0 ) {
			void* retval = curr->m_value;
			curr->m_value = NULL;

			DoUnlinkNode( bucket, curr, prev );
			delete curr;

			return retval;
		}

		prev = curr;
		curr = curr->GetNext();
	} while( curr != first );

	return NULL;
}

long stdHashTableBase::MakeKey( const char *str ) {
	long int_key = 0;

	while( *str )
		int_key += (char)*str++;

	return int_key;
}

/*******************************************************************************
   CLASS stdHashTable DEFINITION
*******************************************************************************/
stdHashTable::stdHashTable( const stdHashTable& table )
	: stdHashTableBase() {
	DoCopy( table );
}

const stdHashTable& stdHashTable::operator=( const stdHashTable& table ) {
	Destroy();
	DoCopy( table );

	return *this;
}

void stdHashTable::DoCopy( const stdHashTable& table ) {
	Create( m_keyType, m_size );
}

void stdHashTable::DoDeleteContents( stdHashTableBase_Node* node ) {
	delete ((stdHashTable_Node*)node)->GetData();
}

void stdHashTable::GetNextNode( size_t bucketStart ) {
	for( size_t i = bucketStart; i < m_size; ++i ) {
		if( m_table[i] != NULL ) {
			m_curr = ((Node*)m_table[i])->GetNext();
			m_currBucket = i;
			return;
		}
	}

	m_curr = NULL;
	m_currBucket = 0;
}

stdHashTable::Node* stdHashTable::Next() {
	if( m_curr == NULL )
		GetNextNode( 0 );
	else {
		m_curr = m_curr->GetNext();

		if( m_curr == ( (Node*)m_table[m_currBucket] )->GetNext() )
			GetNextNode( m_currBucket + 1 );
	}

	return m_curr;
}
		