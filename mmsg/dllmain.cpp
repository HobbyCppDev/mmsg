// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <CommCtrl.h>                  
#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <memory>
#include <chrono>
#include <vector>
using namespace std;

#pragma comment(lib, "comctl32.lib") // use version six of this, not five
#include "mmsg.h"

HINSTANCE hInstance;       

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugString(L"DLL_PROCESS_ATTACH called");
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_PROGRESS_CLASS | ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
        InitCommonControlsEx(&icex);

        ::hInstance = hInstance;

        return TRUE;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    ; // dummy function, we dont use it but still need it
}

/*

MMSG_API INT_PTR CALLBACK PercentDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hProgressBar;

    switch (uMsg) {
    case WM_INITDIALOG:
        hProgressBar = GetDlgItem(hwndDlg, IDC_PROGRESSA);
        SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            EndDialog(hwndDlg, IDOK);
            return TRUE;
        }
        break;
    }

    return FALSE;
}

MMSG_API Dialog::Dialog(HWND hwnd, float& percent) : hWnd(hwnd), percent(percent) {}

MMSG_API Dialog Dialog::NewPercentDialog(float& percent, const wchar_t* message, const wchar_t* caption) {
    HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PROGRESS_DIALOGA), NULL, PercentDialogProc);
    if (!hDlg) {
        DWORD error = GetLastError();
        std::cout << error << std::endl;
        std::cout << MAKEINTRESOURCE(IDD_PROGRESS_DIALOGA) << std::endl;
    }
    ShowWindow(hDlg, SW_SHOW);

    SendMessage(GetDlgItem(hDlg, IDC_MESSAGEA), WM_SETTEXT, 0, (LPARAM)message);
    return Dialog(hDlg, percent);
}

MMSG_API void Dialog::CloseDialog() {
    EndDialog(hWnd, 0);
}

*/ 

bool buttons = true;
HWND g_hTaskDialog;
bool useDefaultTimer = true;
HWND g_hButton;

BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam) {
    HWND hWnd = FindWindowEx(hwndChild, NULL, L"Button", L"Button to be hidden");
    if (hWnd) {
        // FOUND!
        g_hButton = hWnd;
        return FALSE;
    }
    return TRUE;
}

HRESULT CALLBACK TaskDialogCallback(
    HWND hwnd, 
    UINT uNotification, 
    WPARAM wParam, 
    LPARAM lparam,
    LONG_PTR l) {
    if (uNotification == TDN_CREATED) {
        g_hTaskDialog = hwnd;
    }
    if (uNotification == TDN_DIALOG_CONSTRUCTED) { //created and all dialog items created according to docs
        if (!buttons) {
            SetTimer(hwnd, 1, 2500, NULL);
            OutputDebugString(L"Hello");
        }
        else useDefaultTimer = false;
    }
    if (uNotification == TDN_TIMER) {
        OutputDebugString(L"Hello1");
        if (useDefaultTimer) {
            OutputDebugString(L"Attempting to hide buttons");
            HWND hWndButton = FindWindowEx(hwnd, NULL, L"Button", L"Button to be hidden");
            if (hWndButton) {
                /*                ShowWindow(hWndButton, SW_HIDE);
                OutputDebugString(L"Hiding Button\n");
                KillTimer(hwnd, 1);
                useDefaultTimer = false;*/

            }
            else {
                HWND hWndDirectUI = FindWindowEx(hwnd, NULL, L"DirectUIHWND", L"");
                if (hWndDirectUI) {
                    EnumChildWindows(hWndDirectUI, EnumChildProc, (LPARAM)L"Button to be hidden");
                }
                if (g_hButton) {
                    OutputDebugString(L"It works!");
                    ShowWindow(g_hButton, SW_HIDE);
                    OutputDebugString(L"Hiding Button\n");
                    KillTimer(hwnd, 1);
                    useDefaultTimer = false;
                }
            }
        }

    }
    return S_OK;
}

MMSG_API Dialog Dialog::NewDialog(
    const wchar_t* message,
    const wchar_t* caption,
    int buttons,
    int icon,
    void (*listener)(int result)
) {
    return Dialog::NewDialog(
        message,
        caption,
        buttons,
        icon,
        listener,
        NONE
    );
}

