/* Copyright (c) 2013-2018 Mahmoud Fayed <msfclipper@yahoo.com> */
#ifndef simple_vmdll_h
#define simple_vmdll_h
#ifdef _WIN32
#include <windows.h>
#define LoadDLL(x) LoadLibrary(x)
#define GetDLLFunc GetProcAddress
#define CloseDLL FreeLibrary
typedef HINSTANCE LpHandleType ;
#else
#include <dlfcn.h>
#define LoadDLL(x) dlopen(x, RTLD_LAZY)
#define GetDLLFunc dlsym
#define CloseDLL dlclose
typedef void * LpHandleType ;
#endif
typedef void (*loadlibfuncptr)(RingState *) ;

void simple_vm_dll_loadfunctions ( RingState *pRingState ) ;

void simple_vm_dll_loadlib ( void *pPointer ) ;

void simple_vm_dll_closelib ( void *pPointer ) ;
#endif