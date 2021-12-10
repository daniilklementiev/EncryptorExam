// Encryptor.cpp : Defines the entry point for the application.
//

#pragma comment(lib, "shlwapi.lib")
#include "framework.h"
#include "Encryptor.h"
#include <commdlg.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <CommCtrl.h>

#define MAX_LOADSTRING      100
/***************************************************************************/

#define CMD_CIPHER          1001
#define CMD_DECIPHER        1002
#define CMD_DESTINATION     1003
#define CMD_SOURCE          1004
#define CMD_STOP_BUTTON     1005

#define TIMER_FOR_PB        2001


/***************Vars***************/
HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
HWND fileSourceStatic;              // Source file 
HWND fileDestinationStatic;         // Destination source
HWND fileNameStatic;                // Static source file
HWND fileDestinationStaticSource;   // Static destination file
HWND buttonSouceEllissis;           // Button source file [...]
HWND buttonDestinationEllissis;     // Button destination file [...]
HWND passwordStatic;                // Static name "Password"
HWND editorPassword;                // Editbox password
HWND buttonCipher;                  // Cipher hwnd
HWND buttonDecipher;                // Decipher hwnd
HWND progress;                      // HWND progressbar
HWND stopButton;                    // Stop button
HWND timer;                         // Timer that creating for progress bar


