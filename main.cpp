#include <windows.h>
#include <iostream>

/**
 * 剪贴板的下一个窗口
 */
HWND hwndNextViewer = nullptr;

LRESULT APIENTRY MainWndProc(HWND, UINT, WPARAM, LPARAM);

void MyMsgLoop();

HWND CreateWindows(HINSTANCE hInstance);

void ClipboardMessageProcess();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    CreateWindows(hInstance);
    MyMsgLoop();
    return 0;
}

HWND CreateWindows(HINSTANCE hInstance) {
    char clsName[] = "myClipBoardListenerWnd";
    WNDCLASS wc;  //创建一个Windows窗体
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0L;
    wc.cbWndExtra = 0L;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = clsName;

    RegisterClass(&wc);
    HWND hwnd = CreateWindow(clsName, "ClipBoardListenerHiddenWindow",
                             WS_MINIMIZE, 100, 100, 400, 300, NULL, NULL, hInstance, NULL);

    //这里只创建窗口，不显示，应该不需要

    return hwnd;
}

LRESULT APIENTRY MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {// 消息处理
    switch (message) {
        case WM_CREATE:
            hwndNextViewer = SetClipboardViewer(hWnd);
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_DESTROY:
            ChangeClipboardChain(hWnd, hwndNextViewer);
            PostQuitMessage(0);
            break;
        case WM_CHANGECBCHAIN:
            if ((HWND) wParam == hwndNextViewer) {
                hwndNextViewer = (HWND) lParam;
            } else if (hwndNextViewer != nullptr) {
                SendMessage(hwndNextViewer, message, wParam, lParam);
            }
            break;
        case WM_DRAWCLIPBOARD:
            ClipboardMessageProcess();
            SendMessage(hwndNextViewer, message, wParam, lParam);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return (LRESULT) NULL;
}

void ClipboardMessageProcess() {
    static ULONG inc = 0;
    if (inc == 0) {
        //忽略第一次的剪贴板操作
        inc++;
        return;
    } else if (inc >= MAXULONG_PTR) {
        //清空
        inc = 1;
    }
    static UINT auPriorityList[] = {
            CF_OWNERDISPLAY,
            CF_TEXT,
            CF_ENHMETAFILE,
            CF_BITMAP,
            CF_SYLK,
            CF_RIFF,
            CF_UNICODETEXT
    };

    int uFormat = GetPriorityClipboardFormat(auPriorityList, 7);
    std::cout << inc << "," << uFormat << std::endl;
    inc++;
}

void MyMsgLoop() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}