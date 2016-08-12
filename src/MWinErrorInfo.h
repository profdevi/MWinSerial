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


//v2.1 copyright Comine.com 20160328M1345
#ifndef MWinErrorInfo_h
#define MWinErrorInfo_h

///////////////////////////////////////////////////
#include "MBuffer.h"

///////////////////////////////////////////////////
class MWinErrorInfo
	{
	MBuffer mInfo;
	unsigned int mError;
	
	///////////////////////////////////////////////////
	void ClearObject(void);

	///////////////////////////////////////////////////
	public:
	MWinErrorInfo(bool initialize=false);
	~MWinErrorInfo(void);
	bool Create(void);
	bool Destroy(void);
	bool SetError(void);								// Get the last win32 error 
	bool SetError(unsigned int errorno);				// Set win32 error number
	bool TrimRight(void);								// Remove spaces(tabs newlines) from right of error string
	const char *GetErrorString(void);					// Get the Error String
	unsigned int GetErrorNo(void);
	};

#endif // MWinErrorInfo_h