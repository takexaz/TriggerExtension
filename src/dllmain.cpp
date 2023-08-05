#include "_TriggerExtension.h"


// ----------------------------------------------
// RoundTimer
int regRoundtimer(MUGEN_EVAL_TRIGGER_EX* triggers, MUGEN_PLAYER_INFO* playerInfo, const char** endPtr) {
    // 引数がある場合は処理を行う
    return 1;
}

void* procRoundtimer(MUGEN_PLAYER* player, MUGEN_EVAL_TRIGGER_EX* triggers) {
    // 処理
    return (void*)g->roundTimer;
}
// ----------------------------------------------
// ----------------------------------------------

// ----------------------------------------------
// TrueRandom(min,max)
struct TRUE_RANDOM {
    MUGEN_EVAL_VALUE min, max;
};

int regTrueRandom(MUGEN_EVAL_TRIGGER_EX* triggers, MUGEN_PLAYER_INFO* playerInfo, const char** endPtr) {
    // 引数がある場合は処理を行う
    TRUE_RANDOM* trand = new TRUE_RANDOM;
    const char* s = *endPtr + 1;
    SCtrlReadExpList(s, "ii:)", playerInfo, endPtr, &trand->min, &trand->max);
    ++*endPtr;
    triggers->TRX->params = trand;
    return 1;
}

#define RAND_MAX 0x7FFFFFFF
void* procTrueRandom(MUGEN_PLAYER* player, MUGEN_EVAL_TRIGGER_EX* triggers) {
    TRUE_RANDOM* trand = (TRUE_RANDOM*)triggers->TRX->params;
    int min = EvalExpressionN(player, &trand->min, 0);
    int max = EvalExpressionN(player, &trand->max, 0);

    struct timespec ts;
    if (!timespec_get(&ts, TIME_UTC)) {
        srand(ts.tv_nsec ^ ts.tv_sec);
    }
    // minからmaxまでの範囲のランダムな整数を生成
    int random_number = min + rand() % (max - min + 1);

    return (void*)random_number;
}
// ----------------------------------------------
// ----------------------------------------------


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        addTrigger(TRX{
            "roundtimer",
            0,
            regRoundtimer,
            procRoundtimer,
        });

        addTrigger(TRX{
            "truerandom",
            0,
            regTrueRandom,
            procTrueRandom,
        });

        // 登録用フック
        Hook(SCtrlParseTrigger, regModTrigger);

        // 実行用フック
        writeJumpOpcode((void*)0x47ceed, procModTrigger, OP_CALL);
        BYTE procBytes[] = {
            0x85,0xC0,0x75,0x10,0xE8,0x65,0x89,
            0xF9,0xFF,0x90,0x90,0x90,0x90,0x90,
            0x90,0x90,0x90,0x90,0x90,0x90};
        writeBytesToROM((void*)0x47cef2, procBytes, sizeof(procBytes));


        LoadAllDLL("mods", ".trx");
        break;
    }
    case DLL_THREAD_ATTACH: {
        break;
    }
    case DLL_THREAD_DETACH: {
        break;
    }
    case DLL_PROCESS_DETACH: {
        FreeAllDLL("mods", ".trx");
        break;
    }
    }
    return TRUE;
}
