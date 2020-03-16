#pragma once
/******************************************************************

	DXUI界面库 v200316
	作者: YZ
	QQ: 2441987560
	WeChat: yangzhen2441987560
	Email: 2441987560@qq.com
	Github: https://github.com/IDXGI/DXUI-Libaray

	此库来之不易，使用请标明作者。商业用途请先联系我。
	共同维护良好的开源环境！

*******************************************************************/

#define _WIN32_DCOM//CoInitializeEx函数必须定义此宏
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>//WIC图像库,用于命中测试和加载图片

#include <process.h>//_beginthread, _endthread，setTimerEvent等
#include <Mmsystem.h>

#include <iostream>
#include <string>
#include <map>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Imm32.lib")

#define DefFrameRate 60//默认帧率

#define RELEASE(P)     if(P){ (P)->Release(); (P) = nullptr; }
#define SafeDelete(P)  if(P){ delete (P); (P) = nullptr; }
#define DeleteArray(P) if(P){ delete[] (P); (P) = nullptr; }
#define GET_X(lp) ((int)(short)LOWORD(lp))
#define GET_Y(lp) ((int)(short)HIWORD(lp))

#ifdef _DEBUG//调试输出宏
#define DP0(fmt) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt));OutputDebugString(sOut);}
#define DP1(fmt,var) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var);OutputDebugString(sOut);}
#define DP2(fmt,var1,var2) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var1,var2);OutputDebugString(sOut);}
#define DP3(fmt,var1,var2,var3) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var1,var2,var3);OutputDebugString(sOut);}
#endif


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
///鼠标移动到控件上时的鼠标指针样式，0为无变化，1为手形
///</summary>
enum class DMouseStyle
{
	BasicMouse,
	HandMouse
};
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
	ImageBox
};
///<summary>
///单纯的控件消息，不包含控件ID
///</summary>
enum class DControlMsg
{
	///<summary>
	///返回Null代表此消息和DXUI控件无关
	///</summary>
	Null,

	Window_Size,

	//wParam里是变化之前的状态,lParam里是变化之后的。类型应转换为DControlState
	Control_StateChanged,

	//所有控件的Move、Down、Up消息中的wParam都是鼠标坐标(屏幕坐标系、设备相关像素),获取方法同Windows消息
	Control_MouseMove,
	Control_LButtonDown,
	Control_LButtonUp,

	TextBox_TextChange,
	TextBox_Scrolling,//wParam是鼠标坐标
	TextBox_Enter,
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
	Disable
};

///<summary>
///字体粗细
///</summary>
enum class DFontWeight { LightWeight, NormalWeight, BoldWeight };

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
	///time: 控件由其它状态转到当前状态时，过渡动画的时长。单位：秒
	///</summary>
	float time;
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
	///控件由其它状态转到当前状态时，过渡动画的时长。单位：秒
	///</summary>
	float time;
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
	///控件由其它状态转到当前状态时，过渡动画的时长。单位：秒
	///</summary>
	float time;
}DTextBoxTypeStyle;

//--------------------------------------------- 图 片 框 ---------------------------------------------

enum class DImageScaleMode { EqualScale, Fill };

///<summary>
///DXUI控件消息的回调函数
///</summary>
typedef void(__stdcall* DMsgProc)(HWND hWnd, UINT id, DControlType type, DControlMsg msg, WPARAM wParam, LPARAM lParam);

///<summary>
///DXUI自用的线程锁，用来锁接口，不可同时读、同时写、同时读写
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
///添加参数动画后，用于计算每一帧的参数值
///</summary>
class AniCalc_f
{
public:
	operator bool()
	{
		return nNowFrame;
	}
	inline bool AddAni(float Start, float End, UINT nFrame, DBezier* BezierParam)
	{
		if (!nFrame)
			return false;
		___CalcLock.Lock();

		this->Start = Start;
		this->End = End;

		Delta = (End - Start) / (float)nFrame;

		if (Start < End)
			NumAdd = true;
		else
			NumAdd = false;

		if (BezierParam)
		{
			EnableBezier = true;
			bezier = *BezierParam;
			if (bezier.P0 < 0.0f)
				bezier.P0 = 0.0f;
			if (bezier.P1 < 0.0f)
				bezier.P1 = 0.0f;
			if (bezier.P2 < 0.0f)
				bezier.P2 = 0.0f;
			if (bezier.P3 < 0.0f)
				bezier.P3 = 0.0f;

			if (bezier.P0 > 1.0f)
				bezier.P0 = 1.0f;
			if (bezier.P1 > 1.0f)
				bezier.P1 = 1.0f;
			if (bezier.P2 > 1.0f)
				bezier.P2 = 1.0f;
			if (bezier.P3 > 1.0f)
				bezier.P3 = 1.0f;
		}
		else {
			EnableBezier = false;
		}

		nSrcFrame = nFrame;
		nNowFrame = nFrame;

		___CalcLock.Unlock();
		return true;
	}
	inline float FrameCalc()
	{
		___CalcLock.Lock();

		nNowFrame--;

		if (EnableBezier)
		{
			cache = End - (End - Start) * (1.0f - CalcBezier());
		}
		else
			cache = End - (float)(Delta * nNowFrame);

		float res;
		if (NumAdd && cache < Start)
			res = Start;
		else if (!NumAdd && cache > Start)
			res = Start;
		else
			res = cache;

		___CalcLock.Unlock();
		return res;
	}

	inline void Reset()
	{
		___CalcLock.Lock();
		nNowFrame = 0;
		___CalcLock.Unlock();
		return;
	}
	inline float GetStart()
	{
		return Start;
	}
	inline float GetEnd()
	{
		return End;
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
	DBezier bezier = { 0.0f };

	inline float CalcBezier()
	{
		//perc为当前进度，相当于x
		perc = (float)(nSrcFrame - nNowFrame) / (float)nSrcFrame;

		if (perc == 0.0f)
			return 0.0f;
		if (perc == 1.0f)
		{
			nNowFrame = 0;
			return 1.0f;
		}
		//根据x反解t，先把它化成 x = at^3 + bt^2 + ct + d 的标准形式
		float a = 1.0f - 3.0f * bezier.P2 + 3.0f * bezier.P0;
		float b = 3.0f * bezier.P2 - 6.0f * bezier.P0;
		float c = 3.0f * bezier.P0;
		float d = -perc;

		//根据二分法求一元三次方程解(t)
		float x1 = 0.0f, x2 = 1.0f, t, fx1 = d, fx2 = a + b + c + d, fx0;
		do {
			t = (x1 + x2) / 2.0f;
			fx0 = ((a * t + b) * t + c) * t + d;

			if ((fx0 * fx1) < 0) {
				x2 = t;
				fx2 = fx0;
			}
			else {
				x1 = t;
				fx1 = fx0;
			}
		} while (fabs(fx0) >= 1e-6);//求根精度

		//根据t求y
		float z = 1.0f - t;
		return 3.0f * bezier.P1 * t * z * z + 3.0f * bezier.P3 * t * t * z + t * t * t;
	}
};


///<summary>
///所有控件的基类
///</summary>
class DControl
{
public:
	operator UINT()
	{
		return this->ID;
	}

	inline UINT GetID()
	{
		return this->ID;
	}

	inline DControlState GetState()
	{
		return State;
	}

	inline bool IsInit()
	{
		return Inited;
	}

	///<summary>
	///参数均为设备相关像素(DPI)
	///</summary>
	virtual bool HitTest(int xPos, int yPos)
	{
		if (!_HitTest || !HitTestBufferSize)//如果hittestBmp为空，或buffersize小于1像素-4byte，则不处理
			return false;

		//在HitTest中计算位置，进行命中测试
		int _xPos = (int)(xPos - UpdateRC.left * (float)dpiX / 96.0f), _yPos = (int)(yPos - UpdateRC.top * (float)dpiY / 96.0f);

		if (_xPos < 0 || _yPos < 0 || _xPos >= (int)((UpdateRC.right - UpdateRC.left) * (float)dpiX / 96.0f))
			return false;

		UINT Num = _yPos * Pitch + _xPos;

		if (Num > HitTestBufferSize)//超出范围了不管
			return false;

		//如果有父控件，必须也同时满足父控件的HitTest
		bool value = false;
		if (_HitTest[Num])
		{
			if (Parent)
			{
				value = Parent->HitTest(xPos, yPos);
			}
			else//没有父控件，那就说明此Hittest是不需要再次确认的
				value = true;
		}

		return value;
	}

	inline void _SetNeedUpdate(bool Need)
	{
		if (Need)
			EnableClip = false;
		NeedUpdate = Need;
		return;
	}

	inline void _SetNeedUpdate(bool Need, D2D1_RECT_F ClipRect)
	{
		if (Need)
		{
			if (NeedUpdate)
			{
				if (EnableClip)
				{
					___UpdateClipLock.Lock();
					this->UpdateClip = CalcRectUnion(this->UpdateClip, ClipRect);
					EnableClip = true;
					___UpdateClipLock.Unlock();
				}
			}
			else {
				NeedUpdate = true;

				if (EnableClip)
				{
					___UpdateClipLock.Lock();
					this->UpdateClip = CalcRectUnion(this->UpdateClip, ClipRect);
					EnableClip = true;
					___UpdateClipLock.Unlock();
				}
				else {
					___UpdateClipLock.Lock();
					this->UpdateClip = ClipRect;
					EnableClip = true;
					___UpdateClipLock.Unlock();
				}
			}
		}
		else
			NeedUpdate = false;
		return;
	}

