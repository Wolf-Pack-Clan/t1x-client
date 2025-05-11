#ifndef T1X_CORE_H
#define T1X_CORE_H

#ifdef BUILDING_T1X_DLL
#define T1X_API __declspec(dllexport)
#else
#define T1X_API __declspec(dllimport)
#endif

extern "C" T1X_API DWORD address_cgame_mp;
extern "C" T1X_API DWORD address_ui_mp;

#endif