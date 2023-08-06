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
    // �擪�A�h���X���󔒂ł���΋󔒂��X�L�b�v
    *startPtr += countSpace(*startPtr);
    const char* s = *startPtr;
    // �󔒂݂̂ŏI�[�ɂ����當����Ȃ��Ƃ��ă��^�[��
    if (*s == '\0') return 0;

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

int parseRedirect(MUGEN_PLAYER_INFO* playerInfo, MUGEN_EVAL_TRIGGER_EX* triggers, const char **startPtr, const char** endPtr) {
    // ���x�����擾
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

    // ��낪,�ł��邱�Ƃ��m�F
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
    // ���_�C���N�g�p�[�X
    int isredirect = parseRedirect(playerInfo, triggers, &startPtr, endPtr);

    // �g���K�[���p�[�X
    int trigscnt = getLabel(&startPtr, endPtr);

    // String�ɕϊ�
    string lowtrig;
    lowtrig.assign(startPtr, trigscnt);

    // ��������
    transform(lowtrig.begin(), lowtrig.end(), lowtrig.begin(), ::tolower);
    int index = findTargetTriggerByName(lowtrig);

    if (index != -1) {
        //Mod�g���K�[�������ꍇ
        //�X�^�b�N�̐���
        MUGEN_EVAL_TYPE* types = (MUGEN_EVAL_TYPE*)*(stack + 3);
        int maxLength = (int)*(stack + 4);

        triggers->TRX = new MEBIUS_TRX_DATA;
        // triggers->TRX->params ���[�U�[�Ƀp�����[�^�̏���������

        auto reg = reinterpret_cast<int (*)(MUGEN_EVAL_TRIGGER_EX*, MUGEN_PLAYER_INFO*, const char **)>(gTriggerList[index].reg);
        RETVALUE = reg(triggers, playerInfo, endPtr);
        // ���Ƀ��_�C���N�g0�Œ�
        triggers->trigID = (MUGEN_TRIG_ID)TRIGGERID;
        triggers->isFloat = 0; // ���[�U�[�Ɍ��߂�����
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
    //MUGEN_PLAYER* p4 = (MUGEN_PLAYER*)*(stack - 9); // P4Name�p
    //MUGEN_PLAYER* p3 = (MUGEN_PLAYER*)*(stack - 8); // P3Name�p
    //MUGEN_PLAYER* team_first = (MUGEN_PLAYER*)*(stack - 7); // Power�p
    //bool projecontact = *(stack - 6); // 0x47cff7��cmp����@projcontact�Ɋ֌W(0x0047bb8f) by Choon��
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