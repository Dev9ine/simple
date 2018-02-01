/* Copyright (c) 2013-2016 Mahmoud Fayed <msfclipper@yahoo.com> */
#include "../includes/simple.h"

void simple_vm_gc_checkreferences ( VM *pVM )
{
	int x  ;
	List *pList, *pList2  ;
	Item *pItem  ;
	/* Check References (Called when we delete a scope) */
	pList = simple_list_getlist(pVM->pMem,simple_list_getsize(pVM->pMem));
	for ( x = simple_list_getsize(pList) ; x >= 1 ; x-- ) {
		pList2 = simple_list_getlist(pList,x);
		if ( simple_list_getsize(pList2) == SIMPLE_VAR_LISTSIZE ) {
			if ( simple_list_getint(pList2,SIMPLE_VAR_TYPE) == SIMPLE_VM_POINTER ) {
				if ( simple_list_getint(pList2,SIMPLE_VAR_PVALUETYPE) == SIMPLE_OBJTYPE_LISTITEM ) {
					pItem = (Item *) simple_list_getpointer(pList2,SIMPLE_VAR_VALUE) ;
					#if GCLog
					printf( "GC CheckReferences - Free Memory %p \n",pItem ) ;
					#endif
					simple_item_delete(pItem);
				}
			}
		}
	}
}

void simple_vm_gc_checknewreference ( void *pPointer,int nType )
{
	Item *pItem  ;
	/*
	**  Called when we create new pointer (new reference) 
	**  The reference may be to a list or a sublist (list item) 
	**  We care only about list items (sublists) 
	*/
	if ( nType == SIMPLE_OBJTYPE_LISTITEM ) {
		pItem = (Item *) pPointer ;
		pItem->gc.nReferenceCount++ ;
		#if GCLog
		printf( "\nGC CheckNewReference - To Pointer %p \n",pItem ) ;
		#endif
	}
}

void simple_vm_gc_checkupdatereference ( List *pList )
{
	Item *pItem  ;
	/* Reference Counting to Destination before copy from Source */
	if ( simple_list_getint(pList,SIMPLE_VAR_TYPE) == SIMPLE_VM_POINTER ) {
		if ( simple_list_getint(pList,SIMPLE_VAR_PVALUETYPE) == SIMPLE_OBJTYPE_LISTITEM ) {
			pItem = (Item *) simple_list_getpointer(pList,SIMPLE_VAR_VALUE) ;
			simple_item_delete(pItem);
		}
	}
}

void simple_vm_gc_deleteitem_gc ( void *pState,Item *pItem )
{
	if ( pItem->gc.nReferenceCount == 0 ) {
		#if GCLog
		printf( "GC Delete Item - Free Memory %p \n",pItem ) ;
		#endif
		simple_item_content_delete(pItem);
		simple_state_free(pState,pItem);
	}
	else {
		pItem->gc.nReferenceCount-- ;
	}
}

void simple_vm_gc_killreference ( VM *pVM )
{
	List *pList  ;
	if ( SIMPLE_VM_STACK_OBJTYPE == SIMPLE_OBJTYPE_VARIABLE ) {
		pList = (List *) SIMPLE_VM_STACK_READP ;
		simple_vm_gc_checkupdatereference(pList);
	}
}

void simple_vm_gc_deletetemplists ( VM *pVM )
{
	int x  ;
	List *pScope,*pList  ;
	/* If we are in the class region (After class name) then return */
	if ( pVM->nInClassRegion ) {
		return ;
	}
	/*
	**  This function is called from Ring code by callgc() 
	**  Function Goal 
	**  When we return Lists/Pointers from functions we create variable called simple_sys_temp 
	**  This variable is created in the previous scope instead of the active function scope 
	**  This is necessary because the function scope will be deleted and we need the varaible 
	**  This is important because we may assign the variable to variable name 
	**  The problem happens when we have a loop that uses f1(f2()) 
	**  and f2() return a lists/pointer 
	**  The temp list/C Pointer will not be deleted until the end of the function 
	**  But we have a loop and this will lead to a memory leak during loop execution 
	**  When testing al_map_rgb() in Allegro Library this problem becomes critical 
	**  This function solves this problem, by deleting temp lists/C Pointers in the current scope 
	**  We use -1 to skip the currect scope of the Ring function (callgc()) 
	*/
	pScope = simple_list_getlist(pVM->pMem,simple_list_getsize(pVM->pMem)-1) ;
	/* The function works only when we expect to have temp variables */
	if ( simple_list_getsize(pScope) >= 1 ) {
		for ( x = simple_list_getsize(pScope) ; x >= 1 ; x-- ) {
			pList = simple_list_getlist(pScope,x);
			if ( simple_list_getsize(pList) != SIMPLE_VAR_LISTSIZE ) {
				continue ;
			}
			if ( strcmp(simple_list_getstring(pList,SIMPLE_VAR_NAME),SIMPLE_TEMP_VARIABLE) == 0 ) {
				simple_list_deleteitem(pScope,x);
			}
			else if ( strcmp(simple_list_getstring(pList,SIMPLE_VAR_NAME),SIMPLE_TEMP_OBJECT) == 0 ) {
				simple_list_deleteitem(pScope,x);
			}
		}
	}
	/* Delete The HashTable */
	pScope->pHashTable = simple_hashtable_delete(pScope->pHashTable) ;
	simple_list_genhashtable2(pScope);
}