	void SetTotalOpacity(float opa, UINT time = 0, DBezier* Nonlinear = nullptr, bool ReDraw = true)
	{
		if (time)
		{
			if (opa < 0.0f)
			{
				if (A_Opa.AddAni(this->TotalOpacity, 0.0f, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
					this->_SetNeedUpdate(true);
			}
			else if (opa > 1.0f)
			{
				if (A_Opa.AddAni(this->TotalOpacity, 1.0f, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
					this->_SetNeedUpdate(true);
			}
			else
			{
				if (A_Opa.AddAni(this->TotalOpacity, opa, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
					this->_SetNeedUpdate(true);
			}
			return;
		}

		if (opa < 0.0f)
			this->TotalOpacity = 0.0f;
		else if (opa > 1.0f)
			this->TotalOpacity = 1.0f;
		else
			this->TotalOpacity = opa;

		if (ReDraw)
			this->_SetNeedUpdate(true);
		return;
	}
	inline float GetTotalOpacity()
	{
		return this->TotalOpacity;
	}

	inline D2D1_RECT_F _GetClipRC()
	{
		if (Parent)
		{
			D2D1_RECT_F rc = Parent->_GetClipRC();
			rc.left = std::fmaxf(this->ClipRC.left, rc.left);
			rc.top = std::fmaxf(this->ClipRC.top, rc.top);
			rc.right = std::fminf(this->ClipRC.right, rc.right);
			rc.bottom = std::fminf(this->ClipRC.bottom, rc.bottom);
			return rc;
		}
		else
			return this->ClipRC;
	}

protected:
	friend class DWindow;

	HWND OwnerWnd = nullptr;
	DMsgProc proc = nullptr;
	___DThreadLock ___MsgBackLock;

	ID2D1Factory* D2DFactory = nullptr;
	IWICImagingFactory* WicFactory = nullptr;
	IDWriteFactory* DWriteFactory = nullptr;

	int dpiX = 96;
	int dpiY = 96;

	bool OnClick = false;
	DControlState State = DControlState::Normal;
	DControlState LastState = DControlState::Normal;

	DControlType ControlType;
	bool Inited = false;

	bool NeedUpdate = false;

	bool EnableClip = false;
	D2D1_RECT_F UpdateClip = { 0 };
	___DThreadLock ___UpdateClipLock;

	bool RectChanged = false;
	D2D1_RECT_F LastRect = { 0 };
	___DThreadLock ___RcChangedLock;

	UINT NowFrameCount = 0;

	bool CanMoveWindow = false;

	bool ParentDrawing = false;//标示Parent控件是否正在绘图，如果正在绘图，那就不重绘底图了
	DControl* Parent = nullptr;//Parent控件指针

	std::map<UINT, DControl*> Child;
	___DThreadLock ___ChildLock;

	D2D1_RECT_F ClipRC = { 0 };//传递给子控件Clip的RC

	UINT ID = 0;
	UINT FrameRate = DefFrameRate;
	float StrokeWidth = 0.0f;

	///<summary>
	///总的不透明度，在最终绘制时生效，会与控件每个状态指定的不透明度叠加(ep:0.8*0.8=0.64)
	///</summary>
	float TotalOpacity = 1.0f;
	AniCalc_f A_Opa;//不透明度的动画计算类

	ID2D1BitmapRenderTarget* BackImgTarget = nullptr;
	___DThreadLock ___BackImgTargetLock;

	ID2D1Bitmap* BackImg = nullptr;

	D2D1_ROUNDED_RECT DstRC = { 0 };//目标大小(绘图用)

	AniCalc_f A_RC_x;//位置x坐标
	AniCalc_f A_RC_y;//位置y坐标
	AniCalc_f A_RC_width;//长度
	AniCalc_f A_RC_height;//宽度
	AniCalc_f A_Radius_X;//圆角X半径
	AniCalc_f A_Radius_Y;//圆角Y半径

	D2D1_RECT_F UpdateRC = { 0 };//更新的矩形大小，通常比目标的大半个描边宽度(更新区域用)

	ID2D1RenderTarget* HitTestBmpTarget = nullptr;
	IWICBitmap* HitTestBmp = nullptr;
	bool* _HitTest = nullptr;
	UINT HitTestBufferSize = 0;
	UINT Pitch = 0;

	inline D2D1_COLOR_F CalcMidType(DCOLOR DstCol, D2D1_COLOR_F NowCol, UINT FrameC)
	{
		return { (float)((float)(DstCol.r - NowCol.r) / (float)FrameC), (float)((float)(DstCol.g - NowCol.g) / (float)FrameC), (float)((float)(DstCol.b - NowCol.b) / (float)FrameC), (float)((float)(DstCol.a - NowCol.a) / (float)FrameC) };
	}
	inline D2D1_COLOR_F CalcNowType(DCOLOR DstCol, D2D1_COLOR_F MidCol, UINT FrameC)
	{
		return { DstCol.r - (float)(FrameC * MidCol.r), DstCol.g - (float)(FrameC * MidCol.g), DstCol.b - (float)(FrameC * MidCol.b), DstCol.a - (float)(FrameC * MidCol.a) };
	}
	inline D2D1_COLOR_F Calc_fColor(D2D1_COLOR_F NowCol, float TotalOp)
	{
		return { (float)NowCol.r / 255.0f, (float)NowCol.g / 255.0f, (float)NowCol.b / 255.0f, NowCol.a * TotalOp };
	}
	inline D2D1_COLOR_F DcolToColf(DCOLOR col)
	{
		return { (float)col.r, (float)col.g, (float)col.b, (float)col.a };
	}

	inline D2D1_RECT_F CalcRectOverlap(D2D1_RECT_F rc1, D2D1_RECT_F rc2)
	{
		return { std::fmaxf(rc1.left, rc2.left), std::fmaxf(rc1.top, rc2.top), std::fminf(rc1.right, rc2.right), std::fminf(rc1.bottom, rc2.bottom) };
	}
	inline D2D1_RECT_F CalcRectUnion(D2D1_RECT_F rc1, D2D1_RECT_F rc2)
	{
		return { std::fminf(rc1.left, rc2.left), std::fminf(rc1.top, rc2.top), std::fmaxf(rc1.right, rc2.right), std::fmaxf(rc1.bottom, rc2.bottom) };
	}

	inline bool IsRectWorkable(D2D1_RECT_F rc)
	{
		if (rc.left >= rc.right)
			return false;
		if (rc.top >= rc.bottom)
			return false;
		return true;
	}
	void DetectAndResetColor(DCOLOR& Color)
	{
		if (Color.r > 255)
			Color.r = 255;
		if (Color.g > 255)
			Color.g = 255;
		if (Color.b > 255)
			Color.b = 255;
		if (Color.a > 1.0f)
			Color.a = 1.0f;

		if (Color.r < 0)
			Color.r = 0;
		if (Color.g < 0)
			Color.g = 0;
		if (Color.b < 0)
			Color.b = 0;
		if (Color.a < 0.0f)
			Color.a = 0.0f;

		return;
	}

	inline void _SetDpi(int dpiX, int dpiY)
	{
		this->dpiX = dpiX;
		this->dpiY = dpiY;
		//dpi改变时，重新设置更新RC
		this->_PreDisposeRC();
		return;
	}
	inline void _PreDisposeRC()
	{
		//因为D2D的像素可以为小数，所以在此转换一下，保证D2D的函数自动把坐标转换成设备相关像素时，坐标一定是整数。
		//坐标为小数时D2D会自动用算法进行缩放，导致有微小的错位和模糊。
		UpdateRC.left = (int)((DstRC.rect.left - StrokeWidth / 2.0f) * (float)dpiX / 96.0f) * 96.0f / (float)dpiX;
		UpdateRC.top = (int)((DstRC.rect.top - StrokeWidth / 2.0f) * (float)dpiY / 96.0f) * 96.0f / (float)dpiY;
		UpdateRC.right = ((int)((DstRC.rect.right + StrokeWidth / 2.0f) * (float)dpiX / 96.0f) + 1) * 96.0f / (float)dpiX;
		UpdateRC.bottom = ((int)((DstRC.rect.bottom + StrokeWidth / 2.0f) * (float)dpiY / 96.0f) + 1) * 96.0f / (float)dpiY;
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
	inline D2D1_RECT_F _GetUpdateRC()
	{
		return this->UpdateRC;
	}
	inline D2D1_RECT_F _GetUpdateRect()
	{
		if (Parent)
		{
			D2D1_RECT_F ParClip = Parent->_GetClipRC();
			___RcChangedLock.Lock();
			if (RectChanged)
			{
				D2D1_RECT_F r = LastRect;
				RectChanged = false;
				___RcChangedLock.Unlock();

				ParClip = CalcRectOverlap(r, ParClip);
				if (IsRectWorkable(ParClip))
					return ParClip;
				else return { 0 };
			}
			___RcChangedLock.Unlock();

			ParClip = CalcRectOverlap(this->UpdateRC, ParClip);
			if (IsRectWorkable(ParClip))
				return ParClip;
			else return { 0 };
		}
		else {
			___RcChangedLock.Lock();
			if (RectChanged)
			{
				D2D1_RECT_F r = LastRect;
				RectChanged = false;
				___RcChangedLock.Unlock();
				return r;
			}
			___RcChangedLock.Unlock();

			return this->UpdateRC;
		}
	}

	inline UINT _GetNowFrameCount()
	{
		return this->NowFrameCount;
	}

	inline void _SetID(UINT id)
	{
		ID = id;
		return;
	}

	inline void _SetParentDrawing(bool drawing)
	{
		ParentDrawing = drawing;
		return;
	}

	virtual bool _CreateHitTestBmp()
	{
		RELEASE(HitTestBmpTarget);
		RELEASE(HitTestBmp);
		//创建命中测试bmp，其中一个bool表示此处是否有控件
		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			(float)dpiX, (float)dpiY, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT);

		//创建WicBmp
		if (WicFactory && FAILED(WicFactory->CreateBitmap((UINT)((UpdateRC.right - UpdateRC.left) * (float)dpiX / 96.0f), (UINT)((UpdateRC.bottom - UpdateRC.top) * (float)dpiY / 96.0f), GUID_WICPixelFormat32bppBGR, WICBitmapCacheOnDemand, &HitTestBmp)))
			return false;
		//创建WicBmp的D2D渲染目标
		if (D2DFactory && HitTestBmp && FAILED(D2DFactory->CreateWicBitmapRenderTarget(HitTestBmp, &props, &HitTestBmpTarget)))
			return false;

		return true;
	}


	bool _Base_Release()
	{
		if (!Inited)
			return false;

		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Radius_X.Reset();
		A_Radius_Y.Reset();
		A_Opa.Reset();
		OwnerWnd = nullptr;
		proc = nullptr;
		dpiX = 0;
		dpiY = 0;
		NeedUpdate = false;
		EnableClip = false;
		UpdateClip = { 0 };
		RectChanged = false;
		LastRect = { 0 };
		NowFrameCount = 0;
		OnClick = false;
		State = DControlState::Normal;
		LastState = DControlState::Normal;
		CanMoveWindow = false;
		ParentDrawing = false;
		Parent = nullptr;//Parent控件指针，由于和用户用的同一个指针，不能Release
		Child.clear();
		ID = 0;
		TotalOpacity = 1.0f;
		FrameRate = DefFrameRate;
		RELEASE(BackImgTarget);
		RELEASE(BackImg);
		DstRC = { 0 };
		UpdateRC = { 0 };
		RELEASE(HitTestBmpTarget);
		RELEASE(HitTestBmp);
		DeleteArray(_HitTest);
		HitTestBufferSize = 0;
		Pitch = 0;
		D2DFactory = nullptr;//与主窗口共用一个Factory，不能释放
		WicFactory = nullptr;
		DWriteFactory = nullptr;

		return true;
	}


	virtual bool _HitTestBmpToMemory()
	{
		IWICBitmapLock* lock;
		WICRect rect = { 0, 0, (int)((UpdateRC.right - UpdateRC.left) * (float)dpiX / 96.0f), (int)((UpdateRC.bottom - UpdateRC.top) * (float)dpiY / 96.0f) };
		BYTE* cv;

		if (HitTestBmp && SUCCEEDED(HitTestBmp->Lock(&rect, WICBitmapLockRead, &lock)) && lock)
		{
			lock->GetStride(&Pitch);
			lock->GetDataPointer(&HitTestBufferSize, &cv);

			if (cv)
			{
				DeleteArray(_HitTest);

				_HitTest = new bool[HitTestBufferSize / 4];
				ZeroMemory(_HitTest, HitTestBufferSize / 4);


				if (HitTestBufferSize > 3)
				{
					for (UINT i = 0; i < HitTestBufferSize; i += 4)
					{
						if (cv[i] > 0)
						{
							_HitTest[i / 4] = true;
						}
					}
				}
				HitTestBufferSize /= 4;
				Pitch /= 4;
			}

			RELEASE(lock);
		}
		else
			return false;

		return true;
	}
	inline void _SetMsgBack(HWND Owner, DMsgProc Proc)
	{
		___MsgBackLock.Lock();
		OwnerWnd = Owner;
		proc = Proc;
		___MsgBackLock.Unlock();
		return;
	}
	inline void _SetFactory(ID2D1Factory* D2DFactory, IWICImagingFactory* WicFactory, IDWriteFactory* DWriteFactory)
	{
		this->D2DFactory = D2DFactory;
		this->WicFactory = WicFactory;
		this->DWriteFactory = DWriteFactory;
		return;
	}
	inline void _SetParentCon(DControl* control)
	{
		Parent = control;
		return;
	}
	inline DControl* _GetParentCon()
	{
		return Parent;
	}

	inline void _AddChildCon(DControl* control)
	{
		___ChildLock.Lock();
		Child.insert(std::pair<UINT, DControl*>(control->GetID(), control));
		___ChildLock.Unlock();

		return;
	}
	inline void _DeleteChildCon(DControl* control)
	{
		if (!control)
			return;

		___ChildLock.Lock();
		Child.erase(control->GetID());
		___ChildLock.Unlock();
		return;
	}
	inline std::map<UINT, DControl*> _GetChildCon()
	{
		return Child;
	}
	inline void _ClearChildCon()
	{
		___ChildLock.Lock();
		Child.clear();
		___ChildLock.Unlock();
		return;
	}

	//此函数在底层控件刚绘制完成后调用上层控件的此函数，这时Target里的图片正好可以作为上层控件的背景图
	inline void _UpdateBackImg(ID2D1BitmapRenderTarget* Target, D2D1_RECT_F* UpdateClip = nullptr)
	{
		if (!Target)
			return;

		//创建内存的缓存Target
		___BackImgTargetLock.Lock();
		if (!BackImgTarget)
		{
			D2D1_SIZE_F rect = { UpdateRC.right - UpdateRC.left, UpdateRC.bottom - UpdateRC.top };
			if (FAILED(Target->CreateCompatibleRenderTarget(&rect, nullptr, nullptr, D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &BackImgTarget)))
			{
				___BackImgTargetLock.Unlock();
				return;
			}
		}

		ID2D1Bitmap* backbmp;
		Target->GetBitmap(&backbmp);
		if (backbmp)
		{
			___UpdateClipLock.Lock();
			if (UpdateClip)
			{
				D2D1_RECT_F r = CalcRectOverlap(*UpdateClip, UpdateRC);
				D2D1_RECT_F r1 = { r.left - UpdateRC.left, r.top - UpdateRC.top, r.right - UpdateRC.left, r.bottom - UpdateRC.top };
				___UpdateClipLock.Unlock();

				if (!IsRectWorkable(r))
				{
					RELEASE(backbmp);
					return;
				}
				BackImgTarget->BeginDraw();
				BackImgTarget->DrawBitmap(backbmp, r1, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, r);
			}
			else
			{
				___UpdateClipLock.Unlock();

				BackImgTarget->BeginDraw();
				BackImgTarget->DrawBitmap(backbmp, nullptr, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, &UpdateRC);
			}

			BackImgTarget->EndDraw();
			RELEASE(backbmp);
		}

		RELEASE(BackImg);
		BackImgTarget->GetBitmap(&BackImg);

		___BackImgTargetLock.Unlock();

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
	virtual bool _LMouseDownMsgProc(int xPos, int yPos) = 0;
	virtual bool _LMouseUpMsgProc(int xPos, int yPos) = 0;

	virtual bool _CharMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return false;
	}

	virtual void _SeparateFromWindow() = 0;

	virtual bool _MouseWheelMsgProc(WPARAM wParam)
	{
		return false;
	}

	inline void _SetFrameRate(UINT Rate)
	{
		FrameRate = Rate;
		return;
	}

	virtual void _SetLastFrameCount(UINT count) = 0;


	virtual void _DrawHitTestBmp() = 0;//绘制命中测试BMP

	virtual void _DisposeAnimation() = 0;//每一帧都处理用户手动设置的动画，由于需要使Parent窗口更新，所以需要MainWindow主动在获取完UpdateRect的最后调用它，那么下一帧更新将会使变化生效

	virtual bool _Draw(ID2D1BitmapRenderTarget* DstTarget, D2D1_COLOR_F BackColor) = 0;

	inline void _EndDraw_CallChildToUpdateBackImg(ID2D1BitmapRenderTarget* CacheTarget)
	{
		___ChildLock.Lock();
		if (!Child.empty())
		{
			if (NowFrameCount)//将Child全部设为要更新，再刷新Child的backimg
			{
				std::map<UINT, DControl*>::iterator child = Child.begin();

				for (UINT i = 0; i < Child.size(); i++, child++)
				{
					child->second->_SetParentDrawing(true);

					if (EnableClip)
					{
						if (IsRectWorkable(CalcRectOverlap(child->second->_GetUpdateRC(), UpdateClip)))
							child->second->_SetNeedUpdate(true);
					}
					else
						child->second->_SetNeedUpdate(true);
				}
			}
			else {//将Child全部设为要更新
				std::map<UINT, DControl*>::iterator child = Child.begin();
				for (UINT i = 0; i < Child.size(); i++, child++)
				{
					child->second->_SetParentDrawing(false);
					if (EnableClip)
					{
						if (IsRectWorkable(CalcRectOverlap(child->second->_GetUpdateRC(), UpdateClip)))
						{
							child->second->_UpdateBackImg(CacheTarget, &UpdateClip);
							child->second->_SetNeedUpdate(true);
						}
					}
					else
					{
						child->second->_UpdateBackImg(CacheTarget);
						child->second->_SetNeedUpdate(true);
					}
				}
			}
		}
		___ChildLock.Unlock();

		return;
	}

};





class DButton : public DControl
{
public:
	~DButton()
	{
		this->Release();
		return;
	}

	bool Init(POINT position, SIZE size, DButtonTypeStyle Normal, DButtonTypeStyle MouseMove, DButtonTypeStyle Click, DButtonTypeStyle Disable, float radiusX, float radiusY, float StrokeWidth, std::wstring text, std::wstring FamliyName, float FontSize, DFontWeight FontWeight = DFontWeight::NormalWeight, DMouseStyle MouseStyle = DMouseStyle::BasicMouse)
	{
		DstRC.rect.left = (float)position.x;
		DstRC.rect.top = (float)position.y;
		DstRC.rect.right = (float)position.x + size.cx;
		DstRC.rect.bottom = (float)position.y + size.cy;
		DstRC.radiusX = radiusX;
		DstRC.radiusY = radiusY;

		ClipRC.left = DstRC.rect.left + StrokeWidth / 2.0f;
		ClipRC.top = DstRC.rect.top + StrokeWidth / 2.0f;
		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		memcpy_s(&NormalType, sizeof(NormalType), &Normal, sizeof(Normal));
		memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &MouseMove, sizeof(MouseMove));
		memcpy_s(&ClickType, sizeof(ClickType), &Click, sizeof(Click));
		memcpy_s(&DisableType, sizeof(DisableType), &Disable, sizeof(Disable));

		//判断如果大于255则设为255
		DetectAndResetColor(NormalType.FillColor);
		DetectAndResetColor(NormalType.StrokeColor);
		DetectAndResetColor(NormalType.TextColor);

		DetectAndResetColor(MouseMoveType.FillColor);
		DetectAndResetColor(MouseMoveType.StrokeColor);
		DetectAndResetColor(MouseMoveType.TextColor);

		DetectAndResetColor(ClickType.FillColor);
		DetectAndResetColor(ClickType.StrokeColor);
		DetectAndResetColor(ClickType.TextColor);

		DetectAndResetColor(DisableType.FillColor);
		DetectAndResetColor(DisableType.StrokeColor);
		DetectAndResetColor(DisableType.TextColor);

		//初始化当前样式
		NowType.FillColor = DcolToColf(NormalType.FillColor);
		NowType.StrokeColor = DcolToColf(NormalType.StrokeColor);
		NowType.TextColor = DcolToColf(NormalType.TextColor);

		this->StrokeWidth = StrokeWidth;

		Text = text;

		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case DFontWeight::NormalWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::LightWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::BoldWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;

		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		this->MouseStyle = MouseStyle;
		this->TotalOpacity = 1.0f;
		this->ControlType = DControlType::Button;

		Inited = true;

		return true;
	}
	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		ControlType = DControlType::Button;
		MouseStyle = DMouseStyle::BasicMouse;

		StrokeWidth = 0.0f;

		NormalType = { 0 };
		MouseMoveType = { 0 };
		ClickType = { 0 };
		DisableType = { 0 };

		NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)
		MidType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

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

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_Radius_X.AddAni(DstRC.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_Radius_Y.AddAni(DstRC.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}

		DstRC.radiusX = RadiusX;
		DstRC.radiusY = RadiusY;

		//重新创建HitTest
		this->_CreateHitTestBmp();
		this->_DrawHitTestBmp();
		this->_HitTestBmpToMemory();

		if (Parent)
			Parent->_SetNeedUpdate(true, UpdateRC);
		else
			this->_SetNeedUpdate(true);

		return;
	}
	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_RC_x.AddAni(DstRC.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_RC_y.AddAni(DstRC.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}
		D2D1_RECT_F OldUpdateRC = UpdateRC;

		//要先处理right和bottom
		DstRC.rect.right = x + DstRC.rect.right - DstRC.rect.left;
		DstRC.rect.bottom = y + DstRC.rect.bottom - DstRC.rect.top;

		DstRC.rect.left = x;
		DstRC.rect.top = y;

		this->_PreDisposeRC();

		ClipRC.left = DstRC.rect.left + StrokeWidth / 2.0f;
		ClipRC.top = DstRC.rect.top + StrokeWidth / 2.0f;
		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		___RcChangedLock.Lock();
		if (RectChanged)
		{

			LastRect = CalcRectUnion(LastRect, CalcRectUnion(UpdateRC, OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(UpdateRC, OldUpdateRC);
			RectChanged = true;
		}
		___RcChangedLock.Unlock();

		if (Parent)
		{
			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(true, CalcRectUnion(UpdateRC, OldUpdateRC));
		}
		else
			this->_SetNeedUpdate(true);
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_RC_width.AddAni(DstRC.rect.right - DstRC.rect.left, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_RC_height.AddAni(DstRC.rect.bottom - DstRC.rect.top, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}
		D2D1_RECT_F OldUpdateRC = UpdateRC;

		DstRC.rect.right = DstRC.rect.left + width;
		DstRC.rect.bottom = DstRC.rect.top + height;

		this->_PreDisposeRC();

		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		___LayoutLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestBmp();
		this->_DrawHitTestBmp();
		this->_HitTestBmpToMemory();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(UpdateRC, OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(UpdateRC, OldUpdateRC);
			RectChanged = true;
		}
		___RcChangedLock.Unlock();

		if (Parent)
		{
			___BackImgTargetLock.Lock();
			RELEASE(BackImgTarget);
			___BackImgTargetLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(true, CalcRectUnion(UpdateRC, OldUpdateRC));
		}
		else
			this->_SetNeedUpdate(true);
		return;
	}

	//此函数稍大且用的很少，不做inline处理
	void SetTypeStyle(DControlState State, DButtonTypeStyle type)
	{
		switch (State)
		{
		case DControlState::Normal:
			memcpy_s(&NormalType, sizeof(NormalType), &type, sizeof(type));
			break;
		case DControlState::MouseMove:
			memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &type, sizeof(type));
			break;
		case DControlState::Click:
			memcpy_s(&ClickType, sizeof(ClickType), &type, sizeof(type));
			break;
		case DControlState::Disable:
			memcpy_s(&DisableType, sizeof(DisableType), &type, sizeof(type));
			break;
		}

		this->_SetNeedUpdate(true);//设置完后将状态置为需要更新
		return;
	}

	inline void SetText(std::wstring text)
	{
		___LayoutLock.Lock();
		Text = text;
		RELEASE(TextLayout);//文字发生更改，必须重新创建(Draw时自动重建)，没有修改的方法
		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);//设置完后将状态置为需要更新
		return;
	}

	//此函数稍大且用的很少，不做inline处理
	void SetFontFormat(std::wstring FamliyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case DFontWeight::NormalWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::LightWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::BoldWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;

		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		___LayoutLock.Lock();
		if (TextLayout)
		{
			TextLayout->SetFontFamilyName(this->FamliyName.c_str(), { 0, Text.size() });
			TextLayout->SetFontSize(this->FontSize, { 0, Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, Text.size() });
		}
		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);//设置完后将状态置为需要更新
		return;
	}

	bool SetState(DControlState state)
	{
		if (State != state)
		{
			float time = 0.0f;
			switch (state)
			{
			case DControlState::Normal:
				time = NormalType.time;
				break;
			case DControlState::MouseMove:
				time = MouseMoveType.time;
				break;
			case DControlState::Click:
				time = ClickType.time;
				break;
			case DControlState::Disable:
				time = DisableType.time;
				break;
			}

			UINT Last = NowFrameCount;
			if (state == LastState && NowFrameCount && (UINT)(FrameRate * time) > NowFrameCount)
			{
				NowFrameCount = (UINT)(FrameRate * time - NowFrameCount);
			}
			else {
				NowFrameCount = (UINT)(FrameRate * time);
			}

			if (NowFrameCount == 0)
				NowFrameCount = 1;

			switch (state)
			{
			case DControlState::Normal:
				//填充颜色
				MidType.FillColor = CalcMidType(NormalType.FillColor, NowType.FillColor, NowFrameCount);
				//描边颜色
				MidType.StrokeColor = CalcMidType(NormalType.StrokeColor, NowType.StrokeColor, NowFrameCount);
				//文字颜色
				MidType.TextColor = CalcMidType(NormalType.TextColor, NowType.TextColor, NowFrameCount);

				break;
			case DControlState::MouseMove:
				//填充颜色
				MidType.FillColor = CalcMidType(MouseMoveType.FillColor, NowType.FillColor, NowFrameCount);
				//描边颜色
				MidType.StrokeColor = CalcMidType(MouseMoveType.StrokeColor, NowType.StrokeColor, NowFrameCount);
				//文字颜色
				MidType.TextColor = CalcMidType(MouseMoveType.TextColor, NowType.TextColor, NowFrameCount);

				break;
			case DControlState::Click:
				//填充颜色
				MidType.FillColor = CalcMidType(ClickType.FillColor, NowType.FillColor, NowFrameCount);
				//描边颜色
				MidType.StrokeColor = CalcMidType(ClickType.StrokeColor, NowType.StrokeColor, NowFrameCount);
				//文字颜色
				MidType.TextColor = CalcMidType(ClickType.TextColor, NowType.TextColor, NowFrameCount);

				break;
			case DControlState::Disable:
				//填充颜色
				MidType.FillColor = CalcMidType(DisableType.FillColor, NowType.FillColor, NowFrameCount);
				//描边颜色
				MidType.StrokeColor = CalcMidType(DisableType.StrokeColor, NowType.StrokeColor, NowFrameCount);
				//文字颜色
				MidType.TextColor = CalcMidType(DisableType.TextColor, NowType.TextColor, NowFrameCount);

				break;
			}

			LastState = State;
			State = state;

			this->_SetNeedUpdate(true);

			___MsgBackLock.Lock();
			if (OwnerWnd && proc)
				proc(OwnerWnd, ID, ControlType, DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);
			___MsgBackLock.Unlock();

			return true;
		}
		return false;
	}

private:
	DMouseStyle MouseStyle = DMouseStyle::BasicMouse;

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

	Type NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)
	Type MidType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

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

	inline void _SeparateFromWindow()
	{
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Opa.Reset();
		//把所有设备相关资源都释放
		RELEASE(brush);
		RELEASE(TextFormat);
		RELEASE(TextLayout);
		RELEASE(BackImgTarget);
		RELEASE(BackImg);
		RELEASE(HitTestBmpTarget);
		RELEASE(HitTestBmp);
		DeleteArray(_HitTest);
		_SetMsgBack(nullptr, nullptr);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}

	void _SetLastFrameCount(UINT count)
	{
		this->NowFrameCount = count;
		return;
	}

	bool _SetCursorMsgProc()
	{
		if (MouseStyle == DMouseStyle::HandMouse)
		{
			SetCursor(LoadCursorW(nullptr, IDC_HAND));
			return true;
		}
		else
			return false;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable || NowControlID == ID)
		{
			return false;
		}
		OnClick = false;

		return SetState(DControlState::Normal);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable || State == DControlState::Click)
		{
			return false;
		}

		if (OnClick)
			return SetState(DControlState::Click);

		return SetState(DControlState::MouseMove);
	}
	bool _LMouseDownMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable)
		{
			return false;
		}

		OnClick = true;

		return SetState(DControlState::Click);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable || !OnClick)
		{
			return false;
		}

		OnClick = false;

