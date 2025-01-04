// Car2.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Car2.h"

#define MAX_LOADSTRING 100
#define TIMER_ID 1
#define BASE_MOVE_STEP 5 // 每次移动的步长
#define WM_TRAYICON (WM_USER + 1) //为托盘图标定义一个消息 ID

// 小车的属性
struct Car {
    int x, y;   // 小车的坐标
    int width, height; // 小车的宽高
};

Car car = { 0, 0, 50, 30 }; // 初始位置和大小


// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HICON hCarIcon; // 声明小车图标的句柄
NOTIFYICONDATA nid = { 0 };
int moveStep = BASE_MOVE_STEP; // 当前移动步长


// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_CAR2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDI_ICON1));

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CAR2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

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
   hCarIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));//加载图标资源

   HWND hWnd = CreateWindowW(szWindowClass, L"时钟中断实验", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   // 在窗口创建后，创建按钮
   RECT rect;
   GetClientRect(hWnd, &rect);

   // 创建增加速度和减少速度的按钮
   CreateWindowW(L"BUTTON", L"增加速度", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       rect.right - 110, 10, 100, 30, hWnd, (HMENU)1, hInstance, nullptr);

   CreateWindowW(L"BUTTON", L"减少速度", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       rect.right - 220, 10, 100, 30, hWnd, (HMENU)2, hInstance, nullptr);

   //创建托盘
   NOTIFYICONDATA nid = { 0 };
   nid.cbSize = sizeof(NOTIFYICONDATA);
   nid.hWnd = hWnd;
   nid.uID = 1; // 图标 ID
   nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
   nid.uCallbackMessage = WM_TRAYICON; // 托盘图标消息
   nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // 使用的图标
   lstrcpy(nid.szTip, L"时钟中断实验"); // 鼠标悬停时显示的提示

   Shell_NotifyIcon(NIM_ADD, &nid); // 添加图标到托盘

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   SetTimer(hWnd, TIMER_ID, 100, nullptr); // 设置定时器，每100毫秒更新一次



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
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case 1: // 增加速度按钮被点击
                moveStep += 2; // 增加速度步长
                break;
            case 2: // 减少速度按钮被点击
                if (moveStep > 1) // 确保步长不小于1
                    moveStep -= 2; // 减少速度步长
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
               /* DestroyWindow(hWnd);*/
                // 单击菜单中的退出，直接退出程序
                PostQuitMessage(0);
                break;
            case 1001: // 点击托盘菜单中的“恢复”
                ShowWindow(hWnd, SW_RESTORE);
                SetForegroundWindow(hWnd);
                break;
            case 1002: // 点击托盘菜单中的“退出”
                // 托盘菜单中处理退出的逻辑与上面的相同
                DestroyIcon(hCarIcon);
                PostQuitMessage(0);
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
            // TODO: 在此处添加使用 hdc 的任何绘图代码...

            // 绘制小车
            /*Rectangle(hdc, car.x, car.y, car.x + car.width, car.y + car.height);*/
            DrawIcon(hdc, car.x, car.y, hCarIcon);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_TIMER:
    {
        // 更新小车位置
        car.x += moveStep; // 每次向右移动
        car.y += moveStep; // 每次向下移动

        // 如果小车超出窗口范围，则重置位置
        RECT rect;
        GetClientRect(hWnd, &rect);
        if (car.x > rect.right || car.y > rect.bottom)
        {
            car.x = 0; // 从左上角开始
            car.y = 0; // 重置小车的y坐标为窗口顶部
        }

        InvalidateRect(hWnd, nullptr, TRUE); // 重绘窗口
    }
    break;
    case WM_TRAYICON:
        //处理托盘事件
        if (lParam == WM_RBUTTONDOWN) {
            // 创建右键弹出菜单
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, 1001, L"恢复");
            AppendMenu(hMenu, MF_STRING, 1002, L"退出");

            POINT cursorPos;
            GetCursorPos(&cursorPos);
            SetForegroundWindow(hWnd);
            TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, 0, hWnd, nullptr);
            DestroyMenu(hMenu);
        }
        else if (lParam == WM_LBUTTONDBLCLK) {
            // 双击恢复窗口
            ShowWindow(hWnd, SW_RESTORE);
            SetForegroundWindow(hWnd);
        }
        break;
    case WM_DESTROY:
        //Shell_NotifyIcon(NIM_DELETE, &nid); // 移除托盘图标
        ////ShowWindow(hWnd, SW_HIDE); // 隐藏窗口
        ////KillTimer(hWnd, TIMER_ID); // 清除定时器
        //PostQuitMessage(0);
        break;
    case WM_SIZE: 
    {
        // 处理窗口大小变化
        RECT rect;
        GetClientRect(hWnd, &rect);
        car.x = rect.right * car.x / rect.right; // 按比例调整小车的x坐标
        car.y = rect.bottom * car.y / rect.bottom; // 按比例调整小车的y坐标

        // 更新按钮位置
        HWND hButtonInc = GetDlgItem(hWnd, 1);
        HWND hButtonDec = GetDlgItem(hWnd, 2);
        MoveWindow(hButtonInc, rect.right - 110, 10, 100, 30, TRUE);
        MoveWindow(hButtonDec, rect.right - 220, 10, 100, 30, TRUE);
    }
    break;
    case WM_CLOSE:
        // 点击关闭按钮时，只是将程序窗口隐藏
        ShowWindow(hWnd, SW_HIDE);
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
