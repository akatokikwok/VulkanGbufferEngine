#include <windows.h>
#include "BVulkan.h"
#include "scene.h"

#pragma comment(lib, "winmm.lib")// 时间需要调用此windows库
// 识别事件消息类型的回调函数
LRESULT CALLBACK LearnWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_SIZE: {// 当窗口尺寸改变时候,事件会被识别为WM_SIZE型
			RECT rect;
			GetClientRect(hwnd, &rect);// 拿取 显示视口(不是整个窗口)的大小
			OnViewportChanged(rect.right - rect.left, rect.bottom - rect.top);// 调用重适配视口,注意是buttom - top
		} break;
		case WM_CLOSE: {// 当鼠标点击关闭按钮的时候,事件会被识别为WM_CLOSE型
			PostQuitMessage(0);// 当消息为WM_CLOSE型就退出循环
		} break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);// 没有消息事件处理时,就使用默认的处理
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndclassex;
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.cbClsExtra = 0;// 窗口的额外空间
	wndclassex.cbWndExtra = 0;
	wndclassex.hbrBackground = nullptr;//窗口内容是渲染出来的,不需要windows gdi去做
	wndclassex.hCursor = LoadCursor(nullptr, IDC_ARROW);// 箭头光标
	wndclassex.hIcon = nullptr;// 磁盘上的logo
	wndclassex.hIconSm = nullptr;// 程序运行时左上角的logo
	wndclassex.hInstance = hInstance;
	wndclassex.lpfnWndProc = LearnWindowProc;// 监听事件用的回调函数,比如鼠标键盘事件
	wndclassex.lpszMenuName = nullptr;// 没有菜单
	wndclassex.lpszClassName = L"BattleFireWindow"; // 加L把字符从char转换为WChar型,注意使用Unicode字符集
	wndclassex.style = CS_VREDRAW | CS_HREDRAW;// 绘制时采用水平重绘和垂直重绘

	ATOM atom = RegisterClassEx(&wndclassex);// 注册窗口,返回ATOM型
	if (atom == 0) {
		MessageBox(nullptr, L"窗口内容:窗口注册失败", L"窗口标题:错误", MB_OK);
		return -1;
	}

	RECT rect = { 0, 0, 1280, 720 };// 设置一个窗口不包含边框的内容大小,不包含边框的显示区域为1280*720,若包含了边框则需要下一行的方法自动调节计算
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	/* 创建窗口*/
	HWND hwnd = CreateWindowEx(NULL, L"BattleFireWindow", L"LearnWindow", WS_OVERLAPPEDWINDOW, 
		0, 0,/*窗口左上角坐标*/ rect.right - rect.left, rect.bottom - rect.top,
		nullptr,/*父窗口句柄*/ nullptr,/*菜单*/ hInstance, nullptr/*创建窗口时额外的参数*/
	);

	/* 初始化VULKAN环境*/
	InitVulkan(hwnd, 1280, 720);
	/* 调用场景初始化函数*/
	Init();
	/* 呈现窗口句柄*/
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);// 显示窗口后再刷新成要的颜色

	MSG msg;//要存放消息的结构体
	float last_time = timeGetTime() / 1000.0f;// 返回计数,秒
	while (true) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {// 持续获取所有消息后再把它们从队列中移除掉
			if (msg.message == WM_QUIT) {// 若获取到的消息类型识别为WM_QUIT就中断循环
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);// 派发消息,实际上就是用的窗口结构体里那个lpfnWndProc关联的方法
		}
		float current_time = timeGetTime() / 1000.0f;
		float deltaTime = current_time - last_time;
		last_time = current_time;
		Draw(deltaTime);// 死循环里使用60帧率进行绘制
	}
	/* 退出前清理一下场景*/
	OnQuit();
	/* 将vulkan持有的资源全部清除,交还给显卡*/
	VulkanCleanUp();
	return 0;
}

