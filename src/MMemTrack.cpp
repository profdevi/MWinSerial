/*
Copyright (C) 2011-2014, Comine.com ( profdevi@ymail.com )
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- The the names of the contributors of this project may not be used to 
  endorse or promote products derived from this software without specific 
  prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
`AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


//v2.0 copyright Comine.com 20150810M0824
#include "MStdLib.h"
#include "MMemTrack.h"


////////////////////////////////////////////////////////////
#if (defined(MSTDLIB_API_WINDOWS) )
#include <windows.h>

////////////////////////////////////////////////////////////
#elif ( defined(MSTDLIB_API_LINUX) )
#include <new>
#include <pthread.h>

////////////////////////////////////////////////////////////
#elif defined(MSTDLIB_OS_MOSYNC)

#endif // MOSWINDOWS

///////////////////////////////////////////////////////
// Critical Section Variables
#if ( defined(MSTDLIB_API_WINDOWS) )
static volatile LONG GCriticalSectionCounter=0;

//////////////////////////////////////////////////////
#elif (defined(MSTDLIB_API_LINUX) )
static pthread_mutex_t  GCriticalSectionMutex=PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////////
#elif defined(MSTDLIB_OS_MOSYNC)
static volatile int GCriticalSectionCounter=0;

#endif // MSTDLIB_API_WINDOWS


///////////////////////////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
static void GCriticalSectionEnter(void)
	{
	for(;;)
		{
		long newvalue;
		newvalue=InterlockedIncrement((LPLONG)&GCriticalSectionCounter);
		if(newvalue==1) { return; }
		InterlockedDecrement((LPLONG)&GCriticalSectionCounter);
		}
	}


///////////////////////////////////////
static void GCriticalSectionLeave(void)
	{
	InterlockedDecrement((LPLONG)&GCriticalSectionCounter);
	}

////////////////////////////////////////////////////////////
#elif (defined(MSTDLIB_API_LINUX) )
static void GCriticalSectionEnter(void)
	{
	if(pthread_mutex_lock(&GCriticalSectionMutex)!=0)
		{
		return;
		}
	}


///////////////////////////////////////
static void GCriticalSectionLeave(void)
	{
	if(pthread_mutex_unlock(&GCriticalSectionMutex)!=0)
		{
		return;
		}
	}


////////////////////////////////////////////////////////////
#elif defined(MSTDLIB_OS_MOSYNC)
static void GCriticalSectionEnter(void)
	{
	for(;;)
		{
		GCriticalSectionCounter = GCriticalSectionCounter + 1;
		
		if(GCriticalSectionCounter==1) { return; }

		GCriticalSectionCounter = GCriticalSectionCounter - 1;
		}
	}


///////////////////////////////////////
static void GCriticalSectionLeave(void)
	{
	GCriticalSectionCounter=0;
	}

#endif // MSTDLIB_API_WINDOWS

//*******************************************************
//** Node Tracking Infor
//*******************************************************
struct GMemTrackNodeInfo
	{
	const char *FileName;
	int LineNo;
	bool IsArray;
	int BlockSize;
	void *MemBlock;
	GMemTrackNodeInfo * Next;
	};


/////////////////////////////////////////////////////////
static GMemTrackNodeInfo *GMemTrackList=NULL;

/////////////////////////////////////////
bool MMemTrackAdd(void *memory,int blocksize,bool isarray,const char *filename,int lineno)
	{
	if(memory==NULL) { return true; }

	GMemTrackNodeInfo *newnode=new(std::nothrow) GMemTrackNodeInfo;
	if(newnode==NULL)
		{
		return false;
		}

	newnode->BlockSize = blocksize;
	newnode->FileName = filename;
	newnode->LineNo = lineno;
	newnode->IsArray = isarray;
	newnode->MemBlock = memory;
	
	GCriticalSectionEnter();
	newnode->Next = GMemTrackList;
	GMemTrackList=newnode;
	GCriticalSectionLeave();
	
	return true;
	}


/////////////////////////////////////////
bool MMemTrackDel(void *memory,bool isarray,const char *filename,int lineno)
	{
	if(memory==NULL)
		{
		return false;
		}

	// Enter Critical Section
	GCriticalSectionEnter();

	GMemTrackNodeInfo *ptr=GMemTrackList;
	if(ptr==NULL)
		{
		GCriticalSectionLeave();
		return false;
		}

	// Check in first node
	if(ptr->MemBlock == memory)
		{
		// check if array deallaction
		if(ptr->IsArray != isarray)
			{
			//=Continue Anyway
			}

		GMemTrackList=GMemTrackList -> Next;

		delete ptr;
		GCriticalSectionLeave();
		return true;
		}
	
	for(ptr=GMemTrackList;ptr->Next!=NULL;ptr = ptr->Next )
		{
		GMemTrackNodeInfo *nextnode=ptr->Next;
		if(nextnode->MemBlock != memory) { continue; }
		
		if(nextnode->IsArray != isarray)
			{
			//=Continue anyway
			}

		ptr->Next = nextnode->Next;
		delete nextnode;
		GCriticalSectionLeave();
		return true;
		}
	
	GCriticalSectionLeave();
	return false;
	}


///////////////////////////////////////////
void MMemTrackPrint(void)
	{
	GCriticalSectionEnter();

	MStdPrintf("  **   Dynamic Memory List        ** \n");
	MStdPrintf(" ------------------------------------\n");

	int blockcount=0;
	unsigned int memcount=0;
	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		const char *arraydata="     ";
		if(p->IsArray==true)
			{  arraydata = "array";  }

		MStdPrintf("%-30s(%4d) %s : %d bytes\n",p->FileName,p->LineNo,arraydata,p->BlockSize);

		blockcount = blockcount + 1;
		memcount = memcount + p->BlockSize;
		}

	MStdPrintf("  %d Blocks    %u bytes\n",blockcount,memcount);

	GCriticalSectionLeave();
	}


///////////////////////////////////////////
bool MMemTrackPrintDebug(void)
	{
	MMemTrackPrint();
	return true;
	}


//////////////////////////////////////////
bool MMemTrackPrint(void *block)
	{
	GCriticalSectionEnter();

	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		if(p->MemBlock!=block) { continue; }

		const char *arraydata="     ";
		if(p->IsArray==true)
			{  arraydata = "array";  }
		
		MStdPrintf("Memory: %s(%d) %s %d bytes\n",p->FileName,p->LineNo
				,arraydata,p->BlockSize);
		}
	
	GCriticalSectionLeave();

	MStdPrintf("Memory Block: Unknown \n");
	return false;
	}


//////////////////////////////////////////
bool MMemTrackIsValid(void)
	{
	GCriticalSectionEnter();

	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		if(p->FileName==NULL) { return false; }
		if(p->LineNo<=0) { return false; }
		if(p->MemBlock==NULL) { return false; }
		if(p->BlockSize<=0) { return false; }
		}

	GCriticalSectionLeave();
	return true;
	}


//////////////////////////////////////////
int MMemTrackGetBlockCount(void)
	{
	GCriticalSectionEnter();
	int blockcount=0;
	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		blockcount = blockcount + 1;
		}	
	
	GCriticalSectionLeave();
	return blockcount;
	}


//////////////////////////////////////////
unsigned int MMemTrackGetMemorySize(void)
	{
	GCriticalSectionEnter();
	unsigned int memcount=0;
	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		memcount = memcount + p->BlockSize;
		}

	GCriticalSectionLeave();
	return memcount;
	}


