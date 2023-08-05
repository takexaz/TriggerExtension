#pragma once
#include<WinMugen.h>
#include <Windows.h>
#include <filesystem>
using namespace std;

struct TRX {
    string trigger = "";
    int isFloat = 0;
    int (*reg)(MUGEN_EVAL_TRIGGER_EX* triggers) = NULL;
    void* (*proc)(MUGEN_PLAYER* player, MUGEN_EVAL_TRIGGER_EX* triggers) = NULL;
};




#ifdef TREX_EXPORT
#define CLASS_DECLSPECEC extern "C" __declspec(dllexport)
#define CLASS_DECLSPEC __declspec(dllexport)
#else
#define CLASS_DECLSPECEC extern "C" __declspec(dllimport)
#define CLASS_DECLSPEC __declspec(dllimport)
#endif

CLASS_DECLSPECEC void addTrigger(TRX trx);
CLASS_DECLSPECEC void setErrorText(const char* error);
CLASS_DECLSPECEC int clamp(int value, int minimum, int maximum);