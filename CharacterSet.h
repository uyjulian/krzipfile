
#ifndef __CharacterSet_H__
#define __CharacterSet_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <objidl.h>
#include "tp_stub.h"

#define tjs_string std::wstring

extern bool TVPUtf8ToUtf16( tjs_string& out, const std::string& in );
extern bool TVPUtf16ToUtf8( std::string& out, const tjs_string& in );

#endif
