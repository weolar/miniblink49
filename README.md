# miniblink49
----

https://weolar.github.io/miniblink/ 是miniblink的新官网。

关于miniblink的介绍见这篇文章：https://zhuanlan.zhihu.com/p/22611497?group_id=764036386641707008

API文档见：https://weolar.github.io/miniblink/doc-main.html 

miniblink是一个开源、单文件、目前已知的最小的基于chromium的，浏览器控件。通过导出的纯C接口，可以几行代码创建一个浏览器控件。

----

# 使用
* 请前往https://github.com/weolar/miniblink49/releases 下载最新编译后的SDK，里面的demo_src是个完整的用例。

最简单的创建一个窗口：
**Usage**
```cpp
wkeWebView window = wkeCreateWebWindow(WKE_WINDOW_TYPE_TRANSPARENT, NULL, 0, 0, 640, 480);  // 无边框窗体 borderless window
wkeLoadURLW(window, L"miniblink.net");
```
![demo-1](https://raw.githubusercontent.com/wiki/ocornut/imgui_club/images/memory_editor_v19.gif)

# 编译

不推荐自己编译。因为每天有大量更新，我无法确保每次更新都能保证编译通过。如果有编译错误，请等待我的下次提交

----

# mini-electron

mini-electron项目是一个基于miniblink的独立项目，旨在创建一个更小的electron运行环境。目前已经实现了这一目标。
通过替换mini-electron，打包完后的文件仅仅6m左右。

# 联系方式

大家有问题可以选择：

1，加Q群94093808

2，邮箱weolar@qq.com

3，github里留言issue讨论

4，关注知乎专栏：https://zhuanlan.zhihu.com/chrome

----

# 致谢

特别感谢网友zero，他帮我提交了很多代码。

感谢网友core，他赞助了一个论坛：http://miniblink.net/

感谢网友boxue（https://www.zhihu.com/people/coltor/） ，他致力于在微信小程序里推广miniblink架构