/*Forward declaration - prototype*/

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
/*************************************************************/
DWORD CALLBACK OpenFileClick(LPVOID);
DWORD CALLBACK OpenDestination(LPVOID);
DWORD CALLBACK CreatingWindow(LPVOID);
DWORD CALLBACK Cipher(LPVOID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ENCRYPTOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENCRYPTOR));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENCRYPTOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(255, 228, 196));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ENCRYPTOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
       ((GetSystemMetrics(SM_CXSCREEN)) - 400) / 2, ((GetSystemMetrics(SM_CYSCREEN)) - 270) / 2, 400, 270, 
       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:
        if (wParam == TIMER_FOR_PB)
        {
            SendMessageW(progress, PBM_DELTAPOS, 1, 0);
        }
        break;
    case WM_CREATE: {
        CreatingWindow(&hWnd);
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
           

            case CMD_CIPHER: {
                CreateThread(NULL, 0, Cipher, &hWnd, 0, NULL);
                break;
            }
            case CMD_SOURCE: {
                CreateThread(NULL, 0, OpenFileClick, &hWnd, 0, NULL);
                break;
            }
            case CMD_DESTINATION: {
                CreateThread(NULL, 0, OpenDestination, &hWnd, 0, NULL);
                break;
            }
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// creating main window 
DWORD CALLBACK CreatingWindow(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    // Static name source file
    fileSourceStatic = CreateWindowExW(0, L"Static", L"Source file",
        WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT,
        10, 10, 75, 23, hWnd, (HMENU)0, hInst, NULL);
    // Static name destination file
    fileDestinationStatic = CreateWindowExW(0, L"Static", L"Destination file",
        WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT,
        10, 70, 100, 23, hWnd, (HMENU)0, hInst, NULL);
    // Source file
    fileNameStatic = CreateWindowExW(0, L"Static", L" ",
        WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT,
        10, 40, 300, 23, hWnd, (HMENU)0, hInst, NULL);
    // Souce destination file
    fileDestinationStaticSource = CreateWindowExW(0, L"Static", L" ",
        WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT,
        10, 100, 300, 23, hWnd, (HMENU)0, hInst, NULL);
    // Static password
    passwordStatic = CreateWindowExW(0, L"Static", L"Password",
        WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT,
        10, 130, 70, 23, hWnd, (HMENU)0, hInst, NULL);
    // Editbox password
    editorPassword = CreateWindowExW(0, L"Edit", L"****************",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        85, 130, 100, 23, hWnd, (HMENU)0, hInst, NULL);
    // Button Cipher 
    buttonCipher = CreateWindowExW(0, L"Button", L"Cipher",
        WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON,
        10, 165, 75, 23, hWnd, (HMENU)CMD_CIPHER, hInst, NULL);
    // Button Decipher 
    buttonDecipher = CreateWindowExW(0, L"Button", L"Decipher",
        WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON,
        90, 165, 95, 23, hWnd, (HMENU)CMD_DECIPHER, hInst, NULL);
    // Button opener source file
    buttonSouceEllissis = CreateWindowExW(0, L"Button", L"...",
        WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON,
        260, 10, 50, 23, hWnd, (HMENU)CMD_SOURCE, hInst, NULL);
    // Button opener source destination file
    buttonSouceEllissis = CreateWindowExW(0, L"Button", L"...",
        WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON,
        260, 70, 50, 23, hWnd, (HMENU)CMD_DESTINATION, hInst, NULL);
    // Progressbar
    progress = CreateWindowExW(0, PROGRESS_CLASS, L"",
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_VERTICAL,
        330, 10, 30, 150, hWnd, 0, hInst, NULL);
    // Stop button
    stopButton =  CreateWindowExW(0, L"Button", L"Stop",
        WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON,
        320, 165, 50, 23, hWnd, (HMENU)CMD_STOP_BUTTON, hInst, NULL);

    SendMessageW(progress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    SendMessageW(progress, PBM_SETSTEP, 25, 0);
    SendMessageW(progress, PBM_DELTAPOS, 0, 0);
    SendMessageW(progress, PBM_SETBARCOLOR, 0, RGB(0, 255, 0));

    return 0;
}

HANDLE file;
OPENFILENAMEW ofn; // source file structure

// Open source file
DWORD CALLBACK OpenFileClick(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    // killfocus button
    SendMessageW(buttonSouceEllissis, WM_KILLFOCUS, 0, 0);
    WCHAR fname[512] = L"Hello.txt\0";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    ofn.lpstrFile = fname;
    ofn.nMaxFile = 512;
    ofn.nMaxFileTitle = sizeof(fname);
    ofn.lpstrFilter = L"Text files\0*.txt;*.docx;*.pdf\0All files\0*.*\0\0";
    ofn.nFilterIndex = 1;
    if (GetOpenFileNameW(&ofn)) {

        PathStripPath(ofn.lpstrFile);
        
        file = CreateFileW(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (file == 0)
        {
            SendMessageW(fileNameStatic, WM_SETTEXT, 0, (LPARAM)L"File open error");
        }
        else {
            DWORD fSize;
            fSize = GetFileSize(file, NULL);
            if (fSize > 0)
            {
                SendMessageW(fileNameStatic, WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
            }
            else {
                SendMessageW(fileNameStatic, WM_SETTEXT, 0, (LPARAM)"File is empty");
            }
            CloseHandle(file);
        }
    }
    else {
        SendMessageW(fileNameStatic, WM_SETTEXT, 0, (LPARAM)L"Selection cancelled");
    }
    return 0;
}

OPENFILENAMEW ofndest; // destination file structure
// Open destination file
DWORD CALLBACK OpenDestination(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    // killfocus button
    SendMessageW(buttonDestinationEllissis, WM_KILLFOCUS, 0, 0);
    WCHAR fname[512] = L"\0";
    
    
    ZeroMemory(&ofndest, sizeof(ofndest));
    ofndest.lStructSize = sizeof(ofndest);
    ofndest.hwndOwner = hWnd;
    ofndest.hInstance = hInst;
    ofndest.lpstrFile = fname;
    ofndest.nMaxFile = 512;
    ofndest.nMaxFileTitle = sizeof(fname);
    ofndest.lpstrFilter = L"Text files\0*.txt;*.docx;*.pdf\0All files\0*.*\0\0";
    ofndest.nFilterIndex = 1;
    if (GetOpenFileNameW(&ofndest)) {
        
        PathStripPath(ofndest.lpstrFile);

        file = CreateFileW(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (file == 0)
        {
            SendMessageW(fileDestinationStaticSource, WM_SETTEXT, 0, (LPARAM)L"File open error");
        }
        else {
            DWORD fSize;
            fSize = GetFileSize(file, NULL);
            if (fSize > 0)
            {
                // Set text to fileNameStatic
                int resp = MessageBoxW(hWnd, L"File already exist. Do you want to save here?", L"Warning", MB_YESNO | MB_ICONWARNING);
                if (resp == IDYES)
                {
                    SendMessageW(fileDestinationStaticSource, WM_SETTEXT, 0, (LPARAM)ofndest.lpstrFile);
                }
                else if (resp == IDNO) {
                    
                    
                    
                    
                }
            }
            else {
                SendMessageW(fileDestinationStaticSource, WM_SETTEXT, 0, (LPARAM)L"File is empty");
            }
            CloseHandle(file);
        }
    }
    else {
        SendMessageW(fileDestinationStaticSource, WM_SETTEXT, 0, (LPARAM)L"Selection cancelled");
    }
    return 0;
}

DWORD CALLBACK SaveFileClick(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    WCHAR fname[512];
    char content[1024] = { '\0' };
    // достаем контент из едитера
    //SendMessageA(editor, WM_GETTEXT, 1024, (LPARAM)content);
    fname[0] = '\0';
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    ofn.lpstrFile = fname;
    ofn.nMaxFile = 512;
    ofn.nMaxFileTitle = sizeof(fname);
    if (GetOpenFileNameW(&ofn)) {
        // ставим текст в статик с имени файла
        //SendMessageW(fNameStatic, WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
        file = CreateFileW(fname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (file == 0)
        {
            //SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)L"File open error");
            return -1;
        }
        else
        {
            DWORD written;
            if (WriteFile(file, content, strnlen_s(content, 1024), &written, NULL)) {
                if (written != strnlen_s(content, 1024))
                {
                   // SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)L"Error saving");
                    WCHAR buff[512];
                    _snwprintf_s(buff, 512, L"written: %d", written);
                    //SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)buff);

                }
                else {
                    WCHAR buff[512];
                    _snwprintf_s(buff, 512, L"written: %d", written);
                    //SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)buff);
                }
            }
            else {
               // SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)L"Error saving");
            }
            CloseHandle(file);
        }
    }
    return 0;
}

// Function of checking password
DWORD CALLBACK EnteringPassword(LPVOID params) {
    WCHAR thisPassword[512];
    return 0;
}

DWORD CALLBACK Cipher(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    SendMessageW(buttonCipher, WM_KILLFOCUS, 0, 0);
    SetTimer(hWnd, TIMER_FOR_PB, 100, NULL);

    return 0;
}