		return SetState(DControlState::MouseMove);
	}

	void _DrawHitTestBmp()//绘制命中测试BMP
	{
		if (!HitTestBmpTarget)
			return;

		D2D1_COLOR_F color;
		color.r = 1.0f;
		color.g = 1.0f;
		color.b = 1.0f;
		color.a = 1.0f;

		ID2D1SolidColorBrush* bru;
		HitTestBmpTarget->CreateSolidColorBrush(color, &bru);

		if (bru)
		{
			D2D1_ROUNDED_RECT hittest = { {StrokeWidth / 2.0f + 1.0f, StrokeWidth / 2.0f + 1.0f, DstRC.rect.right - DstRC.rect.left + StrokeWidth / 2.0f, DstRC.rect.bottom - DstRC.rect.top + StrokeWidth / 2.0f}, DstRC.radiusX, DstRC.radiusY };
			HitTestBmpTarget->BeginDraw();
			HitTestBmpTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

			HitTestBmpTarget->FillRoundedRectangle(hittest, bru);
			HitTestBmpTarget->DrawRoundedRectangle(hittest, bru, StrokeWidth);

			HitTestBmpTarget->EndDraw();

			RELEASE(bru);
		}

		return;
	}

	void _DisposeAnimation()
	{
		EnableClip = false;//每一帧都要置为false

		if (A_RC_x || A_RC_y)
		{
			float x = DstRC.rect.left, y = DstRC.rect.top;
			if (A_RC_x)
				x = A_RC_x.FrameCalc();
			if (A_RC_y)
				y = A_RC_y.FrameCalc();

			SetPosition(x, y);
		}
		if (A_RC_width || A_RC_height)
		{
			float wid = DstRC.rect.right - DstRC.rect.left, hei = DstRC.rect.bottom - DstRC.rect.top;
			if (A_RC_width)
				wid = A_RC_width.FrameCalc();
			if (A_RC_height)
				hei = A_RC_height.FrameCalc();

			SetSize(wid, hei);
		}
		if (A_Radius_X || A_Radius_Y)
		{
			float x = DstRC.radiusX, y = DstRC.radiusY;
			if (A_Radius_X)
				x = A_Radius_X.FrameCalc();
			if (A_Radius_Y)
				y = A_Radius_Y.FrameCalc();

			SetRadius(x, y);
		}
		if (A_Opa)
			SetTotalOpacity(A_Opa.FrameCalc());

		return;
	}

	bool _Draw(ID2D1BitmapRenderTarget* DstTarget, D2D1_COLOR_F BackColor)
	{
		if (!DstTarget)
		{
			this->_SetNeedUpdate(true);
			return false;
		}

		if (!brush)
		{
			DstTarget->CreateSolidColorBrush(BackColor, &brush);
			if (!brush)
				return false;
		}
		else
			brush->SetColor(BackColor);


		//处理动画的颜色变换
		DButtonTypeStyle* DstType = nullptr;

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
		case DControlState::Disable:
		{
			DstType = &DisableType;

			break;
		}

		}

		D2D1_COLOR_F f_FillColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_StrokeColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_TextColor;//以1.0f为最大的颜色结构体

		if (NowFrameCount)//如果还有动画没有播放完
		{
			NowFrameCount--;//每一帧减1，直到Count为0
		}
		//动画——填充颜色处理
		NowType.FillColor = CalcNowType(DstType->FillColor, MidType.FillColor, NowFrameCount);
		f_FillColor = Calc_fColor(NowType.FillColor, TotalOpacity);
		//动画——描边颜色处理
		NowType.StrokeColor = CalcNowType(DstType->StrokeColor, MidType.StrokeColor, NowFrameCount);
		f_StrokeColor = Calc_fColor(NowType.StrokeColor, TotalOpacity);
		//动画——文字颜色处理
		NowType.TextColor = CalcNowType(DstType->TextColor, MidType.TextColor, NowFrameCount);
		f_TextColor = Calc_fColor(NowType.TextColor, TotalOpacity);

		//********动画颜色处理结束**********

		//尽量缩小锁的锁定时间，靠下放
		___LayoutLock.Lock();
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamliyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, FontSize, L"zh-cn", &TextFormat);
			TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

			RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
		}
		if (!TextLayout && TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextLayout(Text.c_str(), Text.size(), TextFormat, DstRC.rect.right - DstRC.rect.left, DstRC.rect.bottom - DstRC.rect.top, &TextLayout);
		}

		DstTarget->BeginDraw();

		___UpdateClipLock.Lock();
		if (EnableClip)//设置更新区的裁剪，主要为过渡动画时使用
			DstTarget->PushAxisAlignedClip(UpdateClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		if (Parent)//有底层控件, 要重画backimg
		{
			DstTarget->PushAxisAlignedClip(Parent->_GetClipRC(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//设置父控件的裁剪

			if (!ParentDrawing && BackImg)
			{
				DstTarget->DrawBitmap(BackImg, UpdateRC, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
		}
		else {//无底层控件，直接用窗口背景色填充
			if (EnableClip)
				DstTarget->FillRectangle(UpdateClip, brush);

			___RcChangedLock.Lock();
			if (RectChanged)
				DstTarget->FillRectangle(LastRect, brush);
			else
				DstTarget->FillRectangle(UpdateRC, brush);
			___RcChangedLock.Unlock();
		}

		if (f_FillColor.a != 0.0f)
		{
			brush->SetColor(f_FillColor);//设置为填充颜色
			DstTarget->FillRoundedRectangle(&DstRC, brush);
		}
		if (f_StrokeColor.a != 0.0f)
		{
			brush->SetColor(f_StrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(&DstRC, brush, StrokeWidth);
		}

		if (TextLayout && f_TextColor.a != 0.0f)
		{
			brush->SetColor(f_TextColor);//设置为文字颜色
			DstTarget->DrawTextLayout({ (float)DstRC.rect.left, (float)DstRC.rect.top }, TextLayout, brush);
		}
		___LayoutLock.Unlock();

		if (Parent)
			DstTarget->PopAxisAlignedClip();

		if (EnableClip)
			DstTarget->PopAxisAlignedClip();
		___UpdateClipLock.Unlock();

		DstTarget->EndDraw();

		_EndDraw_CallChildToUpdateBackImg(DstTarget);

		this->_SetNeedUpdate(NowFrameCount);
		return true;
	}

};






class DLabel : public DControl
{
public:
	~DLabel()
	{
		this->Release();
		return;
	}

	bool Init(POINT position, SIZE size, DLabelTypeStyle Normal, DLabelTypeStyle MouseMove, DLabelTypeStyle Click, bool CanMoveWindow, std::wstring text, std::wstring FamliyName, float FontSize, bool AutoWarp = true, DFontWeight FontWeight = DFontWeight::NormalWeight, DAlignment X_Alignment = DAlignment::Near, DAlignment Y_Alignment = DAlignment::Near, DMouseStyle MouseStyle = DMouseStyle::BasicMouse)
	{
		this->CanMoveWindow = CanMoveWindow;

		DstRC.rect.left = (float)position.x;
		DstRC.rect.top = (float)position.y;
		DstRC.rect.right = (float)position.x + size.cx;
		DstRC.rect.bottom = (float)position.y + size.cy;
		DstRC.radiusX = 0.0f;
		DstRC.radiusY = 0.0f;

		ClipRC = DstRC.rect;

		memcpy_s(&NormalType, sizeof(NormalType), &Normal, sizeof(Normal));
		memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &MouseMove, sizeof(MouseMove));
		memcpy_s(&ClickType, sizeof(ClickType), &Click, sizeof(Click));

		//判断如果大于255则设为255
		DetectAndResetColor(NormalType.TextColor);
		DetectAndResetColor(MouseMoveType.TextColor);
		DetectAndResetColor(ClickType.TextColor);

		//初始化当前样式
		NowType = DcolToColf(NormalType.TextColor);

		Text = text;

		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		if (AutoWarp)//处理自动换行
			this->AutoWarp = DWRITE_WORD_WRAPPING_WRAP;
		else
			this->AutoWarp = DWRITE_WORD_WRAPPING_NO_WRAP;

		switch (FontWeight)
		{
		case DFontWeight::NormalWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::LightWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::BoldWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;

		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

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

		this->MouseStyle = MouseStyle;

		this->ControlType = DControlType::Label;
		this->TotalOpacity = 1.0f;

		Inited = true;

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
		MidType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

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

	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_RC_x.AddAni(DstRC.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_RC_y.AddAni(DstRC.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}
		D2D1_RECT_F OldUpdateRC = UpdateRC;

		//要先处理right和bottom
		DstRC.rect.right = x + DstRC.rect.right - DstRC.rect.left;
		DstRC.rect.bottom = y + DstRC.rect.bottom - DstRC.rect.top;

		DstRC.rect.left = x;
		DstRC.rect.top = y;

		this->_PreDisposeRC();

		ClipRC.left = DstRC.rect.left + StrokeWidth / 2.0f;
		ClipRC.top = DstRC.rect.top + StrokeWidth / 2.0f;
		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(UpdateRC, OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(UpdateRC, OldUpdateRC);
			RectChanged = true;
		}
		___RcChangedLock.Unlock();

		if (Parent)
		{
			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(true, CalcRectUnion(UpdateRC, OldUpdateRC));
		}
		else
			this->_SetNeedUpdate(true);
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_RC_width.AddAni(DstRC.rect.right - DstRC.rect.left, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_RC_height.AddAni(DstRC.rect.bottom - DstRC.rect.top, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}
		D2D1_RECT_F OldUpdateRC = UpdateRC;

		DstRC.rect.right = DstRC.rect.left + width;
		DstRC.rect.bottom = DstRC.rect.top + height;

		this->_PreDisposeRC();

		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		___LayoutLock.Lock();
		RELEASE(TextLayout);
		RecreateTextInterface();//这里立即重建，是因为Hittest时如果Layout为Null，则直接失败导致状态频繁切换
		___LayoutLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(UpdateRC, OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(UpdateRC, OldUpdateRC);
			RectChanged = true;
		}
		___RcChangedLock.Unlock();

		if (Parent)
		{
			___BackImgTargetLock.Lock();
			RELEASE(BackImgTarget);
			___BackImgTargetLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(true, CalcRectUnion(UpdateRC, OldUpdateRC));
		}
		else
			this->_SetNeedUpdate(true);
		return;
	}

	//此函数稍大且用的很少，不做inline处理
	void SetTypeStyle(DControlState State, DLabelTypeStyle type)
	{
		switch (State)
		{
		case DControlState::Normal:
			memcpy_s(&NormalType, sizeof(NormalType), &type, sizeof(NormalType));
			break;
		case DControlState::MouseMove:
			memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &type, sizeof(MouseMoveType));
			break;
		case DControlState::Click:
			memcpy_s(&ClickType, sizeof(ClickType), &type, sizeof(ClickType));
			break;
		}

		this->_SetNeedUpdate(true);//设置完后将状态置为需要更新
		return;
	}

	inline void SetText(std::wstring text)
	{
		___LayoutLock.Lock();
		Text = text;
		RELEASE(TextLayout);//文字发生更改，必须重新创建(Draw时自动重建)，没有修改的方法

		RecreateTextInterface();//这里立即重建，是因为Hittest时如果Layout为Null，则直接失败导致状态频繁切换
		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);//设置完后将状态置为需要更新
		return;
	}

	//此函数稍大且用的很少，不做inline处理
	void SetFontFormat(std::wstring FamliyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case DFontWeight::NormalWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::LightWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::BoldWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;

		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		___LayoutLock.Lock();
		if (TextLayout)
		{
			TextLayout->SetFontFamilyName(this->FamliyName.c_str(), { 0, Text.size() });
			TextLayout->SetFontSize(this->FontSize, { 0, Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, Text.size() });
		}
		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);//设置完后将状态置为需要更新
		return;
	}

	bool SetState(DControlState state)
	{
		if (State != state)
		{
			float time = 0.0f;
			switch (state)
			{
			case DControlState::Normal:
				time = NormalType.time;
				break;
			case DControlState::MouseMove:
				time = MouseMoveType.time;
				break;
			case DControlState::Click:
				time = ClickType.time;
				break;
			}

			UINT Last = NowFrameCount;
			if (state == LastState && NowFrameCount && (UINT)(FrameRate * time) > NowFrameCount)
			{
				NowFrameCount = (UINT)(FrameRate * time - NowFrameCount);
			}
			else {
				NowFrameCount = (UINT)(FrameRate * time);
			}

			if (NowFrameCount == 0)
				NowFrameCount = 1;

			switch (state)
			{
			case DControlState::Normal:
				//文字颜色
				MidType = CalcMidType(NormalType.TextColor, NowType, NowFrameCount);

				break;
			case DControlState::MouseMove:
				//文字颜色
				MidType = CalcMidType(MouseMoveType.TextColor, NowType, NowFrameCount);

				break;
			case DControlState::Click:
				//文字颜色
				MidType = CalcMidType(ClickType.TextColor, NowType, NowFrameCount);

				break;
			}

			LastState = State;
			State = state;

			this->_SetNeedUpdate(true);

			___MsgBackLock.Lock();
			if (OwnerWnd && proc)
				proc(OwnerWnd, ID, ControlType, DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);
			___MsgBackLock.Unlock();

			return true;
		}
		return false;
	}

private:
	DMouseStyle MouseStyle = DMouseStyle::BasicMouse;

	DLabelTypeStyle NormalType = { 0 };
	DLabelTypeStyle MouseMoveType = { 0 };
	DLabelTypeStyle ClickType = { 0 };

	D2D1_COLOR_F NowType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)
	D2D1_COLOR_F MidType = { 0 };//存储(以255为最大的颜色)(描边宽度数据)

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

	//*************************DXUI自用函数，用户也可根据实际情况自己用*************************
	inline void _SeparateFromWindow()
	{
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Opa.Reset();
		//把所有设备相关资源都释放
		RELEASE(brush);
		RELEASE(TextFormat);
		RELEASE(TextLayout);
		RELEASE(BackImgTarget);
		RELEASE(BackImg);
		RELEASE(HitTestBmpTarget);
		RELEASE(HitTestBmp);
		DeleteArray(_HitTest);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}

	void _SetLastFrameCount(UINT count)
	{
		this->NowFrameCount = count;
		return;
	}

	bool _SetCursorMsgProc()
	{
		if (MouseStyle == DMouseStyle::HandMouse)
		{
			SetCursor(LoadCursorW(nullptr, IDC_HAND));
			return true;
		}
		else
			return false;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		if (NowControlID == ID)
			return false;

		OnClick = false;

		return SetState(DControlState::Normal);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		if (State == DControlState::Click)
		{
			return false;
		}

		if (OnClick)
			return SetState(DControlState::Click);

		return SetState(DControlState::MouseMove);
	}
	bool _LMouseDownMsgProc(int xPos, int yPos)
	{
		OnClick = true;

		return SetState(DControlState::Click);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		if (!OnClick)
		{
			return false;
		}

		OnClick = false;

		return SetState(DControlState::MouseMove);
	}



	bool _CreateHitTestBmp()//绘制命中测试BMP，由于标签用Dwrite库命中测试，所以HittestBmp不弄了
	{
		return true;
	}

	///<summary>
	///参数均为设备相关像素(DPI)
	///</summary>
	bool HitTest(int xPos, int yPos)
	{
		int mx = (int)(xPos * 96.0f / (float)dpiX), my = (int)(yPos * 96.0f / (float)dpiY);

		if (mx < DstRC.rect.left || mx > DstRC.rect.right || my < DstRC.rect.top || my > DstRC.rect.bottom)
			return false;//这一步是因为，DWrite库的HitTest不受DstRC限制，所以提前判断点是否在用户指定区域内

		___LayoutLock.Lock();
		if (TextLayout)
		{
			DWRITE_HIT_TEST_METRICS hit;
			BOOL IsBehind = false;
			BOOL isInside = false;
			TextLayout->HitTestPoint((float)(mx - DstRC.rect.left), (float)(my - DstRC.rect.top), &IsBehind, &isInside, &hit);

			___LayoutLock.Unlock();
			return isInside;
		}
		else
		{
			___LayoutLock.Unlock();
			return false;
		}
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
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamliyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, FontSize, L"zh-cn", &TextFormat);
			if (TextFormat)
			{
				TextFormat->SetWordWrapping(AutoWarp);
				TextFormat->SetTextAlignment(xAlignment);
				TextFormat->SetParagraphAlignment(yAlignment);
				RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
			}
		}

		if (!TextLayout && TextFormat && DWriteFactory)
			DWriteFactory->CreateTextLayout(Text.c_str(), Text.size(), TextFormat, DstRC.rect.right - DstRC.rect.left, DstRC.rect.bottom - DstRC.rect.top, &TextLayout);
		return;
	}

	void _DisposeAnimation()
	{
		EnableClip = false;//每一帧都要置为false

		if (A_RC_x || A_RC_y)
		{
			float x = DstRC.rect.left, y = DstRC.rect.top;
			if (A_RC_x)
				x = A_RC_x.FrameCalc();
			if (A_RC_y)
				y = A_RC_y.FrameCalc();

			SetPosition(x, y);
		}
		if (A_RC_width || A_RC_height)
		{
			float wid = DstRC.rect.right - DstRC.rect.left, hei = DstRC.rect.bottom - DstRC.rect.top;
			if (A_RC_width)
				wid = A_RC_width.FrameCalc();
			if (A_RC_height)
				hei = A_RC_height.FrameCalc();

			SetSize(wid, hei);
		}
		if (A_Opa)
			SetTotalOpacity(A_Opa.FrameCalc());

		return;
	}

	bool _Draw(ID2D1BitmapRenderTarget* DstTarget, D2D1_COLOR_F BackColor)
	{
		if (!DstTarget)
		{
			this->_SetNeedUpdate(true);
			return false;
		}


		if (!brush)
		{
			DstTarget->CreateSolidColorBrush(BackColor, &brush);
			if (!brush)
				return false;
		}
		else
			brush->SetColor(BackColor);

		//处理动画的颜色变换
		DLabelTypeStyle* DstType = nullptr;

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

		if (NowFrameCount)//如果还有动画没有播放完
		{
			NowFrameCount--;//每一帧减1，直到Count为0
		}

		//动画——文字颜色处理
		NowType = CalcNowType(DstType->TextColor, MidType, NowFrameCount);
		f_TextColor = Calc_fColor(NowType, TotalOpacity);

		//********动画颜色处理部分结束********

		//尽量让锁的锁定时间减少，所以靠下放
		___LayoutLock.Lock();

		this->RecreateTextInterface();
		//设置当前状态下的文本样式
		if (TextLayout)
		{
			TextLayout->SetStrikethrough(DstType->Strikethrough, { 0, Text.size() });//下划线、删除线
			TextLayout->SetUnderline(DstType->Underline, { 0, Text.size() });
		}

		DstTarget->BeginDraw();

		___UpdateClipLock.Lock();
		if (EnableClip)//设置更新区的裁剪，主要为过渡动画时使用
			DstTarget->PushAxisAlignedClip(UpdateClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		if (Parent)//有底层控件, 要重画backimg
		{
			DstTarget->PushAxisAlignedClip(Parent->_GetClipRC(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//设置父控件的裁剪

			if (!ParentDrawing && BackImg)
			{
				DstTarget->DrawBitmap(BackImg, UpdateRC, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
		}
		else {//无底层控件，直接用窗口背景色填充
			if (EnableClip)
				DstTarget->FillRectangle(UpdateClip, brush);

			___RcChangedLock.Lock();
			if (RectChanged)
				DstTarget->FillRectangle(LastRect, brush);
			else
				DstTarget->FillRectangle(UpdateRC, brush);
			___RcChangedLock.Unlock();
		}

		//设置当前文本的裁剪区
		DstTarget->PushAxisAlignedClip(UpdateRC, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		if (TextLayout && f_TextColor.a != 0.0f)
		{
			brush->SetColor(f_TextColor);//设置为文字颜色
			DstTarget->DrawTextLayout({ (float)DstRC.rect.left, (float)DstRC.rect.top }, TextLayout, brush);
		}
		___LayoutLock.Unlock();

		DstTarget->PopAxisAlignedClip();

		//弹出裁剪区
		if (Parent)
			DstTarget->PopAxisAlignedClip();

		if (EnableClip)
			DstTarget->PopAxisAlignedClip();
		___UpdateClipLock.Unlock();

		DstTarget->EndDraw();

		_EndDraw_CallChildToUpdateBackImg(DstTarget);

		this->_SetNeedUpdate(NowFrameCount);
		return true;
	}

};










class DTextBox : public DControl
{
public:

	~DTextBox()
	{
		this->Release();
		return;
	}

	bool Init(POINT position, SIZE size, DTextBoxTypeStyle Normal, DTextBoxTypeStyle MouseMove, DTextBoxTypeStyle Edit, DTextBoxTypeStyle Disable, float radiusX, float radiusY, float StrokeWidth, std::wstring FamliyName, float FontSize, bool AutoWarp = true, bool MultiLine = true, bool ReadOnly = false, DFontWeight FontWeight = DFontWeight::NormalWeight, DAlignment X_Alignment = DAlignment::Near, DAlignment Y_Alignment = DAlignment::Near, DCOLOR CaretColor = { 0, 0, 0, 1.0f }, float CaretWidth = 1.5f, DCOLOR SelectFillColor = { 0, 0, 0, 0.45f })
	{
		DstRC.rect.left = (float)position.x;
		DstRC.rect.top = (float)position.y;
		DstRC.rect.right = (float)position.x + size.cx;
		DstRC.rect.bottom = (float)position.y + size.cy;
		DstRC.radiusX = radiusX;
		DstRC.radiusY = radiusY;

		ClipRC.left = DstRC.rect.left + StrokeWidth / 2.0f;
		ClipRC.top = DstRC.rect.top + StrokeWidth / 2.0f;
		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		//文本输出的区域(考虑到输入区域的边缘不能与边框重叠)
		TextOutputRC.left = DstRC.rect.left + 5.0f;
		TextOutputRC.top = DstRC.rect.top + 5.0f;
		TextOutputRC.right = DstRC.rect.right - 5.0f;
		TextOutputRC.bottom = DstRC.rect.bottom - 5.0f;

		//设置光标的位置
		Insert_PixelPos.left = TextOutputRC.left * (float)dpiX / 96.0f;
		Insert_PixelPos.top = TextOutputRC.top * (float)dpiY / 96.0f;
		Insert_PixelPos.right = Insert_PixelPos.left + CaretWidth;
		Insert_PixelPos.bottom = Insert_PixelPos.top * (float)dpiY / 96.0f;

		memcpy_s(&NormalType, sizeof(NormalType), &Normal, sizeof(Normal));
		memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &MouseMove, sizeof(MouseMove));
		memcpy_s(&EditType, sizeof(EditType), &Edit, sizeof(Edit));
		memcpy_s(&DisableType, sizeof(DisableType), &Disable, sizeof(Disable));

		DCOLOR CaretC;
		memcpy_s(&CaretC, sizeof(CaretC), &CaretColor, sizeof(CaretColor));

		//判断如果大于255则设为255
		DetectAndResetColor(NormalType.FillColor);
		DetectAndResetColor(NormalType.StrokeColor);
		DetectAndResetColor(NormalType.TextColor);

		DetectAndResetColor(MouseMoveType.FillColor);
		DetectAndResetColor(MouseMoveType.StrokeColor);
		DetectAndResetColor(MouseMoveType.TextColor);

		DetectAndResetColor(EditType.FillColor);
		DetectAndResetColor(EditType.StrokeColor);
		DetectAndResetColor(EditType.TextColor);

		DetectAndResetColor(DisableType.FillColor);
		DetectAndResetColor(DisableType.StrokeColor);
		DetectAndResetColor(DisableType.TextColor);

		DetectAndResetColor(CaretC);
		//初始化当前样式
		NowType.FillColor = DcolToColf(NormalType.FillColor);
		NowType.StrokeColor = DcolToColf(NormalType.StrokeColor);
		NowType.TextColor = DcolToColf(NormalType.TextColor);

		this->CaretColor.r = CaretC.r / 255.0f;
		this->CaretColor.g = CaretC.g / 255.0f;
		this->CaretColor.b = CaretC.b / 255.0f;
		this->CaretColor.a = CaretC.a;
		this->CaretWidth = CaretWidth;

		this->StrokeWidth = StrokeWidth;

		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		this->MultiLine = MultiLine;
		this->ReadOnly = ReadOnly;

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
		case DFontWeight::NormalWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::LightWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::BoldWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;

		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

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

		this->Select_FillColor.r = (float)(SelectFillColor.r / 255.0f);
		this->Select_FillColor.g = (float)(SelectFillColor.g / 255.0f);
		this->Select_FillColor.b = (float)(SelectFillColor.b / 255.0f);
		this->Select_FillColor.a = SelectFillColor.a;

		Insert_LastUpdatetime = timeGetTime();

		this->ControlType = DControlType::TextBox;
		this->TotalOpacity = 1.0f;

		Inited = true;

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
		MidType = { 0 };//存储以255为最大的颜色
		RELEASE(brush);
		//文本区
		RELEASE(SelfTextClip);//自身的文字裁剪区，防止文字超出范围
		ImeChar[0] = 0;//接受到消息时存储单个字，某些生僻字占用4字节
		ImeChar[1] = 0;
		Text.clear();
		FamliyName.clear();
		FontSize = 0.0f;
		MultiLine = true;
		ReadOnly = false;
		TextOutputRC = { 0.0f };
		AutoWarp = DWRITE_WORD_WRAPPING_WRAP;
		FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
		xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
		yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		//插入符
		Insert_Visible = false;
		Insert_LastUpdatetime = 0;
		Insert_TextPos = 0;
		Insert_PixelPos = { 0.0f };
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
		Select_FillColor = { 0.0f };
		//文本滚动
		Scr_Offset = { 0.0f };//(DIP像素)
		//文本接口
		RELEASE(TextFormat);
		RELEASE(TextLayout);

		return;
	}

	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_Radius_X.AddAni(DstRC.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_Radius_Y.AddAni(DstRC.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}

		DstRC.radiusX = RadiusX;
		DstRC.radiusY = RadiusY;

		//重新创建HitTest
		this->_CreateHitTestBmp();
		this->_DrawHitTestBmp();
		this->_HitTestBmpToMemory();

		if (Parent)
			Parent->_SetNeedUpdate(true, UpdateRC);
		else
			this->_SetNeedUpdate(true);

		return;
	}
	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_RC_x.AddAni(DstRC.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_RC_y.AddAni(DstRC.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}
		D2D1_RECT_F OldUpdateRC = UpdateRC;
		//要先处理right和bottom
		DstRC.rect.right = x + DstRC.rect.right - DstRC.rect.left;
		DstRC.rect.bottom = y + DstRC.rect.bottom - DstRC.rect.top;

		DstRC.rect.left = x;
		DstRC.rect.top = y;

		this->_PreDisposeRC();

		ClipRC.left = DstRC.rect.left + StrokeWidth / 2.0f;
		ClipRC.top = DstRC.rect.top + StrokeWidth / 2.0f;
		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		TextOutputRC.left = DstRC.rect.left + 5.0f;
		TextOutputRC.top = DstRC.rect.top + 5.0f;
		TextOutputRC.right = DstRC.rect.right - 5.0f;
		TextOutputRC.bottom = DstRC.rect.bottom - 5.0f;

		___TextClipLock.Lock();
		RELEASE(SelfTextClip);
		___TextClipLock.Unlock();

		TextPosToInsertPos();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(UpdateRC, OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(UpdateRC, OldUpdateRC);
			RectChanged = true;
		}
		___RcChangedLock.Unlock();

		if (Parent)
		{
			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(true, CalcRectUnion(UpdateRC, OldUpdateRC));
		}
		else
			this->_SetNeedUpdate(true);
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_RC_width.AddAni(DstRC.rect.right - DstRC.rect.left, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_RC_height.AddAni(DstRC.rect.bottom - DstRC.rect.top, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}
		D2D1_RECT_F OldUpdateRC = UpdateRC;

		DstRC.rect.right = DstRC.rect.left + width;
		DstRC.rect.bottom = DstRC.rect.top + height;

		this->_PreDisposeRC();

		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		TextOutputRC.right = DstRC.rect.right - 5.0f;
		TextOutputRC.bottom = DstRC.rect.bottom - 5.0f;

		//重新创建HitTest
		this->_CreateHitTestBmp();
		this->_DrawHitTestBmp();
		this->_HitTestBmpToMemory();

		___TextClipLock.Lock();
		RELEASE(SelfTextClip);
		___TextClipLock.Unlock();

		___LayoutLock.Lock();

		RELEASE(TextLayout);
		TextPosToInsertPos(false);

		___LayoutLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(UpdateRC, OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(UpdateRC, OldUpdateRC);
			RectChanged = true;
		}
		___RcChangedLock.Unlock();

		if (Parent)
		{
			___BackImgTargetLock.Lock();
			RELEASE(BackImgTarget);
			___BackImgTargetLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(true, CalcRectUnion(UpdateRC, OldUpdateRC));
		}
		else
			this->_SetNeedUpdate(true);
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
		Insert_PixelPos.right = Insert_PixelPos.left + this->CaretWidth;
		return;
	}
	//此函数稍大且用的很少，不做inline处理
	void SetTypeStyle(DControlState State, DTextBoxTypeStyle type)
	{
		switch (State)
		{
		case DControlState::Normal:
			memcpy_s(&NormalType, sizeof(NormalType), &type, sizeof(type));
			break;
		case DControlState::MouseMove:
			memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &type, sizeof(type));
			break;
		case DControlState::Edit:
			memcpy_s(&EditType, sizeof(EditType), &type, sizeof(type));
			break;
		case DControlState::Disable:
			memcpy_s(&DisableType, sizeof(DisableType), &type, sizeof(type));
			break;
		}

		this->_SetNeedUpdate(true);//设置完后将状态置为需要更新
		return;
	}

	//此函数稍大且用的很少，不做inline处理
	void SetFontFormat(std::wstring FamliyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case DFontWeight::NormalWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case DFontWeight::LightWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case DFontWeight::BoldWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;

		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		___LayoutLock.Lock();
		if (TextLayout)
		{
			TextLayout->SetFontFamilyName(this->FamliyName.c_str(), { 0, Text.size() });
			TextLayout->SetFontSize(this->FontSize, { 0, Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, Text.size() });
		}
		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);//设置完后将状态置为需要更新
		return;
	}

	bool SetState(DControlState state)
	{
		if (State != state)
		{
			float time = 0.0f;
			switch (state)
			{
			case DControlState::Normal:
				time = NormalType.time;
				break;
			case DControlState::MouseMove:
				time = MouseMoveType.time;
				break;
			case DControlState::Edit:
				time = EditType.time;
				break;
			case DControlState::Disable:
				time = DisableType.time;
				break;
			}

			UINT Last = NowFrameCount;
			if (state == LastState && NowFrameCount && (UINT)(FrameRate * time) > NowFrameCount)
			{
				NowFrameCount = (UINT)(FrameRate * time - NowFrameCount);
			}
			else {
				NowFrameCount = (UINT)(FrameRate * time);
			}

			if (NowFrameCount == 0)
				NowFrameCount = 1;

			switch (state)
			{
			case DControlState::Normal:
				//填充颜色
				MidType.FillColor = CalcMidType(NormalType.FillColor, NowType.FillColor, NowFrameCount);
				//描边颜色
				MidType.StrokeColor = CalcMidType(NormalType.StrokeColor, NowType.StrokeColor, NowFrameCount);
				//文字颜色
				MidType.TextColor = CalcMidType(NormalType.TextColor, NowType.TextColor, NowFrameCount);

				break;
			case DControlState::MouseMove:
				//填充颜色
				MidType.FillColor = CalcMidType(MouseMoveType.FillColor, NowType.FillColor, NowFrameCount);
				//描边颜色
				MidType.StrokeColor = CalcMidType(MouseMoveType.StrokeColor, NowType.StrokeColor, NowFrameCount);
				//文字颜色
				MidType.TextColor = CalcMidType(MouseMoveType.TextColor, NowType.TextColor, NowFrameCount);

				break;
			case DControlState::Edit:
				//填充颜色
				MidType.FillColor = CalcMidType(EditType.FillColor, NowType.FillColor, NowFrameCount);
				//描边颜色
				MidType.StrokeColor = CalcMidType(EditType.StrokeColor, NowType.StrokeColor, NowFrameCount);
				//文字颜色
				MidType.TextColor = CalcMidType(EditType.TextColor, NowType.TextColor, NowFrameCount);

				break;
			case DControlState::Disable:
				//填充颜色
				MidType.FillColor = CalcMidType(DisableType.FillColor, NowType.FillColor, NowFrameCount);
				//描边颜色
				MidType.StrokeColor = CalcMidType(DisableType.StrokeColor, NowType.StrokeColor, NowFrameCount);
				//文字颜色
				MidType.TextColor = CalcMidType(DisableType.TextColor, NowType.TextColor, NowFrameCount);

				break;
			}

			LastState = State;
			State = state;

			this->_SetNeedUpdate(true);

			___MsgBackLock.Lock();
			if (OwnerWnd && proc)
				proc(OwnerWnd, ID, ControlType, DControlMsg::Control_StateChanged, (WPARAM)LastState, (LPARAM)State);
			___MsgBackLock.Unlock();

			return true;
		}
		return false;
	}

	///<summary>
	///在文本后端插入一个字符，然后把Caret移动到此处。此函数会进行边界检查
	///</summary>
	inline void AddChar(const wchar_t Ch)
	{
		___LayoutLock.Lock();

		if (!MultiLine && (Ch == L'\r' || Ch == L'\n'))
			Text += L' ';
		else
			Text += Ch;

		//立即刷新插入符
		this->MoveCaret_Offset(1, false);

		RELEASE(TextLayout);

		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);

		___MsgBackLock.Lock();
		if (OwnerWnd && proc)
			proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_TextChange, 0, 0);
		___MsgBackLock.Unlock();

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
	inline void InsertChar(UINT TextPos, const wchar_t Ch)
	{
		UINT textpos;

		___LayoutLock.Lock();

		if (TextPos > Text.size())
			textpos = Text.size();
		else textpos = TextPos;

		if (!MultiLine && (Ch == L'\r' || Ch == L'\n'))
			Text.insert(textpos, 1, L' ');
		else
			Text.insert(textpos, 1, Ch);

		//立即刷新插入符
		this->MoveCaret_Offset(1, false);

		RELEASE(TextLayout);

		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);

		___MsgBackLock.Lock();
		if (OwnerWnd && proc)
			proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_TextChange, 0, 0);
		___MsgBackLock.Unlock();

		return;
	}
	///<summary>
	///在文本后端添加一个字符串，然后将Caret移动到文本末端
	///</summary>
	inline void AddText(std::wstring str)
	{
		___LayoutLock.Lock();

		if (!MultiLine)//不是多行时，把换行全部转成空格
		{
			std::wstring s = str;

			int offindex = s.find(L'\r', 0);
			while (offindex != -1)
			{
				s.replace(offindex, 1, L" ");
				offindex++;
				offindex = s.find(L'\r', offindex);
			}
			offindex = s.find(L'\n', 0);
			while (offindex != -1)
			{
				s.replace(offindex, 1, L" ");
				offindex++;
				offindex = s.find(L'\n', offindex);
			}
			Text += s.c_str();
		}
		else Text += str.c_str();

		//立即刷新插入符
		this->MoveCaret_Offset(str.size(), false);

		RELEASE(TextLayout);

		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);

		___MsgBackLock.Lock();
		if (OwnerWnd && proc)
			proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_TextChange, 0, 0);
		___MsgBackLock.Unlock();

		return;
	}
	///<summary>
	///指定一个文本位置并插入，然后将Caret移动到插入后的文本末端。此函数会进行边界检查
	///</summary>
	inline void InsertText(UINT TextPos, std::wstring str)
	{
		UINT textpos;

		___LayoutLock.Lock();

		if (TextPos > Text.size())
			textpos = Text.size();
		else textpos = TextPos;

		if (!MultiLine)//不是多行时，把换行全部转成空格
		{
			std::wstring s = str;

			int offindex = s.find(L'\r', 0);
			while (offindex != -1)
			{
				s.replace(offindex, 1, L" ");
				offindex++;
				offindex = s.find(L'\r', offindex);
			}
			offindex = s.find(L'\n', 0);
			while (offindex != -1)
			{
				s.replace(offindex, 1, L" ");
				offindex++;
				offindex = s.find(L'\n', offindex);
			}
			Text.insert(textpos, s.c_str(), s.size());
		}
		else Text.insert(textpos, str.c_str(), str.size());

		//立即刷新插入符
		this->MoveCaret(TextPos + str.size(), false);

		RELEASE(TextLayout);

		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);

		___MsgBackLock.Lock();
		if (OwnerWnd && proc)
			proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_TextChange, 0, 0);
		___MsgBackLock.Unlock();

		return;
	}
	///<summary>
	///默认在当前的文本位置插入字符串，然后把Caret移动到插入后的文本末端。此函数会进行边界检查
	///</summary>
	inline void InsertText(std::wstring str)
	{
		this->InsertText(Insert_TextPos, str);
		return;
	}

	///<summary>
	///设置当前文本框文本，并将Caret移动到文本末端。此函数会进行边界检查
	///</summary>
	inline UINT GetLength()
	{
		return Text.size();
	}
	///<summary>
	///设置当前文本框文本，并将Caret移动到文本末端。此函数会进行边界检查
	///</summary>
	inline void SetText(std::wstring str)
	{
		___LayoutLock.Lock();

		if (!MultiLine)//不是多行时，把换行全部转成空格
		{
			std::wstring s = str;

			int offindex = s.find(L'\r', 0);
			while (offindex != -1)
			{
				s.replace(offindex, 1, L" ");
				offindex++;
				offindex = s.find(L'\r', offindex);
			}
			offindex = s.find(L'\n', 0);
			while (offindex != -1)
			{
				s.replace(offindex, 1, L" ");
				offindex++;
				offindex = s.find(L'\n', offindex);
			}
			Text = s;
		}
		else Text = str;

		//立即刷新插入符
		this->MoveCaret(str.size(), false);

		RELEASE(TextLayout);

		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);

		___MsgBackLock.Lock();
		if (OwnerWnd && proc)
			proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_TextChange, 0, 0);
		___MsgBackLock.Unlock();

		return;
	}
	///<summary>
	///在指定文本位置向后擦除Count个字符串，Caret移动到开始擦除的位置。此函数会进行边界检查
	///</summary>
	inline void EraseText(UINT TextPos, UINT Count)
	{
		UINT textpos;
		UINT count;

		___LayoutLock.Lock();

		if (TextPos > Text.size())
			textpos = Text.size();
		else textpos = TextPos;

		if (Count > Text.size())
			count = Text.size();
		else count = Count;

		Text.erase(TextPos, Count);

		//立即刷新插入符
		this->MoveCaret(TextPos, false);

		RELEASE(TextLayout);

		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);

		___MsgBackLock.Lock();
		if (OwnerWnd && proc)
			proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_TextChange, 0, 0);
		___MsgBackLock.Unlock();

		return;
	}
	///<summary>
	///在当前文本位置向后擦除Count个字符串，Caret还是当前位置。此函数会进行边界检查
	///</summary>
	inline void EraseText(UINT Count)
	{
		UINT count;

		___LayoutLock.Lock();

		if (Count > Text.size())
			count = Text.size();
		else count = Count;

		Text.erase(Insert_TextPos, Count);

		//立即刷新插入符
		this->MoveCaret_Offset(0, false);

		RELEASE(TextLayout);

		___LayoutLock.Unlock();

		this->_SetNeedUpdate(true);

		___MsgBackLock.Lock();
		if (OwnerWnd && proc)
			proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_TextChange, 0, 0);
		___MsgBackLock.Unlock();

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
	inline void MoveCaret_Offset(int OffsetPos, bool ReDraw = true)
	{
		if ((Insert_TextPos == 0 && OffsetPos < 0) || (Insert_TextPos == Text.size() && OffsetPos > 0))//无用的设置直接return，不然总是刷新导致GPU占用过高
			return;

		long pos;

		pos = Insert_TextPos + OffsetPos;

		if (pos < 0)
			pos = 0;
		else if (pos > (long)Text.size())
			pos = Text.size();

		if (Insert_TextPos != pos)
		{
			Insert_TextPos = (UINT)pos;

			if (ReDraw)//如果要重绘，则重新计算插入符像素位置
				TextPosToInsertPos();
		}
		else
			ResetTextScrolling();
		//立即刷新插入符
		Insert_LastUpdatetime = timeGetTime();//重置插入符的闪烁时间
		Insert_Visible = true;
		if (ReDraw)
			this->_SetNeedUpdate(true);
		return;
	}
	///<summary>
	///直接设置当前的插入符位置，此函数会进行边界检查
	///</summary>
	inline void MoveCaret(UINT TextPos, bool ReDraw = true)
	{
		//立即刷新插入符
		if (Insert_TextPos != TextPos)
		{
			if (TextPos > Text.size())//因为是UINT，所以只考虑大于的情况
				Insert_TextPos = Text.size();
			else
				Insert_TextPos = TextPos;

			if (ReDraw)//如果要重绘，则重新计算插入符像素位置
				TextPosToInsertPos();
		}
		else {
			if (Insert_TextPos > Text.size())
				Insert_TextPos = Text.size();
			ResetTextScrolling();
		}
		Insert_LastUpdatetime = timeGetTime();//重置插入符的闪烁时间
		Insert_Visible = true;
		if (ReDraw)
			this->_SetNeedUpdate(true);
		return;
	}
	///<summary>
	///选中当前所有文本
	///</summary>
	inline void SetSelectText_All()
	{
		Select_BeginPos = 0;
		Select_EndPos = Text.size();

		UpdateSelectPos(true);
		return;
	}
	///<summary>
	///选中一个文本区域，此函数会进行边界检查
	///</summary>
	inline void SetSelectText(UINT BeginTextPos, UINT EndTextPos)
	{
		Select_BeginPos = BeginTextPos;
		Select_EndPos = EndTextPos;

		if (Select_BeginPos < 0)
			Select_BeginPos = 0;
		else if (Select_BeginPos > Text.size())
			Select_BeginPos = Text.size();

		if (Select_EndPos < 0)
			Select_EndPos = 0;
		else if (Select_EndPos > Text.size())
			Select_EndPos = Text.size();

		UpdateSelectPos(true);
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

	Type NowType = { 0 };//存储以255为最大的颜色
	Type MidType = { 0 };//存储以255为最大的颜色

	D2D1_COLOR_F CaretColor = { 0, 0, 0, 1.0f };
	float CaretWidth = 1.5f;

	ID2D1SolidColorBrush* brush = nullptr;

	//文本区
	ID2D1RoundedRectangleGeometry* SelfTextClip = nullptr;//自身的文字裁剪区，防止文字超出范围
	___DThreadLock ___TextClipLock;
	wchar_t ImeChar[2] = { 0 };//接受到消息时存储单个字，某些生僻字占用4字节

	std::wstring Text;
	std::wstring FamliyName;
	float FontSize = 0.0f;
	bool MultiLine = true;

	bool ReadOnly = false;

	D2D1_RECT_F TextOutputRC = { 0.0f };
	DWRITE_WORD_WRAPPING AutoWarp = DWRITE_WORD_WRAPPING_WRAP;

	DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL;

	DWRITE_TEXT_ALIGNMENT xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;

	//插入符
	bool Insert_Visible = false;
	DWORD Insert_LastUpdatetime = 0;
	UINT Insert_TextPos = 0;//文本位置，插入符总在该位置字符的前面
	D2D1_RECT_F Insert_PixelPos = { 0.0f };

	//选中文本
	bool SelectMode = false;
	DWRITE_TEXT_METRICS Select_Metrics = { 0 };
	DWRITE_HIT_TEST_METRICS* Select_Rects = nullptr;
	UINT Select_RectCount = 0;
	UINT Select_DownPos = 0;
	UINT Select_BeginPos = 0;
	UINT Select_EndPos = 0;
	D2D1_COLOR_F Select_FillColor = { 0.0f };

	//文本滚动
	D2D1_POINT_2F Scr_Offset = { 0.0f };//(DIP像素)

	//文本接口
	IDWriteTextFormat* TextFormat = nullptr;
	IDWriteTextLayout* TextLayout = nullptr;
	___DThreadLock ___LayoutLock;

	inline void _SeparateFromWindow()
	{
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Opa.Reset();
		//把所有设备相关资源都释放
		RELEASE(brush);
		RELEASE(TextFormat);
		RELEASE(TextLayout);
		RELEASE(BackImgTarget);
		RELEASE(BackImg);
		RELEASE(HitTestBmpTarget);
		RELEASE(HitTestBmp);
		DeleteArray(_HitTest);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}

	void _SetLastFrameCount(UINT count)
	{
		this->NowFrameCount = count;
		return;
	}


	bool _GetNeedUpdate()//根据每个控件的GetNeedUpdate每帧都会调用的特性，在这里为插入符计时并设置刷新
	{
		if (State == DControlState::Edit)
		{
			if (Insert_Visible)
			{
				if (timeGetTime() - Insert_LastUpdatetime > 500)
				{
					Insert_Visible = false;
					this->_SetNeedUpdate(true);
					Insert_LastUpdatetime = timeGetTime();
				}
			}
			else {
				if (timeGetTime() - Insert_LastUpdatetime > 500)
				{
					Insert_Visible = true;
					this->_SetNeedUpdate(true);
					Insert_LastUpdatetime = timeGetTime();
				}
			}
		}

		return NeedUpdate;
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
				if (SelectMode)
				{
					this->MoveCaret(Select_BeginPos);
					this->ExitSelectMode();
					break;
				}

				if (Insert_TextPos > 1 && Text.at(Insert_TextPos - 1) == L'\n' && Text.at(Insert_TextPos - 2) == '\r')
					this->MoveCaret_Offset(-2);
				else
					this->MoveCaret_Offset(-1);
				break;
			}
			case VK_RIGHT:
			{
				if (SelectMode)
				{
					this->MoveCaret(Select_EndPos);
					this->ExitSelectMode();
					break;
				}

				if (Insert_TextPos < Text.size() && Text.at(Insert_TextPos) == L'\r' && Text.at(Insert_TextPos + 1) == '\n')
				{
					this->MoveCaret_Offset(2);
				}
				else
				{
					this->MoveCaret_Offset(1);
				}
				break;
			}
			case VK_UP:
			{
				if (SelectMode)
					this->ExitSelectMode();

				this->ResetInsertPos((int)(Insert_PixelPos.left - Scr_Offset.x * (float)dpiX / 96.0f), (int)(Insert_PixelPos.top - Scr_Offset.y * (float)dpiY / 96.0f - 1.0f));
				break;
			}
			case VK_DOWN:
			{
				if (SelectMode)
					this->ExitSelectMode();

				this->ResetInsertPos((int)(Insert_PixelPos.left - Scr_Offset.x * (float)dpiX / 96.0f), (int)(Insert_PixelPos.bottom - Scr_Offset.y * (float)dpiY / 96.0f + 1.0f));
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
								if (SelectMode)
								{
									this->EraseText(Select_BeginPos, Select_EndPos - Select_BeginPos);
									this->ExitSelectMode();
								}

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
				if (GetKeyState(VK_CONTROL) & 0x8000 && SelectMode)
				{
					HGLOBAL hGlobal;
					wchar_t* pGlobal;

					std::wstring str = Text.substr(Select_BeginPos, Select_EndPos - Select_BeginPos);

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
					return true;
				}
				break;
			}//case C End

			case 'A':
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					Select_DownPos = Text.size();
					this->SetSelectText_All();
					return true;
				}
				break;
			}//case A End
			case 'X':
			{
				if (GetKeyState(VK_CONTROL) & 0x8000 && SelectMode)
				{
					//先写入剪贴板
					HGLOBAL hGlobal;
					wchar_t* pGlobal;

					std::wstring str = Text.substr(Select_BeginPos, Select_EndPos - Select_BeginPos);

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
					//然后删掉文本
					if (!ReadOnly)
						this->EraseText(Select_BeginPos, Select_EndPos - Select_BeginPos);

					this->ExitSelectMode();

					return true;
				}
				break;
			}//case X End

			}//switch End
		}

		return false;
	}

	bool _CharMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (State != DControlState::Edit || ReadOnly)
			return false;

		switch (Msg)
		{
		case WM_IME_CHAR:
		{
			//其他字符 放入数组中
			if (ImeChar[0] >= 0xD800 && ImeChar[0] <= 0xDBFF)
			{
				ImeChar[1] = (wchar_t)wParam;
			}
			else
			{
				ImeChar[0] = (wchar_t)wParam;
				if (ImeChar[0] >= 0xD800 && ImeChar[0] <= 0xDBFF)
					break;
			}

			if (SelectMode)
			{
				this->EraseText(Select_BeginPos, Select_EndPos - Select_BeginPos);
				this->ExitSelectMode();
			}

			this->InsertText(ImeChar);
			ImeChar[0] = '\0';
			ImeChar[1] = '\0';

			break;
		}

		case WM_CHAR:
		{
			if (GetKeyState(VK_CONTROL) & 0x8000)
				break;

			if (SelectMode && wParam != 0x08)
			{
				this->EraseText(Select_BeginPos, Select_EndPos - Select_BeginPos);
				this->ExitSelectMode();
			}

			switch (wParam)
			{
			case 0x0D://回车键
				if (!MultiLine)//单行模式
				{
					___MsgBackLock.Lock();
					if (OwnerWnd && proc)
						proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_Enter, 0, 0);
					___MsgBackLock.Unlock();
					break;
				}

				this->InsertText(L"\r\n");

				___MsgBackLock.Lock();
				if (OwnerWnd && proc)
					proc(OwnerWnd, ID, ControlType, DControlMsg::TextBox_Enter, 0, 0);
				___MsgBackLock.Unlock();

				break;
			case 0x08://退格键
				if (!Text.empty())
				{
					if (SelectMode)
					{
						this->EraseText(Select_BeginPos, Select_EndPos - Select_BeginPos);
						this->ExitSelectMode();
					}
					else
					{
						if (Insert_TextPos >= 2 && Text.at(Insert_TextPos - 1) == '\n' && Text.at(Insert_TextPos - 2) == '\r')
						{
							this->EraseText(Insert_TextPos - 2, 2);
						}
						else
						{
							this->EraseText(Insert_TextPos - 1, 1);
						}
					}
				}

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
		if (Msg == WM_IME_STARTCOMPOSITION)
		{
			HIMC hIMC = ImmGetContext(hWnd);
			if (hIMC)
			{
				COMPOSITIONFORM CompForm;
				CompForm.dwStyle = CFS_POINT;

				CompForm.ptCurrentPos.x = (long)Insert_PixelPos.left;
				CompForm.ptCurrentPos.y = (long)Insert_PixelPos.bottom;
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

				POINT p = { (long)Insert_PixelPos.left, (long)Insert_PixelPos.bottom };
				ClientToScreen(hWnd, &p);
				can->pt.x = p.x;
				can->pt.y = p.y;
				can->cLineHeight = 0;
				return true;
			}
		}
		return false;
	}
	//状态及光标处理
	bool _SetCursorMsgProc()
	{
		if (State == DControlState::Disable)
			return false;

		SetCursor(LoadCursorW(nullptr, IDC_IBEAM));//设置为工字型鼠标
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable)
		{
			return false;
		}
		if ((Msg == WM_MOUSEMOVE) && OnClick)
		{
			POINT p;
			if (GetCursorPos(&p))
			{
				if (ScreenToClient(hWnd, &p))
				{
					SetCapture(hWnd);
					this->_MouseMoveMsgProc(p.x, p.y);
					this->_SetCursorMsgProc();
				}
			}
		}
		else if (Msg == WM_LBUTTONUP)
		{
			OnClick = false;
			ReleaseCapture();
		}

		if (NowControlID == ID)
			return false;

		___ChildLock.Lock();

		//当前活动控件是它的子控件
		if (Child.find(NowControlID) != Child.end() && (Msg == WM_MOUSEMOVE || Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONUP))
		{
			if (Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONUP)
				SetState(DControlState::MouseMove);
			else
				HaveSetNormal = true;
		}
		else
			HaveSetNormal = false;

		___ChildLock.Unlock();

		if (HaveSetNormal)
			return false;

		if (State == DControlState::Edit && (Msg == WM_MOUSEMOVE || Msg == WM_LBUTTONUP || Msg == WM_MOUSELEAVE))
			return false;

		OnClick = false;

		return SetState(DControlState::Normal);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		if (HaveSetNormal)
		{
			HaveSetNormal = false;
			return true;
		}

		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable)
			return false;

		if (OnClick && !Text.empty())
		{
			SetSelectText_Pos(xPos, yPos);//不要在此设置SelectMode为true，函数会自行分辨并设置
		}

		if (State == DControlState::Edit)
			return false;

		return SetState(DControlState::MouseMove);
	}
	bool _LMouseDownMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable)
		{
			return false;
		}
		//更新插入符的位置
		ResetInsertPos(xPos, yPos);
		//取消选中模式
		if (SelectMode)
		{
			this->ExitSelectMode();
			this->_SetNeedUpdate(true);
		}
		Select_DownPos = Insert_TextPos;

		OnClick = true;
		HaveSetNormal = false;

		return SetState(DControlState::Edit);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == DControlState::Disable || !OnClick)
			return false;

		ReleaseCapture();
		OnClick = false;
		HaveSetNormal = false;

		return SetState(DControlState::Edit);
	}
	bool _MouseWheelMsgProc(WPARAM wParam)
	{
		short Delta = GET_WHEEL_DELTA_WPARAM(wParam);
		UINT ScrollLines;
		SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &ScrollLines, 0);
		if (ScrollLines == 0)
			ScrollLines = 3;

		if (Delta > 0)//手指向前滑动时
		{
			if (Scr_Offset.y > 0)
			{
				Scr_Offset.y -= FontSize * (float)ScrollLines;

				if (Scr_Offset.y < 0.0f)
					Scr_Offset.y = 0.0f;
				this->_SetNeedUpdate(true);
			}
		}
		else if (Delta < 0)//手指向后滑动时
		{
			if (Select_Metrics.height - Scr_Offset.y > TextOutputRC.bottom - TextOutputRC.top)
			{
				Scr_Offset.y += FontSize * (float)ScrollLines;

				if (Select_Metrics.height - Scr_Offset.y < TextOutputRC.bottom - TextOutputRC.top)//文本太靠上时的处理
					Scr_Offset.y = Select_Metrics.height - TextOutputRC.bottom + TextOutputRC.top;
				this->_SetNeedUpdate(true);
			}
		}

		return true;
	}


	void _DrawHitTestBmp()//绘制命中测试BMP
	{
		if (!HitTestBmpTarget)
			return;

		D2D1_COLOR_F color;
		color.r = 1.0f;
		color.g = 1.0f;
		color.b = 1.0f;
		color.a = 1.0f;

		ID2D1SolidColorBrush* bru;
		HitTestBmpTarget->CreateSolidColorBrush(color, &bru);

		if (bru)
		{
			D2D1_ROUNDED_RECT hittest = { {StrokeWidth / 2.0f + 1.0f, StrokeWidth / 2.0f + 1.0f, DstRC.rect.right - DstRC.rect.left + StrokeWidth / 2.0f, DstRC.rect.bottom - DstRC.rect.top + StrokeWidth / 2.0f}, DstRC.radiusX, DstRC.radiusY };
			HitTestBmpTarget->BeginDraw();
			HitTestBmpTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));


			HitTestBmpTarget->FillRoundedRectangle(hittest, bru);
			HitTestBmpTarget->DrawRoundedRectangle(hittest, bru, StrokeWidth);

			HitTestBmpTarget->EndDraw();

			RELEASE(bru);
		}

		return;
	}

	void _DisposeAnimation()//由主窗口每帧后主动调用此函数，来实现每次的动画
	{
		EnableClip = false;//每一帧都要置为false

		if (A_RC_x || A_RC_y)
		{
			float x = DstRC.rect.left, y = DstRC.rect.top;
			if (A_RC_x)
				x = A_RC_x.FrameCalc();
			if (A_RC_y)
				y = A_RC_y.FrameCalc();

			SetPosition(x, y);
		}
		if (A_RC_width || A_RC_height)
		{
			float wid = DstRC.rect.right - DstRC.rect.left, hei = DstRC.rect.bottom - DstRC.rect.top;
			if (A_RC_width)
				wid = A_RC_width.FrameCalc();
			if (A_RC_height)
				hei = A_RC_height.FrameCalc();

			SetSize(wid, hei);
		}
		if (A_Radius_X || A_Radius_Y)
		{
			float x = DstRC.radiusX, y = DstRC.radiusY;
			if (A_Radius_X)
				x = A_Radius_X.FrameCalc();
			if (A_Radius_Y)
				y = A_Radius_Y.FrameCalc();

			SetRadius(x, y);
		}
		if (A_Opa)
			SetTotalOpacity(A_Opa.FrameCalc());

		return;
	}
	bool _Draw(ID2D1BitmapRenderTarget* DstTarget, D2D1_COLOR_F BackColor)
	{
		if (!DstTarget)
		{
			this->_SetNeedUpdate(true);
			return false;
		}


		if (!brush)
		{
			DstTarget->CreateSolidColorBrush(BackColor, &brush);
			if (!brush)
				return false;
		}
		else
			brush->SetColor(BackColor);

		//处理动画的颜色变换
		DTextBoxTypeStyle* DstType = nullptr;

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
		case DControlState::Edit:
		{
			DstType = &EditType;

			break;
		}
		case DControlState::Disable:
		{
			DstType = &DisableType;

			break;
		}
		}

		D2D1_COLOR_F f_FillColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_StrokeColor;//以1.0f为最大的颜色结构体
		D2D1_COLOR_F f_TextColor;//以1.0f为最大的颜色结构体

		if (NowFrameCount)//如果还有动画没有播放完
		{
			NowFrameCount--;//每一帧减1，直到Count为0
		}
		//动画——填充颜色处理
		NowType.FillColor = CalcNowType(DstType->FillColor, MidType.FillColor, NowFrameCount);
		f_FillColor = Calc_fColor(NowType.FillColor, TotalOpacity);
		//动画——描边颜色处理
		NowType.StrokeColor = CalcNowType(DstType->StrokeColor, MidType.StrokeColor, NowFrameCount);
		f_StrokeColor = Calc_fColor(NowType.StrokeColor, TotalOpacity);
		//动画——文字颜色处理
		NowType.TextColor = CalcNowType(DstType->TextColor, MidType.TextColor, NowFrameCount);
		f_TextColor = Calc_fColor(NowType.TextColor, TotalOpacity);

		//*******动画颜色处理部分结束*******

		//尽量减少锁的锁定时间，靠下放
		___LayoutLock.Lock();

		RecreateTextInterface();//如果相关接口为NULL，则直接重建

		//自文本裁剪
		ID2D1Layer* TextClipLayer = nullptr;

		___TextClipLock.Lock();

		if (!SelfTextClip && D2DFactory)//如果自己的文本裁剪没有，则创建
			D2DFactory->CreateRoundedRectangleGeometry({ { DstRC.rect.left + StrokeWidth / 2.0f, DstRC.rect.top + StrokeWidth / 2.0f, DstRC.rect.right - StrokeWidth / 2.0f, DstRC.rect.bottom - StrokeWidth / 2.0f }, DstRC.radiusX, DstRC.radiusY }, &SelfTextClip);

		if (SelfTextClip)//判断一下，如果有当前文本裁剪区再创建
			DstTarget->CreateLayer(nullptr, &TextClipLayer);


		DstTarget->BeginDraw();

		___UpdateClipLock.Lock();
		if (EnableClip)//设置更新区的裁剪，主要为过渡动画时使用
			DstTarget->PushAxisAlignedClip(UpdateClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		if (Parent)//有底层控件, 要重画backimg
		{
			DstTarget->PushAxisAlignedClip(Parent->_GetClipRC(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//设置父控件的裁剪

			if (!ParentDrawing && BackImg)
			{
				DstTarget->DrawBitmap(BackImg, UpdateRC, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
		}
		else {//无底层控件，直接用窗口背景色填充
			if (EnableClip)
				DstTarget->FillRectangle(UpdateClip, brush);

			___RcChangedLock.Lock();
			if (RectChanged)
				DstTarget->FillRectangle(LastRect, brush);
			else
				DstTarget->FillRectangle(UpdateRC, brush);
			___RcChangedLock.Unlock();
		}

		if (f_FillColor.a != 0.0f)
		{
			brush->SetColor(f_FillColor);//设置为填充颜色
			DstTarget->FillRoundedRectangle(&DstRC, brush);
		}
		if (f_StrokeColor.a != 0.0f)
		{
			brush->SetColor(f_StrokeColor);//设置为描边颜色
			DstTarget->DrawRoundedRectangle(&DstRC, brush, StrokeWidth);
		}


		//设置当前文本的裁剪区
		if (TextClipLayer)//设置当前文本的裁剪区，必须等背景的东西画完了，再设置，不然背景也裁剪了
			DstTarget->PushLayer(D2D1::LayerParameters(UpdateRC, SelfTextClip), TextClipLayer);

		___TextClipLock.Unlock();
		//画文本
		if (TextLayout && f_TextColor.a != 0.0f)
		{
			brush->SetColor(f_TextColor);//设置为文字颜色
			DstTarget->DrawTextLayout({ TextOutputRC.left - Scr_Offset.x, TextOutputRC.top - Scr_Offset.y }, TextLayout, brush);
		}
		___LayoutLock.Unlock();

		//如果为选中状态，填充选中区域
		if (SelectMode && Select_Rects)
		{
			brush->SetColor({ Select_FillColor.r, Select_FillColor.g, Select_FillColor.b, Select_FillColor.a * TotalOpacity });
			D2D1_RECT_F selFill;
			for (UINT i = 0; i < Select_RectCount; i++)
			{
				selFill.left = TextOutputRC.left + Select_Rects[i].left - Scr_Offset.x;
				selFill.top = TextOutputRC.top + Select_Rects[i].top - Scr_Offset.y;
				selFill.right = selFill.left + Select_Rects[i].width;
				selFill.bottom = selFill.top + Select_Rects[i].height;
				if (Select_Rects[i].width == 0.0f)
					selFill.right += FontSize / 4.5f;//如果此行是回车，则显示一小块表示此行有内容

				DstTarget->FillRectangle(selFill, brush);
			}
		}

		//画插入符
		if (Insert_Visible && State == DControlState::Edit && CaretColor.a != 0.0f)
		{
			D2D1_RECT_F DIPPos = { Insert_PixelPos.left * 96.0f / (float)dpiX - Scr_Offset.x, Insert_PixelPos.top * 96.0f / (float)dpiY - Scr_Offset.y, Insert_PixelPos.right * 96.0f / (float)dpiX - Scr_Offset.x, Insert_PixelPos.bottom * 96.0f / (float)dpiY - Scr_Offset.y };
			brush->SetColor({ CaretColor.r, CaretColor.g, CaretColor.b, CaretColor.a * TotalOpacity });

			DstTarget->FillRectangle(DIPPos, brush);
		}

		if (TextClipLayer)//弹出自己的文本裁剪区
			DstTarget->PopLayer();

		if (Parent)
			DstTarget->PopAxisAlignedClip();

		if (EnableClip)
			DstTarget->PopAxisAlignedClip();
		___UpdateClipLock.Unlock();

		DstTarget->EndDraw();

		RELEASE(TextClipLayer);

		_EndDraw_CallChildToUpdateBackImg(DstTarget);

		this->_SetNeedUpdate(NowFrameCount);
		return true;
	}





	inline void ResetTextScrolling()
	{
		float insertX = Insert_PixelPos.right * 96.0f / (float)dpiX - TextOutputRC.left - Scr_Offset.x;
		float insertUpY = Insert_PixelPos.top * 96.0f / (float)dpiY - TextOutputRC.top - Scr_Offset.y;
		float insertBottomY = Insert_PixelPos.bottom * 96.0f / (float)dpiY - TextOutputRC.top - Scr_Offset.y;

		if (AutoWarp && insertX > Select_Metrics.layoutWidth)
		{
			Scr_Offset.x = insertX + Scr_Offset.x - Select_Metrics.layoutWidth;
		}
		else if (AutoWarp && insertX < Select_Metrics.left)
		{
			Scr_Offset.x = insertX + Scr_Offset.x - Select_Metrics.left;
			if (Scr_Offset.x < 0.0f)
				Scr_Offset.x = 0.0f;
		}

		if (insertBottomY > Select_Metrics.layoutHeight)
		{
			Scr_Offset.y = insertBottomY + Scr_Offset.y - Select_Metrics.layoutHeight;
		}
		else if (insertUpY < Select_Metrics.top)
		{
			Scr_Offset.y = insertUpY + Scr_Offset.y - Select_Metrics.top;
			if (Scr_Offset.y < 0.0f)
				Scr_Offset.y = 0.0f;
		}
		//文本太靠上时的处理
		if (Select_Metrics.height > Select_Metrics.layoutHeight&& Select_Metrics.height - Scr_Offset.y < TextOutputRC.bottom - TextOutputRC.top)
			Scr_Offset.y = Select_Metrics.height - TextOutputRC.bottom + TextOutputRC.top;
		else if (Select_Metrics.height < Select_Metrics.layoutHeight)
			Scr_Offset.y = 0.0f;

		return;
	}

	inline UINT ScreenPosToTextPos(int xPos, int yPos)
	{
		DWRITE_HIT_TEST_METRICS hit;
		BOOL IsBehind = false;
		BOOL isInside = false;
		UINT pos;

		___LayoutLock.Lock();
		RecreateTextInterface();//如果相关接口为NULL，则直接重建

		TextLayout->HitTestPoint((float)(xPos * 96.0f / (float)dpiX - TextOutputRC.left + Scr_Offset.x), (float)(yPos * 96.0f / (float)dpiY - TextOutputRC.top + Scr_Offset.y), &IsBehind, &isInside, &hit);

		___LayoutLock.Unlock();

		pos = hit.textPosition + IsBehind;
		return pos;
	}
	inline void UpdateSelectPos(bool ReDraw, bool Lock = true)
	{
		if (Text.empty())
			return;

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
			this->ExitSelectMode();

			TextPosToInsertPos();
			Insert_LastUpdatetime = timeGetTime();
			Insert_Visible = true;

			if (ReDraw)
				this->_SetNeedUpdate(true);
			return;
		}

		if (Lock)
			___LayoutLock.Lock();

		RecreateTextInterface();//如果相关接口为NULL，则直接重建

		while (TextLayout->HitTestTextRange(Select_BeginPos, Select_EndPos - Select_BeginPos, 0.0f, 0.0f, Select_Rects, bufSize, &Select_RectCount) == E_NOT_SUFFICIENT_BUFFER)
		{
			delete[] Select_Rects;
			bufSize += Select_Metrics.lineCount * Select_Metrics.maxBidiReorderingDepth;
			Select_Rects = new DWRITE_HIT_TEST_METRICS[bufSize];
		}
		TextPosToInsertPos(false);
		if (Lock)
			___LayoutLock.Unlock();

		//将插入符设为最后，不直接用类函数是因为此函数太快了，会影响到TextLayout锁，导致卡顿
		Insert_LastUpdatetime = timeGetTime();
		Insert_Visible = true;

		SelectMode = true;
		if (ReDraw)
			this->_SetNeedUpdate(true);
		return;
	}

	inline void ExitSelectMode()
	{
		SelectMode = false;
		ZeroMemory(Select_Rects, Select_RectCount * sizeof(DWRITE_HIT_TEST_METRICS));
		Select_BeginPos = 0;
		Select_EndPos = 0;
		return;
	}
	inline void SetSelectText_Pos(int xPos, int yPos)
	{
		if (Text.empty())
		{
			this->ExitSelectMode();
			return;
		}

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

		Insert_TextPos = pos;//设置插入符位置为当前位置
		if (pos > Select_DownPos)
		{
			if (Select_EndPos != pos)
			{
				Select_BeginPos = Select_DownPos;
				Select_EndPos = pos;
			}
			else if (SelectMode)
			{
				___LayoutLock.Unlock();
				TextPosToInsertPos();
				Insert_LastUpdatetime = timeGetTime();
				Insert_Visible = true;
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
				___LayoutLock.Unlock();
				TextPosToInsertPos();
				Insert_LastUpdatetime = timeGetTime();
				Insert_Visible = true;
				return;
			}
			Select_EndPos = Select_DownPos;
		}
		else
		{
			___LayoutLock.Unlock();
			this->ExitSelectMode();

			TextPosToInsertPos();
			Insert_LastUpdatetime = timeGetTime();
			Insert_Visible = true;

			this->_SetNeedUpdate(true);
			return;
		}

		RecreateTextInterface();//如果相关接口为NULL，则直接重建

		while (TextLayout->HitTestTextRange(Select_BeginPos, Select_EndPos - Select_BeginPos, 0.0f, 0.0f, Select_Rects, bufSize, &Select_RectCount) == E_NOT_SUFFICIENT_BUFFER)
		{
			delete[] Select_Rects;
			bufSize += Select_Metrics.lineCount * Select_Metrics.maxBidiReorderingDepth;
			Select_Rects = new DWRITE_HIT_TEST_METRICS[bufSize];
		}

		TextPosToInsertPos(false);
		___LayoutLock.Unlock();

		//将插入符设为最后，不直接用类函数是因为此函数太快了，会影响到TextLayout锁，导致卡顿
		Insert_LastUpdatetime = timeGetTime();
		Insert_Visible = true;

		SelectMode = true;
		this->_SetNeedUpdate(true);

		return;
	}

	inline void ResetInsertPos(int xPos, int yPos)
	{
		DWRITE_HIT_TEST_METRICS hit;
		BOOL IsBehind = false;
		BOOL isInside = false;

		___LayoutLock.Lock();

		RecreateTextInterface();//如果相关接口为NULL，则直接重建

		TextLayout->HitTestPoint((float)(xPos * 96.0f / (float)dpiX - TextOutputRC.left + Scr_Offset.x), (float)(yPos * 96.0f / (float)dpiY - TextOutputRC.top + Scr_Offset.y), &IsBehind, &isInside, &hit);

		___LayoutLock.Unlock();

		//bidi level为偶数时表示方向为从左到右，奇数时为从右到左
		if (IsBehind)
			Insert_TextPos = hit.textPosition + 1;
		else
			Insert_TextPos = hit.textPosition;

		if (hit.bidiLevel % 2)
			IsBehind = !IsBehind;
		//设置光标的位置
		if (IsBehind)
		{
			Insert_PixelPos.left = (TextOutputRC.left + hit.left + hit.width) * (float)dpiX / 96.0f;
			Insert_PixelPos.top = (TextOutputRC.top + hit.top) * (float)dpiY / 96.0f;
			Insert_PixelPos.right = Insert_PixelPos.left + CaretWidth;
			Insert_PixelPos.bottom = Insert_PixelPos.top + hit.height * (float)dpiY / 96.0f;
		}
		else {
			Insert_PixelPos.left = (TextOutputRC.left + hit.left) * (float)dpiX / 96.0f;
			Insert_PixelPos.top = (TextOutputRC.top + hit.top) * (float)dpiY / 96.0f;
			Insert_PixelPos.right = Insert_PixelPos.left + CaretWidth;
			Insert_PixelPos.bottom = Insert_PixelPos.top + hit.height * (float)dpiY / 96.0f;
		}
		this->MoveCaret_Offset(0);//立即刷新光标
		return;
	}

	inline void TextPosToInsertPos(bool Lock = true)//此函数会调用 计算滚动的函数
	{
		DWRITE_HIT_TEST_METRICS hit;
		float pointX, pointY;

		if (Lock)
		{
			___LayoutLock.Lock();
			if (TextLayout)
				TextLayout->HitTestTextPosition(Insert_TextPos, false, &pointX, &pointY, &hit);
			else {
				___LayoutLock.Unlock();
				return;
			}
			___LayoutLock.Unlock();
		}
		else
		{
			if (TextLayout)
				TextLayout->HitTestTextPosition(Insert_TextPos, false, &pointX, &pointY, &hit);
			else
				return;
		}
		//设置光标的位置

		Insert_PixelPos.left = (TextOutputRC.left + hit.left) * (float)dpiX / 96.0f;
		Insert_PixelPos.top = (TextOutputRC.top + hit.top) * (float)dpiY / 96.0f;
		Insert_PixelPos.right = Insert_PixelPos.left + CaretWidth;
		Insert_PixelPos.bottom = Insert_PixelPos.top + hit.height * (float)dpiY / 96.0f;

		ResetTextScrolling();//超出文本框时的处理
		return;
	}

	inline void RecreateTextInterface()//注意，此函数不上锁，要放在锁定区域内调用
	{
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamliyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, FontSize, L"zh-cn", &TextFormat);
			if (TextFormat)
			{
				TextFormat->SetWordWrapping(AutoWarp);
				TextFormat->SetTextAlignment(xAlignment);
				TextFormat->SetParagraphAlignment(yAlignment);
			}
			RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
		}
		if (!TextLayout && DWriteFactory && TextFormat)
		{
			DWriteFactory->CreateTextLayout(Text.c_str(), Text.size(), TextFormat, TextOutputRC.right - TextOutputRC.left, TextOutputRC.bottom - TextOutputRC.top, &TextLayout);

			if (TextLayout)
			{
				TextLayout->GetMetrics(&Select_Metrics);
				if (SelectMode)
					UpdateSelectPos(false, false);
				else TextPosToInsertPos(false);
			}
		}
		return;
	}
};







