# 声明

鉴于最近发生的一些不愉快的事情（没错，说的就是那些直接把代码拿去改个名字就开始卖钱的人），

深思熟虑后决定自2019-6-17起将新开一个分支，并逐步减少此仓库的更新

后续更新将以二进制及头文件方式发布，请持续关注。

希望大家尊重开源，尊重作者全职几年持续更新付出的劳动。

**且用且珍惜**

如需获得后续支持，请使用以下联系方式：

Q群：94093808

QQ（weolar）：93527630

email：weolar@miniblink.net


# 简介 Abstract

miniblink is a open source, one file, small browser widget base on chromium.

By using C interface, you can create a browser just some line code.

more information at http://miniblink.net

----

miniblink是一个开源的、单文件、且目前已知的最小的基于chromium的，浏览器控件。

通过其导出的纯C接口，几行代码即可创建一个浏览器控件。

您可以通过官网http://miniblink.net 来获取更多的关于miniblink的信息。


----

# 特性 Features

- 极致小巧的体积 (small size)
- C++，C#，Delphi等语言调用 (support C++，C#，Delphi language to call)
- 内嵌Nodejs，支持electron (with Nodejs, can run electron)
- 随心所欲的定制功能、模拟环境 (simulate other browser environment)
- 支持Windows xp、npapi (support windows xp and npapi)
- 完善的HTML5支持，对各种前端库友好 (support HTML5, and friendly to front framework)
- 关闭跨域开关后，可以使用各种跨域功能 (support cross domain)
- 网络资源拦截，替换任意网站任意js为本地文件 (network intercept, you can replace any resource to local file)
- headless模式，极大节省资源，适用于爬虫 (headless mode, be suitable for Web Crawler)

----

# 文档 Document

关于miniblink的介绍见这篇文章：https://zhuanlan.zhihu.com/p/22611497?group_id=764036386641707008

API文档见：https://miniblink.net/doc-main.html 

----

# 使用 Usage
请前往https://github.com/weolar/miniblink49/releases 下载最新编译后的SDK，里面的demo_src是个完整的用例。

或者前往 https://github.com/weolar/mb-demo 下载

最简单的创建一个窗口：

**Usage**

```cpp
// 无边框窗体 borderless window
wkeWebView window = wkeCreateWebWindow(WKE_WINDOW_TYPE_TRANSPARENT, NULL, 0, 0, 640, 480);  
wkeLoadURLW(window, L"miniblink.net");
```
![demo-1](https://weolar.github.io/miniblink/assets/images/demo-0.gif)

# 编译 Build

不推荐自己编译。请前往https://github.com/weolar/miniblink49/releases 下载编译好的文件使用。

因为每天有大量更新，我无法确保每次更新都能保证编译通过。如果有编译错误，请不要来提问，耐心等待我的下次提交。

----

# mini-electron

mini-electron项目是一个基于miniblink的独立项目，旨在创建一个更小的electron运行环境。目前已经实现了这一目标。

通过替换mini-electron，打包完后的文件仅仅6m左右。

----

# 联系方式

大家有问题可以选择：

- 加Q群94093808

- 邮箱weolar@miniblink.net

- github里留言issue讨论

- 关注知乎专栏：https://zhuanlan.zhihu.com/chrome

----

# 致谢 Thanks

特别感谢网友zero，他是miniblink的代码的重要贡献者。

感谢网友core，感谢网友“大清知府”。

感谢网友boxue（ https://www.zhihu.com/people/coltor/ ），他致力于对miniblink架构的研究及推广。


