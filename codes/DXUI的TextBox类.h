

class DTextBox : public DControl
{
public:

	//*********************用户一般会使用到的函数**********************

	bool Init(POINT position, SIZE size, DTextBoxTypeStyle Normal, DTextBoxTypeStyle MouseMove, DTextBoxTypeStyle Edit, DTextBoxTypeStyle Disable, float radiusX, float radiusY, float StrokeWidth, std::wstring FamliyName, float FontSize, bool AutoWarp = true, bool MultiLine = true, DFontWeight FontWeight = NormalWeight, DAlignment X_Alignment = Near, DAlignment Y_Alignment = Near, DCOLOR SelectFillColor = { 0, 0, 0, 0.45f })
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
		Insert_PixelPos.left = TextOutputRC.left * dpiX / 96.0f;
		Insert_PixelPos.top = TextOutputRC.top * dpiY / 96.0f;
		Insert_PixelPos.right = Insert_PixelPos.left + DTextBox_CaretWidth;
		Insert_PixelPos.bottom = Insert_PixelPos.top * dpiY / 96.0f;

		memcpy_s(&NormalType, sizeof(NormalType), &Normal, sizeof(Normal));
		memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &MouseMove, sizeof(MouseMove));
		memcpy_s(&EditType, sizeof(EditType), &Edit, sizeof(Edit));
		memcpy_s(&DisableType, sizeof(DisableType), &Disable, sizeof(Disable));

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
		//初始化当前样式
		NowType.FillColor.r = (float)NormalType.FillColor.r;
		NowType.FillColor.g = (float)NormalType.FillColor.g;
		NowType.FillColor.b = (float)NormalType.FillColor.b;
		NowType.FillColor.a = (float)NormalType.FillColor.a;
		NowType.StrokeColor.r = (float)NormalType.StrokeColor.r;
		NowType.StrokeColor.g = (float)NormalType.StrokeColor.g;
		NowType.StrokeColor.b = (float)NormalType.StrokeColor.b;
		NowType.StrokeColor.a = (float)NormalType.StrokeColor.a;
		NowType.TextColor.r = (float)NormalType.TextColor.r;
		NowType.TextColor.g = (float)NormalType.TextColor.g;
		NowType.TextColor.b = (float)NormalType.TextColor.b;
		NowType.TextColor.a = (float)NormalType.TextColor.a;

		this->StrokeWidth = StrokeWidth;

		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		this->MultiLine = MultiLine;
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
		case NormalWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case LightWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case BoldWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;

		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		switch (X_Alignment)
		{
		case Near:
			xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		case Center:
			xAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			break;
		case Far:
			xAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
			break;

		default:
			xAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		}
		switch (Y_Alignment)
		{
		case Near:
			yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		case Center:
			yAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			break;
		case Far:
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

		this->ControlType = TextBox;

		Inited = true;

		return true;
	}
	void Release()
	{
		this->_Base_Release();//释放基类保存的变量
		if (!Inited)
			return;

		ControlType = TextBox;

		OnClick = false;
		HaveSetNormal = false;
		State = TB_Normal;
		LastState = TB_Normal;
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

	inline void _SeparateFromWindow()
	{
		//把所有设备相关资源都释放
		RELEASE(brush);
		RELEASE(SelfTextClip);
		DeleteArray(Select_Rects);
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

	void SetPosition(int x, int y)
	{
		//要先处理right和bottom
		DstRC.rect.right = (float)x + DstRC.rect.right - DstRC.rect.left;
		DstRC.rect.bottom = (float)y + DstRC.rect.bottom - DstRC.rect.top;

		DstRC.rect.left = (float)x;
		DstRC.rect.top = (float)y;

		memcpy_s(&LastUpdateRC, sizeof(LastUpdateRC), &UpdateRC, sizeof(UpdateRC));
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

		if (Parent)
			Parent->_SetNeedUpdate(true);
		else
		{
			this->RectChange = true;
			this->NeedUpdate = true;
		}
		return;
	}
	void SetSize(int width, int height)
	{
		DstRC.rect.right = DstRC.rect.left + (float)width;
		DstRC.rect.bottom = DstRC.rect.top + (float)height;

		memcpy_s(&LastUpdateRC, sizeof(LastUpdateRC), &UpdateRC, sizeof(UpdateRC));
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

		if (Parent)
		{
			___BackImgTargetLock.Lock();
			RELEASE(BackImgTarget);
			___BackImgTargetLock.Unlock();

			Parent->_SetNeedUpdate(true);
		}
		else
		{
			this->RectChange = true;
			this->NeedUpdate = true;
		}
		//要先处理right和bottom
		return;
	}


	//此函数稍大且用的很少，不做inline处理
	void SetTypeStyle(DTextBoxState State, DTextBoxTypeStyle type)
	{
		switch (State)
		{
		case TB_Normal:
			memcpy_s(&NormalType, sizeof(NormalType), &type, sizeof(type));
			break;
		case TB_MouseMove:
			memcpy_s(&MouseMoveType, sizeof(MouseMoveType), &type, sizeof(type));
			break;
		case TB_Edit:
			memcpy_s(&EditType, sizeof(EditType), &type, sizeof(type));
			break;
		case TB_Disable:
			memcpy_s(&DisableType, sizeof(DisableType), &type, sizeof(type));
			break;
		}

		NeedUpdate = true;//设置完后将状态置为需要更新
		return;
	}

	//此函数稍大且用的很少，不做inline处理
	void SetFontFormat(std::wstring FamliyName, float FontSize, DFontWeight FontWeight)
	{
		this->FamliyName = FamliyName;
		this->FontSize = FontSize;

		switch (FontWeight)
		{
		case NormalWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		case LightWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			break;
		case BoldWeight:
			this->FontWeight = DWRITE_FONT_WEIGHT_BOLD;
			break;

		default:
			this->FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
			break;
		}

		if (TextLayout)
		{
			___LayoutLock.Lock();
			TextLayout->SetFontFamilyName(this->FamliyName.c_str(), { 0, Text.size() });
			TextLayout->SetFontSize(this->FontSize, { 0, Text.size() });
			TextLayout->SetFontWeight(this->FontWeight, { 0, Text.size() });
			___LayoutLock.Unlock();
		}

		NeedUpdate = true;//设置完后将状态置为需要更新
		return;
	}

	bool SetState(DTextBoxState state)
	{
		if (State != state)
		{
			float time = 0.0f;
			switch (state)
			{
			case TB_Normal:
				time = NormalType.time;
				break;
			case TB_MouseMove:
				time = MouseMoveType.time;
				break;
			case TB_Edit:
				time = EditType.time;
				break;
			case TB_Disable:
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
			case TB_Normal:
				//填充颜色
				MidType.FillColor.r = (float)((float)(NormalType.FillColor.r - NowType.FillColor.r) / (float)NowFrameCount);
				MidType.FillColor.g = (float)((float)(NormalType.FillColor.g - NowType.FillColor.g) / (float)NowFrameCount);
				MidType.FillColor.b = (float)((float)(NormalType.FillColor.b - NowType.FillColor.b) / (float)NowFrameCount);
				MidType.FillColor.a = (float)((float)(NormalType.FillColor.a - NowType.FillColor.a) / (float)NowFrameCount);
				//描边颜色
				MidType.StrokeColor.r = (float)((float)(NormalType.StrokeColor.r - NowType.StrokeColor.r) / (float)NowFrameCount);
				MidType.StrokeColor.g = (float)((float)(NormalType.StrokeColor.g - NowType.StrokeColor.g) / (float)NowFrameCount);
				MidType.StrokeColor.b = (float)((float)(NormalType.StrokeColor.b - NowType.StrokeColor.b) / (float)NowFrameCount);
				MidType.StrokeColor.a = (float)((float)(NormalType.StrokeColor.a - NowType.StrokeColor.a) / (float)NowFrameCount);
				//文字颜色
				MidType.TextColor.r = (float)((float)(NormalType.TextColor.r - NowType.TextColor.r) / (float)NowFrameCount);
				MidType.TextColor.g = (float)((float)(NormalType.TextColor.g - NowType.TextColor.g) / (float)NowFrameCount);
				MidType.TextColor.b = (float)((float)(NormalType.TextColor.b - NowType.TextColor.b) / (float)NowFrameCount);
				MidType.TextColor.a = (float)((float)(NormalType.TextColor.a - NowType.TextColor.a) / (float)NowFrameCount);

				break;
			case TB_MouseMove:
				//填充颜色
				MidType.FillColor.r = (float)((float)(MouseMoveType.FillColor.r - NowType.FillColor.r) / (float)NowFrameCount);
				MidType.FillColor.g = (float)((float)(MouseMoveType.FillColor.g - NowType.FillColor.g) / (float)NowFrameCount);
				MidType.FillColor.b = (float)((float)(MouseMoveType.FillColor.b - NowType.FillColor.b) / (float)NowFrameCount);
				MidType.FillColor.a = (float)((float)(MouseMoveType.FillColor.a - NowType.FillColor.a) / (float)NowFrameCount);
				//描边颜色
				MidType.StrokeColor.r = (float)((float)(MouseMoveType.StrokeColor.r - NowType.StrokeColor.r) / (float)NowFrameCount);
				MidType.StrokeColor.g = (float)((float)(MouseMoveType.StrokeColor.g - NowType.StrokeColor.g) / (float)NowFrameCount);
				MidType.StrokeColor.b = (float)((float)(MouseMoveType.StrokeColor.b - NowType.StrokeColor.b) / (float)NowFrameCount);
				MidType.StrokeColor.a = (float)((float)(MouseMoveType.StrokeColor.a - NowType.StrokeColor.a) / (float)NowFrameCount);
				//文字颜色
				MidType.TextColor.r = (float)((float)(MouseMoveType.TextColor.r - NowType.TextColor.r) / (float)NowFrameCount);
				MidType.TextColor.g = (float)((float)(MouseMoveType.TextColor.g - NowType.TextColor.g) / (float)NowFrameCount);
				MidType.TextColor.b = (float)((float)(MouseMoveType.TextColor.b - NowType.TextColor.b) / (float)NowFrameCount);
				MidType.TextColor.a = (float)((float)(MouseMoveType.TextColor.a - NowType.TextColor.a) / (float)NowFrameCount);

				break;
			case TB_Edit:
				//填充颜色
				MidType.FillColor.r = (float)((float)(EditType.FillColor.r - NowType.FillColor.r) / (float)NowFrameCount);
				MidType.FillColor.g = (float)((float)(EditType.FillColor.g - NowType.FillColor.g) / (float)NowFrameCount);
				MidType.FillColor.b = (float)((float)(EditType.FillColor.b - NowType.FillColor.b) / (float)NowFrameCount);
				MidType.FillColor.a = (float)((float)(EditType.FillColor.a - NowType.FillColor.a) / (float)NowFrameCount);
				//描边颜色
				MidType.StrokeColor.r = (float)((float)(EditType.StrokeColor.r - NowType.StrokeColor.r) / (float)NowFrameCount);
				MidType.StrokeColor.g = (float)((float)(EditType.StrokeColor.g - NowType.StrokeColor.g) / (float)NowFrameCount);
				MidType.StrokeColor.b = (float)((float)(EditType.StrokeColor.b - NowType.StrokeColor.b) / (float)NowFrameCount);
				MidType.StrokeColor.a = (float)((float)(EditType.StrokeColor.a - NowType.StrokeColor.a) / (float)NowFrameCount);
				//文字颜色
				MidType.TextColor.r = (float)((float)(EditType.TextColor.r - NowType.TextColor.r) / (float)NowFrameCount);
				MidType.TextColor.g = (float)((float)(EditType.TextColor.g - NowType.TextColor.g) / (float)NowFrameCount);
				MidType.TextColor.b = (float)((float)(EditType.TextColor.b - NowType.TextColor.b) / (float)NowFrameCount);
				MidType.TextColor.a = (float)((float)(EditType.TextColor.a - NowType.TextColor.a) / (float)NowFrameCount);

				break;
			case TB_Disable:
				//填充颜色
				MidType.FillColor.r = (float)((float)(DisableType.FillColor.r - NowType.FillColor.r) / (float)NowFrameCount);
				MidType.FillColor.g = (float)((float)(DisableType.FillColor.g - NowType.FillColor.g) / (float)NowFrameCount);
				MidType.FillColor.b = (float)((float)(DisableType.FillColor.b - NowType.FillColor.b) / (float)NowFrameCount);
				MidType.FillColor.a = (float)((float)(DisableType.FillColor.a - NowType.FillColor.a) / (float)NowFrameCount);
				//描边颜色
				MidType.StrokeColor.r = (float)((float)(DisableType.StrokeColor.r - NowType.StrokeColor.r) / (float)NowFrameCount);
				MidType.StrokeColor.g = (float)((float)(DisableType.StrokeColor.g - NowType.StrokeColor.g) / (float)NowFrameCount);
				MidType.StrokeColor.b = (float)((float)(DisableType.StrokeColor.b - NowType.StrokeColor.b) / (float)NowFrameCount);
				MidType.StrokeColor.a = (float)((float)(DisableType.StrokeColor.a - NowType.StrokeColor.a) / (float)NowFrameCount);
				//文字颜色
				MidType.TextColor.r = (float)((float)(DisableType.TextColor.r - NowType.TextColor.r) / (float)NowFrameCount);
				MidType.TextColor.g = (float)((float)(DisableType.TextColor.g - NowType.TextColor.g) / (float)NowFrameCount);
				MidType.TextColor.b = (float)((float)(DisableType.TextColor.b - NowType.TextColor.b) / (float)NowFrameCount);
				MidType.TextColor.a = (float)((float)(DisableType.TextColor.a - NowType.TextColor.a) / (float)NowFrameCount);

				break;
			}

			LastState = State;
			State = state;

			NeedUpdate = true;
			return true;
		}
		return false;
	}
	DTextBoxState GetState()
	{
		return State;
	}

	///<summary>
	///在文本后端插入一个字符，然后把Caret移动到此处。此函数会进行边界检查
	///</summary>
	inline void AddChar(const wchar_t Ch)
	{
		if (!MultiLine && (Ch == L'\r' || Ch == L'\n'))
			Text += L' ';
		else
			Text += Ch;

		//立即刷新插入符
		this->MoveCaret_Offset(1, false);

		if (TextLayout)
		{
			___LayoutLock.Lock();
			RELEASE(TextLayout);
			___LayoutLock.Unlock();
		}
		this->NeedUpdate = true;
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
		if (TextPos > Text.size())
			textpos = Text.size();
		else textpos = TextPos;

		if (!MultiLine && (Ch == L'\r' || Ch == L'\n'))
			Text.insert(textpos, 1, L' ');
		else
			Text.insert(textpos, 1, Ch);
		//立即刷新插入符
		this->MoveCaret_Offset(1, false);

		if (TextLayout)
		{
			___LayoutLock.Lock();
			RELEASE(TextLayout);
			___LayoutLock.Unlock();
		}
		this->NeedUpdate = true;
		return;
	}
	///<summary>
	///在文本后端添加一个字符串，然后将Caret移动到文本末端
	///</summary>
	inline void AddText(std::wstring str)
	{
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

		if (TextLayout)
		{
			___LayoutLock.Lock();
			RELEASE(TextLayout);
			___LayoutLock.Unlock();
		}
		this->NeedUpdate = true;
		return;
	}
	///<summary>
	///指定一个文本位置并插入，然后将Caret移动到插入后的文本末端。此函数会进行边界检查
	///</summary>
	inline void InsertText(UINT TextPos, std::wstring str)
	{
		UINT textpos;
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
		this->MoveCaret(TextPos + str.size());

		if (TextLayout)
		{
			___LayoutLock.Lock();
			RELEASE(TextLayout);
			___LayoutLock.Unlock();
		}
		this->NeedUpdate = true;
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
	inline void SetText(std::wstring str)
	{
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
		this->MoveCaret(str.size());

		if (TextLayout)
		{
			___LayoutLock.Lock();
			RELEASE(TextLayout);
			___LayoutLock.Unlock();
		}

		this->NeedUpdate = true;
		return;
	}
	///<summary>
	///在指定文本位置向后擦除Count个字符串，Caret移动到开始擦除的位置。此函数会进行边界检查
	///</summary>
	inline void EraseText(UINT TextPos, UINT Count)
	{
		UINT textpos;
		UINT count;
		if (TextPos > Text.size())
			textpos = Text.size();
		else textpos = TextPos;

		if (Count > Text.size())
			count = Text.size();
		else count = Count;

		Text.erase(TextPos, Count);

		//立即刷新插入符
		this->MoveCaret(TextPos, false);

		if (TextLayout)
		{
			___LayoutLock.Lock();
			RELEASE(TextLayout);
			___LayoutLock.Unlock();
		}
		this->NeedUpdate = true;
		return;
	}
	///<summary>
	///在当前文本位置向后擦除Count个字符串，Caret还是当前位置。此函数会进行边界检查
	///</summary>
	inline void EraseText(UINT Count)
	{
		UINT count;

		if (Count > Text.size())
			count = Text.size();
		else count = Count;

		Text.erase(Insert_TextPos, Count);

		//立即刷新插入符
		this->MoveCaret_Offset(0, false);

		if (TextLayout)
		{
			___LayoutLock.Lock();
			RELEASE(TextLayout);
			___LayoutLock.Unlock();
		}
		this->NeedUpdate = true;
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

			if (TextLayout)
			{
				___LayoutLock.Lock();
				RELEASE(TextLayout);
				___LayoutLock.Unlock();
			}
		}
		else
			ResetTextScrolling();
		//立即刷新插入符
		Insert_LastUpdatetime = timeGetTime();//重置插入符的闪烁时间
		Insert_Visible = true;
		if (ReDraw)
			NeedUpdate = true;
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
			Insert_TextPos = TextPos;
			if (Insert_TextPos > Text.size())
				Insert_TextPos = Text.size();

			if (TextLayout)
			{
				___LayoutLock.Lock();
				RELEASE(TextLayout);
				___LayoutLock.Unlock();
			}
		}
		else {
			if (Insert_TextPos > Text.size())
				Insert_TextPos = Text.size();
			ResetTextScrolling();
		}
		Insert_LastUpdatetime = timeGetTime();//重置插入符的闪烁时间
		Insert_Visible = true;
		if (ReDraw)
			NeedUpdate = true;
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


	//*************************DXUI自用函数，用户也可根据实际情况自己用*************************

	void _SetLastFrameCount(UINT count)
	{
		this->NowFrameCount = count;
		return;
	}


	bool _GetNeedUpdate()//根据每个控件的GetNeedUpdate每帧都会调用的特性，在这里为插入符计时并设置刷新
	{
		if (State == TB_Edit)
		{
			if (Insert_Visible)
			{
				if (timeGetTime() - Insert_LastUpdatetime > 500)
				{
					Insert_Visible = false;
					NeedUpdate = true;
					Insert_LastUpdatetime = timeGetTime();
				}
			}
			else {
				if (timeGetTime() - Insert_LastUpdatetime > 500)
				{
					Insert_Visible = true;
					NeedUpdate = true;
					Insert_LastUpdatetime = timeGetTime();
				}
			}
		}

		return NeedUpdate;
	}

	//输入处理
	bool _KeyDownMsg(WPARAM wParam)
	{
		if (State == TB_Edit)
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
					this->MoveCaret_Offset(2);
				else
					this->MoveCaret_Offset(1);
				break;
			}
			case VK_UP:
			{
				if (SelectMode)
					this->ExitSelectMode();

				this->ResetInsertPos((int)(Insert_PixelPos.left - Scr_Offset.x * dpiX / 96.0f), (int)(Insert_PixelPos.top - Scr_Offset.y * dpiY / 96.0f - 1.0f));
				break;
			}
			case VK_DOWN:
			{
				if (SelectMode)
					this->ExitSelectMode();

				this->ResetInsertPos((int)(Insert_PixelPos.left - Scr_Offset.x * dpiX / 96.0f), (int)(Insert_PixelPos.bottom - Scr_Offset.y * dpiY / 96.0f + 1.0f));
				break;
			}
			case 'V':
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
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
	void _CharMsg(UINT Msg, WPARAM wParam, LPARAM lParam, DControlMsg* ConMsg)
	{
		if (ConMsg)
			* ConMsg = TextBox_CharChange;

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
					if (ConMsg)
						* ConMsg = TextBox_Enter;
					break;
				}

				this->InsertText(L"\r\n");

				break;
			case 0x08://退格键
				if (!Text.empty())
				{
					if (SelectMode)
					{
						this->EraseText(Select_BeginPos, Select_EndPos - Select_BeginPos);
						this->ExitSelectMode();
					}
					else if (Insert_TextPos > 0)
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

		return;
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
		if (State == TB_Disable)
			return false;

		SetCursor(LoadCursorW(nullptr, IDC_IBEAM));//设置为工字型鼠标
		return true;
	}

	bool _SetNormalState(HWND hWnd, UINT Msg, UINT NowControlID = 0)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == TB_Disable)
		{
			return false;
		}
		if ((Msg == WM_MOUSEMOVE || Msg == WM_NCMOUSEMOVE) && OnClick)
		{
			POINT p;
			if (GetCursorPos(&p))
			{
				if (ScreenToClient(hWnd, &p))
				{
					SetCapture(hWnd);
					this->_MouseMoveMsgProc(p.x, p.y, nullptr);
					this->_SetCursorMsgProc();
				}
			}
		}
		else if (Msg == WM_LBUTTONUP || Msg == WM_NCLBUTTONUP)
		{
			OnClick = false;
			ReleaseCapture();
		}

		if (NowControlID == ID)
			return false;

		___ChildLock.BeginRead();

		if (Child.find(NowControlID) != Child.end() && (Msg == WM_MOUSEMOVE || Msg == WM_NCMOUSEMOVE))
			HaveSetNormal = true;
		else
			HaveSetNormal = false;

		___ChildLock.EndRead();

		if (HaveSetNormal)
			return false;

		if (State == TB_Edit && (Msg == WM_MOUSEMOVE || Msg == WM_NCMOUSEMOVE || Msg == WM_LBUTTONUP || Msg == WM_NCLBUTTONUP || Msg == WM_MOUSELEAVE))
			return false;

		OnClick = false;

		return SetState(TB_Normal);
	}

	bool _MouseMoveMsgProc(int xPos, int yPos, DControlMsg* msg)
	{
		if (HaveSetNormal)
		{
			HaveSetNormal = false;
			return true;
		}

		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == TB_Disable)
			return false;

		if (msg)
			* msg = TextBox_MouseMove;

		if (OnClick && !Text.empty())
		{
			SetSelectText_Pos(xPos, yPos);//不要在此设置SelectMode为true，函数会自行分辨并设置
		}

		if (State == TB_Edit)
			return false;

		return SetState(TB_MouseMove);
	}
	bool _LMouseDownMsgProc(int xPos, int yPos, DControlMsg* msg)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == TB_Disable)
		{
			return false;
		}
		//更新插入符的位置
		ResetInsertPos(xPos, yPos);
		//取消选中模式
		if (SelectMode)
		{
			this->ExitSelectMode();
			this->NeedUpdate = true;
		}
		Select_DownPos = Insert_TextPos;

		if (msg)
			* msg = TextBox_LButtonDown;
		OnClick = true;
		HaveSetNormal = false;

		return SetState(TB_Edit);
	}
	bool _LMouseUpMsgProc(int xPos, int yPos, DControlMsg* msg)
	{
		//禁止时，如果是其他三种状态的鼠标消息为来源的设置状态，全部不处理。必须为手动设置其它状态才处理
		if (State == TB_Disable || !OnClick)
			return false;

		if (msg)
			* msg = TextBox_LButtonUp;

		ReleaseCapture();
		OnClick = false;
		HaveSetNormal = false;

		return SetState(TB_Edit);
	}
	bool _MouseWheelMsgProc(DControlMsg* msg, WPARAM wParam)
	{
		if (msg)
			* msg = TextBox_Scrolling;

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
				this->NeedUpdate = true;
			}
		}
		else if (Delta < 0)//手指向后滑动时
		{
			if (Select_Metrics.height - Scr_Offset.y > TextOutputRC.bottom - TextOutputRC.top)
			{
				Scr_Offset.y += FontSize * (float)ScrollLines;

				if (Select_Metrics.height - Scr_Offset.y < TextOutputRC.bottom - TextOutputRC.top)//文本太靠上时的处理
					Scr_Offset.y = Select_Metrics.height - TextOutputRC.bottom + TextOutputRC.top;
				this->NeedUpdate = true;
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

	bool _Draw(ID2D1BitmapRenderTarget* DstTarget, D2D1_COLOR_F BackColor, D2D1_RECT_F MainWndRC)
	{
		if (!DstTarget)
		{
			NeedUpdate = true;
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
		case TB_Normal:
		{
			DstType = &NormalType;

			break;
		}
		case TB_MouseMove:
		{
			DstType = &MouseMoveType;

			break;
		}
		case TB_Edit:
		{
			DstType = &EditType;

			break;
		}
		case TB_Disable:
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

			//动画——填充颜色处理
			NowType.FillColor.r = DstType->FillColor.r - (float)(NowFrameCount * MidType.FillColor.r);
			NowType.FillColor.g = DstType->FillColor.g - (float)(NowFrameCount * MidType.FillColor.g);
			NowType.FillColor.b = DstType->FillColor.b - (float)(NowFrameCount * MidType.FillColor.b);
			NowType.FillColor.a = DstType->FillColor.a - (float)(NowFrameCount * MidType.FillColor.a);

			f_FillColor.r = (float)NowType.FillColor.r / 255.0f;
			f_FillColor.g = (float)NowType.FillColor.g / 255.0f;
			f_FillColor.b = (float)NowType.FillColor.b / 255.0f;
			f_FillColor.a = NowType.FillColor.a;
			//动画——描边颜色处理
			NowType.StrokeColor.r = DstType->StrokeColor.r - (float)(NowFrameCount * MidType.StrokeColor.r);
			NowType.StrokeColor.g = DstType->StrokeColor.g - (float)(NowFrameCount * MidType.StrokeColor.g);
			NowType.StrokeColor.b = DstType->StrokeColor.b - (float)(NowFrameCount * MidType.StrokeColor.b);
			NowType.StrokeColor.a = DstType->StrokeColor.a - (float)(NowFrameCount * MidType.StrokeColor.a);

			f_StrokeColor.r = (float)NowType.StrokeColor.r / 255.0f;
			f_StrokeColor.g = (float)NowType.StrokeColor.g / 255.0f;
			f_StrokeColor.b = (float)NowType.StrokeColor.b / 255.0f;
			f_StrokeColor.a = NowType.StrokeColor.a;
			//动画——文字颜色处理
			NowType.TextColor.r = DstType->TextColor.r - (float)(NowFrameCount * MidType.TextColor.r);
			NowType.TextColor.g = DstType->TextColor.g - (float)(NowFrameCount * MidType.TextColor.g);
			NowType.TextColor.b = DstType->TextColor.b - (float)(NowFrameCount * MidType.TextColor.b);
			NowType.TextColor.a = DstType->TextColor.a - (float)(NowFrameCount * MidType.TextColor.a);

			f_TextColor.r = (float)NowType.TextColor.r / 255.0f;
			f_TextColor.g = (float)NowType.TextColor.g / 255.0f;
			f_TextColor.b = (float)NowType.TextColor.b / 255.0f;
			f_TextColor.a = NowType.TextColor.a;

		}
		else {
			//静态——填充颜色处理
			NowType.FillColor.r = (float)DstType->FillColor.r;
			NowType.FillColor.g = (float)DstType->FillColor.g;
			NowType.FillColor.b = (float)DstType->FillColor.b;
			NowType.FillColor.a = DstType->FillColor.a;

			f_FillColor.r = (float)NowType.FillColor.r / 255.0f;
			f_FillColor.g = (float)NowType.FillColor.g / 255.0f;
			f_FillColor.b = (float)NowType.FillColor.b / 255.0f;
			f_FillColor.a = NowType.FillColor.a;
			//静态——描边颜色处理
			NowType.StrokeColor.r = (float)DstType->StrokeColor.r;
			NowType.StrokeColor.g = (float)DstType->StrokeColor.g;
			NowType.StrokeColor.b = (float)DstType->StrokeColor.b;
			NowType.StrokeColor.a = DstType->StrokeColor.a;

			f_StrokeColor.r = (float)NowType.StrokeColor.r / 255.0f;
			f_StrokeColor.g = (float)NowType.StrokeColor.g / 255.0f;
			f_StrokeColor.b = (float)NowType.StrokeColor.b / 255.0f;
			f_StrokeColor.a = NowType.StrokeColor.a;
			//静态——文字颜色处理
			NowType.TextColor.r = (float)DstType->TextColor.r;
			NowType.TextColor.g = (float)DstType->TextColor.g;
			NowType.TextColor.b = (float)DstType->TextColor.b;
			NowType.TextColor.a = DstType->TextColor.a;

			f_TextColor.r = (float)NowType.TextColor.r / 255.0f;
			f_TextColor.g = (float)NowType.TextColor.g / 255.0f;
			f_TextColor.b = (float)NowType.TextColor.b / 255.0f;
			f_TextColor.a = NowType.TextColor.a;

		}
		//尽量减少锁的锁定时间，靠下放
		___LayoutLock.Lock();
		if (!TextFormat && DWriteFactory)
		{
			DWriteFactory->CreateTextFormat(FamliyName.c_str(), nullptr, FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, FontSize, L"zh-cn", &TextFormat);
			TextFormat->SetWordWrapping(AutoWarp);
			TextFormat->SetTextAlignment(xAlignment);
			TextFormat->SetParagraphAlignment(yAlignment);
			RELEASE(TextLayout);//Format改了，Layout也要重新创建，但一般不会释放Format
		}
		if (!TextLayout && DWriteFactory)
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


		//自文本裁剪
		ID2D1Layer* TextClipLayer = nullptr;

		___TextClipLock.Lock();

		if (!SelfTextClip && D2DFactory)//如果自己的文本裁剪没有，则创建
			D2DFactory->CreateRoundedRectangleGeometry(DstRC, &SelfTextClip);

		if (SelfTextClip)//判断一下，如果有当前文本裁剪区再创建
			DstTarget->CreateLayer(nullptr, &TextClipLayer);


		DstTarget->BeginDraw();

		if (Parent)//有底层控件, 要重画backimg
		{
			DstTarget->PushAxisAlignedClip(Parent->_GetClipRC(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//设置父控件的裁剪

			if (!ParentDrawing && BackImg)
			{
				DstTarget->DrawBitmap(BackImg, UpdateRC, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
		}
		else {//无底层控件，直接用窗口背景色填充
			if (RectChange)
			{
				DstTarget->FillRoundedRectangle({ LastUpdateRC, DstRC.radiusX, DstRC.radiusY }, brush);
			}
			DstTarget->FillRoundedRectangle({ UpdateRC, DstRC.radiusX, DstRC.radiusY }, brush);
		}


		brush->SetColor(f_FillColor);//设置为填充颜色
		DstTarget->FillRoundedRectangle(&DstRC, brush);

		brush->SetColor(f_StrokeColor);//设置为描边颜色
		DstTarget->DrawRoundedRectangle(&DstRC, brush, StrokeWidth);


		//设置当前文本的裁剪区
		if (TextClipLayer)//设置当前文本的裁剪区，必须等背景的东西画完了，再设置，不然背景也裁剪了
			DstTarget->PushLayer(D2D1::LayerParameters(UpdateRC, SelfTextClip), TextClipLayer);

		___TextClipLock.Unlock();
		//画文本
		if (TextLayout)
		{
			brush->SetColor(f_TextColor);//设置为文字颜色
			DstTarget->DrawTextLayout({ TextOutputRC.left - Scr_Offset.x, TextOutputRC.top - Scr_Offset.y }, TextLayout, brush);
		}
		___LayoutLock.Unlock();

		//如果为选中状态，填充选中区域
		if (SelectMode && Select_Rects)
		{
			brush->SetColor(Select_FillColor);
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
		if (Insert_Visible && State == TB_Edit)
		{
			D2D1_RECT_F DIPPos = { Insert_PixelPos.left * 96.0f / dpiX - Scr_Offset.x, Insert_PixelPos.top * 96.0f / dpiY - Scr_Offset.y, Insert_PixelPos.right * 96.0f / dpiX - Scr_Offset.x, Insert_PixelPos.bottom * 96.0f / dpiY - Scr_Offset.y };
			brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

			DstTarget->FillRectangle(DIPPos, brush);
		}

		if (TextClipLayer)//弹出自己的文本裁剪区
			DstTarget->PopLayer();

		if (Parent)
			DstTarget->PopAxisAlignedClip();

		DstTarget->EndDraw();

		RELEASE(TextClipLayer);

		_EndDraw_CallChildToUpdateBackImg(DstTarget);

		NeedUpdate = NowFrameCount;
		return true;
	}


private:
	bool OnClick = false;
	bool HaveSetNormal = false;

	DTextBoxState State = TB_Normal;
	DTextBoxState LastState = TB_Normal;


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

	ID2D1SolidColorBrush* brush = nullptr;

	//文本区
	ID2D1RoundedRectangleGeometry* SelfTextClip = nullptr;//自身的文字裁剪区，防止文字超出范围
	___DInterfaceLock ___TextClipLock;
	wchar_t ImeChar[2] = { 0 };//接受到消息时存储单个字，某些生僻字占用4字节

	std::wstring Text;
	std::wstring FamliyName;
	float FontSize = 0.0f;
	bool MultiLine = true;

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
	___DInterfaceLock ___LayoutLock;


	inline void ResetTextScrolling()
	{
		float insertX = Insert_PixelPos.right * 96.0f / dpiX - TextOutputRC.left - Scr_Offset.x;
		float insertUpY = Insert_PixelPos.top * 96.0f / dpiY - TextOutputRC.top - Scr_Offset.y;
		float insertBottomY = Insert_PixelPos.bottom * 96.0f / dpiY - TextOutputRC.top - Scr_Offset.y;

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
		if (Select_Metrics.height > Select_Metrics.layoutHeight && Select_Metrics.height - Scr_Offset.y < TextOutputRC.bottom - TextOutputRC.top)
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

		TextLayout->HitTestPoint((float)(xPos * 96.0f / dpiX - TextOutputRC.left + Scr_Offset.x), (float)(yPos * 96.0f / dpiY - TextOutputRC.top + Scr_Offset.y), &IsBehind, &isInside, &hit);

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
				NeedUpdate = true;
			return;
		}

		while (!TextLayout)
			Sleep(2);
		if (Lock)
			___LayoutLock.Lock();

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
			NeedUpdate = true;
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

		while (!TextLayout)
			Sleep(2);
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

			NeedUpdate = true;
			return;
		}

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
		NeedUpdate = true;

		return;
	}

	inline void ResetInsertPos(int xPos, int yPos)
	{
		DWRITE_HIT_TEST_METRICS hit;
		BOOL IsBehind = false;
		BOOL isInside = false;

		while (!TextLayout)
			Sleep(2);
		___LayoutLock.Lock();
		TextLayout->HitTestPoint((float)(xPos * 96.0f / dpiX - TextOutputRC.left + Scr_Offset.x), (float)(yPos * 96.0f / dpiY - TextOutputRC.top + Scr_Offset.y), &IsBehind, &isInside, &hit);
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
			Insert_PixelPos.left = (TextOutputRC.left + hit.left + hit.width) * dpiX / 96.0f;
			Insert_PixelPos.top = (TextOutputRC.top + hit.top) * dpiY / 96.0f;
			Insert_PixelPos.right = Insert_PixelPos.left + DTextBox_CaretWidth;
			Insert_PixelPos.bottom = Insert_PixelPos.top + hit.height * dpiY / 96.0f;
		}
		else {
			Insert_PixelPos.left = (TextOutputRC.left + hit.left) * dpiX / 96.0f;
			Insert_PixelPos.top = (TextOutputRC.top + hit.top) * dpiY / 96.0f;
			Insert_PixelPos.right = Insert_PixelPos.left + DTextBox_CaretWidth;
			Insert_PixelPos.bottom = Insert_PixelPos.top + hit.height * dpiY / 96.0f;
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

		Insert_PixelPos.left = (TextOutputRC.left + hit.left) * dpiX / 96.0f;
		Insert_PixelPos.top = (TextOutputRC.top + hit.top) * dpiY / 96.0f;
		Insert_PixelPos.right = Insert_PixelPos.left + DTextBox_CaretWidth;
		Insert_PixelPos.bottom = Insert_PixelPos.top + hit.height * dpiY / 96.0f;

		ResetTextScrolling();//超出文本框时的处理
		return;
	}
};
