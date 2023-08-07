#include "_TriggerExtension.h"

void constExpN(MUGEN_EVAL_VALUE* eval, int value) {
    ConstExpI(eval, value);
    return;
}

void constExpN(MUGEN_EVAL_VALUE* eval, float value) {
    ConstExpF(eval, value);
    return;
}

int EvalExpressionN(MUGEN_PLAYER* player, MUGEN_EVAL_VALUE* eval, int warnNo) {
    return EvalExpressionI(player, eval, warnNo);
}

float EvalExpressionN(MUGEN_PLAYER* player, MUGEN_EVAL_VALUE* eval) {
    return EvalExpressionF(player, eval);
}

MUGEN_EVAL_TYPE EvalExpressionN(MUGEN_PLAYER* player, MUGEN_EVAL_VALUE* eval, int* pInt, float* pFloat) {
    return EvalExpression(player, eval, pInt, pFloat);
}

int GetQuotedStringEx(TPFILE* tpf, const char* label, const char* dest, size_t size) {
    auto GetQuotedStringWrap = reinterpret_cast<int (*)(TPFILE * wtpf, const char* wlabel, const char* wdest, int wsize, int zero)>(0x415df0);
    return GetQuotedStringWrap(tpf, label, dest, size - 1, 0);
}

void setErrorText(const char* error) {
    char* mugen_error = (char*)*((DWORD*)0x4b5b4c) + 0xC534;
    snprintf(mugen_error, 1023, "%s", error);
    return;
}

int clamp(int value, int minimum, int maximum) {
    return min(max(minimum, value), maximum);
}

int parseIsNotEqual(const char** startPtr, const char** endPtr) {
    // 先頭アドレスが空白であれば空白をスキップ
    *endPtr = *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // 空白のみで終端についたらエラーとしてリターン
    if (*s == '\0') return -1;

    // =までループ
    while (*s != '\0') if (*s == '=') {
        break;
    }
    else {
        ++s;
    }
    *endPtr = s + 1;
    // =がないまま終端についたらエラーとしてリターン
    if (*s == '\0') return -1;

    // !が奇数ならNotである
    bool isNot = FALSE;
    --s;
    while (*s == '!') {
        isNot = !isNot;
        --s;
    }

    return isNot;
}

int getTrigQuotedString(char* dest, size_t size, const char** startPtr, const char** endPtr) {
    // 先頭アドレスが空白であれば空白をスキップ
    *endPtr = *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // 空白のみで終端についたらエラー
    if (*s == '\0') return -1;
    // ダブルクォーテーションでなければエラー
    if (*s != '\"') return -1;
    ++s;
    const char* qs = s;

    int count = 0;
    // "までループ
    while (*s != '\0') if (*s == '\"') {
        break;
    }
    else {
        ++count;
        ++s;
    }
    *endPtr = s + 1;
    // "がないまま終端についたらエラーとしてリターン
    if (*s == '\0') return -1;

    count = min(count, size - 1);
    strncpy((char*)dest, qs, count);
    dest[count] = '\0';
    return count;
}

string anyCharToString(const char* str, size_t count) {
    string stri;
    stri.assign(str, count);
    return stri;
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
    // 空白のみで終端についたらエラーとしてリターン
    if (*s == '\0') return -1;

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

int parseRedirect(MUGEN_PLAYER_INFO* playerInfo, MUGEN_EVAL_TRIGGER_EX* triggers, const char** startPtr, const char** endPtr) {
    // ラベルを取得
    int count = getLabel(startPtr, endPtr);
    if (count == -1) return FALSE;
    string redirect = anyCharToString(*startPtr, count);

    // リダイレクトパース
    if (!stricmp(redirect.c_str(), "parent"))      triggers->redirectID = ID_RD_Parent;
    else if (!stricmp(redirect.c_str(), "root"))        triggers->redirectID = ID_RD_Root;
    else if (!stricmp(redirect.c_str(), "helper"))      triggers->redirectID = ID_RD_Helper;
    else if (!stricmp(redirect.c_str(), "target"))      triggers->redirectID = ID_RD_Target;
    else if (!stricmp(redirect.c_str(), "partner"))     triggers->redirectID = ID_RD_Partner;
    else if (!stricmp(redirect.c_str(), "enemy"))       triggers->redirectID = ID_RD_Enemy;
    else if (!stricmp(redirect.c_str(), "enemynear"))   triggers->redirectID = ID_RD_EnemyNear;
    else if (!stricmp(redirect.c_str(), "playerid"))    triggers->redirectID = ID_RD_PlayerID;
    else {
        triggers->redirectID = ID_RD_None;
    }

    // 引数チェック
    const char* next = *endPtr;
    switch (triggers->redirectID)
    {
    case ID_RD_None:
        // リダイレクトと一致しなければ終了
        *endPtr = *startPtr;
        return TRUE;
    case ID_RD_Parent:
    case ID_RD_Root:
    case ID_RD_Partner:
        break;
    case ID_RD_Helper:
    case ID_RD_PlayerID:
        if (*next != '(') return FALSE;
        break;
    case ID_RD_Target:
    case ID_RD_Enemy:
    case ID_RD_EnemyNear:
        if (*next == '(') {
            next++;
            if (SCtrlReadExpList(next, "i:)", playerInfo, endPtr, &triggers->redirectArg) != 1) return FALSE;
        }
        break;
    case ID_RD_Extension:
        break;
    default:
        break;
    }

    // 後ろが,であることを確認
    *startPtr = *endPtr += countSpace(*endPtr);
    if (**startPtr == ',') {
        *startPtr = *endPtr += 1;
        return TRUE;
    }
    return FALSE;
}

bool parseCompare(MUGEN_EVAL_COMPARATOR* comp, const char** startPtr, const char** endPtr) {
    // 先頭アドレスが空白であれば空白をスキップ
    *endPtr = *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // 空白のみで終端についたらエラー
    if (*s == '\0') return FALSE;

    switch (*s) {
    case '=':
        *comp = EVAL_EQ;
        break;
    case '!':
        if (*(s + 1) == '=') {
            return FALSE;
        }
        else {
            *comp = EVAL_NEQ;
            *endPtr += 1;
        }
        break;
    case '>':
        if (*(s + 1) == '=') {
            *comp = EVAL_GTE;
            *endPtr += 1;
        }
        else {
            *comp = EVAL_GT;
        }
        break;
    case '<':
        if (*(s + 1) == '=') {
            *comp = EVAL_LTE;
            *endPtr += 1;
        }
        else {
            *comp = EVAL_LT;
        }
        break;
    default:
        return FALSE;
    }
    s = *endPtr += 1;
}