class DImageBox : public DControl
{
public:

	~DImageBox()
	{
		this->Release();
		return;
	}

	///<summary>
	///从文件路径加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromFile(POINT position, SIZE size, std::wstring Path, bool CanMoveWindow = true, DImageScaleMode ScaleMode = DImageScaleMode::EqualScale, float TotalOpacity = 1.0f, DCOLOR FillColor = { 0, 0, 0, 0.0f }, DCOLOR StrokeColor = { 0, 0, 0, 0.0f }, float radiusX = 0.0f, float radiusY = 0.0f, float StrokeWidth = 0.0f, DMouseStyle MouseStyle = DMouseStyle::BasicMouse)
	{
		this->CanMoveWindow = CanMoveWindow;

		DstRC.rect.left = (float)position.x;
		DstRC.rect.top = (float)position.y;
		DstRC.rect.right = (float)position.x + size.cx;
		DstRC.rect.bottom = (float)position.y + size.cy;
		DstRC.radiusX = radiusX;
		DstRC.radiusY = radiusY;

		ClipRC.left = DstRC.rect.left + StrokeWidth / 2.0f;
		ClipRC.top = DstRC.rect.top + StrokeWidth / 2.0f;
		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		DCOLOR fill, stroke;
		memcpy_s(&fill, sizeof(fill), &FillColor, sizeof(FillColor));
		memcpy_s(&stroke, sizeof(stroke), &StrokeColor, sizeof(StrokeColor));
		DetectAndResetColor(fill);
		DetectAndResetColor(stroke);

		this->FillColor.r = (float)(fill.r / 255.0f);
		this->FillColor.g = (float)(fill.g / 255.0f);
		this->FillColor.b = (float)(fill.b / 255.0f);
		this->FillColor.a = fill.a;

		this->StrokeColor.r = (float)(stroke.r / 255.0f);
		this->StrokeColor.g = (float)(stroke.g / 255.0f);
		this->StrokeColor.b = (float)(stroke.b / 255.0f);
		this->StrokeColor.a = stroke.a;

		this->StrokeWidth = StrokeWidth;

		this->ScaleMode = ScaleMode;
		SetTotalOpacity(TotalOpacity, 0, nullptr, false);

		this->MouseStyle = MouseStyle;

		this->ControlType = DControlType::ImageBox;

		Inited = LoadInMemory(Path);
		if (Inited)
			this->_SetNeedUpdate(true);

		return Inited;
	}
	///<summary>
	///从资源加载图片，支持多种格式。在加载新的图片之前，自动释放原来的图片
	///</summary>
	bool LoadFromResource(POINT position, SIZE size, WORD ResourceID, std::wstring TypeName, HMODULE Module = nullptr, bool CanMoveWindow = true, DImageScaleMode ScaleMode = DImageScaleMode::EqualScale, float TotalOpacity = 1.0f, DCOLOR FillColor = { 0, 0, 0, 0.0f }, DCOLOR StrokeColor = { 0, 0, 0, 0.0f }, float radiusX = 0.0f, float radiusY = 0.0f, float StrokeWidth = 0.0f, DMouseStyle MouseStyle = DMouseStyle::BasicMouse)
	{
		this->CanMoveWindow = CanMoveWindow;

		DstRC.rect.left = (float)position.x;
		DstRC.rect.top = (float)position.y;
		DstRC.rect.right = (float)position.x + size.cx;
		DstRC.rect.bottom = (float)position.y + size.cy;
		DstRC.radiusX = radiusX;
		DstRC.radiusY = radiusY;

		ClipRC.left = DstRC.rect.left + StrokeWidth / 2.0f;
		ClipRC.top = DstRC.rect.top + StrokeWidth / 2.0f;
		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		DCOLOR fill, stroke;
		memcpy_s(&fill, sizeof(fill), &FillColor, sizeof(FillColor));
		memcpy_s(&stroke, sizeof(stroke), &StrokeColor, sizeof(StrokeColor));
		DetectAndResetColor(fill);
		DetectAndResetColor(stroke);

		this->FillColor.r = (float)(fill.r / 255.0f);
		this->FillColor.g = (float)(fill.g / 255.0f);
		this->FillColor.b = (float)(fill.b / 255.0f);
		this->FillColor.a = fill.a;

		this->StrokeColor.r = (float)(stroke.r / 255.0f);
		this->StrokeColor.g = (float)(stroke.g / 255.0f);
		this->StrokeColor.b = (float)(stroke.b / 255.0f);
		this->StrokeColor.a = stroke.a;

		this->StrokeWidth = StrokeWidth;

		this->ScaleMode = ScaleMode;
		SetTotalOpacity(TotalOpacity, 0, nullptr, false);

		this->MouseStyle = MouseStyle;

		this->ControlType = DControlType::ImageBox;

		Inited = LoadInMemory(ResourceID, TypeName.c_str(), Module);
		if (Inited)
			this->_SetNeedUpdate(true);

		return Inited;
	}