MMSG_API Dialog Dialog::NewDialog(
    const wchar_t* message,
    const wchar_t* caption,
    int buttons,
    int icon,
    void (*listener)(int result),
    int options
) {


    TASKDIALOGCONFIG tdc = { 0 };
    tdc.cbSize = sizeof(TASKDIALOGCONFIG);
    tdc.hwndParent = NULL;
    tdc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    tdc.pszWindowTitle = caption;
    switch (icon) {
    case 1:
        tdc.pszMainIcon = TD_INFORMATION_ICON;
        break;
    case 2:
        tdc.pszMainIcon = TD_WARNING_ICON;
        break;
    case 0b00000000:
        tdc.pszMainIcon = 0;
        break;
    case 16:
        tdc.pszMainIcon = TD_ERROR_ICON;
        break;
    default:
        tdc.pszMainIcon = TD_INFORMATION_ICON;
        break;
    }
    tdc.pszContent = message;
    int defaultn = IDOK;
    vector<TASKDIALOG_BUTTON> buttonsa;

    bool NO_BUTTONS = false;
    switch (buttons) {
    case 1:
        buttonsa.push_back({ IDOK, L"OK" });
        break;
    case 2:
        buttonsa.push_back({ IDOK, L"OK" });
        buttonsa.push_back({ IDCANCEL, L"Cancel" });
        break;
    case 3:
        buttonsa.push_back({ IDYES, L"Yes" });
        buttonsa.push_back({ IDNO, L"No" });
        defaultn = IDYES;
        break;
    case 4:
        buttonsa.push_back({ IDYES, L"Yes" });
        buttonsa.push_back({ IDNO, L"No" });
        buttonsa.push_back({ IDCANCEL, L"Cancel" });
        defaultn = IDYES;
        break;
    case 0b00000000:
        buttonsa.push_back({ 1637, L"Button to be hidden" });
        defaultn = 1637;
        NO_BUTTONS = true;
        ::buttons = false;
        break;
    default:
        buttonsa.push_back({ IDOK, L"OK" });
        break;
    }

    tdc.pButtons = buttonsa.data();
    tdc.cButtons = static_cast<UINT>(buttonsa.size());

    if (NO_BUTTONS) {
        tdc.dwFlags = TDF_CALLBACK_TIMER;
    }

    bool hasProgressBar = false;

    //#region "options region"
    if (options & PROGRESS_BAR) {
        tdc.dwFlags += TDF_SHOW_PROGRESS_BAR;
        hasProgressBar = true;
    }
    //#end region

    tdc.nDefaultButton = defaultn;
    tdc.pfCallback = TaskDialogCallback;

    HWND hWnd;
    int nClickedButton=0;
    HRESULT hr=TaskDialogIndirect(&tdc, &nClickedButton, NULL, NULL);
    hWnd=g_hTaskDialog;
    g_hTaskDialog = NULL;
    if (SUCCEEDED(hr)) {listener(nClickedButton);}
    else{cout<<GetLastError()<<endl;}
    Sleep(1);
    return Dialog::Dialog(hWnd, hasProgressBar);
}

MMSG_API Dialog::Dialog() {
    hWnd = nullptr;
}

MMSG_API Dialog::Dialog(HWND hwnd, bool hasProgressBarA) {
    hWnd = hwnd;
    hasProgressBar = hasProgressBarA;
}

MMSG_API void Dialog::UpdateProgressBar(int progress) {
    if (hasProgressBar) {
        this->progress = progress;
        SendMessage(this->hWnd, TDM_SET_PROGRESS_BAR_POS, 0, this->progress);
    }
    else {
        throw new exception("Not implemented");
    }
}

MMSG_API void Dialog::SetProgressBarRange(int range) {
    if (hasProgressBar) {
        SendMessage(this->hWnd, TDM_SET_PROGRESS_BAR_RANGE, 0, range);
    }
}

MMSG_API void Dialog::CloseDialog() {
    SendMessage(hWnd, WM_CLOSE, 0, 0);
}