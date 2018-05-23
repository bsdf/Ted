/************************************************************************/
/*									*/
/*  Mapping of string values to (void *)pointers.			*/
/*									*/
/************************************************************************/

#   ifndef		UTIL_TREE_H
#   define		UTIL_TREE_H

typedef int (*UTIL_TREE_CALLBACK)(	const char *		key,
					void *			val,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

					/**
					 *  Build a tree.
					 *  @param ownKeys If ownKeys is 
					 *	true, the tree will keep 
					 *	its own copy of the keys.
					 *	If ownKeys is false, the tree
					 *	will use the key values 
					 *	internally. In that case, it is
					 *	the responsibility of the caller
					 *	to keep the keys alive while 
					 *	they own values in the tree and
					 *	to free them after that.
					 */
extern void * utilTreeMakeTree(		int			ownKeys );

/**/
extern int utilTreeStoreValue(		void *			tree,
					void **			pPreviousValue,
					const char **		pStoredKey,
					const char *		key,
					void *			val );

/**/
extern void * utilTreeGetLT(		void *			tree,
					const char **		pKey,
					const char *		key );

extern void * utilTreeGetLE(		void *			tree,
					const char **		pKey,
					const char *		key );

extern void * utilTreeGetEQ(		const void *		tree,
					const char **		pKey,
					const char *		key );

extern void * utilTreeGetGE(		void *			tree,
					const char **		pKey,
					const char *		key );

extern void * utilTreeGetGT(		void *			tree,
					const char **		pKey,
					const char *		key );

/**/
extern void * utilTreeGetFirst(		void *			tree,
					const char **		pKey );

extern void * utilTreeGetPrevious(	void *			tree,
					const char **		pKey );

extern void * utilTreeGetCurrent(	void *			tree,
					const char **		pKey );

extern void * utilTreeGetNext(		void *			tree,
					const char **		pKey );

extern void * utilTreeGetLast(		void *			tree,
					const char **		pKey );

/**/
extern int utilTreeDeleteEQ(		void *			tree,
					const char *		key,
					UTIL_TREE_CALLBACK	callback,
					void *			through );

extern int utilTreeDeleteCurrent(	void *			tree,
					UTIL_TREE_CALLBACK	callback,
					void *			through );

/**/
extern int utilTreeForAll(		void *			tree,
					int			stopOnFailure,
					UTIL_TREE_CALLBACK	callback,
					void *			through );

/**/
extern int utilTreeFreeTree(		void *			tree,
					UTIL_TREE_CALLBACK	callback,
					void *			through );

/**/
extern int utilTreeFreeValue(		const char *		key,
					void *			val,
					void *			through );

#   endif	/*	UTIL_TREE_H	*/
