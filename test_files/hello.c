#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>  // 包含工具帮助函数的头文件
 
#define ID_LISTBOX 1  // 定义列表框控件的ID
 
// 函数：填充进程列表到列表框
void PopulateProcessList(HWND hwndListBox) {
    HANDLE hProcessSnap;           // 进程快照句柄
    PROCESSENTRY32 pe32;           // 进程信息结构体
 
    // 创建系统进程的快照
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        // 快照创建失败，返回
        return;
    }
 
    pe32.dwSize = sizeof(PROCESSENTRY32);  // 设置结构体大小
 
    // 获取第一个进程的信息
    if (!Process32First(hProcessSnap, &pe32)) {
        // 获取第一个进程失败，关闭句柄并返回
        CloseHandle(hProcessSnap);
        return;
    }
 
    // 遍历所有进程
    do {
        // 将进程名添加到列表框中
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)pe32.szExeFile);
    } while (Process32Next(hProcessSnap, &pe32));  // 获取下一个进程的信息
    
    // 关闭进程快照句柄
    CloseHandle(hProcessSnap);
}
 
// 窗口过程函数：处理窗口的消息
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndListBox;  // 列表框句柄，静态变量在窗口过程函数中保留
 
    switch (uMsg) {
        case WM_CREATE: {
            // 创建列表框控件
            hwndListBox = CreateWindow(
                "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
                10, 10, 300, 400, hwnd, (HMENU)ID_LISTBOX, ((LPCREATESTRUCT)lParam)->hInstance, NULL
            );
            // 填充进程列表到列表框中
            PopulateProcessList(hwndListBox);
            return 0;
        }
        case WM_DESTROY:
            // 处理窗口销毁消息，发送退出消息并终止应用程序
            PostQuitMessage(0);
            return 0;
    }
    // 默认处理其他消息
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
 
// 程序入口点
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "Sample Window Class";  // 窗口类名
 
    WNDCLASS wc = {0};  // 窗口类结构体初始化
    wc.lpfnWndProc = WindowProc;  // 窗口过程函数
    wc.hInstance = hInstance;  // 应用程序实例句柄
    wc.lpszClassName = CLASS_NAME;  // 窗口类名
    printf("my hInstance = %d", hInstance);
    // 注册窗口类
    RegisterClass(&wc);
 
    // 创建窗口
    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Process List Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 500,
        NULL, NULL, hInstance, NULL
    );
 
    // 检查窗口创建是否成功
    if (hwnd == NULL) {
        return 0;
    }
 
    // 显示窗口并更新其显示
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
 
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);  // 翻译虚拟键消息
        DispatchMessage(&msg);   // 分发消息到窗口过程
    }
 
    return 0;
}
