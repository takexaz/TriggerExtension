#include "_TriggerExtension.h"


void addTrigger(TRX trx) {
    transform(trx.trigName.begin(), trx.trigName.end(), trx.trigName.begin(), ::tolower);
    if (findTargetStateByName(trx.trigName) == -1) {
        gTriggerList.push_back(trx);
    }
    return;
}

int regModTrigger(void** stack, int RETVALUE) {
    if (RETVALUE != 0) return RETVALUE;

    MUGEN_EVAL_TRIGGER_EX* triggers = new MUGEN_EVAL_TRIGGER_EX;
    MUGEN_EVAL_TRIGGER_EX** trigptr = (MUGEN_EVAL_TRIGGER_EX**)*(stack + 2);
    MUGEN_PLAYER_INFO* playerInfo = (MUGEN_PLAYER_INFO*)*(stack + 1);
    *trigptr = triggers;

    const char** endPtr = (const char**)*(stack + 5);
    const char* startPtr = (const char*)*(stack);
    // リダイレクトパース
    if (parseRedirect(playerInfo, triggers, &startPtr, endPtr) == FALSE) {
        return FALSE;
    }

    // トリガー名パース
    int trigscnt = getLabel(&startPtr, endPtr);
    if (trigscnt == -1) return FALSE;

    // Stringに変換
    string lowtrig = anyCharToString(startPtr, trigscnt);

    // 小文字化
    transform(lowtrig.begin(), lowtrig.end(), lowtrig.begin(), ::tolower);
    int index = findTargetStateByName(lowtrig);

    if (index != -1) {
        //Modトリガーだった場合
        //スタックの整理
        setErrorText("");
        MUGEN_EVAL_TYPE* types = (MUGEN_EVAL_TYPE*)*(stack + 3);
        int maxLength = (int)*(stack + 4);

        triggers->TRX = new MEBIUS_TRX_DATA;
        // triggers->TRX->params ユーザーにパラメータの処理させる

        auto reg = reinterpret_cast<int (*)(MUGEN_EVAL_TRIGGER_EX*, MUGEN_PLAYER_INFO*, const char **)>(gTriggerList[index].reg);
        RETVALUE = reg(triggers, playerInfo, endPtr);

        triggers->trigID = (MUGEN_TRIG_ID)TRIGGERID;
        triggers->compareStyle = EVAL_EQ;
        triggers->TRX->extrID = index;
        *(types) = EVAL_TYPE_TRIGGER;
    }
    return RETVALUE;
}

int __stdcall procModTrigger(MUGEN_PLAYER* player, MUGEN_PLAYER* redirect) {
    void** stack;
    _asm {
        MOV stack, EBP
        ADD stack, 0x50
    };

    void** value = (void**)&(*(stack));
    MUGEN_EVAL_TRIGGER_EX* triggers = (MUGEN_EVAL_TRIGGER_EX * )*(stack + 1);

    //MUGEN_PLAYER* enemynear = (MUGEN_PLAYER*)*(stack - 10);
    //MUGEN_PLAYER* p4 = (MUGEN_PLAYER*)*(stack - 9); // P4Name用
    //MUGEN_PLAYER* p3 = (MUGEN_PLAYER*)*(stack - 8); // P3Name用
    //MUGEN_PLAYER* team_first = (MUGEN_PLAYER*)*(stack - 7); // Power用
    //bool projecontact = *(stack - 6); // 0x47cff7にcmpあり　projcontactに関係(0x0047bb8f) by Choon氏
    //MUGEN_PLAYER* enemy1 = (MUGEN_PLAYER*)*(stack - 5);
    //MUGEN_PLAYER* root = (MUGEN_PLAYER*)*(stack - 4);
    //MUGEN_EVAL_COMPARATOR* compOper = (MUGEN_EVAL_COMPARATOR*)*(stack - 3);
    //MUGEN_PLAYER* parent = (MUGEN_PLAYER*)*(stack - 2);

    auto proc = reinterpret_cast<void* (*)(MUGEN_PLAYER * , MUGEN_PLAYER* ,MUGEN_EVAL_TRIGGER_EX*)>(gTriggerList[triggers->TRX->extrID].proc);
    *value = (void*)proc(player, redirect, triggers);

    return TRUE;
}

void freeModTrigger(void** stack) {
    MUGEN_EVAL_TRIGGER_EX* triggers = (MUGEN_EVAL_TRIGGER_EX*)*(stack);
    if (triggers->trigID != 0x7FFFFFFF) return;
    auto free = reinterpret_cast<void* (*)(MUGEN_EVAL_TRIGGER_EX*)>(gTriggerList[triggers->TRX->extrID].free);
    free(triggers);
    return;
}