﻿#pragma once
#ifndef _DXUI_LIBRARY
#define _DXUI_LIBRARY
/******************************************************************

	DXUI界面库
	作者: YZ
	QQ: 2441987560
	WeChat: yangzhen2441987560
	Email: 2441987560@qq.com
	Github: https://github.com/IDXGI/DXUI-Library

	此库来之不易，使用请标明作者。商业用途请先联系我。
	共同维护良好的开源环境！

	>>>>>>>>>>>>>>>>>>>>>>>>>>【 注  意 】<<<<<<<<<<<<<<<<<<<<<<<<<<<
	1. DXUI 所有用户函数参数统一使用 <设备独立像素(DIP)> ，即坐标不需与DPI结合，写死即可。内部会自动根据DPI来缩放
	2. 一定要在 [Debug] [Release] 里设置 [清单工具] —— [输入和输出] —— [每个监视器高 DPI 识别] ，否则程序在高 DPI 屏幕下会被缩放，导致模糊，程序DPI自动缩放功能也无法启用
	3. DXUI全库使用宽字符 (因为DWrite库仅支持宽字符)。遇到转换问题可以使用库内的c2w(),w2c()转换

*******************************************************************/
#define DXUI_VERSION 210221//DXUI版本

#ifndef _WIN32_DCOM//CoInitializeEx函数必须定义此宏
#define _WIN32_DCOM
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d2d1effects.h>
#include <d3d11.h>
#include <dwrite.h>
#include <wincodec.h>
#include <ShellScalingApi.h>

#include <process.h>//_beginthread, _endthread，setTimerEvent等
#include <Mmsystem.h>

#include <cmath>
#include <string>
#include <deque>
#include <vector>
#include <map>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Imm32.lib")

#define RELEASE(P)     if(P){ (P)->Release(); (P) = nullptr; }
#define SafeDelete(P)  if(P){ delete (P); (P) = nullptr; }
#define DeleteArray(P) if(P){ delete[] (P); (P) = nullptr; }
#define GET_X(lp)      ((int)(short)LOWORD(lp))
#define GET_Y(lp)      ((int)(short)HIWORD(lp))
#define DIP2DP(P)      ((P) * (float)Dpi / 96.0f)
#define DP2DIP(P)      ((P) * 96.0f / (float)Dpi)

#ifdef _DEBUG
//抛出错误定义
#define ThrowErr(str) MessageBoxW(nullptr, str, L"DXUI ERROR", MB_OK | MB_ICONERROR);

//检测颜色是否在正确范围
#define DetectColor(Color)\
	if (Color.r > 255 || Color.g > 255 || Color.b > 255 || Color.a > 1.0f || Color.a < 0.0f) ThrowErr(L"控件颜色参数错误");

//检测贝塞尔非线性动画参数是否在正确范围
#define DetectBezierParam(Bezier)\
	if (Bezier.P0 < 0.0f || Bezier.P1 < 0.0f || Bezier.P2 < 0.0f || Bezier.P3 < 0.0f || Bezier.P0 > 1.0f || Bezier.P1 > 1.0f || Bezier.P2 > 1.0f || Bezier.P3 > 1.0f) ThrowErr(L"Bezier动画参数错误");

//调试输出宏
#define DP0(fmt) {wchar_t sOut[256];swprintf_s(sOut,fmt);OutputDebugString(sOut);}
#define DP1(fmt,var) {wchar_t sOut[256];swprintf_s(sOut,fmt,var);OutputDebugString(sOut);}
#define DP2(fmt,var1,var2) {wchar_t sOut[256];swprintf_s(sOut,fmt,var1,var2);OutputDebugString(sOut);}
#define DP3(fmt,var1,var2,var3) {wchar_t sOut[256];swprintf_s(sOut,fmt,var1,var2,var3);OutputDebugString(sOut);}
#define DP4(fmt,var1,var2,var3,var4) {wchar_t sOut[256];swprintf_s(sOut,fmt,var1,var2,var3,var4);OutputDebugString(sOut);}
#define DPRC(var) {wchar_t sOut[256];swprintf_s(sOut,L"%.2f, %.2f, %.2f, %.2f\n",(var).left,(var).top,(var).right,(var).bottom);OutputDebugString(sOut);}
#else//Release模式无实际作用
#define ThrowErr(str)
#define DetectColor(Color)
#define DetectBezierParam(Bezier)
//调试输出宏
#define DP0(fmt)
#define DP1(fmt,var)
#define DP2(fmt,var1,var2)
#define DP3(fmt,var1,var2,var3)
#define DP4(fmt,var1,var2,var3,var4)
#define DPRC(var)
#endif
//运行时错误弹窗
#define RuntimeErr(str) MessageBoxW(nullptr, str, L"DXUI ERROR", MB_OK | MB_ICONERROR);

#define DefFrameRate          60//默认帧率，不可超过60
#define RegisterMessageStr    L"DXUIControlMsg"
#define LocaleName            L"zh-cn"
#define SmoothScrollTime      180//文本框的平滑滚动时间(ms)
#define TextBox_CaretMoveTime 150//文本框的插入符平滑移动时间(ms)
#define SmoothScrollBezier    { 0.5f, 1.0f, 0.89f, 1.0f }//平滑滚动贝塞尔曲线参数
#define Shadow_StrokeWidth    DIP2DP(15)

//DXUI用于循环渲染每一帧的线程
static void __cdecl DXUI_RenderThread(void* param);

//自用多线程高斯模糊
static void _Gau_Blur(BYTE* src, UINT Width, UINT Height, UINT Stride, float GaussianSigma, UINT Threads);
static void _Gau_Vertical(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColInitial, int height, int width, int Channels, float a0a1, float a2a3, float b1b2, float cprev, float cnext);
static void _Gau_Horizontal(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColumn, int width, int height, int Channels, float a0a1, float a2a3, float b1b2, float cprev, float cnext);
static void _Gau_CalCoeff(float sigma, float* a0, float* a1, float* a2, float* a3, float* b1, float* b2, float* cprev, float* cnext);
//自用高斯模糊计算线程
static void __cdecl ___DxLibGaussianBlurThread(void* param);

typedef struct tagGaussianThreadParam
{
	UINT* nT;
	UINT* FinishStep1;

	BYTE* src;
	UINT x;
	UINT y;
	UINT Width;
	UINT Height;
	UINT Stride;
	UINT Channels;

	UINT FullWidth;
	UINT FullHeight;

	unsigned char* tempData;
	float a0a1;
	float a2a3;
	float b1b2;
	float cprev;
	float cnext;
}_GaussianThreadParam;

///<summary>
///DXUI库提供的颜色结构体。其中r,g,b的最大值为255。a为不透明度，最大值为1.0f
///</summary>
typedef struct tagDCOLOR
{
	UINT r;
	UINT g;
	UINT b;
	float a;
}DCOLOR;

///<summary>
///非线性动画的三次贝塞尔曲线参数，最小0.0f，最大1.0f
///</summary>
typedef struct tagDBezier
{
	float P0;
	float P1;
	float P2;
	float P3;
}DBezier;
///<summary>
///控件的类型，包含了所有DXUI库的控件
///</summary>
enum class DControlType
{
	Null,
	///<summary>
	///按钮类型
	///</summary>
	Button,

	///<summary>
	///标签类型
	///</summary>
	Label,

	///<summary>
	///文本框类型
	///</summary>
	TextBox,

	///<summary>
	///图片框类型
	///</summary>
	ImageBox,

	///<summary>
	///Gif框类型
	///</summary>
	GifBox,

	///<summary>
	///单调颜色框类型
	///</summary>
	SolidColorBox,

	///<summary>
	///线性渐变颜色框类型
	///</summary>
	LinearColorBox,

	///<summary>
	///放射渐变颜色框类型
	///</summary>
	RadialColorBox,

	///<summary>
	///滚动视图类型
	///</summary>
	ScrollView,
};
///<summary>
///控件消息
///</summary>
enum class DControlMsg
{
	///<summary>
	///返回Null代表此消息和DXUI控件无关
	///</summary>
	Null,

	Window_Size,//wParam是新的窗口宽度(long),lParam是新的窗口高度(long),均为设备相关像素(DP)
	Window_Position,//wParam是新的窗口x坐标(long),lParam是新的窗口y坐标(long),均为设备相关像素(DP)

	//wParam里是变化之前的状态,lParam里是变化之后的. 类型应转换为DControlState
	Control_StateChanged,

	//所有控件的Move、Down、Up消息中的wParam都是鼠标坐标(屏幕坐标系、设备相关像素). 同Windows消息,可用GET_X(wParam),GET_Y(wParam)获取
	Control_MouseMove,
	Control_Click,
	Control_LButtonDown,
	Control_LButtonUp,

	Control_EndOfAnimation,//wParam是设置动画时设置的AID(UINT)

	TextBox_TextChange,
	TextBox_Scrolling,//鼠标滚动消息触发时wParam是鼠标坐标,可用GET_X(wParam),GET_Y(wParam)获取. lParam是Delta
	TextBox_EndOfScroll,//滚动动画结束时会发送一次
	TextBox_Enter,

	Image_Loaded,
	Gif_Loaded,
	Gif_EndOfPlay,

	ScrollView_Scrolling,//鼠标滚动消息触发时wParam是鼠标坐标,可用GET_X(wParam),GET_Y(wParam)获取. lParam是Delta
	ScrollView_EndOfScroll
};

///<summary>
///DXUI自用结构体
///</summary>
typedef struct ___tagDXUISendConMsg
{
	UINT ID;
	DControlType Type;
	DControlMsg Msg;
	LPARAM lParam;
	WPARAM wParam;

}___DXUISendConMsg;
///<summary>
///控件的状态
///</summary>
enum class DControlState
{
	Normal,
	MouseMove,
	Click,
	Edit,//TextBox特有状态
	Disable//Label, ImageBox, GifBox, ScrollView无此状态
};

///<summary>
///字体粗细
///</summary>
enum class DFontWeight { Thin, Light, Normal, Medium, DemiBold, Bold, ExtraBold, Heavy, ExtraBlack };

///<summary>
///对齐方向（字体等）
///</summary>
enum class DAlignment
{
	///<summary>
	///靠近左边或顶边
	///</summary>
	Near,

	///<summary>
	///居中
	///</summary>
	Center,

	///<summary>
	///靠近右边或底边
	///</summary>
	Far
};

//--------------------------------------------- 按 钮 ---------------------------------------------

///<summary>
///按钮样式（颜色、动画持续时长）
///</summary>
typedef struct tagDButtonTypeStyle
{
	DCOLOR FillColor;
	DCOLOR StrokeColor;
	DCOLOR TextColor;

	///<summary>
	///time: 控件由其它状态转到当前状态时，过渡动画的时长。单位：毫秒
	///</summary>
	UINT time;
}DButtonTypeStyle;

//--------------------------------------------- 标 签 ---------------------------------------------

///<summary>
///标签的样式（颜色、字体样式等）
///</summary>
typedef struct tagDLabelTypeStyle
{
	DCOLOR TextColor;

	bool Strikethrough;
	bool Underline;
	///<summary>
	///控件由其它状态转到当前状态时，过渡动画的时长。单位：毫秒
	///</summary>
	UINT time;
}DLabelTypeStyle;

//--------------------------------------------- 文 本 框 ---------------------------------------------

///<summary>
///文本框的样式（颜色、字体样式等）
///</summary>
typedef struct tagDTextBoxTypeStyle
{
	DCOLOR FillColor;
	DCOLOR StrokeColor;
	DCOLOR TextColor;

	///<summary>
	///控件由其它状态转到当前状态时，过渡动画的时长。单位：毫秒
	///</summary>
	UINT time;
}DTextBoxTypeStyle;

//--------------------------------------------- 图 片 框 ---------------------------------------------

///<summary>
///图片缩放类型
///EqualScale: 等比例缩放
///Fill: 拉伸以填充满整个控件
///</summary>
enum class DScaleMode { EqualScale, Fill };

//--------------------------------------------- 颜 色 框 ---------------------------------------------
///<summary>
///渐变颜色框的渐变色参数，position为 0.0f ~ 1.0f
///</summary>
typedef struct tagDColorBox_Point
{
	DCOLOR color;
	float position;
}DColorBox_Point;


///<summary>
///DXUI控件消息的回调函数类型 DMsgProc
///</summary>
typedef void(__stdcall* DMsgProc)(HWND hWnd, UINT id, DControlType type, DControlMsg msg, WPARAM wParam, LPARAM lParam);

///<summary>
///宽窄字节文本相互转换函数
///</summary>
static std::string w2c(const std::wstring& wstr)
{
	if (!wstr.empty())
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		nLen++;
		char* pszBuf = new char[nLen];
		if (!pszBuf)
			return "";
		ZeroMemory(pszBuf, nLen);
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pszBuf, nLen, nullptr, nullptr);//长度中包含'\0'字符
		pszBuf[nLen - 1] = 0;
		std::string res = pszBuf;
		delete[] pszBuf;
		return res;
	}
	return "";
}
///<summary>
///宽窄字节文本相互转换函数
///</summary>
static std::wstring c2w(const std::string& str)
{
	if (!str.empty())
	{
		int nLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
		nLen++;
		wchar_t* pszBuf = new wchar_t[nLen];
		if (!pszBuf)
			return L"";
		ZeroMemory(pszBuf, nLen);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pszBuf, nLen);//长度中包含'\0'字符
		pszBuf[nLen - 1] = L'\0';
		std::wstring res = pszBuf;
		delete[] pszBuf;
		return res;
	}
	return L"";
}
//由于新API与老版系统不兼容，自己实现获取DPI
static int DXUI_GetWndDpi(const HWND& hWnd)
{
	UINT dpi = 96;
	bool res = true;
	if (hWnd)
	{
		HINSTANCE hInstWinSta = LoadLibraryW(L"SHCore.dll");
		if (hInstWinSta)
		{
			typedef HRESULT(WINAPI* PFN_GDFM)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
			PFN_GDFM pGetDpiForMonitor = (PFN_GDFM)GetProcAddress(hInstWinSta, "GetDpiForMonitor");
			if (pGetDpiForMonitor)
			{
				if (FAILED(pGetDpiForMonitor(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), MDT_EFFECTIVE_DPI, &dpi, &dpi)))
					res = false;
			}
			else res = false;
		}
		else res = false;
	}
	else res = false;
	if (!res)//老版本系统实现方法
		dpi = (UINT)GetDeviceCaps(GetDC(nullptr), LOGPIXELSX);
	if (dpi > 1200)
		dpi = 96;

	return (int)dpi;
}

///<summary>
/// (DXUI自用) 线程锁，用来锁接口，不可同时读、同时写、同时读写
///</summary>
class ___DThreadLock
{
public:
	___DThreadLock()
	{
		InitializeCriticalSection(&lock);
	}
	~___DThreadLock()
	{
		DeleteCriticalSection(&lock);
	}

	inline void Lock()
	{
		EnterCriticalSection(&lock);
	}
	inline void Unlock()
	{
		LeaveCriticalSection(&lock);
	}
private:
	CRITICAL_SECTION lock = { 0 };
};

///<summary>
/// (DXUI自用) 添加参数动画后，用于计算每一帧的参数值
///</summary>
class _AniCalc_f
{
public:
	operator bool()
	{
		return (bool)nNowFrame;
	}
	void AddAni(const float& Start, const float& End, const UINT& nFrame, const DBezier* const& BezierParam, const UINT& AID, const bool& ChangeStart = true)
	{
		___CalcLock.Lock();
		float mStart;
		if (nNowFrame && ChangeStart)
		{
			if (nNowFrame == 1)
				mStart = this->End;
			else if (nNowFrame == nSrcFrame)
				mStart = this->Start;
			else
				mStart = cache;
		}
		else mStart = Start;

		if (!nFrame || mStart == End)
		{
			nSrcFrame = 1;
			nNowFrame = 1;
		}
		else
		{
			nSrcFrame = nFrame;
			nNowFrame = nFrame;
		}

		this->Start = mStart;
		this->End = End;
		this->AID = AID;

		Delta = (End - mStart) / (float)nSrcFrame;

		if (mStart < End)
			NumAdd = true;
		else
			NumAdd = false;

		if (BezierParam)
		{
			EnableBezier = true;
			bezier = *BezierParam;
			DetectBezierParam(bezier);
		}
		else {
			EnableBezier = false;
		}

		___CalcLock.Unlock();
		return;
	}
	float FrameCalc(UINT& aid)
	{
		___CalcLock.Lock();

		--nNowFrame;
		if (!nNowFrame)
		{
			aid = AID;
			cache = End;
		}
		else
		{
			if (EnableBezier)
				cache = End - (End - Start) * (1.0f - CalcBezier());
			else
				cache = End - (float)(Delta * nNowFrame);
		}

		float res;
		if (NumAdd && cache < Start)
		{
			res = Start;
			cache = res;
		}
		else if (!NumAdd && cache > Start)
		{
			res = Start;
			cache = res;
		}
		else
			res = cache;

		___CalcLock.Unlock();
		return res;
	}

	inline float GetEnd()
	{
		return End;
	}
	inline void Reset()
	{
		___CalcLock.Lock();
		nNowFrame = 0;
		___CalcLock.Unlock();
		return;
	}

private:
	___DThreadLock ___CalcLock;

	float cache = 0.0f;
	float perc = 1.0f;

	bool NumAdd = true;

	float Start = 0.0f;
	float End = 0.0f;

	float Delta = 0.0f;

	UINT nSrcFrame = 0;
	UINT nNowFrame = 0;

	bool EnableBezier = false;
	DBezier bezier = { 0 };

	UINT AID = 0;

	inline float CalcBezier()
	{
		//perc为当前进度，相当于x
		perc = 1.0f - ((float)nNowFrame / (float)nSrcFrame);

		//根据x反解t，先把它化成 x = at^3 + bt^2 + ct + d 的标准形式
		float a = 1.0f - 3.0f * bezier.P2 + 3.0f * bezier.P0;
		float b = 3.0f * bezier.P2 - 6.0f * bezier.P0;
		float c = 3.0f * bezier.P0;
		float d = -perc;

		//根据牛顿迭代法求一元三次方程解(t)
		float t = 0.5f;
		for (int i = 0; i < 6; ++i)
			t = t - (d + (c + (b + a * t) * t) * t) / (c + (2.0f * b + a * t * 3.0f) * t);

		//根据t求y
		float z = 1.0f - t;
		return 3.0f * bezier.P1 * t * z * z + 3.0f * bezier.P3 * t * t * z + t * t * t;
	}
};

///<summary>
/// (DXUI自用) 添加参数动画后，用于计算每一帧的参数值
///</summary>
class _AniCalc_l
{
public:
	operator bool()
	{
		return (bool)nNowFrame;
	}
	void AddAni(const long& Start, const long& End, const UINT& nFrame, const DBezier* const& BezierParam, const UINT& AID, const bool& ChangeStart = true)
	{
		___CalcLock.Lock();
		long mStart;
		if (nNowFrame && ChangeStart)
		{
			if (nNowFrame == 1)
				mStart = this->End;
			else if (nNowFrame == nSrcFrame)
				mStart = this->Start;
			else
				mStart = cache;
		}
		else mStart = Start;

		if (!nFrame || mStart == End)
		{
			nSrcFrame = 1;
			nNowFrame = 1;
		}
		else
		{
			nSrcFrame = nFrame;
			nNowFrame = nFrame;
		}

		this->Start = mStart;
		this->End = End;
		this->AID = AID;

		Delta = (End - mStart) / (float)nSrcFrame;

		if (mStart < End)
			NumAdd = true;
		else
			NumAdd = false;

		if (BezierParam)
		{
			EnableBezier = true;
			bezier = *BezierParam;
			DetectBezierParam(bezier);
		}
		else {
			EnableBezier = false;
		}

		___CalcLock.Unlock();
		return;
	}
	long FrameCalc(UINT& aid)
	{
		___CalcLock.Lock();

		--nNowFrame;
		if (!nNowFrame)
		{
			aid = AID;
			cache = End;
		}
		else
		{
			if (EnableBezier)
			{
				cache = End - (long)((End - Start) * (1.0f - CalcBezier()));
			}
			else
				cache = End - (long)(Delta * nNowFrame);
		}

		long res;
		if (NumAdd && cache < Start)
		{
			res = Start;
			cache = res;
		}
		else if (!NumAdd && cache > Start)
		{
			res = Start;
			cache = res;
		}
		else
			res = cache;

		___CalcLock.Unlock();
		return res;
	}

	inline long GetEnd()
	{
		return End;
	}
	inline void Reset()
	{
		___CalcLock.Lock();
		nNowFrame = 0;
		___CalcLock.Unlock();
		return;
	}

private:
	___DThreadLock ___CalcLock;

	long cache = 0;
	float perc = 1.0f;

	bool NumAdd = true;

	long Start = 0;
	long End = 0;

	float Delta = 0.0f;

	UINT nSrcFrame = 0;
	UINT nNowFrame = 0;

	bool EnableBezier = false;
	DBezier bezier = { 0 };

	UINT AID = 0;

	inline float CalcBezier()
	{
		//perc为当前进度，相当于x
		perc = 1.0f - ((float)nNowFrame / (float)nSrcFrame);

		//根据x反解t，先把它化成 x = at^3 + bt^2 + ct + d 的标准形式
		float a = 1.0f - 3.0f * bezier.P2 + 3.0f * bezier.P0;
		float b = 3.0f * bezier.P2 - 6.0f * bezier.P0;
		float c = 3.0f * bezier.P0;
		float d = -perc;

		//根据牛顿迭代法求一元三次方程解(t)
		float t = 0.5f;
		for (int i = 0; i < 6; ++i)
			t = t - (d + (c + (b + a * t) * t) * t) / (c + (2.0f * b + a * t * 3.0f) * t);

		//根据t求y
		float z = 1.0f - t;
		return 3.0f * bezier.P1 * t * z * z + 3.0f * bezier.P3 * t * t * z + t * t * t;
	}
};

class _AniCalc_i
{
public:
	operator bool()
	{
		return (bool)nNowFrame;
	}
	void AddAni(const int& Start, const int& End, const UINT& nFrame, const DBezier* const& BezierParam, const UINT& AID, const bool& ChangeStart = true)
	{
		___CalcLock.Lock();
		int mStart;
		if (nNowFrame && ChangeStart)
		{
			if (nNowFrame == 1)
				mStart = this->End;
			else if (nNowFrame == nSrcFrame)
				mStart = this->Start;
			else
				mStart = cache;
		}
		else mStart = Start;

		if (!nFrame || mStart == End)
		{
			nSrcFrame = 1;
			nNowFrame = 1;
		}
		else
		{
			nSrcFrame = nFrame;
			nNowFrame = nFrame;
		}

		this->Start = mStart;
		this->End = End;
		this->AID = AID;

		Delta = (End - mStart) / (float)nSrcFrame;

		if (mStart < End)
			NumAdd = true;
		else
			NumAdd = false;

		if (BezierParam)
		{
			EnableBezier = true;
			bezier = *BezierParam;
			DetectBezierParam(bezier);
		}
		else {
			EnableBezier = false;
		}

		___CalcLock.Unlock();
		return;
	}
	int FrameCalc(UINT& aid)
	{
		___CalcLock.Lock();

		--nNowFrame;
		if (!nNowFrame)
		{
			aid = AID;
			cache = End;
		}
		else
		{
			if (EnableBezier)
			{
				cache = End - (int)((End - Start) * (1.0f - CalcBezier()));
			}
			else
				cache = End - (int)(Delta * nNowFrame);
		}

		int res;
		if (NumAdd && cache < Start)
		{
			res = Start;
			cache = res;
		}
		else if (!NumAdd && cache > Start)
		{
			res = Start;
			cache = res;
		}
		else
			res = cache;

		___CalcLock.Unlock();
		return res;
	}

	inline int GetEnd()
	{
		return End;
	}
	inline void Reset()
	{
		___CalcLock.Lock();
		nNowFrame = 0;
		___CalcLock.Unlock();
		return;
	}

private:
	___DThreadLock ___CalcLock;

	int cache = 0;
	float perc = 1.0f;

	bool NumAdd = true;

	int Start = 0;
	int End = 0;

	float Delta = 0.0f;

	UINT nSrcFrame = 0;
	UINT nNowFrame = 0;

	bool EnableBezier = false;
	DBezier bezier = { 0 };

	UINT AID = 0;

	inline float CalcBezier()
	{
		//perc为当前进度，相当于x
		perc = 1.0f - ((float)nNowFrame / (float)nSrcFrame);

		//根据x反解t，先把它化成 x = at^3 + bt^2 + ct + d 的标准形式
		float a = 1.0f - 3.0f * bezier.P2 + 3.0f * bezier.P0;
		float b = 3.0f * bezier.P2 - 6.0f * bezier.P0;
		float c = 3.0f * bezier.P0;
		float d = -perc;

		//根据牛顿迭代法求一元三次方程解(t)
		float t = 0.5f;
		for (int i = 0; i < 6; ++i)
			t = t - (d + (c + (b + a * t) * t) * t) / (c + (2.0f * b + a * t * 3.0f) * t);

		//根据t求y
		float z = 1.0f - t;
		return 3.0f * bezier.P1 * t * z * z + 3.0f * bezier.P3 * t * t * z + t * t * t;
	}
};

///<summary>
/// (DXUI自用) 添加颜色动画后，用于计算每一帧的颜色值
///</summary>
class _Ani_Color
{
public:
	operator bool()
	{
		bool res = false;
		if (r)
			res = true;
		else if (g)
			res = true;
		else if (b)
			res = true;
		else if (a)
			res = true;
		return res;
	}

	void AddAni(const D2D1_COLOR_F& Start, const DCOLOR& End, const UINT& nFrame)
	{
		r.AddAni(Start.r, (float)End.r, nFrame, nullptr, 0);
		g.AddAni(Start.g, (float)End.g, nFrame, nullptr, 0);
		b.AddAni(Start.b, (float)End.b, nFrame, nullptr, 0);
		a.AddAni(Start.a, End.a, nFrame, nullptr, 0);
		return;
	}
	bool FrameCalc(D2D1_COLOR_F& col)
	{
		UINT aid;
		bool res = false;
		if (r)
		{
			col.r = r.FrameCalc(aid);
			res = true;
		}
		if (g)
		{
			col.g = g.FrameCalc(aid);
			res = true;
		}
		if (b)
		{
			col.b = b.FrameCalc(aid);
			res = true;
		}
		if (a)
		{
			col.a = a.FrameCalc(aid);
			res = true;
		}
		return res;
	}

	inline void Reset()
	{
		r.Reset();
		g.Reset();
		b.Reset();
		a.Reset();
		return;
	}

private:
	_AniCalc_f r;
	_AniCalc_f g;
	_AniCalc_f b;
	_AniCalc_f a;
};

///<summary>
/// (DXUI自用) Draw时的Clip, Layer自动弹出
///</summary>
class _ClipManager
{
public:
	inline void PushLayer(ID2D1DeviceContext* const& Target, const D2D1_RECT_F& contentBounds, ID2D1Geometry* const& geo, const D2D1_ANTIALIAS_MODE& antiMode)
	{
		Target->PushLayer(D2D1::LayerParameters1(contentBounds, geo, antiMode, D2D1::IdentityMatrix(), 1.0f, nullptr, D2D1_LAYER_OPTIONS1_NONE), nullptr);
		IsClip.push_back(false);
		return;
	}
	inline void PushClip(ID2D1DeviceContext* const& Target, const D2D1_RECT_F& r)
	{
		Target->PushAxisAlignedClip(r, D2D1_ANTIALIAS_MODE_ALIASED);
		IsClip.push_back(true);
		return;
	}
	inline void PopLast(ID2D1DeviceContext* const& Target)
	{
		if (IsClip.size())
		{
			IsClipIt = --IsClip.end();
			if (*IsClipIt)
				Target->PopAxisAlignedClip();
			else
				Target->PopLayer();
			IsClip.erase(IsClipIt);
		}
		return;
	}
	inline void PopAll(ID2D1DeviceContext* const& Target)//这里必须按照设置Clip、Layer的顺序弹出，否则报错
	{
		sz = IsClip.size();
		if (sz)
		{
			IsClipIt = IsClip.end();
			for (UINT i = 0; i < sz; ++i)
			{
				--IsClipIt;
				if (*IsClipIt)
					Target->PopAxisAlignedClip();
				else
					Target->PopLayer();
			}
			IsClip.clear();
		}
		return;
	}

private:
	std::vector<bool> IsClip;
	std::vector<bool>::iterator IsClipIt;
	UINT sz = 0;
};
///<summary>
///所有控件的基类
///</summary>
class DControl
{
public:

	//当前控件的光标样式
	HCURSOR CursorStyle = LoadCursorW(nullptr, IDC_ARROW);

	operator UINT()
	{
		return this->ID;
	}

	virtual~DControl()
	{
		_Base_Release();
		return;
	}

	inline UINT GetID()
	{
		return this->ID;
	}

	inline DControlState GetState()
	{
		return this->State;
	}

	inline bool IsInit()
	{
		return this->Inited;
	}

	///<summary>
	///参数均为设备相关像素(DP)
	///</summary>
	virtual bool HitTest(int xPos, int yPos)
	{
		___HitTestLock.Lock();
		if (!HitTestGeo)
		{
			___HitTestLock.Unlock();
			return false;
		}
		//在HitTest中计算位置，进行命中测试
		float _xPos = (int)xPos - DstRC.rect.left + DIP2DP(StrokeWidth) / 2.0f, _yPos = (int)yPos - DstRC.rect.top + DIP2DP(StrokeWidth) / 2.0f + GetPosOffset();
		if (_xPos < 0 || _yPos < 0 || _xPos >= (int)(UpdateRC.right - UpdateRC.left))
		{
			___HitTestLock.Unlock();
			return false;
		}

		//如果有父控件，必须也同时满足父控件的HitTest
		BOOL value = false;
		HitTestGeo->FillContainsPoint({ _xPos, _yPos }, nullptr, 1.0f, &value);
		if (value)
		{
			if (Parent)
				value = Parent->HitTest(xPos, yPos);
			else//没有父控件，那就说明此Hittest是不需要再次确认的
				value = true;
		}
		___HitTestLock.Unlock();
		return value;
	}

	//如果父控件为ScrollView，用于设置位置偏移指针
	inline void _SetParPosOffPointer(const UINT& ParentID, const std::map<UINT, float*>& ParOfs, float* const& pointer)
	{
		___PosOfsLock.Lock();
		PosOffset = ParOfs;

		if (pointer)
			PosOffset.insert(std::pair<UINT, float*>(ParentID, pointer));
		else
			PosOffset.erase(ParentID);
		___PosOfsLock.Unlock();

		return;
	}

	inline bool SetState(const DControlState& state)
	{
		return m_SetState(state);
	}

	inline void _SetNeedUpdate()
	{
		___NeedUpdateLock.Lock();
		___UpdateClipLock.Lock();
		UpdateClip.clear();
		NeedUpdate = true;
		___UpdateClipLock.Unlock();
		___NeedUpdateLock.Unlock();
		return;
	}

	//特别注意：接收的rc是经过PosOfs转换过的
	inline void _SetNeedUpdate(const D2D1_RECT_F& ClipRect, const bool& CalcSigma = true)
	{
		___NeedUpdateLock.Lock();
		___UpdateClipLock.Lock();
		D2D1_RECT_F r = ClipRect;
		if (BlurSigma > 0.0f && CalcSigma)
		{
			float delta = DIP2DP(BlurSigma) * 3.0f;
			r.left -= delta;
			r.top -= delta;
			r.right += delta;
			r.bottom += delta;
		}
		if (NeedUpdate)
		{
			if (UpdateClip.size())
			{
				std::deque<D2D1_RECT_F>::iterator updIt = --UpdateClip.end();
				if (updIt->left != r.left || updIt->top != r.top || updIt->right != r.right || updIt->bottom != r.bottom)
					UpdateClip.push_back(r);
			}
		}
		else
		{
			UpdateClip.push_back(r);
			NeedUpdate = true;
		}
		___UpdateClipLock.Unlock();
		___NeedUpdateLock.Unlock();
		return;
	}

	void SetTotalOpacity(const float& opa, const UINT& time = 0, const DBezier* const& Nonlinear = nullptr, const bool& ReDraw = true, const UINT& AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Opa.AddAni(this->TotalOpacity, min(max(opa, 0.0f), 1.0f), (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		this->TotalOpacity = min(max(opa, 0.0f), 1.0f);

		if (ReDraw)
			this->_SetNeedUpdate();
		return;
	}
	///<summary>
	///设置控件区域内的实时背景高斯模糊(区域受圆角等设置影响), sigma为模糊程度, 与DPI无关, 性能不受sigma影响
	///</summary>
	bool SetBackgroundGaussianBlur(const float& sigma)
	{
		if (sigma >= 0.0f && BlurSigma != sigma)
		{
			BlurSigma = sigma;
			if (Parent)//如果暂时没有Parent也无妨，数据已经保存
				Parent->_SetNeedUpdate();
			return true;
		}
		return false;
	}
	//都是设备独立像素
	virtual void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true) = 0;
	virtual void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true) = 0;

	///<summary>
	///参数DestX为：如果Position在动画，是否返回动画结束后的目标位置。设备独立像素
	///</summary>
	inline float GetPositionX(bool DestX = false)
	{
		if (DestX && A_RC_x)
			return A_RC_x.GetEnd();
		else
			return DstRC_DIP.rect.left;
	}
	///<summary>
	///参数DestY为：如果Position在动画，是否返回动画结束后的目标位置。设备独立像素
	///</summary>
	inline float GetPositionY(bool DestY = false)
	{
		if (DestY && A_RC_y)
			return A_RC_y.GetEnd();
		else
			return DstRC_DIP.rect.top;
	}
	///<summary>
	///参数DestW为：如果Size在动画，是否返回动画结束后的目标位置
	///</summary>
	inline float GetWidth(bool DestW = false)
	{
		if (DestW && A_RC_width)
			return A_RC_width.GetEnd();
		else
			return DstSize.width;
	}
	///<summary>
	///参数DestH为：如果Size在动画，是否返回动画结束后的目标位置
	///</summary>
	inline float GetHeight(bool DestH = false)
	{
		if (DestH && A_RC_height)
			return A_RC_height.GetEnd();
		else
			return DstSize.height;
	}

	inline float GetTotalOpacity()
	{
		return this->TotalOpacity;
	}

	inline D2D1_RECT_F _GetClipRC()
	{
		D2D1_RECT_F mcliprc;
		float pofs = GetPosOffset();
		mcliprc = { DstRC.rect.left, DstRC.rect.top - pofs, DstRC.rect.right, DstRC.rect.bottom - pofs };

		if (Parent)
		{
			D2D1_RECT_F rc = Parent->_GetClipRC();
			return CalcRectOverlap(mcliprc, rc);
		}
		else
			return mcliprc;
	}

protected:
	friend class DWindow;
	friend class DTransparentWindow;

	struct __updItem
	{
		DControl* con;
		std::deque<D2D1_RECT_F> UpdClip;
	};

	HWND OwnerWnd = nullptr;
	bool OwnerTransparent = false;//控件所在的窗口是否是Transparent窗口
	bool MouseThrough = false;
	int Dpi = 0;

	UINT ConMsg = 0;
	___DThreadLock ___MsgBackLock;

	ID2D1Factory1* D2DFactory = nullptr;
	IWICImagingFactory* WicFactory = nullptr;
	IDWriteFactory* DWriteFactory = nullptr;

	_ClipManager Clip;

	//实现控件范围内的背景高斯模糊
	float BlurSigma = 0.0f;
	ID2D1Effect* BlurEffect = nullptr;
	//用来实现圆角的裁剪区
	ID2D1RoundedRectangleGeometry* RoundGeo = nullptr;
	___DThreadLock ___RoundGeoLock;

	bool OnClick = false;
	DControlState State = DControlState::Normal;
	___DThreadLock ___StateLock;

	DControlType ControlType;
	bool Inited = false;

	bool NeedUpdate = false;
	___DThreadLock ___NeedUpdateLock;

	std::deque<D2D1_RECT_F> UpdateClip;
	___DThreadLock ___UpdateClipLock;

	bool RectChanged = false;
	D2D1_RECT_F LastRect = { 0 };
	___DThreadLock ___RcChangedLock;

	bool CanMoveWindow = false;

	bool ParentDrawing = false;//标示Parent控件是否正在绘图，如果正在绘图，那就不重绘底图了
	DControl* Parent = nullptr;//Parent控件指针

	std::map<UINT, DControl*> Child;
	___DThreadLock ___ChildLock;

	UINT ID = 0;
	UINT FrameRate = DefFrameRate;
	float StrokeWidth = 0.0f;

	///<summary>
	///总的不透明度，在最终绘制时生效，会与控件每个状态指定的不透明度叠加(ep:0.8*0.8=0.64)
	///</summary>
	float TotalOpacity = 1.0f;

	ID2D1Bitmap1* BackImg = nullptr;
	ID2D1Bitmap1* BackImg_Blur = nullptr;
	___DThreadLock ___BackImgLock;

	D2D1_ROUNDED_RECT DstRC_DIP = { 0 };//目标大小的设备独立像素版本
	D2D1_ROUNDED_RECT DstRC = { 0 };//目标大小(绘图用)
	D2D1_SIZE_F DstSize = { 0 };//单独用一个变量保存大小，防止动画过程中重新计算大小导致不精确，设备无关像素
	D2D1_RECT_F UpdateRC = { 0 };//更新的矩形大小，通常比目标的大半个描边宽度(更新区域用)

	_AniCalc_f A_Opa;//不透明度的动画计算类
	_AniCalc_f A_RC_x;//位置x坐标
	_AniCalc_f A_RC_y;//位置y坐标
	_AniCalc_f A_RC_width;//长度
	_AniCalc_f A_RC_height;//宽度
	_AniCalc_f A_Radius_X;//圆角X半径
	_AniCalc_f A_Radius_Y;//圆角Y半径

	//命中测试部分
	ID2D1RoundedRectangleGeometry* HitTestGeo = nullptr;
	___DThreadLock ___HitTestLock;

	//ScrollView控件滚动部分，要在每个控件内植入一个位置偏移量
	std::map<UINT, float*> PosOffset;
	___DThreadLock ___PosOfsLock;

	inline D2D1_COLOR_F Calc_fColor(const D2D1_COLOR_F& NowCol, const float& TotalOp)
	{
		return { (float)NowCol.r / 255.0f, (float)NowCol.g / 255.0f, (float)NowCol.b / 255.0f, NowCol.a * TotalOp };
	}
	inline D2D1_COLOR_F DcolToColf(const DCOLOR& col)
	{
		return { (float)col.r, (float)col.g, (float)col.b, (float)col.a };
	}
	inline D2D1_COLOR_F DcolToColf_255(const DCOLOR& col)
	{
		return { (float)col.r / 255.0f, (float)col.g / 255.0f, (float)col.b / 255.0f, (float)col.a };
	}

	inline D2D1_RECT_F CalcRectOverlap(const D2D1_RECT_F& rc1, const D2D1_RECT_F& rc2)
	{
		return { max(rc1.left, rc2.left), max(rc1.top, rc2.top), min(rc1.right, rc2.right), min(rc1.bottom, rc2.bottom) };
	}
	inline D2D1_RECT_F CalcRectUnion(const D2D1_RECT_F& rc1, const D2D1_RECT_F& rc2)
	{
		return { min(rc1.left, rc2.left), min(rc1.top, rc2.top), max(rc1.right, rc2.right), max(rc1.bottom, rc2.bottom) };
	}

	inline void ClearRect(ID2D1DeviceContext* const& DstTarget, const D2D1_RECT_F& rect, const D2D1_COLOR_F& col)
	{
		DstTarget->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_ALIASED);
		DstTarget->Clear(col);
		DstTarget->PopAxisAlignedClip();
		return;
	}

	//获取DScrollView给子控件设置的偏移量，设备相关像素
	inline float GetPosOffset()
	{
		float res = 0.0f;
		___PosOfsLock.Lock();
		if (PosOffset.size())
		{
			std::map<UINT, float*>::iterator Posit = PosOffset.begin();
			while (Posit != PosOffset.end())
			{
				if (Posit->second)
					res += *Posit->second;

				++Posit;
			}
		}
		___PosOfsLock.Unlock();
		return floor(DIP2DP(res));
	}

	inline bool _DrawDispose(ID2D1DeviceContext* const& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* const& UpdateClip, ID2D1SolidColorBrush*& brush)
	{
		if (!DstTarget)
		{
			this->_SetNeedUpdate();
			return false;
		}
		if (!brush)
		{
			DstTarget->CreateSolidColorBrush(BackColor, &brush);
			if (!brush)
			{
				this->_SetNeedUpdate();
				return false;
			}
		}

		float pofs = GetPosOffset();

		if (Parent)//有底层控件, 要重画backimg
		{
			//如果当前更新区域和父控件的裁剪区域不相交，则不绘制
			if (!IsRectWorkable(CalcRectOverlap(Parent->_GetClipRC(), this->_GetUpdateRC())))
				return false;

			Clip.PushClip(DstTarget, Parent->_GetClipRC());//设置父控件的裁剪

			if (UpdateClip)//设置更新区的裁剪，主要为过渡动画时使用
				Clip.PushClip(DstTarget, *UpdateClip);

			if (pofs != 0.0f)//设置滚动偏移变换
				DstTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.0f, -pofs));

			if (!ParentDrawing || BlurSigma > 0.0f)
				_DrawBackImg(DstTarget);
		}
		else {
			//这里由于没有父控件，所以不可能有pofs
			if (UpdateClip)//设置更新区的裁剪，主要为过渡动画时使用
				Clip.PushClip(DstTarget, *UpdateClip);
			//清除背景
			_NoParentClearBackGru(DstTarget, BackColor);
		}

		return true;
	}

	inline void _Base_DisposeAni()
	{
		if (A_RC_width || A_RC_height)
		{
			UINT AID = 0;
			float wid = DstSize.width, hei = DstSize.height;
			bool resp = false;
			if (A_RC_width)
			{
				wid = A_RC_width.FrameCalc(AID);
				if (!A_RC_width)
					resp = true;
			}
			if (A_RC_height)
			{
				hei = A_RC_height.FrameCalc(AID);
				if (!A_RC_height)
					resp = true;
			}

			SetSize(wid, hei, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		if (A_RC_x || A_RC_y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.rect.left, y = DstRC_DIP.rect.top;
			bool resp = false;
			if (A_RC_x)
			{
				x = A_RC_x.FrameCalc(AID);
				if (!A_RC_x)
					resp = true;
			}
			if (A_RC_y)
			{
				y = A_RC_y.FrameCalc(AID);
				if (!A_RC_y)
					resp = true;
			}

			SetPosition(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		return;
	}

	inline void EndAniSend(const UINT& aid)
	{
		if (aid)//如果动画结束，发送消息
			mSendConMsg(DControlMsg::Control_EndOfAnimation, aid, 0);

		return;
	}

	inline void ResponseCur()
	{
		//让鼠标响应变换
		POINT cp;
		if (OwnerWnd && GetCursorPos(&cp) && ScreenToClient(OwnerWnd, &cp))
		{
			PostMessageW(OwnerWnd, WM_SETCURSOR, 0, MAKELPARAM(0, WM_MOUSEMOVE));
			PostMessageW(OwnerWnd, WM_MOUSEMOVE, 0, MAKELPARAM(cp.x, cp.y));
		}
		return;
	}

	inline void mSendConMsg(const DControlMsg& msg, const WPARAM& wParam, const LPARAM& lParam)
	{
		___MsgBackLock.Lock();
		if (OwnerWnd && ConMsg)
		{
			___DXUISendConMsg* m = new ___DXUISendConMsg;
			m->ID = ID;
			m->Type = ControlType;
			m->Msg = msg;
			m->wParam = wParam;
			m->lParam = lParam;
			PostMessageW(OwnerWnd, ConMsg, 0, (LPARAM)m);
		}
		___MsgBackLock.Unlock();
		return;
	}

	inline void _NoParentClearBackGru(ID2D1DeviceContext* const& DstTarget, const D2D1_COLOR_F& BackColor)
	{
		___RcChangedLock.Lock();
		if (RectChanged)
			ClearRect(DstTarget, LastRect, BackColor);
		else
			ClearRect(DstTarget, UpdateRC, BackColor);
		___RcChangedLock.Unlock();
		return;
	}

	inline bool IsRectWorkable(const D2D1_RECT_F& rc, const bool& strict = true)
	{
		if (strict)
		{
			if (rc.left >= rc.right)
				return false;
			if (rc.top >= rc.bottom)
				return false;
		}
		else {
			if (rc.left > rc.right)
				return false;
			if (rc.top > rc.bottom)
				return false;
		}
		return true;
	}
	virtual bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		return false;
	}

	virtual void _UpdateDpi() = 0;
	void _UpdateDpi_DstRCRadius()
	{
		DstRC.radiusX = floor(DIP2DP(DstRC_DIP.radiusX));
		DstRC.radiusY = floor(DIP2DP(DstRC_DIP.radiusY));
		return;
	}

	inline DControlType _GetControlTypes()
	{
		return this->ControlType;
	}

	virtual bool _GetNeedUpdate()//文本框要重写此函数，因为显示插入符需要计时
	{
		return this->NeedUpdate;
	}
	inline D2D1_RECT_F _GetUpdateRC()//DControl会用到
	{
		float pofs = GetPosOffset();
		if (pofs == 0.0f)
			return this->UpdateRC;
		else
			return { UpdateRC.left, UpdateRC.top - pofs, UpdateRC.right, UpdateRC.bottom - pofs };
	}
	inline D2D1_RECT_F _GetUpdateRect(D2D1_RECT_F* const& UpdateClip)//为主窗口显示刷新区域所用
	{
		D2D1_RECT_F r;
		___RcChangedLock.Lock();
		if (RectChanged)
		{
			r = LastRect;
			___RcChangedLock.Unlock();
		}
		else {
			___RcChangedLock.Unlock();

			r = this->_GetUpdateRC();
		}
		if (Parent)
			r = CalcRectOverlap(Parent->_GetClipRC(), r);

		if (UpdateClip)
			r = CalcRectOverlap(*UpdateClip, r);

		if (IsRectWorkable(r))
			return r;
		else return { 0 };
	}

	inline void _SetID(const UINT& id)
	{
		ID = id;
		return;
	}

	virtual bool _CreateHitTestGeo() = 0;

	bool _Base_Release()
	{
		if (!Inited)
			return false;

		this->_SeparateFromWindow();//先与窗口分离
		Inited = false;

		OwnerWnd = nullptr;
		OwnerTransparent = false;
		MouseThrough = false;
		ConMsg = 0;
		NeedUpdate = false;
		UpdateClip.clear();
		RectChanged = false;
		LastRect = { 0 };
		State = DControlState::Normal;
		CanMoveWindow = false;
		ParentDrawing = false;
		Parent = nullptr;//Parent控件指针，由于和用户用的同一个指针，不能Release
		Child.clear();
		ID = 0;
		Dpi = 0;
		TotalOpacity = 1.0f;
		FrameRate = DefFrameRate;
		StrokeWidth = 0.0f;
		BlurSigma = 0.0f;
		RELEASE(BlurEffect);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(RoundGeo);
		DstRC_DIP = { 0 };
		DstRC = { 0 };
		DstSize = { 0 };
		UpdateRC = { 0 };
		RELEASE(HitTestGeo);
		PosOffset.clear();
		D2DFactory = nullptr;//与主窗口共用一个Factory，不能释放
		WicFactory = nullptr;
		DWriteFactory = nullptr;

		return true;
	}

	inline void _SetMsgBack(const HWND& Owner, const UINT& conMsg)
	{
		___MsgBackLock.Lock();
		OwnerWnd = Owner;
		ConMsg = conMsg;
		___MsgBackLock.Unlock();
		return;
	}
	inline void _SetFactory(ID2D1Factory1* const& D2DFactory, IWICImagingFactory* const& WicFactory, IDWriteFactory* const& DWriteFactory)
	{
		this->D2DFactory = D2DFactory;
		this->WicFactory = WicFactory;
		this->DWriteFactory = DWriteFactory;
		_CreateHitTestGeo();//创建命中测试几何
		return;
	}
	inline void _SetOwnerWndTransparent(bool trans)
	{
		OwnerTransparent = trans;
		return;
	}
	inline void _SetParentCon(DControl* const& control)
	{
		Parent = control;
		return;
	}
	inline DControl* _GetParentCon()
	{
		return Parent;
	}

	virtual void _AddChildCon(DControl* const& control)
	{
		___ChildLock.Lock();

		___PosOfsLock.Lock();
		control->_SetParPosOffPointer(ID, PosOffset, nullptr);
		___PosOfsLock.Unlock();

		Child.insert(std::pair<UINT, DControl*>(control->GetID(), control));
		___ChildLock.Unlock();

		return;
	}

	void _DeleteChildCon(DControl* const& control)
	{
		if (!control)
			return;

		___ChildLock.Lock();
		Child.erase(control->GetID());

		std::map<UINT, float*> clr;
		control->_SetParPosOffPointer(ID, clr, nullptr);

		___ChildLock.Unlock();
		return;
	}
	inline void _ClearChildCon()
	{
		___ChildLock.Lock();
		std::map<UINT, float*> clr;
		std::map<UINT, DControl*>::iterator childit = Child.begin();
		while (childit != Child.end())
		{
			childit->second->_SetParPosOffPointer(ID, clr, nullptr);
			++childit;
		}
		Child.clear();
		___ChildLock.Unlock();
		return;
	}

	inline std::map<UINT, DControl*> _GetChildCon()
	{
		return Child;
	}

	//此函数在底层控件刚绘制完成后调用上层控件的此函数，这时Target里的图片正好可以作为上层控件的背景图
	void _UpdateBackImg(ID2D1DeviceContext* const& Target, D2D1_RECT_F* const& UpdateClip = nullptr)
	{
		if (!Target)
			return;
		//这里是用的父控件的UpdClip
		___BackImgLock.Lock();
		if (!BackImg)
		{
			RELEASE(BackImg_Blur);
			D2D1_BITMAP_PROPERTIES1 prop;
			prop.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
			prop.dpiX = 96.0f;
			prop.dpiY = 96.0f;
			prop.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
			prop.colorContext = nullptr;
			Target->CreateBitmap({ (UINT)ceil(UpdateRC.right - UpdateRC.left), (UINT)ceil(UpdateRC.bottom - UpdateRC.top) }, nullptr, 0, prop, &BackImg);
		}
		if (!BackImg)
		{
			___BackImgLock.Unlock();
			return;
		}

		D2D1_POINT_2U dst;
		D2D1_RECT_U src;
		D2D1_RECT_F WndRc;
		RECT wndrc;
		if (OwnerWnd)
		{
			GetClientRect(OwnerWnd, &wndrc);
			WndRc.left = (float)wndrc.left;
			WndRc.top = (float)wndrc.top;
			WndRc.right = (float)wndrc.right;
			WndRc.bottom = (float)wndrc.bottom;
		}
		else
		{
			wndrc = { 0 };
			WndRc = { 0 };
		}

		D2D1_RECT_F updrc = this->_GetUpdateRC();
		if (UpdateClip)
		{
			___UpdateClipLock.Lock();
			D2D1_RECT_F r = CalcRectOverlap(CalcRectOverlap(*UpdateClip, updrc), WndRc);
			___UpdateClipLock.Unlock();
			if (!IsRectWorkable(r))
			{
				___BackImgLock.Unlock();
				return;
			}

			src.left = (UINT)r.left;
			src.top = (UINT)r.top;
			src.right = (UINT)ceil(r.right);
			src.bottom = (UINT)ceil(r.bottom);
			dst = { (UINT)(r.left - updrc.left), (UINT)(r.top - updrc.top) };
		}
		else
		{
			D2D1_SIZE_U szImg = BackImg->GetPixelSize();
			if (updrc.left < 0.0f)
				dst.x = (UINT)(std::fabsf(updrc.left));
			else
				dst.x = 0;

			if (updrc.top < 0.0f)
				dst.y = (UINT)(std::fabsf(updrc.top));
			else
				dst.y = 0;

			src.left = (UINT)max(updrc.left, 0.0f);
			src.top = (UINT)max(updrc.top, 0.0f);
			src.right = min(src.left + szImg.width, (UINT)wndrc.right);
			src.bottom = min(src.top + szImg.height, (UINT)wndrc.bottom);
		}
		BackImg->CopyFromRenderTarget(&dst, Target, &src);
		if (BlurSigma > 0.0f)
		{
			if (!BackImg_Blur)
			{
				D2D1_BITMAP_PROPERTIES1 prop;
				prop.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
				prop.dpiX = 96.0f;
				prop.dpiY = 96.0f;
				prop.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
				prop.colorContext = nullptr;
				Target->CreateBitmap({ (UINT)ceil(UpdateRC.right - UpdateRC.left), (UINT)ceil(UpdateRC.bottom - UpdateRC.top) }, nullptr, 0, prop, &BackImg_Blur);
			}
			if (!BackImg_Blur)
			{
				___BackImgLock.Unlock();
				return;
			}
			if (!BlurEffect)
				Target->CreateEffect(CLSID_D2D1GaussianBlur, &BlurEffect);

			if (BlurEffect)
			{
				D2D1_SIZE_U sz = BackImg_Blur->GetPixelSize();
				D2D1_RECT_U r;
				r.left = dst.x;
				r.top = dst.y;
				r.right = dst.x + src.right - src.left;
				r.bottom = dst.y + src.bottom - src.top;
				BackImg_Blur->CopyFromBitmap(&dst, BackImg, &r);
				if (UpdateClip)//重新计算扩展后的Clip
				{
					D2D1_RECT_F r1;
					float delta = DIP2DP(BlurSigma) * 3.0f;//因为下面的值是用的XDPI，所以这里统一用X
					r1.left = UpdateClip->left - delta;
					r1.top = UpdateClip->top - delta;
					r1.right = UpdateClip->right + delta;
					r1.bottom = UpdateClip->bottom + delta;
					___UpdateClipLock.Lock();
					r1 = CalcRectOverlap(CalcRectOverlap(r1, updrc), WndRc);
					___UpdateClipLock.Unlock();
					src.left = (UINT)r1.left;
					src.top = (UINT)r1.top;
					src.right = (UINT)ceil(r1.right);
					src.bottom = (UINT)ceil(r1.bottom);
				}

				ID2D1Image* tar;
				Target->GetTarget(&tar);
				Target->SetTarget(BackImg_Blur);
				BlurEffect->SetInput(0, BackImg);
				BlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, DIP2DP(BlurSigma));
				BlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_DIRECTIONALBLUR_OPTIMIZATION_SPEED);
				BlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
				//如果有圆角，则用图层实现圆角
				Target->SetTransform(D2D1::Matrix3x2F::Translation(-UpdateRC.left, -UpdateRC.top));
				Clip.PushClip(Target, { (float)src.left, (float)src.top, (float)src.right, (float)src.bottom });
				if (DstRC.radiusX != 0.0f || DstRC.radiusY != 0.0f)
				{
					___RoundGeoLock.Lock();
					if (!RoundGeo && D2DFactory)
						D2DFactory->CreateRoundedRectangleGeometry(DstRC, &RoundGeo);
					if (RoundGeo)
						Clip.PushLayer(Target, UpdateRC, RoundGeo, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
					___RoundGeoLock.Unlock();
					Target->Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
					Target->DrawImage(BlurEffect, { UpdateRC.left, UpdateRC.top }, { 0.0f, 0.0f, (float)sz.width, (float)sz.height }, D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
					Clip.PopLast(Target);//弹出刚刚推入的层
				}
				else {
					Target->Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
					Target->DrawImage(BlurEffect, { UpdateRC.left, UpdateRC.top }, { 0.0f, 0.0f, (float)sz.width, (float)sz.height }, D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
				}
				Clip.PopLast(Target);
				Target->SetTransform(D2D1::Matrix3x2F::Identity());
				Target->SetTarget(tar);
				RELEASE(tar);//这里返回的也要释放
			}
		}
		___BackImgLock.Unlock();

		return;
	}
	inline void _DrawBackImg(ID2D1DeviceContext* const& Target)
	{
		___BackImgLock.Lock();
		if (BlurSigma > 0.0f)
		{
			if (BackImg_Blur)
			{
				D2D1_SIZE_U szImg = BackImg_Blur->GetPixelSize();
				D2D1_RECT_F upd;
				upd.left = UpdateRC.left;
				upd.top = UpdateRC.top;
				upd.right = upd.left + szImg.width;
				upd.bottom = upd.top + szImg.height;
				ClearRect(Target, upd, { 0.0f, 0.0f, 0.0f, 0.0f });
				Target->DrawBitmap(BackImg_Blur, upd, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
		}
		else
		{
			if (BackImg)
			{
				D2D1_SIZE_U szImg = BackImg->GetPixelSize();
				D2D1_RECT_F upd;
				upd.left = UpdateRC.left;
				upd.top = UpdateRC.top;
				upd.right = upd.left + szImg.width;
				upd.bottom = upd.top + szImg.height;
				ClearRect(Target, upd, { 0.0f, 0.0f, 0.0f, 0.0f });
				Target->DrawBitmap(BackImg, upd, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);//这里必须用Draw，否则无法应用Clip和Transform
			}
		}
		___BackImgLock.Unlock();
		return;
	}

	//输入处理，只有少数需要处理输入的控件用到
	virtual bool _KeyDownMsg(WPARAM wParam)//如果返回true，则应在ReturnValue里设置为0
	{
		return false;
	}
	virtual bool _ImeComposition(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return false;
	}

	//状态及光标设置
	virtual bool _SetCursorMsgProc() = 0;
	virtual bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0) = 0;
	virtual bool _MouseMoveMsgProc(int xPos, int yPos) = 0;
	virtual bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos) = 0;
	virtual bool _LMouseUpMsgProc(int xPos, int yPos) = 0;

	virtual bool _CharMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return false;
	}

	virtual void ReleaseDeviceDependentRes() = 0;
	virtual void _SeparateFromWindow() = 0;

	virtual bool _MouseWheelMsgProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		return false;
	}

	inline void _SetFrameRate(UINT Rate)
	{
		FrameRate = Rate;
		return;
	}

	virtual bool _IsContentChanging() = 0;//内容是否正在改变，不包括Rect改变

	virtual void _DisposeAnimation() = 0;//每一帧都处理用户手动设置的动画，由于需要使Parent窗口更新，所以需要MainWindow主动在获取完UpdateRect的最后调用它，那么下一帧更新将会使变化生效

	virtual bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip) = 0;

	void _CallChild(std::deque<D2D1_RECT_F>& mUpdateClip)
	{
		___ChildLock.Lock();
		if (Child.size())
		{
			std::map<UINT, DControl*>::iterator child = Child.begin();
			std::deque<D2D1_RECT_F>::iterator updit;
			UINT clipi, s = mUpdateClip.size();
			D2D1_RECT_F rover;

			for (UINT i = 0; i < Child.size(); ++i, ++child)
			{
				if (s)
				{
					child->second->ParentDrawing = false;
					//开始根据UpdateClip区域的数量，重复判断更新子控件
					updit = mUpdateClip.begin();
					clipi = 0;
					do
					{
						rover = CalcRectOverlap(child->second->_GetUpdateRC(), *updit);
						if (IsRectWorkable(rover, false))
							child->second->_SetNeedUpdate(*updit);

						++clipi;
						++updit;
					} while (clipi < s);
				}
				else//当前控件没有UpdateClip
				{
					child->second->ParentDrawing = _IsContentChanging();
					if (IsRectWorkable(CalcRectOverlap(child->second->_GetUpdateRC(), this->_GetUpdateRC()), false))
						child->second->_SetNeedUpdate();
				}
			}
		}
		___ChildLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
			RectChanged = false;
		___RcChangedLock.Unlock();
		return;
	}

	void _CallChildUpdateBackImg(ID2D1DeviceContext* const& Target, const std::map<UINT, DControl::__updItem>& UpdCon, const std::map<UINT, DControl::__updItem>::iterator& UpdConIt, std::deque<D2D1_RECT_F>& mUpdateClip)
	{
		std::map<UINT, DControl::__updItem>::iterator mUpdConIt = UpdConIt;
		std::deque<D2D1_RECT_F>::iterator updit;
		UINT clipi, s = mUpdateClip.size();
		++mUpdConIt;
		___ChildLock.Lock();
		while (mUpdConIt != UpdCon.end())
		{
			if (Child.find(mUpdConIt->first) != Child.end())
			{
				if (s)
				{
					//开始根据UpdateClip区域的数量，重复判断更新子控件
					updit = mUpdateClip.begin();
					clipi = 0;
					do
					{
						mUpdConIt->second.con->_UpdateBackImg(Target, &(*updit));
						++clipi;
						++updit;
					} while (clipi < s);
				}
				else//当前控件没有UpdateClip
					mUpdConIt->second.con->_UpdateBackImg(Target);
			}
			++mUpdConIt;
		}
		___ChildLock.Unlock();
		return;
	}
};





class DButton : public DControl
{
public:
	virtual~DButton()
	{
		this->Release();
		return;
	}

	bool Init(POINT position, SIZE size, DButtonTypeStyle Normal, DButtonTypeStyle MouseMove, DButtonTypeStyle Click, DButtonTypeStyle Disable, float radiusX, float radiusY, float StrokeWidth, std::wstring text, std::wstring FamliyName, float FontSize, DFontWeight FontWeight = DFontWeight::Normal)
	{
		if (this->Inited)
			return false;

		this->DstRC_DIP.rect.left = (float)position.x;
		this->DstRC_DIP.rect.top = (float)position.y;
		this->DstSize.width = (float)size.cx;
		this->DstSize.height = (float)size.cy;
		this->DstRC_DIP.rect.right = (float)position.x + this->DstSize.width;
		this->DstRC_DIP.rect.bottom = (float)position.y + this->DstSize.height;
		this->DstRC_DIP.radiusX = radiusX;
		this->DstRC_DIP.radiusY = radiusY;
		this->StrokeWidth = StrokeWidth;
		_UpdateDpi();

		memcpy_s(&this->NormalType, sizeof(this->NormalType), &Normal, sizeof(Normal));
		memcpy_s(&this->MouseMoveType, sizeof(this->MouseMoveType), &MouseMove, sizeof(MouseMove));
		memcpy_s(&this->ClickType, sizeof(this->ClickType), &Click, sizeof(Click));
		memcpy_s(&this->DisableType, sizeof(this->DisableType), &Disable, sizeof(Disable));

		DetectColor(this->NormalType.FillColor);
		DetectColor(this->NormalType.StrokeColor);
		DetectColor(this->NormalType.TextColor);
		DetectColor(this->MouseMoveType.FillColor);
		DetectColor(this->MouseMoveType.StrokeColor);
		DetectColor(this->MouseMoveType.TextColor);
		DetectColor(this->ClickType.FillColor);
		DetectColor(this->ClickType.StrokeColor);
		DetectColor(this->ClickType.TextColor);
		DetectColor(this->DisableType.FillColor);
		DetectColor(this->DisableType.StrokeColor);
		DetectColor(this->DisableType.TextColor);

		//初始化当前样式
		this->NowType.FillColor = DcolToColf(this->NormalType.FillColor);
		this->NowType.StrokeColor = DcolToColf(this->NormalType.StrokeColor);
		this->NowType.TextColor = DcolToColf(this->NormalType.TextColor);

		this->Text = text;
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case DFontWeight::Thin:
			this->FontWeight = DWRITE_FONT_WEIGHT_THIN;
			break;
		case DFontWeight::Normal:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::Light:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::Medium:
			this->FontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
			break;
		case DFontWeight::DemiBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_DEMI_BOLD;
			break;
		case DFontWeight::Bold:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;
		case DFontWeight::ExtraBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
			break;
		case DFontWeight::Heavy:
			this->FontWeight = DWRITE_FONT_WEIGHT_HEAVY;
			break;
		case DFontWeight::ExtraBlack:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
			break;
		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		this->ControlType = DControlType::Button;

		this->Inited = true;
		return true;
	}
	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		A_Col_Fill.Reset();
		A_Col_Stroke.Reset();
		A_Col_Text.Reset();

		ControlType = DControlType::Button;

		NormalType = { 0 };
		MouseMoveType = { 0 };
		ClickType = { 0 };
		DisableType = { 0 };
		NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

		RELEASE(brush);

		//文本区
		Text.clear();
		FamliyName.clear();
		FontSize = 0.0f;
		FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

		RELEASE(TextFormat);
		RELEASE(TextLayout);

		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Radius_X.AddAni(DstRC_DIP.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_Radius_Y.AddAni(DstRC_DIP.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}

		DstRC_DIP.radiusX = RadiusX;
		DstRC_DIP.radiusY = RadiusY;
		_UpdateDpi_DstRCRadius();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();
		this->_SetNeedUpdate();
		return;
	}
	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();
		//重新创建文本裁剪图形
		___LayoutLock.Lock();
		RELEASE(TextLayout);
		___LayoutLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}

	void SetTypeStyle(DControlState State, DButtonTypeStyle type)
	{
		switch (State)
		{
		case DControlState::Normal:
			memcpy_s(&NormalType, sizeof(NormalType), &type, sizeof(type));
			DetectColor(NormalType.FillColor);
			DetectColor(NormalType.StrokeColor);
			DetectColor(NormalType.TextColor);
			break;
		case DControlState::MouseMove:
			memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &type, sizeof(type));
			DetectColor(MouseMoveType.FillColor);
			DetectColor(MouseMoveType.StrokeColor);
			DetectColor(MouseMoveType.TextColor);
			break;
		case DControlState::Click:
			memcpy_s(&ClickType, sizeof(ClickType), &type, sizeof(type));
			DetectColor(ClickType.FillColor);
			DetectColor(ClickType.StrokeColor);
			DetectColor(ClickType.TextColor);
			break;
		case DControlState::Disable:
			memcpy_s(&DisableType, sizeof(DisableType), &type, sizeof(type));
			DetectColor(DisableType.FillColor);
			DetectColor(DisableType.StrokeColor);
			DetectColor(DisableType.TextColor);
			break;
		}
		switch (this->State)
		{
		case DControlState::Normal:
			NowType.FillColor = DcolToColf(NormalType.FillColor);
			NowType.StrokeColor = DcolToColf(NormalType.StrokeColor);
			NowType.TextColor = DcolToColf(NormalType.TextColor);
			break;
		case DControlState::MouseMove:
			NowType.FillColor = DcolToColf(MouseMoveType.FillColor);
			NowType.StrokeColor = DcolToColf(MouseMoveType.StrokeColor);
			NowType.TextColor = DcolToColf(MouseMoveType.TextColor);
			break;
		case DControlState::Click:
			NowType.FillColor = DcolToColf(ClickType.FillColor);
			NowType.StrokeColor = DcolToColf(ClickType.StrokeColor);
			NowType.TextColor = DcolToColf(ClickType.TextColor);
			break;
		case DControlState::Disable:
			NowType.FillColor = DcolToColf(DisableType.FillColor);
			NowType.StrokeColor = DcolToColf(DisableType.StrokeColor);
			NowType.TextColor = DcolToColf(DisableType.TextColor);
			break;
		}

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

	inline void SetText(std::wstring text)
	{
		___LayoutLock.Lock();
		Text = text;
		RELEASE(TextLayout);//文字发生更改，必须重新创建(Draw时自动重建)，没有修改的方法
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

	inline std::wstring GetText()
	{
		return Text;
	}

	void SetFontFormat(std::wstring FamliyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case DFontWeight::Thin:
			this->FontWeight = DWRITE_FONT_WEIGHT_THIN;
			break;
		case DFontWeight::Normal:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::Light:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::Medium:
			this->FontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
			break;
		case DFontWeight::DemiBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_DEMI_BOLD;
			break;
		case DFontWeight::Bold:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;
		case DFontWeight::ExtraBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
			break;
		case DFontWeight::Heavy:
			this->FontWeight = DWRITE_FONT_WEIGHT_HEAVY;
			break;
		case DFontWeight::ExtraBlack:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
			break;
		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		___LayoutLock.Lock();
		if (TextLayout)
		{
			TextLayout->SetFontFamilyName(this->FamliyName.c_str(), { 0, (UINT)Text.size() });
			TextLayout->SetFontSize(DIP2DP(this->FontSize), { 0, (UINT)Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, (UINT)Text.size() });
		}
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

private:
	DButtonTypeStyle NormalType = { 0 };
	DButtonTypeStyle MouseMoveType = { 0 };
	DButtonTypeStyle ClickType = { 0 };
	DButtonTypeStyle DisableType = { 0 };

	struct Type
	{
		D2D1_COLOR_F FillColor;
		D2D1_COLOR_F StrokeColor;
		D2D1_COLOR_F TextColor;
	};

	_Ani_Color A_Col_Fill;
	_Ani_Color A_Col_Stroke;
	_Ani_Color A_Col_Text;

	Type NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

	ID2D1SolidColorBrush* brush = nullptr;

	//文本区
	std::wstring Text;
	std::wstring FamliyName;
	float FontSize = 0.0f;
	DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

	IDWriteTextFormat* TextFormat = nullptr;
	IDWriteTextLayout* TextLayout = nullptr;
	___DThreadLock ___LayoutLock;


	//*************************DXUI自用函数*************************

	void _SeparateFromWindow()
	{
		//重新设置动画颜色，防止下一次加入窗口时还没恢复状态
		NowType.FillColor = DcolToColf(NormalType.FillColor);
		NowType.StrokeColor = DcolToColf(NormalType.StrokeColor);
		NowType.TextColor = DcolToColf(NormalType.TextColor);
		A_Col_Fill.Reset();
		A_Col_Stroke.Reset();
		A_Col_Text.Reset();
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(RoundGeo);
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(TextFormat);
		RELEASE(TextLayout);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(HitTestGeo);
		return;
	}

	bool _IsContentChanging()
	{
		if (A_Col_Fill || A_Col_Stroke || A_Col_Text || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa)
			return true;
		else
			return false;
	}

	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		if (State != state)
		{
			float time = 0.0f;
			DCOLOR* fill = nullptr, * stroke = nullptr, * text = nullptr;
			switch (state)
			{
			case DControlState::Normal:
				time = NormalType.time / 1000.0f;
				fill = &NormalType.FillColor;
				stroke = &NormalType.StrokeColor;
				text = &NormalType.TextColor;
				break;
			case DControlState::MouseMove:
				time = MouseMoveType.time / 1000.0f;
				fill = &MouseMoveType.FillColor;
				stroke = &MouseMoveType.StrokeColor;
				text = &MouseMoveType.TextColor;
				break;
			case DControlState::Click:
				time = ClickType.time / 1000.0f;
				fill = &ClickType.FillColor;
				stroke = &ClickType.StrokeColor;
				text = &ClickType.TextColor;
				break;
			case DControlState::Disable:
				time = DisableType.time / 1000.0f;
				fill = &DisableType.FillColor;
				stroke = &DisableType.StrokeColor;
				text = &DisableType.TextColor;
				break;
			}
			DControlState LastState = State;
			State = state;

			if (fill && stroke && text)
			{
				//填充颜色
				A_Col_Fill.AddAni(NowType.FillColor, *fill, (UINT)(FrameRate * time));
				//描边颜色
				A_Col_Stroke.AddAni(NowType.StrokeColor, *stroke, (UINT)(FrameRate * time));
				//文字颜色
				A_Col_Text.AddAni(NowType.TextColor, *text, (UINT)(FrameRate * time));
			}

			this->_SetNeedUpdate();

			mSendConMsg(DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);

			if (NeedUnlock)
				___StateLock.Unlock();
			return true;
		}
		if (NeedUnlock)
			___StateLock.Unlock();
		return false;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		DstRC.rect.left = floor(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = floor(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = floor(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = floor(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			UpdateRC.left = floor(DstRC.rect.left - DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floor(DstRC.rect.top - DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceil(DstRC.rect.right + DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceil(DstRC.rect.bottom + DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			UpdateRC = DstRC.rect;
		}
		return;
	}

	bool _SetCursorMsgProc()
	{
		if (State == DControlState::Disable)
			return false;

		SetCursor(this->CursorStyle);
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		___StateLock.Lock();
		if (State == DControlState::Disable || NowControlID == ID)
		{
			___StateLock.Unlock();
			return false;
		}

		if (Msg == WM_MOUSEMOVE && OnClick)
		{
			___StateLock.Unlock();
			return false;
		}
		else if (Msg == WM_LBUTTONUP)
			OnClick = false;

		return m_SetState(DControlState::Normal, true);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		___StateLock.Lock();
		if (State == DControlState::Disable || State == DControlState::Click)
		{
			___StateLock.Unlock();
			return false;
		}

		if (OnClick)
			return m_SetState(DControlState::Click, true);

		return m_SetState(DControlState::MouseMove, true);
	}
	bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		___StateLock.Lock();
		if (State == DControlState::Disable)
		{
			___StateLock.Unlock();
			return false;
		}
		OnClick = true;

		return m_SetState(DControlState::Click, true);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		___StateLock.Lock();
		if (State == DControlState::Disable || !OnClick || (OnClick && !this->HitTest(xPos, yPos)))
		{
			___StateLock.Unlock();
			OnClick = false;
			return false;
		}
		OnClick = false;

		return m_SetState(DControlState::MouseMove, true);
	}

	bool _CreateHitTestGeo()//创建命中测试几何
	{
		___HitTestLock.Lock();
		RELEASE(HitTestGeo);
		if (D2DFactory)
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DIP2DP(DstSize.width + StrokeWidth), DIP2DP(DstSize.height + StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Col_Fill)
		{
			A_Col_Fill.FrameCalc(NowType.FillColor);
			this->_SetNeedUpdate();
		}
		if (A_Col_Stroke)
		{
			A_Col_Stroke.FrameCalc(NowType.StrokeColor);
			this->_SetNeedUpdate();
		}
		if (A_Col_Text)
		{
			A_Col_Text.FrameCalc(NowType.TextColor);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(AID);
				if (!A_Radius_Y)
					resp = true;
			}
			SetRadius(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//处理动画的颜色变换
		D2D1_COLOR_F f_FillColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_StrokeColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_TextColor;//以1.0f为最大的颜色结构体

		//动画——填充颜色
		f_FillColor = Calc_fColor(NowType.FillColor, TotalOpacity);
		//动画——描边颜色
		f_StrokeColor = Calc_fColor(NowType.StrokeColor, TotalOpacity);
		//动画——文字颜色
		f_TextColor = Calc_fColor(NowType.TextColor, TotalOpacity);

		//********动画颜色处理结束**********

		//尽量缩小锁的锁定时间，靠下放
		___LayoutLock.Lock();
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamliyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DIP2DP(FontSize), LocaleName, &TextFormat);
			TextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

			RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
		}
		if (!TextLayout && TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextLayout(Text.c_str(), Text.size(), TextFormat, DIP2DP(DstSize.width), DIP2DP(DstSize.height), &TextLayout);
		}

		if (f_FillColor.a != 0.0f)
		{
			brush->SetColor(f_FillColor);//设置为填充颜色
			DstTarget->FillRoundedRectangle(DstRC, brush);
		}
		if (f_StrokeColor.a != 0.0f)
		{
			brush->SetColor(f_StrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		if (TextLayout && f_TextColor.a != 0.0f)
		{
			//如果文字超出目标区域，则裁剪
			DWRITE_TEXT_METRICS met;
			TextLayout->GetMetrics(&met);
			if (met.widthIncludingTrailingWhitespace >= met.layoutWidth || met.height >= met.layoutHeight)
				Clip.PushClip(DstTarget, DstRC.rect);
			
			brush->SetColor(f_TextColor);//设置为文字颜色
			DstTarget->DrawTextLayout({ (float)DstRC.rect.left, (float)DstRC.rect.top }, TextLayout, brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		}
		___LayoutLock.Unlock();

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}

};






class DLabel : public DControl
{
public:
	virtual~DLabel()
	{
		this->Release();
		return;
	}
	inline bool Init(POINT position, SIZE size, DCOLOR color, bool CanMoveWindow, std::wstring text, std::wstring FamliyName, float FontSize, bool AutoWarp = true, DFontWeight FontWeight = DFontWeight::Normal, DAlignment X_Alignment = DAlignment::Near, DAlignment Y_Alignment = DAlignment::Center)
	{
		return Init(position, size, { color, false, false, 0 }, { color, false, false, 0 }, { color, false, false, 0 }, CanMoveWindow, text, FamliyName, FontSize, AutoWarp, FontWeight, X_Alignment, Y_Alignment);
	}
	bool Init(POINT position, SIZE size, DLabelTypeStyle Normal, DLabelTypeStyle MouseMove, DLabelTypeStyle Click, bool CanMoveWindow, std::wstring text, std::wstring FamliyName, float FontSize, bool AutoWarp = true, DFontWeight FontWeight = DFontWeight::Normal, DAlignment X_Alignment = DAlignment::Near, DAlignment Y_Alignment = DAlignment::Center)
	{
		if (this->Inited)
			return false;

		this->CanMoveWindow = CanMoveWindow;

		this->DstRC_DIP.rect.left = (float)position.x;
		this->DstRC_DIP.rect.top = (float)position.y;
		this->DstSize.width = (float)size.cx;
		this->DstSize.height = (float)size.cy;
		this->DstRC_DIP.rect.right = (float)position.x + this->DstSize.width;
		this->DstRC_DIP.rect.bottom = (float)position.y + this->DstSize.height;
		this->DstRC_DIP.radiusX = 0.0f;
		this->DstRC_DIP.radiusY = 0.0f;
		_UpdateDpi();

		memcpy_s(&this->NormalType, sizeof(this->NormalType), &Normal, sizeof(Normal));
		memcpy_s(&this->MouseMoveType, sizeof(this->MouseMoveType), &MouseMove, sizeof(MouseMove));
		memcpy_s(&this->ClickType, sizeof(this->ClickType), &Click, sizeof(Click));

		DetectColor(this->NormalType.TextColor);
		DetectColor(this->MouseMoveType.TextColor);
		DetectColor(this->ClickType.TextColor);

		//初始化当前样式
		this->NowType = DcolToColf(this->NormalType.TextColor);

		this->Text = text;
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		if (AutoWarp)//处理自动换行
			this->AutoWarp = DWRITE_WORD_WRAPPING_WRAP;
		else
			this->AutoWarp = DWRITE_WORD_WRAPPING_NO_WRAP;

		switch (FontWeight)
		{
		case DFontWeight::Thin:
			this->FontWeight = DWRITE_FONT_WEIGHT_THIN;
			break;
		case DFontWeight::Normal:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::Light:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::Medium:
			this->FontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
			break;
		case DFontWeight::DemiBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_DEMI_BOLD;
			break;
		case DFontWeight::Bold:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;
		case DFontWeight::ExtraBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
			break;
		case DFontWeight::Heavy:
			this->FontWeight = DWRITE_FONT_WEIGHT_HEAVY;
			break;
		case DFontWeight::ExtraBlack:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
			break;
		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		switch (X_Alignment)
		{
		case DAlignment::Near:
			this->xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		case DAlignment::Center:
			this->xAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			break;
		case DAlignment::Far:
			this->xAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
			break;

		default:
			this->xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		}
		switch (Y_Alignment)
		{
		case DAlignment::Near:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		case DAlignment::Center:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			break;
		case DAlignment::Far:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
			break;

		default:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		}

		this->ControlType = DControlType::Label;

		this->Inited = true;
		return true;
	}
	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		A_Col.Reset();

		ControlType = DControlType::Label;

		NormalType = { 0 };
		MouseMoveType = { 0 };
		ClickType = { 0 };
		NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

		RELEASE(brush);

		//文本区
		Text.clear();
		FamliyName.clear();
		FontSize = 0.0f;
		AutoWarp = DWRITE_WORD_WRAPPING_WRAP;

		xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
		yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		FontWeight = DWRITE_FONT_WEIGHT_NORMAL;


		RELEASE(TextFormat);
		RELEASE(TextLayout);

		return;
	}

	void SetAlignment(DAlignment X_Alignment, DAlignment Y_Alignment)
	{
		switch (X_Alignment)
		{
		case DAlignment::Near:
			xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		case DAlignment::Center:
			xAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			break;
		case DAlignment::Far:
			xAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
			break;

		default:
			xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		}
		switch (Y_Alignment)
		{
		case DAlignment::Near:
			yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		case DAlignment::Center:
			yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			break;
		case DAlignment::Far:
			yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
			break;

		default:
			yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		}

		if (TextFormat)
		{
			TextFormat->SetWordWrapping(AutoWarp);
			TextFormat->SetTextAlignment(xAlignment);
			TextFormat->SetParagraphAlignment(yAlignment);

			___LayoutLock.Lock();
			RELEASE(TextLayout);//Format改了，Layout也要重新创建
			___LayoutLock.Unlock();
		}
		this->_SetNeedUpdate();
		return;
	}

	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface();//这里立即重建，是因为Hittest时如果Layout为Null，则直接失败导致状态频繁切换
		___LayoutLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}


	void SetTypeStyle(DControlState State, DLabelTypeStyle type)
	{
		switch (State)
		{
		case DControlState::Normal:
			memcpy_s(&NormalType, sizeof(NormalType), &type, sizeof(NormalType));
			DetectColor(NormalType.TextColor);
			break;
		case DControlState::MouseMove:
			memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &type, sizeof(MouseMoveType));
			DetectColor(MouseMoveType.TextColor);
			break;
		case DControlState::Click:
			memcpy_s(&ClickType, sizeof(ClickType), &type, sizeof(ClickType));
			DetectColor(ClickType.TextColor);
			break;
		}

		switch (this->State)
		{
		case DControlState::Normal:
			NowType = DcolToColf(NormalType.TextColor);
			break;
		case DControlState::MouseMove:
			NowType = DcolToColf(MouseMoveType.TextColor);
			break;
		case DControlState::Click:
			NowType = DcolToColf(ClickType.TextColor);
			break;
		}

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

	inline void SetText(std::wstring text)
	{
		___LayoutLock.Lock();
		Text = text;
		RELEASE(TextLayout);//文字发生更改，必须重新创建(Draw时自动重建)，没有修改的方法

		RecreateTextInterface();//这里立即重建，是因为Hittest时如果Layout为Null，则直接失败导致状态频繁切换
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

	inline std::wstring GetText()
	{
		return Text;
	}

	void SetFontFormat(std::wstring FamliyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case DFontWeight::Thin:
			this->FontWeight = DWRITE_FONT_WEIGHT_THIN;
			break;
		case DFontWeight::Normal:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::Light:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::Medium:
			this->FontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
			break;
		case DFontWeight::DemiBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_DEMI_BOLD;
			break;
		case DFontWeight::Bold:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;
		case DFontWeight::ExtraBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
			break;
		case DFontWeight::Heavy:
			this->FontWeight = DWRITE_FONT_WEIGHT_HEAVY;
			break;
		case DFontWeight::ExtraBlack:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
			break;
		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		___LayoutLock.Lock();
		if (TextLayout)
		{
			TextLayout->SetFontFamilyName(this->FamliyName.c_str(), { 0, (UINT)Text.size() });
			TextLayout->SetFontSize(DIP2DP(this->FontSize), { 0, (UINT)Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, (UINT)Text.size() });
		}
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

private:
	DLabelTypeStyle NormalType = { 0 };
	DLabelTypeStyle MouseMoveType = { 0 };
	DLabelTypeStyle ClickType = { 0 };

	_Ani_Color A_Col;
	D2D1_COLOR_F NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

	ID2D1SolidColorBrush* brush = nullptr;

	//文本区
	std::wstring Text;
	std::wstring FamliyName;
	float FontSize = 0.0f;
	DWRITE_WORD_WRAPPING AutoWarp = DWRITE_WORD_WRAPPING_WRAP;

	DWRITE_TEXT_ALIGNMENT xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
	DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

	IDWriteTextFormat* TextFormat = nullptr;
	IDWriteTextLayout* TextLayout = nullptr;
	___DThreadLock ___LayoutLock;


	void _SeparateFromWindow()
	{
		//重新设置动画颜色，防止下一次加入窗口时还没恢复状态
		NowType = DcolToColf(NormalType.TextColor);
		A_Col.Reset();
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(TextFormat);
		RELEASE(TextLayout);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		return;
	}

	bool _IsContentChanging()
	{
		if (A_Col || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Opa)
			return true;
		else
			return false;
	}

	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		if (State != state && state != DControlState::Disable)
		{
			float time = 0.0f;
			DCOLOR* text = nullptr;
			switch (state)
			{
			case DControlState::Normal:
				time = NormalType.time / 1000.0f;
				text = &NormalType.TextColor;
				break;
			case DControlState::MouseMove:
				time = MouseMoveType.time / 1000.0f;
				text = &MouseMoveType.TextColor;
				break;
			case DControlState::Click:
				time = ClickType.time / 1000.0f;
				text = &ClickType.TextColor;
				break;
			}
			DControlState LastState = State;
			State = state;

			//文字颜色
			if (text)
				A_Col.AddAni(NowType, *text, (UINT)(FrameRate * time));

			this->_SetNeedUpdate();

			mSendConMsg(DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);

			if (NeedUnlock)
				___StateLock.Unlock();
			return true;
		}
		if (NeedUnlock)
			___StateLock.Unlock();
		return false;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		DstRC.rect.left = floor(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = floor(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = floor(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = floor(DIP2DP(DstRC_DIP.rect.bottom));
		UpdateRC = DstRC.rect;

		return;
	}

	bool _SetCursorMsgProc()
	{
		SetCursor(this->CursorStyle);
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		if (NowControlID == ID)
			return false;

		if (Msg == WM_MOUSEMOVE && OnClick)
			return false;
		else if (Msg == WM_LBUTTONUP)
			OnClick = false;

		return m_SetState(DControlState::Normal);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		___StateLock.Lock();
		if (State == DControlState::Click)
		{
			___StateLock.Unlock();
			return false;
		}

		if (OnClick)
			return m_SetState(DControlState::Click, true);

		return m_SetState(DControlState::MouseMove, true);
	}
	bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos)
	{
		OnClick = true;

		return m_SetState(DControlState::Click);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		if (!OnClick || (OnClick && !this->HitTest(xPos, yPos)))
		{
			OnClick = false;
			return false;
		}
		OnClick = false;

		return m_SetState(DControlState::MouseMove);
	}

	bool _CreateHitTestGeo()//绘制命中测试BMP，由于标签用Dwrite库命中测试，所以Hittest不弄了
	{
		return true;
	}

	///<summary>
	///参数均为设备相关像素(DP)
	///</summary>
	bool HitTest(int xPos, int yPos)
	{
		float pofs = GetPosOffset();
		int mx = xPos, my = yPos;

		if (mx < DstRC.rect.left || mx > DstRC.rect.right || my < DstRC.rect.top - pofs || my > DstRC.rect.bottom - pofs)
			return false;//这一步是因为，DWrite库的HitTest不受DstRC限制，所以提前判断点是否在用户指定区域内

		bool value = false;
		___LayoutLock.Lock();
		if (TextLayout)
		{
			DWRITE_HIT_TEST_METRICS hit;
			BOOL IsBehind = false;
			BOOL isInside = false;
			TextLayout->HitTestPoint((float)(mx - DstRC.rect.left), (float)(my - DstRC.rect.top + ceil(pofs)), &IsBehind, &isInside, &hit);
			___LayoutLock.Unlock();

			if (Parent)
			{
				value = Parent->HitTest(xPos, yPos) && isInside;
			}
			else//没有父控件，那就说明此Hittest是不需要再次确认的
				value = isInside;
		}
		else
			___LayoutLock.Unlock();

		return value;
	}
	bool _HitTestBmpToMemory()
	{
		return true;
	}
	void _DrawHitTestBmp()
	{
		return;
	}

	inline void RecreateTextInterface()//注意，此函数不上锁，要放在锁定区域内调用
	{
		bool locked = false;
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamliyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DIP2DP(FontSize), LocaleName, &TextFormat);
			if (TextFormat)
			{
				TextFormat->SetWordWrapping(AutoWarp);
				TextFormat->SetTextAlignment(xAlignment);
				TextFormat->SetParagraphAlignment(yAlignment);

				locked = true;
				___LayoutLock.Lock();
				RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
			}
		}

		if (!TextLayout && TextFormat && DWriteFactory)
			DWriteFactory->CreateTextLayout(Text.c_str(), Text.size(), TextFormat, ceil(DIP2DP(DstSize.width)), ceil(DIP2DP(DstSize.height)), &TextLayout);

		if (locked)
			___LayoutLock.Unlock();
		return;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Col)
		{
			A_Col.FrameCalc(NowType);
			this->_SetNeedUpdate();
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//处理动画的颜色变换
		DLabelTypeStyle* DstType = &NormalType;

		switch (State)
		{
		case DControlState::Normal:
		{
			DstType = &NormalType;
			break;
		}
		case DControlState::MouseMove:
		{
			DstType = &MouseMoveType;
			break;
		}
		case DControlState::Click:
		{
			DstType = &ClickType;
			break;
		}
		}

		D2D1_COLOR_F f_TextColor;//以1.0f为最大的颜色结构体
		//动画——文字颜色处理
		f_TextColor = Calc_fColor(NowType, TotalOpacity);
		//********动画颜色处理部分结束********

		//尽量让锁的锁定时间减少，所以靠下放
		___LayoutLock.Lock();

		this->RecreateTextInterface();
		//设置当前状态下的文本样式
		if (TextLayout)
		{
			TextLayout->SetStrikethrough(DstType->Strikethrough, { 0, (UINT)Text.size() });//下划线、删除线
			TextLayout->SetUnderline(DstType->Underline, { 0, (UINT)Text.size() });
		}

		//设置当前文本的裁剪区
		Clip.PushClip(DstTarget, UpdateRC);

		if (TextLayout && f_TextColor.a != 0.0f)
		{
			brush->SetColor(f_TextColor);//设置为文字颜色
			DstTarget->DrawTextLayout({ DstRC.rect.left, DstRC.rect.top }, TextLayout, brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		}
		___LayoutLock.Unlock();

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}
};





class DTextBox : public DControl
{
public:
	//当前控件的光标样式
	HCURSOR CursorStyle = LoadCursorW(nullptr, IDC_IBEAM);

	virtual~DTextBox()
	{
		this->Release();
		return;
	}
	bool Init(POINT position, SIZE size, DTextBoxTypeStyle Normal, DTextBoxTypeStyle MouseMove, DTextBoxTypeStyle Edit, DTextBoxTypeStyle Disable, float radiusX, float radiusY, float StrokeWidth, std::wstring FamliyName, float FontSize, bool AutoWarp = true, bool MultiLine = true, bool ReadOnly = false, DFontWeight FontWeight = DFontWeight::Normal, DAlignment X_Alignment = DAlignment::Near, DAlignment Y_Alignment = DAlignment::Near, DCOLOR CaretColor = { 0, 0, 0, 1.0f }, float CaretWidth = 1.5f, DCOLOR SelectFillColor = { 0, 0, 0, 0.45f }, bool SmoothScrolling = true, bool SmoothCaret = true)
	{
		if (this->Inited)
			return false;

		this->DstRC_DIP.rect.left = (float)position.x;
		this->DstRC_DIP.rect.top = (float)position.y;
		this->DstSize.width = (float)size.cx;
		this->DstSize.height = (float)size.cy;
		this->DstRC_DIP.rect.right = (float)position.x + this->DstSize.width;
		this->DstRC_DIP.rect.bottom = (float)position.y + this->DstSize.height;
		this->DstRC_DIP.radiusX = radiusX;
		this->DstRC_DIP.radiusY = radiusY;
		this->StrokeWidth = StrokeWidth;
		_UpdateDpi();

		memcpy_s(&this->NormalType, sizeof(this->NormalType), &Normal, sizeof(Normal));
		memcpy_s(&this->MouseMoveType, sizeof(this->MouseMoveType), &MouseMove, sizeof(MouseMove));
		memcpy_s(&this->EditType, sizeof(this->EditType), &Edit, sizeof(Edit));
		memcpy_s(&this->DisableType, sizeof(this->DisableType), &Disable, sizeof(Disable));

		DetectColor(this->NormalType.FillColor);
		DetectColor(this->NormalType.StrokeColor);
		DetectColor(this->NormalType.TextColor);
		DetectColor(this->MouseMoveType.FillColor);
		DetectColor(this->MouseMoveType.StrokeColor);
		DetectColor(this->MouseMoveType.TextColor);
		DetectColor(this->EditType.FillColor);
		DetectColor(this->EditType.StrokeColor);
		DetectColor(this->EditType.TextColor);
		DetectColor(this->DisableType.FillColor);
		DetectColor(this->DisableType.StrokeColor);
		DetectColor(this->DisableType.TextColor);
		DetectColor(CaretColor);
		DetectColor(SelectFillColor);

		//初始化当前样式
		this->NowType.FillColor = DcolToColf(this->NormalType.FillColor);
		this->NowType.StrokeColor = DcolToColf(this->NormalType.StrokeColor);
		this->NowType.TextColor = DcolToColf(this->NormalType.TextColor);

		this->CaretColor = DcolToColf_255(CaretColor);
		this->CaretWidth = CaretWidth;
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;
		this->MultiLine = MultiLine;
		this->ReadOnly = ReadOnly;
		this->SmoothScr = SmoothScrolling;
		this->SmoothIns = SmoothCaret;

		if (AutoWarp)//处理自动换行
		{
			if (MultiLine)
				this->AutoWarp = DWRITE_WORD_WRAPPING_WRAP;
			else this->AutoWarp = DWRITE_WORD_WRAPPING_NO_WRAP;
		}
		else
			this->AutoWarp = DWRITE_WORD_WRAPPING_NO_WRAP;

		switch (FontWeight)
		{
		case DFontWeight::Thin:
			this->FontWeight = DWRITE_FONT_WEIGHT_THIN;
			break;
		case DFontWeight::Normal:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::Light:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::Medium:
			this->FontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
			break;
		case DFontWeight::DemiBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_DEMI_BOLD;
			break;
		case DFontWeight::Bold:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;
		case DFontWeight::ExtraBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
			break;
		case DFontWeight::Heavy:
			this->FontWeight = DWRITE_FONT_WEIGHT_HEAVY;
			break;
		case DFontWeight::ExtraBlack:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
			break;
		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		switch (X_Alignment)
		{
		case DAlignment::Near:
			this->xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		case DAlignment::Center:
			this->xAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			break;
		case DAlignment::Far:
			this->xAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
			break;

		default:
			this->xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		}
		switch (Y_Alignment)
		{
		case DAlignment::Near:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		case DAlignment::Center:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			break;
		case DAlignment::Far:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
			break;

		default:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		}

		this->Select_FillColor = DcolToColf_255(SelectFillColor);
		this->Insert_LastUpdatetime = timeGetTime();
		this->ControlType = DControlType::TextBox;

		this->Inited = true;
		return true;
	}
	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		A_Col_Fill.Reset();
		A_Col_Stroke.Reset();
		A_Col_Text.Reset();
		A_ScrX.Reset();
		A_ScrY.Reset();

		ControlType = DControlType::TextBox;

		HaveSetNormal = false;
		NormalType = { 0 };
		MouseMoveType = { 0 };
		EditType = { 0 };
		DisableType = { 0 };
		NowType = { 0 };//存储以255为最大的颜色
		RELEASE(brush);
		//文本区
		RELEASE(SelfTextClip);//自身的文字裁剪区，防止文字超出范围
		Text.clear();
		FamliyName.clear();
		FontSize = 0.0f;
		MultiLine = true;
		ReadOnly = false;
		SmoothScr = true;
		SmoothIns = true;
		TextOutputRC = { 0 };
		AutoWarp = DWRITE_WORD_WRAPPING_WRAP;
		FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
		xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
		yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		//插入符
		Insert_Visible = false;
		Insert_LastUpdatetime = 0;
		Insert_TextPos = 0;
		Insert_PixelPos = { 0 };
		CaretColor = { 0, 0, 0, 1.0f };
		CaretWidth = 1.5f;
		//选中文本
		SelectMode = false;
		Select_Metrics = { 0 };
		DeleteArray(Select_Rects);
		Select_RectCount = 0;
		Select_DownPos = 0;
		Select_BeginPos = 0;
		Select_EndPos = 0;
		Select_FillColor = { 0 };
		//组合字符串
		Comp_Str.clear();
		Comp_InsPos = 0;
		//文本滚动
		Scr_Offset = { 0 };
		//文本接口
		RELEASE(TextFormat);
		RELEASE(TextLayout);

		return;
	}
	void SetPasswordMode(bool enable)
	{
		if (Password != enable)
		{
			___LayoutLock.Lock();
			Password = enable;
			RELEASE(TextLayout);
			___LayoutLock.Unlock();
			this->_SetNeedUpdate();
		}
		return;
	}
	void SetEnglishInputMode(bool enable)
	{
		EndlishInput = enable;
		return;
	}
	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Radius_X.AddAni(DstRC_DIP.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_Radius_Y.AddAni(DstRC_DIP.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}

		DstRC_DIP.radiusX = RadiusX;
		DstRC_DIP.radiusY = RadiusY;
		_UpdateDpi_DstRCRadius();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();
		this->_SetNeedUpdate();
		return;
	}
	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();
		//重新创建文本裁剪图形
		___TextClipLock.Lock();
		RELEASE(SelfTextClip);
		___TextClipLock.Unlock();

		TextPosToInsertPos(true, false, false);

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();

		//重新创建HitTest
		this->_CreateHitTestGeo();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();
		//重新创建文本裁剪图形
		___TextClipLock.Lock();
		RELEASE(SelfTextClip);
		___TextClipLock.Unlock();

		___LayoutLock.Lock();
		if (TextLayout)
		{
			TextLayout->SetMaxWidth(TextOutputRC.right - TextOutputRC.left);
			TextLayout->SetMaxHeight(TextOutputRC.bottom - TextOutputRC.top);
			TextLayout->GetMetrics(&Select_Metrics);
			if (SelectMode)
				UpdateSelectPos(false, false);
			else
				TextPosToInsertPos(false, SmoothScr, SmoothIns, true);
		}
		___LayoutLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}

	void SetCaretColor(DCOLOR co)
	{
		DCOLOR CaretC;
		memcpy_s(&CaretC, sizeof(CaretC), &CaretColor, sizeof(CaretColor));

		this->CaretColor.r = CaretC.r / 255.0f;
		this->CaretColor.g = CaretC.g / 255.0f;
		this->CaretColor.b = CaretC.b / 255.0f;
		this->CaretColor.a = CaretC.a;
		return;
	}
	void SetCaretWidth(float wid)
	{
		this->CaretWidth = wid;
		Insert_PixelPos.right = (UINT)(Insert_PixelPos.left + DIP2DP(this->CaretWidth));
		return;
	}

	void SetTypeStyle(DControlState State, DTextBoxTypeStyle type)
	{
		switch (State)
		{
		case DControlState::Normal:
			memcpy_s(&NormalType, sizeof(NormalType), &type, sizeof(type));
			DetectColor(NormalType.FillColor);
			DetectColor(NormalType.StrokeColor);
			DetectColor(NormalType.TextColor);
			break;
		case DControlState::MouseMove:
			memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &type, sizeof(type));
			DetectColor(MouseMoveType.FillColor);
			DetectColor(MouseMoveType.StrokeColor);
			DetectColor(MouseMoveType.TextColor);
			break;
		case DControlState::Edit:
			memcpy_s(&EditType, sizeof(EditType), &type, sizeof(type));
			DetectColor(EditType.FillColor);
			DetectColor(EditType.StrokeColor);
			DetectColor(EditType.TextColor);
			break;
		case DControlState::Disable:
			memcpy_s(&DisableType, sizeof(DisableType), &type, sizeof(type));
			DetectColor(DisableType.FillColor);
			DetectColor(DisableType.StrokeColor);
			DetectColor(DisableType.TextColor);
			break;
		}

		switch (this->State)
		{
		case DControlState::Normal:
			NowType.FillColor = DcolToColf(NormalType.FillColor);
			NowType.StrokeColor = DcolToColf(NormalType.StrokeColor);
			NowType.TextColor = DcolToColf(NormalType.TextColor);
			break;
		case DControlState::MouseMove:
			NowType.FillColor = DcolToColf(MouseMoveType.FillColor);
			NowType.StrokeColor = DcolToColf(MouseMoveType.StrokeColor);
			NowType.TextColor = DcolToColf(MouseMoveType.TextColor);
			break;
		case DControlState::Click:
			NowType.FillColor = DcolToColf(EditType.FillColor);
			NowType.StrokeColor = DcolToColf(EditType.StrokeColor);
			NowType.TextColor = DcolToColf(EditType.TextColor);
			break;
		case DControlState::Disable:
			NowType.FillColor = DcolToColf(DisableType.FillColor);
			NowType.StrokeColor = DcolToColf(DisableType.StrokeColor);
			NowType.TextColor = DcolToColf(DisableType.TextColor);
			break;
		}

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}


	void SetFontFormat(std::wstring FamliyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case DFontWeight::Thin:
			this->FontWeight = DWRITE_FONT_WEIGHT_THIN;
			break;
		case DFontWeight::Normal:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::Light:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::Medium:
			this->FontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
			break;
		case DFontWeight::DemiBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_DEMI_BOLD;
			break;
		case DFontWeight::Bold:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;
		case DFontWeight::ExtraBold:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
			break;
		case DFontWeight::Heavy:
			this->FontWeight = DWRITE_FONT_WEIGHT_HEAVY;
			break;
		case DFontWeight::ExtraBlack:
			this->FontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
			break;
		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		___LayoutLock.Lock();
		if (TextLayout)
		{
			___TextLock.Lock();
			TextLayout->SetFontFamilyName(this->FamliyName.c_str(), { 0, (UINT)Text.size() });
			TextLayout->SetFontSize(DIP2DP(this->FontSize), { 0, (UINT)Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, (UINT)Text.size() });
			___TextLock.Unlock();
		}
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

	///<summary>
	///在文本后端插入一个字符，然后把Caret移动到此处。此函数会进行边界检查
	///</summary>
	void AddChar(const wchar_t Ch, bool MoveCaret = true)
	{
		___TextLock.Lock();

		if (!MultiLine && (Ch == L'\r' || Ch == L'\n'))
			Text += L' ';
		else
			Text += Ch;

		if (MoveCaret)
			SetInsTextPos(Text.size());
		___TextLock.Unlock();

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface(MoveCaret);
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();

		mSendConMsg(DControlMsg::TextBox_TextChange, 0, 0);

		return;
	}
	///<summary>
	///默认以当前TextPos插入一个字符，然后把Caret移动到文本末端
	///</summary>
	inline void InsertChar(const wchar_t Ch)
	{
		this->InsertChar(Insert_TextPos, Ch);
		return;
	}
	///<summary>
	///在指定位置插入一个字符，并把Caret移动到插入后的文本末端
	///</summary>
	void InsertChar(UINT TextPos, const wchar_t Ch, bool MoveCaret = true)
	{
		UINT textpos;

		___TextLock.Lock();

		if (TextPos > Text.size())
			textpos = (UINT)Text.size();
		else textpos = TextPos;

		if (!MultiLine && (Ch == L'\r' || Ch == L'\n'))
			Text.insert(textpos, 1, L' ');
		else
			Text.insert(textpos, 1, Ch);

		if (MoveCaret)
			SetInsTextPos_Offset(1);
		___TextLock.Unlock();

		if (SelectMode && Select_EndPos > Text.size())
			SetSelectText(Select_BeginPos, Text.size(), false);

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface(MoveCaret);
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();

		mSendConMsg(DControlMsg::TextBox_TextChange, 0, 0);

		return;
	}
	///<summary>
	///在文本后端添加一个字符串，然后将Caret移动到文本末端
	///</summary>
	void AddText(std::wstring str, bool MoveCaret = true)
	{
		if (!MultiLine)//不是多行时，把换行全部转成空格
		{
			std::wstring s = str;

			std::wstring::size_type offindex = s.find(L'\r', 0);
			while (offindex != s.npos)
			{
				s.replace(offindex, 1, L" ");
				++offindex;
				offindex = s.find(L'\r', offindex);
			}
			offindex = s.find(L'\n', 0);
			while (offindex != s.npos)
			{
				s.replace(offindex, 1, L" ");
				++offindex;
				offindex = s.find(L'\n', offindex);
			}
			___TextLock.Lock();
			Text += s.c_str();
		}
		else
		{
			___TextLock.Lock();
			Text += str.c_str();
		}

		if (MoveCaret)
			SetInsTextPos(Text.size());
		___TextLock.Unlock();

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface(MoveCaret);
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();

		mSendConMsg(DControlMsg::TextBox_TextChange, 0, 0);

		return;
	}
	///<summary>
	///指定一个文本位置并插入，然后将Caret移动到插入后的文本末端。此函数会进行边界检查
	///</summary>
	void InsertText(UINT TextPos, std::wstring str, bool MoveCaret = true)
	{
		UINT textpos;

		___TextLock.Lock();

		if (TextPos > Text.size())
			textpos = (UINT)Text.size();
		else textpos = TextPos;

		if (!MultiLine)//不是多行时，把换行全部转成空格
		{
			std::wstring s = str;

			std::wstring::size_type offindex = s.find(L'\r', 0);
			while (offindex != s.npos)
			{
				s.replace(offindex, 1, L" ");
				++offindex;
				offindex = s.find(L'\r', offindex);
			}
			offindex = s.find(L'\n', 0);
			while (offindex != s.npos)
			{
				s.replace(offindex, 1, L" ");
				++offindex;
				offindex = s.find(L'\n', offindex);
			}
			Text.insert(textpos, s.c_str(), s.size());
		}
		else Text.insert(textpos, str.c_str(), str.size());

		if (MoveCaret)
			SetInsTextPos(textpos + str.size());
		___TextLock.Unlock();

		if (SelectMode && Select_EndPos > Text.size())
			SetSelectText(Select_BeginPos, Text.size(), false);

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface(MoveCaret);
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();

		mSendConMsg(DControlMsg::TextBox_TextChange, 0, 0);

		return;
	}
	///<summary>
	///默认在当前的文本位置插入字符串，然后把Caret移动到插入后的文本末端。此函数会进行边界检查
	///</summary>
	inline void InsertText(std::wstring str, bool MoveCaret = true)
	{
		this->InsertText(Insert_TextPos, str, MoveCaret);
		return;
	}

	///<summary>
	///获取文本长度信息
	///</summary>
	inline UINT GetLength()
	{
		return (UINT)Text.size();
	}

	///<summary>
	///获取行数信息，包括自动换行
	///</summary>
	inline UINT GetLineCount()
	{
		___LayoutLock.Lock();
		RecreateTextInterface();//如果没有接口，先创建
		___LayoutLock.Unlock();
		return Select_Metrics.lineCount;
	}

	///<summary>
	///获取每行的高度，设备独立像素
	///</summary>
	inline float GetLineHeight()
	{
		___LayoutLock.Lock();
		RecreateTextInterface(false);//如果没有接口，先创建
		___LayoutLock.Unlock();
		return DP2DIP(Select_Metrics.height) / (float)Select_Metrics.lineCount;
	}
	///<summary>
	///获取向右滚动的偏移，left减去偏移是文本起始X位置，设备独立像素
	///</summary>
	inline float GetOffsetX()
	{
		if (A_ScrX)
			return DP2DIP(A_ScrX.GetEnd());
		else
			return DP2DIP(Scr_Offset.x);
	}
	///<summary>
	///获取向下滚动的偏移，top减去偏移是文本起始Y位置，设备独立像素
	///</summary>
	inline float GetOffsetY()
	{
		if (A_ScrY)
			return DP2DIP(A_ScrY.GetEnd());
		else
			return DP2DIP(Scr_Offset.y);
	}
	///<summary>
	///设置滚动的X偏移，设备独立像素
	///</summary>
	void SetOffsetX(const float& ofsX, const UINT& time = 0, DBezier* const& Nonlinear = nullptr, const UINT& AID = 0)
	{
		DBezier be = SmoothScrollBezier;
		A_ScrX.AddAni(A_ScrX ? A_ScrX.GetEnd() : Scr_Offset.x, (const int)DIP2DP(ofsX), (UINT)(FrameRate * (time / 1000.0f)), &be, 0, false);
		this->_SetNeedUpdate();
		return;
	}
	///<summary>
	///设置滚动的Y偏移，设备独立像素
	///</summary>
	void SetOffsetY(const float& ofsY, const UINT& time = 0, DBezier* const& Nonlinear = nullptr, const UINT& AID = 0)
	{
		if (!MultiLine)//避免掉非多行时的设置
			return;
		DBezier be = SmoothScrollBezier;
		A_ScrY.AddAni(A_ScrY ? A_ScrY.GetEnd() : Scr_Offset.y, (const int)DIP2DP(ofsY), (UINT)(FrameRate * (time / 1000.0f)), &be, 0, false);
		this->_SetNeedUpdate();
		return;
	}
	///<summary>
	///设置当前文本框文本，并将Caret移动到文本末端。此函数会进行边界检查
	///</summary>
	void SetText(std::wstring str, bool MoveCaret = true)
	{
		if (!MultiLine)//不是多行时，把换行全部转成空格
		{
			std::wstring s = str;

			std::wstring::size_type offindex = s.find(L'\r', 0);
			while (offindex != s.npos)
			{
				s.replace(offindex, 1, L" ");
				++offindex;
				offindex = s.find(L'\r', offindex);
			}
			offindex = s.find(L'\n', 0);
			while (offindex != s.npos)
			{
				s.replace(offindex, 1, L" ");
				++offindex;
				offindex = s.find(L'\n', offindex);
			}
			___TextLock.Lock();
			Text = s;
		}
		else
		{
			___TextLock.Lock();
			Text = str;
		}

		if (MoveCaret)
			SetInsTextPos(Text.size());
		___TextLock.Unlock();

		if (SelectMode && Select_EndPos > Text.size())
			SetSelectText(Select_BeginPos, Text.size(), false);

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface(MoveCaret);
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();

		mSendConMsg(DControlMsg::TextBox_TextChange, 0, 0);

		return;
	}
	///<summary>
	///在指定文本位置向后擦除Count个字符串，Caret移动到开始擦除的位置。此函数会进行边界检查
	///</summary>
	void EraseText(UINT TextPos, UINT Count, bool MoveCaret = true)
	{
		UINT textpos;
		UINT count;

		___TextLock.Lock();

		if (TextPos > Text.size())
			textpos = (UINT)Text.size();
		else textpos = TextPos;

		if (Count > Text.size())
			count = (UINT)Text.size();
		else count = Count;

		Text.erase(textpos, count);

		if (MoveCaret)
			SetInsTextPos(textpos);
		___TextLock.Unlock();

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface(MoveCaret);
		___LayoutLock.Unlock();

		UpdateCaret();//立即刷新插入符
		this->_SetNeedUpdate();

		mSendConMsg(DControlMsg::TextBox_TextChange, 0, 0);

		return;
	}
	///<summary>
	///在当前文本位置向后擦除Count个字符串，Caret还是当前位置。此函数会进行边界检查
	///</summary>
	void EraseText(UINT Count, bool MoveCaret = true)
	{
		UINT count;

		___TextLock.Lock();

		if (Count > Text.size())
			count = (UINT)Text.size();
		else count = Count;

		Text.erase(Insert_TextPos, count);

		if (MoveCaret)
			SetInsTextPos_Offset(0);
		___TextLock.Unlock();

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface(MoveCaret);
		___LayoutLock.Unlock();

		UpdateCaret();//立即刷新插入符
		this->_SetNeedUpdate();

		mSendConMsg(DControlMsg::TextBox_TextChange, 0, 0);

		return;
	}
	///<summary>
	///获取当前文本框的文本
	///</summary>
	inline std::wstring GetText()
	{
		return Text;
	}
	///<summary>
	///移动当前的插入符，形式为：当前位置 + OffsetPos。此函数会进行边界检查
	///</summary>
	void MoveCaret_Offset(long OffsetPos)
	{
		___TextLock.Lock();
		if (SetInsTextPos_Offset(OffsetPos))
			TextPosToInsertPos(true, SmoothScr, SmoothIns);
		else
			ResetTextScrolling(SmoothScr);
		___TextLock.Unlock();

		float ofs = GetPosOffset();
		UpdateCaret();//立即刷新插入符
		this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floor(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floor(DIP2DP(1.0f)) });
		return;
	}
	///<summary>
	///直接设置当前的插入符位置，此函数会进行边界检查
	///</summary>
	void MoveCaret(UINT TextPos)
	{
		___TextLock.Lock();
		if (SetInsTextPos(TextPos))
			TextPosToInsertPos(true, SmoothScr, SmoothIns);
		else
			ResetTextScrolling(SmoothScr);
		___TextLock.Unlock();

		float ofs = GetPosOffset();
		UpdateCaret();//立即刷新插入符
		this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floor(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floor(DIP2DP(1.0f)) });
		return;
	}
	///<summary>
	///选中当前所有文本
	///</summary>
	inline void SetSelectText_All()
	{
		___TextLock.Lock();
		Select_BeginPos = 0;
		Select_EndPos = (UINT)Text.size();
		___TextLock.Unlock();
		SetInsTextPos(Select_EndPos);

		UpdateSelectPos(true);
		return;
	}
	///<summary>
	///选中一个文本区域，此函数会进行边界检查
	///</summary>
	inline void SetSelectText(UINT BeginTextPos, UINT EndTextPos, bool ReDraw = true)
	{
		___SelectLock.Lock();

		Select_BeginPos = BeginTextPos;
		Select_EndPos = EndTextPos;

		___TextLock.Lock();
		if (Select_BeginPos > Text.size())
			Select_BeginPos = (UINT)Text.size();

		if (Select_EndPos > Text.size())
			Select_EndPos = (UINT)Text.size();
		___TextLock.Unlock();

		if (Select_BeginPos >= Select_EndPos)
		{
			ExitSelectMode(false);
			if (ReDraw)
				this->_SetNeedUpdate();
			___SelectLock.Unlock();
			return;
		}
		___SelectLock.Unlock();
		UpdateSelectPos(ReDraw);
		return;
	}

	inline void ExitSelectMode(bool Lock = true)
	{
		if (Lock)
			___SelectLock.Lock();
		SelectMode = false;
		ZeroMemory(Select_Rects, Select_RectCount * sizeof(DWRITE_HIT_TEST_METRICS));
		Select_BeginPos = 0;
		Select_EndPos = 0;
		if (Lock)
			___SelectLock.Unlock();
		return;
	}

private:
	bool HaveSetNormal = false;

	DTextBoxTypeStyle NormalType = { 0 };
	DTextBoxTypeStyle MouseMoveType = { 0 };
	DTextBoxTypeStyle EditType = { 0 };
	DTextBoxTypeStyle DisableType = { 0 };

	struct Type
	{
		D2D1_COLOR_F FillColor;
		D2D1_COLOR_F StrokeColor;
		D2D1_COLOR_F TextColor;
	};

	_Ani_Color A_Col_Fill;
	_Ani_Color A_Col_Stroke;
	_Ani_Color A_Col_Text;

	Type NowType = { 0 };//存储以255为最大的颜色

	D2D1_COLOR_F CaretColor = { 0, 0, 0, 1.0f };
	float CaretWidth = 1.5f;

	ID2D1SolidColorBrush* brush = nullptr;

	//文本区
	ID2D1RoundedRectangleGeometry* SelfTextClip = nullptr;//自身的文字裁剪区，防止文字超出范围
	___DThreadLock ___TextClipLock;

	std::wstring Text;
	___DThreadLock ___TextLock;
	std::wstring FamliyName;
	float FontSize = 0.0f;
	bool MultiLine = true;

	bool Password = false;
	bool EndlishInput = false;

	bool ReadOnly = false;

	bool SmoothScr = true;
	bool SmoothIns = true;
	_AniCalc_i A_ScrX;//设备相关像素
	_AniCalc_i A_ScrY;

	D2D1_RECT_F TextOutputRC = { 0 };
	DWRITE_WORD_WRAPPING AutoWarp = DWRITE_WORD_WRAPPING_WRAP;

	DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

	DWRITE_TEXT_ALIGNMENT xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;

	//插入符
	bool Insert_Visible = false;
	DWORD Insert_LastUpdatetime = 0;
	UINT Insert_TextPos = 0;//文本位置，插入符总在该位置字符的前面
	D2D1_RECT_U Insert_PixelPos = { 0 };//设备相关像素
	_AniCalc_i A_InsX;//在动画计算过程中使用设备相关像素
	_AniCalc_i A_InsY;

	//选中文本
	bool SelectMode = false;
	DWRITE_TEXT_METRICS Select_Metrics = { 0 };
	DWRITE_HIT_TEST_METRICS* Select_Rects = nullptr;
	UINT Select_RectCount = 0;
	UINT Select_DownPos = 0;
	UINT Select_BeginPos = 0;
	UINT Select_EndPos = 0;
	D2D1_COLOR_F Select_FillColor = { 0 };
	___DThreadLock ___SelectLock;

	//组合字符串
	std::wstring Comp_Str;
	UINT Comp_InsPos = 0;//以组合字符串的开头为起始位置

	//文本滚动
	D2D1_POINT_2U Scr_Offset = { 0 };//设备相关像素

	//文本接口
	IDWriteTextFormat* TextFormat = nullptr;
	IDWriteTextLayout* TextLayout = nullptr;
	___DThreadLock ___LayoutLock;

	void _SeparateFromWindow()
	{
		//重新设置动画颜色，防止下一次加入窗口时还没恢复状态
		NowType.FillColor = DcolToColf(NormalType.FillColor);
		NowType.StrokeColor = DcolToColf(NormalType.StrokeColor);
		NowType.TextColor = DcolToColf(NormalType.TextColor);
		A_Col_Fill.Reset();
		A_Col_Stroke.Reset();
		A_Col_Text.Reset();
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		A_ScrX.Reset();
		A_ScrY.Reset();
		A_InsX.Reset();
		A_InsY.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(RoundGeo);
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(TextFormat);
		RELEASE(TextLayout);
		RELEASE(SelfTextClip);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(HitTestGeo);
		return;
	}

	bool _IsContentChanging()
	{
		if (A_Col_Fill || A_Col_Stroke || A_Col_Text || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_ScrX || A_ScrY || A_InsX || A_InsY || A_Radius_X || A_Radius_Y || A_Opa)
			return true;
		else
			return false;
	}

	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		if (State != state)
		{
			float time = 0.0f;
			DCOLOR* fill = nullptr, * stroke = nullptr, * text = nullptr;
			switch (state)
			{
			case DControlState::Normal:
				time = NormalType.time / 1000.0f;
				fill = &NormalType.FillColor;
				stroke = &NormalType.StrokeColor;
				text = &NormalType.TextColor;
				break;
			case DControlState::MouseMove:
				time = MouseMoveType.time / 1000.0f;
				fill = &MouseMoveType.FillColor;
				stroke = &MouseMoveType.StrokeColor;
				text = &MouseMoveType.TextColor;
				break;
			case DControlState::Edit:
				time = EditType.time / 1000.0f;
				fill = &EditType.FillColor;
				stroke = &EditType.StrokeColor;
				text = &EditType.TextColor;
				break;
			case DControlState::Disable:
				time = DisableType.time / 1000.0f;
				fill = &DisableType.FillColor;
				stroke = &DisableType.StrokeColor;
				text = &DisableType.TextColor;
				break;
			}
			DControlState LastState = State;
			State = state;

			if (fill && stroke && text)
			{
				//填充颜色
				A_Col_Fill.AddAni(NowType.FillColor, *fill, (UINT)(FrameRate * time));
				//描边颜色
				A_Col_Stroke.AddAni(NowType.StrokeColor, *stroke, (UINT)(FrameRate * time));
				//文字颜色
				A_Col_Text.AddAni(NowType.TextColor, *text, (UINT)(FrameRate * time));
			}

			this->_SetNeedUpdate();

			mSendConMsg(DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);

			if (NeedUnlock)
				___StateLock.Unlock();
			return true;
		}
		if (NeedUnlock)
			___StateLock.Unlock();
		return false;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		DstRC.rect.left = floor(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = floor(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = floor(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = floor(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			UpdateRC.left = floor(DstRC.rect.left - DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floor(DstRC.rect.top - DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceil(DstRC.rect.right + DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceil(DstRC.rect.bottom + DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			UpdateRC = DstRC.rect;
		}

		//文本输出的区域(考虑到输入区域的边缘不能与边框重叠)
		TextOutputRC.left = floor(DstRC.rect.left + DIP2DP(7.0f));
		TextOutputRC.top = floor(DstRC.rect.top + DIP2DP(7.0f));
		TextOutputRC.right = ceil(DstRC.rect.right - DIP2DP(7.0f));
		TextOutputRC.bottom = ceil(DstRC.rect.bottom - DIP2DP(7.0f));
		DetectTextOptRC(TextOutputRC);

		___LayoutLock.Lock();
		RecreateTextInterface(false);
		TextPosToInsertPos(false, true, true, true, false);
		___LayoutLock.Unlock();
		return;
	}

	bool _GetNeedUpdate()//根据每个控件的GetNeedUpdate每帧都会调用的特性，在这里为插入符计时并设置刷新
	{
		if (State == DControlState::Edit && (timeGetTime() - Insert_LastUpdatetime) > 500)
		{
			Insert_Visible = !Insert_Visible;

			float ofs = GetPosOffset();
			this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floor(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floor(DIP2DP(1.0f)) });
			Insert_LastUpdatetime = timeGetTime();
		}

		return NeedUpdate;
	}
	//退出组合状态
	inline void ExitComp(HWND hWnd, bool ReDraw)
	{
		HIMC hIMC = ImmGetContext(hWnd);
		if (hIMC)
		{
			ImmSetOpenStatus(hIMC, false);
			ImmReleaseContext(hWnd, hIMC);
		}
		return;
	}

	//输入处理
	bool _KeyDownMsg(WPARAM wParam)
	{
		if (State == DControlState::Edit)
		{
			switch (wParam)
			{
			case VK_LEFT:
			{
				___SelectLock.Lock();
				if (SelectMode)
				{
					UINT pos = Select_BeginPos;
					this->ExitSelectMode(false);
					this->MoveCaret(pos);
					this->_SetNeedUpdate();
					___SelectLock.Unlock();
					break;
				}
				___SelectLock.Unlock();

				___TextLock.Lock();
				if (Insert_TextPos > 1 && Text.at(Insert_TextPos - 1) == L'\n' && Text.at(Insert_TextPos - 2) == '\r')
					this->MoveCaret_Offset(-2);
				else
					this->MoveCaret_Offset(-1);
				___TextLock.Unlock();
				break;
			}
			case VK_RIGHT:
			{
				___SelectLock.Lock();
				if (SelectMode)
				{
					UINT pos = Select_EndPos;
					this->ExitSelectMode(false);
					this->MoveCaret(pos);
					this->_SetNeedUpdate();
					___SelectLock.Unlock();
					break;
				}
				___SelectLock.Unlock();

				___TextLock.Lock();
				if (Insert_TextPos < Text.size() && Text.at(Insert_TextPos) == L'\r' && Text.at(Insert_TextPos + 1) == '\n')
					this->MoveCaret_Offset(2);
				else
					this->MoveCaret_Offset(1);
				___TextLock.Unlock();
				break;
			}
			case VK_UP:
			{
				___SelectLock.Lock();
				if (SelectMode)
				{
					UINT pos = Select_BeginPos;
					this->ExitSelectMode(false);
					this->MoveCaret(pos);
					___SelectLock.Unlock();
					break;
				}
				___SelectLock.Unlock();

				this->ResetInsertPos((int)Insert_PixelPos.left - (int)Scr_Offset.x, (int)Insert_PixelPos.top - (int)Scr_Offset.y - (int)DIP2DP(3.0f));
				break;
			}
			case VK_DOWN:
			{
				___SelectLock.Lock();
				if (SelectMode)
				{
					UINT pos = Select_EndPos;
					this->ExitSelectMode(false);
					this->MoveCaret(pos);
					___SelectLock.Unlock();
					break;
				}
				___SelectLock.Unlock();

				this->ResetInsertPos((int)Insert_PixelPos.left - (int)Scr_Offset.x, (int)Insert_PixelPos.bottom - (int)Scr_Offset.y + (int)DIP2DP(3.0f));
				break;
			}
			case 'V':
			{
				if ((GetKeyState(VK_CONTROL) & 0x8000) && !ReadOnly)
				{
					if (OpenClipboard(nullptr))
					{
						HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);

						if (hMem)
						{
							//获取UNICODE的字符串
							const wchar_t* lpStr = (LPCTSTR)GlobalLock(hMem);
							if (lpStr)
							{
								___SelectLock.Lock();
								if (SelectMode)
								{
									this->EraseText(Select_BeginPos, Select_EndPos - Select_BeginPos);
									this->ExitSelectMode(false);
								}
								___SelectLock.Unlock();

								this->InsertText(lpStr);

								GlobalUnlock(hMem);
							}
						}
						CloseClipboard();
					}
					return true;
				}
				break;
			}//case V End

			case 'C':
			{
				___SelectLock.Lock();
				if (GetKeyState(VK_CONTROL) & 0x8000 && SelectMode)
				{
					HGLOBAL hGlobal;
					wchar_t* pGlobal;

					___TextLock.Lock();
					std::wstring str = Text.substr(Select_BeginPos, Select_EndPos - Select_BeginPos);
					___TextLock.Unlock();

					hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (str.size() + 1) * sizeof(wchar_t));
					if (hGlobal)
					{
						pGlobal = (wchar_t*)GlobalLock(hGlobal);
						if (pGlobal)
						{
							lstrcpyW(pGlobal, str.c_str());

							if (OpenClipboard(nullptr))
							{
								EmptyClipboard();

								SetClipboardData(CF_UNICODETEXT, hGlobal);
								CloseClipboard();
							}
						}
						GlobalUnlock(hGlobal);
					}
					___SelectLock.Unlock();
					return true;
				}
				___SelectLock.Unlock();
				break;
			}//case C End

			case 'A':
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					___TextLock.Lock();
					Select_DownPos = (UINT)Text.size();
					___TextLock.Unlock();
					this->SetSelectText_All();
					return true;
				}
				break;
			}//case A End
			case 'X':
			{
				___SelectLock.Lock();
				if (GetKeyState(VK_CONTROL) & 0x8000 && SelectMode)
				{
					//先写入剪贴板
					HGLOBAL hGlobal;
					wchar_t* pGlobal;

					___TextLock.Lock();
					std::wstring str = Text.substr(Select_BeginPos, Select_EndPos - Select_BeginPos);
					___TextLock.Unlock();

					hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (str.size() + 1) * sizeof(wchar_t));
					if (hGlobal)
					{
						pGlobal = (wchar_t*)GlobalLock(hGlobal);
						if (pGlobal)
						{
							lstrcpyW(pGlobal, str.c_str());

							if (OpenClipboard(nullptr))
							{
								EmptyClipboard();

								SetClipboardData(CF_UNICODETEXT, hGlobal);
								CloseClipboard();
							}
						}
						GlobalUnlock(hGlobal);
					}

					if (!ReadOnly)
					{
						//然后删掉文本
						UINT pos = Select_BeginPos, count = Select_EndPos - Select_BeginPos;
						this->ExitSelectMode(false);
						this->EraseText(pos, count);
					}

					___SelectLock.Unlock();
					return true;
				}
				___SelectLock.Unlock();
				break;
			}//case X End

			}//switch End
		}

		return false;
	}

	bool _CharMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (State != DControlState::Edit || ReadOnly || OnClick)
			return false;

		switch (Msg)
		{
		case WM_CHAR:
		{
			if (GetKeyState(VK_CONTROL) & 0x8000)
				break;

			___SelectLock.Lock();
			if (SelectMode && wParam != 0x08)
			{
				UINT pos = Select_BeginPos, count = Select_EndPos - Select_BeginPos;
				this->ExitSelectMode(false);
				this->EraseText(pos, count, true);
			}
			___SelectLock.Unlock();

			switch (wParam)
			{
			case 0x0D://回车键
				if (!MultiLine)//单行模式
				{
					mSendConMsg(DControlMsg::TextBox_Enter, 0, 0);
					break;
				}

				this->InsertText(L"\r\n");

				mSendConMsg(DControlMsg::TextBox_Enter, 0, 0);

				break;
			case 0x08://退格键
				if (Text.size())
				{
					___SelectLock.Lock();
					if (SelectMode)
					{
						UINT pos = Select_BeginPos, count = Select_EndPos - Select_BeginPos;
						this->ExitSelectMode(false);
						this->EraseText(pos, count, true);
					}
					else
					{
						if (Insert_TextPos >= 2 && Text.at(Insert_TextPos - 1) == '\n' && Text.at(Insert_TextPos - 2) == '\r')
						{
							this->EraseText(Insert_TextPos - 2, 2, true);
						}
						else if (Insert_TextPos >= 1)
						{
							this->EraseText(Insert_TextPos - 1, 1, true);
						}
					}
					___SelectLock.Unlock();
				}
				else
					___TextLock.Unlock();

				break;
			case 0x20://空格键
				this->InsertChar(L' ');
				break;
			case 0x09://TAB键
				this->InsertChar(L'	');
				break;
			default:
			{
				this->InsertChar(Insert_TextPos, wParam);
				break;
			}

			}//switch wParam End

			break;
		}//WM_CHAR End

		}//switch Msg End

		return true;
	}
	bool _ImeComposition(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (State != DControlState::Edit || ReadOnly)
			return false;

		if (Msg == WM_IME_STARTCOMPOSITION)
		{
			if (EndlishInput)
				ImmAssociateContext(OwnerWnd, nullptr);
			else
				ImmAssociateContextEx(OwnerWnd, nullptr, IACE_DEFAULT);

			HIMC hIMC = ImmGetContext(hWnd);
			if (hIMC)
			{
				ImmSetOpenStatus(hIMC, true);

				COMPOSITIONFORM CompForm;
				CompForm.dwStyle = CFS_POINT;

				int ScrX = A_ScrX ? A_ScrX.GetEnd() : (int)Scr_Offset.x, ScrY = A_ScrY ? A_ScrY.GetEnd() : (int)Scr_Offset.y;

				if (A_InsX)
					CompForm.ptCurrentPos.x = (long)(A_InsX.GetEnd() - ScrX);
				else
					CompForm.ptCurrentPos.x = (long)(Insert_PixelPos.left - ScrX);

				if (A_InsY)
					CompForm.ptCurrentPos.y = (long)(A_InsY.GetEnd() - ScrY - GetPosOffset());
				else
					CompForm.ptCurrentPos.y = (long)(Insert_PixelPos.bottom - ScrY - GetPosOffset());

				ImmSetCompositionWindow(hIMC, &CompForm);
				ImmReleaseContext(hWnd, hIMC);
			}
			return true;
		}
		else if (Msg == WM_IME_REQUEST)
		{
			if (wParam == IMR_QUERYCHARPOSITION)
			{
				IMECHARPOSITION* can = (IMECHARPOSITION*)lParam;
				can->dwSize = sizeof(IMECHARPOSITION);
				POINT p;

				_CharPosWithoutComp(p.x, p.y);//获取不算组合字符串的合成窗口坐标

				ClientToScreen(hWnd, &p);
				can->pt.x = p.x;
				can->pt.y = p.y - (long)GetPosOffset();
				can->cLineHeight = 0;
				return true;
			}
		}
		else if (Msg == WM_IME_COMPOSITION)
		{
			if (lParam & GCS_RESULTSTR && !OnClick)
			{
				DWORD dwSize;
				HGLOBAL hstr;
				LPWSTR lpstr;
				HIMC hIMC = ImmGetContext(hWnd);
				if (!hIMC)
					return false;

				//获取返回文本大小
				dwSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, nullptr, 0);

				//加入结尾空字符大小
				dwSize += sizeof(WCHAR);

				hstr = GlobalAlloc(GHND, dwSize);
				if (!hstr)
				{
					ImmReleaseContext(hWnd, hIMC);
					return false;
				}

				lpstr = (LPWSTR)GlobalLock(hstr);
				if (!lpstr)
				{
					GlobalFree(hstr);
					ImmReleaseContext(hWnd, hIMC);
					return false;
				}

				//获取IME结果字符串
				ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, lpstr, dwSize);
				//处理显示结果字符串
				Comp_Str.clear();
				Comp_InsPos = 0;

				//有选中文本的话，清除
				___SelectLock.Lock();
				if (SelectMode)
				{
					UINT pos = Select_BeginPos, count = Select_EndPos - Select_BeginPos;
					this->ExitSelectMode(false);
					this->EraseText(pos, count, true);
				}
				___SelectLock.Unlock();

				this->InsertText(lpstr);
				//释放内存
				ImmReleaseContext(hWnd, hIMC);
				GlobalUnlock(hstr);
				GlobalFree(hstr);
				return true;
			}
			else if (lParam & GCS_COMPSTR)
			{
				DWORD dwSize;
				HGLOBAL hstr;
				LPWSTR lpstr;
				HIMC hIMC = ImmGetContext(hWnd);
				if (!hIMC)
					return false;

				//获取返回文本大小
				dwSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, nullptr, 0);
				if (!dwSize)
				{
					Comp_Str.clear();
					Comp_InsPos = 0;

					___LayoutLock.Lock();
					RELEASE(TextLayout);
					___LayoutLock.Unlock();
					this->_SetNeedUpdate();

					ImmReleaseContext(hWnd, hIMC);
					return true;
				}

				//加入结尾空字符大小
				dwSize += sizeof(WCHAR);

				hstr = GlobalAlloc(GHND, dwSize);
				if (!hstr)
				{
					ImmReleaseContext(hWnd, hIMC);
					return false;
				}

				lpstr = (LPWSTR)GlobalLock(hstr);
				if (!lpstr)
				{
					GlobalFree(hstr);
					ImmReleaseContext(hWnd, hIMC);
					return false;
				}

				//获取IME输入字符串
				ImmGetCompositionStringW(hIMC, GCS_COMPSTR, lpstr, dwSize);

				OnClick = false;
				//有选中文本的话，清除
				___SelectLock.Lock();
				if (SelectMode)
				{
					UINT pos = Select_BeginPos, count = Select_EndPos - Select_BeginPos;
					this->ExitSelectMode(false);
					this->EraseText(pos, count, true);
				}
				___SelectLock.Unlock();
				//处理显示输入字符串
				Comp_Str = lpstr;
				Comp_InsPos = (UINT)ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, nullptr, 0);

				//刷新并设置合成窗口位置
				___LayoutLock.Lock();
				RELEASE(TextLayout);

				COMPOSITIONFORM CompForm;
				CompForm.dwStyle = CFS_POINT;
				_CharPosWithoutComp(CompForm.ptCurrentPos.x, CompForm.ptCurrentPos.y, false);//获取不算组合字符串的合成窗口坐标
				___LayoutLock.Unlock();

				float ofs = GetPosOffset();
				CompForm.ptCurrentPos.y -= (long)ofs;
				ImmSetCompositionWindow(hIMC, &CompForm);

				UpdateCaret();//立即刷新插入符
				this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floor(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floor(DIP2DP(1.0f)) });

				//释放内存
				ImmReleaseContext(hWnd, hIMC);
				GlobalUnlock(hstr);
				GlobalFree(hstr);
				return true;
			}
		}
		return false;
	}

	void _CharPosWithoutComp(long& x, long& y, bool lock = true)
	{
		int ScrX = A_ScrX ? A_ScrX.GetEnd() : (int)Scr_Offset.x, ScrY = A_ScrY ? A_ScrY.GetEnd() : (int)Scr_Offset.y;

		if (lock)
			___LayoutLock.Lock();

		RecreateTextInterface(true);
		if (TextLayout)
		{
			DWRITE_HIT_TEST_METRICS hit;
			bool IsBehind = false;
			float pointX, pointY;

			TextLayout->HitTestTextPosition(Insert_TextPos, false, &pointX, &pointY, &hit);
			if (lock)
				___LayoutLock.Unlock();

			//bidi level为偶数时表示方向为从左到右，奇数时为从右到左
			if (hit.bidiLevel % 2)
				IsBehind = !IsBehind;

			x = (long)(IsBehind ? (TextOutputRC.left + hit.left + hit.width - ScrX) : (TextOutputRC.left + hit.left - ScrX));
			y = (long)(TextOutputRC.top + hit.top + hit.height - ScrY);
		}
		else
		{
			if (lock)
				___LayoutLock.Unlock();

			if (A_InsX)
				x = (long)(A_InsX.GetEnd() - ScrX);
			else
				x = (long)(Insert_PixelPos.left - ScrX);

			if (A_InsY)
				y = (long)(A_InsY.GetEnd() - ScrY);
			else
				y = (long)(Insert_PixelPos.bottom - ScrY);
		}

		return;
	}
	//状态及光标处理
	bool _SetCursorMsgProc()
	{
		if (State == DControlState::Disable)
			return false;

		SetCursor(this->CursorStyle);
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		___StateLock.Lock();

		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable)
		{
			___StateLock.Unlock();
			return false;
		}

		if (Msg == WM_MOUSEMOVE && OnClick)
		{
			POINT p;
			if (GetCursorPos(&p))
			{
				if (ScreenToClient(hWnd, &p))
				{
					this->_MouseMoveMsgProc(p.x, p.y);
					this->_SetCursorMsgProc();
				}
			}
		}
		else if (Msg == WM_LBUTTONUP)
			OnClick = false;

		if (NowControlID == ID)
		{
			___StateLock.Unlock();
			return false;
		}

		if (HaveSetNormal)
		{
			___StateLock.Unlock();
			return false;
		}

		if (State == DControlState::Edit && (Msg == WM_MOUSEMOVE || Msg == WM_LBUTTONUP || Msg == WM_MOUSELEAVE))
		{
			___StateLock.Unlock();
			return false;
		}

		OnClick = false;

		//退出组合状态
		if (State == DControlState::Edit)
			ExitComp(hWnd, false);
		return m_SetState(DControlState::Normal, true);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		___StateLock.Lock();

		if (HaveSetNormal)
		{
			HaveSetNormal = false;
			___StateLock.Unlock();
			return true;
		}

		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable)
		{
			___StateLock.Unlock();
			return false;
		}

		if (OnClick && Text.size())
		{
			SetSelectText_Pos(xPos, yPos);//不要在此设置SelectMode为true，函数会自行分辨并设置
		}

		if (State == DControlState::Edit)
		{
			___StateLock.Unlock();
			return false;
		}

		return m_SetState(DControlState::MouseMove, true);
	}
	bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos)
	{
		___StateLock.Lock();

		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable)
		{
			___StateLock.Unlock();
			return false;
		}
		//退出组合状态
		ExitComp(hWnd, false);
		//更新插入符的位置
		ResetInsertPos(xPos, yPos, false);
		//取消选中模式
		___SelectLock.Lock();
		if (SelectMode)
		{
			this->ExitSelectMode(false);
			this->_SetNeedUpdate();
		}
		Select_DownPos = Insert_TextPos;
		___SelectLock.Unlock();

		OnClick = true;
		HaveSetNormal = false;

		return m_SetState(DControlState::Edit, true);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		___StateLock.Lock();

		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable || !OnClick || (OnClick && !this->HitTest(xPos, yPos)))
		{
			___StateLock.Unlock();
			OnClick = false;
			return false;
		}
		OnClick = false;
		HaveSetNormal = false;

		return m_SetState(DControlState::Edit, true);
	}
	bool _MouseWheelMsgProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		if (!MultiLine)
			return false;

		short Delta = GET_WHEEL_DELTA_WPARAM(wParam);
		UINT ScrollLines;
		SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &ScrollLines, 0);
		if (ScrollLines == 0)
			ScrollLines = 3;

		int ScrY = A_ScrY ? A_ScrY.GetEnd() : (int)Scr_Offset.y, src = (int)Scr_Offset.y;

		if (Delta > 0)//手指向前滑动时
		{
			if (ScrY > 0)
			{
				ScrY -= (int)(Select_Metrics.height / Select_Metrics.lineCount * (float)ScrollLines);

				if (ScrY < 0)
					ScrY = 0;

				if (ScrY != src)
				{
					DBezier be = SmoothScrollBezier;
					A_ScrY.AddAni(src, ScrY, SmoothScr ? ((UINT)(FrameRate * (SmoothScrollTime / 1000.0f))) : 0, &be, 0, false);
					//无滚动动画，立即重新设置滚动
					if (!SmoothScr && OnClick && Text.size())
					{
						POINT p = { GET_X(lParam), GET_Y(lParam) };
						ScreenToClient(OwnerWnd, &p);
						SetSelectText_Pos(p.x, p.y, true);//不要在此设置SelectMode为true，函数会自行分辨并设置
					}
					this->_SetNeedUpdate();
				}
				else return OnClick;
			}
			else return OnClick;
		}
		else if (Delta < 0)//手指向后滑动时
		{
			if (Select_Metrics.height - (int)ScrY > TextOutputRC.bottom - TextOutputRC.top)
			{
				ScrY += (int)(Select_Metrics.height / Select_Metrics.lineCount * (float)ScrollLines);

				if (Select_Metrics.height - (float)ScrY < TextOutputRC.bottom - TextOutputRC.top)//文本太靠上时的处理
					ScrY = (int)(Select_Metrics.height - TextOutputRC.bottom + TextOutputRC.top);

				if (ScrY != src)
				{
					DBezier be = SmoothScrollBezier;
					A_ScrY.AddAni(src, ScrY, SmoothScr ? ((UINT)(FrameRate * (SmoothScrollTime / 1000.0f))) : 0, &be, 0, false);
					//无滚动动画，立即重新设置滚动
					if (!SmoothScr && OnClick && Text.size())
					{
						POINT p = { GET_X(lParam), GET_Y(lParam) };
						ScreenToClient(OwnerWnd, &p);
						SetSelectText_Pos(p.x, p.y);//不要在此设置SelectMode为true，函数会自行分辨并设置
					}
					this->_SetNeedUpdate();
				}
				else return OnClick;
			}
			else return OnClick;
		}

		//通知给用户
		mSendConMsg(DControlMsg::TextBox_Scrolling, (WPARAM)lParam, (LPARAM)wParam);
		return true;
	}

	bool _CreateHitTestGeo()//创建命中测试几何
	{
		___HitTestLock.Lock();
		RELEASE(HitTestGeo);
		if (D2DFactory)
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DIP2DP(DstSize.width + StrokeWidth), DIP2DP(DstSize.height + StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Col_Fill)
		{
			A_Col_Fill.FrameCalc(NowType.FillColor);
			this->_SetNeedUpdate();
		}
		if (A_Col_Stroke)
		{
			A_Col_Stroke.FrameCalc(NowType.StrokeColor);
			this->_SetNeedUpdate();
		}
		if (A_Col_Text)
		{
			A_Col_Text.FrameCalc(NowType.TextColor);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(AID);
				if (!A_Radius_Y)
					resp = true;
			}
			SetRadius(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		if (A_ScrX)
		{
			UINT AID;
			Scr_Offset.x = (UINT)A_ScrX.FrameCalc(AID);
			this->_SetNeedUpdate();

			if (AID)
			{
				//通知给用户
				mSendConMsg(DControlMsg::TextBox_EndOfScroll, 0, 0);
			}
		}
		if (A_ScrY)
		{
			UINT AID;
			Scr_Offset.y = (UINT)A_ScrY.FrameCalc(AID);
			//纵向滚动时重新设置选中
			if (OnClick && Text.size())
			{
				POINT p;
				if (GetCursorPos(&p) && OwnerWnd)
				{
					ScreenToClient(OwnerWnd, &p);
					SetSelectText_Pos(p.x, p.y, true);//不要在此设置SelectMode为true，函数会自行分辨并设置
				}
			}
			this->_SetNeedUpdate();

			if (AID)
			{
				//通知给用户
				mSendConMsg(DControlMsg::TextBox_EndOfScroll, 0, 0);
			}
		}

		if (A_InsX || A_InsY)
		{
			UINT AID;
			float ofs = GetPosOffset();
			D2D1_RECT_F beforerc = { (float)Insert_PixelPos.left - (float)Scr_Offset.x - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floor(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floor(DIP2DP(1.0f)) };
			if (A_InsX)
			{
				Insert_PixelPos.left = (UINT)A_InsX.FrameCalc(AID);
				Insert_PixelPos.right = (UINT)(Insert_PixelPos.left + DIP2DP(CaretWidth));
			}
			if (A_InsY)
			{
				UINT height = Insert_PixelPos.bottom - Insert_PixelPos.top;
				Insert_PixelPos.top = (UINT)A_InsY.FrameCalc(AID);
				Insert_PixelPos.bottom = (UINT)(Insert_PixelPos.top + height);
			}

			UpdateCaret();//立即刷新插入符
			this->_SetNeedUpdate(CalcRectUnion({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floor(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floor(DIP2DP(1.0f)) }, beforerc));
		}
		return;
	}
	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//处理动画的颜色变换
		D2D1_COLOR_F f_FillColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_StrokeColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_TextColor;//以1.0f为最大的颜色结构体

		//动画——填充颜色处理
		f_FillColor = Calc_fColor(NowType.FillColor, TotalOpacity);
		//动画——描边颜色处理
		f_StrokeColor = Calc_fColor(NowType.StrokeColor, TotalOpacity);
		//动画——文字颜色处理
		f_TextColor = Calc_fColor(NowType.TextColor, TotalOpacity);

		//*******动画颜色处理部分结束*******

		if (f_FillColor.a != 0.0f)
		{
			brush->SetColor(f_FillColor);//设置为填充颜色
			DstTarget->FillRoundedRectangle(DstRC, brush);
		}
		if (f_StrokeColor.a != 0.0f)
		{
			brush->SetColor(f_StrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		//自文本裁剪(如果有圆角，则用图层实现圆角裁剪，否则用clip)
		if (DstRC.radiusX != 0.0f || DstRC.radiusY != 0.0f)
		{
			___TextClipLock.Lock();

			if (!SelfTextClip && D2DFactory)//如果自己的文本裁剪没有，则创建
				D2DFactory->CreateRoundedRectangleGeometry({ { DstRC.rect.left + DIP2DP(StrokeWidth) / 2.0f, DstRC.rect.top + DIP2DP(StrokeWidth) / 2.0f, DstRC.rect.right - DIP2DP(StrokeWidth) / 2.0f, DstRC.rect.bottom - DIP2DP(StrokeWidth) / 2.0f }, DstRC.radiusX, DstRC.radiusY }, &SelfTextClip);

			//设置当前文本的裁剪区
			if (SelfTextClip)//设置当前文本的裁剪区，必须等背景的东西画完了，再设置，不然背景也裁剪了
				Clip.PushLayer(DstTarget, UpdateRC, SelfTextClip, D2D1_ANTIALIAS_MODE_ALIASED);
			___TextClipLock.Unlock();
		}
		else if (Select_Metrics.widthIncludingTrailingWhitespace > Select_Metrics.layoutWidth || Select_Metrics.height > Select_Metrics.layoutHeight)
			Clip.PushClip(DstTarget, { DstRC.rect.left + DIP2DP(StrokeWidth) / 2.0f, DstRC.rect.top + DIP2DP(StrokeWidth) / 2.0f, DstRC.rect.right - DIP2DP(StrokeWidth) / 2.0f, DstRC.rect.bottom - DIP2DP(StrokeWidth) / 2.0f });

		//尽量减少锁的锁定时间，靠下放
		___LayoutLock.Lock();
		RecreateTextInterface();//如果相关接口为NULL，则直接重建
		//画文本
		if (TextLayout && f_TextColor.a != 0.0f)
		{
			brush->SetColor(f_TextColor);//设置为文字颜色
			DstTarget->DrawTextLayout({ floor(TextOutputRC.left - (float)Scr_Offset.x), floor(TextOutputRC.top - (float)Scr_Offset.y) }, TextLayout, brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
		}
		___LayoutLock.Unlock();

		//如果为选中状态，填充选中区域
		___SelectLock.Lock();
		if (SelectMode && Select_Rects)
		{
			brush->SetColor({ Select_FillColor.r, Select_FillColor.g, Select_FillColor.b, Select_FillColor.a * TotalOpacity });
			D2D1_RECT_F selFill;
			for (UINT i = 0; i < Select_RectCount; ++i)
			{
				selFill.left = TextOutputRC.left + Select_Rects[i].left - (float)Scr_Offset.x;
				selFill.top = TextOutputRC.top + Select_Rects[i].top - (float)Scr_Offset.y;
				selFill.right = selFill.left + Select_Rects[i].width;
				selFill.bottom = selFill.top + Select_Rects[i].height;
				if (Select_Rects[i].width == 0.0f)
					selFill.right += DIP2DP(FontSize) / 4.5f;//如果此行是回车，则显示一小块表示此行有内容

				DstTarget->FillRectangle(selFill, brush);
			}
		}
		___SelectLock.Unlock();

		//画插入符
		if (Insert_Visible && !SelectMode && State == DControlState::Edit && CaretColor.a * TotalOpacity != 0.0f)
		{
			D2D1_RECT_F DPPos = { (float)Insert_PixelPos.left - (float)Scr_Offset.x, (float)Insert_PixelPos.top - (float)Scr_Offset.y, (float)Insert_PixelPos.right - (float)Scr_Offset.x, (float)Insert_PixelPos.bottom - (float)Scr_Offset.y };
			brush->SetColor({ CaretColor.r, CaretColor.g, CaretColor.b, CaretColor.a * TotalOpacity });
			DstTarget->FillRectangle(DPPos, brush);
		}

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		return true;
	}


	void ResetTextScrolling(bool ScrAni = false, bool Redraw = true)
	{
		bool mScrAni = ScrAni;
		int ScrX = A_ScrX ? A_ScrX.GetEnd() : (int)Scr_Offset.x, srcX = ScrX;
		int ScrY = A_ScrY ? A_ScrY.GetEnd() : (int)Scr_Offset.y, srcY = ScrY;

		int insertX, insertUpY, insertBottomY;

		if (A_InsX)
			insertX = A_InsX.GetEnd() + (int)DIP2DP(CaretWidth) - (int)TextOutputRC.left - ScrX;
		else
			insertX = Insert_PixelPos.right - (int)TextOutputRC.left - ScrX;

		if (A_InsY)
		{
			int height = (int)(Insert_PixelPos.bottom - Insert_PixelPos.top);
			insertUpY = A_InsY.GetEnd() - (int)TextOutputRC.top - ScrY;
			insertBottomY = A_InsY.GetEnd() + height - (int)TextOutputRC.top - ScrY;
		}
		else {
			insertUpY = Insert_PixelPos.top - (int)TextOutputRC.top - ScrY;
			insertBottomY = Insert_PixelPos.bottom - (int)TextOutputRC.top - ScrY;
		}

		//文本水平滚动时的处理
		if (AutoWarp == DWRITE_WORD_WRAPPING_NO_WRAP)
		{
			if (insertX > (int)Select_Metrics.layoutWidth)
			{
				ScrX += insertX - (int)Select_Metrics.layoutWidth;
			}
			else if (insertX < (int)Select_Metrics.left)
			{
				ScrX += insertX - (int)Select_Metrics.left;
				if (ScrX < 0)
					ScrX = 0;
			}
			//文本太靠左时的处理
			if ((int)Select_Metrics.widthIncludingTrailingWhitespace > (int)Select_Metrics.layoutWidth && (int)Select_Metrics.widthIncludingTrailingWhitespace - ScrX < (int)(TextOutputRC.right - TextOutputRC.left))
				ScrX = (int)(Select_Metrics.widthIncludingTrailingWhitespace - TextOutputRC.right + TextOutputRC.left);
			else if ((int)Select_Metrics.widthIncludingTrailingWhitespace < (int)Select_Metrics.layoutWidth)
				ScrX = 0;
		}
		//文本垂直滚动时的处理
		if (insertBottomY > (int)floor(Select_Metrics.layoutHeight))
		{
			float delta = (float)insertBottomY - Select_Metrics.layoutHeight;
			if (floor((float)insertUpY - delta) < floor(Select_Metrics.top))
			{
				ScrY += (int)floor(((float)insertBottomY - Select_Metrics.layoutHeight - ((float)insertUpY - Select_Metrics.top)) / 2.0f);
				mScrAni = true;
			}
			else
				ScrY += (int)delta;
		}
		else if (insertUpY < (int)floor(Select_Metrics.top))
		{
			float delta = (float)insertUpY - Select_Metrics.top;
			if (floor(insertBottomY - delta) > floor(Select_Metrics.layoutHeight))
			{
				ScrY -= (int)floor(((float)insertBottomY - Select_Metrics.layoutHeight - ((float)insertUpY - Select_Metrics.top)) / 2.0f);
				mScrAni = true;
			}
			else
				ScrY += (int)delta;

			if (ScrY < 0)
				ScrY = 0;
		}
		//文本太靠上时的处理
		if (Select_Metrics.height > Select_Metrics.layoutHeight && Select_Metrics.height - (float)ScrY < Select_Metrics.layoutHeight)
			ScrY = (int)floor(Select_Metrics.height - Select_Metrics.layoutHeight);
		else if (Select_Metrics.height < Select_Metrics.layoutHeight)
			ScrY = 0;

		//应用滚动
		if (srcX != ScrX || (srcY != ScrY && MultiLine))//只有是多行时，才应用垂直滚动
		{
			if (mScrAni)
			{
				DBezier be = SmoothScrollBezier;
				if (srcX != ScrX)
					A_ScrX.AddAni(srcX, ScrX, (UINT)(FrameRate * (SmoothScrollTime / 1000.0f)), &be, 0, false);
				if (MultiLine && srcY != ScrY)//水平滚动进来后，避免掉非多行时的设置
					A_ScrY.AddAni(srcY, ScrY, (UINT)(FrameRate * (SmoothScrollTime / 1000.0f)), &be, 0, false);

				if (Redraw)
					this->_SetNeedUpdate();
			}
			else
			{
				if (srcX != ScrX)
				{
					A_ScrX.Reset();
					Scr_Offset.x = (UINT)ScrX;
				}
				if (MultiLine && srcY != ScrY)//水平滚动进来后，避免掉非多行时的设置
				{
					A_ScrY.Reset();
					Scr_Offset.y = (UINT)ScrY;
				}

				if (Redraw)
					this->_SetNeedUpdate();
			}
		}
		return;
	}

	inline UINT ScreenPosToTextPos(int xPos, int yPos)
	{
		DWRITE_HIT_TEST_METRICS hit = { 0 };
		BOOL IsBehind = false;
		BOOL isInside = false;

		___LayoutLock.Lock();
		RecreateTextInterface(false);//如果相关接口为NULL，则直接重建

		if (TextLayout)
			TextLayout->HitTestPoint((float)xPos - TextOutputRC.left + (float)Scr_Offset.x, (float)yPos - TextOutputRC.top + (float)Scr_Offset.y, &IsBehind, &isInside, &hit);
		___LayoutLock.Unlock();

		//bidi level为偶数时表示方向为从左到右，奇数时为从右到左
		if (hit.bidiLevel % 2)
			IsBehind = !IsBehind;

		return hit.textPosition + IsBehind;
	}

	inline bool SetInsTextPos(UINT pos)//没有lock Text!!
	{
		if (Insert_TextPos != pos)
		{
			if (pos > Text.size())//因为是UINT，所以只考虑大于的情况
				Insert_TextPos = (UINT)Text.size();
			else
				Insert_TextPos = pos;

			return true;
		}
		else {
			if (Insert_TextPos > Text.size())
				Insert_TextPos = (UINT)Text.size();
			return false;
		}
	}
	inline bool SetInsTextPos_Offset(long off)//没有lock Text!!
	{
		if ((Insert_TextPos == 0 && off < 0) || (Insert_TextPos == Text.size() && off > 0))//无用的设置直接return，不然总是刷新导致GPU占用过高
			return false;

		long pos = (long)(Insert_TextPos + off);

		if (pos < 0)
			pos = 0;
		else if (pos > (long)Text.size())
			pos = (long)Text.size();

		if ((long)Insert_TextPos != pos)
		{
			Insert_TextPos = (UINT)pos;

			return true;
		}
		else
			return false;
	}
	inline void UpdateCaret()
	{
		//立即刷新插入符
		Insert_LastUpdatetime = timeGetTime();//重置插入符的闪烁时间
		Insert_Visible = true;
		return;
	}
	void UpdateSelectPos(bool ReDraw, bool Lock = true, bool FromWheel = false)
	{
		if (Text.empty())
			return;

		___SelectLock.Lock();//记得在所有返回的地方解锁

		UINT bufSize = Select_Metrics.lineCount * Select_Metrics.maxBidiReorderingDepth;

		if (!Select_Rects)
		{
			Select_RectCount = bufSize;
			Select_Rects = new DWRITE_HIT_TEST_METRICS[Select_RectCount];
		}
		else {
			bufSize = Select_RectCount;
		}
		if (Select_BeginPos == Select_EndPos)
		{
			this->ExitSelectMode(false);

			___SelectLock.Unlock();

			TextPosToInsertPos(true, SmoothScr, false, !FromWheel);
			Insert_LastUpdatetime = timeGetTime();
			Insert_Visible = true;

			if (ReDraw)
				this->_SetNeedUpdate();
			return;
		}

		if (Lock)
			___LayoutLock.Lock();

		RecreateTextInterface();//如果相关接口为NULL，则直接重建

		while (TextLayout && TextLayout->HitTestTextRange(Select_BeginPos, Select_EndPos - Select_BeginPos, 0.0f, 0.0f, Select_Rects, bufSize, &Select_RectCount) == E_NOT_SUFFICIENT_BUFFER)
		{
			delete[] Select_Rects;
			bufSize += Select_Metrics.lineCount * Select_Metrics.maxBidiReorderingDepth;
			Select_Rects = new DWRITE_HIT_TEST_METRICS[bufSize];
		}
		//不直接用类函数是因为此函数太快了，会影响到TextLayout锁，导致卡顿
		TextPosToInsertPos(false, SmoothScr, SmoothIns, !FromWheel);
		if (Lock)
			___LayoutLock.Unlock();

		Insert_LastUpdatetime = timeGetTime();
		Insert_Visible = true;

		SelectMode = true;
		___SelectLock.Unlock();

		if (ReDraw)
			this->_SetNeedUpdate();
		return;
	}

	void SetSelectText_Pos(int xPos, int yPos, bool FromWheel = false)
	{
		if (Text.empty())
		{
			this->ExitSelectMode();
			return;
		}

		___SelectLock.Lock();//注意在所有返回处解锁
		UINT bufSize = Select_Metrics.lineCount * Select_Metrics.maxBidiReorderingDepth;
		UINT pos;

		if (!Select_Rects)
		{
			Select_RectCount = bufSize;
			Select_Rects = new DWRITE_HIT_TEST_METRICS[Select_RectCount];

			Select_BeginPos = Select_DownPos;
			Select_EndPos = Select_DownPos;
		}
		else {
			bufSize = Select_RectCount;
		}

		___LayoutLock.Lock();

		pos = ScreenPosToTextPos(xPos, yPos);

		SetInsTextPos(pos);//设置插入符位置为当前位置
		if (pos > Select_DownPos)
		{
			if (Select_EndPos != pos)
			{
				Select_BeginPos = Select_DownPos;
				Select_EndPos = pos;
			}
			else if (SelectMode)
			{
				___SelectLock.Unlock();
				TextPosToInsertPos(false, SmoothScr, false, !FromWheel);
				___LayoutLock.Unlock();
				UpdateCaret();//立即刷新插入符
				return;
			}
			Select_BeginPos = Select_DownPos;
		}
		else if (pos < Select_DownPos)
		{
			if (Select_BeginPos != pos)
			{
				Select_BeginPos = pos;
				Select_EndPos = Select_DownPos;
			}
			else if (SelectMode)
			{
				___SelectLock.Unlock();
				TextPosToInsertPos(false, SmoothScr, false, !FromWheel);
				___LayoutLock.Unlock();
				UpdateCaret();//立即刷新插入符
				return;
			}
			Select_EndPos = Select_DownPos;
		}
		else//退出选中时
		{
			this->ExitSelectMode(false);
			___SelectLock.Unlock();
			TextPosToInsertPos(false, SmoothScr, false, !FromWheel);
			___LayoutLock.Unlock();
			UpdateCaret();//立即刷新插入符
			this->_SetNeedUpdate();
			return;
		}
		RecreateTextInterface();//如果相关接口为NULL，则直接重建

		while (TextLayout && TextLayout->HitTestTextRange(Select_BeginPos, Select_EndPos - Select_BeginPos, 0.0f, 0.0f, Select_Rects, bufSize, &Select_RectCount) == E_NOT_SUFFICIENT_BUFFER)
		{
			delete[] Select_Rects;
			bufSize += Select_Metrics.lineCount * Select_Metrics.maxBidiReorderingDepth;
			Select_Rects = new DWRITE_HIT_TEST_METRICS[bufSize];
		}
		//不直接用类函数是因为此函数太快了，会影响到TextLayout锁，导致卡顿
		TextPosToInsertPos(false, SmoothScr, SmoothIns, !FromWheel);
		___LayoutLock.Unlock();

		SelectMode = true;
		___SelectLock.Unlock();

		UpdateCaret();//立即刷新插入符
		this->_SetNeedUpdate();

		return;
	}

	void ResetInsertPos(int xPos, int yPos, bool InsAnimation = true)
	{
		DWRITE_HIT_TEST_METRICS hit = { 0 };
		BOOL IsBehind = false;
		BOOL isInside = false;

		___LayoutLock.Lock();

		RecreateTextInterface(false);//如果相关接口为NULL，则直接重建

		if (TextLayout)
			TextLayout->HitTestPoint((float)xPos - TextOutputRC.left + (float)Scr_Offset.x, (float)yPos - TextOutputRC.top + (float)Scr_Offset.y, &IsBehind, &isInside, &hit);

		___LayoutLock.Unlock();

		//bidi level为偶数时表示方向为从左到右，奇数时为从右到左
		if (IsBehind)
			SetInsTextPos(hit.textPosition + 1);
		else
			SetInsTextPos(hit.textPosition);

		if (hit.bidiLevel % 2)
			IsBehind = !IsBehind;

		//设置光标的位置
		D2D1_RECT_U dst;
		dst.left = (UINT)(IsBehind ? TextOutputRC.left + hit.left + hit.width : TextOutputRC.left + hit.left);
		dst.top = (UINT)(TextOutputRC.top + hit.top);
		dst.right = dst.left + (UINT)DIP2DP(CaretWidth);
		dst.bottom = dst.top + (UINT)hit.height;

		if (Insert_PixelPos == dst)
		{
			float ofs = GetPosOffset();
			UpdateCaret();//立即刷新插入符
			this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floor(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floor(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floor(DIP2DP(1.0f)) });
		}
		else
		{
			//先给长度高度赋值，dispose那里需要根据这个计算
			Insert_PixelPos.right = (UINT)(Insert_PixelPos.left + DIP2DP(CaretWidth));
			Insert_PixelPos.bottom = (UINT)(Insert_PixelPos.top + hit.height);
			DBezier be = SmoothScrollBezier;
			A_InsX.AddAni((int)Insert_PixelPos.left, (int)dst.left, InsAnimation ? ((UINT)(FrameRate * (TextBox_CaretMoveTime / 1000.0f))) : 0, &be, 0);
			A_InsY.AddAni((int)Insert_PixelPos.top, (int)dst.top, InsAnimation ? ((UINT)(FrameRate * (TextBox_CaretMoveTime / 1000.0f))) : 0, &be, 0);

			ResetTextScrolling(true, false);
			UpdateCaret();//立即刷新插入符
			this->_SetNeedUpdate();
		}
		return;
	}

	void TextPosToInsertPos(bool Lock = true, bool ScrAni = false, bool InsAnimation = true, bool ResetScr = true, bool ReDraw = true)//此函数会调用 计算滚动的函数
	{
		DWRITE_HIT_TEST_METRICS hit;
		bool IsBehind = false;
		float pointX, pointY;

		if (Lock)
		{
			___LayoutLock.Lock();
			if (TextLayout)
				TextLayout->HitTestTextPosition(Insert_TextPos + Comp_InsPos, false, &pointX, &pointY, &hit);
			else {
				___LayoutLock.Unlock();
				return;
			}
			___LayoutLock.Unlock();
		}
		else
		{
			if (TextLayout)
				TextLayout->HitTestTextPosition(Insert_TextPos + Comp_InsPos, false, &pointX, &pointY, &hit);
			else
				return;
		}

		//bidi level为偶数时表示方向为从左到右，奇数时为从右到左
		if (hit.bidiLevel % 2)
			IsBehind = !IsBehind;

		//设置光标的位置
		D2D1_RECT_U dst;
		dst.left = IsBehind ? (UINT)(TextOutputRC.left + hit.left + hit.width) : (UINT)(TextOutputRC.left + hit.left);
		dst.top = (UINT)(TextOutputRC.top + hit.top);
		dst.right = (UINT)(dst.left + DIP2DP(CaretWidth));
		dst.bottom = (UINT)(dst.top + hit.height);

		//先给长度高度赋值，dispose那里需要根据这个计算
		Insert_PixelPos.right = (UINT)(Insert_PixelPos.left + DIP2DP(CaretWidth));
		Insert_PixelPos.bottom = (UINT)(Insert_PixelPos.top + hit.height);
		DBezier be = SmoothScrollBezier;
		A_InsX.AddAni((int)Insert_PixelPos.left, (int)dst.left, InsAnimation ? ((UINT)(FrameRate * (TextBox_CaretMoveTime / 1000.0f))) : 0, &be, 0);
		A_InsY.AddAni((int)Insert_PixelPos.top, (int)dst.top, InsAnimation ? ((UINT)(FrameRate * (TextBox_CaretMoveTime / 1000.0f))) : 0, &be, 0);

		if (ResetScr)
			ResetTextScrolling(ScrAni, ReDraw);//超出文本框时的处理
		return;
	}

	inline bool RecreateTextInterface(bool ResetScr = true)//注意，此函数不上锁，要放在锁定区域内调用
	{
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamliyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DIP2DP(FontSize), LocaleName, &TextFormat);
			if (TextFormat)
			{
				TextFormat->SetWordWrapping(AutoWarp);
				TextFormat->SetParagraphAlignment(yAlignment);
			}
			RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
		}
		if (!TextLayout && DWriteFactory && TextFormat)
		{
			___TextLock.Lock();
			UINT mInsPos = min(Insert_TextPos, Text.size());
			if (Password)
			{
				std::wstring strpass;
				for (UINT i = 0; i < Text.size(); ++i)
					strpass += L'l';
				DWriteFactory->CreateTextLayout(strpass.c_str(), strpass.size(), TextFormat, ceil(TextOutputRC.right - TextOutputRC.left), ceil(TextOutputRC.bottom - TextOutputRC.top), &TextLayout);
				if (TextLayout)
					TextLayout->SetFontFamilyName(L"Wingdings", { 0, strpass.size() });
			}
			else
				DWriteFactory->CreateTextLayout((Text.substr(0, mInsPos) + Comp_Str + Text.substr(mInsPos, Text.size() - mInsPos)).c_str(), Text.size() + Comp_Str.size(), TextFormat, ceil(TextOutputRC.right - TextOutputRC.left), ceil(TextOutputRC.bottom - TextOutputRC.top), &TextLayout);
			___TextLock.Unlock();

			if (TextLayout)
			{
				//检测是否超出边缘，如果超出，则水平方向向左对齐
				TextLayout->GetMetrics(&Select_Metrics);
				if (Select_Metrics.widthIncludingTrailingWhitespace > TextOutputRC.right - TextOutputRC.left)
					TextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				else
					TextLayout->SetTextAlignment(xAlignment);

				//如果还有组合字符串，则添加下划线
				if (Comp_Str.size())
					TextLayout->SetUnderline(true, { Insert_TextPos, (UINT)Comp_Str.size() });

				TextLayout->GetMetrics(&Select_Metrics);
				if (ResetScr)
				{
					if (SelectMode)
						UpdateSelectPos(false, false);
					else
						TextPosToInsertPos(false, SmoothScr, SmoothIns, ResetScr);
				}
				return true;
			}
		}
		return false;
	}

	inline void DetectTextOptRC(D2D1_RECT_F& TextOptRC)
	{
		if (TextOptRC.left >= TextOptRC.right)
		{
			TextOptRC.left = DstRC.rect.left;
			TextOptRC.right = DstRC.rect.right;
		}
		if (TextOptRC.top >= TextOptRC.bottom)
		{
			TextOptRC.top = DstRC.rect.top;
			TextOptRC.bottom = DstRC.rect.bottom;
		}
		return;
	}
};





class DImageBox : public DControl
{
public:
	virtual~DImageBox()
	{
		this->Release();
		return;
	}

	void Init(POINT position, SIZE size, bool CanMoveWindow = true, DScaleMode ScaleMode = DScaleMode::EqualScale, float TotalOpacity = 1.0f, DCOLOR FillColor = { 0, 0, 0, 0.0f }, DCOLOR StrokeColor = { 0, 0, 0, 0.0f }, float radiusX = 0.0f, float radiusY = 0.0f, float StrokeWidth = 0.0f)
	{
		if (this->Inited)
			return;

		this->CanMoveWindow = CanMoveWindow;

		this->DstRC_DIP.rect.left = (float)position.x;
		this->DstRC_DIP.rect.top = (float)position.y;
		this->DstSize.width = (float)size.cx;
		this->DstSize.height = (float)size.cy;
		this->DstRC_DIP.rect.right = (float)position.x + this->DstSize.width;
		this->DstRC_DIP.rect.bottom = (float)position.y + this->DstSize.height;
		this->DstRC_DIP.radiusX = radiusX;
		this->DstRC_DIP.radiusY = radiusY;
		this->StrokeWidth = StrokeWidth;
		_UpdateDpi();

		DetectColor(FillColor);
		DetectColor(StrokeColor);

		this->FillColor = DcolToColf_255(FillColor);
		this->StrokeColor = DcolToColf_255(StrokeColor);

		this->ScaleMode = ScaleMode;
		SetTotalOpacity(TotalOpacity, 0, nullptr, false);

		this->ControlType = DControlType::ImageBox;

		this->Inited = true;
		return;
	}
	///<summary>
	///从文件路径加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromFile(std::wstring Path)
	{
		if (!Inited)
			return false;
		if (!LoadInMemory(Path))
			return false;

		this->_SetNeedUpdate();

		return true;
	}
	///<summary>
	///从资源加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromResource(WORD ResourceID, std::wstring TypeName, HMODULE Module = nullptr)
	{
		if (!Inited)
			return false;
		if (!LoadInMemory(ResourceID, TypeName.c_str(), Module))
			return false;

		this->_SetNeedUpdate();

		return true;
	}
	///<summary>
	///从内存加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromMemory(BYTE* data, DWORD dataSize)
	{
		if (!Inited || !data || !dataSize)
			return false;

		DataSize = dataSize;
		DeleteArray(Data);
		Data = new BYTE[DataSize];
		if (Data)
		{
			memcpy_s(Data, DataSize, data, DataSize);
			this->_SetNeedUpdate();
		}
		else
			return false;

		return true;
	}
	///<summary>
	///从内存加载图片像素数据，4通道，格式为BGRA。pitch为宽度*4
	///</summary>
	bool CopyFromMemory(BYTE* data, ULONG dataSize, UINT pitch)
	{
		if (!Inited)
			return false;
		bool res;

		___BmpLock.Lock();
		RELEASE(Bmp);
		DeleteArray(CacheData);
		CacheData = new BYTE[dataSize];
		if (CacheData && pitch && dataSize)
		{
			memcpy_s(CacheData, dataSize, data, dataSize);
			BmpPitch = pitch;
			BufferSize = dataSize;
			res = true;
		}
		else
			res = false;

		___BmpLock.Unlock();

		if (res)
			this->_SetNeedUpdate();

		return res;
	}


	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		ControlType = DControlType::ImageBox;

		A_Ang.Reset();
		A_ImgOpa.Reset();
		A_Fill.Reset();
		FillColor = { 0 };
		StrokeColor = { 0 };
		TotalOpacity = 1.0f;
		RotationAngle = 0.0f;
		ImgOpacity = 1.0f;
		RELEASE(brush);
		ScaleMode = DScaleMode::EqualScale;
		DeleteArray(Data);
		DataSize = 0;
		BmpRC = { 0 };
		RELEASE(Bmp);
		LoadingRC = { 0 };
		//加载成功后，将缩放到合适大小的图像数据保存，以便重建
		DeleteArray(CacheData);
		BufferSize = 0;
		BmpPitch = 0;

		return;
	}

	//设备无关像素
	void GetSize(UINT& Width, UINT& Height)
	{
		Width = (UINT)DP2DIP(BmpSize.width);
		Height = (UINT)DP2DIP(BmpSize.height);
		return;
	}
	//设备相关像素
	void GetPixelSize(UINT& Width, UINT& Height)
	{
		Width = (UINT)BmpSize.width;
		Height = (UINT)BmpSize.height;
		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Radius_X.AddAni(DstRC_DIP.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_Radius_Y.AddAni(DstRC_DIP.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		DstRC_DIP.radiusX = RadiusX;
		DstRC_DIP.radiusY = RadiusY;
		_UpdateDpi_DstRCRadius();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();
		this->_SetNeedUpdate();
		return;
	}

	void SetImageOpacity(float opacity, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_ImgOpa.AddAni(ImgOpacity, opacity, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		ImgOpacity = opacity;
		this->_SetNeedUpdate();
		return;
	}

	void SetImageRotation(float angle, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Ang.AddAni(RotationAngle, angle, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		RotationAngle = angle;
		this->_SetNeedUpdate();
		return;
	}

	void SetBackColor(DCOLOR col, UINT time = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Fill.AddAni({ FillColor.r * 255.0f, FillColor.g * 255.0f, FillColor.b * 255.0f, FillColor.a }, col, (UINT)(FrameRate * (time / 1000.0f)));
			this->_SetNeedUpdate();
			return;
		}
		FillColor = DcolToColf_255(col);
		this->_SetNeedUpdate();
		return;
	}

	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();
		//重新计算Bmp位置, 一定要在UpdateDpi下面
		CalcBmpSize(BmpRC, (UINT)DstSize.width, (UINT)DstSize.height);
		BmpSize.width = BmpRC.right - BmpRC.left;
		BmpSize.height = BmpRC.bottom - BmpRC.top;

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();
		//重新计算Bmp位置, 一定要在UpdateDpi下面
		CalcBmpSize(BmpRC, (UINT)DstSize.width, (UINT)DstSize.height);
		BmpSize.width = BmpRC.right - BmpRC.left;
		BmpSize.height = BmpRC.bottom - BmpRC.top;

		//重新创建HitTest
		this->_CreateHitTestGeo();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}


	bool GaussianBlur(float Sigma, UINT Threads = 4)
	{
		if (!Inited || !CacheData)
			return false;

		___GauLock.Lock();

		___BmpLock.Lock();
		_Gau_Blur(CacheData, (UINT)BmpSize.width, (UINT)BmpSize.height, BmpPitch, Sigma, Threads);

		RELEASE(Bmp);
		___BmpLock.Unlock();

		this->_SetNeedUpdate();

		___GauLock.Unlock();
		return true;
	}

private:
	D2D1_COLOR_F FillColor = { 0 };
	D2D1_COLOR_F StrokeColor = { 0 };
	ID2D1SolidColorBrush* brush = nullptr;

	DScaleMode ScaleMode = DScaleMode::EqualScale;

	//加载时期的变量，如果加载失败不影响现在的图片
	BYTE* Data = nullptr;
	ULONG DataSize = 0;
	D2D1_RECT_F LoadingRC = { 0 };

	___DThreadLock ___BmpLock;
	ID2D1Bitmap* Bmp = nullptr;

	//加载成功后，将缩放到合适大小的图像数据保存，以便重建
	BYTE* CacheData = nullptr;
	UINT BufferSize = 0;
	UINT BmpPitch = 0;
	D2D1_RECT_F BmpRC = { 0 };
	D2D1_SIZE_F BmpSize = { 0 };

	float ImgOpacity = 1.0f;
	_AniCalc_f A_ImgOpa;

	float RotationAngle = 0.0f;
	_AniCalc_f A_Ang;

	_Ani_Color A_Fill;

	___DThreadLock ___GauLock;

	void _SeparateFromWindow()
	{
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		A_ImgOpa.Reset();
		A_Ang.Reset();
		A_Fill.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(RoundGeo);
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(Bmp);
		RELEASE(HitTestGeo);
		return;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		DstRC.rect.left = floor(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = floor(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = floor(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = floor(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			UpdateRC.left = floor(DstRC.rect.left - DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floor(DstRC.rect.top - DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceil(DstRC.rect.right + DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceil(DstRC.rect.bottom + DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			UpdateRC = DstRC.rect;
		}
		return;
	}

	bool _SetCursorMsgProc()
	{
		SetCursor(this->CursorStyle);
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		if (NowControlID == ID)
			return false;

		if (Msg == WM_MOUSEMOVE && OnClick)
			return false;
		else if (Msg == WM_LBUTTONUP)
			OnClick = false;

		return m_SetState(DControlState::Normal);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		___StateLock.Lock();
		if (State == DControlState::Click)
		{
			___StateLock.Unlock();
			return false;
		}

		if (OnClick)
			return m_SetState(DControlState::Click, true);

		return m_SetState(DControlState::MouseMove, true);
	}
	bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos)
	{
		OnClick = true;

		return m_SetState(DControlState::Click);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		if (!OnClick || (OnClick && !this->HitTest(xPos, yPos)))
		{
			OnClick = false;
			return false;
		}
		OnClick = false;

		return m_SetState(DControlState::MouseMove);
	}

	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		if (State != state && state != DControlState::Disable)
		{
			DControlState LastState = State;
			State = state;

			mSendConMsg(DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);

			if (NeedUnlock)
				___StateLock.Unlock();
			return true;
		}
		if (NeedUnlock)
			___StateLock.Unlock();
		return false;
	}

	bool _CreateHitTestGeo()//创建命中测试几何
	{
		___HitTestLock.Lock();
		RELEASE(HitTestGeo);
		if (D2DFactory)
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DIP2DP(DstSize.width + StrokeWidth), DIP2DP(DstSize.height + StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	bool _IsContentChanging()
	{
		if (A_Fill || A_Ang || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa)
			return true;
		else
			return false;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Fill)
		{
			A_Fill.FrameCalc(FillColor);
			FillColor = Calc_fColor(FillColor, 1.0f);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(AID);
				if (!A_Radius_Y)
					resp = true;
			}
			SetRadius(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}
		if (A_ImgOpa)
		{
			UINT AID = 0;
			SetImageOpacity(A_ImgOpa.FrameCalc(AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}
		if (A_Ang)
		{
			UINT AID = 0;
			SetImageRotation(A_Ang.FrameCalc(AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//如果没有加载到图片，则加载
		___BmpLock.Lock();
		if (Data)
		{
			if (MyLoadBmp(DstTarget, Data, DataSize, &Bmp, LoadingRC) && Bmp)
			{
				memcpy_s(&BmpRC, sizeof(BmpRC), &LoadingRC, sizeof(BmpRC));
				BmpSize.width = BmpRC.right - BmpRC.left;
				BmpSize.height = BmpRC.bottom - BmpRC.top;

				mSendConMsg(DControlMsg::Image_Loaded, 0, 0);
			}

			DeleteArray(Data);
			DataSize = 0;
			LoadingRC = { 0 };
		}

		if (!Bmp && CacheData && BmpPitch && BufferSize)
		{
			D2D1_PIXEL_FORMAT fm = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
			D2D1_BITMAP_PROPERTIES prop = { fm, 96.0f, 96.0f };

			D2D1_RECT_U d2drc = { 0, 0, BmpPitch / 4, BufferSize / BmpPitch };

			CalcBmpSize(BmpRC, d2drc.right, d2drc.bottom);

			if (SUCCEEDED(DstTarget->CreateBitmap({ d2drc.right, d2drc.bottom }, prop, &Bmp)) && Bmp)
			{
				Bmp->CopyFromMemory(&d2drc, CacheData, BmpPitch);
			}
		}

		//如果有圆角，则用图层实现圆角
		if (DstRC.radiusX != 0.0f || DstRC.radiusY != 0.0f)
		{
			___RoundGeoLock.Lock();

			if (!RoundGeo && D2DFactory)
				D2DFactory->CreateRoundedRectangleGeometry(DstRC, &RoundGeo);

			if (RoundGeo)
				Clip.PushLayer(DstTarget, UpdateRC, RoundGeo, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

			___RoundGeoLock.Unlock();
		}

		bool HaveSetAngle = false;
		if (RotationAngle != 0.0f)
		{
			DstTarget->SetTransform(D2D1::Matrix3x2F::Rotation(RotationAngle, { (BmpRC.right + BmpRC.left) / 2.0f, (BmpRC.bottom + BmpRC.top) / 2.0f }));
			HaveSetAngle = true;
		}
		//开始绘制
		if (FillColor.a * TotalOpacity != 0.0f)
		{
			brush->SetColor({ FillColor.r, FillColor.g, FillColor.b, FillColor.a * TotalOpacity });//设置为填充颜色
			DstTarget->FillRectangle(DstRC.rect, brush);
		}
		//画图片
		if (Bmp && TotalOpacity * ImgOpacity != 0.0f)
		{
			DstTarget->DrawBitmap(Bmp, BmpRC, TotalOpacity * ImgOpacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
		}

		___BmpLock.Unlock();

		if (StrokeColor.a * TotalOpacity != 0.0f)
		{
			brush->SetColor({ StrokeColor.r, StrokeColor.g, StrokeColor.b, StrokeColor.a * TotalOpacity });//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}


	inline bool LoadInMemory(std::wstring Path)
	{
		HANDLE hFile = CreateFileW(Path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		//返回值size_high,size_low分别表示文件大小的高32位/低32位
		DWORD size_low, size_high;
		size_low = GetFileSize(hFile, &size_high);

		//创建文件的内存映射文件
		HANDLE hMapFile = CreateFileMappingW(hFile, nullptr, PAGE_READONLY, size_high, size_low, nullptr);
		if (hMapFile == INVALID_HANDLE_VALUE || !hMapFile)
		{
			CloseHandle(hFile);
			return false;
		}

		//把文件数据映射到进程的地址空间
		void* pvFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
		if (pvFile)
		{
			DeleteArray(Data);
			Data = new BYTE[size_low];
			if (Data)
			{
				DataSize = size_low;
				memcpy_s(Data, DataSize, pvFile, DataSize);
			}
			//结束
			UnmapViewOfFile(pvFile);//撤销映射
		}
		CloseHandle(hFile);//关闭文件
		return (bool)Data;
	}
	inline bool LoadInMemory(WORD ResourceID, const wchar_t* TypeName, HMODULE Module)
	{
		HRSRC hRsrc = FindResourceW(Module, MAKEINTRESOURCEW(ResourceID), TypeName);
		if (!hRsrc)
			return false;

		//获取资源的大小
		DataSize = SizeofResource(Module, hRsrc);
		if (!DataSize)
			return false;

		//加载资源
		HGLOBAL hGlobal = LoadResource(Module, hRsrc);
		if (!hGlobal)
			return false;

		//锁定资源
		LPVOID pBuffer = LockResource(hGlobal);
		if (!pBuffer)
			return false;

		DeleteArray(Data);
		Data = new BYTE[DataSize];
		if (Data)
		{
			memcpy_s(Data, DataSize, pBuffer, DataSize);
		}
		return (bool)Data;
	}

	inline bool MyLoadBmp(ID2D1DeviceContext*& Target, BYTE* data, ULONG Size, ID2D1Bitmap** bmp, D2D1_RECT_F& bmprc)
	{
		if (!Target || !bmp || !WicFactory)///////使用条件检测
			return false;

		IWICStream* Stream = nullptr;
		//先把内存数据加载到WICStream
		WicFactory->CreateStream(&Stream);
		if (!Stream)
			return false;
		if (FAILED(Stream->InitializeFromMemory(data, Size)))
		{
			RELEASE(Stream);
			return false;
		}

		IWICBitmapDecoder* pDecoder = nullptr;
		IWICBitmapFrameDecode* pFrame = nullptr;
		IWICFormatConverter* pFormat = nullptr;
		IWICBitmapScaler* pScaler = nullptr;

		HRESULT hr = WicFactory->CreateDecoderFromStream(Stream, nullptr, WICDecodeMetadataCacheOnDemand, &pDecoder);
		if (SUCCEEDED(hr))
		{
			hr = pDecoder->GetFrame(0, &pFrame);
		}
		if (SUCCEEDED(hr))
		{
			hr = WicFactory->CreateFormatConverter(&pFormat);
		}
		if (SUCCEEDED(hr))
		{
			hr = pFormat->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
		}

		if (SUCCEEDED(hr))
		{
			UINT wid, hei;
			hr = pFormat->GetSize(&wid, &hei);
			if (SUCCEEDED(hr))
			{
				CalcBmpSize(bmprc, wid, hei);

				//将WICBMP转到D2D的缓存Bmp中来,不要用CreateBitmapFromWicBitmap(),这样加载某些图片会导致播放过程中明显卡一下
				hr = WicFactory->CreateBitmapScaler(&pScaler);

				if (SUCCEEDED(hr))
				{
					UINT ScaleWid = (UINT)(bmprc.right - bmprc.left), ScaleHei = (UINT)(bmprc.bottom - bmprc.top);
					if (ScaleWid < wid && ScaleHei < hei)
						hr = pScaler->Initialize(pFormat, ScaleWid, ScaleHei, WICBitmapInterpolationModeFant);
					else
						hr = pScaler->Initialize(pFormat, ScaleWid, ScaleHei, WICBitmapInterpolationModeCubic);

					if (SUCCEEDED(hr))
					{
						D2D1_PIXEL_FORMAT fm = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
						D2D1_BITMAP_PROPERTIES prop = { fm, 96.0f, 96.0f };

						hr = pScaler->GetSize(&wid, &hei);

						if (SUCCEEDED(hr))
						{
							RELEASE(*bmp);
							hr = Target->CreateBitmap(D2D1::SizeU(wid, hei), prop, bmp);
						}

						if (SUCCEEDED(hr))
						{
							IWICBitmapLock* lock;
							IWICBitmap* WicBmp;
							WICRect rect = { 0, 0, (int)wid, (int)hei };
							D2D1_RECT_U d2drc = D2D1::RectU(0, 0, wid, hei);
							BYTE* cv;

							hr = WicFactory->CreateBitmapFromSource(pScaler, WICBitmapNoCache, &WicBmp);
							if (SUCCEEDED(hr))
							{
								hr = WicBmp->Lock(&rect, WICBitmapLockRead, &lock);
								if (SUCCEEDED(hr) && lock)
								{
									lock->GetStride(&BmpPitch);
									lock->GetDataPointer(&BufferSize, &cv);

									DeleteArray(CacheData);
									CacheData = new BYTE[BufferSize];
									if (!CacheData)
										hr = E_FAIL;
									else
									{
										memcpy_s(CacheData, sizeof(BYTE) * BufferSize, cv, BufferSize);

										RELEASE(lock);

										if (*bmp)
											hr = (*bmp)->CopyFromMemory(&d2drc, CacheData, BmpPitch);
									}
								}
							}
							RELEASE(WicBmp);
						}
					}
				}
			}
		}
		RELEASE(pScaler);
		RELEASE(pFormat);
		RELEASE(pFrame);
		RELEASE(pDecoder);
		RELEASE(Stream);

		return SUCCEEDED(hr);
	}

	//设备相关像素
	void CalcBmpSize(D2D1_RECT_F& bmprc, const UINT& wid, const UINT& hei)
	{
		//计算目标图片大小
		if (this->ScaleMode == DScaleMode::EqualScale)
		{
			RECT GoodRc = { 0 };
			GoodRc = RectSizeWithConstantRatio({ 0, 0, (long)DIP2DP(DstSize.width), (long)DIP2DP(DstSize.height) }, { (long)wid, (long)hei });

			bmprc.left = DstRC.rect.left + (float)GoodRc.left;
			bmprc.top = DstRC.rect.top + (float)GoodRc.top;
			bmprc.right = DstRC.rect.left + (float)GoodRc.right;
			bmprc.bottom = DstRC.rect.top + (float)GoodRc.bottom;
		}
		else if (this->ScaleMode == DScaleMode::Fill)
			bmprc = DstRC.rect;

		return;
	}

	RECT RectSizeWithConstantRatio(RECT rcScreen, SIZE sizePicture)
	{
		RECT rect(rcScreen);

		double dWidth = (double)rcScreen.right - (double)rcScreen.left;
		double dHeight = (double)rcScreen.bottom - (double)rcScreen.top;
		double dAspectRatio = dWidth / dHeight;
		double dPictureWidth = sizePicture.cx;
		double dPictureHeight = sizePicture.cy;
		double dPictureAspectRatio = dPictureWidth / dPictureHeight;

		if (dPictureAspectRatio > dAspectRatio)
		{
			int nNewHeight = (int)(dWidth / dPictureWidth * dPictureHeight);
			int nCenteringFactor = ((rcScreen.bottom - rcScreen.top) - nNewHeight) / 2;
			rect.left = 0;
			rect.top = nCenteringFactor;
			rect.right = (int)dWidth;
			rect.bottom = nNewHeight + nCenteringFactor;
		}
		else if (dPictureAspectRatio < dAspectRatio)
		{

			int nNewWidth = (int)(dHeight / dPictureHeight * dPictureWidth);
			int nCenteringFactor = ((rcScreen.right - rcScreen.left) - nNewWidth) / 2;
			rect.left = nCenteringFactor;
			rect.top = 0;
			rect.right = nNewWidth + nCenteringFactor;
			rect.bottom = (int)dHeight;
		}
		return rect;
	}

};



class DGifBox : public DControl
{
public:
	virtual~DGifBox()
	{
		this->Release();
		return;
	}

	///<summary>
	///从文件路径加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromFile(POINT position, SIZE size, std::wstring Path, bool CanMoveWindow = true, DScaleMode ScaleMode = DScaleMode::EqualScale, float TotalOpacity = 1.0f, DCOLOR FillColor = { 0, 0, 0, 0.0f }, DCOLOR StrokeColor = { 0, 0, 0, 0.0f }, float radiusX = 0.0f, float radiusY = 0.0f, float StrokeWidth = 0.0f)
	{
		this->CanMoveWindow = CanMoveWindow;

		DstRC_DIP.rect.left = (float)position.x;
		DstRC_DIP.rect.top = (float)position.y;
		DstSize.width = (float)size.cx;
		DstSize.height = (float)size.cy;
		DstRC_DIP.rect.right = (float)position.x + DstSize.width;
		DstRC_DIP.rect.bottom = (float)position.y + DstSize.height;
		DstRC_DIP.radiusX = radiusX;
		DstRC_DIP.radiusY = radiusY;
		this->StrokeWidth = StrokeWidth;

		_UpdateDpi();

		DCOLOR fill, stroke;
		memcpy_s(&fill, sizeof(fill), &FillColor, sizeof(FillColor));
		memcpy_s(&stroke, sizeof(stroke), &StrokeColor, sizeof(StrokeColor));
		DetectColor(fill);
		DetectColor(stroke);

		this->FillColor = DcolToColf_255(fill);
		this->StrokeColor = DcolToColf_255(stroke);

		this->ScaleMode = ScaleMode;
		SetTotalOpacity(TotalOpacity, 0, nullptr, false);

		this->ControlType = DControlType::GifBox;

		Inited = LoadInMemory(Path);
		if (Inited)
			this->_SetNeedUpdate();

		return Inited;
	}
	///<summary>
	///从资源加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromResource(POINT position, SIZE size, WORD ResourceID, std::wstring TypeName, HMODULE Module = nullptr, bool CanMoveWindow = true, DScaleMode ScaleMode = DScaleMode::EqualScale, float TotalOpacity = 1.0f, DCOLOR FillColor = { 0, 0, 0, 0.0f }, DCOLOR StrokeColor = { 0, 0, 0, 0.0f }, float radiusX = 0.0f, float radiusY = 0.0f, float StrokeWidth = 0.0f)
	{
		this->CanMoveWindow = CanMoveWindow;

		DstRC_DIP.rect.left = (float)position.x;
		DstRC_DIP.rect.top = (float)position.y;
		DstSize.width = (float)size.cx;
		DstSize.height = (float)size.cy;
		DstRC_DIP.rect.right = (float)position.x + DstSize.width;
		DstRC_DIP.rect.bottom = (float)position.y + DstSize.height;
		DstRC_DIP.radiusX = radiusX;
		DstRC_DIP.radiusY = radiusY;
		this->StrokeWidth = StrokeWidth;

		_UpdateDpi();

		DCOLOR fill, stroke;
		memcpy_s(&fill, sizeof(fill), &FillColor, sizeof(FillColor));
		memcpy_s(&stroke, sizeof(stroke), &StrokeColor, sizeof(StrokeColor));
		DetectColor(fill);
		DetectColor(stroke);

		this->FillColor = DcolToColf_255(fill);
		this->StrokeColor = DcolToColf_255(stroke);

		this->ScaleMode = ScaleMode;
		SetTotalOpacity(TotalOpacity, 0, nullptr, false);

		this->ControlType = DControlType::GifBox;

		Inited = LoadInMemory(ResourceID, TypeName.c_str(), Module);
		if (Inited)
			this->_SetNeedUpdate();

		return Inited;
	}

	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		ControlType = DControlType::GifBox;

		A_Ang.Reset();
		A_Fill.Reset();
		FillColor = { 0 };
		StrokeColor = { 0 };
		TotalOpacity = 1.0f;
		RotationAngle = 0.0f;
		RELEASE(brush);
		ScaleMode = DScaleMode::EqualScale;
		BmpRC = { 0 };
		RELEASE(Bmp);
		DeleteArray(CacheData);
		BufferSize = 0;
		if (WicFactory)
		{
			RELEASE(pDecoder);
			RELEASE(Stream);
		}
		Playing = false;
		FrameCount = 0;
		NowFrameCount = 0;
		LastFrameDelay = 0;
		LastFrameTime = 0;
		BmpSize = { 0 };
		return;
	}

	void GetSize(UINT* Width, UINT* Height)
	{
		if (Width)
			*Width = (UINT)DIP2DP(BmpSize.width);
		if (Height)
			*Height = (UINT)DIP2DP(BmpSize.height);
		return;
	}
	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Radius_X.AddAni(DstRC_DIP.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_Radius_Y.AddAni(DstRC_DIP.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}

		DstRC_DIP.radiusX = RadiusX;
		DstRC_DIP.radiusY = RadiusY;
		_UpdateDpi_DstRCRadius();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();
		this->_SetNeedUpdate();
		return;
	}
	void SetGifRotation(float angle, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Ang.AddAni(RotationAngle, angle, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		RotationAngle = angle;
		this->_SetNeedUpdate();
		return;
	}

	void SetPlay(bool play)
	{
		Playing = play;
		if (play)
			this->_SetNeedUpdate();
	}
	void ResetPlay()
	{
		NowFrameCount = 0;
		this->_SetNeedUpdate();
	}

	void SetBackColor(DCOLOR col, UINT time = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Fill.AddAni({ FillColor.r * 255.0f, FillColor.g * 255.0f, FillColor.b * 255.0f, FillColor.a }, col, (UINT)(FrameRate * (time / 1000.0f)));
			this->_SetNeedUpdate();
			return;
		}
		FillColor = DcolToColf_255(col);
		this->_SetNeedUpdate();
		return;
	}

	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();
		//重新计算Bmp位置, 一定要在UpdateDpi下面
		CalcBmpSize(BmpRC, (UINT)DstSize.width, (UINT)DstSize.height);
		BmpSize.width = BmpRC.right - BmpRC.left;
		BmpSize.height = BmpRC.bottom - BmpRC.top;

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();
		//重新计算Bmp位置, 一定要在UpdateDpi下面
		CalcBmpSize(BmpRC, (UINT)DstSize.width, (UINT)DstSize.height);
		BmpSize.width = BmpRC.right - BmpRC.left;
		BmpSize.height = BmpRC.bottom - BmpRC.top;

		//重新创建HitTest
		this->_CreateHitTestGeo();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}

private:
	D2D1_COLOR_F FillColor = { 0 };
	D2D1_COLOR_F StrokeColor = { 0 };

	//用来实现圆角的裁剪区
	ID2D1RoundedRectangleGeometry* RoundGeo = nullptr;
	___DThreadLock ___RoundGeoLock;

	ID2D1SolidColorBrush* brush = nullptr;

	DScaleMode ScaleMode = DScaleMode::EqualScale;

	bool Playing = false;//是否正在播放状态

	IWICStream* Stream = nullptr;
	IWICBitmapDecoder* pDecoder = nullptr;
	UINT FrameCount = 0;
	UINT NowFrameCount = 0;
	UINT LastFrameDelay = 0;
	DWORD LastFrameTime = 0;

	___DThreadLock ___BmpLock;
	ID2D1Bitmap* Bmp = nullptr;

	//加载成功后，将缩放到合适大小的图像数据保存，以便重建
	BYTE* CacheData = nullptr;
	UINT BufferSize = 0;
	D2D1_RECT_F BmpRC = { 0 };
	D2D1_SIZE_F BmpSize = { 0 };

	float RotationAngle = 0.0f;
	_AniCalc_f A_Ang;

	_Ani_Color A_Fill;

	void _SeparateFromWindow()
	{
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		A_Ang.Reset();
		A_Fill.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		Playing = false;
		RELEASE(pDecoder);
		RELEASE(Stream);
		FrameCount = 0;
		NowFrameCount = 0;
		LastFrameDelay = 0;
		LastFrameTime = 0;
		BmpSize = { 0 };
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(RoundGeo);
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(Bmp);
		RELEASE(HitTestGeo);
		return;
	}

	bool _GetNeedUpdate()//根据每个控件的GetNeedUpdate每帧都会调用的特性，在这里为插入符计时并设置刷新
	{
		if (Playing && FrameCount > 1 && timeGetTime() - LastFrameTime < LastFrameDelay)
		{
			this->_SetNeedUpdate();
		}

		return NeedUpdate;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		DstRC.rect.left = floor(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = floor(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = floor(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = floor(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			UpdateRC.left = floor(DstRC.rect.left - DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floor(DstRC.rect.top - DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceil(DstRC.rect.right + DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceil(DstRC.rect.bottom + DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			UpdateRC = DstRC.rect;
		}
		return;
	}

	bool _SetCursorMsgProc()
	{
		SetCursor(this->CursorStyle);
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		if (NowControlID == ID)
			return false;

		if (Msg == WM_MOUSEMOVE && OnClick)
			return false;
		else if (Msg == WM_LBUTTONUP)
			OnClick = false;

		return m_SetState(DControlState::Normal);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		___StateLock.Lock();
		if (State == DControlState::Click)
		{
			___StateLock.Unlock();
			return false;
		}

		if (OnClick)
			return m_SetState(DControlState::Click, true);

		return m_SetState(DControlState::MouseMove, true);
	}
	bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos)
	{
		OnClick = true;

		return m_SetState(DControlState::Click);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		if (!OnClick || (OnClick && !this->HitTest(xPos, yPos)))
		{
			OnClick = false;
			return false;
		}
		OnClick = false;

		return m_SetState(DControlState::MouseMove);
	}

	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		if (State != state && state != DControlState::Disable)
		{
			DControlState LastState = State;
			State = state;

			mSendConMsg(DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);

			if (NeedUnlock)
				___StateLock.Unlock();
			return true;
		}
		if (NeedUnlock)
			___StateLock.Unlock();
		return false;
	}

	bool _CreateHitTestGeo()//创建命中测试几何
	{
		___HitTestLock.Lock();
		RELEASE(HitTestGeo);
		if (D2DFactory)
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DIP2DP(DstSize.width + StrokeWidth), DIP2DP(DstSize.height + StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	bool _IsContentChanging()
	{
		if (A_Fill || A_Ang || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa)
			return true;
		else
			return false;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Fill)
		{
			A_Fill.FrameCalc(FillColor);
			FillColor = Calc_fColor(FillColor, 1.0f);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(AID);
				if (!A_Radius_Y)
					resp = true;
			}
			SetRadius(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}
		if (A_Ang)
		{
			UINT AID = 0;
			SetGifRotation(A_Ang.FrameCalc(AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//如果没有加载到图片，则加载

		if (!(Stream && pDecoder) && CacheData && BufferSize)
		{
			if (MyLoadData(CacheData, BufferSize))
			{
				BmpSize.width = BmpRC.right - BmpRC.left;
				BmpSize.height = BmpRC.bottom - BmpRC.top;

				mSendConMsg(DControlMsg::Gif_Loaded, 0, 0);
			}
		}

		___BmpLock.Lock();

		if (FrameCount > 1 && Playing)
			RELEASE(Bmp);

		if (!Bmp && pDecoder)
		{
			LastFrameTime = timeGetTime();
			MyNextFrame(DstTarget, &Bmp, BmpRC, LastFrameDelay);

			if (Playing)
			{
				++NowFrameCount;
				if (NowFrameCount >= FrameCount)
				{
					mSendConMsg(DControlMsg::Gif_EndOfPlay, 0, 0);
					NowFrameCount = 0;
				}
			}
		}
		//开始绘制部分
		//如果有圆角，则用图层实现圆角
		if (DstRC.radiusX != 0.0f || DstRC.radiusY != 0.0f)
		{
			___RoundGeoLock.Lock();

			if (!RoundGeo && D2DFactory)
				D2DFactory->CreateRoundedRectangleGeometry(DstRC, &RoundGeo);

			if (RoundGeo)
				Clip.PushLayer(DstTarget, UpdateRC, RoundGeo, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

			___RoundGeoLock.Unlock();
		}

		bool HaveSetAngle = false;
		if (RotationAngle != 0.0f)
		{
			DstTarget->SetTransform(D2D1::Matrix3x2F::Rotation(RotationAngle, { (BmpRC.right + BmpRC.left) / 2.0f, (BmpRC.bottom + BmpRC.top) / 2.0f }));
			HaveSetAngle = true;
		}
		//开始绘制
		if (FillColor.a * TotalOpacity != 0.0f)
		{
			brush->SetColor({ FillColor.r, FillColor.g, FillColor.b, FillColor.a * TotalOpacity });//设置为填充颜色
			DstTarget->FillRectangle(DstRC.rect, brush);
		}
		//画图片
		if (Bmp && TotalOpacity != 0.0f)
		{
			DstTarget->DrawBitmap(Bmp, BmpRC, TotalOpacity, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		}
		___BmpLock.Unlock();

		if (StrokeColor.a * TotalOpacity != 0.0f)
		{
			brush->SetColor({ StrokeColor.r, StrokeColor.g, StrokeColor.b, StrokeColor.a * TotalOpacity });//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		if (HaveSetAngle)
			DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}


	inline bool LoadInMemory(std::wstring Path)
	{
		HANDLE hFile = CreateFileW(Path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		//返回值size_high,size_low分别表示文件大小的高32位/低32位
		DWORD size_low, size_high;
		size_low = GetFileSize(hFile, &size_high);

		//创建文件的内存映射文件
		HANDLE hMapFile = CreateFileMappingW(hFile, nullptr, PAGE_READONLY, size_high, size_low, nullptr);
		if (hMapFile == INVALID_HANDLE_VALUE || !hMapFile)
		{
			CloseHandle(hFile);
			return false;
		}

		//把文件数据映射到进程的地址空间
		void* pvFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
		if (pvFile)
		{
			DeleteArray(CacheData);
			CacheData = new BYTE[size_low];
			if (CacheData)
			{
				BufferSize = size_low;
				memcpy_s(CacheData, BufferSize, pvFile, BufferSize);
			}
			//结束
			UnmapViewOfFile(pvFile);//撤销映射
		}
		CloseHandle(hFile);//关闭文件
		return (bool)CacheData;
	}
	inline bool LoadInMemory(WORD ResourceID, const wchar_t* TypeName, HMODULE Module)
	{
		HRSRC hRsrc = FindResourceW(Module, MAKEINTRESOURCEW(ResourceID), TypeName);
		if (!hRsrc)
			return false;

		//获取资源的大小
		BufferSize = SizeofResource(Module, hRsrc);
		if (!BufferSize)
			return false;

		//加载资源
		HGLOBAL hGlobal = LoadResource(Module, hRsrc);
		if (!hGlobal)
			return false;

		//锁定资源
		LPVOID pBuffer = LockResource(hGlobal);
		if (!pBuffer)
			return false;

		DeleteArray(CacheData);
		CacheData = new BYTE[BufferSize];
		if (CacheData)
		{
			memcpy_s(CacheData, BufferSize, pBuffer, BufferSize);
		}
		return (bool)CacheData;
	}

	bool MyLoadData(BYTE* data, ULONG Size)
	{
		if (!WicFactory)///////使用条件检测
			return false;

		RELEASE(pDecoder);
		RELEASE(Stream);
		FrameCount = 0;
		NowFrameCount = 0;

		//先把内存数据加载到WICStream
		WicFactory->CreateStream(&Stream);
		if (!Stream)
			return false;
		if (FAILED(Stream->InitializeFromMemory(data, Size)))
		{
			RELEASE(Stream);
			return false;
		}
		if (FAILED(WicFactory->CreateDecoderFromStream(Stream, nullptr, WICDecodeMetadataCacheOnDemand, &pDecoder)))
		{
			RELEASE(Stream);
			return false;
		}
		if (FAILED(pDecoder->GetFrameCount(&FrameCount)) || !FrameCount)
		{
			RELEASE(pDecoder);
			RELEASE(Stream);
			return false;
		}

		return true;
	}
	bool MyNextFrame(ID2D1DeviceContext*& Target, ID2D1Bitmap** bmp, D2D1_RECT_F& bmprc, UINT& FrameDelay)
	{
		if (!Target || !bmp || !pDecoder || !WicFactory)///////使用条件检测
			return false;

		IWICBitmapFrameDecode* pFrame = nullptr;
		IWICFormatConverter* pFormat = nullptr;
		IWICBitmapScaler* pScaler = nullptr;

		HRESULT hr = pDecoder->GetFrame(NowFrameCount, &pFrame);
		if (SUCCEEDED(hr))
		{
			MyGetFrameDelay(pFrame, FrameDelay);
			hr = WicFactory->CreateFormatConverter(&pFormat);
		}
		if (SUCCEEDED(hr))
		{
			hr = pFormat->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
		}

		if (SUCCEEDED(hr))
		{
			UINT wid, hei;
			hr = pFormat->GetSize(&wid, &hei);
			if (SUCCEEDED(hr))
			{
				CalcBmpSize(bmprc, wid, hei);

				hr = WicFactory->CreateBitmapScaler(&pScaler);

				if (SUCCEEDED(hr))
				{
					UINT ScaleWid = (UINT)(DIP2DP(bmprc.right - bmprc.left)), ScaleHei = (UINT)(DIP2DP(bmprc.bottom - bmprc.top));
					if (ScaleWid < wid && ScaleHei < hei)
						hr = pScaler->Initialize(pFormat, ScaleWid, ScaleHei, WICBitmapInterpolationModeFant);
					else
						hr = pScaler->Initialize(pFormat, ScaleWid, ScaleHei, WICBitmapInterpolationModeCubic);

					if (SUCCEEDED(hr))
					{
						D2D1_PIXEL_FORMAT fm = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
						D2D1_BITMAP_PROPERTIES prop = { fm, 96.0f, 96.0f };

						hr = pScaler->GetSize(&wid, &hei);

						if (SUCCEEDED(hr))
						{
							RELEASE(*bmp);
							hr = Target->CreateBitmap(D2D1::SizeU(wid, hei), prop, bmp);
						}

						if (SUCCEEDED(hr))
						{
							IWICBitmapLock* lock;
							IWICBitmap* WicBmp;
							WICRect rect = { 0, 0, (int)wid, (int)hei };
							D2D1_RECT_U d2drc = D2D1::RectU(0, 0, wid, hei);
							BYTE* cv;

							hr = WicFactory->CreateBitmapFromSource(pScaler, WICBitmapNoCache, &WicBmp);
							if (SUCCEEDED(hr))
							{
								hr = WicBmp->Lock(&rect, WICBitmapLockRead, &lock);
								if (SUCCEEDED(hr) && lock)
								{
									UINT mPitch = 0, FrameBufSize = 0;
									lock->GetStride(&mPitch);
									lock->GetDataPointer(&FrameBufSize, &cv);

									if (cv && mPitch && (*bmp))
										hr = (*bmp)->CopyFromMemory(&d2drc, cv, mPitch);
									else
										hr = E_FAIL;

									RELEASE(lock);
								}
							}
							RELEASE(WicBmp);
						}
					}
				}
			}
		}
		RELEASE(pScaler);
		RELEASE(pFormat);
		RELEASE(pFrame);

		return SUCCEEDED(hr);
	}

	//获取GIF的帧延迟的值
	HRESULT MyGetFrameDelay(IWICBitmapFrameDecode* pFrame, UINT& delay)
	{
		PROPVARIANT propValue;
		PropVariantInit(&propValue);

		HRESULT hr;
		IWICMetadataQueryReader* pFrameMetadataQueryReader = nullptr;

		hr = pFrame->GetMetadataQueryReader(&pFrameMetadataQueryReader);
		if (SUCCEEDED(hr))
		{
			hr = pFrameMetadataQueryReader->GetMetadataByName(L"/grctlext/Delay", &propValue);

			if (SUCCEEDED(hr))
			{
				hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
				if (SUCCEEDED(hr))
				{
					// Convert the delay retrieved in 10 ms units to a delay in 1 ms units
					hr = UIntMult(propValue.uiVal, 10, &delay);
				}
				PropVariantClear(&propValue);
			}
			else
				delay = 25;
			RELEASE(pFrameMetadataQueryReader);
		}

		return hr;
	}

	void CalcBmpSize(D2D1_RECT_F& bmprc, const UINT& wid, const UINT& hei)
	{
		//计算目标图片大小
		if (this->ScaleMode == DScaleMode::EqualScale)
		{
			RECT GoodRc = { 0 };
			GoodRc = RectSizeWithConstantRatio({ 0, 0, (long)(DIP2DP(DstSize.width)), (long)(DIP2DP(DstSize.height)) }, { (long)wid, (long)hei });

			bmprc.left = DstRC.rect.left + (float)GoodRc.left;
			bmprc.top = DstRC.rect.top + (float)GoodRc.top;
			bmprc.right = DstRC.rect.left + (float)GoodRc.right;
			bmprc.bottom = DstRC.rect.top + (float)GoodRc.bottom;
		}
		else if (this->ScaleMode == DScaleMode::Fill)
			bmprc = DstRC.rect;

		return;
	}

	RECT RectSizeWithConstantRatio(RECT rcScreen, SIZE sizePicture)
	{
		RECT rect(rcScreen);

		double dWidth = (double)rcScreen.right - (double)rcScreen.left;
		double dHeight = (double)rcScreen.bottom - (double)rcScreen.top;
		double dAspectRatio = dWidth / dHeight;
		double dPictureWidth = sizePicture.cx;
		double dPictureHeight = sizePicture.cy;
		double dPictureAspectRatio = dPictureWidth / dPictureHeight;

		if (dPictureAspectRatio > dAspectRatio)
		{
			int nNewHeight = (int)(dWidth / dPictureWidth * dPictureHeight);
			int nCenteringFactor = ((rcScreen.bottom - rcScreen.top) - nNewHeight) / 2;
			rect.left = 0;
			rect.top = nCenteringFactor;
			rect.right = (int)dWidth;
			rect.bottom = nNewHeight + nCenteringFactor;
		}
		else if (dPictureAspectRatio < dAspectRatio)
		{

			int nNewWidth = (int)(dHeight / dPictureHeight * dPictureWidth);
			int nCenteringFactor = ((rcScreen.right - rcScreen.left) - nNewWidth) / 2;
			rect.left = nCenteringFactor;
			rect.top = 0;
			rect.right = nNewWidth + nCenteringFactor;
			rect.bottom = (int)dHeight;
		}
		return rect;
	}

};




class DSolidColorBox : public DControl
{
public:
	virtual~DSolidColorBox()
	{
		this->Release();
		return;
	}

	bool Init(POINT position, SIZE size, DCOLOR FillColor, DCOLOR StrokeColor = { 0 }, float StrokeWidth = 0.0f, float radiusX = 0.0f, float radiusY = 0.0f, bool CanMoveWindow = false)
	{
		if (this->Inited)
			return false;

		this->CanMoveWindow = CanMoveWindow;

		this->DstRC_DIP.rect.left = (float)position.x;
		this->DstRC_DIP.rect.top = (float)position.y;
		this->DstSize.width = (float)size.cx;
		this->DstSize.height = (float)size.cy;
		this->DstRC_DIP.rect.right = (float)position.x + this->DstSize.width;
		this->DstRC_DIP.rect.bottom = (float)position.y + this->DstSize.height;
		this->DstRC_DIP.radiusX = radiusX;
		this->DstRC_DIP.radiusY = radiusY;
		this->StrokeWidth = StrokeWidth;
		_UpdateDpi();

		DetectColor(FillColor);
		DetectColor(StrokeColor);
		SetFillColor(FillColor);
		SetStrokeColor(StrokeColor);

		this->ControlType = DControlType::SolidColorBox;

		this->Inited = true;
		return true;
	}
	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		A_Col.Reset();
		A_SCol.Reset();
		mFillColor = { 0 };
		mStrokeColor = { 0 };
		ControlType = DControlType::SolidColorBox;

		RELEASE(brush);

		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Radius_X.AddAni(DstRC_DIP.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_Radius_Y.AddAni(DstRC_DIP.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}

		DstRC_DIP.radiusX = RadiusX;
		DstRC_DIP.radiusY = RadiusY;
		_UpdateDpi_DstRCRadius();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();
		this->_SetNeedUpdate();
		return;
	}

	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}

	inline void SetFillColor(DCOLOR col, UINT time = 0, bool Reserved = true)
	{
		DCOLOR c = col;
		DetectColor(c);
		if (Reserved)
		{
			A_Col.AddAni(mFillColor, c, (UINT)(FrameRate * (time / 1000.0f)));
			this->_SetNeedUpdate();
			return;
		}

		mFillColor = DcolToColf(c);
		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}
	inline void SetStrokeColor(DCOLOR col, UINT time = 0, bool Reserved = true)
	{
		DCOLOR c = col;
		DetectColor(c);
		if (Reserved)
		{
			A_SCol.AddAni(mStrokeColor, c, (UINT)(FrameRate * (time / 1000.0f)));
			this->_SetNeedUpdate();
			return;
		}

		mStrokeColor = DcolToColf(c);
		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

protected:
	D2D1_COLOR_F mFillColor = { 0 };//存储以255为最大的颜色
	D2D1_COLOR_F mStrokeColor = { 0 };//存储以255为最大的颜色
	_Ani_Color A_Col;
	_Ani_Color A_SCol;

	ID2D1SolidColorBrush* brush = nullptr;

	//*************************DXUI自用函数*************************

	void _SeparateFromWindow()
	{
		//重新设置动画颜色，防止下一次加入窗口时还没恢复状态
		A_Col.Reset();
		A_SCol.Reset();
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(RoundGeo);
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(HitTestGeo);
		return;
	}

	bool _IsContentChanging()
	{
		if (A_Col || A_SCol || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa)
			return true;
		else
			return false;
	}

	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		if (State != state)
		{
			DControlState LastState = State;
			State = state;

			mSendConMsg(DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);

			if (NeedUnlock)
				___StateLock.Unlock();
			return true;
		}
		if (NeedUnlock)
			___StateLock.Unlock();
		return false;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		DstRC.rect.left = floor(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = floor(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = floor(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = floor(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			UpdateRC.left = floor(DstRC.rect.left - DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floor(DstRC.rect.top - DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceil(DstRC.rect.right + DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceil(DstRC.rect.bottom + DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			UpdateRC = DstRC.rect;
		}
		return;
	}

	bool _SetCursorMsgProc()
	{
		if (State == DControlState::Disable)
			return false;

		SetCursor(this->CursorStyle);
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		___StateLock.Lock();
		if (State == DControlState::Disable || NowControlID == ID)
		{
			___StateLock.Unlock();
			return false;
		}

		if (Msg == WM_MOUSEMOVE && OnClick)
		{
			___StateLock.Unlock();
			return false;
		}
		else if (Msg == WM_LBUTTONUP)
			OnClick = false;

		return m_SetState(DControlState::Normal, true);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		___StateLock.Lock();
		if (State == DControlState::Disable || State == DControlState::Click)
		{
			___StateLock.Unlock();
			return false;
		}

		if (OnClick)
			return m_SetState(DControlState::Click, true);

		return m_SetState(DControlState::MouseMove, true);
	}
	bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		___StateLock.Lock();
		if (State == DControlState::Disable)
		{
			___StateLock.Unlock();
			return false;
		}

		OnClick = true;

		return m_SetState(DControlState::Click, true);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		___StateLock.Lock();
		if (State == DControlState::Disable || !OnClick || (OnClick && !this->HitTest(xPos, yPos)))
		{
			___StateLock.Unlock();
			OnClick = false;
			return false;
		}
		OnClick = false;

		return m_SetState(DControlState::MouseMove, true);
	}

	bool _CreateHitTestGeo()//创建命中测试几何
	{
		___HitTestLock.Lock();
		RELEASE(HitTestGeo);
		if (D2DFactory)
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DIP2DP(DstSize.width + StrokeWidth), DIP2DP(DstSize.height + StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Col)
		{
			A_Col.FrameCalc(mFillColor);
			this->_SetNeedUpdate();
		}
		if (A_SCol)
		{
			A_SCol.FrameCalc(mStrokeColor);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(AID);
				if (!A_Radius_Y)
					resp = true;
			}
			SetRadius(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//处理动画的颜色变换
		D2D1_COLOR_F f_FillColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_StrokeColor;//以1.0f为最大的颜色结构体

		//动画——填充颜色
		f_FillColor = Calc_fColor(mFillColor, TotalOpacity);
		f_StrokeColor = Calc_fColor(mStrokeColor, TotalOpacity);

		//********动画颜色处理结束**********

		if (f_FillColor.a != 0.0f)
		{
			brush->SetColor(f_FillColor);//设置为填充颜色
			DstTarget->FillRoundedRectangle(DstRC, brush);
		}
		if (f_StrokeColor.a != 0.0f)
		{
			brush->SetColor(f_StrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}

};

class DLinearColorBox : public DSolidColorBox
{
public:
	virtual~DLinearColorBox()
	{
		this->Release();
		return;
	}

	bool Init(POINT position, SIZE size, DCOLOR BackColor, DColorBox_Point* Points, UINT PointCount, POINT StartPos, POINT EndPos, DCOLOR StrokeColor = { 0 }, float StrokeWidth = 0.0f, float radiusX = 0.0f, float radiusY = 0.0f, bool CanMoveWindow = false)
	{
		if (this->Inited)
			return false;

		this->CanMoveWindow = CanMoveWindow;

		this->DstRC_DIP.rect.left = (float)position.x;
		this->DstRC_DIP.rect.top = (float)position.y;
		this->DstSize.width = (float)size.cx;
		this->DstSize.height = (float)size.cy;
		this->DstRC_DIP.rect.right = (float)position.x + this->DstSize.width;
		this->DstRC_DIP.rect.bottom = (float)position.y + this->DstSize.height;
		this->DstRC_DIP.radiusX = radiusX;
		this->DstRC_DIP.radiusY = radiusY;
		this->StrokeWidth = StrokeWidth;
		_UpdateDpi();

		DetectColor(BackColor);
		DetectColor(StrokeColor);
		SetFillColor(BackColor);
		SetStrokeColor(StrokeColor);

		if (Points && PointCount)
		{
			this->Points = new D2D1_GRADIENT_STOP[PointCount];
			if (this->Points)
			{
				for (UINT i = 0; i < PointCount; ++i)
				{
					DetectColor(Points[i].color);
					this->Points[i].color = DcolToColf_255(Points[i].color);
					this->Points[i].position = Points[i].position;
				}
				this->PointCount = PointCount;
			}
		}

		this->StartPoint.x = (float)StartPos.x;
		this->StartPoint.y = (float)StartPos.y;
		this->EndPoint.x = (float)EndPos.x;
		this->EndPoint.y = (float)EndPos.y;

		this->ControlType = DControlType::LinearColorBox;

		this->Inited = true;
		return true;
	}

	void SetStartPos(const POINT& StartPos, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0)
	{
		A_Sta_X.AddAni(StartPoint.x, (float)StartPos.x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		A_Sta_Y.AddAni(StartPoint.y, (float)StartPos.y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		this->_SetNeedUpdate();
		return;
	}

	void SetEndPos(const POINT& EndPos, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0)
	{
		A_End_X.AddAni(EndPoint.x, (float)EndPos.x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		A_End_Y.AddAni(EndPoint.y, (float)EndPos.y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		this->_SetNeedUpdate();
		return;
	}

	bool SetLinearPoints(DColorBox_Point* Points, UINT PointCount)
	{
		if (Points && PointCount)
		{
			___PointsLock.Lock();
			DeleteArray(this->Points);

			this->Points = new D2D1_GRADIENT_STOP[PointCount];
			if (this->Points)
			{
				for (UINT i = 0; i < PointCount; ++i)
				{
					this->Points[i].color = DcolToColf_255(Points[i].color);
					this->Points[i].position = Points[i].position;
				}
				this->PointCount = PointCount;
			}
			else
			{
				___PointsLock.Unlock();
				return false;
			}
			___PointsLock.Unlock();
		}
		else
			return false;

		___PCollLock.Lock();
		RELEASE(PColl);
		___PCollLock.Unlock();

		this->_SetNeedUpdate();

		return true;
	}

	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		A_Col.Reset();
		A_SCol.Reset();
		A_Sta_X.Reset();
		A_Sta_Y.Reset();
		A_End_X.Reset();
		A_End_Y.Reset();

		ControlType = DControlType::LinearColorBox;

		mFillColor = { 0 };
		mStrokeColor = { 0 };
		LinearOpa = 1.0f;
		StartPoint = { 0 };
		EndPoint = { 0 };
		PointCount = 0;
		DeleteArray(Points);
		RELEASE(PColl);
		RELEASE(LinearBru);
		RELEASE(brush);

		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Radius_X.AddAni(DstRC_DIP.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_Radius_Y.AddAni(DstRC_DIP.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}

		DstRC_DIP.radiusX = RadiusX;
		DstRC_DIP.radiusY = RadiusY;
		_UpdateDpi_DstRCRadius();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();
		this->_SetNeedUpdate();
		return;
	}

	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();
		//重新创建HitTest
		this->_CreateHitTestGeo();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}

	void SetLinearOpacity(float opa, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Lin_Opa.AddAni(LinearOpa, opa, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		LinearOpa = opa;
		this->_SetNeedUpdate();

		return;
	}
private:
	D2D1_POINT_2F StartPoint = { 0 };
	D2D1_POINT_2F EndPoint = { 0 };

	_AniCalc_f A_Sta_X;
	_AniCalc_f A_Sta_Y;
	_AniCalc_f A_End_X;
	_AniCalc_f A_End_Y;
	_AniCalc_f A_Lin_Opa;

	float LinearOpa = 1.0f;

	D2D1_GRADIENT_STOP* Points = nullptr;
	UINT PointCount = 0;
	___DThreadLock ___PointsLock;

	ID2D1GradientStopCollection* PColl = nullptr;
	___DThreadLock ___PCollLock;

	ID2D1LinearGradientBrush* LinearBru = nullptr;
	___DThreadLock ___LinearBruLock;

	//*************************DXUI自用函数*************************

	void _SeparateFromWindow()
	{
		//重新设置动画颜色，防止下一次加入窗口时还没恢复状态
		A_Col.Reset();
		A_SCol.Reset();
		A_Sta_X.Reset();
		A_Sta_Y.Reset();
		A_End_X.Reset();
		A_End_Y.Reset();
		A_Lin_Opa.Reset();
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(PColl);
		RELEASE(LinearBru);
		RELEASE(RoundGeo);
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(HitTestGeo);
		return;
	}

	bool _IsContentChanging()
	{
		if (A_Col || A_SCol || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa || A_Sta_X || A_Sta_Y || A_End_X || A_End_Y || A_Lin_Opa)
			return true;
		else
			return false;
	}
	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Col)
		{
			A_Col.FrameCalc(mFillColor);
			this->_SetNeedUpdate();
		}
		if (A_SCol)
		{
			A_SCol.FrameCalc(mStrokeColor);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(AID);
				if (!A_Radius_Y)
					resp = true;
			}
			SetRadius(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}

		if (A_Sta_X || A_Sta_Y)
		{
			UINT AID = 0;
			if (A_Sta_X)
				StartPoint.x = A_Sta_X.FrameCalc(AID);
			if (A_Sta_Y)
				StartPoint.y = A_Sta_Y.FrameCalc(AID);

			___LinearBruLock.Lock();
			if (LinearBru)
				LinearBru->SetStartPoint({ DIP2DP(StartPoint.x), DIP2DP(StartPoint.y) });
			___LinearBruLock.Unlock();

			this->_SetNeedUpdate();
			EndAniSend(AID);
		}
		if (A_End_X || A_End_Y)
		{
			UINT AID = 0;
			if (A_End_X)
				EndPoint.x = A_End_X.FrameCalc(AID);
			if (A_End_Y)
				EndPoint.y = A_End_Y.FrameCalc(AID);

			___LinearBruLock.Lock();
			if (LinearBru)
				LinearBru->SetEndPoint({ DIP2DP(EndPoint.x), DIP2DP(EndPoint.y) });
			___LinearBruLock.Unlock();

			this->_SetNeedUpdate();
			EndAniSend(AID);
		}
		if (A_Lin_Opa)
		{
			UINT AID = 0;
			SetLinearOpacity(A_Lin_Opa.FrameCalc(AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//处理动画的颜色变换
		D2D1_COLOR_F f_FillColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_StrokeColor;//以1.0f为最大的颜色结构体

		//动画——填充颜色
		f_FillColor = Calc_fColor(mFillColor, TotalOpacity);
		f_StrokeColor = Calc_fColor(mStrokeColor, TotalOpacity);

		//********动画颜色处理结束**********

		___PCollLock.Lock();
		___PointsLock.Lock();
		if (!PColl && Points && PointCount)
		{
			DstTarget->CreateGradientStopCollection(Points, PointCount, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &PColl);

			___LinearBruLock.Lock();
			RELEASE(LinearBru);
			___LinearBruLock.Unlock();
		}
		___PointsLock.Unlock();

		___LinearBruLock.Lock();
		if (!LinearBru && PColl)
		{
			DstTarget->CreateLinearGradientBrush({ { DIP2DP(StartPoint.x), DIP2DP(StartPoint.y) }, { DIP2DP(EndPoint.x), DIP2DP(EndPoint.y) } }, PColl, &LinearBru);
		}
		___PCollLock.Unlock();
		
		//如果有圆角，则用图层实现圆角
		if (DstRC.radiusX != 0.0f || DstRC.radiusY != 0.0f)
		{
			___RoundGeoLock.Lock();

			if (!RoundGeo && D2DFactory)
				D2DFactory->CreateRoundedRectangleGeometry(DstRC, &RoundGeo);

			if (RoundGeo)
				Clip.PushLayer(DstTarget, UpdateRC, RoundGeo, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

			___RoundGeoLock.Unlock();
		}

		//开始绘制
		if (f_FillColor.a != 0.0f)
		{
			brush->SetColor(f_FillColor);//设置为填充颜色
			DstTarget->FillRectangle(DstRC.rect, brush);
		}

		if (LinearBru)
		{
			LinearBru->SetOpacity(TotalOpacity * LinearOpa);//设置总不透明度
			DstTarget->FillRectangle(DstRC.rect, LinearBru);
		}

		if (f_StrokeColor.a != 0.0f)
		{
			brush->SetColor(f_StrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		___LinearBruLock.Unlock();

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}

};




class DRadialColorBox : public DSolidColorBox
{
public:
	virtual~DRadialColorBox()
	{
		this->Release();
		return;
	}

	bool Init(POINT position, SIZE size, DCOLOR BackColor, DColorBox_Point* Points, UINT PointCount, POINT RadialCenter, float RadialRadiusX, float RadialRadiusY, DCOLOR StrokeColor = { 0 }, float StrokeWidth = 0.0f, float RoundRadiusX = 0.0f, float RoundRadiusY = 0.0f, bool CanMoveWindow = false)
	{
		if (this->Inited)
			return false;

		this->CanMoveWindow = CanMoveWindow;

		this->DstRC_DIP.rect.left = (float)position.x;
		this->DstRC_DIP.rect.top = (float)position.y;
		this->DstSize.width = (float)size.cx;
		this->DstSize.height = (float)size.cy;
		this->DstRC_DIP.rect.right = (float)position.x + this->DstSize.width;
		this->DstRC_DIP.rect.bottom = (float)position.y + this->DstSize.height;
		this->DstRC_DIP.radiusX = RoundRadiusX;
		this->DstRC_DIP.radiusY = RoundRadiusY;
		this->StrokeWidth = StrokeWidth;
		_UpdateDpi();

		DetectColor(BackColor);
		DetectColor(StrokeColor);
		SetFillColor(BackColor);
		SetStrokeColor(StrokeColor);

		if (Points && PointCount)
		{
			this->Points = new D2D1_GRADIENT_STOP[PointCount];
			if (this->Points)
			{
				for (UINT i = 0; i < PointCount; ++i)
				{
					DetectColor(Points[i].color);
					this->Points[i].color = DcolToColf_255(Points[i].color);
					this->Points[i].position = Points[i].position;
				}
				this->PointCount = PointCount;
			}
		}

		this->Center.x = (float)RadialCenter.x;
		this->Center.y = (float)RadialCenter.y;
		this->RRadiusX = RadialRadiusX;
		this->RRadiusY = RadialRadiusY;

		this->ControlType = DControlType::RadialColorBox;

		this->Inited = true;
		return true;
	}

	void SetRadialCenter(const POINT& center, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0)
	{
		A_Cen_X.AddAni(Center.x, (float)center.x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		A_Cen_Y.AddAni(Center.y, (float)center.y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		this->_SetNeedUpdate();
		return;
	}

	void SetRadialRadius(float RadialRadiusX, float RadialRadiusY, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0)
	{
		A_RRad_X.AddAni(RRadiusX, RadialRadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		A_RRad_Y.AddAni(RRadiusY, RadialRadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		this->_SetNeedUpdate();
		return;
	}

	bool SetRadialPoints(DColorBox_Point* Points, UINT PointCount)
	{
		if (Points && PointCount)
		{
			___PointsLock.Lock();
			DeleteArray(this->Points);

			this->Points = new D2D1_GRADIENT_STOP[PointCount];
			if (this->Points)
			{
				for (UINT i = 0; i < PointCount; ++i)
				{
					this->Points[i].color = DcolToColf_255(Points[i].color);
					this->Points[i].position = Points[i].position;
				}
				this->PointCount = PointCount;
			}
			else
			{
				___PointsLock.Unlock();
				return false;
			}
			___PointsLock.Unlock();
		}
		else
			return false;

		___PCollLock.Lock();
		RELEASE(PColl);
		___PCollLock.Unlock();

		this->_SetNeedUpdate();

		return true;
	}

	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		A_Col.Reset();
		A_SCol.Reset();
		A_Cen_X.Reset();
		A_Cen_Y.Reset();
		A_RRad_X.Reset();
		A_RRad_Y.Reset();

		ControlType = DControlType::RadialColorBox;

		mFillColor = { 0 };
		mStrokeColor = { 0 };
		RadialOpa = 1.0f;
		Center = { 0 };
		RRadiusX = 0.0f;
		RRadiusY = 0.0f;
		PointCount = 0;
		DeleteArray(Points);
		RELEASE(PColl);
		RELEASE(RadialBru);
		RELEASE(brush);

		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_Radius_X.AddAni(DstRC_DIP.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_Radius_Y.AddAni(DstRC_DIP.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}

		DstRC_DIP.radiusX = RadiusX;
		DstRC_DIP.radiusY = RadiusY;
		_UpdateDpi_DstRCRadius();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestGeo();
		this->_SetNeedUpdate();
		return;
	}

	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();
		//重新创建HitTest
		this->_CreateHitTestGeo();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}

	void SetRadialOpacity(float opa, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RRad_Opa.AddAni(RadialOpa, opa, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		RadialOpa = opa;
		this->_SetNeedUpdate();
		return;
	}

private:
	D2D1_POINT_2F Center = { 0 };
	float RRadiusX = 0.0f;
	float RRadiusY = 0.0f;
	float RadialOpa = 1.0f;

	_AniCalc_f A_Cen_X;
	_AniCalc_f A_Cen_Y;
	_AniCalc_f A_RRad_X;
	_AniCalc_f A_RRad_Y;
	_AniCalc_f A_RRad_Opa;

	D2D1_GRADIENT_STOP* Points = nullptr;
	UINT PointCount = 0;
	___DThreadLock ___PointsLock;

	ID2D1GradientStopCollection* PColl = nullptr;
	___DThreadLock ___PCollLock;

	ID2D1RadialGradientBrush* RadialBru = nullptr;
	___DThreadLock ___RadialBruLock;
	//*************************DXUI自用函数*************************

	void _SeparateFromWindow()
	{
		//重新设置动画颜色，防止下一次加入窗口时还没恢复状态
		A_Col.Reset();
		A_SCol.Reset();
		A_Cen_X.Reset();
		A_Cen_Y.Reset();
		A_RRad_X.Reset();
		A_RRad_Y.Reset();
		A_RRad_Opa.Reset();
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(PColl);
		RELEASE(RadialBru);
		RELEASE(RoundGeo);
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(HitTestGeo);
		return;
	}

	bool _IsContentChanging()
	{
		if (A_Col || A_SCol || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa || A_Cen_X || A_Cen_Y || A_RRad_X || A_RRad_Y || A_RRad_Opa)
			return true;
		else
			return false;
	}
	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Col)
		{
			A_Col.FrameCalc(mFillColor);
			this->_SetNeedUpdate();
		}
		if (A_SCol)
		{
			A_SCol.FrameCalc(mStrokeColor);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(AID);
				if (!A_Radius_Y)
					resp = true;
			}
			SetRadius(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		if (A_Cen_X || A_Cen_Y)
		{
			UINT AID = 0;
			if (A_Cen_X)
				Center.x = A_Cen_X.FrameCalc(AID);
			if (A_Cen_Y)
				Center.y = A_Cen_Y.FrameCalc(AID);

			___RadialBruLock.Lock();
			if (RadialBru)
				RadialBru->SetCenter({ DIP2DP(Center.x), DIP2DP(Center.y) });
			___RadialBruLock.Unlock();

			this->_SetNeedUpdate();
			EndAniSend(AID);
		}
		if (A_RRad_X || A_RRad_Y)
		{
			UINT AID = 0;
			if (A_RRad_X)
				RRadiusX = A_RRad_X.FrameCalc(AID);
			if (A_RRad_Y)
				RRadiusY = A_RRad_Y.FrameCalc(AID);

			___RadialBruLock.Lock();
			if (RadialBru)
			{
				RadialBru->SetRadiusX(DIP2DP(RRadiusX));
				RadialBru->SetRadiusY(DIP2DP(RRadiusY));
			}
			___RadialBruLock.Unlock();

			this->_SetNeedUpdate();
			EndAniSend(AID);
		}
		if (A_RRad_Opa)
		{
			UINT AID = 0;
			SetRadialOpacity(A_RRad_Opa.FrameCalc(AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//处理动画的颜色变换
		D2D1_COLOR_F f_FillColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_StrokeColor;//以1.0f为最大的颜色结构体

		//动画——填充颜色
		f_FillColor = Calc_fColor(mFillColor, TotalOpacity);
		f_StrokeColor = Calc_fColor(mStrokeColor, TotalOpacity);

		//********动画颜色处理结束**********

		___PCollLock.Lock();
		___PointsLock.Lock();
		if (!PColl && Points && PointCount)
		{
			DstTarget->CreateGradientStopCollection(Points, PointCount, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &PColl);

			___RadialBruLock.Lock();
			RELEASE(RadialBru);
			___RadialBruLock.Unlock();
		}
		___PointsLock.Unlock();

		___RadialBruLock.Lock();
		if (!RadialBru && PColl)
		{
			DstTarget->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties({ DIP2DP(Center.x), DIP2DP(Center.y) }, { 0, 0 }, DIP2DP(RRadiusX), DIP2DP(RRadiusY)), PColl, &RadialBru);
		}
		___PCollLock.Unlock();

		//如果有圆角，则用图层实现圆角
		if (DstRC.radiusX != 0.0f || DstRC.radiusY != 0.0f)
		{
			___RoundGeoLock.Lock();

			if (!RoundGeo && D2DFactory)
				D2DFactory->CreateRoundedRectangleGeometry(DstRC, &RoundGeo);

			if (RoundGeo)
				Clip.PushLayer(DstTarget, UpdateRC, RoundGeo, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

			___RoundGeoLock.Unlock();
		}

		//开始绘制
		if (f_FillColor.a != 0.0f)
		{
			brush->SetColor(f_FillColor);//设置为填充颜色
			DstTarget->FillRectangle(DstRC.rect, brush);
		}

		if (RadialBru)
		{
			RadialBru->SetOpacity(TotalOpacity * RadialOpa);//设置总不透明度
			DstTarget->FillRectangle(DstRC.rect, RadialBru);
		}

		if (f_StrokeColor.a != 0.0f)
		{
			brush->SetColor(f_StrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		___RadialBruLock.Unlock();

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}

};



class DScrollView : public DControl
{
public:
	virtual~DScrollView()
	{
		this->Release();
		return;
	}

	void Init(POINT position, SIZE size, float FullHeight, float ScrollDelta = 50.0f, UINT ScrollTime = 200, bool SmoothScroll = true, bool CanMoveWindow = false)
	{
		if (this->Inited)
			return;

		this->CanMoveWindow = CanMoveWindow;

		this->DstRC_DIP.rect.left = (float)position.x;
		this->DstRC_DIP.rect.top = (float)position.y;
		this->DstSize.width = (float)size.cx;
		this->DstSize.height = (float)size.cy;
		this->DstRC_DIP.rect.right = (float)position.x + this->DstSize.width;
		this->DstRC_DIP.rect.bottom = (float)position.y + this->DstSize.height;
		this->DstRC_DIP.radiusX = 0.0f;
		this->DstRC_DIP.radiusY = 0.0f;
		_UpdateDpi();

		this->FullHeight = FullHeight;
		this->ScrDelta = max(0.0f, ScrollDelta);
		this->ScrTime = ScrollTime;
		this->SmoothScr = SmoothScroll;

		this->ControlType = DControlType::ScrollView;

		this->Inited = true;
		return;
	}


	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Scr.Reset();

		ControlType = DControlType::ScrollView;
		RELEASE(brush);
		FullHeight = 0.0f;
		ScrOffset = 0.0f;
		ScrDelta = 0.0f;
		ScrTime = 0;
		SmoothScr = true;

		return;
	}

	void SetFullHeight(float fullheight)
	{
		FullHeight = fullheight;

		float Scr, src;
		if (A_Scr)
			Scr = A_Scr.GetEnd();
		else
			Scr = ScrOffset;
		src = ScrOffset;

		if (FullHeight - Scr < DstRC_DIP.rect.bottom - DstRC_DIP.rect.top)//太靠上时的处理
		{
			Scr = FullHeight - DstRC_DIP.rect.bottom + DstRC_DIP.rect.top;
			if (Scr < 0.0f)
				Scr = 0.0f;

			if (src != Scr)
			{
				DBezier be = SmoothScrollBezier;
				A_Scr.AddAni(src, Scr, SmoothScr ? ((UINT)(FrameRate * (ScrTime / 1000.0f))) : 0, &be, 1, false);
				this->_SetNeedUpdate();
			}
		}
		return;
	}

	float GetFullHeight()
	{
		return FullHeight;
	}
	///<summary>
	///Offset为大于等于0的小数(即DIP像素)，表示向上偏移的 设备无关像素 数量
	///</summary>
	void SetScrollOffset(float Offset, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		float Scr = Offset, src = ScrOffset;

		if (FullHeight - Scr < DstRC_DIP.rect.bottom - DstRC_DIP.rect.top)//太靠上时的处理
			Scr = FullHeight - DstRC_DIP.rect.bottom + DstRC_DIP.rect.top;

		if (Scr < 0.0f)
			Scr = 0.0f;

		if (src != Scr)
		{
			A_Scr.AddAni(src, Scr, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, 1, false);
			this->_SetNeedUpdate(_GetUpdateRC());
		}
		return;
	}

	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_x.AddAni(DstRC_DIP.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_y.AddAni(DstRC_DIP.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		//要先处理right和bottom
		DstRC_DIP.rect.right = x + DstSize.width;
		DstRC_DIP.rect.bottom = y + DstSize.height;
		DstRC_DIP.rect.left = x;
		DstRC_DIP.rect.top = y;

		_UpdateDpi();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			A_RC_width.AddAni(DstSize.width, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_RC_height.AddAni(DstSize.height, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			if (Parent)
				Parent->_SetNeedUpdate(_GetUpdateRC(), false);
			else
				this->_SetNeedUpdate();
			return;
		}
		D2D1_RECT_F OldUpdateRC = this->_GetUpdateRC();

		DstSize.width = width;
		DstSize.height = height;
		DstRC_DIP.rect.right = DstRC_DIP.rect.left + DstSize.width;
		DstRC_DIP.rect.bottom = DstRC_DIP.rect.top + DstSize.height;

		_UpdateDpi();

		//重新创建HitTest
		this->_CreateHitTestGeo();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(this->_GetUpdateRC(), OldUpdateRC);
			RectChanged = true;
		}
		if (Parent)
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged ? LastRect : this->_GetUpdateRC(), false);
		}
		else
			this->_SetNeedUpdate();
		___RcChangedLock.Unlock();
		return;
	}

private:
	ID2D1SolidColorBrush* brush = nullptr;

	float FullHeight = 0.0f;//设备无关像素
	float ScrOffset = 0.0f;//设备无关像素
	float ScrDelta = 0.0f;//滚动一次像素偏移值，设备无关像素
	UINT ScrTime = 0;

	bool SmoothScr = true;
	_AniCalc_f A_Scr;//设备独立像素

	void _SeparateFromWindow()
	{
		ScrOffset = 0.0f;

		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Scr.Reset();
		this->_ClearChildCon();
		OnClick = false;//important
		OwnerTransparent = false;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		PosOffset.clear();
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}
	void ReleaseDeviceDependentRes()
	{
		RELEASE(BlurEffect);
		RELEASE(brush);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(HitTestGeo);
		return;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		DstRC.rect.left = floor(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = floor(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = floor(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = floor(DIP2DP(DstRC_DIP.rect.bottom));
		UpdateRC = DstRC.rect;
		return;
	}

	//要重载基类的AddChildCon，并加上自己的ScrOffset
	void _AddChildCon(DControl* const& control)
	{
		___ChildLock.Lock();

		___PosOfsLock.Lock();
		control->_SetParPosOffPointer(ID, PosOffset, &ScrOffset);
		___PosOfsLock.Unlock();

		Child.insert(std::pair<UINT, DControl*>(control->GetID(), control));
		___ChildLock.Unlock();

		return;
	}

	bool _SetCursorMsgProc()
	{
		SetCursor(this->CursorStyle);
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		if (NowControlID == ID)
			return false;

		if (Msg == WM_MOUSEMOVE && OnClick)
			return false;
		else if (Msg == WM_LBUTTONUP)
			OnClick = false;

		return m_SetState(DControlState::Normal);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		___StateLock.Lock();
		if (State == DControlState::Click)
		{
			___StateLock.Unlock();
			return false;
		}

		if (OnClick)
			return m_SetState(DControlState::Click, true);

		return m_SetState(DControlState::MouseMove, true);
	}
	bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos)
	{
		OnClick = true;

		return m_SetState(DControlState::Click);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		if (!OnClick || (OnClick && !this->HitTest(xPos, yPos)))
		{
			OnClick = false;
			return false;
		}
		OnClick = false;

		return m_SetState(DControlState::MouseMove);
	}

	bool _MouseWheelMsgProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		short Delta = GET_WHEEL_DELTA_WPARAM(wParam);

		float Scr, src;
		if (A_Scr)
			Scr = A_Scr.GetEnd();
		else
			Scr = ScrOffset;
		src = ScrOffset;

		if (Delta > 0)//手指向前滑动时
		{
			if (Scr > 0.0f)
			{
				Scr -= DIP2DP(ScrDelta);

				if (Scr < 0.0f)
					Scr = 0.0f;

				if (src != Scr)
				{
					DBezier be = SmoothScrollBezier;
					A_Scr.AddAni(src, Scr, SmoothScr ? ((UINT)(FrameRate * (ScrTime / 1000.0f))) : 0, &be, 1, false);
					this->_SetNeedUpdate(_GetUpdateRC());
				}
				else return false;
			}
			else return false;
		}
		else if (Delta < 0)//手指向后滑动时
		{
			if (FullHeight - Scr > DstRC_DIP.rect.bottom - DstRC_DIP.rect.top)
			{
				Scr += DIP2DP(ScrDelta);

				if (FullHeight - Scr < DstRC_DIP.rect.bottom - DstRC_DIP.rect.top)//太靠上时的处理
					Scr = FullHeight - DstRC_DIP.rect.bottom + DstRC_DIP.rect.top;

				if (Scr < 0.0f)
					Scr = 0.0f;

				if (src != Scr)
				{
					DBezier be = SmoothScrollBezier;
					A_Scr.AddAni(src, Scr, SmoothScr ? ((UINT)(FrameRate * (ScrTime / 1000.0f))) : 0, &be, 1, false);
					this->_SetNeedUpdate(_GetUpdateRC());
				}
				else return false;
			}
			else return false;
		}

		//通知给用户
		mSendConMsg(DControlMsg::ScrollView_Scrolling, (WPARAM)lParam, (LPARAM)wParam);
		return true;
	}

	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		if (State != state && state != DControlState::Disable)
		{
			DControlState LastState = State;
			State = state;

			mSendConMsg(DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);

			if (NeedUnlock)
				___StateLock.Unlock();
			return true;
		}
		if (NeedUnlock)
			___StateLock.Unlock();
		return false;
	}

	bool _CreateHitTestGeo()//创建命中测试几何
	{
		___HitTestLock.Lock();
		RELEASE(HitTestGeo);
		if (D2DFactory)
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DIP2DP(DstSize.width), DIP2DP(DstSize.height) }, 0.0f, 0.0f }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	bool _IsContentChanging()
	{
		if (A_Scr || A_RC_x || A_RC_y || A_RC_width || A_RC_height)
			return true;
		else
			return false;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();

		if (A_Scr)
		{
			UINT AID = 0;
			ScrOffset = A_Scr.FrameCalc(AID);
			ResponseCur();//这个滚动要随时响应鼠标
			this->_SetNeedUpdate();
			//如果动画结束，发送消息
			if (AID)
				mSendConMsg(DControlMsg::ScrollView_EndOfScroll, 0, 0);
		}
		return;
	}

	bool _Draw(ID2D1DeviceContext*& DstTarget, const D2D1_COLOR_F& BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//此处绘制滚动条

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}

};




class __DWinShadow
{
public:
	operator bool()
	{
		return (bool)hWnd;
	}

	~__DWinShadow()
	{
		this->Release();
		return;
	}

	bool Create(HINSTANCE hInst, HWND MainHwnd, std::wstring ClassName, float Radius, float Alpha, int Dpi)
	{
		if (!hInst || !MainHwnd)
			return false;
		//创建窗口
		___DrawLock.Lock();
		this->Dpi = Dpi;
		radius = Radius;
		alpha = Alpha;
		EdgeOut = (UINT)(DIP2DP(radius) * 6.0f);
		GetWindowRect(MainHwnd, &MainRC);

		hWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT, ClassName.c_str(), nullptr, WS_POPUP,
			MainRC.left - EdgeOut, MainRC.top - EdgeOut,
			MainRC.right - MainRC.left + EdgeOut * 2, MainRC.bottom - MainRC.top + EdgeOut * 2, MainHwnd, nullptr, hInst, nullptr);
		if (!hWnd)
			return false;

		if (alpha != 0.0f)
			this->ReDrawMem(alpha);
		___DrawLock.Unlock();

		return true;
	}

	void SetDpi(int Dpi)
	{
		this->Dpi = Dpi;
		EdgeOut = (UINT)(DIP2DP(radius) * 6.0f);
		return;
	}

	bool ReDrawMem(float ReDrawAlpha)
	{
		___DrawLock.Lock();
		RECT rc;
		GetClientRect(hWnd, &rc);

		//创建资源
		if (!MemDC)
			MemDC = CreateCompatibleDC(nullptr);
		if (MemDC)
		{
			if (MemBmp)
			{
				SelectObject(MemDC, MemOldBmp);
				DeleteObject(MemBmp);
			}

			BITMAPINFO bitmapinfo;
			bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapinfo.bmiHeader.biBitCount = 32;
			bitmapinfo.bmiHeader.biHeight = rc.bottom;
			bitmapinfo.bmiHeader.biWidth = rc.right;
			bitmapinfo.bmiHeader.biPlanes = 1;
			bitmapinfo.bmiHeader.biCompression = BI_RGB;
			bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
			bitmapinfo.bmiHeader.biYPelsPerMeter = 0;
			bitmapinfo.bmiHeader.biClrUsed = 0;
			bitmapinfo.bmiHeader.biClrImportant = 0;
			bitmapinfo.bmiHeader.biSizeImage = bitmapinfo.bmiHeader.biWidth * bitmapinfo.bmiHeader.biHeight * 4;

			MemBmp = CreateDIBSection(MemDC, &bitmapinfo, 0, nullptr, nullptr, 0);
			if (MemBmp)
			{
				MemOldBmp = (HBITMAP)SelectObject(MemDC, MemBmp);

				HDC dc = CreateCompatibleDC(MemDC);
				HPEN Pen = CreatePen(PS_SOLID, (int)ceil(Shadow_StrokeWidth), RGB(0, 0, 0));
				HPEN OldPen = (HPEN)SelectObject(dc, Pen);
				HBRUSH WhiteBru = CreateSolidBrush(RGB(255, 255, 255));
				HBRUSH OldBru = (HBRUSH)SelectObject(dc, WhiteBru);

				UINT leftWid = min(EdgeOut * 2 + (UINT)Shadow_StrokeWidth, EdgeOut + (MainRC.right - MainRC.left) / 2);
				UINT leftMainWid = leftWid - EdgeOut;

				BITMAPINFO lbi;
				lbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				lbi.bmiHeader.biBitCount = 32;
				lbi.bmiHeader.biHeight = rc.bottom;
				lbi.bmiHeader.biWidth = leftWid;
				lbi.bmiHeader.biPlanes = 1;
				lbi.bmiHeader.biCompression = BI_RGB;
				lbi.bmiHeader.biXPelsPerMeter = 0;
				lbi.bmiHeader.biYPelsPerMeter = 0;
				lbi.bmiHeader.biClrUsed = 0;
				lbi.bmiHeader.biClrImportant = 0;
				lbi.bmiHeader.biSizeImage = lbi.bmiHeader.biWidth * lbi.bmiHeader.biHeight * 4;
				HBITMAP Left = CreateDIBSection(dc, &lbi, 0, nullptr, nullptr, 0);
				HBITMAP OldBmp = (HBITMAP)SelectObject(dc, Left);

				RECT rect = { 0, 0, (long)leftWid, rc.bottom };
				FillRect(dc, &rect, WhiteBru);

				Rectangle(dc, EdgeOut + (int)ceil(Shadow_StrokeWidth / 2.0f), EdgeOut + (int)ceil(Shadow_StrokeWidth / 2.0f), leftWid + (int)floor(Shadow_StrokeWidth * 2.0f), rc.bottom - EdgeOut - (int)floor(Shadow_StrokeWidth / 2.0f));

				//开始获取Left的图像数据
				BYTE* leftBits = new BYTE[lbi.bmiHeader.biSizeImage];
				ZeroMemory(leftBits, lbi.bmiHeader.biSizeImage);
				if (GetDIBits(dc, Left, 0, lbi.bmiHeader.biHeight, leftBits, &lbi, DIB_RGB_COLORS) == 0)
					DeleteArray(leftBits);
				//Left数据获取完毕

				//画Top图像
				int topWid = (MainRC.right - MainRC.left) - leftMainWid * 2;

				BITMAPINFO tbi;
				tbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				tbi.bmiHeader.biBitCount = 32;
				tbi.bmiHeader.biHeight = EdgeOut + (int)ceil(Shadow_StrokeWidth * 2.0f);
				tbi.bmiHeader.biWidth = topWid;
				tbi.bmiHeader.biPlanes = 1;
				tbi.bmiHeader.biCompression = BI_RGB;
				tbi.bmiHeader.biXPelsPerMeter = 0;
				tbi.bmiHeader.biYPelsPerMeter = 0;
				tbi.bmiHeader.biClrUsed = 0;
				tbi.bmiHeader.biClrImportant = 0;
				tbi.bmiHeader.biSizeImage = tbi.bmiHeader.biWidth * tbi.bmiHeader.biHeight * 4;

				HBITMAP Top = CreateDIBSection(dc, &tbi, 0, nullptr, nullptr, 0);
				BYTE* topBits = nullptr;
				if (Top)
				{
					SelectObject(dc, Top);

					rect = { 0, 0, topWid, tbi.bmiHeader.biHeight };
					FillRect(dc, &rect, WhiteBru);

					MoveToEx(dc, 0, EdgeOut + (int)ceil(Shadow_StrokeWidth / 2.0f), nullptr);
					LineTo(dc, topWid, EdgeOut + (int)ceil(Shadow_StrokeWidth / 2.0f));

					//开始获取Top图像数据
					topBits = new BYTE[tbi.bmiHeader.biSizeImage];
					ZeroMemory(topBits, tbi.bmiHeader.biSizeImage);
					if (!GetDIBits(dc, Top, 0, tbi.bmiHeader.biHeight, topBits, &tbi, DIB_RGB_COLORS))
						DeleteArray(topBits);

					SelectObject(dc, OldBmp);
				}
				//Top图像数据获取完毕
				SelectObject(dc, OldPen);
				SelectObject(dc, OldBru);

				DeleteObject(Pen);
				DeleteObject(WhiteBru);

				//处理两个图像源数据数据
				for (DWORD i = 0; i < lbi.bmiHeader.biSizeImage - 1; i += 4)
				{
					if (leftBits[i] == 255 && leftBits[i + 1] == 255 && leftBits[i + 2] == 255)
					{
						leftBits[i] = 0;
						leftBits[i + 1] = 0;
						leftBits[i + 2] = 0;
						leftBits[i + 3] = 0;
					}
					else if (leftBits[i] == 0 && leftBits[i + 1] == 0 && leftBits[i + 2] == 0)
					{
						leftBits[i + 3] = 255;
					}
				}
				if (topBits)
				{
					for (DWORD i = 0; i < tbi.bmiHeader.biSizeImage - 1; i += 4)
					{
						if (topBits[i] == 255 && topBits[i + 1] == 255 && topBits[i + 2] == 255)
						{
							topBits[i] = 0;
							topBits[i + 1] = 0;
							topBits[i + 2] = 0;
							topBits[i + 3] = 0;
						}
						else if (topBits[i] == 0 && topBits[i + 1] == 0 && topBits[i + 2] == 0)
						{
							topBits[i + 3] = 255;
						}
					}
				}
				//数据处理完毕

				//开始多线程高斯模糊
				_Gau_Blur(leftBits, leftWid, rc.bottom, leftWid * 4, DIP2DP(radius), 2);
				if (topBits)
					_Gau_Blur(topBits, topWid, tbi.bmiHeader.biHeight, topWid * 4, DIP2DP(radius), 2);

				//再把数据读入HBITMAP
				SetDIBits(dc, Left, 0, lbi.bmiHeader.biHeight, leftBits, &lbi, DIB_RGB_COLORS);
				if (Top && topBits)
					SetDIBits(dc, Top, 0, tbi.bmiHeader.biHeight, topBits, &tbi, DIB_RGB_COLORS);

				//把两部分阴影画到主MemDC上
				//将MemDC颜色和透明度清零
				BYTE* memBits = new BYTE[bitmapinfo.bmiHeader.biSizeImage];
				if (memBits)
				{
					ZeroMemory(memBits, bitmapinfo.bmiHeader.biSizeImage);

					SetDIBits(MemDC, MemBmp, 0, bitmapinfo.bmiHeader.biHeight, memBits, &bitmapinfo, DIB_RGB_COLORS);

					BLENDFUNCTION blendFunc;
					blendFunc.AlphaFormat = AC_SRC_ALPHA;
					blendFunc.BlendFlags = 0;
					blendFunc.BlendOp = AC_SRC_OVER;
					blendFunc.SourceConstantAlpha = (BYTE)(255 * ReDrawAlpha);

					if (Top)
					{
						OldBmp = (HBITMAP)SelectObject(dc, Top);
						//Top绘制
						AlphaBlend(MemDC, leftWid, 0, topWid, tbi.bmiHeader.biHeight, dc, 0, 0, topWid, tbi.bmiHeader.biHeight, blendFunc);
						//垂直翻转
						tbi.bmiHeader.biHeight = -tbi.bmiHeader.biHeight;
						SetDIBits(dc, Top, 0, -tbi.bmiHeader.biHeight, topBits, &tbi, DIB_RGB_COLORS);
						//Bottom绘制
						AlphaBlend(MemDC, leftWid, rc.bottom + tbi.bmiHeader.biHeight, topWid, -tbi.bmiHeader.biHeight, dc, 0, 0, topWid, -tbi.bmiHeader.biHeight, blendFunc);
						SelectObject(dc, OldBmp);
					}
					//Left绘制
					SelectObject(dc, Left);
					AlphaBlend(MemDC, 0, 0, leftWid, rc.bottom, dc, 0, 0, leftWid, rc.bottom, blendFunc);
					//水平翻转
					BYTE* rightBits = new BYTE[lbi.bmiHeader.biSizeImage];
					if (rightBits)
					{
						ZeroMemory(rightBits, lbi.bmiHeader.biSizeImage);

						DWORD nPos, nPos1;
						for (UINT Line = 0; Line < (UINT)lbi.bmiHeader.biHeight; ++Line)
						{
							for (UINT x = 0; x < leftWid; ++x)
							{
								nPos = Line * leftWid * 4 + x * 4;
								nPos1 = (Line + 1) * leftWid * 4 - x * 4 - 1;

								if (nPos + 3 < lbi.bmiHeader.biSizeImage && nPos1 - 3 < lbi.bmiHeader.biSizeImage)
								{
									rightBits[nPos] = leftBits[nPos1 - 3];
									rightBits[nPos + 1] = leftBits[nPos1 - 2];
									rightBits[nPos + 2] = leftBits[nPos1 - 1];
									rightBits[nPos + 3] = leftBits[nPos1];
								}
							}
						}

						SetDIBits(dc, Left, 0, lbi.bmiHeader.biHeight, rightBits, &lbi, DIB_RGB_COLORS);
						//Right绘制
						AlphaBlend(MemDC, leftWid + topWid, 0, leftWid, rc.bottom, dc, 0, 0, leftWid, rc.bottom, blendFunc);

						//将主窗口的区域挖空
						ZeroMemory(memBits, bitmapinfo.bmiHeader.biSizeImage);

						if (GetDIBits(MemDC, MemBmp, 0, bitmapinfo.bmiHeader.biHeight, memBits, &bitmapinfo, DIB_RGB_COLORS))
						{
							UINT MainWid = MainRC.right - MainRC.left, MainHei = MainRC.bottom - MainRC.top;
							for (UINT Line = EdgeOut; Line < MainHei + EdgeOut; ++Line)
							{
								for (UINT x = EdgeOut; x < EdgeOut + MainWid; ++x)
								{
									nPos = Line * bitmapinfo.bmiHeader.biWidth * 4 + x * 4;
									memBits[nPos] = 0;
									memBits[nPos + 1] = 0;
									memBits[nPos + 2] = 0;
									memBits[nPos + 3] = 0;
								}
							}

							SetDIBits(MemDC, MemBmp, 0, bitmapinfo.bmiHeader.biHeight, memBits, &bitmapinfo, DIB_RGB_COLORS);
						}
						DeleteArray(rightBits);
					}//rightBits
				}//memBits
				//释放资源
				DeleteArray(leftBits);
				DeleteArray(topBits);
				DeleteArray(memBits);
				SelectObject(dc, OldBmp);
				DeleteObject(Left);
				DeleteObject(Top);
				DeleteDC(dc);
			}
		}
		___DrawLock.Unlock();
		return true;
	}
	bool Update()
	{
		___DrawLock.Lock();
		if (MemDC && MemBmp)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			BLENDFUNCTION blendFunc;
			blendFunc.AlphaFormat = AC_SRC_ALPHA;
			blendFunc.BlendFlags = 0;
			blendFunc.BlendOp = AC_SRC_OVER;
			if (A_Sha_Opa)
				blendFunc.SourceConstantAlpha = (BYTE)(255 * alpha);
			else
				blendFunc.SourceConstantAlpha = 255;

			SIZE sz = { rc.right, rc.bottom };
			POINT DestPt = { 0, 0 };
			UpdateLayeredWindow(hWnd, nullptr, nullptr, &sz, MemDC, &DestPt, 0, &blendFunc, ULW_ALPHA);
			___DrawLock.Unlock();
			return true;
		}
		else
		{
			___DrawLock.Unlock();
			return false;
		}
	}
	inline void SetShadowOpacity(float opa, UINT time, DBezier* Nonlinear, UINT FrameRate)
	{
		___DrawLock.Lock();
		if (time)
		{
			this->ReDrawMem(1.0f);
			A_Sha_Opa.AddAni(alpha, opa, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, 233);
			___DrawLock.Unlock();
			return;
		}
		this->ReDrawMem(opa);
		alpha = opa;
		Update();
		___DrawLock.Unlock();
		return;
	}
	inline float GetShadowOpacity()
	{
		return alpha;
	}
	inline HWND GetHwnd()
	{
		return hWnd;
	}

	void Release()
	{
		if (!hWnd)
			return;

		___DrawLock.Lock();
		if (MemDC && MemOldBmp)
			SelectObject(MemDC, MemOldBmp);
		if (MemDC)
			DeleteDC(MemDC);
		if (MemBmp)
			DeleteObject(MemBmp);

		DestroyWindow(hWnd);

		A_Sha_Opa.Reset();
		hWnd = nullptr;
		MainRC = { 0 };
		Dpi = 0;
		radius = 0.0f;
		alpha = 0.0f;
		EdgeOut = 0;
		MemDC = nullptr;
		MemBmp = nullptr;
		MemOldBmp = nullptr;

		___DrawLock.Unlock();
		return;
	}

	inline void ShadowAnimation()
	{
		___DrawLock.Lock();
		if (A_Sha_Opa)
		{
			UINT aid;
			alpha = A_Sha_Opa.FrameCalc(aid);
			if (aid == 233)
			{
				ReDrawMem(alpha);
			}
			Update();
		}
		___DrawLock.Unlock();
		return;
	}

	inline void PosChangingProc(const HWND& MainHwnd, const LPARAM& lParam, const RECT& rc)
	{
		WINDOWPOS* pPos = (WINDOWPOS*)lParam;
		if (pPos)
		{
			GetWindowRect(MainHwnd, &MainRC);
			pPos->x = MainRC.left - EdgeOut;
			pPos->y = MainRC.top - EdgeOut;
			pPos->cx = MainRC.right - MainRC.left + EdgeOut * 2;
			pPos->cy = MainRC.bottom - MainRC.top + EdgeOut * 2;
		}
		return;
	}
	void MainMsgProc(const HWND& MainHwnd, const UINT& message, const WPARAM& wParam, const LPARAM& lParam)
	{
		switch (message)
		{
		case WM_WINDOWPOSCHANGED:
		{
			WINDOWPOS* pPos = (WINDOWPOS*)lParam;
			if (pPos)
			{
				if (pPos->cx != MainRC.right - MainRC.left || pPos->cy != MainRC.bottom - MainRC.top)
				{
					___DrawLock.Lock();
					if (A_Sha_Opa)
						ReDrawMem(1.0f);
					else
						ReDrawMem(alpha);
					Update();
					___DrawLock.Unlock();
				}
				MoveWindow(hWnd, pPos->x - EdgeOut, pPos->y - EdgeOut, pPos->cx + EdgeOut * 2, pPos->cy + EdgeOut * 2, true);
			}
			break;
		}
		case WM_SIZE:
		{
			if (wParam == SIZE_MAXIMIZED || wParam == SIZE_MINIMIZED)
				Show(SW_HIDE);
			else if (wParam == SIZE_RESTORED)
			{
				GetWindowRect(MainHwnd, &MainRC);
				MoveWindow(hWnd, MainRC.left - EdgeOut, MainRC.top - EdgeOut,
					MainRC.right - MainRC.left + EdgeOut * 2, MainRC.bottom - MainRC.top + EdgeOut * 2, true);

				___DrawLock.Lock();
				if (A_Sha_Opa)
					ReDrawMem(1.0f);
				else
					ReDrawMem(alpha);
				Show(SW_SHOW);
				___DrawLock.Unlock();
			}
			break;
		}
		case WM_SHOWWINDOW:
		{
			if (wParam)//指示窗口是否被隐藏
			{
				___DrawLock.Lock();
				Show(SW_SHOW);
				___DrawLock.Unlock();
			}
			else
				Show(SW_HIDE);
			break;
		}
		default:
		{
			break;
		}
		}
		return;
	}
	bool Show(int nCmdShow = SW_SHOWNORMAL)
	{
		if (hWnd)
		{
			Update();
			return ShowWindow(this->hWnd, nCmdShow);
		}
		return false;
	}

private:
	HWND hWnd = nullptr;
	RECT MainRC = { 0 };
	UINT EdgeOut = 0;
	int Dpi = 0;
	float radius = 0.0f;
	float alpha = 1.0f;

	HDC MemDC = nullptr;
	HBITMAP MemBmp = nullptr;
	HBITMAP MemOldBmp = nullptr;

	_AniCalc_f A_Sha_Opa;
	___DThreadLock ___DrawLock;
};



class DWindow
{
public:
	DWORD ExStyle = 0;
	DWORD Style = WS_POPUP;
	HICON hIcon = nullptr;
	HICON hIconSmall = nullptr;
	HWND Parent = nullptr;

	DWindow()
	{
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	}

	virtual~DWindow()
	{
		Release();
		CoUninitialize();
		return;
	}

	//定义隐式转换，此类可被看作为一个窗口句柄
	operator HWND()
	{
		return this->hWnd;
	}

	bool Create(HINSTANCE hInst, std::wstring ClassName, std::wstring Title, WNDPROC WndProc, int width, int height)
	{
		if (Inited || hWnd || !hInst || ClassName.empty() || Title.empty() || !WndProc || width < 1 || height < 1)
			return false;

		this->hInst = hInst;
		this->ClassName = ClassName;
		this->Title = Title;

		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEXW);

		if (!GetClassInfoEx(hInst, ClassName.c_str(), &wcex))
		{
			//窗口未注册，注册
			wcex.cbSize = sizeof(WNDCLASSEXW);
			wcex.style = 0;
			wcex.lpfnWndProc = WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hInst;
			wcex.hIcon = hIcon;
			wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			wcex.hbrBackground = nullptr;
			wcex.lpszMenuName = nullptr;
			wcex.lpszClassName = ClassName.c_str();
			wcex.hIconSm = hIconSmall;
			if (!RegisterClassExW(&wcex))
				return false;
		}

		Dpi = DXUI_GetWndDpi(nullptr);//获取一次DPI
		//这里的窗口大小要根据DPI的大小来改变，所以用了一套公式重新计算大小
		int X, Y, mw = max(width, 1), mh = max(height, 1);
		if (Parent)//如果有父窗口，此时的位置应在父窗口的正上方
		{
			RECT ParRc;
			GetWindowRect(Parent, &ParRc);
			X = (ParRc.left + ParRc.right - (int)DIP2DP(mw)) / 2;
			Y = (ParRc.top + ParRc.bottom - (int)DIP2DP(mh)) / 2;
		}
		else {
			X = (GetSystemMetrics(SM_CXSCREEN) - (int)DIP2DP(mw)) / 2;
			Y = (GetSystemMetrics(SM_CYSCREEN) - (int)DIP2DP(mh)) / 2;
		}
		this->hWnd = CreateWindowExW(ExStyle | (TransparWindow ? WS_EX_LAYERED : 0), ClassName.c_str(), Title.c_str(), Style,
			X, Y, mw * Dpi / 96, mh * Dpi / 96, Parent, nullptr, hInst, nullptr);

		if (!hWnd)
			return false;
		int mDpiWnd = DXUI_GetWndDpi(hWnd);//获取一次DPI
		if (Dpi != mDpiWnd)
		{
			Dpi = mDpiWnd;
			if (Parent)
			{
				RECT ParRc;
				GetWindowRect(Parent, &ParRc);
				X = (ParRc.left + ParRc.right - (int)DIP2DP(mw)) / 2;
				Y = (ParRc.top + ParRc.bottom - (int)DIP2DP(mh)) / 2;
			}
			else {
				X = (GetSystemMetrics(SM_CXSCREEN) - (int)DIP2DP(mw)) / 2;
				Y = (GetSystemMetrics(SM_CYSCREEN) - (int)DIP2DP(mh)) / 2;
			}
			SetWindowPos(hWnd, nullptr, X, Y, mw * Dpi / 96, mh * Dpi / 96, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW | SWP_ASYNCWINDOWPOS);
		}

		return true;
	}

	bool Show(int nCmdShow = SW_SHOWNORMAL)
	{
		if (Inited && hWnd)
		{
			return ShowWindow(this->hWnd, nCmdShow);
		}
		return false;
	}

	inline void SetShadowAlpha(float alpha, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		Shadow.SetShadowOpacity(alpha, time, Nonlinear, FrameRate);
		return;
	}
	//初始化渲染，要先创建窗口再初始化，可以用Create方法
	bool Init(DMsgProc Proc, float ShadowRadius = 2.0f, float ShadowAlpha = 0.9f, bool Moved = true, UINT FrameRate = DefFrameRate, DCOLOR Background = { 255, 255, 255, 1.0f })
	{
		if (this->Inited || !hWnd || !Proc)//条件检测
			return false;

		this->proc = Proc;
		this->FrameRate = max(1, min(60, FrameRate));
		this->BackColor.r = Background.r / 255.0f;
		this->BackColor.g = Background.g / 255.0f;
		this->BackColor.b = Background.b / 255.0f;
		this->BackColor.a = Background.a;
		this->CanBeMoved = Moved;

		Dpi = DXUI_GetWndDpi(hWnd);//有窗口句柄后，重新根据句柄获取一次DPI
		//获取客户区大小
		GetClientRect(hWnd, &this->rc);

		HRESULT hr;
		//创建D2D上下文
		hr = this->CreateDevice();
		if (FAILED(hr))
		{
			this->Release();
			return false;
		}
		//创建Dwrite库
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&this->DWFactory));
		if (FAILED(hr))
		{
			this->Release();
			return false;
		}
		//创建WIC图像库。因为版本2不支持Win7，所以失败后继续测试版本1
		hr = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&this->WicFactory));

		if (FAILED(hr))
		{
			hr = CoCreateInstance(CLSID_WICImagingFactory1, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&this->WicFactory));
			if (FAILED(hr))
			{
				this->Release();
				return false;
			}
		}

		//创建窗口边缘阴影
		this->Shadow.Create(this->hInst, this->hWnd, this->ClassName.c_str(), ShadowRadius, ShadowAlpha, Dpi);
		//设置窗口消息
		this->ConMsg = RegisterWindowMessageW(RegisterMessageStr);
		UpdateAll();//全屏更新一下

		this->Inited = true;
		return true;
	}

	void Release()
	{
		if (!Inited)
			return;
		Inited = false;

		this->ExitRenderThread();

		if (this->hWnd)
		{
			DestroyWindow(this->hWnd);
			this->hWnd = nullptr;
		}
		if (Shadow)
			Shadow.Release();

		//将子控件全部分离，因为停止渲染线程了，所以可以先分离后删除控件。否则必须注意顺序
		___ConLock.Lock();
		if (Controls.size())
		{
			std::map<UINT, DControl*>::iterator con = Controls.begin();
			for (UINT i = 0; i < Controls.size(); ++i, ++con)
			{
				con->second->_SeparateFromWindow();
			}
			Controls.clear();
		}
		___ConLock.Unlock();

		ClassName.clear();
		Title.clear();
		A_Wnd_X.Reset();
		A_Wnd_Y.Reset();
		hInst = nullptr;
		rc = { 0 };
		Dpi = 0;
		FlushInvalidRect = false;
		proc = nullptr;
		ConMsg = 0;
		BackgroundBlur = false;
		HardwareRendering = true;
		TrackLeave = false;
		CanBeMoved = true;
		OnMove = false;
		ClickPos = { 0 };
		m_dstTranspar = TransparWindow;
		FrameRate = DefFrameRate;//帧率
		AllUpdate = false;
		//保存当前活动的控件
		ActiveCon.clear();
		//背景颜色
		BackColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		this->ReleaseDevice();//释放D2D相关设备
		RELEASE(DWFactory);
		RELEASE(WicFactory);
		return;
	}
	void DeleteControl(UINT ControlID)
	{
		___ConLock.Lock();
		if (Controls.size())
		{
			std::map<UINT, DControl*>::iterator conit = Controls.find(ControlID);
			if (conit != Controls.end())
			{
				DControl* con = conit->second;

				DControl* parent = con->_GetParentCon();
				if (parent)
					parent->_DeleteChildCon(con);

				std::map<UINT, DControl*> child = con->_GetChildCon();
				if (child.size())
				{
					std::map<UINT, DControl*>::iterator chi = child.begin();
					for (UINT i = 0; i < child.size(); ++i, ++chi)
					{
						chi->second->_SetParentCon(parent);//将 欲删除的控件 的 Child 的 父控件 设为 欲删除控件的父控件
					}
				}

				con->_ClearChildCon();

				Controls.erase(conit);
				//由于删除控件时，可能正在渲染，所以必须先从控件列表中删除，然后再调用控件的分离函数
				con->_SeparateFromWindow();
			}
		}
		___ConLock.Unlock();
		return;
	}
	//将设备相关像素转换为设备无关像素
	inline int DPToDIP(int dp)
	{
		return (int)DP2DIP(dp);
	}
	//将设备无关像素转换为设备相关像素
	inline int DIPToDP(int dip)
	{
		return (int)DIP2DP(dip);
	}
	inline bool IsInit()
	{
		return Inited;
	}
	inline HWND GetHwnd()
	{
		return hWnd;
	}
	inline int GetDpi()
	{
		return Dpi;
	}
	inline void SetDpi(const int& dpi)//设置DXUI整个库的当前DPI，96为默认值，即无缩放
	{
		if (!hWnd)
			return;
		___RenderMutex.Lock();
		RECT r;
		if (GetWindowRect(hWnd, &r))
		{
			long wid = r.right - r.left, hei = r.bottom - r.top;
			wid = (long)ceil(wid / (float)Dpi * (float)dpi);
			hei = (long)ceil(hei / (float)Dpi * (float)dpi);
			Dpi = dpi;//在此处才能设置
			Shadow.SetDpi(Dpi);
			SetWindowPos(hWnd, nullptr, 0, 0, max(wid, 1), max(hei, 1), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW | SWP_ASYNCWINDOWPOS);
			//获取客户区大小
			GetClientRect(hWnd, &this->rc);
			this->ReleaseDevice();
			this->CreateDevice(true);
			UpdateAll();
		}
		___RenderMutex.Unlock();
		return;
	}

	inline void UpdateAll()
	{
		___AllUpdateLock.Lock();
		AllUpdate = true;
		___AllUpdateLock.Unlock();
		return;
	}

	inline bool CreateRenderThread()//创建DXUI渲染线程
	{
		if (RenderThreadAlive || !Inited)
			return false;

		//在创建前先画上一帧，保证画面立即显示，不用等待线程启动
		this->_RenderFrame();

		if (_beginthread(DXUI_RenderThread, 0, this) == -1)
			return false;
		else
			return true;
	}

	//设置窗口为背景透明的窗口。此设置会保存，所以想要创建时即透明，可以在创建之前设置。
	void SetTransparent(bool transparent)
	{
		___TransLock.Lock();
		m_dstTranspar = transparent;
		___TransLock.Unlock();
		return;
	}
	//获取当前窗口是否为背景透明窗口
	inline bool GetTransparentWindow()
	{
		return TransparWindow;
	}

	//设置窗口的鼠标穿透，必须在有窗口存在时才能成功。此设置不保存，重建窗口时需要重新设置
	inline void SetWindowMouseThrough(bool through)
	{
		if (hWnd)
			SetWindowLongW(hWnd, GWL_EXSTYLE, (through ? (GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT) : (GetWindowLongW(hWnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT)));
		return;
	}

	//获取是否应用Transparent窗口的窗口背景模糊，仅在Win10下有效，仅当窗口已创建后才有意义。此设置不保存，重建窗口时需要重新设置
	inline bool GetBackgroundBlur_Win10()
	{
		return BackgroundBlur;
	}
	//设置Transparent窗口的窗口背景模糊，仅在Win10下有效，仅当窗口已创建后才会应用。此设置不保存，重建窗口时需要重新设置
	bool SetBackgroundBlur_Win10(bool blur)
	{
		if (!hWnd || BackgroundBlur == blur)
			return false;

		typedef enum class _WINDOWCOMPOSITIONATTRIB
		{
			WCA_ACCENT_POLICY = 19,
		} WINDOWCOMPOSITIONATTRIB;
		typedef struct _WINDOWCOMPOSITIONATTRIBDATA
		{
			WINDOWCOMPOSITIONATTRIB Attrib;
			PVOID pvData;
			SIZE_T cbData;
		} WINDOWCOMPOSITIONATTRIBDATA;
		typedef enum class _ACCENT_STATE
		{
			ACCENT_DISABLED = 0,
			ACCENT_ENABLE_BLURBEHIND = 3,
		} ACCENT_STATE;

		typedef struct _ACCENT_POLICY
		{
			ACCENT_STATE AccentState;
			DWORD AccentFlags;
			DWORD GradientColor;
			DWORD AnimationId;
		} ACCENT_POLICY;

		typedef BOOL(WINAPI* pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

		HMODULE hUser = GetModuleHandleW(L"User32.dll");
		if (hUser)
		{
			pfnSetWindowCompositionAttribute setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
			if (setWindowCompositionAttribute)
			{
				BackgroundBlur = blur;
				ACCENT_POLICY accent = { BackgroundBlur ? ACCENT_STATE::ACCENT_ENABLE_BLURBEHIND : ACCENT_STATE::ACCENT_DISABLED, 0, 0, 0 };
				WINDOWCOMPOSITIONATTRIBDATA data;
				data.Attrib = WINDOWCOMPOSITIONATTRIB::WCA_ACCENT_POLICY;
				data.pvData = &accent;
				data.cbData = sizeof(accent);
				return setWindowCompositionAttribute(hWnd, &data);
			}
			else return false;
		}
		else return false;
	}

	inline bool _GetExitRenderThreadFlag()
	{
		return ExitRenderThreadFlag;
	}

	inline void _SetRenderThreadAlive(bool alive)
	{
		RenderThreadAlive = alive;
		return;
	}

	inline void ExitRenderThread()//退出渲染线程
	{
		if (RenderThreadAlive)
		{
			ExitRenderThreadFlag = true;
			while (RenderThreadAlive)
				Sleep(3);
			ExitRenderThreadFlag = false;
		}
		return;
	}

	bool _RenderFrame(bool Lock = true)
	{
		if (!Inited)
			return false;

		if (Lock)
			___RenderMutex.Lock();//渲染线程锁开启

		//响应用户更改Transparent。在线程里响应不会阻塞主程序
		___TransLock.Lock();
		if (TransparWindow != m_dstTranspar)
			m_SetTransparent(m_dstTranspar);
		___TransLock.Unlock();
		
		if (!D2DContext || !TargetBmp || (TransparWindow ? (!CacheDC || !CacheBmp) : !SwapChain))
		{
			this->ReleaseDevice();
			if (SUCCEEDED(this->CreateDevice()))
				UpdateAll();
			else
			{
				if (Lock)
					___RenderMutex.Unlock();//解锁
				return false;
			}
		}

		DisposeAnimation();//处理窗口相关动画帧
		bool Began = false;

		___AllUpdateLock.Lock();
		bool thiscallUpdate = AllUpdate;
		if (thiscallUpdate)
		{
			AllUpdate = false;
			D2DContext->BeginDraw();
			Began = true;
			D2DContext->Clear(BackColor);
		}
		___AllUpdateLock.Unlock();
		___FlushRectLock.Lock();
		bool thiscallFlush = FlushInvalidRect;
		if (thiscallFlush)
			FlushInvalidRect = false;
		___FlushRectLock.Unlock();

		std::vector<RECT> dirtyRect;
		___ConLock.Lock();
		if (Controls.size())
		{
			std::map<UINT, DControl::__updItem> UpdCon;
			std::map<UINT, DControl::__updItem>::iterator UpdConIt;
			std::pair<std::map<UINT, DControl::__updItem>::iterator, bool> insIt;
			std::map<UINT, DControl*>::iterator con = Controls.begin();
			std::deque<D2D1_RECT_F>::iterator updit;
			UINT i1, s;

			//预处理一下，先把屏清了
			for (size_t i = 0; i < Controls.size(); ++i, ++con)
			{
				if (con->second)
				{
					if (thiscallUpdate)
						con->second->_SetNeedUpdate();

					con->second->___NeedUpdateLock.Lock();
					if (con->second->_GetNeedUpdate())
					{
						con->second->___UpdateClipLock.Lock();
						insIt = UpdCon.insert(std::pair<UINT, DControl::__updItem>(con->first, { con->second, con->second->UpdateClip }));
						con->second->UpdateClip.clear();//每一帧都要置为空
						con->second->NeedUpdate = false;
						con->second->___UpdateClipLock.Unlock();
						con->second->___NeedUpdateLock.Unlock();

						if (insIt.second)
						{
							//呼叫子控件更新
							insIt.first->second.con->_CallChild(insIt.first->second.UpdClip);
						}
					}//NeedUpdate End
					else
						con->second->___NeedUpdateLock.Unlock();
				}
			}//for End

			UpdConIt = UpdCon.begin();
			UINT nowID;
			D2D1_RECT_F r1;//临时使用，在循环外防止重复创建
			//开始循环绘制
			while (UpdConIt != UpdCon.end())
			{
				if (!Began)
				{
					D2DContext->BeginDraw();
					Began = true;
				}

				//开始根据UpdateClip分区域画内容
				nowID = UpdConIt->first;
				updit = UpdConIt->second.UpdClip.begin();
				i1 = 0;
				s = UpdConIt->second.UpdClip.size();
				if (s)
				{
					do
					{
						r1 = *updit;
						r1 = { (float)max(min(r1.left, rc.right), rc.left), (float)max(min(r1.top, rc.bottom), rc.top), (float)min(max(r1.right, rc.left), rc.right), (float)min(max(r1.bottom, rc.top), rc.bottom) };
						if (IsRectWorkable(r1) && UpdConIt->second.con->_Draw(D2DContext, BackColor, &r1))
							dirtyRect.push_back({ (long)r1.left, (long)r1.top, (long)r1.right, (long)r1.bottom });

						++i1;
						++updit;
					} while (i1 < s);
				}
				else {
					r1 = UpdConIt->second.con->_GetUpdateRC();
					RECT r2 = { (long)max(min(r1.left, rc.right), rc.left), (long)max(min(r1.top, rc.bottom), rc.top), (long)min(max(r1.right, rc.left), rc.right), (long)min(max(r1.bottom, rc.top), rc.bottom) };
					if (IsRectWorkable(r2) && UpdConIt->second.con->_Draw(D2DContext, BackColor, nullptr))
						dirtyRect.push_back(r2);
				}
				UpdConIt->second.con->_CallChildUpdateBackImg(D2DContext, UpdCon, UpdConIt, UpdConIt->second.UpdClip);

				//调试选项
				if (Dbg_ShowUpdate)
				{
					srand((UINT)timeGetTime());
					ID2D1SolidColorBrush* b;
					D2DContext->CreateSolidColorBrush({ (float)rand() / RAND_MAX + 0.2f, (float)rand() / RAND_MAX + 0.2f, (float)rand() / RAND_MAX + 0.2f, 1.0f }, &b);
					if (b)
					{
						updit = UpdConIt->second.UpdClip.begin();
						i1 = 0;
						s = UpdConIt->second.UpdClip.size();
						D2D1_RECT_F r;
						do
						{
							r = UpdConIt->second.con->_GetUpdateRect(s ? &(*updit) : nullptr);
							D2DContext->DrawRectangle({ r.left + DIP2DP(0.75f), r.top + DIP2DP(0.75f), r.right - DIP2DP(0.75f), r.bottom - DIP2DP(0.75f) }, b, DIP2DP(1.5f));
							++i1;
							if (s)
								++updit;
						} while (i1 < s);
						b->Release();
					}
				}
				++UpdConIt;
			}//while End
			//集体通知当前活动控件计算下一帧数据
			UpdConIt = UpdCon.begin();
			while (UpdConIt != UpdCon.end())
			{
				UpdConIt->second.con->_DisposeAnimation();
				++UpdConIt;
			}
		}
		___ConLock.Unlock();

		if (Began || thiscallFlush)
		{
			HRESULT hr = S_OK;
			if (Began)
				hr = D2DContext->EndDraw();//对于DeviceContext，只需在Present时处理返回即可

			if (TransparWindow)//Trans时需要判断EndDraw返回值，其它时候不需要
			{
				if (SUCCEEDED(hr))
				{
					UINT dtrSz = dirtyRect.size();
					if (dtrSz)
					{
						bool NewCreate;
						//创建 Cache D2D Bitmap
						if (!CacheD2DBmp)
						{
							D2D1_BITMAP_PROPERTIES1 prop;
							prop.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
							prop.dpiX = 96.0f;
							prop.dpiY = 96.0f;
							prop.bitmapOptions = D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
							prop.colorContext = nullptr;
							hr = this->D2DContext->CreateBitmap({ (UINT)(rc.right - rc.left), (UINT)(rc.bottom - rc.top) }, nullptr, 0, prop, &CacheD2DBmp);
							NewCreate = true;
						}
						else NewCreate = false;

						if (SUCCEEDED(hr))
						{
							BITMAPINFO lbi;
							lbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
							lbi.bmiHeader.biBitCount = 32;
							lbi.bmiHeader.biHeight = rc.bottom - rc.top;
							lbi.bmiHeader.biWidth = rc.right - rc.left;
							lbi.bmiHeader.biPlanes = 1;
							lbi.bmiHeader.biCompression = BI_RGB;
							lbi.bmiHeader.biXPelsPerMeter = 0;
							lbi.bmiHeader.biYPelsPerMeter = 0;
							lbi.bmiHeader.biClrUsed = 0;
							lbi.bmiHeader.biClrImportant = 0;
							lbi.bmiHeader.biSizeImage = lbi.bmiHeader.biWidth * lbi.bmiHeader.biHeight * 4;
							D2D1_POINT_2U point;
							D2D1_RECT_U Src;
							if (thiscallUpdate || NewCreate)
							{
								point.x = 0;
								point.y = 0;
								Src.left = 0;
								Src.top = 0;
								Src.right = lbi.bmiHeader.biWidth;
								Src.bottom = lbi.bmiHeader.biHeight;
								hr = CacheD2DBmp->CopyFromBitmap(&point, TargetBmp, &Src);
							}
							else
							{
								std::vector<RECT>::iterator dtrIt = dirtyRect.begin();
								for (UINT i = 0; i < dtrSz; ++i)
								{
									point.x = dtrIt->left;
									point.y = dtrIt->top;
									Src.left = dtrIt->left;
									Src.top = dtrIt->top;
									Src.right = dtrIt->right;
									Src.bottom = dtrIt->bottom;
									hr = CacheD2DBmp->CopyFromBitmap(&point, TargetBmp, &Src);
									if (FAILED(hr))
										break;
									++dtrIt;
								}
							}
							if (SUCCEEDED(hr))
							{
								D2D1_MAPPED_RECT map;
								hr = CacheD2DBmp->Map(D2D1_MAP_OPTIONS_READ, &map);
								if (SUCCEEDED(hr))
								{
									lbi.bmiHeader.biSizeImage = map.pitch * lbi.bmiHeader.biHeight;
									lbi.bmiHeader.biWidth = map.pitch / 4;
									lbi.bmiHeader.biHeight = -lbi.bmiHeader.biHeight;
									SetDIBits(CacheDC, CacheBmp, 0, -lbi.bmiHeader.biHeight, map.bits, &lbi, DIB_RGB_COLORS);
									CacheD2DBmp->Unmap();

									BLENDFUNCTION blendFunc;
									blendFunc.AlphaFormat = AC_SRC_ALPHA;
									blendFunc.BlendFlags = 0;
									blendFunc.BlendOp = AC_SRC_OVER;
									blendFunc.SourceConstantAlpha = 255;
									SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
									POINT DestPt = { 0, 0 };
									UpdateLayeredWindow(hWnd, nullptr, nullptr, &sz, CacheDC, &DestPt, 0, &blendFunc, ULW_ALPHA);
								}
							}
						}
					}
				}
			}
			else {
				DXGI_PRESENT_PARAMETERS p = { 0 };
				if (thiscallUpdate || thiscallFlush || !HardwareRendering)//全部刷新或窗口由屏幕外移动进屏幕时，需要刷新区域。软件渲染时也需要整个刷新，否则会有轻微卡顿。
					hr = SwapChain->Present1(1, 0, &p);
				else
				{
					p.DirtyRectsCount = dirtyRect.size();
					if (p.DirtyRectsCount)
					{
						if (p.DirtyRectsCount == 1)//如果count为1，则系统不会触发局部更新。我们来手动触发
						{
							RECT dtr[2];
							dtr[0] = dirtyRect[0];
							dtr[1] = { 0, 0, 1, 1 };
							++p.DirtyRectsCount;
							p.pDirtyRects = dtr;
							hr = SwapChain->Present1(1, 0, &p);
						}
						else
						{
							p.pDirtyRects = &dirtyRect.front();
							hr = SwapChain->Present1(1, 0, &p);
						}
					}
				}
			}
			//错误处理
			if (hr != S_OK && (TransparWindow ? true : (hr != DXGI_STATUS_OCCLUDED)))
				this->ReleaseDevice();//释放D2D相关设备
		}

		if (Lock)
			___RenderMutex.Unlock();//解锁

		return Began;
	}

	inline UINT GetFrameRate()
	{
		return FrameRate;
	}
	inline DWORD GetFrameDealy()
	{
		return 1000 / FrameRate;
	}

	//DXUI处理Win消息的函数。返回值为当前活动的控件ID，没有活动控件返回0
	LRESULT DXUIMessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		//传递给窗口阴影消息
		if (Shadow)
		{
			if (hWnd == this->hWnd)
				Shadow.MainMsgProc(hWnd, message, wParam, lParam);
			else if (hWnd == Shadow.GetHwnd() && message == WM_WINDOWPOSCHANGING)
				Shadow.PosChangingProc(this->hWnd, lParam, rc);
		}
		if (!Inited)
			return -1;

		LRESULT ReturnValue = -1;
		if (hWnd == this->hWnd && Controls.size())
		{
			switch (message)
			{
			case WM_MOUSEMOVE:
			{
				if (!TrackLeave)//设置WM_MOUSELEAVE消息
				{
					TRACKMOUSEEVENT tme = { 0 };
					tme.cbSize = sizeof(tme);
					tme.hwndTrack = hWnd;
					tme.dwFlags = TME_LEAVE;

					TrackMouseEvent(&tme);
					TrackLeave = true;
				}
				int xPos = GET_X(lParam);
				int yPos = GET_Y(lParam);

				//如果在OnMove，那就手动移动
				if (OnMove)
				{
					A_Wnd_X.Reset();
					A_Wnd_Y.Reset();

					RECT NowRC;
					if (GetWindowRect(hWnd, &NowRC))
					{
						SetWindowPos(hWnd, nullptr, NowRC.left + xPos - ClickPos.x, NowRC.top + yPos - ClickPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW | SWP_ASYNCWINDOWPOS);
						break;
					}
				}

				___ConLock.Lock();
				std::map<UINT, DControl*>::iterator con = Main_HitTest(xPos, yPos);//命中测试

				//处理鼠标穿透
				while (con != Controls.end() && con->second->MouseThrough)
				{
					if (con->second->Parent)
						con = Controls.find(con->second->Parent->ID);
					else
						con = Controls.end();
				}

				if (con != Controls.end())
				{
					RestoreLastActiveControl(con->second->GetID(), message);

					UINT conID = con->second->ID;
					DControlType type = con->second->ControlType;

					con->second->_MouseMoveMsgProc(xPos, yPos);
					if (con->second->State != DControlState::Normal)
					{
						___ActConLock.Lock();
						ActiveCon.insert(std::pair<UINT, DControl*>(con->second->GetID(), con->second));
						___ActConLock.Unlock();
					}
					___ConLock.Unlock();

					//通知给用户
					DMsgProc p = proc;
					if (p)
						p(hWnd, conID, type, DControlMsg::Control_MouseMove, lParam, 0);
				}
				else {
					___ConLock.Unlock();
					RestoreLastActiveControl(0, message);
				}

				ReturnValue = 0;//如果程序处理此消息。返回值必须为0
				break;
			}
			case WM_SETCURSOR:
			{
				POINT pos;
				GetCursorPos(&pos);//获取鼠标指针位置（屏幕坐标）
				ScreenToClient(hWnd, &pos);//将鼠标指针位置转换为窗口坐标

				___ConLock.Lock();
				std::map<UINT, DControl*>::iterator con = Main_HitTest((int)pos.x, (int)pos.y);//命中测试

				//处理鼠标穿透
				while (con != Controls.end() && con->second->MouseThrough)
				{
					if (con->second->Parent)
						con = Controls.find(con->second->Parent->ID);
					else
						con = Controls.end();
				}

				if (con != Controls.end())
				{
					if (con->second->_SetCursorMsgProc())
						ReturnValue = 0;
				}
				___ConLock.Unlock();

				break;
			}
			case WM_MOVE:
			{
				___FlushRectLock.Lock();
				FlushInvalidRect = true;
				___FlushRectLock.Unlock();
				DMsgProc p = proc;
				if (p)
					p(hWnd, 0, DControlType::Null, DControlMsg::Window_Position, GET_X(lParam), GET_Y(lParam));
				break;
			}
			case WM_LBUTTONDOWN:
			{
				int xPos = GET_X(lParam);
				int yPos = GET_Y(lParam);

				___ConLock.Lock();
				std::map<UINT, DControl*>::iterator con = Main_HitTest(xPos, yPos);//命中测试

				//处理鼠标穿透
				while (con != Controls.end() && con->second->MouseThrough)
				{
					if (con->second->Parent)
						con = Controls.find(con->second->Parent->ID);
					else
						con = Controls.end();
				}

				if (con != Controls.end())
				{
					RestoreLastActiveControl(con->second->GetID(), message);

					UINT conID = con->second->ID;
					DControlType type = con->second->ControlType;

					con->second->_LMouseDownMsgProc(hWnd, xPos, yPos);

					con->second->_ImeComposition(hWnd, WM_IME_STARTCOMPOSITION, wParam, lParam);

					___ConLock.Unlock();

					//通知给用户
					DMsgProc p = proc;
					if (p)
						p(hWnd, conID, type, DControlMsg::Control_LButtonDown, lParam, 0);
				}
				else
				{
					___ConLock.Unlock();
					RestoreLastActiveControl(0, message);
				}

				//将鼠标按下时的位置记录下来
				if ((con != Controls.end() && con->second->CanMoveWindow) || (CanBeMoved && con == Controls.end()))
				{
					OnMove = true;
					ClickPos.x = xPos;
					ClickPos.y = yPos;
				}
				SetCapture(hWnd);

				break;
			}
			case WM_LBUTTONUP:
			{
				int xPos = GET_X(lParam);
				int yPos = GET_Y(lParam);

				ReleaseCapture();

				if (OnMove)
					OnMove = false;

				___ConLock.Lock();
				std::map<UINT, DControl*>::iterator con = Main_HitTest(xPos, yPos);//命中测试

				//处理鼠标穿透
				while (con != Controls.end() && con->second->MouseThrough)
				{
					if (con->second->Parent)
						con = Controls.find(con->second->Parent->ID);
					else
						con = Controls.end();
				}

				if (con != Controls.end())
				{
					RestoreLastActiveControl(con->second->GetID(), message, true);

					UINT conID = con->second->ID;
					DControlType type = con->second->ControlType;

					bool Disposed = con->second->_LMouseUpMsgProc(xPos, yPos);
					___ConLock.Unlock();

					//通知给用户
					DMsgProc p = proc;
					if (p)
						p(hWnd, conID, type, DControlMsg::Control_LButtonUp, lParam, 0);
					p = proc;
					if (Disposed && p)
						p(hWnd, conID, type, DControlMsg::Control_Click, lParam, 0);
				}
				else
				{
					___ConLock.Unlock();
					RestoreLastActiveControl(0, message, true);
				}

				break;
			}
			case WM_KEYDOWN:
			{
				___ActConLock.Lock();
				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = --ActiveCon.end();

					while (con != ActiveCon.end())
					{
						if (con->second->_KeyDownMsg(wParam))
						{
							ReturnValue = 0;
							break;
						}
						--con;
					}
				}
				___ActConLock.Unlock();
				break;
			}
			case WM_IME_STARTCOMPOSITION:
			{
				___ActConLock.Lock();
				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = --ActiveCon.end();

					while (con != ActiveCon.end())
					{
						if (con->second->_ImeComposition(hWnd, message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
						--con;
					}
				}
				___ActConLock.Unlock();

				break;
			}
			case WM_IME_REQUEST:
			{
				___ActConLock.Lock();
				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = --ActiveCon.end();

					while (con != ActiveCon.end())
					{
						if (con->second->_ImeComposition(hWnd, message, wParam, lParam))
						{
							ReturnValue = 1;
							break;
						}
						--con;
					}
				}
				___ActConLock.Unlock();

				break;
			}
			case WM_IME_CHAR:
			{
				___ActConLock.Lock();

				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = --ActiveCon.end();

					while (con != ActiveCon.end())
					{
						if (con->second->_CharMsg(message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
						--con;
					}
				}
				___ActConLock.Unlock();

				break;
			}
			case WM_IME_COMPOSITION:
			{
				___ActConLock.Lock();
				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = --ActiveCon.end();

					while (con != ActiveCon.end())
					{
						if (con->second->_ImeComposition(hWnd, message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
						--con;
					}
				}
				___ActConLock.Unlock();
				break;
			}
			case WM_CHAR://与WM_IME_CHAR完全一样，因为_CharMsg对两种字符消息都接收
			{
				___ActConLock.Lock();

				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = --ActiveCon.end();

					while (con != ActiveCon.end())
					{
						if (con->second->_CharMsg(message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
						--con;
					}
				}
				___ActConLock.Unlock();

				break;
			}
			case WM_MOUSEWHEEL:
			{
				POINT p;
				p.x = GET_X(lParam);
				p.y = GET_Y(lParam);
				ScreenToClient(hWnd, &p);

				___ConLock.Lock();

				std::map<UINT, DControl*>::iterator con = Main_HitTest((int)p.x, (int)p.y);//命中测试

				//处理鼠标穿透
				while (con != Controls.end() && con->second->MouseThrough)
				{
					if (con->second->Parent)
						con = Controls.find(con->second->Parent->ID);
					else
						con = Controls.end();
				}

				if (con != Controls.end())
				{
					DControl* whlCon = con->second;
					while (whlCon)
					{
						if (whlCon->_MouseWheelMsgProc(hWnd, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
						else
							whlCon = whlCon->_GetParentCon();
					}
				}
				___ConLock.Unlock();

				break;
			}
			case WM_MOUSELEAVE://鼠标离开，删除当前活动控件，并把该控件设为正常状态
			{
				OnMove = false;
				TrackLeave = false;
				this->RestoreLastActiveControl(0, message);
				break;
			}
			case WM_SIZE:
			{
				this->Resize(wParam, lParam);
				ReturnValue = 0;
				break;
			}
			case WM_SHOWWINDOW:
			{
				if (wParam)
				{
					if (lParam == SW_PARENTOPENING && Parent)
						this->Show(SW_SHOW);
					GetClientRect(hWnd, &rc);
					UpdateAll();
					ReturnValue = 0;
				}
				break;
			}
			case WM_DESTROY:
			{
				if (Inited)
					this->Release();
				break;
			}
			case WM_DPICHANGED://在 Win8.1 以上可能会收到此消息，响应更改
			{
				SetDpi(LOWORD(wParam));
				ReturnValue = 0;//处理此消息需返回0
				break;
			}
			default:
				break;
			}
			if (message == ConMsg)
			{
				___DXUISendConMsg* m = (___DXUISendConMsg*)lParam;
				if (m && proc)
				{
					proc(hWnd, m->ID, m->Type, m->Msg, m->wParam, m->lParam);
					delete m;
				}
			}
		}
		return ReturnValue;
	}

	void SetPosition(long x, long y, UINT time = 0, DBezier* Nonlinear = nullptr, UINT AID = 0, bool Reserved = true)
	{
		if (Reserved)
		{
			RECT r;
			GetWindowRect(hWnd, &r);
			A_Wnd_X.AddAni(r.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			A_Wnd_Y.AddAni(r.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			return;
		}

		SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		return;
	}

	bool AddControl(DControl* control, UINT ParentID = 0, bool MouseThrough = false)
	{
		if (!Inited || !control || !control->IsInit())
			return false;

		control->_SetNeedUpdate();//设为true，加入到控件列表后即绘制

		UINT ID;

		___ConLock.Lock();
		if (Controls.empty())
			ID = 1;
		else {
			std::map<UINT, DControl*>::iterator end = Controls.end();
			--end;
			ID = end->first + 1;
		}
		control->_SetID(ID);//为控件设置ID，以便于管理多个控件

		//给控件传递所有者窗口和回调信息
		control->_SetMsgBack(hWnd, ConMsg);
		//传递鼠标穿透值
		control->MouseThrough = MouseThrough;
		//给控件传递Factory
		control->_SetFactory(D2DFactory, WicFactory, DWFactory);
		//设置主窗口是否是透明窗口
		control->_SetOwnerWndTransparent(TransparWindow);
		//传递主窗口的dpi，此函数会设置UpdateRC
		control->Dpi = Dpi;
		control->_UpdateDpi();
		//传递帧率给控件
		control->_SetFrameRate(FrameRate);
		//寻找父控件，并处理
		if (ParentID)
		{
			std::map<UINT, DControl*>::iterator parent = Controls.find(ParentID);
			if (parent != Controls.end())
			{
				parent->second->_AddChildCon(control);
				control->_SetParentCon(parent->second);
				parent->second->_SetNeedUpdate();//将父控件设为要刷新，以更新子控件的BackImg
			}
		}

		//绘制命中测试BMP
		control->_CreateHitTestGeo();
		//重置控件状态
		control->_SetNormalState(this->hWnd, 0);
		//最后插入到控件列表
		Controls.insert(std::pair<UINT, DControl*>(ID, control));
		___ConLock.Unlock();

		return true;
	}


	inline void Dbg_SetShowUpdateRect(bool show)
	{
		Dbg_ShowUpdate = show;
		if (!Dbg_ShowUpdate)
			UpdateAll();
		return;
	}
	inline bool Dbg_GetShowUpdateRect()
	{
		return Dbg_ShowUpdate;
	}
private:
	bool Dbg_ShowUpdate = false;
	bool HardwareRendering = true;
	bool BackgroundBlur = false;
	bool TransparWindow = false;
	bool m_dstTranspar = false;//用户设置时暂时保存用
	___DThreadLock ___TransLock;

	bool Inited = false;

	std::wstring ClassName;
	std::wstring Title;
	HINSTANCE hInst = nullptr;
	HWND hWnd = nullptr;
	RECT rc = { 0 };
	int Dpi = 0;

	bool FlushInvalidRect = false;//窗口位置移动时，标志渲染线程更新前缓冲区
	___DThreadLock ___FlushRectLock;

	_AniCalc_l A_Wnd_X;
	_AniCalc_l A_Wnd_Y;
	DMsgProc proc = nullptr;
	UINT ConMsg = 0;
	__DWinShadow Shadow;

	bool TrackLeave = false;
	bool CanBeMoved = true;
	bool OnMove = false;//鼠标是否正在被按住
	POINT ClickPos = { 0 };//鼠标按住时记录下位置，以便鼠标拖动时移动窗口

	___DThreadLock ___RenderMutex;
	//渲染线程的控制变量
	bool RenderThreadAlive = false;
	bool ExitRenderThreadFlag = false;

	ID2D1Factory1* D2DFactory = nullptr;
	ID2D1DeviceContext* D2DContext = nullptr;
	IDXGISwapChain1* SwapChain = nullptr;
	ID2D1Bitmap1* TargetBmp = nullptr;
	ID2D1Bitmap1* CacheD2DBmp = nullptr;
	//TransparentWindow使用
	HDC CacheDC = nullptr;
	HBITMAP CacheBmp = nullptr;
	HBITMAP CacheOldBmp = nullptr;

	IDWriteFactory* DWFactory = nullptr;
	IWICImagingFactory* WicFactory = nullptr;

	UINT FrameRate = DefFrameRate;//帧率

	bool AllUpdate = false;
	___DThreadLock ___AllUpdateLock;
	//保存所有控件的列表
	std::map<UINT, DControl*> Controls;
	___DThreadLock ___ConLock;
	//保存当前活动的控件
	std::map<UINT, DControl*> ActiveCon;//可能存在文本框（Edit状态）和按钮（MouseMove状态）同时活动的情况，所以用map
	___DThreadLock ___ActConLock;
	//背景颜色
	D2D1_COLOR_F BackColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	//ReleaseDevice与CreateDevice外围都要加锁
	inline void ReleaseDevice()
	{
		if (D2DContext)
			D2DContext->SetTarget(nullptr);
		Trans_DeleteCacheSurface();
		RELEASE(CacheD2DBmp);
		RELEASE(TargetBmp);
		RELEASE(SwapChain);
		RELEASE(D2DContext);
		RELEASE(D2DFactory);
		return;
	}
	HRESULT CreateDevice(bool CallChildUpdateDpi = false)
	{
		if (!hWnd)
			return E_FAIL;
		if (TransparWindow)
			return Trans_CreateDevice(CallChildUpdateDpi);

		HRESULT hr;
		UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		ID3D11Device* D3DDevice = nullptr;
		ID3D11DeviceContext* D3DContext = nullptr;
		ID2D1Device* D2DDevice = nullptr;
		IDXGIFactory2* DXGIFactory = nullptr;
		IDXGIDevice1* DXGIDevice = nullptr;
		IDXGIAdapter* DXGIAdapter = nullptr;
		IDXGISurface1* BackBuffer = nullptr;
		D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0,D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0,D3D_FEATURE_LEVEL_9_3,D3D_FEATURE_LEVEL_9_2,D3D_FEATURE_LEVEL_9_1 }, FeatureLevel;

		//创建 D3D设备
#ifdef _DEBUG//如果为Debug模式，则启用D3D调试层
		Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flags, FeatureLevels, 7, D3D11_SDK_VERSION, &D3DDevice, &FeatureLevel, &D3DContext);
		if (SUCCEEDED(hr))
			HardwareRendering = true;
		else if (hr == DXGI_ERROR_UNSUPPORTED)//不支持GPU渲染，切换CPU渲染
		{
			hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, Flags, FeatureLevels, 7, D3D11_SDK_VERSION, &D3DDevice, &FeatureLevel, &D3DContext);
			if (SUCCEEDED(hr))
				HardwareRendering = false;
		}

		if (SUCCEEDED(hr))
		{
			//通过 D3D设备 获取 DXGI设备
			hr = D3DDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&DXGIDevice);
			if (SUCCEEDED(hr))
			{
				//创建多线程 D2D Factory
#ifdef _DEBUG//如果为Debug模式，则启用D2D调试层
				D2D1_FACTORY_OPTIONS options;
				options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
				hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &options, reinterpret_cast<void**>(&this->D2DFactory));
#else
				hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), reinterpret_cast<void**>(&this->D2DFactory));
#endif
				if (SUCCEEDED(hr))
				{
					//通过 DXGI设备 与 D2D Factory 创建 D2D设备
					hr = this->D2DFactory->CreateDevice(DXGIDevice, &D2DDevice);
					if (SUCCEEDED(hr))
					{
						//通过 D2D设备 创建 D2DContext
						hr = D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &this->D2DContext);
						if (SUCCEEDED(hr))
						{
							//通过 DXGI设备 获取 DXGI Factory，以便创建交换链
							hr = DXGIDevice->GetAdapter(&DXGIAdapter);
							if (SUCCEEDED(hr))
							{
								hr = DXGIAdapter->GetParent(IID_PPV_ARGS(&DXGIFactory));
								if (SUCCEEDED(hr))
								{
									//交换链信息
									DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = { 0 };
									SwapChainDesc.Width = this->rc.right;
									SwapChainDesc.Height = this->rc.bottom;
									SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
									SwapChainDesc.Stereo = FALSE;
									SwapChainDesc.SampleDesc.Count = 1;
									SwapChainDesc.SampleDesc.Quality = 0;
									SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
									SwapChainDesc.BufferCount = 1;
									SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
									SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;//定义SEQUENTIAL此处必须为STRETCH
									SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
									SwapChainDesc.Flags = 0;
									//通过 D3D设备 与 DXGI Factory 创建交换链
									hr = DXGIFactory->CreateSwapChainForHwnd(D3DDevice, hWnd, &SwapChainDesc, nullptr, nullptr, &this->SwapChain);
									DP0(L"***DXUI提示：DXGI WARNING无需理会。此代码仅在Debug模式下执行。***\n");
									if (SUCCEEDED(hr))
									{
										hr = DXGIDevice->SetMaximumFrameLatency(1);
										if (SUCCEEDED(hr))
										{
											//获取后缓冲区的 DXGI Surface
											hr = this->SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));
											{
												if (SUCCEEDED(hr))
												{
													//通过后缓冲区 DXGI Surface 创建 D2D Bitmap
													D2D1_BITMAP_PROPERTIES1 prop;
													prop.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
													prop.dpiX = 96.0f;
													prop.dpiY = 96.0f;
													prop.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
													prop.colorContext = nullptr;
													hr = this->D2DContext->CreateBitmapFromDxgiSurface(BackBuffer, &prop, &TargetBmp);
													if (SUCCEEDED(hr))
													{
														//设置绘制目标为 D2D Bitmap
														this->D2DContext->SetTarget(TargetBmp);
														//设置单元格式为 设备相关像素(DP)
														this->D2DContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (SUCCEEDED(hr))
		{
			//更新所有控件的Factory
			___ConLock.Lock();
			UINT sz = Controls.size();
			if (sz)
			{
				std::map<UINT, DControl*>::iterator conIt = Controls.begin();
				for (UINT i = 0; i < sz; ++i, ++conIt)
				{
					conIt->second->ReleaseDeviceDependentRes();
					conIt->second->Dpi = Dpi;
					conIt->second->_SetFactory(D2DFactory, WicFactory, DWFactory);
					conIt->second->_SetOwnerWndTransparent(TransparWindow);//设置主窗口是否透明
					if (CallChildUpdateDpi)//因为UpdateDpi消耗较大，所以条件判断
						conIt->second->_UpdateDpi();
				}
			}
			___ConLock.Unlock();
		}
		else {
			ReleaseDevice();
			//弹出错误
			wchar_t err[256];
			swprintf_s(err, 256, L"· 创建设备失败 (%#X)\n· 如果系统为 Windows 7，请搜索 \"Platform Update for Windows 7\" 并安装补丁", hr);
			RuntimeErr(err);
		}

		RELEASE(BackBuffer);
		RELEASE(DXGIAdapter);
		RELEASE(DXGIDevice);
		RELEASE(DXGIFactory);
		RELEASE(D2DDevice);
		RELEASE(D3DContext);
		RELEASE(D3DDevice);
		return hr;
	}

	HRESULT Trans_CreateDevice(bool CallChildUpdateDpi = false)
	{
		HRESULT hr;
		UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		ID3D11Device* D3DDevice = nullptr;
		ID3D11DeviceContext* D3DContext = nullptr;
		ID2D1Device* D2DDevice = nullptr;
		IDXGIDevice1* DXGIDevice = nullptr;
		D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0,D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0,D3D_FEATURE_LEVEL_9_3,D3D_FEATURE_LEVEL_9_2,D3D_FEATURE_LEVEL_9_1 }, FeatureLevel;

		//创建 D3D设备
#ifdef _DEBUG//如果为Debug模式，则启用D3D调试层
		Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flags, FeatureLevels, 7, D3D11_SDK_VERSION, &D3DDevice, &FeatureLevel, &D3DContext);
		if (SUCCEEDED(hr))
			HardwareRendering = true;
		else if (hr == DXGI_ERROR_UNSUPPORTED)//不支持GPU渲染，切换CPU渲染
		{
			hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, Flags, FeatureLevels, 7, D3D11_SDK_VERSION, &D3DDevice, &FeatureLevel, &D3DContext);
			if (SUCCEEDED(hr))
				HardwareRendering = false;
		}

		if (SUCCEEDED(hr))
		{
			//通过 D3D设备 获取 DXGI设备
			hr = D3DDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&DXGIDevice);
			if (SUCCEEDED(hr))
			{
				//创建多线程 D2D Factory
#ifdef _DEBUG//如果为Debug模式，则启用D2D调试层
				D2D1_FACTORY_OPTIONS options;
				options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
				hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &options, reinterpret_cast<void**>(&this->D2DFactory));
#else
				hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), reinterpret_cast<void**>(&this->D2DFactory));
#endif
				if (SUCCEEDED(hr))
				{
					//通过 DXGI设备 与 D2D Factory 创建 D2D设备
					hr = this->D2DFactory->CreateDevice(DXGIDevice, &D2DDevice);
					if (SUCCEEDED(hr))
					{
						//通过 D2D设备 创建 D2DContext
						hr = D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &this->D2DContext);
						if (SUCCEEDED(hr))
						{
							if (!Trans_ReCreateCacheSurface())
								hr = E_FAIL;
							if (SUCCEEDED(hr))
							{
								//创建 D2D Bitmap
								D2D1_SIZE_U sz;
								sz.width = rc.right - rc.left;
								sz.height = rc.bottom - rc.top;
								D2D1_BITMAP_PROPERTIES1 prop;
								prop.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
								prop.dpiX = 96.0f;
								prop.dpiY = 96.0f;
								prop.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
								prop.colorContext = nullptr;
								hr = this->D2DContext->CreateBitmap(sz, nullptr, 0, prop, &TargetBmp);
								if (SUCCEEDED(hr))
								{
									//设置绘制目标为 D2D Bitmap
									this->D2DContext->SetTarget(TargetBmp);
									//设置单元格式为 设备相关像素(DP)
									this->D2DContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
								}
							}
						}
					}
				}
			}
		}
		if (SUCCEEDED(hr))
		{
			//更新所有控件的Factory
			___ConLock.Lock();
			UINT sz = Controls.size();
			if (sz)
			{
				std::map<UINT, DControl*>::iterator conIt = Controls.begin();
				for (UINT i = 0; i < sz; ++i, ++conIt)
				{
					conIt->second->ReleaseDeviceDependentRes();
					conIt->second->Dpi = Dpi;
					conIt->second->_SetFactory(D2DFactory, WicFactory, DWFactory);
					conIt->second->_SetOwnerWndTransparent(TransparWindow);//设置主窗口是否透明
					if (CallChildUpdateDpi)//因为UpdateDpi消耗较大，所以条件判断
						conIt->second->_UpdateDpi();
				}
			}
			___ConLock.Unlock();
		}
		else ReleaseDevice();

		RELEASE(DXGIDevice);
		RELEASE(D2DDevice);
		RELEASE(D3DContext);
		RELEASE(D3DDevice);
		return hr;
	}

	inline bool m_SetTransparent(bool transparent)
	{
		if (transparent == TransparWindow)
			return false;
		___RenderMutex.Lock();
		TransparWindow = transparent;
		if (hWnd)
			SetWindowLongW(hWnd, GWL_EXSTYLE, (TransparWindow ? (GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED) : (GetWindowLongW(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED)));
		this->ReleaseDevice();
		HRESULT hr = this->CreateDevice();//Create里会更新子控件的OwnerTransparent
		UpdateAll();//整体更新
		___RenderMutex.Unlock();
		return SUCCEEDED(hr);
	}

	bool Trans_ReCreateCacheSurface()
	{
		if (!CacheDC)
			CacheDC = CreateCompatibleDC(nullptr);
		if (CacheDC)
		{
			if (CacheBmp)
			{
				SelectObject(CacheDC, CacheOldBmp);
				DeleteObject(CacheBmp);
			}

			BITMAPINFO bitmapinfo;
			bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapinfo.bmiHeader.biBitCount = 32;
			bitmapinfo.bmiHeader.biHeight = rc.bottom - rc.top;
			bitmapinfo.bmiHeader.biWidth = rc.right - rc.left;
			bitmapinfo.bmiHeader.biPlanes = 1;
			bitmapinfo.bmiHeader.biCompression = BI_RGB;
			bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
			bitmapinfo.bmiHeader.biYPelsPerMeter = 0;
			bitmapinfo.bmiHeader.biClrUsed = 0;
			bitmapinfo.bmiHeader.biClrImportant = 0;
			bitmapinfo.bmiHeader.biSizeImage = bitmapinfo.bmiHeader.biWidth * bitmapinfo.bmiHeader.biHeight * 4;

			CacheBmp = CreateDIBSection(CacheDC, &bitmapinfo, 0, nullptr, nullptr, 0);
			if (CacheBmp)
			{
				CacheOldBmp = (HBITMAP)SelectObject(CacheDC, CacheBmp);
			}
			else {
				DeleteObject(CacheDC);
				CacheDC = nullptr;
				return false;
			}
		}
		else return false;
		return true;
	}
	inline void Trans_DeleteCacheSurface()
	{
		if (CacheDC && CacheBmp)
		{
			SelectObject(CacheDC, CacheOldBmp);
			DeleteObject(CacheBmp);
			DeleteDC(CacheDC);
			CacheBmp = nullptr;
			CacheDC = nullptr;
		}
		if (CacheBmp)
		{
			DeleteObject(CacheBmp);
			CacheBmp = nullptr;
		}
		if (CacheDC)
		{
			DeleteDC(CacheDC);
			CacheDC = nullptr;
		}
		return;
	}

	void Resize(WPARAM wParam, LPARAM lParam)
	{
		if (wParam == SIZE_MINIMIZED)
			return;
		int wid = max(GET_X(lParam), 1), hei = max(GET_Y(lParam), 1);
		if (rc.right - rc.left == wid && rc.bottom - rc.top == hei)
		{
			UpdateAll();//整体更新
			return;
		}

		___RenderMutex.Lock();
		rc.right = rc.left + wid;
		rc.bottom = rc.top + hei;

		if (D2DContext && (TransparWindow ? true : (bool)SwapChain))
		{
			D2DContext->SetTarget(nullptr);
			RELEASE(TargetBmp);
			HRESULT hr = S_OK;
			if (TransparWindow)
			{
				if (!Trans_ReCreateCacheSurface())
					hr = E_FAIL;
				if (SUCCEEDED(hr))
				{
					//创建 D2D Bitmap
					D2D1_SIZE_U sz;
					sz.width = rc.right - rc.left;
					sz.height = rc.bottom - rc.top;
					D2D1_BITMAP_PROPERTIES1 prop;
					prop.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
					prop.dpiX = 96.0f;
					prop.dpiY = 96.0f;
					prop.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
					prop.colorContext = nullptr;
					hr = this->D2DContext->CreateBitmap(sz, nullptr, 0, prop, &TargetBmp);
					if (SUCCEEDED(hr))
					{
						//设置绘制目标为 D2D Bitmap
						this->D2DContext->SetTarget(TargetBmp);
					}
				}
			}
			else {
				//修改窗口Target大小
				SwapChain->ResizeBuffers(1, (UINT)wid, (UINT)hei, DXGI_FORMAT_UNKNOWN, 0);
				//获取后缓冲区的 DXGI Surface
				IDXGISurface1* BackBuffer;
				hr = SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));
				{
					if (SUCCEEDED(hr))
					{
						//通过后缓冲区 DXGI Surface 创建 D2D Bitmap
						D2D1_BITMAP_PROPERTIES1 prop;
						prop.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
						prop.dpiX = 96.0f;
						prop.dpiY = 96.0f;
						prop.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
						prop.colorContext = nullptr;
						hr = this->D2DContext->CreateBitmapFromDxgiSurface(BackBuffer, &prop, &TargetBmp);
						if (SUCCEEDED(hr))
						{
							//设置绘制目标为 D2D Bitmap
							D2DContext->SetTarget(TargetBmp);
						}
					}
					RELEASE(BackBuffer);
				}
			}
		}
		else {
			this->ReleaseDevice();
			this->CreateDevice();//不处理返回，失败就失败了
		}

		UpdateAll();//整体更新
		_RenderFrame(false);
		___RenderMutex.Unlock();

		DMsgProc p = proc;
		if (p)
			p(hWnd, 0, DControlType::Null, DControlMsg::Window_Size, rc.right - rc.left, rc.bottom - rc.top);
		return;
	}

	inline void DisposeAnimation()
	{
		Shadow.ShadowAnimation();//处理阴影窗口的动画

		if (A_Wnd_X || A_Wnd_Y)
		{
			UINT AID = 0;

			RECT r;
			GetWindowRect(hWnd, &r);
			long wx = r.left, wy = r.top;
			if (A_Wnd_X)
				wx = A_Wnd_X.FrameCalc(AID);
			if (A_Wnd_Y)
				wy = A_Wnd_Y.FrameCalc(AID);

			SetPosition(wx, wy, 0, nullptr, 0, false);
			if (AID)
			{
				//通知给用户
				DMsgProc p = proc;
				if (p)
					p(hWnd, 0, DControlType::Null, DControlMsg::Control_EndOfAnimation, AID, 0);
			}
		}
		return;
	}

	inline bool IsRectWorkable(const D2D1_RECT_F& rc)
	{
		if (rc.left >= rc.right)
			return false;
		if (rc.top >= rc.bottom)
			return false;
		return true;
	}
	inline bool IsRectWorkable(const RECT& rc)
	{
		if (rc.left >= rc.right)
			return false;
		if (rc.top >= rc.bottom)
			return false;
		return true;
	}
	//宏定义的max, min相比STL更快，少了传值的过程，但宏对应参数的值会被计算2遍，所以传入的一定是计算耗时少的。
	inline D2D1_RECT_F CalcRectOverlap(const D2D1_RECT_F& rc1, const D2D1_RECT_F& rc2)
	{
		return { max(rc1.left, rc2.left), max(rc1.top, rc2.top), min(rc1.right, rc2.right), min(rc1.bottom, rc2.bottom) };
	}
	inline D2D1_RECT_F CalcRectUnion(const D2D1_RECT_F& rc1, const D2D1_RECT_F& rc2)
	{
		return { min(rc1.left, rc2.left), min(rc1.top, rc2.top), max(rc1.right, rc2.right), max(rc1.bottom, rc2.bottom) };
	}

	inline bool DetectRectContain(D2D1_RECT_F& Owner, D2D1_RECT_F& Contained)
	{
		if (max(Owner.left, Contained.left) == Contained.left && max(Owner.top, Contained.top) == Contained.top && min(Owner.right, Contained.right) == Contained.right && min(Owner.bottom, Contained.bottom) == Contained.bottom)
			return true;
		else
			return false;
	}
	inline bool DetectRectOverlap(D2D1_RECT_F& rc1, D2D1_RECT_F& rc2)
	{
		if (max(rc1.left, rc2.left) >= min(rc1.right, rc2.right))
			return false;
		if (max(rc1.top, rc2.top) >= min(rc1.bottom, rc2.bottom))
			return false;
		return true;
	}

	//要在ConLock锁里调用才能保证线程安全
	std::map<UINT, DControl*>::iterator Main_HitTest(int xPos, int yPos)
	{
		if (Controls.size())
		{
			//控件捕获状态的处理
			___ActConLock.Lock();
			if (ActiveCon.size() && ActiveCon.begin()->second->OnClick)
			{
				___ActConLock.Unlock();
				return Controls.find(ActiveCon.begin()->second->ID);
			}
			___ActConLock.Unlock();

			std::map<UINT, DControl*>::iterator con = Controls.end();
			--con;
			for (UINT i = Controls.size(); i != 0; --i, --con)
			{
				if (con->second->HitTest(xPos, yPos))
					return con;
			}
		}
		return Controls.end();
	}

	inline bool ActiveControlOnClick()
	{
		___ActConLock.Lock();
		if (ActiveCon.size())
		{
			std::map<UINT, DControl*>::iterator con = ActiveCon.begin();

			while (con != ActiveCon.end())
			{
				if (con->second->OnClick)
				{
					___ActConLock.Unlock();
					return true;
				}
				++con;
			}
		}
		___ActConLock.Unlock();
		return false;
	}
	inline void RestoreLastActiveControl(UINT NowControlID, UINT Msg, bool FromMouUp = false)
	{
		___ActConLock.Lock();
		if (ActiveCon.size())
		{
			std::map<UINT, DControl*>::iterator con = ActiveCon.begin();

			while (con != ActiveCon.end())
			{
				if ((!con->second->OnClick || FromMouUp) && con->second->_SetNormalState(hWnd, Msg, NowControlID))
				{
					std::map<UINT, DControl*>::iterator conNext = con;
					++conNext;

					ActiveCon.erase(con);
					con = conNext;
				}
				else
					++con;
			}
		}
		___ActConLock.Unlock();

		return;
	}

};


//********DXUI自用快速多线程高斯模糊系列函数*********
static void _Gau_CalCoeff(float sigma, float* a0, float* a1, float* a2, float* a3, float* b1, float* b2, float* cprev, float* cnext)
{
	float alpha, lamma, k;

	if (sigma < 0.5f)
		sigma = 0.5f;
	alpha = (float)exp((0.726) * (0.726)) / sigma;
	lamma = (float)exp(-alpha);
	*b2 = (float)exp(-2 * alpha);
	k = (1 - lamma) * (1 - lamma) / (1 + 2 * alpha * lamma - (*b2));
	*a0 = k; *a1 = k * (alpha - 1) * lamma;
	*a2 = k * (alpha + 1) * lamma;
	*a3 = -k * (*b2);
	*b1 = -2 * lamma;
	*cprev = (*a0 + *a1) / (1 + *b1 + *b2);
	*cnext = (*a2 + *a3) / (1 + *b1 + *b2);
	return;
}

static void _Gau_Horizontal(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColumn, int width, int height, int Channels, float a0a1, float a2a3, float b1b2, float cprev, float cnext)
{
	int HeightStep = Channels * height;
	int WidthSubOne = width - 1;
	if (Channels == 3)
	{
		float prevOut[3];
		prevOut[0] = (lpRowInitial[0] * cprev);
		prevOut[1] = (lpRowInitial[1] * cprev);
		prevOut[2] = (lpRowInitial[2] * cprev);
		for (int x = 0; x < width; ++x) {
			prevOut[0] = ((lpRowInitial[0] * (a0a1)) - (prevOut[0] * (b1b2)));
			prevOut[1] = ((lpRowInitial[1] * (a0a1)) - (prevOut[1] * (b1b2)));
			prevOut[2] = ((lpRowInitial[2] * (a0a1)) - (prevOut[2] * (b1b2)));
			bufferPerLine[0] = (unsigned char)prevOut[0];
			bufferPerLine[1] = (unsigned char)prevOut[1];
			bufferPerLine[2] = (unsigned char)prevOut[2];
			bufferPerLine += Channels;
			lpRowInitial += Channels;
		}
		lpRowInitial -= Channels;
		lpColumn += HeightStep * WidthSubOne;
		bufferPerLine -= Channels;
		prevOut[0] = (lpRowInitial[0] * cnext);
		prevOut[1] = (lpRowInitial[1] * cnext);
		prevOut[2] = (lpRowInitial[2] * cnext);

		for (int x = WidthSubOne; x >= 0; --x) {
			prevOut[0] = ((lpRowInitial[0] * (a2a3)) - (prevOut[0] * (b1b2)));
			prevOut[1] = ((lpRowInitial[1] * (a2a3)) - (prevOut[1] * (b1b2)));
			prevOut[2] = ((lpRowInitial[2] * (a2a3)) - (prevOut[2] * (b1b2)));
			bufferPerLine[0] += (unsigned char)prevOut[0];
			bufferPerLine[1] += (unsigned char)prevOut[1];
			bufferPerLine[2] += (unsigned char)prevOut[2];
			lpColumn[0] = bufferPerLine[0];
			lpColumn[1] = bufferPerLine[1];
			lpColumn[2] = bufferPerLine[2];
			lpRowInitial -= Channels;
			lpColumn -= HeightStep;
			bufferPerLine -= Channels;
		}
	}
	else if (Channels == 4)
	{
		float prevOut[4];

		prevOut[0] = (lpRowInitial[0] * cprev);
		prevOut[1] = (lpRowInitial[1] * cprev);
		prevOut[2] = (lpRowInitial[2] * cprev);
		prevOut[3] = (lpRowInitial[3] * cprev);
		for (int x = 0; x < width; ++x) {
			prevOut[0] = ((lpRowInitial[0] * (a0a1)) - (prevOut[0] * (b1b2)));
			prevOut[1] = ((lpRowInitial[1] * (a0a1)) - (prevOut[1] * (b1b2)));
			prevOut[2] = ((lpRowInitial[2] * (a0a1)) - (prevOut[2] * (b1b2)));
			prevOut[3] = ((lpRowInitial[3] * (a0a1)) - (prevOut[3] * (b1b2)));

			bufferPerLine[0] = (unsigned char)prevOut[0];
			bufferPerLine[1] = (unsigned char)prevOut[1];
			bufferPerLine[2] = (unsigned char)prevOut[2];
			bufferPerLine[3] = (unsigned char)prevOut[3];
			bufferPerLine += Channels;
			lpRowInitial += Channels;
		}
		lpRowInitial -= Channels;
		lpColumn += HeightStep * WidthSubOne;
		bufferPerLine -= Channels;

		prevOut[0] = (lpRowInitial[0] * cnext);
		prevOut[1] = (lpRowInitial[1] * cnext);
		prevOut[2] = (lpRowInitial[2] * cnext);
		prevOut[3] = (lpRowInitial[3] * cnext);

		for (int x = WidthSubOne; x >= 0; --x) {
			prevOut[0] = ((lpRowInitial[0] * a2a3) - (prevOut[0] * b1b2));
			prevOut[1] = ((lpRowInitial[1] * a2a3) - (prevOut[1] * b1b2));
			prevOut[2] = ((lpRowInitial[2] * a2a3) - (prevOut[2] * b1b2));
			prevOut[3] = ((lpRowInitial[3] * a2a3) - (prevOut[3] * b1b2));
			bufferPerLine[0] += (unsigned char)prevOut[0];
			bufferPerLine[1] += (unsigned char)prevOut[1];
			bufferPerLine[2] += (unsigned char)prevOut[2];
			bufferPerLine[3] += (unsigned char)prevOut[3];
			lpColumn[0] = bufferPerLine[0];
			lpColumn[1] = bufferPerLine[1];
			lpColumn[2] = bufferPerLine[2];
			lpColumn[3] = bufferPerLine[3];
			lpRowInitial -= Channels;
			lpColumn -= HeightStep;
			bufferPerLine -= Channels;
		}
	}
	else if (Channels == 1)
	{
		float prevOut = (lpRowInitial[0] * cprev);

		for (int x = 0; x < width; ++x) {
			prevOut = ((lpRowInitial[0] * (a0a1)) - (prevOut * (b1b2)));
			bufferPerLine[0] = (unsigned char)prevOut;
			bufferPerLine += Channels;
			lpRowInitial += Channels;
		}
		lpRowInitial -= Channels;
		lpColumn += HeightStep * WidthSubOne;
		bufferPerLine -= Channels;

		prevOut = (lpRowInitial[0] * cnext);

		for (int x = WidthSubOne; x >= 0; --x) {
			prevOut = ((lpRowInitial[0] * a2a3) - (prevOut * b1b2));
			bufferPerLine[0] += (unsigned char)prevOut;
			lpColumn[0] = bufferPerLine[0];
			lpRowInitial -= Channels;
			lpColumn -= HeightStep;
			bufferPerLine -= Channels;
		}
	}
	return;
}

static void _Gau_Vertical(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColInitial, int height, int width, int Channels, float a0a1, float a2a3, float b1b2, float cprev, float cnext)
{
	int WidthStep = Channels * width;
	int HeightSubOne = height - 1;
	if (Channels == 3)
	{
		float prevOut[3];
		prevOut[0] = (lpRowInitial[0] * cprev);
		prevOut[1] = (lpRowInitial[1] * cprev);
		prevOut[2] = (lpRowInitial[2] * cprev);

		for (int y = 0; y < height; ++y) {
			prevOut[0] = ((lpRowInitial[0] * a0a1) - (prevOut[0] * b1b2));
			prevOut[1] = ((lpRowInitial[1] * a0a1) - (prevOut[1] * b1b2));
			prevOut[2] = ((lpRowInitial[2] * a0a1) - (prevOut[2] * b1b2));
			bufferPerLine[0] = (unsigned char)prevOut[0];
			bufferPerLine[1] = (unsigned char)prevOut[1];
			bufferPerLine[2] = (unsigned char)prevOut[2];
			bufferPerLine += Channels;
			lpRowInitial += Channels;
		}
		lpRowInitial -= Channels;
		bufferPerLine -= Channels;
		lpColInitial += WidthStep * HeightSubOne;
		prevOut[0] = (lpRowInitial[0] * cnext);
		prevOut[1] = (lpRowInitial[1] * cnext);
		prevOut[2] = (lpRowInitial[2] * cnext);
		for (int y = HeightSubOne; y >= 0; --y) {
			prevOut[0] = ((lpRowInitial[0] * a2a3) - (prevOut[0] * b1b2));
			prevOut[1] = ((lpRowInitial[1] * a2a3) - (prevOut[1] * b1b2));
			prevOut[2] = ((lpRowInitial[2] * a2a3) - (prevOut[2] * b1b2));
			bufferPerLine[0] += (unsigned char)prevOut[0];
			bufferPerLine[1] += (unsigned char)prevOut[1];
			bufferPerLine[2] += (unsigned char)prevOut[2];
			lpColInitial[0] = bufferPerLine[0];
			lpColInitial[1] = bufferPerLine[1];
			lpColInitial[2] = bufferPerLine[2];
			lpRowInitial -= Channels;
			lpColInitial -= WidthStep;
			bufferPerLine -= Channels;
		}
	}
	else if (Channels == 4)
	{
		float prevOut[4];

		prevOut[0] = (lpRowInitial[0] * cprev);
		prevOut[1] = (lpRowInitial[1] * cprev);
		prevOut[2] = (lpRowInitial[2] * cprev);
		prevOut[3] = (lpRowInitial[3] * cprev);

		for (int y = 0; y < height; ++y) {
			prevOut[0] = ((lpRowInitial[0] * a0a1) - (prevOut[0] * b1b2));
			prevOut[1] = ((lpRowInitial[1] * a0a1) - (prevOut[1] * b1b2));
			prevOut[2] = ((lpRowInitial[2] * a0a1) - (prevOut[2] * b1b2));
			prevOut[3] = ((lpRowInitial[3] * a0a1) - (prevOut[3] * b1b2));
			bufferPerLine[0] = (unsigned char)prevOut[0];
			bufferPerLine[1] = (unsigned char)prevOut[1];
			bufferPerLine[2] = (unsigned char)prevOut[2];
			bufferPerLine[3] = (unsigned char)prevOut[3];
			bufferPerLine += Channels;
			lpRowInitial += Channels;
		}
		lpRowInitial -= Channels;
		bufferPerLine -= Channels;
		lpColInitial += WidthStep * HeightSubOne;
		prevOut[0] = (lpRowInitial[0] * cnext);
		prevOut[1] = (lpRowInitial[1] * cnext);
		prevOut[2] = (lpRowInitial[2] * cnext);
		prevOut[3] = (lpRowInitial[3] * cnext);
		for (int y = HeightSubOne; y >= 0; --y) {
			prevOut[0] = ((lpRowInitial[0] * a2a3) - (prevOut[0] * b1b2));
			prevOut[1] = ((lpRowInitial[1] * a2a3) - (prevOut[1] * b1b2));
			prevOut[2] = ((lpRowInitial[2] * a2a3) - (prevOut[2] * b1b2));
			prevOut[3] = ((lpRowInitial[3] * a2a3) - (prevOut[3] * b1b2));
			bufferPerLine[0] += (unsigned char)prevOut[0];
			bufferPerLine[1] += (unsigned char)prevOut[1];
			bufferPerLine[2] += (unsigned char)prevOut[2];
			bufferPerLine[3] += (unsigned char)prevOut[3];
			lpColInitial[0] = bufferPerLine[0];
			lpColInitial[1] = bufferPerLine[1];
			lpColInitial[2] = bufferPerLine[2];
			lpColInitial[3] = bufferPerLine[3];
			lpRowInitial -= Channels;
			lpColInitial -= WidthStep;
			bufferPerLine -= Channels;
		}
	}
	else if (Channels == 1)
	{
		float prevOut = 0;
		prevOut = (lpRowInitial[0] * cprev);
		for (int y = 0; y < height; ++y) {
			prevOut = ((lpRowInitial[0] * a0a1) - (prevOut * b1b2));
			bufferPerLine[0] = (unsigned char)prevOut;
			bufferPerLine += Channels;
			lpRowInitial += Channels;
		}
		lpRowInitial -= Channels;
		bufferPerLine -= Channels;
		lpColInitial += WidthStep * HeightSubOne;
		prevOut = (lpRowInitial[0] * cnext);
		for (int y = HeightSubOne; y >= 0; --y) {
			prevOut = ((lpRowInitial[0] * a2a3) - (prevOut * b1b2));
			bufferPerLine[0] += (unsigned char)prevOut;
			lpColInitial[0] = bufferPerLine[0];
			lpRowInitial -= Channels;
			lpColInitial -= WidthStep;
			bufferPerLine -= Channels;
		}
	}
	return;
}

static void _Gau_Blur(BYTE* src, UINT Width, UINT Height, UINT Stride, float GaussianSigma, UINT Threads)
{
	UINT Channels = Stride / Width;
	float a0, a1, a2, a3, b1, b2, cprev, cnext;
	_Gau_CalCoeff(GaussianSigma, &a0, &a1, &a2, &a3, &b1, &b2, &cprev, &cnext);

	float a0a1 = a0 + a1;
	float a2a3 = a2 + a3;
	float b1b2 = b1 + b2;

	unsigned char* tempData = (unsigned char*)malloc(Height * Stride);
	if (!tempData)
		return;
	//开始多线程计算
	UINT threads = min(min(32, max(1, Threads)), min(Width, Height));//如果宽或高小于线程数，则设为最小值
	UINT nT = 0;
	UINT FinishStep1 = threads;

	UINT deltax = Width / threads;
	UINT deltay = Height / threads;
	UINT LastWid = 0, LastHei = 0;

	_GaussianThreadParam* p = new _GaussianThreadParam[threads];
	if (!p)
		return;

	for (UINT i = 0; i < threads; ++i)
	{
		p[i].a0a1 = a0a1;
		p[i].a2a3 = a2a3;
		p[i].b1b2 = b1b2;
		p[i].cprev = cprev;
		p[i].cnext = cnext;
		p[i].Channels = Channels;
		p[i].Stride = Stride;
		p[i].src = src;
		p[i].nT = &nT;
		p[i].FinishStep1 = &FinishStep1;
		p[i].tempData = tempData;


		p[i].x = LastWid;
		p[i].y = LastHei;
		p[i].Width = deltax;
		p[i].Height = deltay;
		p[i].FullWidth = Width;
		p[i].FullHeight = Height;

		if (i == threads - 1)
		{
			p[i].Width = Width - p[i].x;
			p[i].Height = Height - p[i].y;
		}
		if (_beginthread(___DxLibGaussianBlurThread, 0, &p[i]) != -1)
			InterlockedIncrement(&nT);

		LastWid = p[i].x + p[i].Width;
		LastHei = p[i].y + p[i].Height;
	}
	while (nT)
		Sleep(1);

	free(tempData);
	delete[] p;

	return;
}

static void __cdecl ___DxLibGaussianBlurThread(void* param)
{
	if (!param)
		return;

	_GaussianThreadParam* p = (_GaussianThreadParam*)param;

	unsigned char* bufferPerLine = (unsigned char*)malloc((p->FullWidth > p->FullHeight ? p->FullWidth : p->FullHeight) * p->Channels);
	if (!bufferPerLine)
	{
		InterlockedDecrement(p->nT);
		return;
	}

	unsigned char* lpColInitial;
	unsigned char* lpRowInitial;

	for (UINT y = p->y; y < p->y + p->Height; ++y)
	{
		lpRowInitial = p->src + p->Stride * y;
		lpColInitial = p->tempData + y * p->Channels;
		_Gau_Horizontal(bufferPerLine, lpRowInitial, lpColInitial, p->FullWidth, p->FullHeight, p->Channels, p->a0a1, p->a2a3, p->b1b2, p->cprev, p->cnext);
	}
	//等待其余线程执行完上面代码
	InterlockedDecrement(p->FinishStep1);
	while ((*(p->FinishStep1)))
		Sleep(1);

	for (UINT x = p->x; x < p->x + p->Width; ++x)
	{
		lpColInitial = p->src + x * p->Channels;
		lpRowInitial = p->tempData + p->FullHeight * p->Channels * x;
		_Gau_Vertical(bufferPerLine, lpRowInitial, lpColInitial, p->FullHeight, p->FullWidth, p->Channels, p->a0a1, p->a2a3, p->b1b2, p->cprev, p->cnext);
	}

	free(bufferPerLine);
	InterlockedDecrement(p->nT);
	return;
}


//**********DXUI渲染线程**********
inline void __cdecl DXUI_RenderThread(void* param)
{
	if (param)
	{
		if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
		{
			CoUninitialize();
			ThrowErr(L"COM组件初始化错误！");
			return;
		}

		DWindow* ui = (DWindow*)param;
		ui->_SetRenderThreadAlive(true);//将线程存活状态设置为true

		DWORD LastFrameTime = timeGetTime();
		DWORD timePhase = 0;
		DWORD Delay;

		bool succ = timeBeginPeriod(1);
		while (!ui->_GetExitRenderThreadFlag())
		{
			Delay = ui->GetFrameDealy();

			ui->_RenderFrame();

			timePhase = timeGetTime() - LastFrameTime + 1;
			if (timePhase < Delay)//实际耗费的时间<每帧的时间
			{
				Sleep(Delay - timePhase);//将剩余的时间等待
			}
			LastFrameTime = timeGetTime();//在这里调用可以将循环耗费的时间也算进去
		}
		if (succ)
			timeEndPeriod(1);

		ui->_SetRenderThreadAlive(false);
		CoUninitialize();
	}
	return;
}

#endif//_DXUI_LIBRARY
