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
    // �擪�A�h���X���󔒂ł���΋󔒂��X�L�b�v
    *endPtr = *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // �󔒂݂̂ŏI�[�ɂ�����G���[�Ƃ��ă��^�[��
    if (*s == '\0') return -1;

    // =�܂Ń��[�v
    while (*s != '\0') if (*s == '=') {
        break;
    }
    else {
        ++s;
    }
    *endPtr = s + 1;
    // =���Ȃ��܂܏I�[�ɂ�����G���[�Ƃ��ă��^�[��
    if (*s == '\0') return -1;

    // !����Ȃ�Not�ł���
    bool isNot = FALSE;
    --s;
    while (*s == '!') {
        isNot = !isNot;
        --s;
    }

    return isNot;
}

int getTrigQuotedString(char* dest, size_t size, const char** startPtr, const char** endPtr) {
    // �擪�A�h���X���󔒂ł���΋󔒂��X�L�b�v
    *endPtr = *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // �󔒂݂̂ŏI�[�ɂ�����G���[
    if (*s == '\0') return -1;
    // �_�u���N�H�[�e�[�V�����łȂ���΃G���[
    if (*s != '\"') return -1;
    ++s;
    const char* qs = s;

    int count = 0;
    // "�܂Ń��[�v
    while (*s != '\0') if (*s == '\"') {
        break;
    }
    else {
        ++count;
        ++s;
    }
    *endPtr = s + 1;
    // "���Ȃ��܂܏I�[�ɂ�����G���[�Ƃ��ă��^�[��
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
    // �擪�A�h���X���󔒂ł���΋󔒂��X�L�b�v
    *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // �󔒂݂̂ŏI�[�ɂ�����G���[�Ƃ��ă��^�[��
    if (*s == '\0') return -1;

    int cnt = 0;
    int cntsp = 0;
    // ���x�����������J�E���g����endPtr���Ō�̈ʒu�ɂ���
    while (*s != '\0') {
        // �A���t�@�x�b�g��.�Ȃ�J�E���g++���ăJ�E���g+=�󔒃J�E���g
        if (isalpha(*s) || isdigit(*s) || *s == '.') {
            cnt += cntsp;
            cntsp = 0;
            cnt++;
        }
        // �󔒂Ȃ�󔒃J�E���g++
        else if (*s == ' ') {
            cntsp++;
        }
        // �L���Ȃ�I��
        else {
            break;
        }
        s++;
    }
    *endPtr = s;
    return cnt;
}

int parseRedirect(MUGEN_PLAYER_INFO* playerInfo, MUGEN_EVAL_TRIGGER_EX* triggers, const char** startPtr, const char** endPtr) {
    // ���x�����擾
    int count = getLabel(startPtr, endPtr);
    if (count == -1) return FALSE;
    string redirect = anyCharToString(*startPtr, count);

    // ���_�C���N�g�p�[�X
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

    // �����`�F�b�N
    const char* next = *endPtr;
    switch (triggers->redirectID)
    {
    case ID_RD_None:
        // ���_�C���N�g�ƈ�v���Ȃ���ΏI��
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

    // ��낪,�ł��邱�Ƃ��m�F
    *startPtr = *endPtr += countSpace(*endPtr);
    if (**startPtr == ',') {
        *startPtr = *endPtr += 1;
        return TRUE;
    }
    return FALSE;
}

bool parseCompare(MUGEN_EVAL_COMPARATOR* comp, const char** startPtr, const char** endPtr) {
    // �擪�A�h���X���󔒂ł���΋󔒂��X�L�b�v
    *endPtr = *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // �󔒂݂̂ŏI�[�ɂ�����G���[
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