#include "_TriggerExtension.h"


// ----------------------------------------------
// RoundTimer
int regRoundtimer(MUGEN_EVAL_TRIGGER_EX* triggers, MUGEN_PLAYER_INFO* playerInfo, const char** endPtr) {
    // 引数がある場合は処理を行う
    return 1;
}

void* procRoundtimer(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect, MUGEN_EVAL_TRIGGER_EX* triggers) {
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
    const char* s = *endPtr;
    // カッコであることを確認
    if (*s == '(') {
        TRUE_RANDOM* trand = new TRUE_RANDOM;
        // カッコの次からパラメータを取得
        s += 1;
        SCtrlReadExpList(s, "ii:)", playerInfo, endPtr, &trand->min, &trand->max);
        // カッコ終了地点をカッコの次にしておく
        *endPtr += 1;
        triggers->TRX->params = trand;
    }
    else {
        return FALSE;
    }
    return TRUE;
}

#define RAND_MAX 0x7FFFFFFF
void* procTrueRandom(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect, MUGEN_EVAL_TRIGGER_EX* triggers) {
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

// ----------------------------------------------
// DisplayName = ""
struct DISPLAYNAME {
    bool isNotEqual;
    char displayname[48];
};

int regDisplayName(MUGEN_EVAL_TRIGGER_EX* triggers, MUGEN_PLAYER_INFO* playerInfo, const char** endPtr) {
    DISPLAYNAME* disp = new DISPLAYNAME;

    // 先頭アドレスをendPtrにしてパース開始地点に合わせる
    const char* s = *endPtr;
    // 文字列の前の=をパース
    int isNotE = parseIsNotEqual(&s, endPtr);
    if (isNotE == -1) return FALSE;
    disp->isNotEqual = isNotE;

    // 先頭アドレスをendPtrにして次のパース開始地点に合わせる
    s = *endPtr;
    // ""の文字列をパース
    int count = getTrigQuotedString(disp->displayname, sizeof(disp->displayname), &s, endPtr);
    if (count == -1) return FALSE;

    triggers->TRX->params = disp;
    return TRUE;
}

void* procDisplayName(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect, MUGEN_EVAL_TRIGGER_EX* triggers) {
    DISPLAYNAME* disp = (DISPLAYNAME*)triggers->TRX->params;
    bool isMatch = strcmp(disp->displayname, redirect->info->displayName) == 0 ? TRUE : FALSE;

    if (disp->isNotEqual) return (void*)!isMatch;
    return (void*)isMatch;
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

        addTrigger(TRX{
            "displayname",
            0,
            regDisplayName,
            procDisplayName,
            });

        // 登録用フック
        Hook(SCtrlParseTrigger, regModTrigger);

        // 実行用フック
        BYTE beforeProcBytes[] = { 0x56, 0x57 };
        writeBytesToROM((void*)0x47ceed, beforeProcBytes, sizeof(beforeProcBytes));
        writeJumpOpcode((void*)0x47ceef, procModTrigger, OP_CALL);
        BYTE afterProcBytes[] = {
            0x85,0xC0,0x75,0x0E,0xE8,0x63,0x89,0xF9,
            0xFF,0x90,0x90,0x90,0x90,0x90,0x90,0x90,
            0x90,0x90 };
        writeBytesToROM((void*)0x47cef4, afterProcBytes, sizeof(afterProcBytes));


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
