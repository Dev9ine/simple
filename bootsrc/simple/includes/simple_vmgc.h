/* Copyright (c) 2013-2017 Mahmoud Fayed <msfclipper@yahoo.com> */
#ifndef simple_gc_h
#define simple_gc_h
/* Functions */

void simple_vm_gc_checkreferences ( VM *pVM ) ;

void simple_vm_gc_checknewreference ( void *pPointer,int nType ) ;

void simple_vm_gc_checkupdatereference ( List *pList ) ;

void simple_vm_gc_deleteitem ( Item *pItem ) ;

void simple_vm_gc_killreference ( VM *pVM ) ;

void simple_vm_gc_deletetemplists ( VM *pVM ) ;

void simple_vm_gc_newitemreference ( Item *pItem ) ;
/* Memory Functions (General) */

SIMPLE_API void * simple_malloc ( size_t size ) ;

SIMPLE_API void simple_free ( void *ptr ) ;

SIMPLE_API void * simple_calloc ( size_t nitems, size_t size ) ;

SIMPLE_API void * simple_realloc ( void *ptr, size_t size ) ;
/* Memory Functions (Ring StateAware) */

SIMPLE_API void * simple_state_malloc ( void *pState,size_t size ) ;

SIMPLE_API void simple_state_free ( void *pState,void *pMemory ) ;

SIMPLE_API void * simple_state_calloc ( void *pState,size_t nitems, size_t size ) ;

SIMPLE_API void * simple_state_realloc ( void *pState,void *ptr, size_t size ) ;

void simple_vm_gc_deleteitem_gc ( void *pState,Item *pItem ) ;
/* Macro */
#define simple_vm_gc_cleardata(pItem) (pItem->gc.nReferenceCount = 0)
#define GCLog 0
/* Pool Manager Functions */

void simple_poolmanager_newblock ( RingState *pRingState ) ;

void * simple_poolmanager_allocate ( RingState *pRingState,size_t size ) ;

void simple_poolmanager_free ( RingState *pRingState,void *pMemory ) ;

void simple_poolmanager_delete ( RingState *pRingState ) ;
#endif