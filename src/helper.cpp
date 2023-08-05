#include "_TriggerExtension.h"

void addTrigger(TRX trx) {
    transform(trx.trigger.begin(), trx.trigger.end(), trx.trigger.begin(), ::tolower);
    if (findTargetTriggerByName(trx.trigger) == -1) {
        gTriggerList.push_back(trx);
    }
    return;
}

int parseTrigger(const char* s) {
    int cnt = 0;
    int cntsp = 0;
    while (*s != '\0') {
        // �󔒂Ȃ�󔒃J�E���g++
        if (*s == ' ') {
            cntsp++;
        // �A���t�@�x�b�g��.�Ȃ�J�E���g++���ăJ�E���g+=�󔒃J�E���g
        } else if (isalpha(*s) || isdigit(*s) || *s == '.') {
            cnt += cntsp;
            cntsp = 0;
            cnt++;
        }
        // �󔒁A�A���t�@�x�b�g�ȊO�Ȃ�I��
        else {
            break;
        }
        s++;
    }
    return cnt;
}

int regModTrigger(void** stack, int RETVALUE) {
    if (RETVALUE != 0) return RETVALUE;

    // �g���K�[���p�[�X
    const char* s = (const char*)*(stack);
    int trigscnt = parseTrigger(s);

    // String�ɕϊ�
    string lowtrig;
    lowtrig.assign(s, trigscnt);

    // ��������
    transform(lowtrig.begin(), lowtrig.end(), lowtrig.begin(), ::tolower);
    int index = findTargetTriggerByName(lowtrig);

    if (index != -1) {
        //Mod�g���K�[�������ꍇ
        //�X�^�b�N�̐���
        MUGEN_PLAYER_INFO* playerInfo = (MUGEN_PLAYER_INFO*)*(stack + 1);
        MUGEN_EVAL_TRIGGER_EX* triggers = new MUGEN_EVAL_TRIGGER_EX;
        MUGEN_EVAL_TRIGGER_EX** trigptr = (MUGEN_EVAL_TRIGGER_EX**)*(stack + 2);
        *trigptr = triggers;
        MUGEN_EVAL_TYPE* types = (MUGEN_EVAL_TYPE*)*(stack + 3);
        int maxLength = (int)*(stack + 4);
        const char** endPtr = (const char**)*(stack + 5);
        *endPtr = s + trigscnt;

        triggers->TRX = new MEBIUS_TRX_DATA;
        // triggers->TRX->params ���[�U�[�Ƀp�����[�^�̏���������
        *(types) = EVAL_TYPE_TRIGGER;

        auto reg = reinterpret_cast<int (*)(MUGEN_EVAL_TRIGGER_EX*)>(gTriggerList[index].reg);
        RETVALUE = reg(triggers);

        triggers->trigID = (MUGEN_TRIG_ID)TRIGGERID;
        triggers->isFloat = 0; // ���[�U�[�Ɍ��߂�����
        triggers->redirectID = ID_RD_None;
        triggers->compareStyle = EVAL_EQ;
        triggers->TRX->extrID = index;

    }
    return RETVALUE;
}

int __stdcall procModTrigger(void) {
    void** stack;
    _asm {
        MOV stack, EBP
        ADD stack, 0x48
    };

    void** value = (void**)&(*(stack));
    MUGEN_EVAL_TRIGGER_EX* triggers = (MUGEN_EVAL_TRIGGER_EX * )*(stack + 1);
    MUGEN_PLAYER* player = (MUGEN_PLAYER*)*(stack - 16);
    MUGEN_PLAYER* team_first = (MUGEN_PLAYER*)*(stack - 7);
    MUGEN_PLAYER* partner = (MUGEN_PLAYER*)*(stack - 8);
    MUGEN_PLAYER* enemy1 = (MUGEN_PLAYER*)*(stack - 10);
    MUGEN_PLAYER* enemy2 = (MUGEN_PLAYER*)*(stack - 9);

    auto proc = reinterpret_cast<void* (*)(MUGEN_PLAYER * , MUGEN_EVAL_TRIGGER_EX*)>(gTriggerList[triggers->TRX->extrID].proc);
    *value = (void*)proc(player, triggers);

    return TRUE;
}

void freeModState(void** stack) {

}