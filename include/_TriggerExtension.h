#pragma once
#include <Mebius.h>
#include "TriggerExtension.h"
using namespace std;

extern vector<TRX> gTriggerList;
static int TRIGGERID = 0x7FFFFFFF;

int findTargetTriggerByName(string);
int regModTrigger(void** , int);
int __stdcall procModTrigger(void);
void freeModState(void**);