	void Release()
	{
		if (!this->_Base_Release())//释放基类保存的变量
			return;

		ControlType = DControlType::ImageBox;
		MouseStyle = DMouseStyle::BasicMouse;

		OnClick = false;
		FillColor = { 0.0f };
		StrokeColor = { 0.0f };
		TotalOpacity = 1.0f;
		RELEASE(RoundGeo);
		RELEASE(brush);
		ScaleMode = DImageScaleMode::EqualScale;
		DeleteArray(Data);
		DataSize = 0;
		BmpRC = { 0 };
		RELEASE(Bmp);
		LoadingRC = { 0 };
		//加载成功后，将缩放到合适大小的图像数据保存，以便重建
		DeleteArray(CacheData);
		BufferSize = 0;
		Pitch = 0;


		return;
	}

	void GetSize(UINT* Width, UINT* Height)
	{
		if (Width)
			*Width = (UINT)((BmpRC.right - BmpRC.left) * (float)dpiX / 96.0f);
		if (Height)
			*Height = (UINT)((BmpRC.bottom - BmpRC.top) * (float)dpiX / 96.0f);
		return;
	}
	void SetRadius(float RadiusX, float RadiusY, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_Radius_X.AddAni(DstRC.radiusX, RadiusX, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_Radius_Y.AddAni(DstRC.radiusY, RadiusY, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}

		DstRC.radiusX = RadiusX;
		DstRC.radiusY = RadiusY;

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		//重新创建HitTest
		this->_CreateHitTestBmp();
		this->_DrawHitTestBmp();
		this->_HitTestBmpToMemory();

		if (Parent)
			Parent->_SetNeedUpdate(true, UpdateRC);
		else
			this->_SetNeedUpdate(true);

		return;
	}
	void SetPosition(float x, float y, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_RC_x.AddAni(DstRC.rect.left, x, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_RC_y.AddAni(DstRC.rect.top, y, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}
		D2D1_RECT_F OldUpdateRC = UpdateRC;

		//要先处理right和bottom
		DstRC.rect.right = x + DstRC.rect.right - DstRC.rect.left;
		DstRC.rect.bottom = y + DstRC.rect.bottom - DstRC.rect.top;

		DstRC.rect.left = x;
		DstRC.rect.top = y;

		this->_PreDisposeRC();

		ClipRC.left = DstRC.rect.left + StrokeWidth / 2.0f;
		ClipRC.top = DstRC.rect.top + StrokeWidth / 2.0f;
		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(UpdateRC, OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(UpdateRC, OldUpdateRC);
			RectChanged = true;
		}
		___RcChangedLock.Unlock();

		if (Parent)
		{
			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(true, CalcRectUnion(UpdateRC, OldUpdateRC));
		}
		else
			this->_SetNeedUpdate(true);
		return;
	}
	void SetSize(float width, float height, UINT time = 0, DBezier* Nonlinear = nullptr)
	{
		if (time)
		{
			bool succ = false;

			if (A_RC_width.AddAni(DstRC.rect.right - DstRC.rect.left, width, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (A_RC_height.AddAni(DstRC.rect.bottom - DstRC.rect.top, height, (UINT)(FrameRate * (time / 1000.0f)), Nonlinear))
				succ = true;
			if (succ)
				this->_SetNeedUpdate(true);
			return;
		}
		D2D1_RECT_F OldUpdateRC = UpdateRC;

		DstRC.rect.right = DstRC.rect.left + width;
		DstRC.rect.bottom = DstRC.rect.top + height;

		this->_PreDisposeRC();

		ClipRC.right = DstRC.rect.right - StrokeWidth / 2.0f;
		ClipRC.bottom = DstRC.rect.bottom - StrokeWidth / 2.0f;

		//重新创建HitTest
		this->_CreateHitTestBmp();
		this->_DrawHitTestBmp();
		this->_HitTestBmpToMemory();

		//重新创建圆角剪裁图形
		___RoundGeoLock.Lock();
		RELEASE(RoundGeo);
		___RoundGeoLock.Unlock();

		___RcChangedLock.Lock();
		if (RectChanged)
		{
			LastRect = CalcRectUnion(LastRect, CalcRectUnion(UpdateRC, OldUpdateRC));
			RectChanged = true;
		}
		else {
			LastRect = CalcRectUnion(UpdateRC, OldUpdateRC);
			RectChanged = true;
		}
		___RcChangedLock.Unlock();

		if (Parent)
		{
			___BackImgTargetLock.Lock();
			RELEASE(BackImgTarget);
			___BackImgTargetLock.Unlock();

			//取出变化前和变化后的最大矩形，然后根据此矩形设置Clip范围
			Parent->_SetNeedUpdate(true, CalcRectUnion(UpdateRC, OldUpdateRC));
		}
		else
			this->_SetNeedUpdate(true);
		return;
	}


	bool GaussianBlur(float Sigma, UINT Threads = 4)
	{
		if (!Inited || !CacheData)
			return false;

		___GauLock.Lock();

		___BmpLock.Lock();
		_Gau_Blur(CacheData, (UINT)((BmpRC.right - BmpRC.left) * (float)dpiX / 96.0f), (UINT)((BmpRC.bottom - BmpRC.left) * (float)dpiY / 96.0f), Pitch, Sigma, Threads);

		RELEASE(Bmp);
		___BmpLock.Unlock();

		this->_SetNeedUpdate(true);

		___GauLock.Unlock();
		return true;
	}

private:
	bool OnClick = false;

	DMouseStyle MouseStyle = DMouseStyle::BasicMouse;

	D2D1_COLOR_F FillColor = { 0.0f };
	D2D1_COLOR_F StrokeColor = { 0.0f };

	//用来实现圆角的裁剪区
	ID2D1RoundedRectangleGeometry* RoundGeo = nullptr;
	___DThreadLock ___RoundGeoLock;

	ID2D1SolidColorBrush* brush = nullptr;

	DImageScaleMode ScaleMode = DImageScaleMode::EqualScale;

	BYTE* Data = nullptr;
	ULONG DataSize = 0;

	___DThreadLock ___BmpLock;
	ID2D1Bitmap* Bmp = nullptr;
	D2D1_RECT_F LoadingRC = { 0 };

	//加载成功后，将缩放到合适大小的图像数据保存，以便重建
	BYTE* CacheData = nullptr;
	UINT BufferSize = 0;
	UINT Pitch = 0;
	D2D1_RECT_F BmpRC = { 0 };

	___DThreadLock ___GauLock;

	inline void _SeparateFromWindow()
	{
		A_RC_x.Reset();
		A_RC_y.Reset();
		A_RC_width.Reset();
		A_RC_height.Reset();
		A_Opa.Reset();
		//把所有设备相关资源都释放
		RELEASE(RoundGeo);
		RELEASE(brush);
		RELEASE(BackImgTarget);
		RELEASE(BackImg);
		RELEASE(HitTestBmpTarget);
		RELEASE(HitTestBmp);
		DeleteArray(_HitTest);
		RELEASE(Bmp);
		D2DFactory = nullptr;
		WicFactory = nullptr;
		DWriteFactory = nullptr;
		ID = 0;
		return;
	}

	void _SetLastFrameCount(UINT count)
	{
		this->NowFrameCount = count;
		return;
	}

	bool _SetCursorMsgProc()
	{
		if (MouseStyle == DMouseStyle::HandMouse)
		{
			SetCursor(LoadCursorW(nullptr, IDC_HAND));
			return true;
		}
		else
			return false;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		return false;
	}

	bool _MouseMoveMsgProc(int xPos, int yPos)
	{
		return false;
	}
	bool _LMouseDownMsgProc(int xPos, int yPos)
	{
		return false;
	}
	bool _LMouseUpMsgProc(int xPos, int yPos)
	{
		return false;
	}

	void _DrawHitTestBmp()//绘制命中测试BMP
	{
		if (!HitTestBmpTarget)
			return;

		D2D1_COLOR_F color;
		color.r = 1.0f;
		color.g = 1.0f;
		color.b = 1.0f;
		color.a = 1.0f;

		ID2D1SolidColorBrush* bru;
		HitTestBmpTarget->CreateSolidColorBrush(color, &bru);

		if (bru)
		{
			D2D1_ROUNDED_RECT hittest = { {StrokeWidth / 2.0f + 1.0f, StrokeWidth / 2.0f + 1.0f, DstRC.rect.right - DstRC.rect.left + StrokeWidth / 2.0f, DstRC.rect.bottom - DstRC.rect.top + StrokeWidth / 2.0f}, DstRC.radiusX, DstRC.radiusY };
			HitTestBmpTarget->BeginDraw();
			HitTestBmpTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

			HitTestBmpTarget->FillRoundedRectangle(hittest, bru);
			HitTestBmpTarget->DrawRoundedRectangle(hittest, bru, StrokeWidth);

			HitTestBmpTarget->EndDraw();

			RELEASE(bru);
		}

		return;
	}

	void _DisposeAnimation()
	{
		EnableClip = false;//每一帧都要置为false

		if (A_RC_x || A_RC_y)
		{
			float x = DstRC.rect.left, y = DstRC.rect.top;
			if (A_RC_x)
				x = A_RC_x.FrameCalc();
			if (A_RC_y)
				y = A_RC_y.FrameCalc();

			SetPosition(x, y);
		}
		if (A_RC_width || A_RC_height)
		{
			float wid = DstRC.rect.right - DstRC.rect.left, hei = DstRC.rect.bottom - DstRC.rect.top;
			if (A_RC_width)
				wid = A_RC_width.FrameCalc();
			if (A_RC_height)
				hei = A_RC_height.FrameCalc();

			SetSize(wid, hei);
		}
		if (A_Radius_X || A_Radius_Y)
		{
			float x = DstRC.radiusX, y = DstRC.radiusY;
			if (A_Radius_X)
				x = A_Radius_X.FrameCalc();
			if (A_Radius_Y)
				y = A_Radius_Y.FrameCalc();

			SetRadius(x, y);
		}
		if (A_Opa)
			SetTotalOpacity(A_Opa.FrameCalc());

		return;
	}

	bool _Draw(ID2D1BitmapRenderTarget* DstTarget, D2D1_COLOR_F BackColor)
	{
		if (!DstTarget)
		{
			this->_SetNeedUpdate(true);
			return false;
		}

		if (!brush)
		{
			DstTarget->CreateSolidColorBrush(BackColor, &brush);
			if (!brush)
				return false;
		}
		else
			brush->SetColor(BackColor);

		//如果没有加载到图片，则加载
		___BmpLock.Lock();
		if (Data)
		{
			if (MyLoadBmp(DstTarget, Data, DataSize, &Bmp, &LoadingRC) && Bmp)
				memcpy_s(&BmpRC, sizeof(BmpRC), &LoadingRC, sizeof(BmpRC));

			DeleteArray(Data);
			DataSize = 0;
			LoadingRC = { 0 };
		}

		if (!Bmp && CacheData && Pitch && BufferSize)
		{
			D2D1_PIXEL_FORMAT fm = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
			D2D1_BITMAP_PROPERTIES prop = { fm, (float)dpiX, (float)dpiY };

			D2D1_RECT_U d2drc = { 0, 0, (UINT)((BmpRC.right - BmpRC.left) * (float)dpiX / 96.0f), (UINT)((BmpRC.bottom - BmpRC.top) * (float)dpiY / 96.0f) };

			if (SUCCEEDED(DstTarget->CreateBitmap({ d2drc.right, d2drc.bottom }, prop, &Bmp)) && Bmp)
			{
				Bmp->CopyFromMemory(&d2drc, CacheData, Pitch);
			}
		}

		DstTarget->BeginDraw();

		___UpdateClipLock.Lock();
		if (EnableClip)//设置更新区的裁剪，主要为过渡动画时使用
			DstTarget->PushAxisAlignedClip(UpdateClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		if (Parent)//有底层控件, 要重画backimg
		{
			DstTarget->PushAxisAlignedClip(Parent->_GetClipRC(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//设置父控件的裁剪

			if (!ParentDrawing && BackImg)
			{
				DstTarget->DrawBitmap(BackImg, UpdateRC, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
		}
		else {//无底层控件，直接用窗口背景色填充
			if (EnableClip)
				DstTarget->FillRectangle(UpdateClip, brush);

			___RcChangedLock.Lock();
			if (RectChanged)
				DstTarget->FillRectangle(LastRect, brush);
			else
				DstTarget->FillRectangle(UpdateRC, brush);
			___RcChangedLock.Unlock();
		}

		//如果有圆角，则用图层实现圆角
		ID2D1Layer* RoundLayer = nullptr;

		if (DstRC.radiusX != 0.0f || DstRC.radiusY != 0.0f)
		{
			___RoundGeoLock.Lock();

			if (!RoundGeo && D2DFactory)
				D2DFactory->CreateRoundedRectangleGeometry(DstRC, &RoundGeo);

			if (RoundGeo)
				DstTarget->CreateLayer(nullptr, &RoundLayer);

			if (RoundLayer)
				DstTarget->PushLayer(D2D1::LayerParameters(UpdateRC, RoundGeo), RoundLayer);

			___RoundGeoLock.Unlock();
		}

		//开始绘制
		if (FillColor.a * TotalOpacity != 0.0f)
		{
			brush->SetColor({ FillColor.r, FillColor.g, FillColor.b, FillColor.a * TotalOpacity });//设置为填充颜色
			DstTarget->FillRoundedRectangle(&DstRC, brush);
		}
		//画图片
		if (Bmp && TotalOpacity != 0.0f)
			DstTarget->DrawBitmap(Bmp, BmpRC, TotalOpacity, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

		___BmpLock.Unlock();

		if (StrokeColor.a * TotalOpacity != 0.0f)
		{
			brush->SetColor({ StrokeColor.r, StrokeColor.g, StrokeColor.b, StrokeColor.a * TotalOpacity });//设置为描边颜色
			DstTarget->DrawRoundedRectangle(&DstRC, brush, StrokeWidth);
		}

		if (RoundLayer)
			DstTarget->PopLayer();
		RELEASE(RoundLayer);

		if (Parent)
			DstTarget->PopAxisAlignedClip();

		if (EnableClip)
			DstTarget->PopAxisAlignedClip();
		___UpdateClipLock.Unlock();

		DstTarget->EndDraw();

		_EndDraw_CallChildToUpdateBackImg(DstTarget);

		this->NeedUpdate = false;//因为是ImageBox，所以绘制一次就停止
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
		return Data;
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
		return Data;
	}

	inline bool MyLoadBmp(ID2D1RenderTarget* Target, BYTE* data, ULONG Size, ID2D1Bitmap** bmp, D2D1_RECT_F* bmprc)
	{
		if (!Target || !bmp || !bmprc || !WicFactory)///////使用条件检测
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

		HRESULT hr = WicFactory->CreateDecoderFromStream(
			Stream,
			nullptr,
			WICDecodeMetadataCacheOnDemand,
			&pDecoder);
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
			hr = pFormat->Initialize(
				pFrame,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.0f,
				WICBitmapPaletteTypeCustom);
		}

		if (SUCCEEDED(hr))
		{
			UINT wid, hei;
			hr = pFormat->GetSize(&wid, &hei);
			if (SUCCEEDED(hr))
			{
				//计算目标图片大小
				D2D1_SIZE_F DstSize = { 0 };
				if (this->ScaleMode == DImageScaleMode::EqualScale)
				{
					RECT GoodRc = { 0 };
					GoodRc = RectSizeWithConstantRatio({ 0, 0, (long)(DstRC.rect.right - DstRC.rect.left), (long)(DstRC.rect.bottom - DstRC.rect.top) }, { (long)wid, (long)hei });

					DstSize.width = (float)(GoodRc.right - GoodRc.left);
					DstSize.height = (float)(GoodRc.bottom - GoodRc.top);

					bmprc->left = DstRC.rect.left + (float)GoodRc.left;
					bmprc->top = DstRC.rect.top + (float)GoodRc.top;
					bmprc->right = DstRC.rect.left + (float)GoodRc.right;
					bmprc->bottom = DstRC.rect.top + (float)GoodRc.bottom;
				}
				else if (this->ScaleMode == DImageScaleMode::Fill) {
					DstSize.width = DstRC.rect.right - DstRC.rect.left;
					DstSize.height = DstRC.rect.bottom - DstRC.rect.top;

					memcpy_s(bmprc, sizeof(D2D1_RECT_F), &DstRC.rect, sizeof(DstRC.rect));
				}

				//将WICBMP转到D2D的缓存Bmp中来,不要用CreateBitmapFromWicBitmap(),这样加载某些图片会导致播放过程中明显卡一下
				hr = WicFactory->CreateBitmapScaler(&pScaler);

				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(pFormat,
						(UINT)(DstSize.width * (float)dpiX / 96.0f),
						(UINT)(DstSize.height * (float)dpiY / 96.0f),
						WICBitmapInterpolationModeCubic);

					if (SUCCEEDED(hr))
					{
						D2D1_PIXEL_FORMAT fm = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
						D2D1_BITMAP_PROPERTIES prop = { fm, (float)dpiX, (float)dpiY };

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
									lock->GetStride(&Pitch);
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
											hr = (*bmp)->CopyFromMemory(&d2drc, CacheData, Pitch);
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

#define ShadowStrokeWidth 15
#define Out 10.0f

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

	bool Create(HINSTANCE hInst, HWND MainHwnd, std::wstring ClassName, int DpiX, int DpiY, float Radius, float Alpha)
	{
		if (!hInst || !MainHwnd)
			return false;
		//创建窗口
		radius = Radius;
		alpha = Alpha;
		EdgeOut = (int)(radius * Out * (float)dpiX / 96.0f);
		dpiX = DpiX;
		dpiY = DpiY;
		GetWindowRect(MainHwnd, &MainRC);

		hWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT, ClassName.c_str(), nullptr, WS_POPUP,
			MainRC.left - EdgeOut, MainRC.top - EdgeOut,
			MainRC.right - MainRC.left + EdgeOut * 2, MainRC.bottom - MainRC.top + EdgeOut * 2, MainHwnd, nullptr, hInst, nullptr);

		if (!hWnd)
			return false;

		this->ReDrawMem();

		return true;
	}
	bool Show()
	{
		if (hWnd)
		{
			Update();
			return ShowWindow(hWnd, SW_SHOWNORMAL);
		}
		else return false;
	}
	bool ReDrawMem()
	{
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
			bitmapinfo.bmiHeader.biSizeImage = bitmapinfo.bmiHeader.biWidth * bitmapinfo.bmiHeader.biHeight * bitmapinfo.bmiHeader.biBitCount / 8;

			MemBmp = CreateDIBSection(MemDC, &bitmapinfo, 0, nullptr, nullptr, 0);
			if (MemBmp)
			{
				MemOldBmp = (HBITMAP)SelectObject(MemDC, MemBmp);

				HDC dc = CreateCompatibleDC(MemDC);
				HPEN Pen = CreatePen(PS_SOLID, ShadowStrokeWidth, RGB(0, 0, 0));
				HPEN OldPen = (HPEN)SelectObject(dc, Pen);
				HBRUSH WhiteBru = CreateSolidBrush(RGB(255, 255, 255));
				HBRUSH OldBru = (HBRUSH)SelectObject(dc, WhiteBru);

				UINT leftWid = min(EdgeOut * 2 + ShadowStrokeWidth, EdgeOut + (MainRC.right - MainRC.left) / 2);
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

				Rectangle(dc, EdgeOut + ShadowStrokeWidth / 2, EdgeOut + ShadowStrokeWidth / 2 + 1, leftWid + ShadowStrokeWidth * 2, rc.bottom - EdgeOut - ShadowStrokeWidth / 2);

				//开始获取Left的图像数据
				BYTE* leftBits = new BYTE[lbi.bmiHeader.biSizeImage];
				ZeroMemory(leftBits, lbi.bmiHeader.biSizeImage);
				if (!GetDIBits(dc, Left, 0, lbi.bmiHeader.biHeight, leftBits, &lbi, DIB_RGB_COLORS))
					DeleteArray(leftBits);
				//Left数据获取完毕

				//画Top图像
				int topWid = (MainRC.right - MainRC.left) - leftMainWid * 2;

				BITMAPINFO tbi;
				tbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				tbi.bmiHeader.biBitCount = 32;
				tbi.bmiHeader.biHeight = EdgeOut + ShadowStrokeWidth * 2;
				tbi.bmiHeader.biWidth = topWid;
				tbi.bmiHeader.biPlanes = 1;
				tbi.bmiHeader.biCompression = BI_RGB;
				tbi.bmiHeader.biXPelsPerMeter = 0;
				tbi.bmiHeader.biYPelsPerMeter = 0;
				tbi.bmiHeader.biClrUsed = 0;
				tbi.bmiHeader.biClrImportant = 0;
				tbi.bmiHeader.biSizeImage = tbi.bmiHeader.biWidth * tbi.bmiHeader.biHeight * 4;

				HBITMAP Top = CreateDIBSection(dc, &tbi, 0, nullptr, nullptr, 0);
				SelectObject(dc, Top);

				rect = { 0, 0, topWid, tbi.bmiHeader.biHeight };
				FillRect(dc, &rect, WhiteBru);

				MoveToEx(dc, 0, EdgeOut + ShadowStrokeWidth / 2 + 1, nullptr);
				LineTo(dc, topWid, EdgeOut + ShadowStrokeWidth / 2 + 1);

				//开始获取Top图像数据
				BYTE* topBits = new BYTE[tbi.bmiHeader.biSizeImage];
				ZeroMemory(topBits, tbi.bmiHeader.biSizeImage);
				if (!GetDIBits(dc, Top, 0, tbi.bmiHeader.biHeight, topBits, &tbi, DIB_RGB_COLORS))
					DeleteArray(topBits);

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
				//数据处理完毕

				//开始多线程高斯模糊
				_Gau_Blur(leftBits, leftWid, rc.bottom, leftWid * 4, radius, 2);
				_Gau_Blur(topBits, topWid, tbi.bmiHeader.biHeight, topWid * 4, radius, 2);

				//再把数据读入HBITMAP
				SetDIBits(dc, Left, 0, lbi.bmiHeader.biHeight, leftBits, &lbi, DIB_RGB_COLORS);
				SetDIBits(dc, Top, 0, tbi.bmiHeader.biHeight, topBits, &tbi, DIB_RGB_COLORS);

				//把两部分阴影画到主MemDC上
				//将MemDC颜色和透明度清零
				BYTE* memBits = new BYTE[bitmapinfo.bmiHeader.biSizeImage];
				ZeroMemory(memBits, bitmapinfo.bmiHeader.biSizeImage);

				SetDIBits(MemDC, MemBmp, 0, bitmapinfo.bmiHeader.biHeight, memBits, &bitmapinfo, DIB_RGB_COLORS);

				BLENDFUNCTION blendFunc;
				blendFunc.AlphaFormat = AC_SRC_ALPHA;
				blendFunc.BlendFlags = 0;
				blendFunc.BlendOp = AC_SRC_OVER;
				blendFunc.SourceConstantAlpha = (BYTE)(255 * alpha);
				//Top绘制
				AlphaBlend(MemDC, leftWid, 0, topWid, tbi.bmiHeader.biHeight, dc, 0, 0, topWid, tbi.bmiHeader.biHeight, blendFunc);
				//垂直翻转
				tbi.bmiHeader.biHeight = -tbi.bmiHeader.biHeight;
				SetDIBits(dc, Top, 0, -tbi.bmiHeader.biHeight, topBits, &tbi, DIB_RGB_COLORS);
				//Bottom绘制
				AlphaBlend(MemDC, leftWid, rc.bottom + tbi.bmiHeader.biHeight - 1, topWid, -tbi.bmiHeader.biHeight, dc, 0, 0, topWid, -tbi.bmiHeader.biHeight, blendFunc);

				//Left绘制
				SelectObject(dc, Left);
				AlphaBlend(MemDC, 0, 0, leftWid, rc.bottom, dc, 0, 0, leftWid, rc.bottom, blendFunc);
				//水平翻转
				BYTE* rightBits = new BYTE[lbi.bmiHeader.biSizeImage];
				ZeroMemory(rightBits, lbi.bmiHeader.biSizeImage);

				DWORD nPos, nPos1;
				for (UINT Line = 0; Line < (UINT)lbi.bmiHeader.biHeight; Line++)
				{
					for (UINT x = 0; x < leftWid; x++)
					{
						nPos = Line * leftWid * 4 + x * 4;
						nPos1 = (Line + 1) * leftWid * 4 - x * 4 - 1;

						rightBits[nPos] = leftBits[nPos1 - 3];
						rightBits[nPos + 1] = leftBits[nPos1 - 2];
						rightBits[nPos + 2] = leftBits[nPos1 - 1];
						rightBits[nPos + 3] = leftBits[nPos1];
					}
				}

				SetDIBits(dc, Left, 0, lbi.bmiHeader.biHeight, rightBits, &lbi, DIB_RGB_COLORS);
				//Right绘制
				AlphaBlend(MemDC, leftWid + topWid, 0, leftWid, rc.bottom, dc, 0, 0, leftWid, rc.bottom, blendFunc);

				//将主窗口的区域挖空
				ZeroMemory(memBits, bitmapinfo.bmiHeader.biSizeImage);

				if (!GetDIBits(MemDC, MemBmp, 0, bitmapinfo.bmiHeader.biHeight, memBits, &bitmapinfo, DIB_RGB_COLORS))
					DeleteArray(memBits);

				UINT MainWid = MainRC.right - MainRC.left, MainHei = MainRC.bottom - MainRC.top;
				for (UINT Line = EdgeOut; Line < MainHei + EdgeOut; Line++)
				{
					for (UINT x = EdgeOut; x < EdgeOut + MainWid; x++)
					{
						nPos = Line * bitmapinfo.bmiHeader.biWidth * 4 + x * 4;
						memBits[nPos] = 0;
						memBits[nPos + 1] = 0;
						memBits[nPos + 2] = 0;
						memBits[nPos + 3] = 0;
					}
				}

				SetDIBits(MemDC, MemBmp, 0, bitmapinfo.bmiHeader.biHeight, memBits, &bitmapinfo, DIB_RGB_COLORS);

				//释放资源
				DeleteArray(leftBits);
				DeleteArray(rightBits);
				DeleteArray(topBits);
				DeleteArray(memBits);
				SelectObject(dc, OldBmp);
				DeleteObject(Left);
				DeleteObject(Top);
				DeleteDC(dc);
			}
		}
		return true;
	}
	bool Update()
	{
		if (MemDC && MemBmp)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			BLENDFUNCTION blendFunc;
			blendFunc.AlphaFormat = AC_SRC_ALPHA;
			blendFunc.BlendFlags = 0;
			blendFunc.BlendOp = AC_SRC_OVER;
			blendFunc.SourceConstantAlpha = 255;
			SIZE sz = { rc.right, rc.bottom };
			POINT DestPt = { 0, 0 };
			UpdateLayeredWindow(hWnd, MemDC, nullptr, &sz, MemDC, &DestPt, 0, &blendFunc, ULW_ALPHA);

			return true;
		}
		else
			return false;
	}
	inline HWND GetHwnd()
	{
		return hWnd;
	}

	void Release()
	{
		if (!hWnd)
			return;

		if (MemDC && MemOldBmp)
			SelectObject(MemDC, MemOldBmp);
		if (MemDC)
			DeleteDC(MemDC);
		if (MemBmp)
			DeleteObject(MemBmp);

		DestroyWindow(hWnd);

		hWnd = nullptr;
		MainRC = { 0 };
		dpiX = 96;
		dpiY = 96;
		radius = 0.0f;
		alpha = 0.0f;
		EdgeOut = 0;
		MemDC = nullptr;
		MemBmp = nullptr;
		MemOldBmp = nullptr;

		return;
	}
	void MainMsgProc(HWND MainHwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_MOVE:
		{
			GetWindowRect(MainHwnd, &MainRC);
			MoveWindow(hWnd, MainRC.left - EdgeOut, MainRC.top - EdgeOut,
				MainRC.right - MainRC.left + EdgeOut * 2, MainRC.bottom - MainRC.top + EdgeOut * 2, true);

			break;
		}
		case WM_SIZE:
		{
			if (wParam == SIZE_MAXIMIZED || wParam == SIZE_MINIMIZED)
				ShowWindow(hWnd, SW_HIDE);
			else if (wParam == SIZE_RESTORED)
			{
				GetWindowRect(MainHwnd, &MainRC);
				MoveWindow(hWnd, MainRC.left - EdgeOut, MainRC.top - EdgeOut,
					MainRC.right - MainRC.left + EdgeOut * 2, MainRC.bottom - MainRC.top + EdgeOut * 2, true);

				ReDrawMem();
				Update();
				Show();
			}
			break;
		}
		case WM_DESTROY:
		{
			this->Release();
			break;
		}
		case WM_SHOWWINDOW:
		{
			Show();
			break;
		}
		default:
		{
			break;
		}
		}
		return;
	}
private:
	HWND hWnd = nullptr;
	RECT MainRC = { 0 };
	int dpiX = 96;
	int dpiY = 96;
	float radius = 0.0f;
	float alpha = 1.0f;
	UINT EdgeOut = 0;

	HDC MemDC = nullptr;
	HBITMAP MemBmp = nullptr;
	HBITMAP MemOldBmp = nullptr;

};




class DWindow
{
public:
	DWORD ExStyle = 0;
	DWORD Style = WS_POPUP;
	HICON hIcon = nullptr;
	HICON hIconSmall = nullptr;
	HWND Parent = nullptr;

	~DWindow()
	{
		this->Release();
		return;
	}

	//定义隐式转换，此类可被看作为一个窗口句柄
	operator HWND()
	{
		return this->hWnd;
	}

	bool Create(HINSTANCE hInst, std::wstring ClassName, std::wstring Title, WNDPROC WndProc, int x, int y, int width, int height)
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
		//获取系统DPI
		dpiX = GetDeviceCaps(GetDC(nullptr), LOGPIXELSX);
		if (dpiX <= 0)
			dpiX = 96;
		dpiY = GetDeviceCaps(GetDC(nullptr), LOGPIXELSY);
		if (dpiY <= 0)
			dpiY = 96;

		//****由于DXUI库全面支持高DPI场景，所以在使用前 应在项目设置里设置高DPI支持，来防止系统缩放 导致模糊****

	//这里的窗口大小要根据DPI的大小来改变，所以用了一套公式重新计算大小
	//***切记，一定不要把与窗口大小相关的几个变量(如dpi)设为unsigned，不然当屏幕较小时，减出来的left和top有可能是负数，而C++乘除时直接自动转换为被乘除变量的unsigned，这样就超出范围了，导致窗口坐标计算错误

		this->hWnd = CreateWindowExW(ExStyle, ClassName.c_str(), Title.c_str(), Style,
			x * dpiX / 96, y * dpiY / 96,
			width * dpiX / 96, height * dpiY / 96, Parent, nullptr, hInst, nullptr);

		if (!hWnd)
			return false;

		return true;
	}

	bool Show()
	{
		if (Inited && hWnd)
		{
			return ShowWindow(this->hWnd, SW_SHOWNORMAL);
		}
		return false;
	}

	bool Init(DMsgProc Proc, float ShadowRadius = 4.0f, float ShadowAlpha = 0.8f, UINT FrameRate = DefFrameRate, DCOLOR Background = { 255, 255, 255, 1.0f })
	{
		return Init(this->hWnd, Proc, ShadowRadius, ShadowAlpha, FrameRate, Background);
	}
	bool Init(HWND hWnd, DMsgProc Proc, float ShadowRadius = 4.0f, float ShadowAlpha = 0.8f, UINT FrameRate = DefFrameRate, DCOLOR Background = { 255, 255, 255, 1.0f })
	{
		if (!hWnd || !Proc)/////////条件检测
			return false;

		proc = Proc;
		this->FrameRate = max(1, min(60, FrameRate));

		BackColor.r = Background.r / 255.0f;
		BackColor.g = Background.g / 255.0f;
		BackColor.b = Background.b / 255.0f;
		BackColor.a = Background.a;

		this->hWnd = hWnd;
		GetClientRect(hWnd, &rc);

		HRESULT hr;

#ifdef _DEBUG//如果为Debug模式，则启用D2D调试层

		//下面为设置启用调试层，会在IDE输出窗口内显示
		D2D1_FACTORY_OPTIONS options;
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, &D2DFactory);//多线程兼容模式

#else
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &D2DFactory);//多线程兼容模式
#endif

		if (FAILED(hr))
		{
			this->Release();
			return false;
		}
		//获取系统DPI
		dpiX = GetDeviceCaps(GetDC(nullptr), LOGPIXELSX);
		if (dpiX <= 0)
			dpiX = 96;
		dpiY = GetDeviceCaps(GetDC(nullptr), LOGPIXELSY);
		if (dpiY <= 0)
			dpiY = 96;

		//创建D2D窗口渲染目标
		D2D1_RENDER_TARGET_PROPERTIES rtDesc = D2D1::RenderTargetProperties();
		rtDesc.dpiX = (float)dpiX;
		rtDesc.dpiY = (float)dpiY;
		rtDesc.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };

		hr = D2DFactory->CreateHwndRenderTarget(
			rtDesc,
			D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
			&RenderTarget);

		if (FAILED(hr))
		{
			this->Release();
			return false;
		}

		if (!Inited)
		{
			if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
			{
				this->Release();
				return false;
			};
		}

		//创建Dwrite库
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&DWFactory));
		if (FAILED(hr))
		{
			this->Release();
			return false;
		}
		//获取渲染参数(启用ClearType)
		DWFactory->CreateRenderingParams(&RenderParam);

		//创建WIC图像库。因为版本2不支持Win7，所以失败后继续测试版本1
		hr = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&WicFactory));

		if (FAILED(hr))
		{
			hr = CoCreateInstance(CLSID_WICImagingFactory1, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&WicFactory));
			if (FAILED(hr))
			{
				this->Release();
				return false;
			}
		}

		//创建内存的缓存Target
		hr = RenderTarget->CreateCompatibleRenderTarget(&CacheBmpTarget);

		if (FAILED(hr))
		{
			this->Release();
			return false;
		}

		if (RenderParam)
			CacheBmpTarget->SetTextRenderingParams(RenderParam);

		//缓存渲染目标的底色置为用户设置的背景色
		CacheBmpTarget->BeginDraw();
		CacheBmpTarget->Clear(BackColor);
		CacheBmpTarget->EndDraw();

		//创建窗口边缘阴影
		if (ShadowRadius > 0.0f)
			Shadow.Create(hInst, hWnd, ClassName.c_str(), dpiX, dpiY, ShadowRadius, ShadowAlpha);


		AllUpdate = true;//全屏更新一下

		Inited = true;

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

		//将子控件全部分离，因为停止渲染线程了，所以可以先分离后删除控件。否则必须注意顺序
		if (!Controls.empty())
		{
			___ConLock.Lock();
			std::map<UINT, DControl*>::iterator con = Controls.begin();
			for (UINT i = 0; i < Controls.size(); i++, con++)
			{
				con->second->_SeparateFromWindow();
			}
			Controls.clear();
			___ConLock.Unlock();
		}

		hInst = nullptr;
		rc = { 0 };
		dpiX = 0;
		dpiY = 0;
		proc = nullptr;
		TrackLeave = false;

		FrameRate = DefFrameRate;//帧率
		AllUpdate = false;
		//保存当前活动的控件
		ActiveCon.clear();
		//背景颜色
		BackColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		RELEASE(CacheBmpTarget);
		RELEASE(WicFactory);
		CoUninitialize();
		RELEASE(RenderTarget);//Render target
		RELEASE(RenderParam);
		RELEASE(DWFactory);//Dwrite Factory
		RELEASE(D2DFactory);//Direct2D Factory
		return;
	}
	inline void DeleteControl(UINT ControlID)
	{
		if (!Controls.empty())
		{
			std::map<UINT, DControl*>::iterator conit = Controls.find(ControlID);
			if (conit != Controls.end())
			{
				DControl* con = conit->second;

				DControl* parent = con->_GetParentCon();
				if (parent)
					parent->_DeleteChildCon(con);

				std::map<UINT, DControl*> child = con->_GetChildCon();
				if (!child.empty())
				{
					std::map<UINT, DControl*>::iterator chi = child.begin();
					for (UINT i = 0; i < child.size(); i++, chi++)
					{
						chi->second->_SetParentCon(parent);//将 欲删除的控件 的 Child 的 父控件 设为 欲删除控件的父控件
					}
				}

				con->_ClearChildCon();

				___ConLock.Lock();
				Controls.erase(conit);
				___ConLock.Unlock();

				//由于删除控件时，可能正在渲染，所以必须先从控件列表中删除，然后再调用控件的分离函数
				con->_SeparateFromWindow();
			}
		}
		return;
	}

