#include "_TriggerExtension.h"


// ----------------------------------------------
// RoundTimer
int regRoundtimer(MUGEN_EVAL_TRIGGER_EX* triggers, MUGEN_PLAYER_INFO* playerInfo, const char** endPtr) {
    // 返す値を設定(int)
    triggers->isFloat = FALSE;
    return TRUE;
}

value32_t procRoundtimer(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect, MUGEN_EVAL_TRIGGER_EX* triggers) {
    // 処理
    value32_t roundtimer;
    roundtimer.i = g->roundTimer;
    return roundtimer;
}

void freeRoundtimer(MUGEN_EVAL_TRIGGER_EX* triggers) {
    return;
}
// ----------------------------------------------
// ----------------------------------------------

// ----------------------------------------------
// TrueRandom(min,max)
struct TRUE_RANDOM {
    MUGEN_EVAL_VALUE min, max;
};

int regTrueRandom(MUGEN_EVAL_TRIGGER_EX* triggers, MUGEN_PLAYER_INFO* playerInfo, const char** endPtr) {
    // 初期設定 返す値(int)
    triggers->isFloat = FALSE;
    const char* s = *endPtr;

    // ランダム用の構造体作成
    TRUE_RANDOM* trand = new TRUE_RANDOM;
    triggers->TRX->params = trand;

    // カッコでなければエラー
    if (*s != '(') return FALSE;

    // カッコの次からパラメータを取得
    s += 1;

    // 取得＆取得数が2でなければエラー
    if(SCtrlReadExpList(s, "ii:)", playerInfo, endPtr, &trand->min, &trand->max) != 2) return FALSE;

    return TRUE;
}

#define RAND_MAX 0x7FFFFFFF
value32_t procTrueRandom(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect, MUGEN_EVAL_TRIGGER_EX* triggers) {
    TRUE_RANDOM* trand = (TRUE_RANDOM*)triggers->TRX->params;
    int min = EvalExpressionN(player, &trand->min, 0);
    int max = EvalExpressionN(player, &trand->max, 0);

    struct timespec ts;
    if (!timespec_get(&ts, TIME_UTC)) {
        srand(ts.tv_nsec ^ ts.tv_sec);
    }
    // minからmaxまでの範囲のランダムな整数を生成
    value32_t random_number;
    random_number.i = min + rand() % (max - min + 1);

    return random_number;
}

void freeTrueRandom(MUGEN_EVAL_TRIGGER_EX* triggers) {
    TRUE_RANDOM* trand = (TRUE_RANDOM*)triggers->TRX->params;
    FreeExpression(&trand->max);
    FreeExpression(&trand->min);
    delete trand;
    return;
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
    // 初期設定 返す値(bool)
    triggers->isFloat = FALSE;
    const char* s = *endPtr;

    DISPLAYNAME* disp = new DISPLAYNAME;
    triggers->TRX->params = disp;

    // 比較演算子をパース
    int isNotE = parseIsNotEqual(&s, endPtr);
    // パースに失敗したらエラー
    if (isNotE == -1) return FALSE;
    disp->isNotEqual = isNotE;

    // 先頭アドレスをendPtrにして次のパース開始地点に合わせる
    s = *endPtr;
    // 文字列をパース＆失敗したらエラー
    if (getTrigQuotedString(disp->displayname, sizeof(disp->displayname), &s, endPtr) == -1) return FALSE;

    return TRUE;
}

value32_t procDisplayName(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect, MUGEN_EVAL_TRIGGER_EX* triggers) {
    DISPLAYNAME* disp = (DISPLAYNAME*)triggers->TRX->params;
    value32_t isMatch;
    isMatch.b = strcmp(disp->displayname, redirect->info->displayName) == 0 ? TRUE : FALSE;
    if (disp->isNotEqual) isMatch.b = !isMatch.b;
    return isMatch;
}

void freeDisplayName(MUGEN_EVAL_TRIGGER_EX* triggers) {
    DISPLAYNAME* disp = (DISPLAYNAME*)triggers->TRX->params;
    delete disp;
    return;
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
            regRoundtimer,
            procRoundtimer,
            freeRoundtimer,
        });

        addTrigger(TRX{
            "truerandom",
            regTrueRandom,
            procTrueRandom,
            freeTrueRandom,
        });

        addTrigger(TRX{
            "displayname",
            regDisplayName,
            procDisplayName,
            freeDisplayName,
            });

        // SCtrlReadExpListのバグ修復
        BYTE fixReadExpBytes[] = { 
            0x31,0xC0,0x8B,0x54,0x24,0x20,0x38,0x0A,
            0x75,0x6C,0xFF,0x44,0x24,0x20,0xEB,0x0A,
            0x90,0x90,0x90,0x90 };
        writeBytesToROM((void*)0x47d900, fixReadExpBytes, sizeof(fixReadExpBytes));

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

        // 解放用フック
        Hook(TriggerFree, freeModTrigger);

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
