#ifdef _WIN32

#include "Win32Lib.h"
#include "../EScript/EScript.h"

#include <windows.h>

using namespace EScript;

/*!	[ESF]	void setClipboard( string )	*/
ESF(esf_setClipboard,1,1,(Win32Lib::setClipboard(parameter[0].toString()),Void::get()))

/*!	[ESF]	string getClipboard( )	*/
ESF(esf_getClipboard,0,0,String::create(Win32Lib::getClipboard()))

/*!	[ESF]	bool loadLibrary(string )	*/
ES_FUNCTION(esf_loadLibrary) {
    assertParamCount(runtime,parameter.count(),1,1);

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
}

// ---------------------------------------------------------

void Win32Lib::init(EScript::Namespace * globals) {
    Namespace * lib=new Namespace();
    declareConstant(globals,"Win32",lib);
    declareFunction(lib,"setClipboard",esf_setClipboard);
    declareFunction(lib,"getClipboard",esf_getClipboard);
    declareFunction(lib,"loadLibrary",esf_loadLibrary);
//    Object * sys=o->getAttribute("System");
//    if(!sys) {
//        sys=new Object();
//        o->setAttribute("System",sys);
//    }

}

/*!	(static) */
void Win32Lib::setClipboard(const std::string & s){
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        HGLOBAL hClipboardData;
        hClipboardData = GlobalAlloc(GMEM_DDESHARE, s.size()+1);
        // strData.GetLength()+1);
        char * pchData;
        pchData = reinterpret_cast<char*>(GlobalLock(hClipboardData));

        strcpy(pchData, s.c_str());// LPCSTR(strData));
        GlobalUnlock(hClipboardData);
        SetClipboardData(CF_TEXT,hClipboardData);
        CloseClipboard();
    } else std::cout << "Could not open Clipboard!\n";
}

/*!	(static) */
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