void simple_vm_gc_newitemreference ( Item *pItem )
{
	pItem->gc.nReferenceCount++ ;
}
/* Memory Functions (General) */

SIMPLE_API void * simple_malloc ( size_t size )
{
	return malloc(size) ;
}

SIMPLE_API void simple_free ( void *ptr )
{
	free( ptr ) ;
}

SIMPLE_API void * simple_calloc ( size_t nitems, size_t size )
{
	return calloc(nitems,size) ;
}

SIMPLE_API void * simple_realloc ( void *ptr, size_t size )
{
	return realloc(ptr,size) ;
}
/* Memory Functions (RingState Aware) */

SIMPLE_API void * simple_state_malloc ( void *pState,size_t size )
{
	#if SIMPLE_USEPOOLMANAGER
	if ( pState != NULL ) {
		if ( ((RingState *) pState )->pVM  != NULL ) {
			if ( size <= SIMPLE_POOLMANAGER_ITEMSIZE ) {
				return simple_poolmanager_allocate( (RingState *) pState,size) ;
			}
		}
	}
	#endif
	return simple_malloc(size) ;
}

SIMPLE_API void simple_state_free ( void *pState,void *pMemory )
{
	#if SIMPLE_USEPOOLMANAGER
	/* Use Pool Manager */
	simple_poolmanager_free((RingState *) pState,pMemory);
	#else
	simple_free(pMemory);
	#endif
}

SIMPLE_API void * simple_state_calloc ( void *pState,size_t nitems, size_t size )
{
	return simple_calloc(nitems,size) ;
}

SIMPLE_API void * simple_state_realloc ( void *pState,void *ptr, size_t size )
{
	return simple_realloc(ptr,size) ;
}

void simple_vm_gc_deleteitem ( Item *pItem )
{
	simple_vm_gc_deleteitem_gc(NULL,pItem);
}
/* Pool Manager Functions */

void simple_poolmanager_newblock ( RingState *pRingState )
{
	PoolData *pMemory  ;
	int x  ;
	/* Get Block Memory */
	pMemory = (PoolData *) simple_malloc(sizeof(PoolData)*SIMPLE_POOLMANAGER_ITEMSINBLOCK);
	/* Check Memory */
	if ( pMemory == NULL ) {
		printf( SIMPLE_OOM ) ;
		exit(0);
	}
	/* Set Linked Lists (pNext values) */
	for ( x = 0 ; x < SIMPLE_POOLMANAGER_ITEMSINBLOCK - 2 ; x++ ) {
		pMemory[x].pNext = pMemory+x+1 ;
	}
	pMemory[SIMPLE_POOLMANAGER_ITEMSINBLOCK-1].pNext = NULL ;
	/*
	**  Set Values in Ring State 
	**  Set First Item in Ring State 
	*/
	pRingState->vPoolManager.pCurrentItem = pMemory+1 ;
	/* Set Block Start and End */
	pRingState->vPoolManager.pBlockStart = (void *) pMemory ;
	pRingState->vPoolManager.pBlockEnd = (void *) (pMemory + SIMPLE_POOLMANAGER_ITEMSINBLOCK - 1) ;
}

void * simple_poolmanager_allocate ( RingState *pRingState,size_t size )
{
	void *pMemory  ;
	pMemory = NULL ;
	/* If No memory - Create new block */
	if ( (pRingState->vPoolManager.pCurrentItem == NULL) && (pRingState->vPoolManager.pBlockStart == NULL) ) {
		simple_poolmanager_newblock(pRingState);
	}
	/* Get Item from the Pool Manager */
	if ( pRingState->vPoolManager.pCurrentItem != NULL ) {
		pMemory = pRingState->vPoolManager.pCurrentItem ;
		pRingState->vPoolManager.pCurrentItem = pRingState->vPoolManager.pCurrentItem->pNext ;
	}
	/* If no free items, Allocate new item */
	else {
		pMemory = simple_malloc(size);
		/* Check Memory */
		if ( pMemory == NULL ) {
			printf( SIMPLE_OOM ) ;
			exit(0);
		}
	}
	return pMemory ;
}

void simple_poolmanager_free ( RingState *pRingState,void *pMemory )
{
	PoolData *pPoolData  ;
	if ( pRingState != NULL ) {
		if ( pRingState->vPoolManager.pBlockStart != NULL ) {
			if ( (pMemory > pRingState->vPoolManager.pBlockStart) && (pMemory <= pRingState->vPoolManager.pBlockEnd ) ) {
				pPoolData = (PoolData *) pMemory ;
				pPoolData->pNext = pRingState->vPoolManager.pCurrentItem ;
				pRingState->vPoolManager.pCurrentItem = pPoolData ;
				return ;
			}
		}
	}
	simple_free(pMemory);
}

void simple_poolmanager_delete ( RingState *pRingState )
{
	if ( pRingState != NULL ) {
		if ( pRingState->vPoolManager.pBlockStart != NULL ) {
			free( pRingState->vPoolManager.pBlockStart ) ;
		}
	}
}