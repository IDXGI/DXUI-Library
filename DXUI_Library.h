#pragma once
#ifndef _DXUI_LIBRARY
#define _DXUI_LIBRARY
/******************************************************************

	DXUI界面库
	版本: 见下方常量 DXUI_VERSION
	作者: YZ
	QQ: 2441987560
	WeChat: yangzhen2441987560
	Email: 2441987560@qq.com
	Github: https://github.com/IDXGI/DXUI-Library

	此库来之不易，使用请标明。维护良好的开源环境。

	>>>>>>>>>>>>>>>>>>>>>>>>>>【 注  意 】<<<<<<<<<<<<<<<<<<<<<<<<<<<
	1. DXUI 支持 多字节字符串 和 Unicode字符串，参数可以接收两种字符集的文本。但强烈建议原生使用 Unicode 字符集，以避免内部转换带来的内存开销

*******************************************************************/
#define DXUI_VERSION 220126//DXUI版本

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

#include <process.h>//_beginthread, _endthread，setTimerEvent等
#include <Mmsystem.h>

#include <tchar.h>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <string>
#include <deque>
#include <vector>
#include <map>

#define RELEASE(P)     if(P){ (P)->Release(); (P) = nullptr; }
#define SafeDelete(P)  if(P){ delete (P); (P) = nullptr; }
#define DeleteArray(P) if(P){ delete[] (P); (P) = nullptr; }
#define GET_X(lp)      ((int)(short)LOWORD(lp))
#define GET_Y(lp)      ((int)(short)HIWORD(lp))
#define DIP2DP(P)      ((P) * (float)Dpi / 96.0f)
#define DP2DIP(P)      ((P) * 96.0f / (float)Dpi)
#ifndef Max
#define Max(a,b)       (((a) > (b)) ? (a) : (b))
#endif
#ifndef Min
#define Min(a,b)       (((a) < (b)) ? (a) : (b))
#endif

#ifdef _DEBUG//Debug模式定义
//调试输出宏
#define DP0(fmt) {wchar_t sOut[256];swprintf_s(sOut,fmt);OutputDebugStringW(sOut);}
#define DP1(fmt,var) {wchar_t sOut[256];swprintf_s(sOut,fmt,var);OutputDebugStringW(sOut);}
#define DP2(fmt,var1,var2) {wchar_t sOut[256];swprintf_s(sOut,fmt,var1,var2);OutputDebugStringW(sOut);}
#define DP3(fmt,var1,var2,var3) {wchar_t sOut[256];swprintf_s(sOut,fmt,var1,var2,var3);OutputDebugStringW(sOut);}
#define DP4(fmt,var1,var2,var3,var4) {wchar_t sOut[256];swprintf_s(sOut,fmt,var1,var2,var3,var4);OutputDebugStringW(sOut);}
#define DPRC(var) {wchar_t sOut[256];swprintf_s(sOut,L"%.2f, %.2f, %.2f, %.2f\n",(var).left,(var).top,(var).right,(var).bottom);OutputDebugStringW(sOut);}
#else//Release模式无实际作用
//调试输出宏
#define DP0(fmt)
#define DP1(fmt,var)
#define DP2(fmt,var1,var2)
#define DP3(fmt,var1,var2,var3)
#define DP4(fmt,var1,var2,var3,var4)
#define DPRC(var)
#endif

#define DefFrameRate              60//默认帧率，不可超过60
#define RegisterMessageStr        L"DXUIControlMsg"
#define DefUserNamedType          L"Undefined"
#define DefFontName               L"Microsoft YaHei UI"
#define LocaleName                L"zh-cn"
#define SmoothScrollTime          180//文本框的平滑滚动时间(ms)
#define TextBox_TextEdgeIn        DIP2DP(6.0f)//文本框的文本显示范围收缩量
#define TextBox_CaretMoveTime     120//文本框的插入符平滑移动时间(ms)
#define TextBox_CaretGradientTime 150//文本框的插入符渐显渐隐时间(ms)
#define Shadow_StrokeWidth        DIP2DP(15)
#define Size_DetectEdge           DIP2DP(6.0f)

//DXUI自用Windows消息处理回调函数
inline LRESULT CALLBACK ___DXUIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//DXUI用于循环渲染每一帧的线程
inline void __cdecl DXUI_RenderThread(void* param);

//自用多线程高斯模糊
inline void _Gau_Blur(BYTE* src, UINT Width, UINT Height, UINT Stride, float GaussianSigma, UINT Threads);
inline void _Gau_Vertical(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColInitial, int height, int width, int Channels, float a0a1, float a2a3, float b1b2, float cprev, float cnext);
inline void _Gau_Horizontal(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColumn, int width, int height, int Channels, float a0a1, float a2a3, float b1b2, float cprev, float cnext);
inline void _Gau_CalCoeff(float sigma, float* a0, float* a1, float* a2, float* a3, float* b1, float* b2, float* cprev, float* cnext);
//自用高斯模糊计算线程
inline void __cdecl ___DxLibGaussianBlurThread(void* param);

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

//DXUI提供的默认动画的贝塞尔曲线参数，适用于渐入或渐出
inline const DBezier Bezier_Def = { 0.19f, 1.0f, 0.22f, 1.0f };
//平滑滚动贝塞尔曲线参数
inline const DBezier Bezier_SmoothScroll = { 0.5f, 1.0f, 0.89f, 1.0f };
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

	///<summary>
	///裁剪区域类型
	///</summary>
	ClipRect,

	///<summary>
	///组合控件类型
	///</summary>
	CombinedCon,
};
///<summary>
///控件消息
///</summary>
enum class DControlMsg
{
	///<summary>
	///返回Null代表此消息和DXUI控件无关，可能是原始Windows消息，Wnd_Msg、Wnd_lParam、Wnd_wParam是原始Win消息参数
	///</summary>
	Null,

