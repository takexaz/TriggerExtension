#include "_TriggerExtension.h"




int regRoundtimer(MUGEN_EVAL_TRIGGER_EX* triggers) {
    // 引数がある場合は処理を行う
    return 1;
}

void* procRoundtimer(MUGEN_PLAYER* player, MUGEN_EVAL_TRIGGER_EX* triggers) {
    // 処理
    return (void*)g->roundTimer;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        struct TRX roundtimer =  {
            "roundtimer",
            0,
            regRoundtimer,
            procRoundtimer,
        };
        addTrigger(roundtimer);

        // 登録用フック
        Hook(SCtrlParseTrigger, regModTrigger);

        // 実行用フック
        writeJumpOpcode((void*)0x47ceed, procModTrigger, OP_CALL);
        BYTE procBytes[] = {
            0x85,0xC0,0x75,0x10,0xE8,0x65,0x89,
            0xF9,0xFF,0x90,0x90,0x90,0x90,0x90,
            0x90,0x90,0x90,0x90,0x90,0x90};
        writeBytesToROM((void*)0x47cef2, procBytes, sizeof(procBytes));


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
