# DXUI-Library
### 此库来之不易，使用请标明作者。商业用途请先联系我。
**共同维护良好的开源环境！**<br>
**QQ: 2441987560<br>
WeChat: yangzhen2441987560<br>
Email: 2441987560@qq.com**<br>
**欢迎通过各种方式提出宝贵的意见，每个周末我都会查看，有不懂的也可以找我交流。**<br><br>

一个 **超级简单、动画华丽** 的界面库<br>
取“DXUI”这个名字是因为此界面库使用Direct 2D图形库来绘制，使用Wic库实现加载图片及部分功能，使用DWrite库来绘制处理文字。<br>
基于Direct系列的界面库少之又少，一是实现太过复杂，学习成本太高。二是考虑到硬件兼容的问题。<br>
而Direct 2D完美的解决了这些问题，同时保留了硬件加速的优点，用来做界面库，实现流畅的动画再好不过了。<br><br>
DXUI界面库的优点是：
* 1.控件样式可以90%自定义，并且完善的透明度支持为界面增色不少。<br>
* 2.基于事件驱动的动画，用户无操作时不消耗资源。<br>
* 3.极低的CPU、GPU和内存占用，不用担心办公电脑等低配置环境的运行问题。<br>
* 4.局部刷新技术，背景图再大，上层控件动画也能流畅播放。<br>
* 5.非常简单，开发者只需要考虑如何找到好看的颜色，仅通过几行代码即可添加控件。<br>
* 6.高兼容性，无需考虑硬件支持问题，就算不支持，可无缝切换到软件绘制，效率优于GDI。<br>
* 7.拥有比GDI/GDI+更高的绘图质量，界面更清晰大方。<br>
* 8.完善的高DPI支持，再也不用担心系统缩放模糊的问题。<br><br>
**测试程序全部代码仅有500行(包括基础Win程序框架和获取性能信息部分)，真正的简单、不繁琐。**<br>
预览图：<br><br>
**注意：这里的内存占用与任务管理器显示的不一致，实际远小于这个值。**<br>
![main](https://github.com/IDXGI/DXUI-Library/blob/master/preview%20pictures/Main.jpg)<br><br>
以后有时间会继续添加颜色框、控件页、列表框、视频框等控件。由于我上学，1周或2周回家一次，更新进度会比较慢。<br><br>


**————————200424 更新内容————————**<br>
1.修复了许多的小BUG(忘了是啥了，也懒得写)，代码量由7k+增至1w+。<br>
2.TextBox增加可选平滑滚动动画、插入符动画。增加某些输入法(如微软拼音)的组合字符串实时显示。<br>
3.增加DSolidColorBox、DLinearColorBox和DRadialColorBox用来显示单调颜色、线性渐变与梯度渐变。并且后两者支持渐变起始位置和终点位置的动画。<br>
4.增加窗口阴影的透明度动画，一行代码即可实现。<br>
5.增加DTransparentWindow，与DWindow完全一样的使用方法，但可以来实现异形窗口，此类创建的窗口可没有背景色，只显示控件。<br>
6.优化TextBox字符处理部分的稳定性。<br>
7.每个控件增加整体的透明度TotalOpacity，并与填充颜色、描边颜色等透明度互不干扰，TotalOpacity也支持动画。<br>
8.优化绘制功能的性能，改进了绘制结构，在高强度绘制时有明显改善。<br>
9.优化了日常绘制时的卡顿感，还原对应帧率的真实体验。<br>
10.为确保稳定性，通知给调用者的控件消息都自动转为主线程再通知，防止调用者处理时不在同一线程而出错。<br>
11.每个动画启动时都可以设置AID，动画结束时，会发送Control_EndOfAnimation消息来通知用户，并会把启动时的AID传递过去，可据此进行处理。<br>
<br>
由于不在住所，所以没有修改示例程序的代码，旧代码可能会有些小地方不兼容。最新的程序ChatUI会在晚些时候发布。<br>
**————————200316 更新内容————————**<br>
1.根据方程的建议简化了部分冗余代码。<br>
2.修正用户手动SetState和修改TextBox文本不会触发消息的问题。<br>
3.TextBox增加GetLength()类函数。<br>
4.修正TextBox文本的裁剪区域为超出一半描边宽度的问题。<br>
5.优化了重叠的控件在动画时的更新区域。<br>
6.示例程序增加一个重叠的TextBox。<br>
**————————200315 更新内容————————**<br>
1.修复各种大大小小的BUG。<br>
2.更改控件消息的传递方式，不在WndProc里。更简洁、直接。<br>
3.简化控件消息。<br>
4.增加设置窗口阴影。<br>
5.优化动画流畅度。<br>
6.增加了控件 位置、大小、不透明度、圆角半径的动画，并且可选用贝塞尔曲线参数实现非线性动画。<br>
7.ImageBox新增一行代码实现软件多线程高斯模糊功能。<br>
8.示例程序增加了多种华丽的非线性动画。<br>
写完几个大的功能再更新吧，更新好麻烦呀。<br>
**————————191130 更新内容————————**<br>
1.修复文本框在接收文本时单击鼠标左键会无限期假死的问题。<br>
2.修改了示例程序控件的部分参数，更加美观。<br>
期中成绩优秀，最近更新没有那么积极。<br>
**————————191102 更新内容————————**<br>
1.修复文本框插入符不能左右移动的问题。<br>
2.创建渲染线程时，自动先把第一帧画上，防止显示窗口时白屏一下。<br>
3.示例程序的控件样式优化，更加美观。文本框也添加了文字颜色过渡动画。<br>
本周还是只有一天时间，而且面临期中考试，近期不会有大的更新（我太难了）。<br>
**————————191019 更新内容————————**<br>
1.修复线程锁的问题，用户可多线程调用此库。<br>
2.大幅优化ImageBox加载图片后占用的内存。<br>
3.控件被析构后，自动释放，不用再每次手动释放。<br>
4.enum语法改为C++11新特性enum class。<br>
5.修复Label在 高速修改内容 同时 鼠标在移动 时会造成动画闪烁的问题，修复后无论修改多快都完美流畅显示。<br>
6.示例程序左下角新增 系统CPU占用 和 程序内存占用 信息，并单独开了一个线程来更新信息。由于获取这些信息，示例程序代码量增到500行。<br>
7.示例程序改为静态编译，在没有VC++运行库的电脑上也能运行。<br>
因为本周只有一天时间，只优化了一些小问题，预计下次更新一周后，更新步入正轨。<br>
**————————191004 更新内容————————**<br>
1.修复了当窗口大小超出屏幕大小时，窗口坐标计算出错导致窗口无法显示的问题。问题出在示例程序上，库没有问题。<br>
由于国庆回去后段考，认真在家复习，国庆暂不更新。预计下一次更新为2周后，要添加控件的位置、大小、不透明度的动画，然后给ImageBox增加高斯模糊功能及模糊半径的动画。<br>
**————————190922 更新内容————————**<br>
1.ImageBox实现圆角功能。<br>
2.优化控件绘制时逻辑，不透明度为0时不绘图。<br>
3.TextBox的插入符颜色和宽度支持自定义。<br>
**————————190921 更新内容————————**<br>
1.优化了在客户区拖动窗口的实现方式，新方法没有任何特殊限制。<br>
2.优化ImageBox的加载函数，理论支持图片大小可达系统极限值，且上层控件动画依然流畅。还优化了加载失败后的处理。<br>
3.优化了示例程序的控件参数，使其更自然、流畅。还增加了一些控件，更好地展示此库的简单、美观。更新后示例程序去掉注释和换行，依旧只有350行左右。<br>
**———————————————————————**<br>