	Window_Size,//wParam是新的窗口宽度(long),lParam是新的窗口高度(long),均为设备相关像素(DP)
	Window_Position,//wParam是新的窗口x坐标(long),lParam是新的窗口y坐标(long),均为设备相关像素(DP)
	Window_Destroy,//窗口被销毁时，DWindow类Release之前，会传递此消息

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

//抛出错误 (使用宏而不是直接使用下面的函数)
#ifdef _DEBUG
#define _DXUI_DBG_ERROR(ErrCode) _DXUI_ThrowErr_Code(ErrCode, true)
#define _DXUI_DBG_ASSERT(Expression, ErrCode) if(Expression)_DXUI_DBG_ERROR(ErrCode)
#else
#define _DXUI_DBG_ERROR(ErrCode)
#define _DXUI_DBG_ASSERT(Expression, ErrCode)
#endif

#define _DXUI_ERROR(ErrCode) _DXUI_ThrowErr_Code(ErrCode, false)
#define _DXUI_ASSERT(Expression, ErrCode) if(Expression)_DXUI_ERROR(ErrCode)

inline void _DXUI_ThrowErr_Code(UINT Code, bool OnlyDbg) noexcept
{
	std::wstring Reason, Describe;
	bool Critical;
	switch (Code)
	{
	case 1001:
	{
		Reason = L"加载 DLL 模块失败";
		Describe = L"请检查系统是否缺失 DLL。";
		Critical = true;
		break;
	}
	case 1002:
	{
		Reason = L"COM 组件初始化失败";
		Describe = L"请检查系统是否完整。";
		Critical = true;
		break;
	}
	case 1003:
	{
		Reason = L"创建窗口类失败";
		Describe = L"请确认传入参数是否正确。";
		Critical = true;
		break;
	}
	case 1004:
	{
		Reason = L"创建窗口失败";
		Describe = L"请确认传入参数是否正确。";
		Critical = true;
		break;
	}
	case 1005:
	{
		Reason = L"创建 Direct 设备失败";
		Describe = L"如果系统为 Windows 7，请搜索 \"Platform Update for Windows 7\" 并安装补丁。";
		Critical = true;
		break;
	}
	case 1006:
	{
		Reason = L"创建 DWrite 组件失败";
		Describe = L"请检查系统完整性。";
		Critical = true;
		break;
	}
	case 1007:
	{
		Reason = L"创建 WIC 组件失败";
		Describe = L"请检查系统完整性。";
		Critical = true;
		break;
	}
	case 1008:
	{
		Reason = L"设置高DPI感知失败";
		Describe = L"程序可能在高分辨率屏幕下被系统缩放，导致模糊。请在程序清单内设置[ 每个监视器高 DPI 识别 ]来替代运行时设置。";
		Critical = false;
		break;
	}
	case 1009:
	{
		Reason = L"无效的参数";
		Describe = L"颜色参数不正确，请检查参数后重试。";
		Critical = false;
		break;
	}
	case 1010:
	{
		Reason = L"DImageBox从路径加载文件失败";
		Describe = L"请检查文件是否存在。如果为 Visual Studio 调试时，请注意调试目录与程序目录不一致，可能导致找不到文件。";
		Critical = false;
		break;
	}
	case 1011:
	{
		Reason = L"DImageBox从资源加载文件失败";
		Describe = L"请检查资源是否存在。";
		Critical = false;
		break;
	}
	case 1012:
	{
		Reason = L"DImageBox从内存加载文件失败";
		Describe = L"请检查图片数据是否正确。";
		Critical = false;
		break;
	}
	case 1013:
	{
		Reason = L"DGifBox从路径加载文件失败";
		Describe = L"请检查文件是否存在。如果为 Visual Studio 调试时，请注意调试目录与程序目录不一致，可能导致找不到文件。";
		Critical = false;
		break;
	}
	case 1014:
	{
		Reason = L"DGifBox从资源加载文件失败";
		Describe = L"请检查资源是否存在。";
		Critical = false;
		break;
	}
	case 1015:
	{
		Reason = L"渲染线程已意外退出";
		Describe = L"原因为 DXUI 未正确加载或 COM 组件初始化失败。";
		Critical = true;
		break;
	}
	case 1016:
	{
		Reason = L"无效的参数";
		Describe = L"DTextBox 中，如果设置了多行显示，那么一定是水平左对齐、垂直上对齐。";
		Critical = false;
		break;
	}
	case 1017:
	{
		Reason = L"无效的参数";
		Describe = L"DTextBox 中，如果设置了单行显示，那么一定不能设置自动换行。";
		Critical = false;
		break;
	}

	default://缺省时
		Reason = L"未定义的错误代码";
		Describe = L"错误代码未知，请确认是否为内部代码错误。";
		Critical = false;
		break;
	}

	//格式化文本，弹窗
	wchar_t err[512];
	swprintf_s(err, 512, L"%s%s%s (%#X)\n● %s", OnlyDbg ? L"● 仅调试时提示: \n" : L"", Critical ? L"● 严重错误: " : L"● ", Reason.c_str(), Code, Describe.c_str());
	MessageBoxW(nullptr, err, L"[ DXUI ERROR ]", MB_OK | MB_ICONERROR);
	return;
}

//检测颜色是否在正确范围
#define DetectColor(Color) _DXUI_DBG_ASSERT(Color.r > 255 || Color.g > 255 || Color.b > 255 || Color.a > 1.0f || Color.a < 0.0f, 1009)

//动态调用函数定义

//GetDpiForMonitor
enum MONITOR_DPI_TYPE
{
	MDT_EFFECTIVE_DPI = 0,
	MDT_ANGULAR_DPI = 1,
	MDT_RAW_DPI = 2,
	MDT_DEFAULT = MDT_EFFECTIVE_DPI
};
typedef HRESULT(WINAPI* PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
inline PFN_GetDpiForMonitor pGetDpiForMonitor = nullptr;
//SetProcessDpiAwareness
enum PROCESS_DPI_AWARENESS
{
	PROCESS_DPI_UNAWARE,
	PROCESS_SYSTEM_DPI_AWARE,
	PROCESS_PER_MONITOR_DPI_AWARE
};
typedef HRESULT(WINAPI* PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
inline PFN_SetProcessDpiAwareness pSetProcessDpiAwareness = nullptr;
//D2D1CreateFactory
typedef HRESULT(WINAPI* PFN_D2D1CreateFactory)(D2D1_FACTORY_TYPE, REFIID, CONST D2D1_FACTORY_OPTIONS*, void** ppIFactory);
inline PFN_D2D1CreateFactory pD2D1CreateFactory = nullptr;
//D2D1MakeRotateMatrix
typedef HRESULT(WINAPI* PFN_D2D1MakeRotateMatrix)(FLOAT, D2D1_POINT_2F, D2D1_MATRIX_3X2_F*);
inline PFN_D2D1MakeRotateMatrix pD2D1MakeRotateMatrix = nullptr;
//AlphaBlend
typedef BOOL(WINAPI* PFN_AlphaBlend)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
inline PFN_AlphaBlend pAlphaBlend = nullptr;
//CoInitializeEx
typedef HRESULT(WINAPI* PFN_CoInitializeEx)(LPVOID, DWORD);
inline PFN_CoInitializeEx pCoInitializeEx = nullptr;
//CoUninitialize
typedef void(WINAPI* PFN_CoUninitialize)();
inline PFN_CoUninitialize pCoUninitialize = nullptr;
//CoCreateInstance
typedef HRESULT(WINAPI* PFN_CoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
inline PFN_CoCreateInstance pCoCreateInstance = nullptr;
//DWriteCreateFactory
typedef HRESULT(WINAPI* PFN_DWriteCreateFactory)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
inline PFN_DWriteCreateFactory pDWriteCreateFactory = nullptr;
//D3D11CreateDevice
typedef HRESULT(WINAPI* PFN_D3D11CreateDevice)\
(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
inline PFN_D3D11CreateDevice pD3D11CreateDevice = nullptr;
//timeBeginPeriod
typedef MMRESULT(WINAPI* PFN_timeBeginPeriod)(UINT);
inline PFN_timeBeginPeriod ptimeBeginPeriod = nullptr;
//timeGetTime
typedef DWORD(WINAPI* PFN_timeGetTime)();
inline PFN_timeGetTime ptimeGetTime = nullptr;
//timeEndPeriod
typedef MMRESULT(WINAPI* PFN_timeEndPeriod)(UINT);
inline PFN_timeEndPeriod ptimeEndPeriod = nullptr;
//ImmAssociateContext
typedef HIMC(WINAPI* PFN_ImmAssociateContext)(HWND, HIMC);
inline PFN_ImmAssociateContext pImmAssociateContext = nullptr;
//ImmSetOpenStatus
typedef BOOL(WINAPI* PFN_ImmSetOpenStatus)(HIMC, BOOL);
inline PFN_ImmSetOpenStatus pImmSetOpenStatus = nullptr;
//ImmAssociateContextEx
typedef BOOL(WINAPI* PFN_ImmAssociateContextEx)(HWND, HIMC, DWORD);
inline PFN_ImmAssociateContextEx pImmAssociateContextEx = nullptr;
//ImmGetCompositionStringW
typedef LONG(WINAPI* PFN_ImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
inline PFN_ImmGetCompositionStringW pImmGetCompositionStringW = nullptr;
//ImmSetCompositionWindow
typedef BOOL(WINAPI* PFN_ImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
inline PFN_ImmSetCompositionWindow pImmSetCompositionWindow = nullptr;
//ImmReleaseContext
typedef BOOL(WINAPI* PFN_ImmReleaseContext)(HWND, HIMC);
inline PFN_ImmReleaseContext pImmReleaseContext = nullptr;
//ImmGetContext
typedef HIMC(WINAPI* PFN_ImmGetContext)(HWND);
inline PFN_ImmGetContext pImmGetContext = nullptr;

//是否成功初始化
inline char _DXUI_SuccessInit = 1;//0=初始化成功, 1=还未初始化, 2=初始化失败
//每次运行需要初始化的函数
inline void _DXUI_Init()
{
	if (_DXUI_SuccessInit != 1)
		return;
	HINSTANCE hInstDLL;

	hInstDLL = LoadLibraryW(L"Shcore.dll");
	if (hInstDLL)//不可以检查这两个函数，因为低版本Windows没有
	{
		pGetDpiForMonitor = (PFN_GetDpiForMonitor)GetProcAddress(hInstDLL, "GetDpiForMonitor");
		pSetProcessDpiAwareness = (PFN_SetProcessDpiAwareness)GetProcAddress(hInstDLL, "SetProcessDpiAwareness");
	}
	hInstDLL = LoadLibraryW(L"D2d1.dll");
	if (hInstDLL)
	{
		pD2D1CreateFactory = (PFN_D2D1CreateFactory)GetProcAddress(hInstDLL, "D2D1CreateFactory");
		pD2D1MakeRotateMatrix = (PFN_D2D1MakeRotateMatrix)GetProcAddress(hInstDLL, "D2D1MakeRotateMatrix");
	}
	hInstDLL = LoadLibraryW(L"Msimg32.dll");
	if (hInstDLL)
	{
		pAlphaBlend = (PFN_AlphaBlend)GetProcAddress(hInstDLL, "AlphaBlend");
	}
	hInstDLL = LoadLibraryW(L"Ole32.dll");
	if (hInstDLL)
	{
		pCoInitializeEx = (PFN_CoInitializeEx)GetProcAddress(hInstDLL, "CoInitializeEx");
		pCoUninitialize = (PFN_CoUninitialize)GetProcAddress(hInstDLL, "CoUninitialize");
		pCoCreateInstance = (PFN_CoCreateInstance)GetProcAddress(hInstDLL, "CoCreateInstance");
	}
	hInstDLL = LoadLibraryW(L"Dwrite.dll");
	if (hInstDLL)
	{
		pDWriteCreateFactory = (PFN_DWriteCreateFactory)GetProcAddress(hInstDLL, "DWriteCreateFactory");
	}
	hInstDLL = LoadLibraryW(L"D3D11.dll");
	if (hInstDLL)
	{
		pD3D11CreateDevice = (PFN_D3D11CreateDevice)GetProcAddress(hInstDLL, "D3D11CreateDevice");
	}
	hInstDLL = LoadLibraryW(L"Winmm.dll");
	if (hInstDLL)
	{
		ptimeBeginPeriod = (PFN_timeBeginPeriod)GetProcAddress(hInstDLL, "timeBeginPeriod");
		ptimeGetTime = (PFN_timeGetTime)GetProcAddress(hInstDLL, "timeGetTime");
		ptimeEndPeriod = (PFN_timeEndPeriod)GetProcAddress(hInstDLL, "timeEndPeriod");
	}
	hInstDLL = LoadLibraryW(L"Imm32.dll");
	if (hInstDLL)
	{
		pImmAssociateContext = (PFN_ImmAssociateContext)GetProcAddress(hInstDLL, "ImmAssociateContext");
		pImmSetOpenStatus = (PFN_ImmSetOpenStatus)GetProcAddress(hInstDLL, "ImmSetOpenStatus");
		pImmAssociateContextEx = (PFN_ImmAssociateContextEx)GetProcAddress(hInstDLL, "ImmAssociateContextEx");
		pImmGetCompositionStringW = (PFN_ImmGetCompositionStringW)GetProcAddress(hInstDLL, "ImmGetCompositionStringW");
		pImmSetCompositionWindow = (PFN_ImmSetCompositionWindow)GetProcAddress(hInstDLL, "ImmSetCompositionWindow");
		pImmReleaseContext = (PFN_ImmReleaseContext)GetProcAddress(hInstDLL, "ImmReleaseContext");
		pImmGetContext = (PFN_ImmGetContext)GetProcAddress(hInstDLL, "ImmGetContext");
	}
	if (pD2D1CreateFactory && pD2D1MakeRotateMatrix && pAlphaBlend && pCoInitializeEx && pCoUninitialize && pCoCreateInstance && pDWriteCreateFactory && pD3D11CreateDevice && ptimeBeginPeriod && ptimeGetTime && ptimeEndPeriod && pImmAssociateContext && pImmSetOpenStatus && pImmAssociateContextEx && pImmGetCompositionStringW && pImmSetCompositionWindow && pImmReleaseContext && pImmGetContext)
		_DXUI_SuccessInit = 0;

	_DXUI_ASSERT(_DXUI_SuccessInit != 0, 1001);
	return;
}
//由于新API与老版系统不兼容，自己实现获取DPI
inline int _DXUI_GetWndDpi(const HWND& hWnd)
{
	UINT dpi = 96;
	bool res = true;
	if (hWnd)
	{
		if (!pGetDpiForMonitor || FAILED(pGetDpiForMonitor(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &dpi, &dpi)))
			res = false;
	}
	else res = false;
	if (!res)//老版本系统实现方法
		dpi = (UINT)GetDeviceCaps(GetDC(nullptr), LOGPIXELSX);
	if (dpi > 1200)
		dpi = 96;

	return (int)dpi;
}

///<summary>
/// DXUI提供的用来快速构建消息循环的函数
///</summary>
inline WPARAM DXUI_MsgLoop() noexcept
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

inline bool _DXUI_IsRectWork(const D2D1_RECT_F& rc, const bool& strict = true) noexcept
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

inline bool _DXUI_IsRectWork(const RECT& rc, const bool& strict = true) noexcept
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

//计算两个矩形的交集
inline D2D1_RECT_F _DXUI_CalcRectOverlap(const D2D1_RECT_F& rc1, const D2D1_RECT_F& rc2) noexcept
{
	if (!_DXUI_IsRectWork(rc1))
		return rc1;
	if (!_DXUI_IsRectWork(rc2))
		return rc2;
	return { Max(rc1.left, rc2.left), Max(rc1.top, rc2.top), Min(rc1.right, rc2.right), Min(rc1.bottom, rc2.bottom) };
}
//计算两个矩形的并集
inline D2D1_RECT_F _DXUI_CalcRectUnion(const D2D1_RECT_F& rc1, const D2D1_RECT_F& rc2) noexcept
{
	return { Min(rc1.left, rc2.left), Min(rc1.top, rc2.top), Max(rc1.right, rc2.right), Max(rc1.bottom, rc2.bottom) };
}
//检测两个矩形是否包含
inline bool _DXUI_DetectRectContain(const D2D1_RECT_F& Owner, const D2D1_RECT_F& Contained) noexcept
{
	if (Max(Owner.left, Contained.left) == Contained.left && Max(Owner.top, Contained.top) == Contained.top && Min(Owner.right, Contained.right) == Contained.right && Min(Owner.bottom, Contained.bottom) == Contained.bottom)
		return true;
	else
		return false;
}
//检测两个矩形是否相交
inline bool _DXUI_DetectRectOverlap(const D2D1_RECT_F& rc1, const D2D1_RECT_F& rc2) noexcept
{
	if (Max(rc1.left, rc2.left) >= Min(rc1.right, rc2.right))
		return false;
	if (Max(rc1.top, rc2.top) >= Min(rc1.bottom, rc2.bottom))
		return false;
	return true;
}

///<summary>
/// DXUI用来保存文本的类，传递参数使用，支持宽字符、窄字符相互转化
///</summary>
class DString
{
public:
	operator std::wstring()
	{
		if (wstr.size())
			return wstr;
		else if (str.size())
		{
			wstr = c2w(str);
			return wstr;
		}
		else
			return L"";
	}
	DString()
	{
		return;
	}
	DString(const std::wstring WideChar) noexcept
	{
		wstr = WideChar;
		str.clear();
	}
	DString(const std::string MultiByte) noexcept
	{
		str = MultiByte;
		wstr = c2w(str);
	}
	DString(const wchar_t* const WideChar) noexcept
	{
		wstr = WideChar;
		str.clear();
	}
	DString(const char* const MultiByte) noexcept
	{
		str = MultiByte;
		wstr = c2w(str);
	}
	//不要在此方法返回时使用string的c_str()，因为临时对象已销毁
	const std::string GetMultiByte() const noexcept
	{
		if (str.size())
			return str;
		else if (wstr.size())
		{
			str = w2c(wstr);
			return str;
		}
		else
			return "";
	}
	//不要在此方法返回时使用wstring的c_str()，因为临时对象已销毁
	const std::wstring GetWideChar() const noexcept
	{
		if (wstr.size())
			return wstr;
		else if (str.size())
		{
			wstr = c2w(str);
			return wstr;
		}
		else
			return L"";
	}

	const char* c_strA() const noexcept
	{
		if (str.size())
			return str.c_str();
		else if (wstr.size())
		{
			str = w2c(wstr);
			return str.c_str();
		}
		else
			return "";
	}

	const wchar_t* c_strW() const noexcept
	{
		if (wstr.size())
			return wstr.c_str();
		else if (str.size())
		{
			wstr = c2w(str);
			return wstr.c_str();
		}
		else
			return L"";
	}

	const DString operator+(const DString& str) noexcept
	{
		return GetWideChar() + str.GetWideChar();
	}
	const DString operator+(const wchar_t* const WideChar) noexcept
	{
		return GetWideChar() + WideChar;
	}
	const DString operator+(const char* const MultiByte) noexcept
	{
		return GetMultiByte() + MultiByte;
	}
	void operator+=(const DString& s) noexcept
	{
		wstr = GetWideChar() + s.GetWideChar();
		str.clear();
	}
	void operator+=(const wchar_t* const WideChar) noexcept
	{
		wstr = GetWideChar() + WideChar;
		str.clear();
	}
	void operator+=(const char* const MultiByte) noexcept
	{
		str = GetMultiByte() + MultiByte;
		wstr = c2w(str);
	}
	void operator=(const DString& s) noexcept
	{
		wstr = s.GetWideChar();
		str.clear();
		return;
	}
	bool operator==(const DString& str) noexcept
	{
		return this->GetWideChar() == str.GetWideChar();
	}
	const size_t size() noexcept
	{
		return GetWideChar().size();
	}
	const bool empty() const noexcept
	{
		return GetWideChar().empty();
	}
	void clear() noexcept
	{
		wstr.clear();
		str.clear();
	}
private:
	mutable std::wstring wstr;
	mutable std::string str;

	const std::string w2c(const std::wstring& wstr) const
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
	const std::wstring c2w(const std::string& str) const
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

};
inline const DString operator+(const DString& str1, const DString& str2) noexcept
{
	return str1.GetWideChar() + str2.GetWideChar();
}
inline const bool operator==(const DString& str1, const DString& str2) noexcept
{
	return str1.GetWideChar() == str2.GetWideChar();
}
inline const bool operator!=(const DString& str1, const DString& str2) noexcept
{
	return str1.GetWideChar() != str2.GetWideChar();
}


///<summary>
///DXUI控件消息的回调函数类型 DMsgProc
///</summary>
typedef void(__stdcall* DMsgProc)(HWND hWnd, UINT id, DControlType type, DString UserNamedType, DControlMsg msg, UINT Wnd_Msg, WPARAM Wnd_wParam, LPARAM Wnd_lParam);

///<summary>
///DXUI自用结构体
///</summary>
typedef struct ___tagDXUISendConMsg
{
	UINT ID;
	DControlType Type;
	DString UserNamedType;
	DControlMsg Msg;
	LPARAM lParam;
	WPARAM wParam;

}___DXUISendConMsg;

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

inline float _DXUI_CalcBezier(DBezier bezier, float& perc, UINT nNowFrame, UINT nSrcFrame) noexcept
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
	for (int i = 0; i < 3; ++i)
		t = t - (d + (c + (b + a * t) * t) * t) / (c + (2.0f * b + a * t * 3.0f) * t);

	//根据t求y
	float z = 1.0f - t;
	return 3.0f * bezier.P1 * t * z * z + 3.0f * bezier.P3 * t * t * z + t * t * t;
}

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
	void AddAni(const float Start, const float End, const UINT nFrame, const DBezier* const BezierParam, const UINT AID, const bool ChangeStart = true)
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
		}
		else {
			EnableBezier = false;
		}

		___CalcLock.Unlock();
		return;
	}
	float FrameCalc(UINT* aid) noexcept
	{
		___CalcLock.Lock();

		--nNowFrame;
		if (!nNowFrame)
		{
			if (aid)//这里只有最后一帧赋值，其他情况不赋值
				*aid = AID;
			cache = End;
		}
		else
		{
			if (EnableBezier)
				cache = End - (End - Start) * (1.0f - _DXUI_CalcBezier(bezier, perc, nNowFrame, nSrcFrame));
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

	inline float GetEnd() noexcept
	{
		return End;
	}
	inline void Reset() noexcept
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
	void AddAni(const long Start, const long End, const UINT nFrame, const DBezier* const BezierParam, const UINT AID, const bool ChangeStart = true)
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
		}
		else {
			EnableBezier = false;
		}

		___CalcLock.Unlock();
		return;
	}
	long FrameCalc(UINT* aid) noexcept
	{
		___CalcLock.Lock();

		--nNowFrame;
		if (!nNowFrame)
		{
			if (aid)//这里只有最后一帧赋值，其他情况不赋值
				*aid = AID;
			cache = End;
		}
		else
		{
			if (EnableBezier)
			{
				cache = End - (long)((End - Start) * (1.0f - _DXUI_CalcBezier(bezier, perc, nNowFrame, nSrcFrame)));
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

	inline long GetEnd() noexcept
	{
		return End;
	}
	inline void Reset() noexcept
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

};

class _AniCalc_i
{
public:
	operator bool()
	{
		return (bool)nNowFrame;
	}
	void AddAni(const int Start, const int End, const UINT nFrame, const DBezier* const BezierParam, const UINT AID, const bool ChangeStart = true)
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
		}
		else {
			EnableBezier = false;
		}

		___CalcLock.Unlock();
		return;
	}
	int FrameCalc(UINT* aid) noexcept
	{
		___CalcLock.Lock();

		--nNowFrame;
		if (!nNowFrame)
		{
			if (aid)//这里只有最后一帧赋值，其他情况不赋值
				*aid = AID;
			cache = End;
		}
		else
		{
			if (EnableBezier)
			{
				cache = End - (int)((End - Start) * (1.0f - _DXUI_CalcBezier(bezier, perc, nNowFrame, nSrcFrame)));
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

	inline int GetEnd() noexcept
	{
		return End;
	}
	inline void Reset() noexcept
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

	void AddAni(const D2D1_COLOR_F Start, const DCOLOR End, const UINT nFrame) noexcept
	{
		r.AddAni(Start.r, (float)End.r, nFrame, nullptr, 0);
		g.AddAni(Start.g, (float)End.g, nFrame, nullptr, 0);
		b.AddAni(Start.b, (float)End.b, nFrame, nullptr, 0);
		a.AddAni(Start.a, End.a, nFrame, nullptr, 0);
		return;
	}
	bool FrameCalc(D2D1_COLOR_F& col) noexcept
	{
		bool res = false;
		if (r)
		{
			col.r = r.FrameCalc(nullptr);
			res = true;
		}
		if (g)
		{
			col.g = g.FrameCalc(nullptr);
			res = true;
		}
		if (b)
		{
			col.b = b.FrameCalc(nullptr);
			res = true;
		}
		if (a)
		{
			col.a = a.FrameCalc(nullptr);
			res = true;
		}
		return res;
	}

	inline void Reset() noexcept
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
	inline void PushLayer(ID2D1DeviceContext* const Target, const D2D1_RECT_F contentBounds, ID2D1Geometry* const geo, const D2D1_ANTIALIAS_MODE antiMode)
	{
		Target->PushLayer(D2D1::LayerParameters1(contentBounds, geo, antiMode, D2D1::IdentityMatrix(), 1.0f, nullptr, D2D1_LAYER_OPTIONS1_NONE), nullptr);
		IsClip.push_back(false);
		return;
	}
	inline void PushClip(ID2D1DeviceContext* const Target, const D2D1_RECT_F r)
	{
		Target->PushAxisAlignedClip(r, D2D1_ANTIALIAS_MODE_ALIASED);
		IsClip.push_back(true);
		return;
	}
	inline void PopLast(ID2D1DeviceContext* const Target)
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
	inline void PopAll(ID2D1DeviceContext* const Target)//这里必须按照设置Clip、Layer的顺序弹出，否则报错
	{
		sz = (UINT)IsClip.size();
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

//维护一个窗口列表，用于消息统一处理
inline std::map<HWND, void*> ___WndList;
inline ___DThreadLock ___WndListLock;

///<summary>
///所有控件的基类
///</summary>
class DControl
{
public:

	//当前控件的光标样式
	HCURSOR CursorStyle = LoadCursor(nullptr, IDC_ARROW);
	//用户可以设置此类型名，以便在接收消息时进行某些特别控件的判断、设计组合控件等。默认值为 Undefined
	DString UserNamedType = DefUserNamedType;

	operator UINT()
	{
		return this->ID;
	}

	virtual~DControl()
	{
		_Base_Release();
		return;
	}

	inline UINT GetID() noexcept
	{
		return this->ID;
	}

	inline DControlState GetState() noexcept
	{
		return this->State;
	}

	inline bool IsInit() noexcept
	{
		return this->Inited;
	}

	///<summary>
	///参数均为设备相关像素(DP)
	///</summary>
	virtual bool HitTest(int xPos, int yPos, bool FromChild = false)//FromChild在Label用来命中判断，而不是在字里才命中成功
	{
		___HitTestLock.Lock();
		if (!HitTestGeo)
		{
			___HitTestLock.Unlock();
			return false;
		}
		//在HitTest中计算位置，进行命中测试
		float _xPos = (int)xPos - DstRC.rect.left + DIP2DP(StrokeWidth) / 2.0f, _yPos = (int)yPos - DstRC.rect.top + DIP2DP(StrokeWidth) / 2.0f + GetPosOffset();
		if (_xPos < 0 || _yPos < 0)
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
				value = Parent->HitTest(xPos, yPos, true);
			else//没有父控件，那就说明此Hittest是不需要再次确认的
				value = true;
		}
		___HitTestLock.Unlock();
		return value;
	}

	//如果父控件为ScrollView，用于设置位置偏移指针
	inline void _SetParPosOffPointer(const UINT ParentID, const std::map<UINT, float*>& ParOfs, float* const pointer)
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
	///<summary>
	///设置控件状态
	///</summary>
	inline bool SetState(const DControlState state)
	{
		return m_SetState(state);
	}

	virtual void _SetNeedUpdate(bool need = true)//DCombinedCon 要重载此函数
	{
		___NeedUpdateLock.Lock();
		___UpdateClipLock.Lock();
		UpdateClip.clear();
		NeedUpdate = need;
		___UpdateClipLock.Unlock();
		___NeedUpdateLock.Unlock();
		return;
	}

	//特别注意：接收的rc是经过PosOfs转换过的。只有当父控件调用子控件，或控件自身调用时，需要设定CalcSigma为true
	virtual void _SetNeedUpdate(const D2D1_RECT_F ClipRect, const bool CalcSigma = true)//DCombinedCon 要重载此函数
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
		if (RectChanged == 0)
			r = _DXUI_CalcRectOverlap(r, _GetUpdateRC());
		else
			r = _DXUI_CalcRectUnion(r, RectChanged == 1 ? LastRect : _DXUI_CalcRectUnion(LastRect, LastLastRect));

		if (NeedUpdate)
		{
			if (UpdateClip.size())
			{
				std::vector<D2D1_RECT_F>::iterator updIt = --UpdateClip.end();
				if (!_DXUI_DetectRectContain(*updIt, r))
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

	///<summary>
	///设置是否鼠标穿透
	///</summary>
	void SetMouseThrough(bool through) noexcept
	{
		MouseThrough = through;
		return;
	}

	///<summary>
	///获取是否鼠标穿透
	///</summary>
	bool GetMouseThrough() noexcept
	{
		return MouseThrough;
	}

	///<summary>
	///当前控件是否处于鼠标按下时
	///</summary>
	bool IsOnClick() noexcept
	{
		return OnClick;
	}

	///<summary>
	///设置总的 (包括底色、描边、文字等) 不透明度，与单个其他元素的不透明度叠加，不会互相干扰
	///</summary>
	void SetTotalOpacity(const float opa, const UINT time = 0, const DBezier* const Nonlinear = nullptr, const bool ReDraw = true, const UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
		{
			A_Opa.AddAni(this->TotalOpacity, Min(Max(opa, 0.0f), 1.0f), (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		this->TotalOpacity = Min(Max(opa, 0.0f), 1.0f);

		if (ReDraw)
			this->_SetNeedUpdate();
		return;
	}
	///<summary>
	///设置控件区域内的实时背景高斯模糊(区域受圆角等设置影响), sigma为模糊程度, 与DPI无关, 性能不受sigma影响
	///</summary>
	bool SetBackgroundGaussianBlur(const float sigma)
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
	///<summary>
	///设置控件位置
	///</summary>
	void SetPosition(float x, float y, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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
		
		//呼叫各个控件自己的处理函数
		mSetPosition(x, y);

		//处理位置变化
		___RcChangedLock.Lock();

		++RectChanged;
		if (RectChanged > 2)
			RectChanged = 2;
		LastLastRect = LastRect;
		LastRect = OldUpdateRC;

		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(RectChanged == 2\
				? _DXUI_CalcRectUnion(LastRect, _DXUI_CalcRectUnion(this->_GetUpdateRC(), LastLastRect))\
				: _DXUI_CalcRectUnion(this->_GetUpdateRC(), LastRect), false);
		else
			this->_SetNeedUpdate(RectChanged == 2\
				? _DXUI_CalcRectUnion(LastRect, _DXUI_CalcRectUnion(this->_GetUpdateRC(), LastLastRect))\
				: _DXUI_CalcRectUnion(this->_GetUpdateRC(), LastRect), false);

		___RcChangedLock.Unlock();
		return;
	}
	///<summary>
	///设置控件大小
	///</summary>
	void SetSize(float width, float height, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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

		//呼叫各个控件自己的处理函数
		mSetSize(width, height);

		//处理位置变化
		___RcChangedLock.Lock();

		++RectChanged;
		if (RectChanged > 2)
			RectChanged = 2;
		LastLastRect = LastRect;
		LastRect = OldUpdateRC;
		
		if (Parent)//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
		{
			___BackImgLock.Lock();
			RELEASE(BackImg);
			___BackImgLock.Unlock();

			Parent->_SetNeedUpdate(RectChanged == 2\
				? _DXUI_CalcRectUnion(LastRect, _DXUI_CalcRectUnion(this->_GetUpdateRC(), LastLastRect))\
				: _DXUI_CalcRectUnion(this->_GetUpdateRC(), LastRect), false);
		}
		else
			this->_SetNeedUpdate(RectChanged == 2\
				? _DXUI_CalcRectUnion(LastRect, _DXUI_CalcRectUnion(this->_GetUpdateRC(), LastLastRect))\
				: _DXUI_CalcRectUnion(this->_GetUpdateRC(), LastRect), false);

		___RcChangedLock.Unlock();
		return;
	}

	///<summary>
	///参数DestX为：如果Position在动画，是否返回动画结束后的目标位置。设备独立像素
	///</summary>
	inline float GetPositionX(bool DestX = false) noexcept
	{
		if (DestX && A_RC_x)
			return A_RC_x.GetEnd();
		else
			return DP2DIP(DstRC.rect.left);
	}
	///<summary>
	///参数DestY为：如果Position在动画，是否返回动画结束后的目标位置。设备独立像素
	///</summary>
	inline float GetPositionY(bool DestY = false) noexcept
	{
		if (DestY && A_RC_y)
			return A_RC_y.GetEnd();
		else
			return DP2DIP(DstRC.rect.top);
	}
	///<summary>
	///参数DestW为：如果Size在动画，是否返回动画结束后的目标位置
	///</summary>
	inline float GetWidth(bool DestW = false) noexcept
	{
		if (DestW && A_RC_width)
			return A_RC_width.GetEnd();
		else
			return DP2DIP(DstRC.rect.right - DstRC.rect.left);
	}
	///<summary>
	///参数DestH为：如果Size在动画，是否返回动画结束后的目标位置
	///</summary>
	inline float GetHeight(bool DestH = false) noexcept
	{
		if (DestH && A_RC_height)
			return A_RC_height.GetEnd();
		else
			return DP2DIP(DstRC.rect.bottom - DstRC.rect.top);
	}

	///<summary>
	///获取总的 (包括底色、描边、文字等) 不透明度，与单个其他元素的不透明度叠加，不会互相干扰
	///</summary>
	inline float GetTotalOpacity() noexcept
	{
		return this->TotalOpacity;
	}

	inline D2D1_RECT_F _GetClipRC() noexcept
	{
		D2D1_RECT_F mcliprc;
		float pofs = GetPosOffset();
		mcliprc = { DstRC.rect.left, DstRC.rect.top - pofs, DstRC.rect.right, DstRC.rect.bottom - pofs };

		if (Parent)
		{
			D2D1_RECT_F rc = Parent->_GetClipRC();
			return _DXUI_CalcRectOverlap(mcliprc, rc);
		}
		else
			return mcliprc;
	}

protected:
	friend class DWindow;
	friend class DCombinedCon;

	struct __updItem
	{
		DControl* con;
		std::vector<D2D1_RECT_F> UpdClip;
	};

	HWND OwnerWnd = nullptr;
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

	bool OneMoreFrame = false;//Flip模型多渲染一帧，保存信息用
	std::vector<D2D1_RECT_F> LastUpdateClip;

	bool NeedUpdate = false;
	___DThreadLock ___NeedUpdateLock;

	std::vector<D2D1_RECT_F> UpdateClip;
	___DThreadLock ___UpdateClipLock;

	char RectChanged = 0;//0=没有位置改变, 1=刚改变第一次,只有LastRect里有内容, 2=已改变超过2次, LastLastRect里已有内容。_Base_DisposeAni()会视情况清除
	D2D1_RECT_F LastRect = { 0 };
	D2D1_RECT_F LastLastRect = { 0 };//位置改变时，保存前面隔一帧的位置，因为Flip模型有2个缓冲区，只保存Last会导致移动时清除不干净
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

	D2D1_ROUNDED_RECT DstRC_DIP = { 0 };//<<<没有处理，不要随便使用>>>目标大小的设备独立像素版本
	D2D1_ROUNDED_RECT DstRC = { 0 };//目标大小(绘图用)，<<<设备相关像素>>>
	D2D1_SIZE_F DstSize = { 0 };//<<<没有处理，不要随便使用>>> 单独用一个变量保存大小，防止动画过程中重新计算大小导致不精确，设备无关像素
	D2D1_RECT_F UpdateRC = { 0 };//更新的矩形大小，通常比目标的大半个描边宽度(更新区域用)

	_AniCalc_f A_Opa;//不透明度的动画计算类
	_AniCalc_f A_RC_x;//位置x坐标，设备无关
	_AniCalc_f A_RC_y;//位置y坐标，设备无关
	_AniCalc_f A_RC_width;//长度，设备无关
	_AniCalc_f A_RC_height;//宽度，设备无关
	_AniCalc_f A_Radius_X;//圆角X半径
	_AniCalc_f A_Radius_Y;//圆角Y半径

	//命中测试部分
	ID2D1RoundedRectangleGeometry* HitTestGeo = nullptr;
	___DThreadLock ___HitTestLock;

	//ScrollView控件滚动部分，要在每个控件内植入一个位置偏移量
	std::map<UINT, float*> PosOffset;
	___DThreadLock ___PosOfsLock;

	//一定要在Draw里使用，不可直接对 以1.0f为最大的颜色 来修改，因为经过动画计算后，可能某些颜色值没有变为以255为最大的值，还是以1.0f为最大
	inline D2D1_COLOR_F Calc_fColor(const D2D1_COLOR_F& NowCol, const float& TotalOp) noexcept
	{
		return { (float)NowCol.r / 255.0f, (float)NowCol.g / 255.0f, (float)NowCol.b / 255.0f, NowCol.a * TotalOp };
	}
	inline D2D1_COLOR_F DcolToColf(const DCOLOR& col) noexcept
	{
		return { (float)col.r, (float)col.g, (float)col.b, (float)col.a };
	}
	inline D2D1_COLOR_F DcolToColf_255(const DCOLOR& col) noexcept
	{
		return { (float)col.r / 255.0f, (float)col.g / 255.0f, (float)col.b / 255.0f, (float)col.a };
	}

	inline void ClearRect(ID2D1DeviceContext* const DstTarget, const D2D1_RECT_F& rect, const D2D1_COLOR_F& col)
	{
		DstTarget->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_ALIASED);
		DstTarget->Clear(col);
		DstTarget->PopAxisAlignedClip();
		return;
	}

	virtual void mSetPosition(float x, float y) = 0;
	virtual void mSetSize(float width, float height) = 0;

	//获取DScrollView给子控件设置的偏移量，设备相关像素。由屏幕位置转换到实际位置要加ofs，反则减ofs。ofs为正时表明内容向上滚动。
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
		return floorf(DIP2DP(res));
	}

	//在 DCombinedCon 里有复制的实现，修改时要注意
	inline bool _DrawDispose(ID2D1DeviceContext* const DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* const UpdateClip, ID2D1SolidColorBrush*& brush)
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
			if (!_DXUI_IsRectWork(_DXUI_CalcRectOverlap(Parent->_GetClipRC(), this->_GetUpdateRC())))
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
			//设置更新区的裁剪，主要为过渡动画时使用
			if (UpdateClip)
				Clip.PushClip(DstTarget, *UpdateClip);

			//清除背景
			if (ID != 0)// ID 为 0 是组件，不需要清除背景
				ClearRect(DstTarget, UpdateClip ? *UpdateClip : UpdateRC, BackColor);
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
				wid = A_RC_width.FrameCalc(&AID);
				if (!A_RC_width)
					resp = true;
			}
			if (A_RC_height)
			{
				hei = A_RC_height.FrameCalc(&AID);
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
				x = A_RC_x.FrameCalc(&AID);
				if (!A_RC_x)
					resp = true;
			}
			if (A_RC_y)
			{
				y = A_RC_y.FrameCalc(&AID);
				if (!A_RC_y)
					resp = true;
			}

			SetPosition(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		//在此处恢复RcChanged为false
		if (!A_RC_width && !A_RC_height && !A_RC_x && !A_RC_y)
		{
			___RcChangedLock.Lock();
			RectChanged = 0;
			___RcChangedLock.Unlock();
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
		//让鼠标下的控件响应鼠标当前位置
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
			m->UserNamedType = UserNamedType;
			m->Msg = msg;
			m->wParam = wParam;
			m->lParam = lParam;
			PostMessageW(OwnerWnd, ConMsg, 0, (LPARAM)m);
		}
		___MsgBackLock.Unlock();
		return;
	}

	virtual bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		return false;
	}

	virtual void _UpdateDpi() = 0;
	void _UpdateDpi_DstRCRadius() noexcept
	{
		DstRC.radiusX = roundf(DIP2DP(DstRC_DIP.radiusX));
		DstRC.radiusY = roundf(DIP2DP(DstRC_DIP.radiusY));
		return;
	}

	inline DControlType _GetControlTypes() noexcept
	{
		return this->ControlType;
	}

	virtual bool _GetNeedUpdate() noexcept//不可多次重复调用。文本框要重写此函数，因为显示插入符需要计时，DCombinedCon 要重载此函数
	{
		return NeedUpdate;
	}
	virtual bool _GetOneMoreFrame() noexcept//为主窗口提供获取 OneMoreFrame 的方法，DCombinedCon 要重载此函数
	{
		return OneMoreFrame;
	}

	virtual void _SetOneMoreFrame(bool OneMore) noexcept//为主窗口提供设置 OneMoreFrame 的方法，DCombinedCon 要重载此函数
	{
		OneMoreFrame = OneMore;
		return;
	}

	inline D2D1_RECT_F _GetUpdateRC() noexcept//DControl会用到
	{
		float pofs = GetPosOffset();
		if (pofs == 0.0f)
			return this->UpdateRC;
		else
			return { UpdateRC.left, UpdateRC.top - pofs, UpdateRC.right, UpdateRC.bottom - pofs };
	}

	inline void _SetID(const UINT& id) noexcept
	{
		ID = id;
		return;
	}

	virtual bool _CreateHitTestGeo() = 0;

	bool _Base_Release() noexcept
	{
		if (!Inited)
			return false;

		this->_SeparateFromWindow(false);//先与窗口分离
		Inited = false;

		OwnerWnd = nullptr;
		MouseThrough = false;
		ConMsg = 0;
		NeedUpdate = false;
		UpdateClip.clear();
		OneMoreFrame = false;
		LastUpdateClip.clear();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
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

	inline void _SetMsgBack(const HWND& Owner, const UINT& conMsg) noexcept
	{
		___MsgBackLock.Lock();
		OwnerWnd = Owner;
		ConMsg = conMsg;
		___MsgBackLock.Unlock();
		return;
	}
	virtual void _SetFactory(ID2D1Factory1* const D2DFactory, IWICImagingFactory* const WicFactory, IDWriteFactory* const DWriteFactory)
	{
		this->D2DFactory = D2DFactory;
		this->WicFactory = WicFactory;
		this->DWriteFactory = DWriteFactory;
		_CreateHitTestGeo();//创建命中测试几何
		return;
	}
	inline void _SetParentCon(DControl* const control) noexcept
	{
		Parent = control;
		return;
	}
	inline DControl* _GetParentCon() noexcept
	{
		return Parent;
	}

	virtual void _AddChildCon(DControl* const control)
	{
		___ChildLock.Lock();
		control->_SetParPosOffPointer(ID, PosOffset, nullptr);

		Child.insert(std::pair<UINT, DControl*>(control->GetID(), control));
		___ChildLock.Unlock();

		return;
	}

	void _DeleteChildCon(DControl* const control)
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
	inline void _ClearChildCon()//单纯只清空列表
	{
		___ChildLock.Lock();
		Child.clear();
		___ChildLock.Unlock();
		return;
	}

	inline void _SeparateDisposeCon(bool FromParent) noexcept//对parent、child的释放一定在此函数下面
	{
		___ChildLock.Lock();
		if (Parent && !FromParent)//调用必须不从父控件来，因为父控件会做清理
			Parent->_DeleteChildCon(this);
		if (Child.size())
		{
			std::map<UINT, DControl*>::iterator chi = Child.begin();
			for (UINT i = 0; i < Child.size(); ++i, ++chi)
				chi->second->_SeparateFromWindow(true);
		}
		___ChildLock.Unlock();
		return;
	}

	inline std::map<UINT, DControl*> _GetChildCon() noexcept
	{
		return Child;
	}

	//此函数在底层控件刚绘制完成后调用上层控件的此函数，这时Target里的图片正好可以作为上层控件的背景图
	void _UpdateBackImg(ID2D1DeviceContext* const Target, D2D1_RECT_F* const UpdateClip = nullptr)
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
			Target->CreateBitmap({ (UINT)ceilf(UpdateRC.right - UpdateRC.left), (UINT)ceilf(UpdateRC.bottom - UpdateRC.top) }, nullptr, 0, prop, &BackImg);
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
			D2D1_RECT_F r = _DXUI_CalcRectOverlap(_DXUI_CalcRectOverlap(*UpdateClip, updrc), WndRc);
			___UpdateClipLock.Unlock();
			if (!_DXUI_IsRectWork(r))
			{
				___BackImgLock.Unlock();
				return;
			}

			src.left = (UINT)r.left;
			src.top = (UINT)r.top;
			src.right = (UINT)ceilf(r.right);
			src.bottom = (UINT)ceilf(r.bottom);
			dst = { (UINT)(r.left - updrc.left), (UINT)(r.top - updrc.top) };
		}
		else
		{
			D2D1_SIZE_U szImg = BackImg->GetPixelSize();
			if (updrc.left < 0.0f)
				dst.x = (UINT)(std::fabs(updrc.left));
			else
				dst.x = 0;

			if (updrc.top < 0.0f)
				dst.y = (UINT)(std::fabs(updrc.top));
			else
				dst.y = 0;

			src.left = (UINT)Max(updrc.left, 0.0f);
			src.top = (UINT)Max(updrc.top, 0.0f);
			src.right = Min(src.left + szImg.width, (UINT)wndrc.right);
			src.bottom = Min(src.top + szImg.height, (UINT)wndrc.bottom);
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
				Target->CreateBitmap({ (UINT)ceilf(UpdateRC.right - UpdateRC.left), (UINT)ceilf(UpdateRC.bottom - UpdateRC.top) }, nullptr, 0, prop, &BackImg_Blur);
			}
			if (!BackImg_Blur)
			{
				___BackImgLock.Unlock();
				return;
			}
			if (!BlurEffect)
				Target->CreateEffect(IID{ 0x1feb6d69, 0x2fe6, 0x4ac9, 0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5 }, &BlurEffect);

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
					r1 = _DXUI_CalcRectOverlap(_DXUI_CalcRectOverlap(r1, updrc), WndRc);
					___UpdateClipLock.Unlock();
					src.left = (UINT)r1.left;
					src.top = (UINT)r1.top;
					src.right = (UINT)ceilf(r1.right);
					src.bottom = (UINT)ceilf(r1.bottom);
				}

				ID2D1Image* tar;
				Target->GetTarget(&tar);
				Target->SetTarget(BackImg_Blur);

				enum D2D1_DIRECTIONALBLUR_OPTIMIZATION
				{
					D2D1_DIRECTIONALBLUR_OPTIMIZATION_SPEED = 0,
					D2D1_DIRECTIONALBLUR_OPTIMIZATION_BALANCED = 1,
					D2D1_DIRECTIONALBLUR_OPTIMIZATION_QUALITY = 2,
					D2D1_DIRECTIONALBLUR_OPTIMIZATION_FORCE_DWORD = 0xffffffff
				};
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
	inline void _DrawBackImg(ID2D1DeviceContext* const Target)
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
	virtual void _SeparateFromWindow(bool FromParent) = 0;

	virtual bool _MouseWheelMsgProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		return false;
	}

	virtual void _SetFrameRate(UINT Rate) noexcept//DCombinedCon要重载此函数
	{
		FrameRate = Rate;
		return;
	}

	virtual bool _IsContentChanging() = 0;//此控件内容是否正在改变，必须是全部改变，不能像文本框插入符一样是局部，如果为真，那子控件不会绘制底图。不包括Rect改变

	virtual void _DisposeAnimation() = 0;//每一帧都处理用户手动设置的动画，由于需要使Parent窗口更新，所以需要MainWindow主动在获取完UpdateRect的最后调用它，那么下一帧更新将会使变化生效
	// OnlyDraw 在 DCombinedCon 中使用
	virtual bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip) = 0;

	void _CallChild(std::vector<D2D1_RECT_F>& mUpdateClip)
	{
		___ChildLock.Lock();
		if (Child.size())
		{
			std::map<UINT, DControl*>::iterator child = Child.begin();
			std::vector<D2D1_RECT_F>::iterator updit;
			UINT clipi, s = (UINT)mUpdateClip.size();
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
						rover = _DXUI_CalcRectOverlap(child->second->_GetUpdateRC(), *updit);
						if (_DXUI_IsRectWork(rover, false))
							child->second->_SetNeedUpdate(*updit);

						++clipi;
						++updit;
					} while (clipi < s);
				}
				else//当前控件没有UpdateClip
				{
					child->second->ParentDrawing = _IsContentChanging();
					if (_DXUI_IsRectWork(_DXUI_CalcRectOverlap(child->second->_GetUpdateRC(), this->_GetUpdateRC()), false))
						child->second->_SetNeedUpdate();
				}
			}
		}
		___ChildLock.Unlock();
		return;
	}

	void _CallChildUpdateBackImg(ID2D1DeviceContext* const Target, const std::map<UINT, DControl::__updItem>& UpdCon, const std::map<UINT, DControl::__updItem>::iterator& UpdConIt, std::vector<D2D1_RECT_F>& mUpdateClip)
	{
		std::map<UINT, DControl::__updItem>::iterator mUpdConIt = UpdConIt;
		std::vector<D2D1_RECT_F>::iterator updit;
		UINT clipi, s = (UINT)mUpdateClip.size();
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

	bool Init(POINT position, SIZE size, float radiusX, float radiusY, DButtonTypeStyle Normal, DButtonTypeStyle MouseMove, DButtonTypeStyle Click, DButtonTypeStyle Disable, float StrokeWidth, DString text, DString FamilyName, float FontSize, DFontWeight FontWeight = DFontWeight::Normal)
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
		this->FamilyName = FamilyName.size() ? FamilyName : DefFontName;
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

		ControlType = DControlType::Button;

		NormalType = { 0 };
		MouseMoveType = { 0 };
		ClickType = { 0 };
		DisableType = { 0 };
		NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

		RELEASE(brush);

		//文本区
		Text.clear();
		FamilyName.clear();
		FontSize = 0.0f;
		FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

		RELEASE(TextFormat);
		RELEASE(TextLayout);

		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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

	inline void SetText(DString text)
	{
		___LayoutLock.Lock();
		Text = text;
		RELEASE(TextLayout);//文字发生更改，必须重新创建(Draw时自动重建)，没有修改的方法
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

	inline DString GetText()
	{
		return Text;
	}

	void SetFontFormat(DString FamilyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamilyName = FamilyName.size() ? FamilyName : DefFontName;
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
			TextLayout->SetFontFamilyName(this->FamilyName.c_str(), { 0, (UINT)Text.size() });
			TextLayout->SetFontSize(DIP2DP(this->FontSize), { 0, (UINT)Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, (UINT)Text.size() });
		}
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

protected:
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
	std::wstring FamilyName;
	float FontSize = 0.0f;
	DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

	IDWriteTextFormat* TextFormat = nullptr;
	IDWriteTextLayout* TextLayout = nullptr;
	___DThreadLock ___LayoutLock;


	//*************************DXUI自用函数*************************

	void _SeparateFromWindow(bool FromParent)
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
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
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

	void mSetPosition(float x, float y)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();
		return;
	}
	void mSetSize(float width, float height)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建文本裁剪图形
		___LayoutLock.Lock();
		RELEASE(TextLayout);
		___LayoutLock.Unlock();

		return;
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
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));
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
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DstRC.rect.right - DstRC.rect.left + DIP2DP(StrokeWidth), DstRC.rect.bottom - DstRC.rect.top + DIP2DP(StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

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
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
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
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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
			DWriteFactory->CreateTextFormat(FamilyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DIP2DP(FontSize), LocaleName, &TextFormat);
			TextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

			RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
		}
		if (!TextLayout && TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextLayout(Text.c_str(), (UINT32)Text.size(), TextFormat, DstRC.rect.right - DstRC.rect.left, DstRC.rect.bottom - DstRC.rect.top, &TextLayout);
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



class DButton_WaterWave : public DButton
{
public:
	virtual~DButton_WaterWave()
	{
		this->Release();
		return;
	}

	bool Init(POINT position, SIZE size, float radiusX, float radiusY, DButtonTypeStyle Normal, DButtonTypeStyle MouseMove, DButtonTypeStyle Click, DButtonTypeStyle Disable, DColorBox_Point* Points, UINT WaveTime, float StrokeWidth, DString text, DString FamilyName, float FontSize, DFontWeight FontWeight = DFontWeight::Normal)
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
		this->FamilyName = FamilyName.size() ? FamilyName : DefFontName;
		this->FontSize = FontSize;

		//处理渐变颜色
		if (Points)
		{
			this->Points = new D2D1_GRADIENT_STOP[3];
			if (this->Points)
			{
				for (UINT i = 0; i < 3; ++i)
				{
					DetectColor(Points[i].color);
					this->Points[i].color = DcolToColf_255(Points[i].color);
					this->Points[i].position = Points[i].position;
				}
			}
		}
		this->WaveTime = WaveTime;

		//处理文本样式
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

		ControlType = DControlType::Button;

		NormalType = { 0 };
		MouseMoveType = { 0 };
		ClickType = { 0 };
		DisableType = { 0 };
		NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

		Center = { 0 };
		WaveTime = 0;
		RadialOpa = 1.0f;
		RRadiusX = 0.0f;
		RRadiusY = 0.0f;
		DeleteArray(Points);
		RELEASE(PColl);
		RELEASE(RadialBru);
		RELEASE(brush);

		//文本区
		Text.clear();
		FamilyName.clear();
		FontSize = 0.0f;
		FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

		RELEASE(TextFormat);
		RELEASE(TextLayout);

		return;
	}

	bool SetRadialPoints(DColorBox_Point* Points)
	{
		if (Points)
		{
			___PointsLock.Lock();
			DeleteArray(this->Points);

			this->Points = new D2D1_GRADIENT_STOP[3];
			if (this->Points)
			{
				for (UINT i = 0; i < 3; ++i)
				{
					this->Points[i].color = DcolToColf_255(Points[i].color);
					this->Points[i].position = Points[i].position;
				}
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

protected:
	_AniCalc_f A_RRad_X;
	_AniCalc_f A_RRad_Y;
	_AniCalc_f A_RRad_Opa;

	//渐变
	UINT WaveTime = 0;
	D2D1_POINT_2F Center = { 0 };//设备相关像素
	float RRadiusX = 0.0f;//设备相关像素
	float RRadiusY = 0.0f;//设备相关像素
	float RadialOpa = 1.0f;
	D2D1_GRADIENT_STOP* Points = nullptr;
	___DThreadLock ___PointsLock;

	ID2D1GradientStopCollection* PColl = nullptr;
	___DThreadLock ___PCollLock;

	ID2D1RadialGradientBrush* RadialBru = nullptr;
	___DThreadLock ___RadialBruLock;

	//*************************DXUI自用函数*************************
	void SetRadialOpacity(float opa, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
		{
			A_RRad_Opa.AddAni(RadialOpa, opa, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		RadialOpa = opa;
		this->_SetNeedUpdate();
		return;
	}

	void SetRadialRadius(float RadialRadiusX, float RadialRadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0)
	{
		A_RRad_X.AddAni(RRadiusX, RadialRadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		A_RRad_Y.AddAni(RRadiusY, RadialRadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		this->_SetNeedUpdate();
		return;
	}

	void _SeparateFromWindow(bool FromParent)
	{
		//重新设置动画颜色，防止下一次加入窗口时还没恢复状态
		NowType.FillColor = DcolToColf(NormalType.FillColor);
		NowType.StrokeColor = DcolToColf(NormalType.StrokeColor);
		NowType.TextColor = DcolToColf(NormalType.TextColor);
		A_Col_Fill.Reset();
		A_Col_Stroke.Reset();
		A_Col_Text.Reset();
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
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();
		//释放渐变相关参数
		WaveTime = 0;
		Center = { 0 };
		RRadiusX = 0.0f;
		RRadiusY = 0.0f;
		RadialOpa = 1.0f;

		ParentDrawing = false;
		Parent = nullptr;
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
		RELEASE(TextFormat);
		RELEASE(TextLayout);
		RELEASE(BackImg_Blur);
		RELEASE(BackImg);
		RELEASE(HitTestGeo);
		return;
	}

	bool _IsContentChanging()
	{
		if (A_Col_Fill || A_Col_Stroke || A_Col_Text || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa || A_RRad_X || A_RRad_Y || A_RRad_Opa)
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

			//处理水波渐变
			if (State == DControlState::MouseMove || State == DControlState::Normal)
				this->SetRadialOpacity(0.0f, 250);

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

		this->SetRadialRadius(0.0f, 0.0f);
		this->SetRadialOpacity(1.0f);
		Center.x = (float)xPos;
		Center.y = (float)yPos;
		___RadialBruLock.Lock();
		if (RadialBru)
			RadialBru->SetCenter(Center);
		___RadialBruLock.Unlock();
		float w = DstRC.rect.right - DstRC.rect.left, h = DstRC.rect.bottom - DstRC.rect.top;
		float r = DIP2DP(fabs(sqrt(w * w + h * h) / Points[1].position / 0.9f));
		this->SetRadialRadius(r, r, WaveTime);

		return m_SetState(DControlState::Click, true);
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
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
				if (!A_Radius_Y)
					resp = true;
			}
			SetRadius(x, y, 0, nullptr, 0, false);
			if (resp)
				ResponseCur();
			EndAniSend(AID);
		}
		if (A_RRad_X || A_RRad_Y)
		{
			UINT AID = 0;
			if (A_RRad_X)
				RRadiusX = A_RRad_X.FrameCalc(&AID);
			if (A_RRad_Y)
				RRadiusY = A_RRad_Y.FrameCalc(&AID);

			___RadialBruLock.Lock();
			if (RadialBru)
			{
				RadialBru->SetRadiusX(RRadiusX);
				RadialBru->SetRadiusY(RRadiusY);
			}
			___RadialBruLock.Unlock();

			this->_SetNeedUpdate();
			EndAniSend(AID);
		}
		if (A_RRad_Opa)
		{
			UINT AID = 0;
			SetRadialOpacity(A_RRad_Opa.FrameCalc(&AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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
		//渐变处理
		___PCollLock.Lock();
		___PointsLock.Lock();
		if (!PColl && Points)
		{
			DstTarget->CreateGradientStopCollection(Points, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &PColl);

			___RadialBruLock.Lock();
			RELEASE(RadialBru);
			___RadialBruLock.Unlock();
		}
		___PointsLock.Unlock();

		___RadialBruLock.Lock();
		if (!RadialBru && PColl)
		{
			DstTarget->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(Center, { 0, 0 }, RRadiusX, RRadiusY), PColl, &RadialBru);
		}
		___PCollLock.Unlock();

		//文本处理，尽量缩小锁的锁定时间，靠下放
		___LayoutLock.Lock();
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamilyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DIP2DP(FontSize), LocaleName, &TextFormat);
			TextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

			RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
		}
		if (!TextLayout && TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextLayout(Text.c_str(), (UINT32)Text.size(), TextFormat, DstRC.rect.right - DstRC.rect.left, DstRC.rect.bottom - DstRC.rect.top, &TextLayout);
		}
		//绘制
		if (f_FillColor.a != 0.0f)
		{
			brush->SetColor(f_FillColor);//设置为填充颜色
			DstTarget->FillRoundedRectangle(DstRC, brush);
		}
		if (RadialBru)
		{
			RadialBru->SetOpacity(TotalOpacity * RadialOpa);//设置总不透明度
			DstTarget->FillRoundedRectangle(DstRC, RadialBru);
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
		___RadialBruLock.Unlock();

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
	inline bool Init(POINT position, SIZE size, DCOLOR color, bool CanMoveWindow, DString text, DString FamilyName, float FontSize, bool AutoWarp = true, DFontWeight FontWeight = DFontWeight::Normal, DAlignment X_Alignment = DAlignment::Near, DAlignment Y_Alignment = DAlignment::Center)
	{
		return Init(position, size, { color, false, false, 0 }, { color, false, false, 0 }, { color, false, false, 0 }, CanMoveWindow, text, FamilyName, FontSize, AutoWarp, FontWeight, X_Alignment, Y_Alignment);
	}
	bool Init(POINT position, SIZE size, DLabelTypeStyle Normal, DLabelTypeStyle MouseMove, DLabelTypeStyle Click, bool CanMoveWindow, DString text, DString FamilyName, float FontSize, bool AutoWarp = true, DFontWeight FontWeight = DFontWeight::Normal, DAlignment X_Alignment = DAlignment::Near, DAlignment Y_Alignment = DAlignment::Center)
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
		this->FamilyName = FamilyName.size() ? FamilyName : DefFontName;
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

		ControlType = DControlType::Label;

		NormalType = { 0 };
		MouseMoveType = { 0 };
		ClickType = { 0 };
		NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

		RELEASE(brush);

		//文本区
		Text.clear();
		FamilyName.clear();
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

	inline void SetText(DString text)
	{
		___LayoutLock.Lock();
		Text = text;
		RELEASE(TextLayout);//文字发生更改，必须重新创建(Draw时自动重建)，没有修改的方法

		RecreateTextInterface();//这里立即重建，是因为Hittest时如果Layout为Null，则直接失败导致状态频繁切换
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

	inline DString GetText()
	{
		return Text;
	}

	void SetFontFormat(DString FamilyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamilyName = FamilyName.size() ? FamilyName : DefFontName;
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
			TextLayout->SetFontFamilyName(this->FamilyName.c_str(), { 0, (UINT)Text.size() });
			TextLayout->SetFontSize(DIP2DP(this->FontSize), { 0, (UINT)Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, (UINT)Text.size() });
		}
		___LayoutLock.Unlock();

		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}

protected:
	DLabelTypeStyle NormalType = { 0 };
	DLabelTypeStyle MouseMoveType = { 0 };
	DLabelTypeStyle ClickType = { 0 };

	_Ani_Color A_Col;
	D2D1_COLOR_F NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

	ID2D1SolidColorBrush* brush = nullptr;

	//文本区
	std::wstring Text;
	std::wstring FamilyName;
	float FontSize = 0.0f;
	DWRITE_WORD_WRAPPING AutoWarp = DWRITE_WORD_WRAPPING_WRAP;

	DWRITE_TEXT_ALIGNMENT xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
	DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

	IDWriteTextFormat* TextFormat = nullptr;
	IDWriteTextLayout* TextLayout = nullptr;
	___DThreadLock ___LayoutLock;


	void _SeparateFromWindow(bool FromParent)
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
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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

	void mSetPosition(float x, float y)
	{
		return;
	}
	void mSetSize(float width, float height)
	{
		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface();//这里立即重建，是因为Hittest时如果Layout为Null，则直接失败导致状态频繁切换
		___LayoutLock.Unlock();

		return;
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
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		//扩展区域
		UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
		UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
		UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
		UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));

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
	bool HitTest(int xPos, int yPos, bool FromChild = false)
	{
		float pofs = GetPosOffset();
		int mx = xPos, my = yPos;

		if (mx < DstRC.rect.left || mx > DstRC.rect.right || my < DstRC.rect.top - pofs || my > DstRC.rect.bottom - pofs)
			return false;//这一步是因为，DWrite库的HitTest不受DstRC限制，所以提前判断点是否在用户指定区域内
		if (FromChild)//如果来自于子控件，那判断在控件区域内即可
			return true;

		bool value = false;
		___LayoutLock.Lock();
		if (TextLayout)
		{
			DWRITE_HIT_TEST_METRICS hit;
			BOOL IsBehind = false;
			BOOL isInside = false;
			TextLayout->HitTestPoint((float)(mx - DstRC.rect.left), (float)(my - DstRC.rect.top + ceilf(pofs)), &IsBehind, &isInside, &hit);
			___LayoutLock.Unlock();

			if (Parent)
			{
				value = Parent->HitTest(xPos, yPos, true) && isInside;
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
			DWriteFactory->CreateTextFormat(FamilyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DIP2DP(FontSize), LocaleName, &TextFormat);
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
			DWriteFactory->CreateTextLayout(Text.c_str(), (UINT32)Text.size(), TextFormat, DstRC.rect.right - DstRC.rect.left, DstRC.rect.bottom - DstRC.rect.top, &TextLayout);

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
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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
	HCURSOR CursorStyle = LoadCursor(nullptr, IDC_IBEAM);

	virtual~DTextBox()
	{
		this->Release();
		return;
	}
	bool Init(POINT position, SIZE size, float radiusX, float radiusY, DTextBoxTypeStyle Normal, DTextBoxTypeStyle MouseMove, DTextBoxTypeStyle Edit, DTextBoxTypeStyle Disable, float StrokeWidth, DString FamilyName, float FontSize, bool AutoWarp = true, bool MultiLine = true, bool ReadOnly = false, DFontWeight FontWeight = DFontWeight::Normal, DAlignment X_Alignment = DAlignment::Near, DAlignment Y_Alignment = DAlignment::Near, DCOLOR CaretColor = { 0, 0, 0, 1.0f }, float CaretWidth = 1.5f, DCOLOR SelectFillColor = { 0, 0, 0, 0.45f }, bool SmoothScrolling = true, bool SmoothCaret = true)
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
		this->FamilyName = FamilyName.size() ? FamilyName : DefFontName;
		this->FontSize = FontSize;
		this->MultiLine = MultiLine;
		this->ReadOnly = ReadOnly;
		this->SmoothScr = SmoothScrolling;
		this->SmoothIns = SmoothCaret;

		if (AutoWarp)//处理自动换行
		{
			if (MultiLine)
				this->AutoWarp = DWRITE_WORD_WRAPPING_WRAP;
			else
			{
				this->AutoWarp = DWRITE_WORD_WRAPPING_NO_WRAP;
				_DXUI_DBG_ERROR(1017);
			}
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
			this->yAlignment = MultiLine ? DWRITE_PARAGRAPH_ALIGNMENT_NEAR : DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			break;
		case DAlignment::Far:
			this->yAlignment = MultiLine ? DWRITE_PARAGRAPH_ALIGNMENT_NEAR : DWRITE_PARAGRAPH_ALIGNMENT_FAR;
			break;

		default:
			this->yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		}
		_DXUI_DBG_ASSERT(MultiLine && Y_Alignment != DAlignment::Near, 1016);

		this->Select_FillColor = DcolToColf_255(SelectFillColor);
		this->Insert_LastUpdatetime = _DXUI_SuccessInit == 0 ? ptimeGetTime() : 0;
		this->ControlType = DControlType::TextBox;

		this->Inited = true;
		return true;
	}
	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

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
		FamilyName.clear();
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
		Go0f = false;
		InsOpa = 0.0f;
		Insert_LastUpdatetime = 0;
		Insert_TextPos = 0;
		Insert_PixelPos = { 0 };
		LastInsert_PixelPos = { 0 };
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
	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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
		Insert_PixelPos.right = Insert_PixelPos.left + (UINT)DIP2DP(this->CaretWidth);
		this->_SetNeedUpdate();
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


	void SetFontFormat(DString FamilyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamilyName = FamilyName.size() ? FamilyName : DefFontName;
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
			TextLayout->SetFontFamilyName(this->FamilyName.c_str(), { 0, (UINT)Text.size() });
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
			SetInsTextPos((UINT)Text.size());
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
			SetSelectText(Select_BeginPos, (UINT)Text.size(), false);

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
	void AddText(DString str, bool MoveCaret = true)
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
			Text += str.c_strW();
		}

		if (MoveCaret)
			SetInsTextPos((UINT)Text.size());
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
	void InsertText(UINT TextPos, DString str, bool MoveCaret = true)
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
		else Text.insert(textpos, str.c_strW(), str.size());

		if (MoveCaret)
			SetInsTextPos(textpos + (UINT)str.size());
		___TextLock.Unlock();

		if (SelectMode && Select_EndPos > Text.size())
			SetSelectText(Select_BeginPos, (UINT)Text.size(), false);

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
	inline void InsertText(DString str, bool MoveCaret = true)
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
	///在单行模式下，获取第一行的文本宽度
	///</summary>
	inline float GetTextWidth_SingleLine()
	{
		if (MultiLine)
			return 0.0f;
		___LayoutLock.Lock();
		RecreateTextInterface(false);//如果没有接口，先创建
		___LayoutLock.Unlock();
		return DP2DIP(Select_Metrics.widthIncludingTrailingWhitespace);
	}

	///<summary>
	///获取某一行的高度，设备独立像素
	///</summary>
	inline float GetLineHeight(UINT Count = 1)
	{
		HRESULT hr;
		float res = 0.0f;
		DWRITE_LINE_METRICS* lm;
		UINT32 ac;
		___LayoutLock.Lock();
		RecreateTextInterface(false);//如果没有接口，先创建
		TextLayout->GetLineMetrics(nullptr, 0, &ac);
		lm = new DWRITE_LINE_METRICS[ac];
		hr = TextLayout->GetLineMetrics(lm, 1, &ac);
		___LayoutLock.Unlock();
		if (SUCCEEDED(hr) && ac >= 1 && Count >= 1 && Count <= ac)
			res = lm[Count - 1].height;
		return DP2DIP(res);
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
	void SetOffsetX(const float& ofsX, const UINT& time = 0, const DBezier* const Nonlinear = nullptr, const UINT& AID = 0)
	{
		A_ScrX.AddAni(A_ScrX ? A_ScrX.GetEnd() : Scr_Offset.x, (const int)DIP2DP(ofsX), (UINT)(FrameRate * (time / 1000.0f)), &Bezier_SmoothScroll, 0, false);
		this->_SetNeedUpdate();
		return;
	}
	///<summary>
	///设置滚动的Y偏移，设备独立像素
	///</summary>
	void SetOffsetY(const float& ofsY, const UINT& time = 0, const DBezier* const Nonlinear = nullptr, const UINT& AID = 0)
	{
		if (!MultiLine)//避免掉非多行时的设置
			return;

		A_ScrY.AddAni(A_ScrY ? A_ScrY.GetEnd() : Scr_Offset.y, (const int)DIP2DP(ofsY), (UINT)(FrameRate * (time / 1000.0f)), &Bezier_SmoothScroll, 0, false);
		this->_SetNeedUpdate();
		return;
	}
	///<summary>
	///设置当前文本框文本，并将Caret移动到文本末端。此函数会进行边界检查
	///</summary>
	void SetText(DString str, bool MoveCaret = true)
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
			SetInsTextPos((UINT)Text.size());
		___TextLock.Unlock();

		if (SelectMode && Select_EndPos > Text.size())
			SetSelectText(Select_BeginPos, (UINT)Text.size(), false);

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
	inline DString GetText()
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
		this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) });
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
		this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) });
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

protected:
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
	std::wstring FamilyName;
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
	bool Go0f = false;//标记当前插入符不透明度是过渡到0.0f还是1.0f
	DWORD Insert_LastUpdatetime = 0;
	UINT Insert_TextPos = 0;//文本位置，插入符总在该位置字符的前面
	D2D1_RECT_U Insert_PixelPos = { 0 };//设备相关像素
	D2D1_RECT_F LastInsert_PixelPos = { 0 };//设备相关像素，为了应对Flip模型，需要保存上一次位置
	float InsOpa = 0.0f;//插入符透明度，与插入符颜色透明度叠加
	_AniCalc_i A_InsX;//在动画计算过程中使用设备相关像素
	_AniCalc_i A_InsY;
	_AniCalc_f A_InsOpa;//插入符的透明度渐变动画，为在原有颜色上叠加，范围为0~1.0f

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

	void _SeparateFromWindow(bool FromParent)
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
		A_InsOpa.Reset();
		//插入符
		InsOpa = 0.0f;
		Insert_LastUpdatetime = 0;
		Insert_PixelPos = { 0 };
		LastInsert_PixelPos = { 0 };

		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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

	bool _IsContentChanging()//A_InsX 和 A_InsY 和 A_InsOpa 是局部改变，不能加到这里
	{
		if (A_Col_Fill || A_Col_Stroke || A_Col_Text || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_ScrX || A_ScrY || A_Radius_X || A_Radius_Y || A_Opa)
			return true;
		else
			return false;
	}

	void mSetPosition(float x, float y)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();
		//重新创建文本裁剪图形
		___TextClipLock.Lock();
		RELEASE(SelfTextClip);
		___TextClipLock.Unlock();

		TextPosToInsertPos(true, false, false);
		return;
	}
	void mSetSize(float width, float height)
	{
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
			//检测是否超出边缘，如果超出，则水平方向向左对齐
			if (Select_Metrics.widthIncludingTrailingWhitespace > TextOutputRC.right - TextOutputRC.left)
				TextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			else
				TextLayout->SetTextAlignment(xAlignment);

			if (SelectMode)
				UpdateSelectPos(false, false);
			else
				TextPosToInsertPos(false, SmoothScr, SmoothIns, true);
		}
		___LayoutLock.Unlock();

		return;
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
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));
		}

		//文本输出的区域(考虑到输入区域的边缘不能与边框重叠)
		TextOutputRC.left = floorf(DstRC.rect.left + TextBox_TextEdgeIn);
		TextOutputRC.top = floorf(DstRC.rect.top + TextBox_TextEdgeIn);
		TextOutputRC.right = ceilf(DstRC.rect.right - TextBox_TextEdgeIn);
		TextOutputRC.bottom = ceilf(DstRC.rect.bottom - TextBox_TextEdgeIn);
		DetectTextOptRC(TextOutputRC);

		//预先设置一下插入符的位置，防止其从窗口(0, 0)开始移动到初始位置
		if (Insert_PixelPos.left == 0.0f && Insert_PixelPos.top == 0.0f && Insert_PixelPos.right == 0.0f && Insert_PixelPos.bottom == 0.0f)
		{
			A_InsX.Reset();
			A_InsY.Reset();
			A_InsOpa.Reset();
			Insert_PixelPos.left = (UINT32)TextOutputRC.left;
			Insert_PixelPos.top = (UINT32)TextOutputRC.top;
			Insert_PixelPos.right = (UINT32)TextOutputRC.left + 1;
			Insert_PixelPos.bottom = (UINT32)TextOutputRC.top + 1;
		}

		___LayoutLock.Lock();
		RecreateTextInterface(false);
		TextPosToInsertPos(false, true, true, true, false);
		___LayoutLock.Unlock();
		return;
	}

	bool _GetNeedUpdate() noexcept//根据每个控件的GetNeedUpdate每帧都会调用的特性，在这里为插入符计时并设置刷新
	{
		if (_DXUI_SuccessInit != 0)
			return false;
		if (State == DControlState::Edit && (ptimeGetTime() - Insert_LastUpdatetime) > 500)
		{
			Go0f = !Go0f;
			if (Go0f)
				A_InsOpa.AddAni(InsOpa, 1.0f, (UINT)(FrameRate * (TextBox_CaretGradientTime / 1000.0f)), nullptr, 0);
			else
				A_InsOpa.AddAni(InsOpa, 0.0f, (UINT)(FrameRate * (TextBox_CaretGradientTime / 1000.0f)), nullptr, 0);

			float ofs = GetPosOffset();
			this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) });
			Insert_LastUpdatetime = ptimeGetTime();
		}

		return NeedUpdate;
	}
	//退出组合状态
	inline void ExitComp(HWND hWnd, bool ReDraw)
	{
		HIMC hIMC = pImmGetContext(hWnd);
		if (hIMC)
		{
			pImmSetOpenStatus(hIMC, false);
			pImmReleaseContext(hWnd, hIMC);
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
							LPCWSTR s = (LPCWSTR)GlobalLock(hMem);
							std::wstring lpStr = s ? s : L"";
							if (lpStr.size())
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
	//处理字符消息
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
				this->InsertChar(Insert_TextPos, (const wchar_t)wParam);
				break;
			}

			}//switch wParam End

			break;
		}//WM_CHAR End

		}//switch Msg End

		return true;
	}
	//处理输入字符串消息
	bool _ImeComposition(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (State != DControlState::Edit || ReadOnly)
			return false;

		if (Msg == WM_IME_STARTCOMPOSITION)
		{
			if (EndlishInput)
				pImmAssociateContext(OwnerWnd, nullptr);
			else
				pImmAssociateContextEx(OwnerWnd, nullptr, IACE_DEFAULT);

			HIMC hIMC = pImmGetContext(hWnd);
			if (hIMC)
			{
				pImmSetOpenStatus(hIMC, true);

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

				pImmSetCompositionWindow(hIMC, &CompForm);
				pImmReleaseContext(hWnd, hIMC);
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
				HIMC hIMC = pImmGetContext(hWnd);
				if (!hIMC)
					return false;

				//获取返回文本大小
				dwSize = pImmGetCompositionStringW(hIMC, GCS_RESULTSTR, nullptr, 0);

				//加入结尾空字符大小
				dwSize += sizeof(WCHAR);

				hstr = GlobalAlloc(GHND, dwSize);
				if (!hstr)
				{
					pImmReleaseContext(hWnd, hIMC);
					return false;
				}

				lpstr = (LPWSTR)GlobalLock(hstr);
				if (!lpstr)
				{
					GlobalFree(hstr);
					pImmReleaseContext(hWnd, hIMC);
					return false;
				}

				//获取IME结果字符串
				pImmGetCompositionStringW(hIMC, GCS_RESULTSTR, lpstr, dwSize);
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
				pImmReleaseContext(hWnd, hIMC);
				GlobalUnlock(hstr);
				GlobalFree(hstr);
				return true;
			}
			else if (lParam & GCS_COMPSTR)
			{
				DWORD dwSize;
				HGLOBAL hstr;
				LPWSTR lpstr;
				HIMC hIMC = pImmGetContext(hWnd);
				if (!hIMC)
					return false;

				//获取返回文本大小
				dwSize = pImmGetCompositionStringW(hIMC, GCS_COMPSTR, nullptr, 0);
				if (!dwSize)
				{
					Comp_Str.clear();
					Comp_InsPos = 0;

					___LayoutLock.Lock();
					RELEASE(TextLayout);
					___LayoutLock.Unlock();
					this->_SetNeedUpdate();

					pImmReleaseContext(hWnd, hIMC);
					return true;
				}

				//加入结尾空字符大小
				dwSize += sizeof(WCHAR);

				hstr = GlobalAlloc(GHND, dwSize);
				if (!hstr)
				{
					pImmReleaseContext(hWnd, hIMC);
					return false;
				}

				lpstr = (LPWSTR)GlobalLock(hstr);
				if (!lpstr)
				{
					GlobalFree(hstr);
					pImmReleaseContext(hWnd, hIMC);
					return false;
				}

				//获取IME输入字符串
				pImmGetCompositionStringW(hIMC, GCS_COMPSTR, lpstr, dwSize);

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
				Comp_InsPos = (UINT)pImmGetCompositionStringW(hIMC, GCS_CURSORPOS, nullptr, 0);

				//刷新并设置合成窗口位置
				___LayoutLock.Lock();
				RELEASE(TextLayout);

				COMPOSITIONFORM CompForm;
				CompForm.dwStyle = CFS_POINT;
				_CharPosWithoutComp(CompForm.ptCurrentPos.x, CompForm.ptCurrentPos.y, false);//获取不算组合字符串的合成窗口坐标
				___LayoutLock.Unlock();

				float ofs = GetPosOffset();
				CompForm.ptCurrentPos.y -= (long)ofs;
				pImmSetCompositionWindow(hIMC, &CompForm);

				UpdateCaret();//立即刷新插入符
				this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) });

				//释放内存
				pImmReleaseContext(hWnd, hIMC);
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

		if (State == DControlState::Edit)
		{
			//退出组合状态
			ExitComp(hWnd, false);
			//重置插入符
			float ofs = GetPosOffset();
			A_InsOpa.AddAni(InsOpa, 0.0f, (UINT)(FrameRate * (TextBox_CaretGradientTime / 1000.0f)), nullptr, 0);
			Go0f = true;
			this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) });
		}
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
		bool res = m_SetState(DControlState::Edit, false);//为了适应插入符动画，把它放在这里，有点特殊
		OnClick = true;
		HaveSetNormal = false;

		//刷新插入符动画
		if (res && State == DControlState::Edit)//编辑状态
		{
			A_InsOpa.Reset();
			InsOpa = 0.0f;
			Go0f = false;
		}
		___StateLock.Unlock();

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

		return res;
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
					A_ScrY.AddAni(src, ScrY, SmoothScr ? ((UINT)(FrameRate * (SmoothScrollTime / 1000.0f))) : 0, &Bezier_SmoothScroll, 0, false);
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
					A_ScrY.AddAni(src, ScrY, SmoothScr ? ((UINT)(FrameRate * (SmoothScrollTime / 1000.0f))) : 0, &Bezier_SmoothScroll, 0, false);
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
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DstRC.rect.right - DstRC.rect.left + DIP2DP(StrokeWidth), DstRC.rect.bottom - DstRC.rect.top + DIP2DP(StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

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
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
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
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		if (A_ScrX)
		{
			UINT AID = 0;
			Scr_Offset.x = (UINT)A_ScrX.FrameCalc(&AID);
			this->_SetNeedUpdate();

			if (AID)
			{
				//通知给用户
				mSendConMsg(DControlMsg::TextBox_EndOfScroll, 0, 0);
			}
		}
		if (A_ScrY)
		{
			UINT AID = 0;
			Scr_Offset.y = (UINT)A_ScrY.FrameCalc(&AID);
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
			float ofs = GetPosOffset();
			//保存上上帧位置
			D2D1_RECT_F LastLastInsRC = LastInsert_PixelPos;
			//保存上一帧位置
			LastInsert_PixelPos = { (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) };
			if (A_InsX)
			{
				Insert_PixelPos.left = (UINT)A_InsX.FrameCalc(nullptr);
				Insert_PixelPos.right = (UINT)(Insert_PixelPos.left + DIP2DP(CaretWidth));
			}
			if (A_InsY)
			{
				UINT height = Insert_PixelPos.bottom - Insert_PixelPos.top;
				Insert_PixelPos.top = (UINT)A_InsY.FrameCalc(nullptr);
				Insert_PixelPos.bottom = (UINT)(Insert_PixelPos.top + height);
			}

			UpdateCaret();//立即刷新插入符
			this->_SetNeedUpdate(_DXUI_CalcRectUnion(_DXUI_CalcRectUnion({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) }, LastInsert_PixelPos), LastLastInsRC));
		}
		if (A_InsOpa)//处理插入符透明度渐变
		{
			float ofs = GetPosOffset();
			InsOpa = A_InsOpa.FrameCalc(nullptr);
			this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) });
		}
		return;
	}
	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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
			DstTarget->DrawTextLayout({ floorf(TextOutputRC.left - (float)Scr_Offset.x), floorf(TextOutputRC.top - (float)Scr_Offset.y) }, TextLayout, brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
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
		if (!SelectMode && CaretColor.a * TotalOpacity * InsOpa != 0.0f)
		{
			D2D1_RECT_F DPPos = { (float)Insert_PixelPos.left - (float)Scr_Offset.x, (float)Insert_PixelPos.top - (float)Scr_Offset.y, (float)Insert_PixelPos.right - (float)Scr_Offset.x, (float)Insert_PixelPos.bottom - (float)Scr_Offset.y };
			brush->SetColor({ CaretColor.r, CaretColor.g, CaretColor.b, CaretColor.a * TotalOpacity * InsOpa });
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
		if (insertBottomY > (int)floorf(Select_Metrics.layoutHeight))
		{
			float delta = (float)insertBottomY - Select_Metrics.layoutHeight;
			if (floorf((float)insertUpY - delta) < floorf(Select_Metrics.top))
			{
				ScrY += (int)floorf(((float)insertBottomY - Select_Metrics.layoutHeight - ((float)insertUpY - Select_Metrics.top)) / 2.0f);
				mScrAni = true;
			}
			else
				ScrY += (int)delta;
		}
		else if (insertUpY < (int)floorf(Select_Metrics.top))
		{
			float delta = (float)insertUpY - Select_Metrics.top;
			if (floorf(insertBottomY - delta) > floorf(Select_Metrics.layoutHeight))
			{
				ScrY -= (int)floorf(((float)insertBottomY - Select_Metrics.layoutHeight - ((float)insertUpY - Select_Metrics.top)) / 2.0f);
				mScrAni = true;
			}
			else
				ScrY += (int)delta;

			if (ScrY < 0)
				ScrY = 0;
		}
		//文本太靠上时的处理
		if (Select_Metrics.height > Select_Metrics.layoutHeight && Select_Metrics.height - (float)ScrY < Select_Metrics.layoutHeight)
			ScrY = (int)floorf(Select_Metrics.height - Select_Metrics.layoutHeight);
		else if (Select_Metrics.height < Select_Metrics.layoutHeight)
			ScrY = 0;

		//应用滚动
		if (srcX != ScrX || (srcY != ScrY && MultiLine))//只有是多行时，才应用垂直滚动
		{
			if (mScrAni)
			{
				if (srcX != ScrX)
					A_ScrX.AddAni(srcX, ScrX, (UINT)(FrameRate * (SmoothScrollTime / 1000.0f)), &Bezier_SmoothScroll, 0, false);
				if (MultiLine && srcY != ScrY)//水平滚动进来后，避免掉非多行时的设置
					A_ScrY.AddAni(srcY, ScrY, (UINT)(FrameRate * (SmoothScrollTime / 1000.0f)), &Bezier_SmoothScroll, 0, false);

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

	//通过坐标获取文本位置
	inline UINT ScreenPosToTextPos(int xPos, int yPos)
	{
		float ofs = GetPosOffset();
		DWRITE_HIT_TEST_METRICS hit = { 0 };
		BOOL IsBehind = false;
		BOOL isInside = false;

		___LayoutLock.Lock();
		RecreateTextInterface(false);//如果相关接口为NULL，则直接重建

		if (TextLayout)
			TextLayout->HitTestPoint((float)xPos - TextOutputRC.left + (float)Scr_Offset.x, (float)yPos - TextOutputRC.top + (float)Scr_Offset.y + ofs, &IsBehind, &isInside, &hit);
		___LayoutLock.Unlock();

		//bidi level为偶数时表示方向为从左到右，奇数时为从右到左
		if (hit.bidiLevel % 2)
			IsBehind = !IsBehind;

		return hit.textPosition + IsBehind;
	}

	//设置插入符的文本位置
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

	//通过偏移设置插入符的文本位置
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
		if (State == DControlState::Edit)
		{
			//立即刷新插入符
			Insert_LastUpdatetime = _DXUI_SuccessInit == 0 ? ptimeGetTime() : 0;//重置插入符的闪烁时间
			A_InsOpa.AddAni(InsOpa, 1.0f, (UINT)(FrameRate * (TextBox_CaretGradientTime / 1000.0f)), nullptr, 0);
			Go0f = false;
		}
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
			UpdateCaret();//刷新插入符

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

		UpdateCaret();//刷新插入符

		SelectMode = true;
		___SelectLock.Unlock();

		if (ReDraw)
			this->_SetNeedUpdate();
		return;
	}

	//接收坐标处理选中区域
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

	//通过坐标确定光标位置
	void ResetInsertPos(int xPos, int yPos, bool InsAnimation = true)
	{
		DWRITE_HIT_TEST_METRICS hit = { 0 };
		BOOL IsBehind = false;
		BOOL isInside = false;
		float ofs = GetPosOffset();

		___LayoutLock.Lock();

		RecreateTextInterface(false);//如果相关接口为NULL，则直接重建

		if (TextLayout)
			TextLayout->HitTestPoint((float)xPos - TextOutputRC.left + (float)Scr_Offset.x, (float)yPos - TextOutputRC.top + (float)Scr_Offset.y + ofs, &IsBehind, &isInside, &hit);

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

		if (Insert_PixelPos.left == dst.left && Insert_PixelPos.top == dst.top && Insert_PixelPos.right == dst.right && Insert_PixelPos.bottom == dst.bottom)
		{
			UpdateCaret();//立即刷新插入符
			this->_SetNeedUpdate({ (float)Insert_PixelPos.left - (float)Scr_Offset.x - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.top - (float)Scr_Offset.y - ofs - floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.right - (float)Scr_Offset.x + floorf(DIP2DP(1.0f)), (float)Insert_PixelPos.bottom - (float)Scr_Offset.y - ofs + floorf(DIP2DP(1.0f)) });
		}
		else
		{
			//先给长度高度赋值，dispose那里需要根据这个计算
			Insert_PixelPos.right = (UINT)(Insert_PixelPos.left + DIP2DP(CaretWidth));
			Insert_PixelPos.bottom = (UINT)(Insert_PixelPos.top + hit.height);

			A_InsX.AddAni((int)Insert_PixelPos.left, (int)dst.left, InsAnimation ? ((UINT)(FrameRate * (TextBox_CaretMoveTime / 1000.0f))) : 0, &Bezier_SmoothScroll, 0);
			A_InsY.AddAni((int)Insert_PixelPos.top, (int)dst.top, InsAnimation ? ((UINT)(FrameRate * (TextBox_CaretMoveTime / 1000.0f))) : 0, &Bezier_SmoothScroll, 0);

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

		A_InsX.AddAni((int)Insert_PixelPos.left, (int)dst.left, InsAnimation ? ((UINT)(FrameRate * (TextBox_CaretMoveTime / 1000.0f))) : 0, &Bezier_SmoothScroll, 0);
		A_InsY.AddAni((int)Insert_PixelPos.top, (int)dst.top, InsAnimation ? ((UINT)(FrameRate * (TextBox_CaretMoveTime / 1000.0f))) : 0, &Bezier_SmoothScroll, 0);

		if (ResetScr)
			ResetTextScrolling(ScrAni, ReDraw);//超出文本框时的处理
		return;
	}

	inline bool RecreateTextInterface(bool ResetScr = true)//注意，此函数不上锁，要放在锁定区域内调用
	{
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamilyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DIP2DP(FontSize), LocaleName, &TextFormat);
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
			UINT mInsPos = Min(Insert_TextPos, (UINT)Text.size());
			if (Password)
			{
				std::wstring strpass;
				for (UINT i = 0; i < Text.size(); ++i)
					strpass += L'l';
				DWriteFactory->CreateTextLayout(strpass.c_str(), (UINT32)strpass.size(), TextFormat, ceilf(TextOutputRC.right - TextOutputRC.left), ceilf(TextOutputRC.bottom - TextOutputRC.top), &TextLayout);
				if (TextLayout)
					TextLayout->SetFontFamilyName(L"Wingdings", { 0, (UINT32)strpass.size() });
			}
			else
				DWriteFactory->CreateTextLayout((Text.substr(0, mInsPos) + Comp_Str + Text.substr(mInsPos, Text.size() - mInsPos)).c_str(), (UINT32)(Text.size() + Comp_Str.size()), TextFormat, ceilf(TextOutputRC.right - TextOutputRC.left), ceilf(TextOutputRC.bottom - TextOutputRC.top), &TextLayout);
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

	inline void DetectTextOptRC(D2D1_RECT_F TextOptRC)
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

	void Init(POINT position, SIZE size, float radiusX = 0.0f, float radiusY = 0.0f, bool CanMoveWindow = true, DScaleMode ScaleMode = DScaleMode::EqualScale, float TotalOpacity = 1.0f, DCOLOR FillColor = { 0, 0, 0, 0.0f }, DCOLOR StrokeColor = { 0, 0, 0, 0.0f }, float StrokeWidth = 0.0f)
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

		this->FillColor = DcolToColf(FillColor);
		this->StrokeColor = DcolToColf(StrokeColor);

		this->ScaleMode = ScaleMode;
		SetTotalOpacity(TotalOpacity, 0, nullptr, false);

		this->ControlType = DControlType::ImageBox;

		this->Inited = true;
		return;
	}
	///<summary>
	///从文件路径加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromFile(DString Path)
	{
		if (!Inited)
			return false;
		bool res = LoadInMemory(Path);
		if (res)
			this->_SetNeedUpdate();
		else
		{
			_DXUI_DBG_ERROR(1010);
		}

		return res;
	}
	///<summary>
	///从资源加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromResource(WORD ResourceID, DString TypeName, HMODULE Module = nullptr)
	{
		if (!Inited)
			return false;
		bool res = LoadInMemory(ResourceID, TypeName.c_strW(), Module);
		if (res)
			this->_SetNeedUpdate();
		else
		{
			_DXUI_DBG_ERROR(1011);
		}

		return res;
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
			return true;
		}
		else
		{
			_DXUI_DBG_ERROR(1012);
			return false;
		}
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
		A_Stroke.Reset();
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
	void GetImgSize(UINT& Width, UINT& Height)
	{
		Width = (UINT)DP2DIP(BmpSize.width);
		Height = (UINT)DP2DIP(BmpSize.height);
		return;
	}
	//设备相关像素
	void GetImgPixelSize(UINT& Width, UINT& Height)
	{
		Width = (UINT)BmpSize.width;
		Height = (UINT)BmpSize.height;
		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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

	void SetImageOpacity(float opacity, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
		{
			A_ImgOpa.AddAni(ImgOpacity, opacity, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		ImgOpacity = opacity;
		this->_SetNeedUpdate();
		return;
	}

	void SetImageRotation(float angle, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
		{
			A_Ang.AddAni(RotationAngle, angle, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		RotationAngle = angle;
		this->_SetNeedUpdate();
		return;
	}

	void SetBackColor(DCOLOR col, UINT time = 0, bool _Reserved = true)
	{
		A_Fill.AddAni(FillColor, col, (UINT)(FrameRate * (time / 1000.0f)));
		this->_SetNeedUpdate();
		return;
	}

	void SetStrokeColor(DCOLOR col, UINT time = 0, bool _Reserved = true)
	{
		A_Stroke.AddAni(StrokeColor, col, (UINT)(FrameRate * (time / 1000.0f)));
		this->_SetNeedUpdate();
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

protected:
	//以255位最大的颜色
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
	_Ani_Color A_Stroke;

	___DThreadLock ___GauLock;

	void _SeparateFromWindow(bool FromParent)
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
		A_Stroke.Reset();
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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

	void mSetPosition(float x, float y)
	{
		//重新计算Bmp位置, 一定要在UpdateDpi下面
		CalcBmpSize(BmpRC, (UINT)BmpSize.width, (UINT)BmpSize.height);
		BmpSize.width = BmpRC.right - BmpRC.left;
		BmpSize.height = BmpRC.bottom - BmpRC.top;

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
	}
	void mSetSize(float width, float height)
	{
		//重新计算Bmp位置, 一定要在UpdateDpi下面
		CalcBmpSize(BmpRC, (UINT)BmpSize.width, (UINT)BmpSize.height);
		BmpSize.width = BmpRC.right - BmpRC.left;
		BmpSize.height = BmpRC.bottom - BmpRC.top;

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));
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
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DstRC.rect.right - DstRC.rect.left + DIP2DP(StrokeWidth), DstRC.rect.bottom - DstRC.rect.top + DIP2DP(StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	bool _IsContentChanging()
	{
		if (A_Fill || A_Stroke || A_Ang || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa)
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
			this->_SetNeedUpdate();
		}
		if (A_Stroke)
		{
			A_Stroke.FrameCalc(StrokeColor);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
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
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}
		if (A_ImgOpa)
		{
			UINT AID = 0;
			SetImageOpacity(A_ImgOpa.FrameCalc(&AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}
		if (A_Ang)
		{
			UINT AID = 0;
			SetImageRotation(A_Ang.FrameCalc(&AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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
		//处理颜色
		D2D1_COLOR_F mFillColor = Calc_fColor(FillColor, TotalOpacity);
		D2D1_COLOR_F mStrokeColor = Calc_fColor(StrokeColor, TotalOpacity);
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
			D2D1::Matrix3x2F rotation;
			pD2D1MakeRotateMatrix(RotationAngle, { (BmpRC.right + BmpRC.left) / 2.0f, (BmpRC.bottom + BmpRC.top) / 2.0f }, &rotation);
			DstTarget->SetTransform(rotation);
			HaveSetAngle = true;
		}
		//开始绘制
		if (mFillColor.a != 0.0f)
		{
			brush->SetColor(mFillColor);//设置为填充颜色
			DstTarget->FillRectangle(DstRC.rect, brush);
		}
		//画图片
		if (Bmp && TotalOpacity * ImgOpacity != 0.0f)
		{
			DstTarget->DrawBitmap(Bmp, BmpRC, TotalOpacity * ImgOpacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
		}
		___BmpLock.Unlock();
		if (mStrokeColor.a != 0.0f)
		{
			brush->SetColor(mStrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}


	inline bool LoadInMemory(DString Path)
	{
		HANDLE hFile = CreateFileW(Path.c_strW(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

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

	inline bool MyLoadBmp(ID2D1DeviceContext* Target, BYTE* data, ULONG Size, ID2D1Bitmap** bmp, D2D1_RECT_F& bmprc)
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
			hr = pFormat->Initialize(pFrame, REFWICPixelFormatGUID{ 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10 }, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
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

	//设备相关像素。bmprc为可显示区域大小，wid、hei为图片长宽
	void CalcBmpSize(D2D1_RECT_F& bmprc, const UINT wid, const UINT hei)
	{
		//计算目标图片大小
		if (this->ScaleMode == DScaleMode::EqualScale)
		{
			RECT GoodRc = { 0 };
			GoodRc = RectSizeWithConstantRatio({ 0, 0, (long)(DstRC.rect.right - DstRC.rect.left), (long)(DstRC.rect.bottom - DstRC.rect.top) }, { (long)wid, (long)hei });

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

	bool Init(POINT position, SIZE size, float radiusX = 0.0f, float radiusY = 0.0f, bool CanMoveWindow = true, DScaleMode ScaleMode = DScaleMode::EqualScale, float TotalOpacity = 1.0f, DCOLOR FillColor = { 0, 0, 0, 0.0f }, DCOLOR StrokeColor = { 0, 0, 0, 0.0f }, float StrokeWidth = 0.0f)
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

		DetectColor(FillColor);
		DetectColor(StrokeColor);

		this->FillColor = DcolToColf(FillColor);
		this->StrokeColor = DcolToColf(StrokeColor);

		this->ScaleMode = ScaleMode;
		SetTotalOpacity(TotalOpacity, 0, nullptr, false);

		this->ControlType = DControlType::GifBox;
		Inited = true;
		return true;
	}
	///<summary>
	///从文件路径加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromFile(DString Path)
	{
		if (!Inited)
			return false;
		bool res = LoadInMemory(Path);
		if (res)
			this->_SetNeedUpdate();
		else
		{
			_DXUI_DBG_ERROR(1013);
		}

		return res;
	}
	///<summary>
	///从资源加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromResource(WORD ResourceID, DString TypeName, HMODULE Module = nullptr)
	{
		if (!Inited)
			return false;
		bool res = LoadInMemory(ResourceID, TypeName.c_strW(), Module);
		if (res)
			this->_SetNeedUpdate();
		else
		{
			_DXUI_DBG_ERROR(1014);
		}

		return res;
	}

	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		ControlType = DControlType::GifBox;

		A_Ang.Reset();
		A_Fill.Reset();
		A_Stroke.Reset();
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

	void GetGifSize(UINT& Width, UINT& Height)
	{
		Width = (UINT)DIP2DP(BmpSize.width);
		Height = (UINT)DIP2DP(BmpSize.height);
		return;
	}
	void GetGifPixelSize(UINT& Width, UINT& Height)
	{
		Width = (UINT)BmpSize.width;
		Height = (UINT)BmpSize.height;
		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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
	void SetGifRotation(float angle, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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

	void SetBackColor(DCOLOR col, UINT time = 0, bool _Reserved = true)
	{
		A_Fill.AddAni(FillColor, col, (UINT)(FrameRate * (time / 1000.0f)));
		this->_SetNeedUpdate();
		return;
	}

	void SetStrokeColor(DCOLOR col, UINT time = 0, bool _Reserved = true)
	{
		A_Stroke.AddAni(StrokeColor, col, (UINT)(FrameRate * (time / 1000.0f)));
		this->_SetNeedUpdate();
		return;
	}

protected:
	//以255为最大的颜色
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
	_Ani_Color A_Stroke;

	void _SeparateFromWindow(bool FromParent)
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
		A_Stroke.Reset();
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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

	bool _GetNeedUpdate() noexcept//根据每个控件的GetNeedUpdate每帧都会调用的特性，在这里为插入符计时并设置刷新
	{
		if (Playing && FrameCount > 1 && (_DXUI_SuccessInit == 0 ? ptimeGetTime() : 0) - LastFrameTime < LastFrameDelay)
		{
			this->_SetNeedUpdate();
		}

		return NeedUpdate;
	}

	void mSetPosition(float x, float y)
	{
		//重新计算Bmp位置, 一定要在UpdateDpi下面
		CalcBmpSize(BmpRC, (UINT)BmpSize.width, (UINT)BmpSize.height);
		BmpSize.width = BmpRC.right - BmpRC.left;
		BmpSize.height = BmpRC.bottom - BmpRC.top;

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
	}
	void mSetSize(float width, float height)
	{
		//重新计算Bmp位置, 一定要在UpdateDpi下面
		CalcBmpSize(BmpRC, (UINT)BmpSize.width, (UINT)BmpSize.height);
		BmpSize.width = BmpRC.right - BmpRC.left;
		BmpSize.height = BmpRC.bottom - BmpRC.top;

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));
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
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DstRC.rect.right - DstRC.rect.left + DIP2DP(StrokeWidth), DstRC.rect.bottom - DstRC.rect.top + DIP2DP(StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	bool _IsContentChanging()
	{
		if (A_Fill || A_Stroke || A_Ang || A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa)
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
			this->_SetNeedUpdate();
		}
		if (A_Stroke)
		{
			A_Stroke.FrameCalc(StrokeColor);
			this->_SetNeedUpdate();
		}
		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
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
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}
		if (A_Ang)
		{
			UINT AID = 0;
			SetGifRotation(A_Ang.FrameCalc(&AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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
			LastFrameTime = _DXUI_SuccessInit == 0 ? ptimeGetTime() : 0;
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
		D2D1_COLOR_F mFillColor = Calc_fColor(FillColor, TotalOpacity);
		D2D1_COLOR_F mStrokeColor = Calc_fColor(StrokeColor, TotalOpacity);
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
			D2D1::Matrix3x2F rotation;
			pD2D1MakeRotateMatrix(RotationAngle, { (BmpRC.right + BmpRC.left) / 2.0f, (BmpRC.bottom + BmpRC.top) / 2.0f }, &rotation);
			DstTarget->SetTransform(rotation);
			HaveSetAngle = true;
		}
		//开始绘制
		if (mFillColor.a != 0.0f)
		{
			brush->SetColor(mFillColor);//设置为填充颜色
			DstTarget->FillRectangle(DstRC.rect, brush);
		}
		//画图片
		if (Bmp && TotalOpacity != 0.0f)
		{
			DstTarget->DrawBitmap(Bmp, BmpRC, TotalOpacity, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		}
		___BmpLock.Unlock();

		if (mStrokeColor.a != 0.0f)
		{
			brush->SetColor(mStrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(DstRC, brush, DIP2DP(StrokeWidth));
		}

		if (HaveSetAngle)
			DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}


	inline bool LoadInMemory(DString Path)
	{
		HANDLE hFile = CreateFileW(Path.c_strW(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

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
	bool MyNextFrame(ID2D1DeviceContext* Target, ID2D1Bitmap** bmp, D2D1_RECT_F& bmprc, UINT& FrameDelay)
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
			hr = pFormat->Initialize(pFrame, REFWICPixelFormatGUID{ 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10 }, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
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
					delay = propValue.uiVal * 10;
				PropVariantClear(&propValue);
			}
			else
				delay = 25;
			RELEASE(pFrameMetadataQueryReader);
		}

		return hr;
	}

	void CalcBmpSize(D2D1_RECT_F& bmprc, const UINT wid, const UINT hei)
	{
		//计算目标图片大小
		if (this->ScaleMode == DScaleMode::EqualScale)
		{
			RECT GoodRc = { 0 };
			GoodRc = RectSizeWithConstantRatio({ 0, 0, (long)(DstRC.rect.right - DstRC.rect.left), (long)(DstRC.rect.bottom - DstRC.rect.top) }, { (long)wid, (long)hei });

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

	bool Init(POINT position, SIZE size, float radiusX, float radiusY, DCOLOR FillColor, DCOLOR StrokeColor = { 0 }, float StrokeWidth = 0.0f, bool CanMoveWindow = false)
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

		mFillColor = { 0 };
		mStrokeColor = { 0 };
		ControlType = DControlType::SolidColorBox;

		RELEASE(brush);

		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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

	inline void SetFillColor(DCOLOR col, UINT time = 0, bool _Reserved = true)
	{
		DCOLOR c = col;
		DetectColor(c);
		if (_Reserved)
		{
			A_Col.AddAni(mFillColor, c, (UINT)(FrameRate * (time / 1000.0f)));
			this->_SetNeedUpdate();
			return;
		}

		mFillColor = DcolToColf(c);
		this->_SetNeedUpdate();//设置完后将状态置为需要更新
		return;
	}
	inline void SetStrokeColor(DCOLOR col, UINT time = 0, bool _Reserved = true)
	{
		DCOLOR c = col;
		DetectColor(c);
		if (_Reserved)
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

	void _SeparateFromWindow(bool FromParent)
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
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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

	void mSetPosition(float x, float y)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
	}
	void mSetSize(float width, float height)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
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
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		if (StrokeWidth != 0.0f)
		{
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top)) - ceilf(DIP2DP(StrokeWidth) / 2.0f);
			if (UpdateRC.left < 0.0f)
				UpdateRC.left = 0.0f;
			if (UpdateRC.top < 0.0f)
				UpdateRC.top = 0.0f;
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom)) + ceilf(DIP2DP(StrokeWidth) / 2.0f);
		}
		else {
			//扩展区域
			UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
			UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
			UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
			UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));
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
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DstRC.rect.right - DstRC.rect.left + DIP2DP(StrokeWidth), DstRC.rect.bottom - DstRC.rect.top + DIP2DP(StrokeWidth) }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

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
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
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
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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

	bool Init(POINT position, SIZE size, float radiusX, float radiusY, DCOLOR BackColor, DColorBox_Point* Points, UINT PointCount, POINT StartPos, POINT EndPos, DCOLOR StrokeColor = { 0 }, float StrokeWidth = 0.0f, bool CanMoveWindow = false)
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

	void SetStartPos(const POINT& StartPos, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0)
	{
		A_Sta_X.AddAni(StartPoint.x, (float)StartPos.x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		A_Sta_Y.AddAni(StartPoint.y, (float)StartPos.y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		this->_SetNeedUpdate();
		return;
	}

	void SetEndPos(const POINT& EndPos, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0)
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

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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

	void SetLinearOpacity(float opa, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
		{
			A_Lin_Opa.AddAni(LinearOpa, opa, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		LinearOpa = opa;
		this->_SetNeedUpdate();

		return;
	}
protected:
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

	void _SeparateFromWindow(bool FromParent)
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
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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

	void mSetPosition(float x, float y)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
	}
	void mSetSize(float width, float height)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
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
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
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
				StartPoint.x = A_Sta_X.FrameCalc(&AID);
			if (A_Sta_Y)
				StartPoint.y = A_Sta_Y.FrameCalc(&AID);

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
				EndPoint.x = A_End_X.FrameCalc(&AID);
			if (A_End_Y)
				EndPoint.y = A_End_Y.FrameCalc(&AID);

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
			SetLinearOpacity(A_Lin_Opa.FrameCalc(&AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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

	bool Init(POINT position, SIZE size, float RoundRadiusX, float RoundRadiusY, DCOLOR BackColor, DColorBox_Point* Points, UINT PointCount, POINT RadialCenter, float RadialRadiusX, float RadialRadiusY, DCOLOR StrokeColor = { 0 }, float StrokeWidth = 0.0f, bool CanMoveWindow = false)
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

	void SetRadialCenter(const POINT& center, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0)
	{
		A_Cen_X.AddAni(Center.x, (float)center.x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		A_Cen_Y.AddAni(Center.y, (float)center.y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
		this->_SetNeedUpdate();
		return;
	}

	void SetRadialRadius(float RadialRadiusX, float RadialRadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0)
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

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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

	void SetRadialOpacity(float opa, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
		{
			A_RRad_Opa.AddAni(RadialOpa, opa, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear, AID);
			this->_SetNeedUpdate();
			return;
		}
		RadialOpa = opa;
		this->_SetNeedUpdate();
		return;
	}

protected:
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

	void _SeparateFromWindow(bool FromParent)
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
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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

	void mSetPosition(float x, float y)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
	}
	void mSetSize(float width, float height)
	{
		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
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
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
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
				Center.x = A_Cen_X.FrameCalc(&AID);
			if (A_Cen_Y)
				Center.y = A_Cen_Y.FrameCalc(&AID);

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
				RRadiusX = A_RRad_X.FrameCalc(&AID);
			if (A_RRad_Y)
				RRadiusY = A_RRad_Y.FrameCalc(&AID);

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
			SetRadialOpacity(A_RRad_Opa.FrameCalc(&AID), 0, nullptr, 0, false);
			EndAniSend(AID);
		}
		if (A_Opa)
		{
			UINT AID = 0;
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
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
		this->ScrDelta = Max(0.0f, ScrollDelta);
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
		_DisposeScr(nullptr, SmoothScr ? ((UINT)(FrameRate * (ScrTime / 1000.0f))) : 0, &Bezier_SmoothScroll);
		return;
	}

	float GetFullHeight()
	{
		return FullHeight;
	}
	///<summary>
	///Offset为大于等于0的小数(即DIP像素)，表示向上偏移的 设备无关像素 数量
	///</summary>
	void SetScrollOffset(float Offset, UINT time = 0, const DBezier* Nonlinear = nullptr)
	{
		float Scr = Offset;
		_DisposeScr(&Scr, time, Nonlinear);
		return;
	}

protected:
	ID2D1SolidColorBrush* brush = nullptr;

	float FullHeight = 0.0f;//设备无关像素
	float ScrOffset = 0.0f;//设备无关像素
	float ScrDelta = 0.0f;//滚动一次像素偏移值，设备无关像素
	UINT ScrTime = 0;

	bool SmoothScr = true;
	_AniCalc_f A_Scr;//设备独立像素

	void _SeparateFromWindow(bool FromParent)
	{
		ScrOffset = 0.0f;

		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Scr.Reset();
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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

	void _DisposeScr(float* scr, UINT time, const DBezier* Nonlinear)
	{
		float Scr, src = ScrOffset;
		if (scr)
			Scr = *scr;
		else
		{
			if (A_Scr)
				Scr = A_Scr.GetEnd();
			else
				Scr = ScrOffset;
		}

		if (FullHeight - Scr < DP2DIP(DstRC.rect.bottom - DstRC.rect.top))//太靠上时的处理
			Scr = FullHeight - DP2DIP(DstRC.rect.bottom - DstRC.rect.top);//修改记得变号

		if (Scr < 0.0f)
			Scr = 0.0f;

		if (src != Scr)
		{
			A_Scr.AddAni(src, Scr, time, Nonlinear, 1, false);
			this->_SetNeedUpdate();
		}
		return;
	}
	void mSetPosition(float x, float y)
	{
		return;
	}
	void mSetSize(float width, float height)
	{
		_DisposeScr(nullptr, SmoothScr ? ((UINT)(FrameRate * (ScrTime / 1000.0f))) : 0, &Bezier_SmoothScroll);
		return;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		//扩展区域
		UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
		UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
		UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
		UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));
		return;
	}

	//要重载基类的AddChildCon，并加上自己的ScrOffset
	void _AddChildCon(DControl* const control)
	{
		___ChildLock.Lock();
		control->_SetParPosOffPointer(ID, PosOffset, &ScrOffset);

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
					A_Scr.AddAni(src, Scr, SmoothScr ? ((UINT)(FrameRate * (ScrTime / 1000.0f))) : 0, &Bezier_SmoothScroll, 1, false);
					this->_SetNeedUpdate();
				}
				else return false;
			}
			else return false;
		}
		else if (Delta < 0)//手指向后滑动时
		{
			if (FullHeight - Scr > DP2DIP(DstRC.rect.bottom - DstRC.rect.top))
			{
				Scr += DIP2DP(ScrDelta);

				if (FullHeight - Scr < DP2DIP(DstRC.rect.bottom - DstRC.rect.top))//太靠上时的处理
					Scr = FullHeight - DP2DIP(DstRC.rect.bottom - DstRC.rect.top);//修改记得变号

				if (Scr < 0.0f)
					Scr = 0.0f;

				if (src != Scr)
				{
					A_Scr.AddAni(src, Scr, SmoothScr ? ((UINT)(FrameRate * (ScrTime / 1000.0f))) : 0, &Bezier_SmoothScroll, 1, false);
					this->_SetNeedUpdate();
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
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DstRC.rect.right - DstRC.rect.left, DstRC.rect.bottom - DstRC.rect.top }, 0.0f, 0.0f }, &HitTestGeo);

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
			ScrOffset = A_Scr.FrameCalc(&AID);
			ResponseCur();//这个滚动要随时响应鼠标
			this->_SetNeedUpdate();
			//如果动画结束，发送消息
			if (AID)
			{
				//随时处理滚动界限
				_DisposeScr(nullptr, SmoothScr ? ((UINT)(FrameRate * (ScrTime / 1000.0f))) : 0, &Bezier_SmoothScroll);
				mSendConMsg(DControlMsg::ScrollView_EndOfScroll, 0, 0);
			}
		}
		return;
	}

	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		//此处绘制滚动条

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}

};



class DClipRect : public DControl
{
public:
	virtual~DClipRect()
	{
		this->Release();
		return;
	}

	void Init(POINT position, SIZE size, bool CanMoveWindow = false)
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

		this->ControlType = DControlType::ClipRect;

		this->Inited = true;
		return;
	}
	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		ControlType = DControlType::ClipRect;
		RELEASE(brush);
		return;
	}

protected:
	ID2D1SolidColorBrush* brush = nullptr;

	void _SeparateFromWindow(bool FromParent)
	{
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
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
	void mSetPosition(float x, float y)
	{
		return;
	}
	void mSetSize(float width, float height)
	{
		return;
	}
	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		//扩展区域
		UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
		UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
		UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
		UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));
		return;
	}
	bool _SetCursorMsgProc()
	{
		return true;
	}
	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		return true;
	}
	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		return true;
	}
	bool _LMouseDownMsgProc(HWND hWnd, int xPos, int yPos)
	{
		return true;
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		return true;
	}
	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		return true;
	}
	bool _CreateHitTestGeo()//创建命中测试几何
	{
		___HitTestLock.Lock();
		RELEASE(HitTestGeo);
		if (D2DFactory)
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DstRC.rect.right - DstRC.rect.left, DstRC.rect.bottom - DstRC.rect.top }, 0.0f, 0.0f }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}
	bool _IsContentChanging()
	{
		if (A_RC_x || A_RC_y || A_RC_width || A_RC_height)
			return true;
		else
			return false;
	}
	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		_Base_DisposeAni();
		return;
	}
	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;

		Clip.PopAll(DstTarget);//弹出裁剪区
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return true;
	}
};


//被加入 DCombinedCon 中的控件称为组件，组件不会自发地响应鼠标以及各种消息，只有显示以及动画的效果，并且这些动画不会自发地显示。
//在将 DCombinedCon 添加到窗口时，其组件不会添加到窗口中，这意味着实际添加的只有一个控件，即此控件。
//库用户以继承的方式使用此控件，如果用户没有继承或没有添加组件而直接将其添加到窗口，那么此控件的作用和没有字的 DLable 类似，不过是多了一个圆角，但只存在于鼠标响应检测中能体现。
//库用户在继承的类中，可以实现自己的初始化函数，比如 MyInit() 用于初始化自己的组件等，要在自定义的初始化函数里调用 DCombinedCon::Init() 函数来初始化此组合控件，也可以另外调用。
//库用户可以在自定义的初始化函数，比如在 MyInit() 里添加此 DCombinedCon 的组件。
//库用户可以在继承此类后重载 On_XXX() 系列的函数，来设置相关组件变换颜色、大小等样式，来响应软件用户的操作。组件的响应完全由库用户自己控制。
//绘制顺序为：先绘制先加入到 DCombinedCon 的组件，然后绘制后来加入的。并且每一次刷新都会重新绘制所有组件。
//由于已经确定的绘制顺序，允许了控件半遮盖的情况存在，可以打破 DXUI 原有的控件不允许半遮盖的绘制限制。
class DCombinedCon : public DControl
{
public:
	virtual~DCombinedCon()
	{
		this->Release();
		return;
	}

	//设置当前组合控件鼠标命中探测的圆角矩形，不会影响其组件
	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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

protected:
	ID2D1SolidColorBrush* brush = nullptr;
	struct __comp
	{
		DControl* con;
		DControl* clip;
	};

	std::vector<__comp> Cons;
	___DThreadLock ___ConsLock;

	//以下On方法，用户可以重载。内部会在合适的时间调用，不要直接使用

	//位置改变时
	virtual void On_SetPosition(float x, float y)//设备<独立>像素
	{
		return;
	}
	//大小改变时
	virtual void On_SetSize(float width, float height)//设备<独立>像素
	{
		return;
	}
	//鼠标在此移动时
	virtual void On_MouseMove(int xPos, int yPos)//设备<独立>像素
	{
		return;
	}
	//鼠标左键在此按下时
	virtual void On_LMouseDown(int xPos, int yPos)//设备<独立>像素
	{
		return;
	}
	//鼠标左键在此抬起时
	virtual void On_LMouseUp(int xPos, int yPos)//设备<独立>像素
	{
		return;
	}
	//状态改变时
	virtual void On_StateChange(DControlState Last, DControlState Now)
	{
		return;
	}

	//在每一帧时
	//【特别注意】：不要直接在里面进行控件更新操作，要增加条件判断。否则每一帧都会导致下一帧刷新，会造成该控件持续刷新。
	//不要在此函数内进行太耗时的操作，否则会影响此控件刷新时整个窗口的流畅度。可以设置某些控件的样式风格等，这些不太耗时。
	virtual void On_Frame()
	{
		return;
	}

	//*************************DXUI自用函数*************************

	void _SeparateFromWindow(bool FromParent)
	{
		//重新设置动画颜色，防止下一次加入窗口时还没恢复状态
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		_SeparateDisposeCon(FromParent);//处理父子控件关系，必须在清除Parent、Child的上面
		_ClearChildCon();
		//释放父控件偏移
		___PosOfsLock.Lock();
		PosOffset.clear();
		___PosOfsLock.Unlock();

		OnClick = false;//important
		//释放位置改变影响的区域
		___RcChangedLock.Lock();
		RectChanged = 0;
		LastRect = { 0 };
		LastLastRect = { 0 };
		___RcChangedLock.Unlock();

		ParentDrawing = false;
		Parent = nullptr;
		ReleaseDeviceDependentRes();//把所有设备相关资源都释放
		_SetMsgBack(nullptr, 0);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;

		//通知组件与窗口分离，仅是为了清除资源
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					(*it->con)._SeparateFromWindow(false);
				++it;
			}
			Cons.clear();
		}
		___ConsLock.Unlock();
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
		//这里不能获取组件的ContentChanging，因为组件可能是局部改变，这里要全局改变才行
		if (A_RC_x || A_RC_y || A_RC_width || A_RC_height || A_Radius_X || A_Radius_Y || A_Opa)
			return true;
		else
			return false;
	}
	//添加组件。这里的ClipCon加入后，会以 ClipCon 的控件区域来裁剪，设置为空则不裁剪，但全部组件都不能超出此 DCombinedCon，否则超出部分会裁剪
	bool AddComponent(DControl* Comp, DControl* ClipCon = nullptr)
	{
		if (!Comp)
			return false;
		___ConsLock.Lock();

		DControl* con = nullptr, *clip = nullptr;
		std::vector<__comp>::iterator it = Cons.begin();
		while (it != Cons.end())
		{
			if (it->con && it->con == Comp)
				con = Comp;
			if (it->con && it->con == ClipCon)
				clip = ClipCon;
			++it;
		}

		if (!con && (ClipCon ? (bool)clip : true) && Comp != this && ClipCon != this && Comp->IsInit())
		{
			Cons.push_back({ Comp, ClipCon });

			//将新数据传递给组件
			if (ID != 0)//如果组合控件已经添加到窗口
			{
				Comp->Dpi = this->Dpi;
				Comp->_UpdateDpi();
				Comp->FrameRate = this->FrameRate;
				Comp->_SetFactory(D2DFactory, WicFactory, DWriteFactory);
			}
			___ConsLock.Unlock();
			this->_SetNeedUpdate();//刷新一下
			return true;
		}
		else
		{
			___ConsLock.Unlock();
			return false;
		}
	}

	bool DeleteComponent(DControl* Comp)
	{
		if (!Comp)
			return false;
		___ConsLock.Lock();

		std::vector<__comp>::iterator it = Cons.begin();
		while (it != Cons.end())
		{
			if (it->con && it->con == Comp)
				break;
			++it;
		}

		if (it->con)
		{
			it->con->_SeparateFromWindow(false);//通知控件与窗口分离
			Cons.erase(it);
			___ConsLock.Unlock();
			this->_SetNeedUpdate();//刷新一下
			return true;
		}
		else
		{
			___ConsLock.Unlock();
			return false;
		}
	}

	bool Init(POINT position, SIZE size, float radiusX = 0.0f, float radiusY = 0.0f, bool CanMoveWindow = false)
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
		_UpdateDpi();

		___ConsLock.Lock();
		Cons.clear();
		___ConsLock.Unlock();

		this->ControlType = DControlType::CombinedCon;
		this->Inited = true;
		return true;
	}

	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量(里面会调用_SeparateFromWindow，通知控件与窗口分离)
			return;

		ControlType = DControlType::CombinedCon;
		RELEASE(brush);
		___ConsLock.Lock();
		Cons.clear();
		___ConsLock.Unlock();

		return;
	}

	void _SetFrameRate(UINT Rate) noexcept//DCombinedCon要重载此函数
	{
		FrameRate = Rate;

		//通知组件设置帧率
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					(*it->con).FrameRate = this->FrameRate;
				++it;
			}
		}
		___ConsLock.Unlock();
		return;
	}

	void _SetFactory(ID2D1Factory1* const D2DFactory, IWICImagingFactory* const WicFactory, IDWriteFactory* const DWriteFactory)
	{
		this->D2DFactory = D2DFactory;
		this->WicFactory = WicFactory;
		this->DWriteFactory = DWriteFactory;
		_CreateHitTestGeo();//创建命中测试几何

		//通知组件设置Factory
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					(*it->con)._SetFactory(D2DFactory, WicFactory, DWriteFactory);
				++it;
			}
		}
		___ConsLock.Unlock();
		return;
	}

	//特别注意：接收的rc是经过PosOfs转换过的。只有当父控件调用子控件，或控件自身调用时，需要设定CalcSigma为true
	void _SetNeedUpdate(const D2D1_RECT_F ClipRect, const bool CalcSigma = true)//DCombinedCon 要重载此函数，并且不给组件设置ClipRect，在Draw时会在此控件的Clip中绘制组件
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
				std::vector<D2D1_RECT_F>::iterator updIt = --UpdateClip.end();
				if (!_DXUI_DetectRectContain(*updIt, r))
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

		//通知组件更新，必须是全局更新，组件UpdClip没有实际作用
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					(*it->con)._SetNeedUpdate();
				++it;
			}
		}
		___ConsLock.Unlock();
		return;
	}

	//特别注意：接收的rc是经过PosOfs转换过的。只有当父控件调用子控件，或控件自身调用时，需要设定CalcSigma为true
	void _SetNeedUpdate(bool need = true)//DCombinedCon 要重载此函数，DWindow通过此函数设置，此控件借此机会将组件的NeedUpdate修改，并且不给组件设置ClipRect，在Draw时会在此控件的Clip中绘制组件
	{
		___NeedUpdateLock.Lock();
		___UpdateClipLock.Lock();
		UpdateClip.clear();
		NeedUpdate = need;
		___UpdateClipLock.Unlock();
		___NeedUpdateLock.Unlock();

		//通知组件更新，必须是全局更新，组件UpdClip没有实际作用
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					(*it->con)._SetNeedUpdate(need);
				++it;
			}
		}
		___ConsLock.Unlock();
		return;
	}


	bool _GetNeedUpdate() noexcept//文本框要重写此函数，因为显示插入符需要计时，DCombinedCon 要重载此函数
	{
		bool res = NeedUpdate;
		//获取组件NeedUpdate，当有一个为NeedUpdate，则退出返回true
		___ConsLock.Lock();
		___UpdateClipLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					res = res || (*it->con)._GetNeedUpdate();
				if (res)break;
				++it;
			}
		}
		___UpdateClipLock.Unlock();
		___ConsLock.Unlock();

		return res;
	}
	bool _GetOneMoreFrame() noexcept//为主窗口提供获取OneMoreFrame的方法，DCombinedCon 要重载此函数
	{
		bool res = OneMoreFrame;
		//获取组件 OneMoreFrame，一真则真，全假则假
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					res = res || (*it->con)._GetOneMoreFrame();
				if (res) break;
				++it;
			}
		}
		___ConsLock.Unlock();

		return res;
	}

	void _SetOneMoreFrame(bool OneMore) noexcept//为主窗口提供设置OneMoreFrame的方法，DCombinedCon 要重载此函数
	{
		OneMoreFrame = OneMore;
		//通知组件做出改动
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					(*it->con)._SetOneMoreFrame(OneMore);
				++it;
			}
		}
		___ConsLock.Unlock();
		return;
	}

	void mSetPosition(float x, float y)
	{
		On_SetPosition(x, y);//调用用户函数

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();
		return;
	}
	void mSetSize(float width, float height)
	{
		On_SetSize(width, height);//调用用户函数

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		return;
	}

	bool m_SetState(DControlState state, bool NeedUnlock = false)
	{
		if (State != state)
		{
			DControlState LastState = State;
			State = state;
			this->_SetNeedUpdate();
			mSendConMsg(DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);
			if (NeedUnlock)
				___StateLock.Unlock();

			On_StateChange(LastState, State);//调用用户函数
			return true;
		}
		if (NeedUnlock)
			___StateLock.Unlock();
		return false;
	}

	void _UpdateDpi()
	{
		//将接收到的设备独立像素转换为设备相关像素
		//近似区域
		DstRC.rect.left = roundf(DIP2DP(DstRC_DIP.rect.left));
		DstRC.rect.top = roundf(DIP2DP(DstRC_DIP.rect.top));
		DstRC.rect.right = roundf(DIP2DP(DstRC_DIP.rect.right));
		DstRC.rect.bottom = roundf(DIP2DP(DstRC_DIP.rect.bottom));
		_UpdateDpi_DstRCRadius();
		//扩展区域
		UpdateRC.left = floorf(DIP2DP(DstRC_DIP.rect.left));
		UpdateRC.top = floorf(DIP2DP(DstRC_DIP.rect.top));
		UpdateRC.right = ceilf(DIP2DP(DstRC_DIP.rect.right));
		UpdateRC.bottom = ceilf(DIP2DP(DstRC_DIP.rect.bottom));

		//通知组件处理DPI
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
				{
					(*it->con).Dpi = this->Dpi;
					(*it->con)._UpdateDpi();
				}
				++it;
			}
		}
		___ConsLock.Unlock();
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
		On_MouseMove((int)roundf((float)xPos * 96 / Dpi), (int)roundf((float)yPos * 96 / Dpi));//调用用户函数

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
		On_LMouseDown((int)roundf((float)xPos * 96 / Dpi), (int)roundf((float)yPos * 96 / Dpi));//调用用户函数

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
		On_LMouseUp((int)roundf((float)xPos * 96 / Dpi), (int)roundf((float)yPos * 96 / Dpi));//调用用户函数

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
			D2DFactory->CreateRoundedRectangleGeometry({ { 0.0f, 0.0f, DstRC.rect.right - DstRC.rect.left, DstRC.rect.bottom - DstRC.rect.top }, DstRC.radiusX, DstRC.radiusY }, &HitTestGeo);

		___HitTestLock.Unlock();
		return HitTestGeo;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		On_Frame();//调用用户函数

		_Base_DisposeAni();

		if (A_Radius_X || A_Radius_Y)
		{
			UINT AID = 0;
			float x = DstRC_DIP.radiusX, y = DstRC_DIP.radiusY;
			bool resp = false;
			if (A_Radius_X)
			{
				x = A_Radius_X.FrameCalc(&AID);
				if (!A_Radius_X)
					resp = true;
			}
			if (A_Radius_Y)
			{
				y = A_Radius_Y.FrameCalc(&AID);
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
			SetTotalOpacity(A_Opa.FrameCalc(&AID), 0, nullptr, true, 0, false);
			EndAniSend(AID);
		}

		//通知组件处理动画
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
					(*it->con)._DisposeAnimation();
				++it;
			}
		}
		___ConsLock.Unlock();
		return;
	}
	//由于当组件移动时，不会超出此组合控件的区域，并且此组合控件移动时会覆盖Flip模型的2帧，所以不用理会组件的UpdateClip
	bool _Draw(ID2D1DeviceContext* DstTarget, const D2D1_COLOR_F BackColor, D2D1_RECT_F* UpdateClip)
	{
		if (!this->_DrawDispose(DstTarget, BackColor, UpdateClip, brush))
			return false;
		//弹出
		DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		float pofs = GetPosOffset();
		//通知组件绘制
		___ConsLock.Lock();
		if (Cons.size())
		{
			std::vector<__comp>::iterator it = Cons.begin();
			while (it != Cons.end())
			{
				if (it->con)
				{
					Clip.PushClip(DstTarget, _GetUpdateRC());//获取自身的UpdateRC，包含Ofs
					if (pofs != 0.0f)//设置滚动偏移变换
						DstTarget->SetTransform(D2D1::Matrix3x2F::Translation(0.0f, -pofs));

					if (it->clip)//加入裁剪控件的Clip (因为组件没有Ofs，所以放在设置变换的下面)
						Clip.PushClip(DstTarget, (*it->clip)._GetUpdateRC());

					//每一帧都清除组件的UpdateClip，防止积累过多
					(*it->con).UpdateClip.clear();
					//将清屏颜色透明度设为0.0f，防止底层内容消失
					(*it->con)._Draw(DstTarget, BackColor, nullptr);

					if (it->clip)
						Clip.PopLast(DstTarget);
					Clip.PopLast(DstTarget);
					//弹出裁剪控件的裁剪区域
					DstTarget->SetTransform(D2D1::Matrix3x2F::Identity());
				}
				++it;
			}
		}
		___ConsLock.Unlock();

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

	bool Create(HINSTANCE hInst, HWND MainHwnd, float Radius, float Alpha, int Dpi)
	{
		if (!hInst || !MainHwnd)
			return false;
		//创建窗口
		___DrawLock.Lock();
		this->Dpi = Dpi;
		radius = Radius;
		alpha = Alpha;
		EdgeOut = (UINT)(DIP2DP(radius) * DIP2DP(5.0f));
		GetWindowRect(MainHwnd, &MainRC);

		hWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT, L"DXUI Class", nullptr, WS_POPUP,
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
				HPEN Pen = CreatePen(PS_SOLID, (int)ceilf(Shadow_StrokeWidth), RGB(0, 0, 0));
				HPEN OldPen = (HPEN)SelectObject(dc, Pen);
				HBRUSH WhiteBru = CreateSolidBrush(RGB(255, 255, 255));
				HBRUSH OldBru = (HBRUSH)SelectObject(dc, WhiteBru);

				UINT leftWid = Min(EdgeOut * 2 + (UINT)Shadow_StrokeWidth, EdgeOut + (MainRC.right - MainRC.left) / 2);
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

				Rectangle(dc, EdgeOut + (int)ceilf(Shadow_StrokeWidth / 2.0f), EdgeOut + (int)ceilf(Shadow_StrokeWidth / 2.0f), leftWid + (int)floorf(Shadow_StrokeWidth * 2.0f), rc.bottom - EdgeOut - (int)floorf(Shadow_StrokeWidth / 2.0f));

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
				tbi.bmiHeader.biHeight = EdgeOut + (int)ceilf(Shadow_StrokeWidth * 2.0f);
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

					MoveToEx(dc, 0, EdgeOut + (int)ceilf(Shadow_StrokeWidth / 2.0f), nullptr);
					LineTo(dc, topWid, EdgeOut + (int)ceilf(Shadow_StrokeWidth / 2.0f));

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
						pAlphaBlend(MemDC, leftWid, 0, topWid, tbi.bmiHeader.biHeight, dc, 0, 0, topWid, tbi.bmiHeader.biHeight, blendFunc);
						//垂直翻转
						tbi.bmiHeader.biHeight = -tbi.bmiHeader.biHeight;
						SetDIBits(dc, Top, 0, -tbi.bmiHeader.biHeight, topBits, &tbi, DIB_RGB_COLORS);
						//Bottom绘制
						pAlphaBlend(MemDC, leftWid, rc.bottom + tbi.bmiHeader.biHeight, topWid, -tbi.bmiHeader.biHeight, dc, 0, 0, topWid, -tbi.bmiHeader.biHeight, blendFunc);
						SelectObject(dc, OldBmp);
					}
					//Left绘制
					SelectObject(dc, Left);
					pAlphaBlend(MemDC, 0, 0, leftWid, rc.bottom, dc, 0, 0, leftWid, rc.bottom, blendFunc);
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
						pAlphaBlend(MemDC, leftWid + topWid, 0, leftWid, rc.bottom, dc, 0, 0, leftWid, rc.bottom, blendFunc);

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
	inline void SetShadowOpacity(float opa, UINT time, const DBezier* Nonlinear, UINT FrameRate)
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
			UINT aid = 0;
			alpha = A_Sha_Opa.FrameCalc(&aid);
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


//每个程序只执行一次，设置进程DPI感知
inline char _IsSetDPIAware = 0;//0=还未设置, 1=设置成功, 2=设置失败，以后不再设置
inline void _SetDPIAware()
{
	if (_IsSetDPIAware != 0)
		return;

	bool res = true;
	if (!pSetProcessDpiAwareness || FAILED(pSetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE)))
		res = false;

	if (!res)//如果高版本Windows的方法失败，那使用低版本的方法，缺点就是没有了每个屏幕的高DPI支持
		res = SetProcessDPIAware();

	_IsSetDPIAware = res ? 1 : 2;

	_DXUI_DBG_ASSERT(!res, 1008);
	return;
}

