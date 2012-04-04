/**
*	@file CXBindingsHashTableBase.h
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 31-Janv-2009
*	@version 0.0.1
*/
#ifndef CXBINDINGS_HASHTABLE_H
#define CXBINDINGS_HASHTABLE_H

#include <iostream>

/*******************************************************************************
   DEFINES / TYPDEFS / ENUMS
*******************************************************************************/
/* std imports*/
class CXBindingsObjectBase;
class CXBindingsHashTableBase;

#if !defined(stdENUM_KEY_TYPE_DEFINED)
#define stdENUM_KEY_TYPE_DEFINED

/* Enumerate key types for the hash table */
enum stdKeyType {
	ktKEY_NONE,
	ktKEY_INTEGER,
	ktKEY_STRING
};

#endif

/* key value is either a long either a string */
union stdHashKeyValue {
	long integer;
	char* string;
};

/*******************************************************************************
   CLASS CXBindingsHashTableBase_Node DECLARATION
*******************************************************************************/
/**
*	@class CXBindingsHashTableBase_Node
*	@brief This is the base class for handling hash table elements.
*	@note 	Please note that the principle of Hash tables have been adapted
*						from wxWidgets one for std, and for convenience, we keep the
*						same organization.
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 10-Feb-2009
*	@version 0.0.1
*/
class CXBindingsHashTableBase_Node {
	/*********************************************************************************************
	* FRIENDS AND DEFINITIONS
	*********************************************************************************************/
	/** Give our private members access to the CXBindingsHashTableBase */
	friend class CXBindingsHashTableBase;

	/** Define _Node as me */
	typedef class CXBindingsHashTableBase_Node _Node;

public:

	/*********************************************************************************************
	* CONSTRUCTORS
	*********************************************************************************************/
	/**
	*	Constructor with a key as Integer (long)
	*	@param key the Node's key as long
	*	@param value the value or data in the node
	*	@param table the parent's table
	*/
	CXBindingsHashTableBase_Node( long key, void* value,
	                       CXBindingsHashTableBase* table );

	/**
	* 	Constructor with a key as string
	* 	@param key the Node's key as char*
	* 	@param value the value or data in the node
	* 	@param table the parent's table
	*/
	CXBindingsHashTableBase_Node( const char* key, void* value,
	                       CXBindingsHashTableBase* table );

	/** Default destructor */
	~CXBindingsHashTableBase_Node();

	/*********************************************************************************************
	* METHODS
	*********************************************************************************************/
	/** Gets the key as integer */
	long GetKeyInteger() const {
		return m_key.integer;
	}

	/** Gets the key as string */
	const char* GetKeyString() const {
		return m_key.string;
	}

	/** Gets the data pointer */
	void* GetData() const {
		return m_value;
	}

	/** Sets the data pointer
	* @param data Data pointer cast in void for managing multiple objects types
	*/
	void SetData( void* data ) {
		m_value = data;
	}

protected:
	/*********************************************************************************************
	* PROTECTED METHODS (INTERN)
	*********************************************************************************************/
	/** Forward to the next element in the list */
	_Node* GetNext() const {
		return m_next;
	}

protected:
	/*********************************************************************************************
	* PROTECTED VARIABLES
	*********************************************************************************************/
	/** Just stock the next node in the table */
	CXBindingsHashTableBase_Node* m_next;

	/** key value of the node */
	stdHashKeyValue m_key;

	/** Data pointer contained in the node */
	void* m_value;

	/** Pointer to the parent hash table */
	CXBindingsHashTableBase* m_hashPtr;
};

/*******************************************************************************
   CLASS CXBindingsHashTableBase DECLARATION
*******************************************************************************/
/**
*	@class CXBindingsHashTableBase
*	@brief This is the base class for defining a HashTable
*	@note 	Please note that the principle of Hash tables have been adapted
*						from wxWidgets one for std, and for convenience, we keep the
*						same organization.
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 10-Feb-2009
*	@version 0.0.1
*/
class CXBindingsHashTableBase {

	/*********************************************************************************************
	* FRIENDS AND DEFINITIONS
	*********************************************************************************************/
	/** Give our private memeber access to CXBindingsHashTableBase_Node */
	friend class CXBindingsHashTableBase_Node;

	/** Define Node as a CXBindingsHashTableBase_Node */
	typedef CXBindingsHashTableBase_Node Node;

public:

	/*********************************************************************************************
	* CONSTRUCTORS AND DESTRUCTORS
	*********************************************************************************************/
	/** Default constrcutor */
	CXBindingsHashTableBase();

	/** Default destructor, make virtual as the class has to be derived */
	virtual ~CXBindingsHashTableBase() { }

	/** Create the table with a key type and a given size
	* @param keyType the key types of the HashTable
	* @param size the hash table's size
	*/
	void Create( stdKeyType keyType = ktKEY_INTEGER,
	             size_t size = 10 );

