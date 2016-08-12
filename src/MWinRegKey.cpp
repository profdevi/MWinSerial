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


//v2.1 copyright Comine.com 20150911F0941
#include <Windows.h>
#include <Shlwapi.h>
#include "MStdLib.h"
#include "MString.h"
#include "MBuffer.h"
#include "MWinRegKey.h"


/////////////////////////////////////////////////////////////////
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"Shlwapi.lib")

//////////////////////////////////////////////////////////////////////
DWORD MWinRegKey::mTmpType=0;
DWORD MWinRegKey::mTmpSize=0;


//////////////////////////////////////////////////////////////////////
void MWinRegKey::ClearObject(void)
	{
	mKeyHandle=NULL;
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::CreateNewKey(const char *keyname,HKEY root,bool setvolatile
			,REGSAM access)
	{
	DWORD volatileoption;
	if(setvolatile==true)
		{ volatileoption=REG_OPTION_VOLATILE; }
	else
		{ volatileoption=REG_OPTION_NON_VOLATILE;  }

	DWORD disposition=0;
	LONG ret=0;
	ret=RegCreateKeyExA(root,keyname,0,NULL,volatileoption,access
			,NULL,&mKeyHandle,&disposition);
	if(ret!=ERROR_SUCCESS)
		{
		return false;
		}
	
	return true;
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::OpenExistingKey(const char *keyname,HKEY root,REGSAM access)
	{
	LONG ret=0;
	ret=RegOpenKeyExA(root,keyname,0,access,&mKeyHandle);
	if(ret!=ERROR_SUCCESS)
		{
		return false;
		}
	
	return true;
	}


//////////////////////////////////////////////////////////////////////
MWinRegKey::MWinRegKey(void)
	{  ClearObject();  }


/////////////////////////////////////////////////////////////////////
MWinRegKey::~MWinRegKey(void)
	{  Destroy();  }


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::Create(bool create		// =true if create a fresh key
		,const char *keyname,HKEY root,bool setvolatile
		,REGSAM access) // Create/Open Key
	{
	Destroy();

	// Check if Create called
	if(create==true)
		{
		//=Create a fresh key
		if(CreateNewKey(keyname,root,setvolatile,access)==false)
			{
			Destroy();
			return false;
			}

		return true;
		}
	else
		{
		//=Open existing key
		if(OpenExistingKey(keyname,root,access)==false)
			{
			Destroy();
			return false;
			}

		return true;
		}	
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::Destroy(void)
	{
	if(mKeyHandle==NULL) { return true; }
	if(RegCloseKey(mKeyHandle)==ERROR_SUCCESS)
		{
		mKeyHandle=NULL;
		return true;
		}

	mKeyHandle=NULL;  
	return false;
	}


/////////////////////////////////////////////////////////////////////
HKEY MWinRegKey::GetKey(void)
	{
	return mKeyHandle;
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::SetValueData(const char *name,const char *data)
	{
	LONG ret=RegSetValueExA(mKeyHandle,(char *)name,0,REG_SZ
			,(CONST BYTE *)data,MStdStrLen(data)+1);
	if(ret==ERROR_SUCCESS) { return true; }
	
	return false;
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::SetValueDataExpand(const char *name,const char *data)
	{
	LONG ret=RegSetValueExA(mKeyHandle,(char *)name,0,REG_EXPAND_SZ
			,(CONST BYTE *)data,MStdStrLen(data)+1);
	if(ret==ERROR_SUCCESS) { return true; }

	return false;
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::SetValueData(const char *name,DWORD value)
	{
	LONG ret=RegSetValueExA(mKeyHandle,(char *)name,0,REG_DWORD
			,(CONST BYTE *)&value,sizeof(value));
	if(ret==ERROR_SUCCESS) { return true; }

	return false;	
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::GetValueData(const char *name,MString &outvalue)
	{
	DWORD type=0;  DWORD size=0;  LONG ret=0;

	// Get Size Now
	ret=RegQueryValueExA(mKeyHandle,name,0,&type,NULL,&size);
	if(ret!=ERROR_SUCCESS)
		{
		return false;
		}

	if(type!=REG_SZ && type!=REG_EXPAND_SZ)
		{
		outvalue.Create("");
		return false;
		}

	MBuffer buffer;
	if(buffer.Create((int)size)==false)
		{
		return false;
		}
	
	// Get Data now
	size=buffer.GetSize();
	ret=RegQueryValueExA(mKeyHandle,name,0,&type,(BYTE *)buffer.GetBuffer(),&size);
	if(ret!=ERROR_SUCCESS)
		{
		return false;
		}

	if(outvalue.Create(buffer.GetBuffer())==false)
		{
		return false;
		}
	
	return true;
	}


/////////////////////////////////////////////////////////////
bool MWinRegKey::GetValueData(const char *name,DWORD &value)
	{
	DWORD type=0;  DWORD size=sizeof(value);  LONG ret=0;

	// Get Size Now
	ret=RegQueryValueExA(mKeyHandle,name,0,&type,(BYTE *)&value,&size);
	if(ret!=ERROR_SUCCESS)
		{
		return false;
		}

	if(type!=REG_DWORD)
		{
		value=0;  return false;
		}

	return true;
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::GetValueData(const char *name,MBuffer &outvalue)
	{
	DWORD type=0;  DWORD size=0;  LONG ret=0;

	// Get Size Now
	ret=RegQueryValueExA(mKeyHandle,name,0,&type,NULL,&size);
	if(ret!=ERROR_SUCCESS)
		{
		return false;
		}

	if(type!=REG_BINARY )
		{
		outvalue.Create(1);
		return false;
		}

	if(outvalue.Create((int)size)==false)
		{
		return false;
		}
	
	// Get Data now
	size=outvalue.GetSize();
	ret=RegQueryValueExA(mKeyHandle,name,0,&type,(BYTE *)outvalue.GetBuffer(),&size);
	if(ret!=ERROR_SUCCESS)
		{
		return false;
		}

	return true;
	}



/////////////////////////////////////////////////////////////
bool MWinRegKey::DeleteValue(const char *name)
	{	
	LONG ret=RegDeleteValueA(mKeyHandle,name);
	if(ret==ERROR_SUCCESS) { return true; }

	return false;
	}


/////////////////////////////////////////////////////////////
bool MWinRegKey::GetValueTypeSize(const char *name,DWORD &type,DWORD &size)
	{
	LONG ret=0; size=0;  type=0;
	ret=RegQueryValueExA(mKeyHandle,name,0,&type,NULL,&size);
	if(ret!=ERROR_SUCCESS)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////////
bool MWinRegKey::GetSubKeyName(int index,MString &name)
	{
	MBuffer tmpname;
	if(tmpname.Create(MAX_PATH+1)==false)
		{
		return false;
		}

	DWORD tmpnamesize=tmpname.GetSize();  FILETIME ftime;

	LONG ret=RegEnumKeyExA(mKeyHandle,index,tmpname.GetBuffer(),&tmpnamesize
			,NULL,NULL,NULL,&ftime);

	if(ret==ERROR_SUCCESS)
		{  name.Create(tmpname.GetBuffer());  return true;  }

	if(ret==ERROR_MORE_DATA || ret==ERROR_NO_MORE_ITEMS) {  return false; }

	return false;
	}


/////////////////////////////////////////////////////////////////////
bool MWinRegKey::GetValueName(int index,MString &name,DWORD &type,DWORD &size)
	{
	MBuffer tmpname;
	if(tmpname.Create(1024)==false)
		{
		return false;
		}

	DWORD tmpnamesize=tmpname.GetSize()-1;
	LONG ret=::RegEnumValueA(mKeyHandle,index,tmpname,&tmpnamesize,NULL,&type,NULL,&size);
	if(ret==ERROR_SUCCESS)
		{
		name=tmpname.GetBuffer();
		return true;
		}
	
	return false;
	}


////////////////////////////////////////////////////////////////////
bool MWinRegKey::DoesNameExist(const char *name)
	{
	MString dataname;
	for(int i=0; ;++i)
		{	
		if(GetValueName(i,dataname)==false) { return false; }
		if(MStdStrICmp(dataname.Get(),name)==0) { return true; }
		}

	return false;
	}


//*************************************************************
//** Class Methods
//*************************************************************
bool MWinRegKey::Delete(const char *keyname,HKEY root)
	{
	if(::RegDeleteKeyA(root,keyname)==ERROR_SUCCESS) { return true; }
	return false;
	}


/////////////////////////////////////////////////////////
bool MWinRegKey::DeleteTree(const char *keyname,HKEY root)
	{
	if(::SHDeleteKeyA(root,keyname)==ERROR_SUCCESS) { return true; }
	return false;
	}



//////////////////////////////////////////////////////////////////
bool MWinRegKey::DoesKeyExist(const char *keyname,HKEY hive)
	{
	LONG ret=0;
	HKEY hkey=NULL;
	ret=RegOpenKeyExA(hive,keyname,0,KEY_READ,&hkey);
	if(ret!=ERROR_SUCCESS) {  return false;  }
	RegCloseKey(hkey);  return true;
	}