class DWindow
{
public:
	DWORD ExStyle = 0;
	DWORD Style = WS_SYSMENU | WS_MINIMIZEBOX | WS_POPUP;
	HICON hIcon = nullptr;
	HICON hIconSmall = nullptr;
	HWND Parent = nullptr;

	DWindow()
	{
		_DXUI_Init();//初始化，一个进程只需执行一次
		_SetDPIAware();
		if (_DXUI_SuccessInit == 0)
			_DXUI_ASSERT(FAILED(pCoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)), 1002);
	}

	virtual~DWindow()
	{
		Release();
		if (_DXUI_SuccessInit == 0)
			pCoUninitialize();
		return;
	}

	//定义隐式转换，此类可被看作为一个窗口句柄
	operator HWND()
	{
		return this->hWnd;
	}

	bool Create(DString Title, int width, int height)
	{
		this->hInst = GetModuleHandle(nullptr);
		if (Inited || hWnd || !hInst || Title.empty() || width < 1 || height < 1)
			return false;

		this->Title = Title;

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		//这里不能写死为W，否则g++这类编译器编译时窗口标题等会乱码
#ifdef UNICODE
		if (!GetClassInfoEx(hInst, _T("DXUI Class"), &wcex))
#else
		if (!GetClassInfoEx(hInst, _T("DXUI Class"), &wcex))
#endif
		{
			//窗口未注册，注册
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_GLOBALCLASS;
			wcex.lpfnWndProc = ___DXUIWndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hInst;
			wcex.hIcon = hIcon;
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = nullptr;
			wcex.lpszMenuName = nullptr;
#ifdef UNICODE
			wcex.lpszClassName = _T("DXUI Class");
#else
			wcex.lpszClassName = _T("DXUI Class");
#endif
			wcex.hIconSm = hIconSmall;
			if (!RegisterClassEx(&wcex))
			{
				_DXUI_ERROR(1003);
				return false;
			}
		}

		Dpi = _DXUI_GetWndDpi(nullptr);//获取一次DPI
		//这里的窗口大小要根据DPI的大小来改变，所以用了一套公式重新计算大小
		int X, Y, mw = Max(width, 1), mh = Max(height, 1);
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

		//创建窗口
#ifdef UNICODE
		this->hWnd = CreateWindowEx(ExStyle | (TransparWindow ? WS_EX_LAYERED : 0), _T("DXUI Class"), Title.c_strW(), Style,
			X, Y, mw * Dpi / 96, mh * Dpi / 96, Parent, nullptr, hInst, nullptr);
#else
		this->hWnd = CreateWindowEx(ExStyle | (TransparWindow ? WS_EX_LAYERED : 0), _T("DXUI Class"), Title.c_strA(), Style,
			X, Y, mw * Dpi / 96, mh * Dpi / 96, Parent, nullptr, hInst, nullptr);
#endif
		if (!hWnd)
		{
			_DXUI_ERROR(1004);
			return false;
		}

		//将此DWindow加入窗口列表中，以便能接受到消息
		___WndListLock.Lock();
		___WndList.insert(std::pair<HWND, void*>(hWnd, (void*)this));
		___WndListLock.Unlock();
		//获取一次DPI
		int mDpiWnd = _DXUI_GetWndDpi(hWnd);
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

	inline void SetShadowAlpha(float alpha, UINT time = 0, const DBezier* Nonlinear = nullptr)
	{
		Shadow.SetShadowOpacity(alpha, time, Nonlinear, FrameRate);
		return;
	}
	//初始化渲染，要先创建窗口再初始化，可以用Create方法
	bool Init(DMsgProc Proc, float ShadowRadius = 3.0f, float ShadowAlpha = 0.5f, bool Moved = true, UINT FrameRate = DefFrameRate, DCOLOR Background = { 255, 255, 255, 1.0f }, bool TransParent = false)
	{
		if (this->Inited || !hWnd || !Proc || _DXUI_SuccessInit != 0)//条件检测
			return false;

		EnableSizeChange = false;//***********保留功能
		//处理透明窗口
		TransparWindow = TransParent;
		SetWindowLongW(hWnd, GWL_EXSTYLE, (TransparWindow ? (GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED) : (GetWindowLongW(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED)));

		this->proc = Proc;
		this->FrameRate = Max(1, Min(60, FrameRate));
		this->BackColor.r = Background.r / 255.0f;
		this->BackColor.g = Background.g / 255.0f;
		this->BackColor.b = Background.b / 255.0f;
		this->BackColor.a = Background.a;
		this->CanBeMoved = Moved;

		Dpi = _DXUI_GetWndDpi(hWnd);//有窗口句柄后，重新根据句柄获取一次DPI
		//获取客户区大小
		GetClientRect(hWnd, &this->rc);

		HRESULT hr;
		//创建D2D上下文
		hr = this->CreateDevice();
		if (FAILED(hr))
		{
			//弹出错误
			_DXUI_ERROR(1005);
			this->Release();
			return false;
		}
		//创建Dwrite库
		hr = pDWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, IID{ 0xb859ee5a, 0xd838, 0x4b5b, 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 }, reinterpret_cast<IUnknown**>(&this->DWFactory));
		if (FAILED(hr))
		{
			//弹出错误
			_DXUI_ERROR(1006);
			this->Release();
			return false;
		}
		//创建WIC图像库。因为版本2不支持Win7，所以失败后继续测试版本1
		hr = pCoCreateInstance(IID{ 0x317d06e8, 0x5f24, 0x433d, 0xbd, 0xf7, 0x79, 0xce, 0x68, 0xd8, 0xab, 0xc2 }, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&this->WicFactory));

		if (FAILED(hr))
		{
			hr = pCoCreateInstance(IID{ 0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa }, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&this->WicFactory));
			if (FAILED(hr))
			{
				//弹出错误
				_DXUI_ERROR(1007);
				this->Release();
				return false;
			}
		}

		//创建窗口边缘阴影
		this->Shadow.Create(this->hInst, this->hWnd, ShadowRadius, 0.0f, Dpi);
		if (ShadowAlpha != 0.0f)//如果有阴影，则渐入阴影，否则可能显示时间不同
			this->SetShadowAlpha(ShadowAlpha, 250);
		//设置窗口消息
		this->ConMsg = RegisterWindowMessageW(RegisterMessageStr);
		this->Inited = true;
		//在创建渲染线程时会刷新
		return true;
	}

	void Release(bool FromDestroy = false)
	{
		if (!Inited)
			return;

		if (this->hWnd && !FromDestroy)//在DESTROY消息里会再次调用Release，返回为了确保消息接收完整
		{
			//退出渲染线程
			this->ExitRenderThread();
			//销毁窗口
			DestroyWindow(this->hWnd);
			return;
		}
		Inited = false;

		//在窗口列表里删除此窗口
		___WndListLock.Lock();
		___WndList.erase(hWnd);
		___WndListLock.Unlock();
		this->hWnd = nullptr;

		//删除阴影窗口
		if (Shadow)
			Shadow.Release();

		//将子控件全部分离，因为停止渲染线程了，所以可以先分离后删除控件。否则必须注意顺序
		___ConLock.Lock();
		if (Controls.size())
		{
			std::map<UINT, DControl*>::iterator con = Controls.begin();
			for (UINT i = 0; i < Controls.size(); ++i, ++con)
			{
				con->second->_SeparateFromWindow(false);
			}
			Controls.clear();
		}
		___ConLock.Unlock();

		Title.clear();
		A_Wnd_X.Reset();
		A_Wnd_Y.Reset();
		hInst = nullptr;
		rc = { 0 };
		Dpi = 0;
		proc = nullptr;
		ConMsg = 0;
		BackgroundBlur = false;
		HardwareRendering = true;
		EnableSizeChange = false;
		OnSizeChange = 0;
		TrackLeave = false;
		CanBeMoved = true;
		OnMove = false;
		ClickPos = { 0 };
		TransparWindow = false;
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
				Controls.erase(conit);
				//由于删除控件时，可能正在渲染，所以必须先从控件列表中删除，然后再调用控件的分离函数
				con->_SeparateFromWindow(false);
			}
		}
		___ConLock.Unlock();
		return;
	}
	//将设备相关像素转换为设备无关像素
	inline float DPToDIP(int dp) noexcept
	{
		return roundf(DP2DIP(dp));
	}
	//将设备无关像素转换为设备相关像素
	inline int DIPToDP(float dip) noexcept
	{
		return (int)roundf(DIP2DP(dip));
	}
	inline HINSTANCE GetHInst() noexcept
	{
		return hInst ? hInst : GetModuleHandle(nullptr);
	}
	inline bool IsInit() noexcept
	{
		return Inited;
	}
	inline HWND GetHwnd() noexcept
	{
		return hWnd;
	}
	inline int GetDpi() noexcept
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
			wid = (long)ceilf(wid / (float)Dpi * (float)dpi);
			hei = (long)ceilf(hei / (float)Dpi * (float)dpi);
			Dpi = dpi;//在此处才能设置
			Shadow.SetDpi(Dpi);
			SetWindowPos(hWnd, nullptr, 0, 0, Max(wid, 1), Max(hei, 1), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW | SWP_ASYNCWINDOWPOS);
			//获取客户区大小
			GetClientRect(hWnd, &this->rc);
			this->ReleaseDevice();
			this->CreateDevice(true);//此函数会通知子控件更新DPI
			UpdateAll();
		}
		___RenderMutex.Unlock();
		return;
	}
	/*//设置是否允许大小调节
	void SetSizeAdjust(bool Enable) noexcept
	{
		EnableSizeChange = Enable;
		return;
	}
	//获取是否允许大小调节
	bool GetSizeAdjust() noexcept
	{
		return EnableSizeChange;
	}*/

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
		//更新两次，填满前后缓冲区
		UpdateAll();
		_RenderFrame();
		UpdateAll();
		_RenderFrame();

		if (_beginthread(DXUI_RenderThread, 0, this) == -1)
			return false;
		else
			return true;
	}

	//获取当前窗口是否为背景透明窗口
	inline bool GetTransparentWindow() noexcept
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

	inline bool _GetExitRenderThreadFlag() noexcept
	{
		return ExitRenderThreadFlag;
	}

	inline void _SetRenderThreadAlive(bool alive) noexcept
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
				Sleep(2);
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

		std::vector<RECT> dirtyRect;
		___ConLock.Lock();
		if (Controls.size())
		{
			std::map<UINT, DControl::__updItem> UpdCon;
			std::map<UINT, DControl::__updItem>::iterator UpdConIt;
			std::pair<std::map<UINT, DControl::__updItem>::iterator, bool> insIt;
			std::map<UINT, DControl*>::iterator con = Controls.begin();
			std::vector<D2D1_RECT_F>::iterator updit;
			UINT i1, s;

			//**********************************************预处理一下，先把屏清了并收集更新区域，防止绘制时有变动
			for (UINT i = 0; i < (UINT)Controls.size(); ++i, ++con)
			{
				if (con->second)
				{
					if (thiscallUpdate)
						con->second->_SetNeedUpdate();

					con->second->___NeedUpdateLock.Lock();
					bool needUpd = con->second->_GetNeedUpdate();
					bool needOneM = con->second->_GetOneMoreFrame();
					if (needUpd || needOneM)
					{
						bool IsOneMore = !needUpd && needOneM;//是否为Flip模型多渲染的一帧
						con->second->___UpdateClipLock.Lock();
						insIt = UpdCon.insert(std::pair<UINT, DControl::__updItem>(con->first, { con->second, IsOneMore ? con->second->LastUpdateClip : con->second->UpdateClip }));

						if (IsOneMore)
						{
							con->second->_SetOneMoreFrame(false);
							con->second->LastUpdateClip.clear();
						}
						else
						{
							con->second->_SetOneMoreFrame(true);
							con->second->LastUpdateClip = con->second->UpdateClip;
						}
						con->second->UpdateClip.clear();//每一帧都要清空
						con->second->_SetNeedUpdate(false);
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

			//****************************************************************开始循环绘制
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
				s = (UINT)UpdConIt->second.UpdClip.size();
				if (s)//有UpdateClip，需要多次局部循环绘制
				{
					do
					{
						r1 = *updit;
						r1 = _DXUI_CalcRectOverlap({ (float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom }, r1);
						if (UpdConIt->second.con->Parent)
							r1 = _DXUI_CalcRectOverlap(UpdConIt->second.con->Parent->_GetUpdateRC(), r1);
						if (_DXUI_IsRectWork(r1) && UpdConIt->second.con->_Draw(D2DContext, BackColor, &r1))
						{
							dirtyRect.push_back({ (long)r1.left, (long)r1.top, (long)r1.right, (long)r1.bottom });
#if _DEBUG
							Dbg_DrawUpdateRect(D2DContext, r1);//调试时绘制刷新区域
#endif
						}

						++i1;
						++updit;
					} while (i1 < s);
				}
				else {//没有UpdateClip，仅需要单次绘制
					r1 = UpdConIt->second.con->_GetUpdateRC();
					r1 = _DXUI_CalcRectOverlap({ (float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom }, r1);
					if (UpdConIt->second.con->Parent)
						r1 = _DXUI_CalcRectOverlap(UpdConIt->second.con->Parent->_GetUpdateRC(), r1);
					if (_DXUI_IsRectWork(r1) && UpdConIt->second.con->_Draw(D2DContext, BackColor, nullptr))
					{
						dirtyRect.push_back({ (long)r1.left, (long)r1.top, (long)r1.right, (long)r1.bottom });
#if _DEBUG
						Dbg_DrawUpdateRect(D2DContext, r1);//调试时绘制刷新区域
#endif
					}
				}
				UpdConIt->second.con->_CallChildUpdateBackImg(D2DContext, UpdCon, UpdConIt, UpdConIt->second.UpdClip);
				++UpdConIt;
			}//while End

			if (Began)
				D2DContext->Flush();//提前提交给GPU绘制
			//**********************************************************集体通知当前活动控件计算下一帧数据
			UpdConIt = UpdCon.begin();
			while (UpdConIt != UpdCon.end())
			{
				UpdConIt->second.con->_DisposeAnimation();
				++UpdConIt;
			}
		}
		___ConLock.Unlock();

		//************************************************************************呈现到屏幕
		if (Began)
		{
			HRESULT hr = S_OK;
			if (Began)
				hr = D2DContext->EndDraw();//对于DeviceContext，只需在Present时处理返回即可

			if (TransparWindow)//Trans时需要判断EndDraw返回值，其它时候不需要
			{
				if (SUCCEEDED(hr))
				{
					UINT dtrSz = (UINT)dirtyRect.size();
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
				if (thiscallUpdate || !HardwareRendering)//全部刷新或窗口由屏幕外移动进屏幕时，需要刷新区域。软件渲染时也需要整个刷新，否则会有轻微卡顿。
					hr = SwapChain->Present1(1, 0, &p);
				else
				{
					p.DirtyRectsCount = (UINT)dirtyRect.size();
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

	//DXUI处理Win消息的函数。返回值为处理消息后反馈给Windows的值，没有活动控件返回-1
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
		if (hWnd != this->hWnd || !Inited)
			return -1;

		//先处理给用户发一下消息
		DMsgProc p = proc;
		if (p)
		{
			if (message == ConMsg)//控件消息，通知用户
			{
				___DXUISendConMsg* m = (___DXUISendConMsg*)lParam;
				if (m)
				{
					p(hWnd, m->ID, m->Type, m->UserNamedType, m->Msg, message, m->wParam, m->lParam);
					delete m;
				}
				return 0;
			}
			else
				p(hWnd, 0, DControlType::Null, DefUserNamedType, DControlMsg::Null, message, wParam, lParam);
		}

		LRESULT ReturnValue = -1;
		if (Controls.size())
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

				if (!IsZoomed(hWnd))//如果没有在最大化
				{
					//如果在OnMove，那就手动移动或调节大小
					if (OnMove)
					{
						A_Wnd_X.Reset();
						A_Wnd_Y.Reset();

						RECT NowRC;
						if (GetWindowRect(hWnd, &NowRC))
						{
							//如果在大小调节状态
							if (EnableSizeChange && OnSizeChange != 0)
							{
								int nx = NowRC.left, ny = NowRC.top, nw = NowRC.right - NowRC.left, nh = NowRC.bottom - NowRC.top;
								if (OnSizeChange == 1)//左
								{
								}
								else if (OnSizeChange == 2)//右
									nw += xPos - ClickPos.x;
								else if (OnSizeChange == 3)//上
								{
								}
								else if (OnSizeChange == 4)//下
									nh += yPos - ClickPos.y;
								else if (OnSizeChange == 5)//左上
								{
								}
								else if (OnSizeChange == 6)//右上
								{
								}
								else if (OnSizeChange == 7)//左下
								{
								}
								else if (OnSizeChange == 8)//右下
								{
								}
								SetWindowPos(hWnd, nullptr, nx, ny, nw, nh, SWP_NOZORDER | SWP_NOREDRAW | SWP_ASYNCWINDOWPOS);
							}
							else
							{
								SetWindowPos(hWnd, nullptr, NowRC.left + xPos - ClickPos.x, NowRC.top + yPos - ClickPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW | SWP_ASYNCWINDOWPOS);
							}
							ReturnValue = 0;//如果程序处理此消息。返回值必须为0
							break;
						}
					}
					else
					{
						//判断是否在调节区域内
						if (EnableSizeChange)
							mCalcPos(xPos, yPos);
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

					UINT conID = con->second->GetID();

					con->second->_MouseMoveMsgProc(xPos, yPos);
					if (con->second->State != DControlState::Normal)
					{
						___ActConLock.Lock();
						ActiveCon.insert(std::pair<UINT, DControl*>(conID, con->second));
						___ActConLock.Unlock();
					}
					___ConLock.Unlock();

					//通知给用户
					if (p)
						p(hWnd, conID, con->second->ControlType, con->second->UserNamedType, DControlMsg::Control_MouseMove, 0, lParam, 0);
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
				//设置大小调节光标
				if (!IsZoomed(hWnd) && EnableSizeChange && OnSizeChange != 0)
				{
					if (OnSizeChange == 1 || OnSizeChange == 2)
						SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
					else if (OnSizeChange == 3 || OnSizeChange == 4)
						SetCursor(LoadCursor(nullptr, IDC_SIZENS));
					else if (OnSizeChange == 5 || OnSizeChange == 8)
						SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
					else if (OnSizeChange == 6 || OnSizeChange == 7)
						SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
					else
						break;
					ReturnValue = 0;//处理后应置为0
					break;
				}

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
						ReturnValue = 0;//处理后应置为0
				}
				___ConLock.Unlock();

				break;
			}
			case WM_MOVE:
			{
				if (p)
					p(hWnd, 0, DControlType::Null, DefUserNamedType, DControlMsg::Window_Position, 0, GET_X(lParam), GET_Y(lParam));
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
					UINT conID = con->second->GetID();
					RestoreLastActiveControl(conID, message);

					con->second->_LMouseDownMsgProc(hWnd, xPos, yPos);

					con->second->_ImeComposition(hWnd, WM_IME_STARTCOMPOSITION, wParam, lParam);

					___ConLock.Unlock();

					//通知给用户
					if (p)
						p(hWnd, conID, con->second->ControlType, con->second->UserNamedType, DControlMsg::Control_LButtonDown, 0, lParam, 0);
				}
				else
				{
					___ConLock.Unlock();
					RestoreLastActiveControl(0, message);
				}

				//判断按下区域是否在调节区域内
				if (EnableSizeChange)
					mCalcPos(xPos, yPos);
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
					UINT conID = con->second->GetID();
					RestoreLastActiveControl(conID, message, true);

					bool Disposed = con->second->_LMouseUpMsgProc(xPos, yPos);
					___ConLock.Unlock();

					//通知给用户
					if (p)
						p(hWnd, conID, con->second->ControlType, con->second->UserNamedType, DControlMsg::Control_LButtonUp, 0, lParam, 0);
					p = proc;//多调用一次
					if (Disposed && p)
						p(hWnd, conID, con->second->ControlType, con->second->UserNamedType, DControlMsg::Control_Click, 0, lParam, 0);
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
					std::map<UINT, DControl*>::iterator con = ActiveCon.end();
					do
					{
						--con;
						if (con->second->_KeyDownMsg(wParam))
						{
							ReturnValue = 0;
							break;
						}
					} while ((con != ActiveCon.begin()));
				}
				___ActConLock.Unlock();
				break;
			}
			case WM_IME_STARTCOMPOSITION:
			{
				___ActConLock.Lock();
				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.end();
					do
					{
						--con;
						if (con->second->_ImeComposition(hWnd, message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
					} while ((con != ActiveCon.begin()));
				}
				___ActConLock.Unlock();
				break;
			}
			case WM_IME_REQUEST:
			{
				___ActConLock.Lock();
				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.end();
					do
					{
						--con;
						if (con->second->_ImeComposition(hWnd, message, wParam, lParam))
						{
							ReturnValue = 1;
							break;
						}
					} while ((con != ActiveCon.begin()));
				}
				___ActConLock.Unlock();
				break;
			}
			case WM_IME_CHAR:
			{
				___ActConLock.Lock();

				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.end();
					do
					{
						--con;
						if (con->second->_CharMsg(message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
					} while ((con != ActiveCon.begin()));
				}
				___ActConLock.Unlock();
				break;
			}
			case WM_IME_COMPOSITION:
			{
				___ActConLock.Lock();
				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.end();
					do
					{
						--con;
						if (con->second->_ImeComposition(hWnd, message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
					} while ((con != ActiveCon.begin()));
				}
				___ActConLock.Unlock();
				break;
			}
			case WM_CHAR://与WM_IME_CHAR完全一样，因为_CharMsg对两种字符消息都接收
			{
				___ActConLock.Lock();

				if (ActiveCon.size())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.end();
					do
					{
						--con;
						if (con->second->_CharMsg(message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
					} while ((con != ActiveCon.begin()));
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
			case WM_GETMINMAXINFO://窗口大小改变时限制大小
			{
				MINMAXINFO* r = (MINMAXINFO*)lParam;
				if (r)
				{
					RECT rt;
					SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rt, 0);
					r->ptMaxPosition.x = Max(r->ptMaxPosition.x, rt.left);
					r->ptMaxPosition.y = Max(r->ptMaxPosition.y, rt.top);
					r->ptMaxSize.x = Min(r->ptMaxSize.x, rt.right - rt.left);
					r->ptMaxSize.y = Min(r->ptMaxSize.y, rt.bottom - rt.top);
					ReturnValue = 0;
				}
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
				if (p)
					p(hWnd, 0, DControlType::Null, DefUserNamedType, DControlMsg::Window_Destroy, 0, 0, 0);
				this->Release(true);//释放类
				ReturnValue = 0;
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
		}
		return ReturnValue;
	}

	//设备 <相关> 像素，需要时用 DIPToDP() 转换
	void SetPosition(int x, int y, UINT time = 0, const DBezier* Nonlinear = nullptr, UINT AID = 0, bool _Reserved = true)
	{
		if (_Reserved)
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
	//设备 <相关> 像素，需要时用 DIPToDP() 转换。如果有父窗口，则默认以父窗口的中心为准居中。
	void SetSize(int wid, int hei, bool SetCenter = true)
	{
		if (SetCenter)
		{
			//这里的窗口大小要根据DPI的大小来改变，所以用了一套公式重新计算大小
			int X, Y;
			if (Parent)//如果有父窗口，此时的位置应在父窗口的正上方
			{
				RECT ParRc;
				GetWindowRect(Parent, &ParRc);
				X = (ParRc.left + ParRc.right - (int)wid) / 2;
				Y = (ParRc.top + ParRc.bottom - (int)hei) / 2;
			}
			else {
				X = (GetSystemMetrics(SM_CXSCREEN) - (int)wid) / 2;
				Y = (GetSystemMetrics(SM_CYSCREEN) - (int)hei) / 2;
			}
			SetWindowPos(hWnd, nullptr, X, Y, wid, hei, SWP_NOZORDER | SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		}
		SetWindowPos(hWnd, nullptr, 0, 0, wid, hei, SWP_NOMOVE | SWP_NOZORDER | SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
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
		//传递主窗口的dpi，此函数会设置UpdateRC。要先传递DPI再传递Factory，其内部会根据此创建命中测试几何
		control->Dpi = Dpi;
		control->_UpdateDpi();
		//传递帧率给控件
		control->_SetFrameRate(FrameRate);
		//给控件传递Factory
		control->_SetFactory(D2DFactory, WicFactory, DWFactory);
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
		//重置控件状态
		control->_SetNormalState(this->hWnd, 0);
		//最后插入到控件列表
		Controls.insert(std::pair<UINT, DControl*>(ID, control));
		___ConLock.Unlock();

		return true;
	}

#if _DEBUG//调试选项，只有Debug模式才有
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
#endif

private:
#if _DEBUG
	bool Dbg_ShowUpdate = false;
#endif
	bool HardwareRendering = true;
	bool BackgroundBlur = false;
	bool TransparWindow = false;
	___DThreadLock ___TransLock;

	bool Inited = false;

	std::wstring Title;
	HINSTANCE hInst = nullptr;
	HWND hWnd = nullptr;
	RECT rc = { 0 };
	int Dpi = 0;

	_AniCalc_l A_Wnd_X;
	_AniCalc_l A_Wnd_Y;
	DMsgProc proc = nullptr;
	UINT ConMsg = 0;
	__DWinShadow Shadow;

	//大小调节相关
	bool EnableSizeChange = false;
	UINT OnSizeChange = 0;//0=未在调节区域，1=左，2=右，3=上，4=下，5=左上，6=右上，7=左下，8=右下

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

#if _DEBUG
	void Dbg_DrawUpdateRect(ID2D1DeviceContext* const D2DContext, const D2D1_RECT_F& r)
	{
		if (Dbg_ShowUpdate)
		{
			srand((UINT)(rand() + (_DXUI_SuccessInit == 0 ? ptimeGetTime() : 0)));
			ID2D1SolidColorBrush* b;
			D2DContext->CreateSolidColorBrush({ (float)rand() / RAND_MAX + 0.1f, (float)rand() / RAND_MAX + 0.1f, (float)rand() / RAND_MAX + 0.1f, 1.0f }, &b);
			if (b)
			{
				D2DContext->DrawRectangle({ r.left + DIP2DP(0.75f), r.top + DIP2DP(0.75f), r.right - DIP2DP(0.75f), r.bottom - DIP2DP(0.75f) }, b, DIP2DP(1.5f));
				b->Release();
			}
		}
		return;
	}
#endif

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
		if (!hWnd || _DXUI_SuccessInit != 0)
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
		hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flags, FeatureLevels, 7, D3D11_SDK_VERSION, &D3DDevice, &FeatureLevel, &D3DContext);

		if (SUCCEEDED(hr))
			HardwareRendering = true;
		else if (hr == DXGI_ERROR_UNSUPPORTED)//不支持GPU渲染，切换CPU渲染
		{
			hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, Flags, FeatureLevels, 7, D3D11_SDK_VERSION, &D3DDevice, &FeatureLevel, &D3DContext);
			if (SUCCEEDED(hr))
				HardwareRendering = false;
		}

		if (SUCCEEDED(hr))
		{
			//通过 D3D设备 获取 DXGI设备
			hr = D3DDevice->QueryInterface(IID{ 0x77db970f, 0x6276, 0x48ba, 0xba, 0x28, 0x07, 0x01, 0x43, 0xb4, 0x39, 0x2c }, (void**)&DXGIDevice);
			if (SUCCEEDED(hr))
			{
				//创建多线程 D2D Factory
#ifdef _DEBUG//如果为Debug模式，则启用D2D调试层
				D2D1_FACTORY_OPTIONS options;
				options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
				hr = pD2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID{ 0xbb12d362, 0xdaee, 0x4b9a, 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f }, &options, reinterpret_cast<void**>(&this->D2DFactory));
#else
				hr = pD2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID{ 0xbb12d362, 0xdaee, 0x4b9a, 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f }, nullptr, reinterpret_cast<void**>(&this->D2DFactory));
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
									SwapChainDesc.Stereo = false;
									SwapChainDesc.SampleDesc.Count = 1;//Sample必须为1和0
									SwapChainDesc.SampleDesc.Quality = 0;
									SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
									SwapChainDesc.BufferCount = HardwareRendering ? 2 : 1;
									SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
									SwapChainDesc.Scaling = DXGI_SCALING_NONE;
									SwapChainDesc.SwapEffect = HardwareRendering ? DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL : DXGI_SWAP_EFFECT_SEQUENTIAL;
									SwapChainDesc.Flags = 0;
									//通过 D3D设备 与 DXGI Factory 创建交换链
									hr = DXGIFactory->CreateSwapChainForHwnd(D3DDevice, hWnd, &SwapChainDesc, nullptr, nullptr, &this->SwapChain);
									//如果失败，可能是 Win7.1 系统不支持 FlipModel，那就使用 BitBltModel 再试一次
									if (FAILED(hr))
									{
										SwapChainDesc.BufferCount = 1;
										SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;//Win7.1不可使用DXGI_SCALING_NONE
										SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
										hr = DXGIFactory->CreateSwapChainForHwnd(D3DDevice, hWnd, &SwapChainDesc, nullptr, nullptr, &this->SwapChain);
									}

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
			UINT sz = (UINT)Controls.size();
			if (sz)
			{
				std::map<UINT, DControl*>::iterator conIt = Controls.begin();
				for (UINT i = 0; i < sz; ++i, ++conIt)
				{
					conIt->second->ReleaseDeviceDependentRes();
					conIt->second->Dpi = Dpi;
					if (CallChildUpdateDpi)//因为UpdateDpi消耗较大，所以条件判断
						conIt->second->_UpdateDpi();
					conIt->second->_SetFactory(D2DFactory, WicFactory, DWFactory);
				}
			}
			___ConLock.Unlock();
		}
		else
		{
			ReleaseDevice();
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
		hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flags, FeatureLevels, 7, D3D11_SDK_VERSION, &D3DDevice, &FeatureLevel, &D3DContext);
		if (SUCCEEDED(hr))
			HardwareRendering = true;
		else if (hr == DXGI_ERROR_UNSUPPORTED)//不支持GPU渲染，切换CPU渲染
		{
			hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, Flags, FeatureLevels, 7, D3D11_SDK_VERSION, &D3DDevice, &FeatureLevel, &D3DContext);
			if (SUCCEEDED(hr))
				HardwareRendering = false;
		}

		if (SUCCEEDED(hr))
		{
			//通过 D3D设备 获取 DXGI设备
			hr = D3DDevice->QueryInterface(IID{ 0x77db970f, 0x6276, 0x48ba, 0xba, 0x28, 0x07, 0x01, 0x43, 0xb4, 0x39, 0x2c }, (void**)&DXGIDevice);
			if (SUCCEEDED(hr))
			{
				//创建多线程 D2D Factory
#ifdef _DEBUG//如果为Debug模式，则启用D2D调试层
				D2D1_FACTORY_OPTIONS options;
				options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
				hr = pD2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID{ 0xbb12d362, 0xdaee, 0x4b9a, 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f }, &options, reinterpret_cast<void**>(&this->D2DFactory));
#else
				hr = pD2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID{ 0xbb12d362, 0xdaee, 0x4b9a, 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f }, nullptr, reinterpret_cast<void**>(&this->D2DFactory));
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
			UINT sz = (UINT)Controls.size();
			if (sz)
			{
				std::map<UINT, DControl*>::iterator conIt = Controls.begin();
				for (UINT i = 0; i < sz; ++i, ++conIt)
				{
					conIt->second->ReleaseDeviceDependentRes();
					conIt->second->Dpi = Dpi;
					if (CallChildUpdateDpi)//因为UpdateDpi消耗较大，所以条件判断
						conIt->second->_UpdateDpi();
					conIt->second->_SetFactory(D2DFactory, WicFactory, DWFactory);
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
		int wid = Max(GET_X(lParam), 1), hei = Max(GET_Y(lParam), 1);
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
				SwapChain->ResizeBuffers(0, (UINT)wid, (UINT)hei, DXGI_FORMAT_UNKNOWN, 0);
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
			p(hWnd, 0, DControlType::Null, DefUserNamedType, DControlMsg::Window_Size, 0, rc.right - rc.left, rc.bottom - rc.top);
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
				wx = A_Wnd_X.FrameCalc(&AID);
			if (A_Wnd_Y)
				wy = A_Wnd_Y.FrameCalc(&AID);

			SetPosition(wx, wy, 0, nullptr, 0, false);
			if (AID)
			{
				//通知给用户
				DMsgProc p = proc;
				if (p)
					p(hWnd, 0, DControlType::Null, DefUserNamedType, DControlMsg::Control_EndOfAnimation, 0, AID, 0);
			}
		}
		return;
	}
	//允许大小调节时判断鼠标位置
	void mCalcPos(int x, int y)
	{
		POINT pos;
		pos.x = x;
		pos.y = y;
		GetCursorPos(&pos);//获取鼠标指针位置（屏幕坐标）
		ScreenToClient(this->hWnd, &pos);//将鼠标指针位置转换为窗口坐标
		bool left = (pos.x < Size_DetectEdge);
		bool top = (pos.y < Size_DetectEdge);
		bool right = (pos.x > rc.right - rc.left - Size_DetectEdge);
		bool bottom = (pos.y > rc.bottom - rc.top - Size_DetectEdge);
		//判断区域
		if (left && !top && !bottom)//左边
			OnSizeChange = 1;
		else if (right && !top && !bottom)//右边
			OnSizeChange = 2;
		else if (top && !left && !right)//上边
			OnSizeChange = 3;
		else if (bottom && !left && !right)//下边
			OnSizeChange = 4;
		else if (left && top)//左上
			OnSizeChange = 5;
		else if (right && top)//右上
			OnSizeChange = 6;
		else if (left && bottom)//左下
			OnSizeChange = 7;
		else if (right && bottom)//右下
			OnSizeChange = 8;
		else
			OnSizeChange = 0;
		return;
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
			for (UINT i = (UINT)Controls.size(); i != 0; --i, --con)
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
					UINT idNext = conNext == ActiveCon.end() ? 0 : conNext->first;

					ActiveCon.erase(con);
					con = ActiveCon.find(idNext);
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
inline void _Gau_CalCoeff(float sigma, float* a0, float* a1, float* a2, float* a3, float* b1, float* b2, float* cprev, float* cnext)
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

inline void _Gau_Horizontal(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColumn, int width, int height, int Channels, float a0a1, float a2a3, float b1b2, float cprev, float cnext)
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

inline void _Gau_Vertical(unsigned char* bufferPerLine, unsigned char* lpRowInitial, unsigned char* lpColInitial, int height, int width, int Channels, float a0a1, float a2a3, float b1b2, float cprev, float cnext)
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

inline void _Gau_Blur(BYTE* src, UINT Width, UINT Height, UINT Stride, float GaussianSigma, UINT Threads)
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
	UINT threads = Min(Min(32, Max(1, Threads)), Min(Width, Height));//如果宽或高小于线程数，则设为最小值
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

inline void __cdecl ___DxLibGaussianBlurThread(void* param)
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

//DXUI自用Windows消息处理回调函数
inline LRESULT CALLBACK ___DXUIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//让DXUI处理响应的消息，来改变控件的状态
	LRESULT res = -1;
	___WndListLock.Lock();
	if (___WndList.size())
	{
		std::map<HWND, void*>::iterator wndit = ___WndList.begin();
		UINT sz = (UINT)___WndList.size();
		std::map<HWND, void*>::iterator nextwndit;
		HWND nextHWND;
		while (wndit != ___WndList.end())
		{
			//保存状态，以便发生改变时恢复遍历
			nextwndit = wndit;
			++nextwndit;
			if (nextwndit != ___WndList.end())
				nextHWND = nextwndit->first;
			else
				nextHWND = nullptr;

			if (wndit->second)
			{
				LRESULT r = ((DWindow*)wndit->second)->DXUIMessageProc(hWnd, message, wParam, lParam);
				if (res == -1)
					res = r;

				if (sz != (UINT)___WndList.size())
				{
					wndit = ___WndList.find(nextHWND);
					sz = (UINT)___WndList.size();
				}
				else
					++wndit;
			}
		}
	}
	___WndListLock.Unlock();
	//如果返回值不为-1，则返回DXUI库处理后的返回值
	if (res != -1)
		return res;
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
}

//**********DXUI渲染线程**********
inline void __cdecl DXUI_RenderThread(void* param)
{
	if (param)
	{
		if (_DXUI_SuccessInit != 0 || FAILED(pCoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
		{
			_DXUI_ERROR(1015);
			return;
		}

		DWindow* ui = (DWindow*)param;
		ui->_SetRenderThreadAlive(true);//将线程存活状态设置为true

		DWORD LastFrameTime = ptimeGetTime();
		DWORD timePhase = 0;
		DWORD Delay;

		bool succ = ptimeBeginPeriod(1);
		while (!ui->_GetExitRenderThreadFlag())
		{
			Delay = ui->GetFrameDealy();

			ui->_RenderFrame();

			timePhase = ptimeGetTime() - LastFrameTime + 1;
			if (timePhase < Delay)//实际耗费的时间<每帧的时间
			{
				Sleep(Delay - timePhase);//将剩余的时间等待
			}
			LastFrameTime = ptimeGetTime();//在这里调用可以将循环耗费的时间也算进去
		}
		if (succ)
			ptimeEndPeriod(1);

		ui->_SetRenderThreadAlive(false);
		pCoUninitialize();
	}
	return;
}

#endif//_DXUI_LIBRARY
