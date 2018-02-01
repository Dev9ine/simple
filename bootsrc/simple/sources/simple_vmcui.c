/* Copyright (c) 2013-2018 Mahmoud Fayed <msfclipper@yahoo.com> */
#include "../includes/simple.h"
/* User Interface - Commands Implementation (Faster) - Because we don't have functions call */

void simple_vm_see ( VM *pVM )
{
	Item *pItem  ;
	char cStr[100]  ;
	List *pList  ;
	char *cString  ;
	int x  ;
	if ( pVM->nFuncExecute > 0 ) {
		pVM->nFuncExecute-- ;
	}
	if ( SIMPLE_VM_STACK_ISSTSIMPLE ) {
		cString = SIMPLE_VM_STACK_READC ;
		if ( strlen(cString) != (unsigned int) SIMPLE_VM_STACK_STSIMPLESIZE ) {
			for ( x = 0 ; x < SIMPLE_VM_STACK_STSIMPLESIZE ; x++ ) {
				printf( "%c",cString[x] ) ;
			}
		} else {
			printf( "%s",cString ) ;
		}
	}
	else if ( SIMPLE_VM_STACK_ISPOINTER ) {
		if ( SIMPLE_VM_STACK_OBJTYPE == SIMPLE_OBJTYPE_VARIABLE ) {
			pList = simple_list_getlist((List *) SIMPLE_VM_STACK_READP,SIMPLE_VAR_VALUE);
			if ( simple_vm_oop_isobject(pList) ) {
				simple_vm_oop_printobj(pVM,pList);
			} else {
				simple_list_print(pList);
			}
		}
		else if ( SIMPLE_VM_STACK_OBJTYPE ==SIMPLE_OBJTYPE_LISTITEM ) {
			pItem = (Item *) SIMPLE_VM_STACK_READP ;
			pList = simple_item_getlist(pItem) ;
			if ( simple_vm_oop_isobject(pList) ) {
				simple_vm_oop_printobj(pVM,pList);
			} else {
				simple_list_print(pList);
			}
		}
	}
	else if ( SIMPLE_VM_STACK_ISNUMBER ) {
		simple_vm_numtostring(pVM,SIMPLE_VM_STACK_READN,cStr);
		printf( "%s",cStr ) ;
	}
	SIMPLE_VM_STACK_POP ;
	simple_vm_freestack(pVM);
	fflush(stdout);
}

void simple_vm_give ( VM *pVM )
{
	int x  ;
	char cLine[256]  ;
	List *pVar  ;
	Item *pItem  ;
	/* Get Input From the User and save it in the variable */
	fgets(cLine , 256 , stdin );
	/* Remove New Line */
	for ( x = 0 ; x <= 255 ; x++ ) {
		if ( cLine[x] == '\n' ) {
			cLine[x] = '\0' ;
			break ;
		}
	}
	if ( SIMPLE_VM_STACK_ISPOINTER ) {
		if ( SIMPLE_VM_STACK_OBJTYPE == SIMPLE_OBJTYPE_VARIABLE ) {
			pVar = (List *) SIMPLE_VM_STACK_READP ;
			SIMPLE_VM_STACK_POP ;
			simple_list_setint_gc(pVM->pRingState,pVar, SIMPLE_VAR_TYPE ,SIMPLE_VM_STSIMPLE);
			simple_list_setstsimple_gc(pVM->pRingState,pVar, SIMPLE_VAR_VALUE, cLine);
		}
		else if ( SIMPLE_VM_STACK_OBJTYPE ==SIMPLE_OBJTYPE_LISTITEM ) {
			pItem = (Item *) SIMPLE_VM_STACK_READP ;
			SIMPLE_VM_STACK_POP ;
			simple_item_settype_gc(pVM->pRingState,pItem,ITEMTYPE_STSIMPLE);
			simple_stsimple_set_gc(pVM->pRingState,simple_item_getstring(pItem),cLine);
		}
	}
}
/* User Interface Functions (Another implementation) - Flexibile (We can replace functions in Ring Code) */

void simple_vmlib_see ( void *pPointer )
{
	char *cString  ;
	int x  ;
	char cStr[100]  ;
	List *pList  ;
	VM *pVM  ;
	pVM = (VM *) pPointer ;
	if ( SIMPLE_API_ISSTSIMPLE(1) ) {
		cString = SIMPLE_API_GETSTSIMPLE(1) ;
		if ( strlen(cString) != (unsigned int) SIMPLE_API_GETSTSIMPLESIZE(1) ) {
			for ( x = 0 ; x < SIMPLE_API_GETSTSIMPLESIZE(1) ; x++ ) {
				printf( "%c",cString[x] ) ;
			}
		} else {
			printf( "%s",cString ) ;
		}
	}
	else if ( SIMPLE_API_ISNUMBER(1) ) {
		simple_vm_numtostring(pVM,SIMPLE_API_GETNUMBER(1),cStr);
		printf( "%s",cStr ) ;
	}
	else if ( SIMPLE_API_ISLIST(1) ) {
		pList = SIMPLE_API_GETLIST(1);
		if ( simple_vm_oop_isobject(pList) ) {
			simple_vm_oop_printobj(pVM,pList);
		} else {
			simple_list_print(pList);
		}
	}
	fflush(stdout);
}

void simple_vmlib_give ( void *pPointer )
{
	int x  ;
	char cLine[256]  ;
	/* Get Input From the User and save it in the variable */
	fgets(cLine , 256 , stdin );
	/* Remove New Line */
	for ( x = 0 ; x <= 255 ; x++ ) {
		if ( cLine[x] == '\n' ) {
			cLine[x] = '\0' ;
			break ;
		}
	}
	SIMPLE_API_RETSTSIMPLE(cLine);
}