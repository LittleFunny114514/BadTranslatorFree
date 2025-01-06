// TranslatorUI.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "TranslatorUI.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void SetMultiLineText(HWND, const wchar_t*);
DWORD WINAPI TranslateThread(LPVOID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TRANSLATORUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRANSLATORUI));

    MSG msg;

    // 主消息循环:
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



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRANSLATORUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TRANSLATORUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndInputBox, hwndOutputBox,
        hwndTranscntScroll, hwndButton, 
        hwndTranscntValue,hwndLangEdit;
    static RECT rect; static int value; static wchar_t szBuffer[10];
    HFONT font= CreateFontW(0, 0, 0, 0, FW_NORMAL, 
        FALSE, FALSE, FALSE, GB2312_CHARSET, 
        OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, 
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"等线");
    switch (message)
    {
    case WM_CREATE:
    {
        GetClientRect(hWnd, &rect);
        hwndInputBox = CreateWindowEx(
            0, L"EDIT",   // predefined class 
            L"输入文本",         // no window title 
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 0, 0,   // set size in WM_SIZE message 
            hWnd,         // parent window 
            (HMENU)ID_INPUTEDIT,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        SendMessage(hwndInputBox, WM_SETFONT, (WPARAM)font, true);

        hwndOutputBox = CreateWindowEx(
            0, L"EDIT",   // predefined class 
            L"生草结果",         // no window title 
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 0, 0,   // set size in WM_SIZE message 
            hWnd,         // parent window 
            (HMENU)ID_OUTPUTEDIT,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        SendMessage(hwndOutputBox, WM_SETFONT, (WPARAM)font, true);

        hwndButton = CreateWindowW(
            L"BUTTON",   // 按钮类名
            L"翻译", // 按钮文本
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // 样式
            0, 0, 0, 0,
            hWnd,       // 父窗口句柄
            (HMENU)ID_TRANSLATE_BUTTON,   // 按钮编号
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),  // 应用程序实例句柄
            NULL);     // 附加参数
        SendMessage(hwndButton, WM_SETFONT, (WPARAM)font, true);

        hwndTranscntScroll = CreateWindowEx(
            0, L"SCROLLBAR",   // predefined class 
            L"20",         // no window title 
            WS_CHILD | WS_VISIBLE |  WS_TABSTOP |SBS_HORZ ,
            0, 0, 0, 0,   // set size in WM_SIZE message 
            hWnd,         // parent window 
            (HMENU)ID_TRANSCNT_SCROLL,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        SendMessage(hwndTranscntScroll, WM_SETFONT, (WPARAM)font, true);
        SetScrollRange(hwndTranscntScroll, SB_CTL, TRANSCNT_MIN, TRANSCNT_MAX, false);
        SetScrollPos(hwndTranscntScroll, SB_CTL, 20, false);

        hwndTranscntValue = CreateWindow(TEXT("static"), TEXT("20"),
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            0, 0, 0, 0,
            hWnd, (HMENU)ID_TRANSCNT_DISPLAY,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        SendMessage(hwndTranscntValue, WM_SETFONT, (WPARAM)font, true);

        hwndLangEdit = CreateWindow(L"EDIT", L"Default", WS_CHILD | WS_VISIBLE | WS_BORDER,
            0,0,0,0, hWnd, (HMENU)ID_LANGEDIT, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        SendMessage(hwndLangEdit, WM_SETFONT, (WPARAM)font, true);
        //向组合框添加选项 
        break;
    }
    case WM_HSCROLL:
    {
        switch (LOWORD(wParam))
        { 
        case SB_PAGEUP:
            value -= 10;
            break;
        case SB_PAGEDOWN:
            value += 10;
            break;
        case SB_LINEDOWN:
            value++;
            break;
        case SB_LINEUP:
            value--;
            break;
        case SB_TOP:
            value = 0;
            break;
        case SB_BOTTOM:
            value = TRANSCNT_MAX;
            break;
        case SB_THUMBTRACK:
            value = HIWORD(wParam);
            break;
        default:
            break;
        }value = min(value, TRANSCNT_MAX);
        value = max(value, TRANSCNT_MIN);
        SetScrollPos(hwndTranscntScroll, SB_CTL, value, TRUE);
        wsprintf(szBuffer, TEXT("%i"), value);
        SetWindowText(hwndTranscntValue, szBuffer);
        break;
    }
    case WM_SIZE:
    {
        GetClientRect(hWnd, &rect);
        MoveWindow(hwndInputBox, 16, 16, (rect.right - rect.left) / 2 - 24, (rect.bottom - rect.top) - 112, true);
        MoveWindow(hwndOutputBox, (rect.right - rect.left) / 2 + 8, 16, (rect.right - rect.left) / 2 - 16, (rect.bottom - rect.top) - 112, true);
        MoveWindow(hwndButton, 16, (rect.bottom - rect.top) - 48, (rect.right - rect.left) - 128, 32, true);
        MoveWindow(hwndTranscntScroll, (rect.right - rect.left)-104, (rect.bottom - rect.top) - 48, 96, 32, true);
        MoveWindow(hwndTranscntValue, (rect.right - rect.left) - 104, (rect.bottom - rect.top) - 84, 96, 32, true);
        MoveWindow(hwndLangEdit, 16, (rect.bottom - rect.top) - 84, (rect.right - rect.left) - 128, 32, true);
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case ID_TRANSLATE_BUTTON:
            {
                CreateThread(0, 0, &TranslateThread, hWnd, 0, 0);
                break;
            }
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_HELP:
            {
                using namespace std;
                wifstream ifs("help.txt");
                ifs.imbue(std::locale("chs"));
                wostringstream oss;
                oss.str(L"");
                oss << ifs.rdbuf();
                MessageBox(hWnd, oss.str().c_str(), L"BadTranslator使用说明", MB_ICONINFORMATION);
                break;
            }
            
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        MessageBox(0, (std::wostringstream() << message << ' ' << wParam << ' ' << lParam).str().c_str(), L"", 0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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
void SetMultiLineText(HWND hWndEdit, const wchar_t text[]) {
    int len = lstrlenW(text);
    using namespace std;
    wostringstream oss;
    for (int i = 0; i < len;i++) {
        if (text[i] == '\n')
            oss << "\n";
        else oss << text[i];
    }
    SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)(oss.str().c_str()));
}
DWORD WINAPI TranslateThread(LPVOID lp) {
    using namespace std;
    HWND hwnd = (HWND)lp,
        hwndButton = GetDlgItem(hwnd, ID_TRANSLATE_BUTTON);
    SetWindowText(hwndButton, L"正在翻译");
    EnableWindow(hwndButton, false);
    wstring wstr; wstr.resize(1048576);
    string str; str.resize(1024);
    wofstream ofs(L"in");
    ofs.imbue(utf8_locale);
    GetWindowTextW(GetDlgItem(hwnd, ID_INPUTEDIT), &wstr[0], 1048576);
    ofs << wstr.c_str();
    ofs.close();
    wostringstream woss;
    ostringstream oss;
    wstr.resize(1024);
    GetWindowTextA(GetDlgItem(hwnd, ID_LANGEDIT), &str[0], 1024);
    oss << "BadTransCore.exe -Lang " << &str[0]
        << " -TextIn file in -TextOut file out -Count "
        << GetScrollPos(GetDlgItem(hwnd, ID_TRANSCNT_SCROLL), SB_CTL);
    system(oss.str().c_str());
    wifstream ifs("out");
    ifs.imbue(utf8_locale);
    woss.str(L"");
    woss << ifs.rdbuf();
    SetMultiLineText(GetDlgItem(hwnd, ID_OUTPUTEDIT), woss.str().c_str());
    SetWindowText(hwndButton, L"翻译");
    EnableWindow(hwndButton, true);
    return 0;
}