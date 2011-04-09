// Win32Lib.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifdef _WIN32

#include "Win32Lib.h"
#include "../EScript/EScript.h"

#include <windows.h>

using namespace EScript;

//! (static)
void Win32Lib::init(EScript::Namespace * globals) {
	Namespace * lib=new Namespace();
	declareConstant(globals,"Win32",lib);

	//! [ESF]	void setClipboard( string )
	ESF_DECLARE(lib,"setClipboard",1,1,(Win32Lib::setClipboard(parameter[0].toString()),Void::get()))

	//! [ESF]	string getClipboard( )
	ESF_DECLARE(lib,"getClipboard",0,0,String::create(Win32Lib::getClipboard()))

	//! [ESF]	bool loadLibrary(string )
	ES_FUNCTION_DECLARE(lib,"loadLibrary",1,1, {
		HINSTANCE hDLL;
		libInitFunction *  f;    // Function pointer

		hDLL = LoadLibrary(parameter[0]->toString().c_str());
		if (hDLL == NULL)
			return Bool::create(false);

		f = reinterpret_cast<libInitFunction*>(GetProcAddress(hDLL,"init"));
		if (!f)    {
			// handle the error
			FreeLibrary(hDLL);
			return Bool::create(false);
		}
		EScript::initLibrary(f);

		return Bool::create(true);
	})

}

//! (static)
void Win32Lib::setClipboard(const std::string & s){
	if (OpenClipboard(NULL)) {
		EmptyClipboard();
		HGLOBAL hClipboardData;
		hClipboardData = GlobalAlloc(GMEM_DDESHARE, s.size()+1);
		// strData.GetLength()+1);
		char * pchData;
		pchData = reinterpret_cast<char*>(GlobalLock(hClipboardData));

		strncpy(pchData, s.c_str(),s.size());// LPCSTR(strData));
		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_TEXT,hClipboardData);
		CloseClipboard();
	} else std::cout << "Could not open Clipboard!\n";
}

//! (static)
std::string Win32Lib::getClipboard(){
	if (!OpenClipboard(NULL)) {
		std::cout << "Could not open Clipboard!\n";
		return "";
	}

	HGLOBAL hClipboardData;
	char * lpstr;

	hClipboardData=GetClipboardData(CF_TEXT);
	lpstr=reinterpret_cast<char*>(GlobalLock(hClipboardData));

	std::string s=std::string(lpstr);
	//std::cout << s;
	// ...

	GlobalUnlock(hClipboardData);
	CloseClipboard();
	return s;
}

#endif