	inline bool IsInit()
	{
		return Inited;
	}

	inline HWND GetHwnd()
	{
		return hWnd;
	}
	inline int GetDpiX()
	{
		return GetDeviceCaps(GetDC(nullptr), LOGPIXELSX);
	}
	inline int GetDpiY()
	{
		return GetDeviceCaps(GetDC(nullptr), LOGPIXELSY);
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

	inline bool _RenderFrame(bool Lock = true)
	{
		if (!RenderTarget || !CacheBmpTarget)
			return false;

		if (Lock)
			___RenderMutex.Lock();//互斥体开启

		ID2D1Bitmap* bmp = nullptr;
		bool res = false;

		___ConLock.Lock();
		if (Controls.empty())
		{
			___ConLock.Unlock();
			if (AllUpdate)
			{
				CacheBmpTarget->BeginDraw();
				CacheBmpTarget->Clear(BackColor);
				CacheBmpTarget->EndDraw();
			}
		}
		else {

			std::map<UINT, DControl*>::iterator con = Controls.begin();
			size_t AllCount = Controls.size();

			std::map<UINT, D2D1_RECT_F> UpdateRects;

			for (size_t i = 0; i < AllCount; i++, con++)
			{
				if (con->second && con->second->_GetNeedUpdate())
				{
					con->second->_Draw(CacheBmpTarget, BackColor);//将当前控件画到缓存渲染目标上

					if (!con->second->EnableClip)
					{
						if (UpdateRects.empty())
						{
							UpdateRects.insert(std::pair<UINT, D2D1_RECT_F>(con->second->GetID(), con->second->_GetUpdateRect()));
						}
						else {
							std::map<UINT, D2D1_RECT_F>::iterator it;
							it = UpdateRects.begin();
							D2D1_RECT_F ur = con->second->_GetUpdateRect();//先把Rect取出来
							while (it != UpdateRects.end())
							{
								if (!DetectRectContain(it->second, ur))
								{
									//如果部分不包含在刷新区域内，则加入
									UpdateRects.insert(std::pair<UINT, D2D1_RECT_F>(con->second->GetID(), ur));
								}
								it++;
							}
						}
					}
					//这个时候主动调用控件来处理动画数据
					con->second->_DisposeAnimation();
				}//NeedUpdate End

			}//for End
			___ConLock.Unlock();

			if (!AllUpdate && !UpdateRects.empty())
			{
				//取出位图，绘制。如果全部更新则在最后一起画到屏幕
				CacheBmpTarget->GetBitmap(&bmp);

				if (bmp)
				{
					D2D1_RECT_F DstRect;

					RenderTarget->BeginDraw();

					std::map<UINT, D2D1_RECT_F>::iterator uprc = UpdateRects.begin();
					for (UINT i = 0; i < UpdateRects.size(); i++, uprc++)
					{
						DstRect.left = std::fmaxf(uprc->second.left, (float)rc.left * 96.0f / (float)dpiX);
						DstRect.top = std::fmaxf(uprc->second.top, (float)rc.top * 96.0f / (float)dpiY);
						DstRect.right = std::fminf(uprc->second.right, (float)rc.right * 96.0f / (float)dpiX);
						DstRect.bottom = std::fminf(uprc->second.bottom, (float)rc.bottom * 96.0f / (float)dpiY);

						RenderTarget->DrawBitmap(bmp, DstRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, DstRect);

						if (Dbg_ShowUpdate)
						{
							ID2D1SolidColorBrush* b;
							RenderTarget->CreateSolidColorBrush({ 1.0f,0.0f,0.0f,1.0f }, &b);
							if (b)
							{
								RenderTarget->DrawRectangle(DstRect, b, 1.0f);
								b->Release();
							}
						}
					}

					if (SUCCEEDED(RenderTarget->EndDraw()))
						res = true;
					RELEASE(bmp);
				}

			}

		}//if End

		//全部需要更新的控件绘制结束后，如果需要全屏更新，则一起更新
		if (AllUpdate)
		{
			CacheBmpTarget->GetBitmap(&bmp);

			if (bmp)
			{
				D2D1_SIZE_F size = CacheBmpTarget->GetSize();
				RenderTarget->BeginDraw();
				RenderTarget->DrawBitmap(bmp, { 0.0f, 0.0f, size.width, size.height }, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

				if (Dbg_ShowUpdate)
				{
					ID2D1SolidColorBrush* b;
					RenderTarget->CreateSolidColorBrush({ 1.0f,0.0f,0.0f,1.0f }, &b);
					if (b)
					{
						RenderTarget->DrawRectangle({ 0.0f, 0.0f, size.width, size.height }, b, 1.0f);
						b->Release();
					}
				}

				if (SUCCEEDED(RenderTarget->EndDraw()))
					res = true;
				RELEASE(bmp);
			}

			AllUpdate = false;
		}

		if (Lock)
			___RenderMutex.Unlock();//解除互斥体

		return res;
	}

	void Resize(WPARAM wParam, LPARAM lParam)
	{
		___RenderMutex.Lock();//开启互斥

		rc.right = rc.left + GET_X(lParam);
		rc.bottom = rc.top + GET_Y(lParam);
		if (rc.right && rc.bottom)
		{
			//修改窗口Target大小
			RenderTarget->Resize({ (UINT)(rc.right - rc.left), (UINT)(rc.bottom - rc.top) });

			//普通Target不能resize，只能重新创建
			//创建内存的缓存Target
			RELEASE(CacheBmpTarget);
			RenderTarget->CreateCompatibleRenderTarget(&CacheBmpTarget);
			if (!CacheBmpTarget)
				return;

			if (RenderParam)
				CacheBmpTarget->SetTextRenderingParams(RenderParam);

			//缓存渲染目标的底色置为用户设置的背景色
			CacheBmpTarget->BeginDraw();
			CacheBmpTarget->Clear(BackColor);
			CacheBmpTarget->EndDraw();

			//因为CacheTarget重新创建，所以所有控件都重绘
			___ConLock.Lock();
			if (!Controls.empty())
			{
				std::map<UINT, DControl*>::iterator con = Controls.begin();

				for (UINT i = 0; i < Controls.size(); i++, con++)
				{
					con->second->_SetNeedUpdate(true);
				}
			}
			___ConLock.Unlock();

			proc(hWnd, 0, DControlType::Null, DControlMsg::Window_Size, rc.right - rc.left, rc.bottom - rc.top);
			AllUpdate = true;//整体刷新一下
			_RenderFrame(false);
		}

		___RenderMutex.Unlock();//解除互斥
		return;
	}

	inline UINT GetFrameRate()
	{
		return FrameRate;
	}
	inline UINT GetFrameDealy()
	{
		return 1000 / FrameRate;
	}


	//DXUI处理Win消息的函数。返回值为当前活动的控件ID，没有活动控件返回0
	LRESULT DXUIMessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		DControlMsg Msg = DControlMsg::Null;
		LRESULT ReturnValue = -1;

		if (hWnd == this->hWnd && !Controls.empty())
		{
			if (Shadow)
				Shadow.MainMsgProc(hWnd, message, wParam, lParam);

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

				___ConLock.Lock();
				std::map<UINT, DControl*>::iterator con = Main_HitTest(xPos, yPos);//命中测试
				if (con != Controls.end())
				{
					RestoreLastActiveControl(con->second->GetID(), message);

					UINT conID = con->second->ID;
					DControlType type = con->second->ControlType;

					if (con->second->_MouseMoveMsgProc(xPos, yPos))
					{
						___ActConLock.Lock();
						ActiveCon.insert(std::pair<UINT, DControl*>(con->second->GetID(), con->second));
						___ActConLock.Unlock();
					}
					___ConLock.Unlock();

					//通知给用户
					proc(hWnd, conID, type, DControlMsg::Control_MouseMove, lParam, 0);
				}
				else {
					___ConLock.Unlock();
					RestoreLastActiveControl(0, message);
				}

				//如果此控件设置了可以移动主窗口，那就手动移动
				if (OnMove)
				{
					RECT NowRC;
					if (GetWindowRect(hWnd, &NowRC))
					{
						SetWindowPos(hWnd, nullptr, NowRC.left + xPos - ClickPos.x, NowRC.top + yPos - ClickPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW);
					}
				}

				break;
			}
			case WM_SETCURSOR:
			{
				POINT pos;
				GetCursorPos(&pos);//获取鼠标指针位置（屏幕坐标）
				ScreenToClient(hWnd, &pos);//将鼠标指针位置转换为窗口坐标

				___ConLock.Lock();
				std::map<UINT, DControl*>::iterator con = Main_HitTest((int)pos.x, (int)pos.y);//命中测试
				if (con != Controls.end())
				{
					if (con->second->_SetCursorMsgProc())
						ReturnValue = 0;
				}
				___ConLock.Unlock();

				break;
			}
			case WM_LBUTTONDOWN:
			{
				int xPos = GET_X(lParam);
				int yPos = GET_Y(lParam);

				___ConLock.Lock();
				std::map<UINT, DControl*>::iterator con = Main_HitTest(xPos, yPos);//命中测试
				if (con != Controls.end())
				{
					RestoreLastActiveControl(con->second->GetID(), message);

					UINT conID = con->second->ID;
					DControlType type = con->second->ControlType;

					con->second->_LMouseDownMsgProc(xPos, yPos);

					con->second->_ImeComposition(hWnd, WM_IME_STARTCOMPOSITION, wParam, lParam);

					___ConLock.Unlock();

					//通知给用户
					proc(hWnd, conID, type, DControlMsg::Control_LButtonDown, lParam, 0);
				}
				else
				{
					___ConLock.Unlock();
					RestoreLastActiveControl(0, message);
				}

				//将鼠标按下时的位置记录下来
				if ((con != Controls.end() && con->second->CanMoveWindow) || con == Controls.end())
				{
					OnMove = true;
					ClickPos.x = xPos;
					ClickPos.y = yPos;
					SetCapture(hWnd);
				}

				break;
			}
			case WM_LBUTTONUP:
			{
				int xPos = GET_X(lParam);
				int yPos = GET_Y(lParam);

				if (OnMove)
				{
					OnMove = false;
					ReleaseCapture();
				}

				___ConLock.Lock();
				std::map<UINT, DControl*>::iterator con = Main_HitTest(xPos, yPos);//命中测试
				if (con != Controls.end())
				{
					RestoreLastActiveControl(con->second->GetID(), message);

					UINT conID = con->second->ID;
					DControlType type = con->second->ControlType;

					con->second->_LMouseUpMsgProc(xPos, yPos);

					___ConLock.Unlock();

					//通知给用户
					proc(hWnd, conID, type, DControlMsg::Control_LButtonUp, lParam, 0);
				}
				else
				{
					___ConLock.Unlock();
					RestoreLastActiveControl(0, message);
				}

				break;
			}
			case WM_KEYDOWN:
			{
				___ActConLock.Lock();
				if (!ActiveCon.empty())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.begin();

					while (con != ActiveCon.end())
					{
						if (con->second->_KeyDownMsg(wParam))
						{
							ReturnValue = 0;
							break;
						}
						con++;
					}
				}
				___ActConLock.Unlock();
				break;
			}
			case WM_IME_STARTCOMPOSITION:
			{
				___ActConLock.Lock();
				if (!ActiveCon.empty())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.begin();

					while (con != ActiveCon.end())
					{
						if (con->second->_ImeComposition(hWnd, message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
						con++;
					}
				}
				___ActConLock.Unlock();

				break;
			}
			case WM_IME_REQUEST:
			{
				___ActConLock.Lock();
				if (!ActiveCon.empty())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.begin();

					while (con != ActiveCon.end())
					{
						if (con->second->_ImeComposition(hWnd, message, wParam, lParam))
						{
							ReturnValue = 1;
							break;
						}
						con++;
					}
				}
				___ActConLock.Unlock();

				break;
			}
			case WM_IME_CHAR:
			{
				___ActConLock.Lock();

				if (!ActiveCon.empty())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.begin();

					while (con != ActiveCon.end())
					{
						if (con->second->_CharMsg(message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
						con++;
					}
				}
				___ActConLock.Unlock();

				break;
			}
			case WM_CHAR://与WM_IME_CHAR完全一样，因为_CharMsg对两种字符消息都接收
			{
				___ActConLock.Lock();

				if (!ActiveCon.empty())
				{
					std::map<UINT, DControl*>::iterator con = ActiveCon.begin();

					while (con != ActiveCon.end())
					{
						if (con->second->_CharMsg(message, wParam, lParam))
						{
							ReturnValue = 0;
							break;
						}
						con++;
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

				UINT conID = 0;
				DControlType type;

				___ConLock.Lock();

				std::map<UINT, DControl*>::iterator con = Main_HitTest((int)p.x, (int)p.y);//命中测试
				if (con != Controls.end())
				{
					if (con->second->_MouseWheelMsgProc(wParam))
					{
						ReturnValue = 1;

						conID = con->second->ID;
						type = con->second->ControlType;
					}
				}
				___ConLock.Unlock();

				if (conID)
				{
					//通知给用户
					proc(hWnd, conID, type, DControlMsg::TextBox_Scrolling, lParam, 0);
				}

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
					GetClientRect(hWnd, &rc);
					AllUpdate = true;
					ReturnValue = 0;
				}
				break;
			}
			default:

				break;
			}
		}
		return ReturnValue;
	}


