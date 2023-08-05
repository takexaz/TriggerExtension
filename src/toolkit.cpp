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