#define WIN32_LEAN_AND_MEAN// 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <Windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <tchar.h>
#include <process.h>
#include "resource.h"

#include "DXUI_Library_200316.h"

//主窗口的参数定义
#define Title       L"DXUI Demo"//标题栏文本
#define WindowClass L"DXUI Class"//主窗口类名
#define WindowWidth  350
#define WindowHeight 300

DBezier bIn = { 0.19f, 1.0f, 0.22f, 1.0f };
//DBezier bOut = { 0.77f, 0, 0.175f, 1 };

DWindow Main;
DButton Close;
DButton MinSize;
DImageBox BackImg;

DLabel lb1;
DLabel lb2;
DLabel lb3;
DLabel lb4;

DTextBox Delay;

DButton Begin;
DButton End;

#define SleepInterval 50
long delay = 10;
long ClickedCount = 0;

void __cdecl ClickThread(void* param);
bool ThreadAlive = false;
bool ExitThreadFlag = false;

HINSTANCE hInst;//当前实例

ATOM HotKeyBegin;
ATOM HotKeyStop;

BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void __stdcall DConProc(HWND hWnd, UINT id, DControlType type, DControlMsg msg, WPARAM wParam, LPARAM lParam);

void BeginClick();
void StopClick();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;

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

	return (int)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	//****由于DXUI库全面支持高DPI场景，所以在使用前 应在项目设置里设置高DPI支持，来防止系统缩放 导致模糊****

	if (!Main.Create(hInst, WindowClass, Title, WndProc,
		(GetSystemMetrics(SM_CXSCREEN) * 96 / (int)Main.GetDpiX() - WindowWidth) / 2,
		(GetSystemMetrics(SM_CYSCREEN) * 96 / (int)Main.GetDpiY() - WindowHeight) / 2,
		WindowWidth, WindowHeight))
		return false;

	//使用步骤：初始化各个控件及窗口 —— 向窗口添加控件 —— 创建渲染线程

	Close.Init({ WindowWidth - 35, 0 }, { 35, 35 },
		//    ↓ Normal状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 0, 0, 0, 0.0f }, { 0, 0, 0, 0.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		//    ↓ MouseMove状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 255, 70, 50, 0.8f }, { 200, 200, 200, 0.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		//    ↓ Click状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 150, 50, 30, 1.0f }, { 150, 150, 200, 0.0f }, { 0, 0, 0, 1.0f }, 0.05f },
		//    ↓ Disable状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		0.0f, 0.0f, 0.0f, L"×", L"微软雅黑", 25, DFontWeight::LightWeight);

	MinSize.Init({ WindowWidth - 70, 0 }, { 35, 35 },
		{ { 0, 0, 0, 0.0f }, { 26, 217, 110, 0.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		{ { 128, 128, 128, 0.6f }, { 35, 231, 131, 0.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		{ { 128, 128, 128, 0.8f }, { 31, 207, 108, 1.0f }, { 0, 0, 0, 1.0f }, 0.05f },
		{ { 128, 128, 128, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		0.0f, 0.0f, 0.0f, L"—", L"微软雅黑", 10, DFontWeight::BoldWeight);

	BackImg.LoadFromResource({ 0, 0 }, { WindowWidth, WindowHeight }, IDB_PNG1, L"png", nullptr, true, DImageScaleMode::Fill);

	lb1.Init({ WindowWidth / 2 - 75, 20 }, { 150, 50 },
		{ { 0, 137, 255, 1.0f }, false, false, 0.0f },
		{ { 0, 137, 255, 1.0f }, false, false, 0.0f },
		{ { 0, 137, 255, 1.0f }, false, false, 0.0f },
		true, L"鼠标连点器", L"微软雅黑", 30, false, DFontWeight::NormalWeight, DAlignment::Center, DAlignment::Center);

	lb2.Init({ 25, 100 }, { 90, 30 },
		{ { 0, 0, 0, 1.0f }, false, false, 0.0f },
		{ { 0, 0, 0, 1.0f }, false, false, 0.0f },
		{ { 0, 0, 0, 1.0f }, false, false, 0.0f },
		true, L"点击间隔 :", L"微软雅黑", 20, false, DFontWeight::NormalWeight, DAlignment::Far, DAlignment::Center);

	lb3.Init({ 280, 100 }, { 30, 30 },
		{ { 0, 0, 0, 1.0f }, false, false, 0.0f },
		{ { 0, 0, 0, 1.0f }, false, false, 0.0f },
		{ { 0, 0, 0, 1.0f }, false, false, 0.0f },
		true, L"ms", L"微软雅黑", 20, false, DFontWeight::NormalWeight, DAlignment::Near, DAlignment::Center);

	lb4.Init({ -150, WindowHeight - 71 }, { 145, 20 },
		{ { 50, 50, 50, 1.0f }, false, false, 0.0f },
		{ { 50, 50, 50, 1.0f }, false, false, 0.0f },
		{ { 50, 50, 50, 1.0f }, false, false, 0.0f },
		true, L"已点击 0 次", L"微软雅黑", 13, false, DFontWeight::NormalWeight, DAlignment::Near, DAlignment::Center);

	Delay.Init({ 120, 100 }, { 150, 30 },
		{ {0, 0, 0, 0.1f }, { 160, 160, 160, 0.8f }, { 0, 0, 0, 1.0f }, 0.25f },
		{ {0, 0, 0, 0.3f }, { 120, 120, 120, 0.8f }, { 0, 0, 0, 1.0f }, 0.25f },
		{ {0, 0, 0, 0.05f }, { 80, 80, 80, 1.0f }, { 0, 0, 0, 1.0f }, 0.15f },
		{ {240, 240, 240, 1.0f }, { 150, 150, 150, 1.0f }, {0, 0, 0, 1.0f}, 0.3f },
		1.5f, 1.5f, 1.5f, L"微软雅黑", 15, false, false, false, DFontWeight::BoldWeight, DAlignment::Near, DAlignment::Center, { 0, 0, 0, 1.0f }, 2.0f, { 0, 0, 0, 0.3f });
	Delay.SetText(L"10");

	Begin.Init({ 0, WindowHeight - 50 }, { 175, 50 },
		{ { 0, 0, 0, 0.1f }, { 26, 217, 110, 0.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		{ { 0, 0, 0, 0.3f }, { 35, 231, 131, 0.0f }, { 0, 0, 0, 1.0f }, 0.25f },
		{ { 0, 0, 0, 0.5f }, { 31, 207, 108, 1.0f }, { 0, 0, 0, 1.0f }, 0.05f },
		{ { 0, 0, 0, 0.1f }, { 26, 217, 110, 0.0f }, { 128, 128, 128, 1.0f }, 0.15f },
		0.0f, 0.0f, 0.0f, L"开始(F2)", L"微软雅黑", 20, DFontWeight::NormalWeight);

	End.Init({ 175, WindowHeight - 50 }, { 175, 50 },
		{ { 0, 0, 0, 0.1f }, { 26, 217, 110, 0.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		{ { 0, 0, 0, 0.3f }, { 35, 231, 131, 0.0f }, { 0, 0, 0, 1.0f }, 0.25f },
		{ { 0, 0, 0, 0.5f }, { 31, 207, 108, 1.0f }, { 0, 0, 0, 1.0f }, 0.05f },
		{ { 0, 0, 0, 0.1f }, { 26, 217, 110, 0.0f }, { 128, 128, 128, 1.0f }, 0.15f },
		0.0f, 0.0f, 0.0f, L"停止(F3)", L"微软雅黑", 20, DFontWeight::NormalWeight);
	End.SetState(DControlState::Disable);

	//主窗口初始化
	Main.Init(DConProc, 3.5f, 0.6f);

	//Main.Dbg_ShowUpdateRect(true);

	//添加控件，控件遮盖关系以添加的顺序为依据
	//如果想让某控件的背景为另一个控件，则必须添加背景控件为该控件的Parent，那该控件叫做背景控件的Child
	//Child控件不能超出Parent控件，超出后会以Parent控件的边缘裁剪
	Main.AddControl(&BackImg);
	Main.AddControl(&Close, BackImg);
	Main.AddControl(&MinSize, BackImg);
	Main.AddControl(&lb1, BackImg);
	Main.AddControl(&lb2, BackImg);
	Main.AddControl(&lb3, BackImg);
	Main.AddControl(&lb4, BackImg);
	Main.AddControl(&Delay, BackImg);
	Main.AddControl(&Begin, BackImg);
	Main.AddControl(&End, BackImg);

	HotKeyBegin = GlobalAddAtomW(L"YzClickBegin") - 0xC000;
	HotKeyStop = GlobalAddAtomW(L"YzClickStop") - 0xC000;

	RegisterHotKey(Main, HotKeyBegin, 0, VK_F2);
	RegisterHotKey(Main, HotKeyStop, 0, VK_F3);

	//绘制是在一个独立的线程内进行的（保证了流畅性），这里要创建一个渲染线程，所有控件就开始绘制了
	Main.CreateRenderThread();

	Main.Show();

	return true;
}

void __stdcall DConProc(HWND hWnd, UINT id, DControlType type, DControlMsg msg, WPARAM wParam, LPARAM lParam)
{
	if (hWnd == Main)
	{
		if (msg == DControlMsg::Control_LButtonUp)//多窗口时的分辨处理
		{
			if (id == Close)//关闭按钮被按下
			{
				DestroyWindow(hWnd);
			}
			else if (id == MinSize)//最小化按钮被按下
			{
				ShowWindow(hWnd, SW_MINIMIZE);
			}
			else if (id == Begin)
			{
				BeginClick();
			}
			else if (id == End)
			{
				StopClick();
			}

		}
		else if (msg == DControlMsg::Control_StateChanged)
		{

		}
	}

	return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//让DXUI处理响应的消息，来改变控件的状态
	LRESULT res = Main.DXUIMessageProc(hWnd, message, wParam, lParam);


	//由于DXUI处理的Windows某些消息后，不能让DefWindowProc再处理，所以ReturnValue不为-1时(即表示需要返回一个值)，则把值返回过去。
	if (res != -1)
		return res;

	switch (message)
	{
	case WM_HOTKEY:
		if (LOWORD(lParam) == 0)
		{
			if (HIWORD(lParam) == VK_F2)
				BeginClick();
			if (HIWORD(lParam) == VK_F3)
				StopClick();
		}
		break;
	case WM_DESTROY:
		UnregisterHotKey(Main, HotKeyBegin);
		UnregisterHotKey(Main, HotKeyStop);
		StopClick();

		Main.Release();

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void BeginClick()
{
	if (!ThreadAlive)
	{
		delay = _wtol(Delay.GetText().c_str());
		if (delay < 0)
			delay = 0;

		//将文本框内容设为计算后的值
		wchar_t* wstr = new wchar_t[128];
		_ltow_s(delay, wstr, 128, 10);
		Delay.SetText(wstr);
		delete[] wstr;

		_beginthread(ClickThread, 0, nullptr);
	}
	return;
}
void StopClick()
{
	if (ThreadAlive)
	{
		ExitThreadFlag = true;
		while (ThreadAlive)
		{
			Sleep(SleepInterval);
		}
	}
	Main.Show();
	return;
}
void __cdecl ClickThread(void* param)
{
	if (ThreadAlive)
		return;

	ThreadAlive = true;
	ExitThreadFlag = false;
	Begin.SetState(DControlState::Disable);
	End.SetState(DControlState::Normal);
	ClickedCount = 0;
	lb4.SetText(L"已点击 0 次");
	lb4.SetPosition(5, WindowHeight - 71, 1500, &bIn);


	POINT P = { 0 };
	bool succ = timeBeginPeriod(1);
	wchar_t* buf = new wchar_t[30];
	while (!ExitThreadFlag)
	{
		GetCursorPos(&P);
		mouse_event(MOUSEEVENTF_LEFTDOWN, P.x, P.y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, P.x, P.y, 0, 0);
		
		//更新点击次数
		ClickedCount++;
		swprintf_s(buf, 30, L"已点击 %d 次", ClickedCount);
		lb4.SetText(buf);

		if (delay <= SleepInterval)
		{
			Sleep(delay);
		}
		else {
			long restDelay = delay;
			while (restDelay > SleepInterval && !ExitThreadFlag)
			{
				Sleep(SleepInterval);
				restDelay -= SleepInterval;
			}
			if (restDelay > 0 && !ExitThreadFlag)
				Sleep(restDelay);
		}
	}
	if (succ)
		timeEndPeriod(1);

	lb4.SetPosition(-150, WindowHeight - 71, 600, nullptr);

	End.SetState(DControlState::Disable);
	Begin.SetState(DControlState::Normal);
	ExitThreadFlag = false;
	ThreadAlive = false;
	return;
}
