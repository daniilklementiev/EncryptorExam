#include "framework.h"
#include "Encryptor.h"


#define MAX_LOADSTRING      100

/****************My definitions****************/

#define CMD_CIPHER          1001
#define CMD_DECIPHER        1002
#define CMD_DESTINATION     1003
#define CMD_SOURCE          1004
#define CMD_STOP_BUTTON     1005
#define CMD_EDIT_PASS       1006
#define TIMER_FOR_PB        2001

typedef char (*crypto_t)(char, char);

/***************Vars***************/
HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];  

/********************************************/

HWND fileSourceStatic;               // Source file 
HWND fileDestinationStatic;         // Destination source
HWND fileNameStatic;                // Static source file
HWND fileDestinationStaticSource;   // Static destination file
HWND buttonSouceEllissis;           // Button source file [...]
HWND buttonDestinationEllipssis;     // Button destination file [...]
HWND passwordStatic;                // Static name "Password"
HWND editorPassword;                // Editbox password
HWND buttonCipher;                  // Cipher hwnd
HWND buttonDecipher;                // Decipher hwnd
HWND progress;                      // HWND progressbar
HWND stopButton;                    // Stop button
HWND timer;                         // Timer that creating for progress bar

HWND source;
HWND dest;
BOOL stop = FALSE;
HMODULE dll;
HANDLE mutex;


char sourceName[512] = "Source.txt";
char destName[512] = "Dest.txt";

/*Forward declarations*/

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*************************************************************/

