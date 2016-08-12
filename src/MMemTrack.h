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
#ifndef MMemTrack_h
#define MMemTrack_h

/////////////////////////////////////
#include "MStdLib.h"

/////////////////////////////////////////////////////////
bool MMemTrackAdd(void *memory,int blocksize,bool isarray,const char *filename,int lineno);
bool MMemTrackDel(void *memory,bool isarray,const char *filename,int lineno);
void MMemTrackPrint(void);
bool MMemTrackPrintDebug(void);
bool MMemTrackPrint(void *block);
bool MMemTrackIsValid(void);
int MMemTrackGetBlockCount(void);
unsigned int MMemTrackGetMemorySize(void);

//////////////////////////////////////////////////////////
#ifndef MMemTrackNew
#define MMemTrackNew(PTR)					MMemTrackAdd(PTR,sizeof(*PTR),false,__FILE__,__LINE__)
#endif // MMemTrackNew

//////////////////////////////////////////////////////////
#ifndef MMemTrackNewArray
#define MMemTrackNewArray(PTR,N)			MMemTrackAdd(PTR,N*sizeof(*PTR),true,__FILE__,__LINE__)
#endif // MMemTrackNewArray

//////////////////////////////////////////////////////////
#ifndef MMemTrackFree
#define MMemTrackDelete(PTR)				MMemTrackDel(PTR,false,__FILE__,__LINE__)
#endif // MMemTrackFree

//////////////////////////////////////////////////////////
#ifndef MMemTrackFreeArray
#define MMemTrackDeleteArray(PTR)			MMemTrackDel(PTR,true,__FILE__,__LINE__)
#endif // MMemTrackFreeArray

#endif // MMemTrack_h

