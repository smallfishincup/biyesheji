#ifndef _UTILITY_MACRO_H
#define _UTILITY_MACRO_H

//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)					{if(p != NULL) { delete (p);     (p) = NULL; } }   //Delete object by New create 
#endif

#ifndef SAFE_DELETEA	
#define SAFE_DELETEA(p)					{if(p != NULL) { delete[] (p);   (p) = NULL; } }   //Delete Arrary
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)					{if(p != NULL) { (p)->Release(); (p) = NULL; } }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p)					{if(p != NULL) { free(p); (p) = NULL; } }
#endif

#ifndef CheckPointer
#define CheckPointer(pointer,hr)		{if(pointer == NULL) return hr;};
#endif

#ifndef FALSE_RETURN
#define FALSE_RETURN(bRes,hr)			{if(bRes == FALSE) return hr;};
#endif

#ifndef FALSE_GOTO
#define FALSE_GOTO(bRes,Lable)			{if(bRes == FALSE) goto Lable;};
#endif

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(handle)		{if(handle != NULL) {CloseHandle(handle);handle = NULL;}};
#endif

#endif