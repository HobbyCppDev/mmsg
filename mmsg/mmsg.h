// mmsg.h
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <CommCtrl.h>                  
using namespace std;

#pragma comment(lib, "comctl32.lib")
#define IDD_PROGRESS_DIALOGA 101
#define IDC_PROGRESSA 102
#define IDC_MESSAGEA 103

#ifdef MMSG_EXPORTS
#define MMSG_API __declspec(dllexport)
#else
#define MMSG_API __declspec(dllimport)
#endif

// Dialog class with MMSG_API
int INFO_ICON = 1;
int WARNING_ICON = 2;
int ERROR_ICON = 16;

int OK = 1;
int CANCEL_OK = 2;
int YES_NO = 3;
int YES_NO_CANCEL = 4;

int NONE = 0b00000000;
int PROGRESS_BAR = 0b00000001;

/*
if (options & PROGRESS_BAR) {
    // bitwise checking
}
*/

class MMSG_API Dialog {
public:
    HWND hWnd;
    void CloseDialog();
    static Dialog NewDialog(
        const wchar_t* message,
        const wchar_t* caption,
        int buttons,
        int icon,
        void (*listener)(int result)
    );
    static Dialog NewDialog(
        const wchar_t* message,
        const wchar_t* caption,
        int buttons,
        int icon,
        void (*listener)(int result),
        int options
    );
    void UpdateProgressBar(int progress);
    void SetProgressBarRange(int range);
    Dialog();
private:
    bool hasProgressBar;
    int progress;
    Dialog(HWND hwnd, bool hasProgressBar);
};

HRESULT CALLBACK TaskDialogCallback(
    HWND hwnd,
    UINT uNotification,
    WPARAM wParam,
    LPARAM lparam,
    LONG_PTR l);