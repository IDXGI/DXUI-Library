# DXUI-Libaray
一个简单、多动画的界面库

取“DXUI”这个名字是因为此界面库使用Direct 2D图形库来绘制，使用Wic库实现加载图片及部分功能，使用DWrite库来绘制处理文字。
基于Direct系列的界面库少之又少，一是实现太过复杂，学习成本太高。二是考虑到硬件兼容的问题。
而Direct 2D完美的解决了这些问题，同时保留了硬件加速的优点，用来做界面库，实现流畅的动画再好不过了。

DXUI界面库的优点是：
1.控件样式可以90%自定义，并且完善的透明度支持为界面增色不少。
2.基于事件驱动的动画，用户无操作时不消耗资源。
3.局部刷新技术，背景图再大，上层控件动画也能流畅播放。
4.非常简单，开发者只需要考虑如何找到好看的颜色，仅通过几行代码即可添加控件。
5.高兼容性，无需考虑硬件支持问题，就算不支持，可无缝切换到软件绘制，效率优于GDI。
6.拥有比GDI/GDI+更高的绘图质量，界面更清晰大方。
7.完善的高DPI支持，再也不用担心系统缩放模糊的问题。

预览图：
![main](https://github.com/IDXGI/DXUI-Libaray/blob/master/preview%20picture/main.jpg)
![main1](https://github.com/IDXGI/DXUI-Libaray/blob/master/preview%20picture/main1.jpg)
![About](https://github.com/IDXGI/DXUI-Libaray/blob/master/preview%20picture/about.jpg)
