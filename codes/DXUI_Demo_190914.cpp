// DXUI.cpp : 定义应用程序的入口点。
//

// // 包含 SDKDDKVer.h 可定义可用的最高版本的 Windows 平台。
// 如果希望为之前的 Windows 平台构建应用程序，在包含 SDKDDKVer.h 之前请先包含 WinSDKVer.h 并
// 将 _WIN32_WINNT 宏设置为想要支持的平台。
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN// 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "resource.h"//程序资源文件
#include <commdlg.h>//更换背景图的文件浏览框使用


//*************以上是Windows程序基础头文件
#include "DXUI_Library.h"//**********DXUI界面库***********


DWindow Main;
DWindow About;

DButton Close;
DButton MinSize;
DButton SetBackImg;
DButton BAbout;

DButton About_OK;

DLabel DXUI;
DLabel good;
DLabel good1;

DLabel my;

DTextBox tb;

DImageBox ib;

UINT mdpiX = 96;
UINT mdpiY = 96;

HINSTANCE hInst;//当前实例

//主窗口的参数定义
#define Title       L"DXUI Demo"//标题栏文本
#define WindowClass L"DXUI Class"//主窗口类名
#define WindowWidth  1200
#define WindowHeight 700

//关于窗口的长宽定义
#define AboutWidth 400
#define AboutHeight 500

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;
	MyRegisterClass(hInstance);

	//执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return false;
	}

	MSG msg;

	//主消息循环:
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (!TranslateAcceleratorW(msg.hwnd, nullptr, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	ib.Release();
	tb.Release();
	my.Release();
	good1.Release();
	good.Release();
	DXUI.Release();
	About_OK.Release();
	BAbout.Release();
	SetBackImg.Release();
	MinSize.Release();
	Close.Release();

	About.Release();
	Main.Release();

	return (int)msg.wParam;
}

//  函数: MyRegisterClass()
//  目标: 注册窗口类。
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_SAVEBITS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXUI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = WindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	mdpiX = (UINT)Main.GetDpiX();
	mdpiY = (UINT)Main.GetDpiY();

	//****由于DXUI库全面支持高DPI场景，所以在使用前应在项目设置里设置高DPI支持来防止系统缩放，导致模糊****

	//这里的窗口大小要根据DPI的大小来改变，所以用了一套公式重新计算大小
	HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP,
		(GetSystemMetrics(SM_CXSCREEN) - WindowWidth * mdpiX / 96) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - WindowHeight * mdpiY / 96) / 2,
		WindowWidth * mdpiX / 96, WindowHeight * mdpiY / 96, nullptr, nullptr, hInstance, nullptr);

	//将窗口设置为无边框
	SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE);

	if (!hWnd)
		return false;

	//使用步骤：初始化各个控件及窗口 —— 向窗口添加控件 —— 创建渲染线程

	Close.Init({ WindowWidth - 35, 0 }, { 35, 35 },
		//    ↓ Normal状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 255, 60, 40, 0.0f }, { 0, 0, 0, 0.0f }, { 255, 255, 255, 0.8f }, 0.3f },

		//    ↓ MouseMove状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 255, 70, 50, 0.8f }, { 200, 200, 200, 0.0f }, { 255, 255, 255, 0.8f }, 0.3f },

		//    ↓ Click状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 150, 50, 30, 1.0f }, { 150, 150, 200, 0.0f }, { 255, 255, 255, 0.8f }, 0.05f },

		//    ↓ Disable状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		2.0f, 2.0f, 0.0f, L"×", L"微软雅黑", 30, LightWeight);

	MinSize.Init({ WindowWidth - 70, 0 }, { 35, 35 },
		{ { 255, 255, 255, 0.0f }, { 26, 217, 110, 0.0f }, { 255, 255, 255, 0.8f }, 0.3f },
		{ { 255, 255, 255, 0.3f }, { 35, 231, 131, 0.0f }, { 255, 255, 255, 0.8f }, 0.3f },
		{ { 255, 255, 255, 0.5f }, { 31, 207, 108, 1.0f }, { 255, 255, 255, 1.0f }, 0.05f },
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		2.0f, 2.0f, 0.0f, L"—", L"微软雅黑", 13, BoldWeight);

	SetBackImg.Init({ WindowWidth - 270, WindowHeight - 55 }, { 125, 50 },
		{ { 255, 255, 255, 0.6f }, { 60, 60, 60, 0.3f }, { 60, 60, 60, 0.9f }, 0.3f },
		{ { 80, 80, 80, 0.8f }, { 60, 60, 60, 0.5f }, { 255, 255, 255, 0.9f }, 0.3f },
		{ { 60, 60, 60, 0.85f }, { 30, 30, 30, 1.0f }, { 255, 255, 255, 1.0f }, 0.1f },
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		5, 5, 0.0f, L"设置背景图", L"微软雅黑", 20);

	BAbout.Init({ WindowWidth - 135, WindowHeight - 55 }, { 125, 50 },
		{ { 255, 255, 255, 0.0f }, { 255, 255, 255, 1.0f }, { 255, 255, 255, 1.0f }, 0.3f },
		{ { 255, 255, 255, 0.7f }, { 255, 255, 255, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		{ { 255, 255, 255, 1.0f }, { 255, 255, 255, 1.0f }, { 0, 0, 0, 1.0f }, 0.1f },
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		25, 25, 2.0f, L"关  于", L"微软雅黑", 23);

	About_OK.Init({ AboutWidth / 2 - 50, AboutHeight - 90 }, { 100, 40 },
		{ { 50, 50, 50, 0.0f }, { 50, 50, 50, 1.0f }, { 50, 50, 50, 1.0f }, 0.25f },
		{ { 50, 50, 50, 1.0f }, { 50, 50, 50, 1.0f }, { 255, 255, 255, 1.0f }, 0.25f },
		{ { 100, 100, 100, 1.0f }, { 100, 100, 100, 1.0f }, { 255, 255, 255, 1.0f }, 0.05f },
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		20, 20, 2.0f, L"确定", L"微软雅黑", 22, LightWeight, HandMouse);

	DXUI.Init({ WindowWidth / 2 - 150, 20 }, { 300, 90 },
		{ { 255, 255, 255, 0.85f }, false, false, 0.3f },
		{ { 107, 98, 255, 1.0f }, false, false, 0.3f },
		{ { 107, 98, 255, 1.0f }, false, false, 0.1f },
		false, L"DXUI 界面库", L"微软雅黑", 50, false, NormalWeight, Center, Center);

	good.Init({ 50, 115 }, { WindowWidth - 100, 275 },
		{ { 255, 255, 255, 0.9f }, false, false, 0.3f },
		{ { 81, 123, 255, 1.0f }, false, false, 0.3f },
		{ { 81, 123, 255, 1.0f }, false, false, 0.1f },
		false, L"取“DXUI”这个名字是因为此界面库使用Direct 2D图形库来绘制，使用Wic库实现加载图片及部分功能，使用DWrite库来绘制处理文字。基于Direct系列的界面库少之又少，一是实现太过复杂，学习成本太高。二是考虑到硬件兼容的问题。而Direct 2D完美的解决了这些问题，同时保留了硬件加速的优点，用来做界面库，实现流畅的动画再好不过了。\nDXUI界面库的优点就是：   1.控件样式可以90%自定义，并且完善的透明度支持为界面增色不少。\n2.基于事件驱动的动画，用户无操作时不消耗资源。\n3.局部刷新技术，背景图再大，上层控件动画也能流畅播放。\n4.非常简单，开发者只需要考虑如何找到好看的颜色，仅通过几行代码即可添加控件。\n5.高兼容性，无需考虑硬件支持问题，就算不支持，可无缝切换到软件绘制，效率优于GDI。\n6.拥有比GDI/GDI+更高的绘图质量，界面更清晰大方。\n7.完善的高DPI支持，再也不用担心系统缩放模糊的问题。", L"微软雅黑", 21, true, NormalWeight, Center, Center);

	good1.Init({ 570, 400 }, { 590, 100 },
		{ { 255, 255, 255, 0.85f }, false, false, 0.3f },
		{ { 66, 158, 30, 1.0f }, false, false, 0.3f },
		{ { 66, 158, 30, 1.0f }, false, false, 0.1f },
		false, L"目前有4种控件：按钮、标签、全自绘编辑框、图片框\n以后有时间准备增加控件页、列表等复杂控件", L"微软雅黑", 25, true, NormalWeight, Center, Center);

	my.Init({ AboutWidth / 2 - 185, 50 }, { 360, 300 },
		{ { 60, 60, 60, 1.0f }, false, false, 0.35f },
		{ { 66, 158, 30, 1.0f }, false, false, 0.35f },
		{ { 66, 158, 30, 1.0f }, false, false, 0.1f },
		false, L"杨X，山东济南，2019级高一，会点C/C++，大概学了5年了，平时喜欢编程。\n\nQQ:2441987560", L"微软雅黑", 23, true, NormalWeight, Center, Center);

	tb.Init({ 50, 400 }, { 500, 250 },
		{ {240, 240, 240, 0.5f }, {200, 200, 200, 0.8f}, {0, 0, 0, 1.0f}, 0.3f },
		{ {245, 245, 245, 0.55f }, {180, 180, 180, 0.9f}, {0, 0, 0, 1.0f}, 0.35f },
		{ {255, 255, 255, 0.7f }, {150, 150, 150, 1.0f}, {0, 0, 0, 1.0f}, 0.25f },
		{ {240, 240, 240, 1.0f }, {220, 220, 220, 1.0f}, {0, 0, 0, 1.0f}, 0.3f },
		10.0f, 10.0f, 1.0f, L"微软雅黑", 20);

	// 图片框（从资源加载），ResourceID为图片资源ID，TypeName为添加资源时设置的TypeName。
	//Module为当前模块ID，默认为null即当前程序（资源在Dll中时，Module应为Dll的Module）
	//CanMoveWindow为是否可以拖动此控件来移动主窗口
	//ScaleMode为图片是 等比例缩放 还是 拉伸以填充
	ib.LoadFromResource({ 0, 0 }, { WindowWidth, WindowHeight }, IDR_JPEG1, L"jpeg", nullptr, true, Fill);

	//主窗口初始化
	Main.Init(hWnd, { 255, 255, 255, 1.0f });
	//设置帧率（不设置则为30帧）
	Main.SetFrameRate(35);

	//添加控件，控件遮盖关系以添加的顺序为依据
	//如果想让某控件的背景为另一个控件，则必须添加背景控件为该控件的Parent，那该控件叫做背景控件的Child
	//Child控件不能超出Parent控件，超出后会以Parent控件的边缘裁剪
	Main.AddControl(&ib);
	Main.AddControl(&Close, ib.GetID());
	Main.AddControl(&MinSize, ib.GetID());


	Main.AddControl(&SetBackImg, ib.GetID());
	Main.AddControl(&BAbout, ib.GetID());

	Main.AddControl(&DXUI, ib.GetID());
	Main.AddControl(&good, ib.GetID());
	Main.AddControl(&good1, ib.GetID());

	Main.AddControl(&tb, ib.GetID());

	//绘制是在一个独立的线程内进行的（保证了流畅性），这里要创建一个渲染线程，所有控件就开始绘制了
	Main.CreateRenderThread();

	ShowWindow(hWnd, SW_NORMAL);

	return true;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//让DXUI处理响应的消息，来改变控件的状态
	DReturnMsg dmsg = Main.DXUIMessageProc(hWnd, message, wParam, lParam);
	DReturnMsg dmsg1 = About.DXUIMessageProc(hWnd, message, wParam, lParam);

	//用户可以处理控件的消息返回值，来达到响应控件操作的目的

	if (hWnd == Main.GethWnd())//多窗口时的分辨处理
	{
		switch (dmsg.Msg)//分辨消息，并比较ID判断
		{
		case Button_LButtonUp://这里响应的按钮的按下操作
		{
			if (dmsg.ControlID == Close)//关闭按钮被按下
			{
				PostQuitMessage(0);
			}
			else if (dmsg.ControlID == MinSize)//最小化按钮被按下
			{
				ShowWindow(hWnd, SW_MINIMIZE);
			}
			else if (dmsg.ControlID == SetBackImg)//设置背景图按钮被按下，这里弹出文件浏览框并设置图片
			{
				OPENFILENAME ofn;
				static wchar_t szFile[256];
				static wchar_t szFileTitle[256];
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = L"Picture File(*.bmp,*.jpg,*.jpeg,*.png,*.heic)\0*.bmp;*.jpg;*.jpeg;*.png;*.heic;\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFileTitle = szFileTitle;
				ofn.nMaxFileTitle = sizeof(szFileTitle);
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
				if(GetOpenFileNameW(&ofn))
					ib.LoadFromFile({ 0, 0 }, { WindowWidth, WindowHeight }, szFile, true, Fill);
			}
			else if (dmsg.ControlID == BAbout)//关于 按钮被按下，这里显示关于窗口
			{
				if (About.IsInit())
				{
					MessageBoxW(hWnd, L"您已打开关于窗口！", L"按钮消息", MB_OK);
					break;
				}
				//ib.SetSize(800, 500);

				HWND hWnd1 = CreateWindowExW(0, WindowClass, Title, 0,
					(GetSystemMetrics(SM_CXSCREEN) - AboutWidth * mdpiX / 96) / 2,
					(GetSystemMetrics(SM_CYSCREEN) - AboutHeight * mdpiY / 96) / 2,
					AboutWidth * mdpiX / 96, AboutHeight * mdpiY / 96, nullptr, nullptr, hInst, nullptr);

				if (!hWnd1)
				{
					return 0;
				}

				About.Init(hWnd1, { 255, 255, 255, 1.0f });
				About.AddControl(&About_OK);//关于窗口的“确定”键
				About.AddControl(&my);//介绍的标签
				About.CreateRenderThread();

				ShowWindow(hWnd1, SW_NORMAL);
			}
			break;
		}
		}
	}
	else if (hWnd == About.GethWnd())
	{
		if (dmsg1.Msg == Button_LButtonUp && dmsg1.ControlID == About_OK)
		{
			DestroyWindow(About.GethWnd());
		}
	}
	//由于DXUI处理的Windows某些消息后，不能让DefWindowProc再处理，所以ReturnValue不为-1时(即表示需要返回一个值)，则把值返回过去。
	if (dmsg.ReturnValue != -1)
		return dmsg.ReturnValue;
	else if (dmsg1.ReturnValue != -1)//如果返回值不为-1，则返回DXUI库处理后的返回值
		return dmsg1.ReturnValue;


	switch (message)
	{
	case WM_DESTROY:
		if (hWnd == About.GethWnd())//如果是关于窗口销毁了，那就释放关于窗口的类
		{
			About.Release();
			break;//仅是关于窗口关闭了，所以不能让下面的QuitMessage消息发送出去来退出程序
		}

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
