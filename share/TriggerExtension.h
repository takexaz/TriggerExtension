#pragma once
#include<WinMugen.h>
#include <Windows.h>
#include <filesystem>
using namespace std;

struct TRX {
    string trigName = "";
    int (*reg)(MUGEN_EVAL_TRIGGER_EX* triggers, MUGEN_PLAYER_INFO* playerInfo, const char** endPtr) = NULL;
    value32_t (*proc)(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect, MUGEN_EVAL_TRIGGER_EX* triggers) = NULL;
    void (*free)(MUGEN_EVAL_TRIGGER_EX* triggers) = NULL;
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

CLASS_DECLSPEC void constExpN(MUGEN_EVAL_VALUE* eval, int value);
CLASS_DECLSPEC void constExpN(MUGEN_EVAL_VALUE* eval, float value);
CLASS_DECLSPEC int EvalExpressionN(MUGEN_PLAYER* player, MUGEN_EVAL_VALUE* eval, int warnNo);
CLASS_DECLSPEC float EvalExpressionN(MUGEN_PLAYER* player, MUGEN_EVAL_VALUE* eval);
CLASS_DECLSPEC MUGEN_EVAL_TYPE EvalExpressionN(MUGEN_PLAYER* player, MUGEN_EVAL_VALUE* eval, int* pInt, float* pFloat);

CLASS_DECLSPECEC int parseIsNotEqual(const char** startPtr, const char** endPtr);
CLASS_DECLSPECEC bool parseRange(MUGEN_EVAL_COMPARATOR* comp, const char** startPtr, const char** endPtr);
CLASS_DECLSPECEC int getTrigQuotedString(char* dest, size_t size, const char** startPtr, const char** endPtr);
CLASS_DECLSPECEC int getLabel(const char** startPtr, const char** endPtr);