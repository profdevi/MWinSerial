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


//v1.0 copyright Comine.com 20150912S1207
#include "MStdLib.h"
#include "MWinRegKey.h"
#include "MString.h"
#include "MBuffer.h"
#include "MWinSerial.h"


//******************************************************
//**  Module Elements
//******************************************************
static const HKEY GRootHive=HKEY_LOCAL_MACHINE;
static const char GCurrentVersionKey[]="SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";


//******************************************************
//**  MWinSerial class
//******************************************************
void MWinSerial::ClearObject(void)
	{
	}


////////////////////////////////////////////////
MWinSerial::MWinSerial(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinSerial::~MWinSerial(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWinSerial::Create(void)
	{
	Destroy();

	// Open Key
	MWinRegKey prodname;
	if(prodname.Create(false,GCurrentVersionKey,GRootHive,false,KEY_READ|KEY_WOW64_64KEY)==false)
		{
		Destroy();
		return false;
		}

	if(prodname.GetValueData("ProductName",mProductName)==false)
		{
		Destroy();
		return false;
		}

	if(prodname.GetValueData("ProductId",mProductID)==false)
		{
		Destroy();
		return false;
		}

	MBuffer digitalproductid;
	if(prodname.GetValueData("DigitalProductId",digitalproductid)==false)
		{
		Destroy();
		return false;
		}

	// Ref code translation from vsb to C++ from online source. 
	// The original code was not written by me.
	// Begin : Translated Code
	MBuffer keyoutput(200);
	keyoutput.SetString("");

	const int keyoffset=52;
	const char *chardata="BCDFGHJKMPQRTVWXY2346789";

	unsigned int iswin8;
	iswin8= (((unsigned char)(digitalproductid[66]) / 6) & 1);
	digitalproductid[66] = ( ((unsigned char) digitalproductid[66] & 0xf7) | ((iswin8 & 2) *4) );

	unsigned int last=0;
	for(int i=24;i>=0;--i)
		{
		int cur=0;
		for(int x=14;x>=0;--x)
			{
			cur = cur*256;
			cur=(*(unsigned char *)(digitalproductid+x+keyoffset))+cur;
			digitalproductid[x+keyoffset] = cur / 24;
			cur=cur % 24;
			}

		keyoutput.CharPrepend(chardata[cur]);
		last=cur;
		}

	if(iswin8==1)
		{
		MBuffer keypart1;
		keyoutput.GetSubString(1,last,keypart1);
		MStdStrInsert(keyoutput.GetBuffer(),keyoutput.GetSize(),last+1,"N");
		keyoutput.StrRemove(1);
		if(last==0) { keyoutput.CharPrepend('N');  }			
		}

	MBuffer abuf,bbuf,cbuf,dbuf,ebuf;
	keyoutput.GetSubString(0,5,abuf);
	keyoutput.GetSubString(5, 5, bbuf);
	keyoutput.GetSubString(10, 5, cbuf);
	keyoutput.GetSubString(15, 5, dbuf);
	keyoutput.GetSubString(20, 5, ebuf);
	
	MBuffer finalkey(100);
	finalkey.SetString("");
	finalkey.StringAppend(abuf.GetBuffer());
	finalkey.StringAppend("-");
	finalkey.StringAppend(bbuf.GetBuffer());
	finalkey.StringAppend("-");
	finalkey.StringAppend(cbuf.GetBuffer());
	finalkey.StringAppend("-");
	finalkey.StringAppend(dbuf.GetBuffer());
	finalkey.StringAppend("-");
	finalkey.StringAppend(ebuf.GetBuffer());
	// End : Transalted Code

	if(mSerialKey.Create(finalkey.GetBuffer())==false)
		{
		Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinSerial::Destroy(void)
	{
	mProductName.Destroy();
	mProductID.Destroy();
	mSerialKey.Destroy();
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
const char *MWinSerial::GetProductName(void)
	{
	return mProductName.Get();
	}


////////////////////////////////////////////////
const char *MWinSerial::GetProductID(void)
	{
	return mProductID.Get();
	}


/////////////////////////////////////////////////
const char *MWinSerial::GetSerialNumber(void)
	{
	return mSerialKey.Get();
	}


