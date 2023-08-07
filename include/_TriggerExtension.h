#pragma once
#include <Mebius.h>
#include "TriggerExtension.h"
using namespace std;

extern vector<TRX> gTriggerList;
static int TRIGGERID = 0x7FFFFFFF;

int findTargetStateByName(string);
int regModTrigger(void** , int);
int __stdcall procModTrigger(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect);
void freeModTrigger(void**);

int countSpace(const char* s);
int parseRedirect(MUGEN_PLAYER_INFO* playerInfo, MUGEN_EVAL_TRIGGER_EX* triggers, const char** startPtr, const char** endPtr);
string anyCharToString(const char* str, size_t count);