	/**
	* Clear the table but do not delete childrens
	*/
	void Clear();

	/**
	*	Destroy the table and all its childrens nodes
	*/
	void Destroy();

	/*********************************************************************************************
	* METHODS
	*********************************************************************************************/
	/** Gets the table size (allocated size must be greather or equal to number of elements ) */
	size_t GetSize() const {
		return m_size;
	}

	/** Gets the table number of elements */
	size_t GetCount() const {
		return m_count;
	}

	/** Specifies if the content shall be deleted on destroy
	* @param falg set to true if we have to destroy all elements
	*/
	void DeleteContents( bool flag ) {
		m_deleteContents = flag;
	}

	/** Change a string key in an integer key
	* @param string the string to make as long;
	*/
	static long MakeKey(const char *string);

protected:
	/*********************************************************************************************
	* PROTECTED METHODS
	*********************************************************************************************/
	/**
	*	@brief This method is for adding a node in the table
	*	@param key the elements key as integer
	*	@param hash the index in the table where to insert the element
	*	@param data the data pointer associated to the element
	*/
	void DoPut( long key, long hash, void* data );

	/**
	*	@brief This method is for adding a node in the table
	*	@param key the elements key as string
	*	@param hash the index in the table where to insert the element
	*	@param data the data pointer associated to the element
	*/
	void DoPut( const char* key, long hash, void* data );

	/**
	*	@brief This method is for getting a node data
	*	@param key the elements key as integer
	*	@param hash the index in the table where to get the element
	*	@return this will return the data pointer contained in the node
	*/
	void* DoGet( long key, long hash ) const;

	/**
	*	@brief This method is for getting a node data
	*	@param key the elements key as string
	*	@param hash the index in the table where to get the element
	*	@return this will return the data pointer contained in the node
	*/
	void* DoGet( const char* key, long hash ) const;

	/**
	*   @brief This method is for deleting a node
	*   @param key the elements key as integer
	*   @param hash the index in the table where to delete the element
	*   @return this will return the data pointer contained in the deleted node
	*/
	void* DoDelete( long key, long hash );

	/**
	* 	@brief This method is for deleting a node
	* 	@param key the elements key as string
	* 	@param hash the index in the table where to delete the element
	* 	@return this will return the data pointer contained in the deleted node
	*/
	void* DoDelete( const char* key, long hash );

private:

	/*********************************************************************************************
	* PRIVATE METHODS
	*********************************************************************************************/
	/**
	* 	@brief Remove the node from the hash, *only called from
	* 						~wxHashTable*_Node destructor
	*	@param node the node to remove
	*/
	void DoRemoveNode( CXBindingsHashTableBase_Node* node );

	/**
	* 	@brief 	destroys data contained in the node if appropriate:
	* 						deletes the key if it is a string and destroys
	*							the value if m_deleteContents is true.
	*	@param node the node to remove
	*/
	void DoDestroyNode( CXBindingsHashTableBase_Node* node );

	/**
	* 	inserts a node in the table (at the given position)
	* 	@param bucket the index where the insertion has to be done
	* 	@param node the node to insert
	*/
	void DoInsertNode( size_t bucket, CXBindingsHashTableBase_Node* node );

	/**
	*	Removes a node from the table (given a pointer to the previous
	*	but does not delete it (only deletes its contents)
	*	@param bucket the position of the node in the table
	*	@param node the node to delete
	*	@param prev the previous node of the node to delete
	*/
	void DoUnlinkNode( size_t bucket, CXBindingsHashTableBase_Node* node,
	                   CXBindingsHashTableBase_Node* prev );

	/**
	*	unconditionally deletes node value (invoking the correct destructor)
	*	@param node the node to delete the content
	*/
	virtual void DoDeleteContents( CXBindingsHashTableBase_Node* node ) = 0;

protected:
	/*********************************************************************************************
	* PRIVATE VARIABLES
	*********************************************************************************************/
	/** number of buckets */
	size_t m_size;

	/** number of nodes (key/value pairs) */
	size_t m_count;

	/** table */
	Node** m_table;

	/** key typ (INTEGER/STRING) */
	stdKeyType m_keyType;

	/** delete contents when hash is cleared */
	bool m_deleteContents;
};

/*******************************************************************************
   CLASS CXBindingsHashTable_Node DECLARATION
*******************************************************************************/
/**
*	@class CXBindingsHashTable_Node
*	@brief This is the base class for handling hash table of CXBindingsObjectBases
*	@note 	Please note that the principle of Hash tables have been adapted
*						from wxWidgets one for std, and for convenience, we keep the
*						same organization.
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 10-Feb-2009
*	@version 0.0.1
*/
class CXBindingsHashTable_Node : public CXBindingsHashTableBase_Node {