DWORD CALLBACK OpenSource(LPVOID);
DWORD CALLBACK DestinationFileClick(LPVOID);
DWORD CALLBACK CreatingWindow(LPVOID);
DWORD CALLBACK Cipher(LPVOID);
DWORD CALLBACK Decipher(LPVOID);
DWORD CALLBACK StopButton(LPVOID);
/*************************************************************/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ENCRYPTOR, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);
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
    wcex.hbrBackground  = CreateSolidBrush(RGB(255, 248, 220));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ENCRYPTOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

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
        mutex = CreateMutex(NULL, FALSE, NULL);
        if (mutex == NULL) {
            MessageBoxW(NULL, L"Mutex not created", L"APP STOPPED", MB_ICONWARNING | MB_OK);
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            int notificationCode = HIWORD(wParam);
            if (notificationCode == EN_SETFOCUS) {
                 SendMessageW(editorPassword, WM_SETTEXT, 0, (LPARAM)L"");
                 break;
            }

            // Parse the menu selections:
            switch (wmId)
            {
            case CMD_CIPHER: {
                CreateThread(NULL, 0, Cipher, &hWnd, 0, NULL);
                break;
            }
            case CMD_DECIPHER: {
                CreateThread(NULL, 0, Decipher, &hWnd, 0, NULL);
                break;
            }
            case CMD_STOP_BUTTON: {
                CreateThread(NULL, 0, StopButton, &hWnd, 0, NULL);
                break;
            }
            case CMD_SOURCE: {
                CreateThread(NULL, 0, OpenSource, &hWnd, 0, NULL);
                break;
            }
            case CMD_DESTINATION: {
                CreateThread(NULL, 0, DestinationFileClick, &hWnd, 0, NULL);
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




/************************************************************/


DWORD CALLBACK CreatingWindow(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    dll = LoadLibraryW(L"Cipher_DLL.dll");
    WaitForSingleObject(mutex, INFINITE);
    if (dll == NULL) {

        if (MessageBoxW(NULL, L"?????? ?? ?? ????? ???????????", L"?????????? ?? ???????", MB_YESNO | MB_ICONERROR) == IDYES) {

            char dllPath[512] = "\0";

            OPENFILENAMEA ofn;
            ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
            ofn.lStructSize = sizeof(OPENFILENAMEA);
            ofn.hwndOwner = hWnd;
            ofn.hInstance = hInst;
            ofn.lpstrFile = dllPath;
            ofn.nMaxFile = 512;

            if (GetOpenFileNameA(&ofn)) {
                dll = LoadLibraryA(dllPath);
            }
            else {
                SendMessageA(dest, WM_SETTEXT, 0, (LPARAM)L"DLL open error");
            }
        }
    }
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
    editorPassword = CreateWindowExW(0, L"Edit", L"**********************",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
        85, 130, 135, 23, hWnd, (HMENU)0, hInst, NULL);
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
    buttonDestinationEllipssis = CreateWindowExW(0, L"Button", L"...",
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
    Release(mutex);
    return 0;
}

DWORD CALLBACK OpenSource(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    WaitForSingleObject(mutex, INFINITE);
    SendMessageW(buttonSouceEllissis, WM_KILLFOCUS, 0, 0);

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    ofn.lpstrFile = sourceName;
    ofn.nMaxFile = 512;


    if (GetOpenFileNameA(&ofn)) {


        SendMessageA(fileNameStatic, WM_SETTEXT, 0, (LPARAM)sourceName);
    }
    else {

        SendMessageW(fileNameStatic, WM_SETTEXT, 0, (LPARAM)L"Selection cancelled");

    }

    Release(mutex);
    return 0;
}

DWORD CALLBACK DestinationFileClick(LPVOID params) {
    WaitForSingleObject(mutex, INFINITE);
    HWND hWnd = *((HWND*)params);
    //const size_t len = strlen(destName) + 1;
    WCHAR fName[512] = L"\0";
    //mbstowcs(fName, destName, len);
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    ofn.lpstrFile = destName;
    ofn.nMaxFile = 512;

    if (GetOpenFileNameA(&ofn)) {
        HANDLE hFile = CreateFileA(destName, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == 0) {
            SendMessageW(fileDestinationStaticSource, WM_SETTEXT, 0, (LPARAM)L"File save error");
        }
        else {
            int idMB = 0;
            if (GetFileSize(hFile, NULL) > 0) {

                WCHAR fNameSource[512] = L"\0";
                SendMessageW(fileNameStatic, WM_GETTEXT, sizeof(WCHAR) * 512, (LPARAM)fNameSource);
                if (wcscmp(fNameSource, fName) == 0) {
                    MessageBoxW(hWnd, L"?????? ???????? ????", L"??????", MB_OK | MB_ICONERROR);
                    CloseHandle(hFile);
                    return 0;
                }
                else {
                    idMB = MessageBoxW(hWnd, L"???????????? ???? ?????", L"??????????????", MB_YESNO | MB_ICONINFORMATION);
                    if (idMB == IDNO) {
                        SendMessageW(fileDestinationStaticSource, WM_SETTEXT, 0, (LPARAM)L"Cancelled");
                        CloseHandle(hFile);
                        return 0;
                    }
                }
            }
            SendMessageA(fileDestinationStaticSource, WM_SETTEXT, 0, (LPARAM)destName);
        }
        CloseHandle(hFile);

    }
    else {
        SendMessageW(fileDestinationStaticSource, WM_SETTEXT, 0, (LPARAM)L"Selection cancelled");
    }
    Release(mutex);
    return 0;
}

DWORD CALLBACK Cipher(LPVOID params) {
    crypto_t cipher = (crypto_t)GetProcAddress(dll, "Cipher");
    WaitForSingleObject(mutex, INFINITE);
    if (cipher == NULL)
    {
        MessageBoxW(NULL, L"Cipher method not found", L"Error", MB_ICONERROR | MB_OK);
        FreeLibrary(dll);
    }

    if (dll == 0) {
        MessageBoxW(NULL, L"Error loading DLL", L"DLL Error", MB_ICONERROR | MB_OK);
    }
    else {
        Button_Enable(buttonSouceEllissis, FALSE);
        Button_Enable(buttonDestinationEllipssis, FALSE);
        Button_Enable(buttonCipher, FALSE);
        Button_Enable(buttonDecipher, FALSE);
        if (GetWindowTextLengthA(editorPassword) > 16) {

            MessageBoxA(NULL, "Password should be less than 16 characters", "Password is too big", MB_ICONERROR | MB_OK);
        }
        else if (GetWindowTextLengthA(editorPassword) == 0) {
            MessageBoxA(NULL, "Enter password", "Error", MB_ICONERROR | MB_OK);
        }
        else {
            FILE* sourceFile;
            FILE* destFile;
            sourceFile = fopen(sourceName, "r+");
            destFile = fopen(destName, "w+");

            char pass[16];
            SendMessageA(editorPassword, WM_GETTEXT, 0, (LPARAM)pass);

            int readed = 0;
            char buffer[1];

            int fSize = 0;
            fseek(sourceFile, 0, SEEK_END);

            fSize = ftell(sourceFile);

            fseek(sourceFile, 0, SEEK_SET);

            SendMessageW(progress, PBM_SETRANGE, 0, MAKELPARAM(0, fSize));
            SendMessageW(progress, PBM_SETBARCOLOR, 0, RGB(0, 255, 0));


            while (fread(buffer, sizeof(char), 1, sourceFile) == 1) {
                if (stop == TRUE)
                {
                    fclose(sourceFile);
                    fclose(destFile);
                    Sleep(2000);
                    SendMessageW(progress, PBM_SETPOS, 0, 0);
                    MessageBoxW(NULL, L"Stopped Ciphering", L"Stopped", MB_OK | MB_ICONERROR);
                    break;
                }
                else {
                    ++readed;
                    SendMessageW(progress, PBM_DELTAPOS, 1, 0);
                    buffer[0] = cipher(buffer[0], pass[readed % strlen(pass)]);

                    fwrite(buffer, sizeof(char), 1, destFile);
                    Sleep(100);
                }
            }
            if (stop == FALSE)
            {
                MessageBoxW(NULL, L"Successful Ciphering", L"Success", MB_OK | MB_ICONASTERISK);
            }
            stop = FALSE;
            fclose(sourceFile);
            fclose(destFile);
            Sleep(2000);
            SendMessageW(progress, PBM_SETPOS, 0, 0);
            SendMessageW(progress, PBM_SETBARCOLOR, 0, RGB(0, 255, 0));
            SendMessageW(editorPassword, WM_SETTEXT, 0, (LPARAM)L"*********************");
            
        }
    }
    Button_Enable(buttonSouceEllissis, TRUE);
    Button_Enable(buttonDestinationEllipssis, TRUE);
    Button_Enable(buttonCipher, TRUE);
    Button_Enable(buttonDecipher, TRUE);
    Release(mutex);
    return 0;
}

DWORD CALLBACK Decipher(LPVOID params) {
    WaitForSingleObject(mutex, INFINITE);
    crypto_t decipher = (crypto_t)GetProcAddress(dll, "Decipher");
    if (decipher == NULL)
    {
        MessageBoxW(NULL, L"Decipher method not found", L"Error", MB_ICONERROR | MB_OK);
        FreeLibrary(dll);
    }

    if (dll == 0) {
        MessageBoxW(NULL, L"Error loading DLL", L"DLL Error", MB_ICONERROR | MB_OK);
    }
    else {
        Button_Enable(buttonSouceEllissis, FALSE);
        Button_Enable(buttonDestinationEllipssis, FALSE);
        Button_Enable(buttonCipher, FALSE);
        Button_Enable(buttonDecipher, FALSE);
        if (GetWindowTextLengthA(editorPassword) > 16) {

            MessageBoxA(NULL, "Password should be less than 16 characters", "Password is too big", MB_ICONERROR | MB_OK);
        }
        else if (GetWindowTextLengthA(editorPassword) == 0) {
            MessageBoxA(NULL, "Enter password", "Error", MB_ICONERROR | MB_OK);
        }
        else {
            FILE* sourceFile;
            FILE* destFile;
            sourceFile = fopen(sourceName, "r+");
            destFile = fopen(destName, "w+");

            char pass[16];
            SendMessageA(editorPassword, WM_GETTEXT, 0, (LPARAM)pass);

            int readed = 0;
            char buffer[1];

            int fSize = 0;
            fseek(sourceFile, 0, SEEK_END);

            fSize = ftell(sourceFile);

            fseek(sourceFile, 0, SEEK_SET);

            SendMessageW(progress, PBM_SETRANGE, 0, MAKELPARAM(0, fSize));
            SendMessageW(progress, PBM_SETBARCOLOR, 0, RGB(0, 255, 0));


            while (fread(buffer, sizeof(char), 1, sourceFile) == 1) {
                if (stop == TRUE)
                {
                    fclose(sourceFile);
                    fclose(destFile);
                    Sleep(2000);
                    SendMessageW(progress, PBM_SETPOS, 0, 0);
                    MessageBoxW(NULL, L"Stopped Deciphering", L"Stopped", MB_OK | MB_ICONERROR);
                    break;
                }
                else {
                    ++readed;
                    SendMessageW(progress, PBM_DELTAPOS, 1, 0);
                    buffer[0] = decipher(buffer[0], pass[readed % strlen(pass)]);

                    fwrite(buffer, sizeof(char), 1, destFile);
                    Sleep(100);
                }
            }
            if (stop == FALSE)
            {
                MessageBoxW(NULL, L"Successful Deciphering", L"Success", MB_OK | MB_ICONASTERISK);
            }
            stop = FALSE;
            fclose(sourceFile);
            fclose(destFile);
            Sleep(2000);
            SendMessageW(progress, PBM_SETPOS, 0, 0);
            SendMessageW(progress, PBM_SETBARCOLOR, 0, RGB(0, 255, 0));
            SendMessageW(editorPassword, WM_SETTEXT, 0, (LPARAM)L"*********************");

        }
    }
    Button_Enable(buttonSouceEllissis, TRUE);
    Button_Enable(buttonDestinationEllipssis, TRUE);
    Button_Enable(buttonCipher, TRUE);
    Button_Enable(buttonDecipher, TRUE);
    Release(mutex);
    return 0;
}

DWORD CALLBACK StopButton(LPVOID params) {
    stop = TRUE;
    SendMessageW(stopButton, WM_KILLFOCUS, 0, 0);
    SendMessageW(progress, PBM_SETBARCOLOR, 0, RGB(255, 0, 0));
    return 0;
}