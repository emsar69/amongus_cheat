#include <Windows.h>
#include <thread>

#include "gui.h"
#include "hooks.h"
#include "logs.h"

void Setup(HMODULE hModule) {

    try {
        gui::Setup();
        hooks::Setup();
        logger::Init();
    }
    catch (const std::exception& error) {
        MessageBoxA(NULL, error.what(), "Error", MB_OK | MB_ICONWARNING);
        goto UNLOAD;
    }

    while (!GetAsyncKeyState(VK_END))
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

UNLOAD:
    hooks::Destroy();
    gui::Destroy();
    logger::Destroy();

    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);
        HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Setup, hModule, 0, NULL);
        if (thread) CloseHandle(thread);
    }
    case DLL_PROCESS_DETACH: {

    }
        break;
    }
    return TRUE;
}