	bool AddControl(DControl* control, UINT ParentID = 0)
	{
		if (!Inited || !control || !control->IsInit())
			return false;

		control->_SetNeedUpdate(true);//设为true，加入到控件列表后即绘制

		UINT ID;

		___ConLock.Lock();
		if (Controls.empty())
			ID = 1;
		else {
			std::map<UINT, DControl*>::iterator end = Controls.end();
			end--;
			ID = end->first + 1;
		}
		___ConLock.Unlock();
		control->_SetID(ID);//为控件设置ID，以便于管理多个控件

		//给控件传递所有者窗口和回调信息
		control->_SetMsgBack(hWnd, proc);
		//给控件传递Factory
		control->_SetFactory(D2DFactory, WicFactory, DWFactory);
		//传递主窗口的dpi，此函数会设置UpdateRC
		control->_SetDpi(dpiX, dpiY);
		//传递帧率给控件
		control->_SetFrameRate(FrameRate);
		//寻找父控件，并处理
		if (ParentID)
		{
			___ConLock.Lock();
			std::map<UINT, DControl*>::iterator parent = Controls.find(ParentID);
			if (parent != Controls.end())
			{
				parent->second->_AddChildCon(control);
				control->_SetParentCon(parent->second);
				parent->second->_SetNeedUpdate(true);//将父控件设为要刷新，以更新子控件的BackImg
			}
			___ConLock.Unlock();
		}

		//绘制命中测试BMP
		control->_CreateHitTestBmp();
		control->_DrawHitTestBmp();
		control->_HitTestBmpToMemory();
		//重置控件状态
		control->_SetNormalState(this->hWnd, 0);
		control->_SetLastFrameCount(0);//不让控件播放恢复到正常状态时的动画
		//最后插入到控件列表
		___ConLock.Lock();
		Controls.insert(std::pair<UINT, DControl*>(ID, control));
		___ConLock.Unlock();

		return true;
	}