	/*********************************************************************************************
	* FRIENDS AND DEFINITIONS
	*********************************************************************************************/
	/** Give our private member access to stdHasTable */
	friend class CXBindingsHashTable;

public:
	/*********************************************************************************************
	* CONSTRUCTORS AND DESTRUCTORS
	*********************************************************************************************/
	/**
	*	Constructor with a key as Integer (long)
	*	@param key the Node's key as long
	*	@param value the value or data in the node
	*	@param table the parent's table
	*/
	CXBindingsHashTable_Node( long key, void* value,
	                   CXBindingsHashTableBase* table )
		: CXBindingsHashTableBase_Node( key, value, table ) { };

	/**
	* 	Constructor with a key as string (char *)
	* 	@param key the Node's key as long
	* 	@param value the value or data in the node
	* 	@param table the parent's table
	*/
	CXBindingsHashTable_Node( const char* key, void* value,
	                   CXBindingsHashTableBase* table )
		: CXBindingsHashTableBase_Node( key, value, table ) { };

	/**
	* 	Overload of the wright GetData method for casting the Data as
	* 	std Objects
	* 	@return Returns the CXBindingsObjectBase contained in the node
	*/
	CXBindingsObjectBase* GetData() const {
		return (CXBindingsObjectBase*)CXBindingsHashTableBase_Node::GetData();
	}

	/**
	*	Overload of the wright GetData method for casting the Data as
	*	std Objects
	*	@param data the CXBindingsObjectBase to set in the node
	*/
	void SetData( CXBindingsObjectBase* data ) {
		CXBindingsHashTableBase_Node::SetData( data );
	}

	/**
	*	Overload of the wright GetNext method for getting the next node
	*	as a CXBindingsHashTable_Node
	*	@return The next CXBindingsHashTable_Node in the hash table
	*/
	CXBindingsHashTable_Node* GetNext() const {
		return (CXBindingsHashTable_Node*)CXBindingsHashTableBase_Node::GetNext();
	}
};

/*******************************************************************************
   CLASS CXBindingsHashTable DECLARATION
*******************************************************************************/
/**
*	@class CXBindingsHashTable
*	@brief This is the base class for handling hash table of CXBindingsObjectBases
*	@note 	Please note that the principle of Hash tables have been adapted
*						from wxWidgets one for std, and for convenience, we keep the
*						same organization.
*	@author Nicolas Macherey (nicolas.macherey@gmail.com)
*	@date 10-Feb-2009
*	@version 0.0.1
*/
class CXBindingsHashTable : public CXBindingsHashTableBase {
	/*********************************************************************************************
		* FRIENDS AND DEFINITIONS
		*********************************************************************************************/
	/** define the hash as a CXBindingsHashTableBase */
	typedef CXBindingsHashTableBase hash;
public:

	/** define Node as a CXBindingsHashTable_Node */
	typedef CXBindingsHashTable_Node Node;

	/** define a compatibility_iterator for iterating nodes */
	typedef CXBindingsHashTable_Node* compatibility_iterator;

public:

	/*********************************************************************************************
	* CONSTRUCTORS AND DESTRUCTORS
	*********************************************************************************************/
	/** Create the table with a key type and a given size
	*	@param keyType the key types of the HashTable
	*	@param size the hash table's size
	*/
	CXBindingsHashTable( stdKeyType keyType = ktKEY_INTEGER,
	              size_t size = 10 )
		: CXBindingsHashTableBase() {
		Create( keyType, size );
		BeginFind();
	}

	/** Copy constructor */
	CXBindingsHashTable( const CXBindingsHashTable& table );

	/** Default destructor */
	virtual ~CXBindingsHashTable() {
		Destroy();
	}

	/** Copy constructor by overloading = operator */
	const CXBindingsHashTable& operator=( const CXBindingsHashTable& );

	/*********************************************************************************************
	* METHODS
	*********************************************************************************************/
	/**
	*	@brief This method is for adding a CXBindingsObjectBase in the table
	*	@param value the elements key as integer (same as the index in the table)
	*	@param object the CXBindingsObjectBase pointer associated to the element
	*/
	void Put(long value, CXBindingsObjectBase *object) {
		DoPut( value, value, object );
	}

	/**
	*	@brief This method is for adding a CXBindingsObjectBase in the table
	*	@param lhash the index of the object where to insert
	*	@param value the elements key as integer
	*	@param object the CXBindingsObjectBase pointer associated to the element
	*/
	void Put(long lhash, long value, CXBindingsObjectBase *object) {
		DoPut( value, lhash, object );
	}

	/**
	*	@brief This method is for adding a CXBindingsObjectBase in the table
	*	@param value the elements key as string (same as the index in the table)
	*	@param object the CXBindingsObjectBase pointer associated to the element
	*/
	void Put(const char *value, CXBindingsObjectBase *object) {
		DoPut( value, MakeKey( value ), object );
	}

