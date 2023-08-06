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