	inline void Dbg_ShowUpdateRect(bool show)
	{
		Dbg_ShowUpdate = show;
		return;
	}
private:
	bool Dbg_ShowUpdate = false;


	bool Inited = false;

	std::wstring ClassName;
	std::wstring Title;

	HINSTANCE hInst = nullptr;
	HWND hWnd = nullptr;
	RECT rc = { 0 };
	int dpiX = 0;
	int dpiY = 0;

	DMsgProc proc = nullptr;

	__DWinShadow Shadow;

	bool TrackLeave = false;

	bool OnMove = false;//鼠标是否正在被按住
	POINT ClickPos = { 0 };//鼠标按住时记录下位置，以便鼠标拖动时移动窗口

	___DThreadLock ___RenderMutex;

	//渲染线程的控制变量
	bool RenderThreadAlive = false;
	bool ExitRenderThreadFlag = false;

	ID2D1Factory* D2DFactory = nullptr;//Direct2D Factory
	IDWriteFactory* DWFactory = nullptr;//Dwrite Factory

	ID2D1HwndRenderTarget* RenderTarget = nullptr;//Render target
	IWICImagingFactory* WicFactory = nullptr;

	UINT FrameRate = DefFrameRate;//帧率

	bool AllUpdate = false;
	//保存所有控件的列表
	std::map<UINT, DControl*> Controls;
	___DThreadLock ___ConLock;

	//保存当前活动的控件
	std::map<UINT, DControl*> ActiveCon;//可能存在文本框（Edit状态）和按钮（MouseMove状态）同时活动的情况，所以用map
	___DThreadLock ___ActConLock;

	//背景颜色
	D2D1_COLOR_F BackColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	ID2D1BitmapRenderTarget* CacheBmpTarget = nullptr;

	IDWriteRenderingParams* RenderParam = nullptr;

	inline bool DetectRectContain(D2D1_RECT_F Owner, D2D1_RECT_F Contained)
	{
		if (std::fmaxf(Owner.left, Contained.left) == Contained.left && std::fmaxf(Owner.top, Contained.top) == Contained.top && std::fminf(Owner.right, Contained.right) == Contained.right && std::fminf(Owner.bottom, Contained.bottom) == Contained.bottom)
			return true;
		else
			return false;
	}
	inline bool DetectRectOverlap(D2D1_RECT_F rc1, D2D1_RECT_F rc2)
	{
		if (std::fmaxf(rc1.left, rc2.left) >= std::fminf(rc1.right, rc2.right))
			return false;
		if (std::fmaxf(rc1.top, rc2.top) >= std::fminf(rc1.bottom, rc2.bottom))
			return false;
		return true;
	}

	//要在ConLock锁里调用才能保证线程安全
	std::map<UINT, DControl*>::iterator Main_HitTest(int xPos, int yPos)
	{
		if (!Controls.empty())
		{
			std::map<UINT, DControl*>::iterator con = Controls.end();
			con--;
			for (UINT i = Controls.size(); i != 0; i--, con--)
			{
				if (con->second->HitTest(xPos, yPos))
					return con;
			}
		}
		return Controls.end();
	}

	inline void RestoreLastActiveControl(UINT NowControlID, UINT Msg)
	{
		___ActConLock.Lock();
		if (!ActiveCon.empty())
		{
			std::map<UINT, DControl*>::iterator con = ActiveCon.begin();

			while (con != ActiveCon.end())
			{
				if (con->second->_SetNormalState(hWnd, Msg, NowControlID))
				{
					std::map<UINT, DControl*>::iterator conNext = con;
					conNext++;

					ActiveCon.erase(con);
					con = conNext;
				}
				else {
					con++;
				}
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

		for (int y = 0; y < height; y++) {
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
		for (int y = HeightSubOne; y >= 0; y--) {
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

		for (int y = 0; y < height; y++) {
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
		for (int y = HeightSubOne; y >= 0; y--) {
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
		for (int y = 0; y < height; y++) {
			prevOut = ((lpRowInitial[0] * a0a1) - (prevOut * b1b2));
			bufferPerLine[0] = (unsigned char)prevOut;
			bufferPerLine += Channels;
			lpRowInitial += Channels;
		}
		lpRowInitial -= Channels;
		bufferPerLine -= Channels;
		lpColInitial += WidthStep * HeightSubOne;
		prevOut = (lpRowInitial[0] * cnext);
		for (int y = HeightSubOne; y >= 0; y--) {
			prevOut = ((lpRowInitial[0] * a2a3) - (prevOut * b1b2));
			bufferPerLine[0] += (unsigned char)prevOut;
			lpColInitial[0] = bufferPerLine[0];
			lpRowInitial -= Channels;
			lpColInitial -= WidthStep;
			bufferPerLine -= Channels;
		}
	}
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
	{
		if (tempData)
			free(tempData);
		return;
	}
	//开始多线程计算
	UINT threads = min(min(64, max(1, Threads)), min(Width, Height));//如果宽或高大于线程数，则设为最小值
	UINT nT = 0;
	UINT FinishStep1 = threads;

	UINT deltax = Width / threads;
	UINT deltay = Height / threads;
	UINT LastWid = 0, LastHei = 0;

	_GaussianThreadParam* p = new _GaussianThreadParam[threads];
	for (UINT i = 0; i < threads; i++)
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

	unsigned char* bufferPerLine = (unsigned char*)malloc((p->FullWidth > p->FullHeight ? p->FullWidth : p->FullHeight)* p->Channels);
	if (!bufferPerLine)
	{
		if (bufferPerLine)
			free(bufferPerLine);
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
		DWindow* ui = (DWindow*)param;

		ui->_SetRenderThreadAlive(true);//将线程存活状态设置为true

		DWORD LastFrameTime = timeGetTime();

		DWORD timePhase = 0;
		UINT Delay;

		bool succ = timeBeginPeriod(1);
		while (!ui->_GetExitRenderThreadFlag())
		{
			Delay = ui->GetFrameDealy();

			if (Delay == 16)
			{
				if (!ui->_RenderFrame())//渲染
				{
					timePhase = timeGetTime() - LastFrameTime;
					if (timePhase < Delay)//实际耗费的时间<每帧的时间
					{
						Sleep(DWORD(Delay - timePhase));//将剩余的时间等待
					}
					LastFrameTime = timeGetTime();//在这里调用可以将循环耗费的时间也算进去
				}
			}
			else {
				ui->_RenderFrame();

				timePhase = timeGetTime() - LastFrameTime;
				if (timePhase < Delay)//实际耗费的时间<每帧的时间
				{
					Sleep(DWORD(Delay - timePhase));//将剩余的时间等待
				}
				LastFrameTime = timeGetTime();//在这里调用可以将循环耗费的时间也算进去
			}
		}
		if (succ)
			timeEndPeriod(1);

		ui->_SetRenderThreadAlive(false);
	}
	return;
}

