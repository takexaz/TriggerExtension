#include "_TriggerExtension.h"

void addTrigger(TRX trx) {
    transform(trx.trigger.begin(), trx.trigger.end(), trx.trigger.begin(), ::tolower);
    if (findTargetTriggerByName(trx.trigger) == -1) {
        gTriggerList.push_back(trx);
    }
    return;
}

int countSpace(const char* s) {
    int count = 0;
    while (*s != '\0') {
        if (!isspace(*s)) {
            break;
        }
        ++s;
        ++count;
    }
    return count;
}

int getLabel(const char** startPtr, const char** endPtr) {
    // 先頭アドレスが空白であれば空白をスキップ
    *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // 空白のみで終端についたら文字列なしとしてリターン
    if (*s == '\0') return 0;

    int cnt = 0;
    int cntsp = 0;
    // ラベル文字数をカウントしてendPtrを最後の位置にする
    while (*s != '\0') {
        // アルファベットか.ならカウント++してカウント+=空白カウント
        if (isalpha(*s) || isdigit(*s) || *s == '.') {
            cnt += cntsp;
            cntsp = 0;
            cnt++;
        }
        // 空白なら空白カウント++
        else if (*s == ' ') {
            cntsp++;
        }
        // 記号なら終了
        else {
            break;
        }
        s++;
    }
    *endPtr = s;
    return cnt;
}

int parseRedirect(MUGEN_PLAYER_INFO* playerInfo, MUGEN_EVAL_TRIGGER_EX* triggers, const char **startPtr, const char** endPtr) {
    // ラベルを取得
    int count = getLabel(startPtr, endPtr);
    string redirect;
    redirect.assign(*startPtr, count);
    const char* s = *startPtr = *endPtr;

    if (!stricmp(redirect.c_str(), "parent")) {
        triggers->redirectID = ID_RD_Parent;
    }
    else if (!stricmp(redirect.c_str(), "root")) {
        triggers->redirectID = ID_RD_Root;
    }
    else if (!stricmp(redirect.c_str(), "helper")) {
        if (*s == '(') {
            s++;
            SCtrlReadExpList(s, "i:)", playerInfo, endPtr, &triggers->redirectArg);
            *startPtr = *endPtr += 1;
            triggers->redirectID = ID_RD_Helper;
        }
        else {
            return FALSE;
        }
    }
    else if (!stricmp(redirect.c_str(), "target")) {
        if (*s == '(') {
            s++;
            SCtrlReadExpList(s, "i:)", playerInfo, endPtr, &triggers->redirectArg);
            *startPtr = *endPtr += 1;
            triggers->redirectID = ID_RD_Target;
        }
        else {
            triggers->redirectID = ID_RD_Target;
        }
    }
    else if (!stricmp(redirect.c_str(), "partner")) {
        triggers->redirectID = ID_RD_Partner;
    }
    else if (!stricmp(redirect.c_str(), "enemy")) {
        if (*s == '(') {
            s++;
            SCtrlReadExpList(s, "i:)", playerInfo, endPtr, &triggers->redirectArg);
            *startPtr = *endPtr += 1;
            triggers->redirectID = ID_RD_Enemy;
        }
        else {
            triggers->redirectID = ID_RD_Enemy;
        }
    }
    else if (!stricmp(redirect.c_str(), "enemynear")) {
        if (*s == '(') {
            s++;
            SCtrlReadExpList(s, "i:)", playerInfo, endPtr, &triggers->redirectArg);
            *startPtr = *endPtr += 1;
            triggers->redirectID = ID_RD_EnemyNear;
        }
        else {
            triggers->redirectID = ID_RD_EnemyNear;
        }
    }
    else if (!stricmp(redirect.c_str(), "playerid")) {
        if (*s == '(') {
            s++;
            SCtrlReadExpList(s, "i:)", playerInfo, endPtr, &triggers->redirectArg);
            *startPtr = *endPtr += 1;
            triggers->redirectID = ID_RD_PlayerID;
        }
        else {
            return FALSE;
        }
    }
    else {
        *endPtr = *startPtr;
        triggers->redirectID = ID_RD_None;
        return TRUE;
    }

    // 後ろが,であることを確認
    count = getLabel(startPtr, endPtr);
    if (!count && **startPtr == ',') {
        *startPtr = *endPtr += 1;
        return TRUE;
    }
    return FALSE;
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
    int isredirect = parseRedirect(playerInfo, triggers, &startPtr, endPtr);

    // トリガー名パース
    int trigscnt = getLabel(&startPtr, endPtr);

    // Stringに変換
    string lowtrig;
    lowtrig.assign(startPtr, trigscnt);

    // 小文字化
    transform(lowtrig.begin(), lowtrig.end(), lowtrig.begin(), ::tolower);
    int index = findTargetTriggerByName(lowtrig);

    if (index != -1) {
        //Modトリガーだった場合
        //スタックの整理
        MUGEN_EVAL_TYPE* types = (MUGEN_EVAL_TYPE*)*(stack + 3);
        int maxLength = (int)*(stack + 4);

        triggers->TRX = new MEBIUS_TRX_DATA;
        // triggers->TRX->params ユーザーにパラメータの処理させる

        auto reg = reinterpret_cast<int (*)(MUGEN_EVAL_TRIGGER_EX*, MUGEN_PLAYER_INFO*, const char **)>(gTriggerList[index].reg);
        RETVALUE = reg(triggers, playerInfo, endPtr);
        // 仮にリダイレクト0固定
        triggers->trigID = (MUGEN_TRIG_ID)TRIGGERID;
        triggers->isFloat = 0; // ユーザーに決めさせる
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

void freeModState(void** stack) {

}