	/**
	*	@brief This method is for adding a CXBindingsObjectBase in the table
	*	@param lhash the index of the object where to insert
	*	@param value the elements key as string
	*	@param object the CXBindingsObjectBase pointer associated to the element
	*/
	void Put(long lhash, const char *value, CXBindingsObjectBase *object) {
		DoPut( value, lhash, object );
	}

	/**
	*	@brief This method is for getting a node data
	*	@param value the elements key as integer (same as index )
	*	@return this will return the CXBindingsObjectBase pointer contained in the node if found
	*/
	CXBindingsObjectBase *Get(long value) const {
		return (CXBindingsObjectBase*)DoGet( value, value );
	}

	/**
	*	@brief This method is for getting a node data
	*	@param lhash index where to insert
	*	@return this will return the CXBindingsObjectBase pointer contained in the node if found
	*/
	CXBindingsObjectBase *Get(long lhash, long value) const {
		return (CXBindingsObjectBase*)DoGet( value, lhash );
	}

	/**
	*	@brief This method is for getting a node data
	*	@param value the elements key as string (same as index )
	*	@return this will return the CXBindingsObjectBase pointer contained in the node if found
	*/
	CXBindingsObjectBase *Get(const char *value) const {
		return (CXBindingsObjectBase*)DoGet( value, MakeKey( value ) );
	}

	/**
	*	@brief This method is for getting a node data
	*	@param lhash index where to insert
	*	@param value the elements key as string
	*	@return this will return the CXBindingsObjectBase pointer contained in the node if found
	*/
	CXBindingsObjectBase *Get(long lhash, const char *value) const {
		return (CXBindingsObjectBase*)DoGet( value, lhash );
	}

	/**
	*	@brief This method is for deleting a node
	*	@param key the elements key as integer (same as index )
	*	@return this will return the CXBindingsObjectBase pointer contained in the node if found
	*/
	CXBindingsObjectBase *Delete(long key) {
		return (CXBindingsObjectBase*)DoDelete( key, key );
	}

	/**
	*	@brief This method is for deleting a node
	*	@param lhash index where to delete
	*	@param key the elements key as integer
	*	@return this will return the CXBindingsObjectBase pointer contained in the node if found
	*/
	CXBindingsObjectBase *Delete(long lhash, long key) {
		return (CXBindingsObjectBase*)DoDelete( key, lhash );
	}

	/**
	*	@brief This method is for deleting a node
	*	@param key the elements key as string (same as index )
	*	@return this will return the CXBindingsObjectBase pointer contained in the node if found
	*/
	CXBindingsObjectBase *Delete(const char *key) {
		return (CXBindingsObjectBase*)DoDelete( key, MakeKey( key ) );
	}

	/**
	*	@brief This method is for deleting a node
	*	@param lhash index where to delete
	*	@param key the elements key as string
	*	@return this will return the CXBindingsObjectBase pointer contained in the node if found
	*/
	CXBindingsObjectBase *Delete(long lhash, const char *key) {
		return (CXBindingsObjectBase*)DoDelete( key, lhash );
	}

	/** Construct your own integer key from a string, e.g. in case
	*	you need to combine it with something
	*	@param string the string to transform
	*/
	long MakeKey(const char *string) const {
		return CXBindingsHashTableBase::MakeKey(string);
	}

	/** Way of iterating through whole hash table (e.g. to delete everything)
	*	Not necessary, of course, if you're only storing pointers to
	*	objects maintained separately
	*/
	void BeginFind() {
		m_curr = NULL;
		m_currBucket = 0;
	}

	/** Get the next node */
	Node* Next();

	/** Clear the table but do not delete its content */
	void Clear() {
		CXBindingsHashTableBase::Clear();
	}

	/** Get the table number of elements */
	size_t GetCount() const {
		return CXBindingsHashTableBase::GetCount();
	}

protected:
	/*********************************************************************************************
	* PROTECTED METHODS
	*********************************************************************************************/
	/** copy helper
	*	@param copy the stdHasTable to copy
	*/
	void DoCopy( const CXBindingsHashTable& copy );

	/** searches the next node starting from bucket bucketStart and sets
	*	m_curr to it and m_currBucket to its bucket
	*	@param bucketStart the index from which to start
	*/
	void GetNextNode( size_t bucketStart );

private :
	/*********************************************************************************************
	* PRIVATE METHODS OVERLOADED
	*********************************************************************************************/
	/** Delete the content of a node
	*	@param node the node to delete the content
	*/
	virtual void DoDeleteContents( CXBindingsHashTableBase_Node* node );

	/** current node */
	Node* m_curr;

	/** bucket the current node belongs to */
	size_t m_currBucket;
};

#endif
