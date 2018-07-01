# miniblink49
=======================================================

关于miniblink的介绍见这篇文章：https://zhuanlan.zhihu.com/p/22611497?group_id=764036386641707008
API文档见：https://blog.csdn.net/weolar/article/details/80458523


简单的说，miniblink是个浏览器控件，如果你用过cef、wke、webbrowser，应该很好理解。

一、关于编译
========================================================
1、目前编译尽量使用VS2015以上的编译器，否则可能出现未知错误。尽量用vs2015.bat打开（重要）

2、Release版本默认采用VC6运行时库，此特性可以通过USING_VC6RT宏控制

3、miniblink默认同时导出CEF和WKE两种接口，导出接口可以通过ENABLE_CEF和ENABLE_WKE两个宏控制.#这里至少选择一种导出接口

4、更详细内容请见 http://miniblink.net/article/4 编译指南

5、如果只想看demo，可以编译 https://github.com/weolar/miniblink49/releases 里自带的demo_src
   或者只下载https://github.com/weolar/miniblink49/tree/master/wkexe 目录然后编译

6、需要编译好的dll以及demo的源码和工程，在 https://github.com/weolar/miniblink49/releases 下载

二、杂项说明
========================================================
1、miniblink主要是用来做PC端的浏览器嵌入组件，由于导出的是wke和cef的接口，所以你可以替换到你的项目中去体验和学习。基本相当于一个wke的升级版本，或者cef的精简版。

2、electron的接口目前已经实现绝大部分接口，可以实战使用

3、例子可见源码中的wkexe工程，或者cefclient（被废弃）

三、联系方式
========================================================
大家有问题可以选择：

1，加Q群94093808

2，邮箱weolar@qq.com

3，csdn：blog.csdn.net/weolar

4，github里留言issue讨论

5，关注知乎专栏：https://zhuanlan.zhihu.com/chrome


当然我不会所有问题都回答的，毕竟还要去工地搬砖养家糊口。

最后特别感谢zero，他帮我提交了很多代码。

另外感谢网友core，他赞助了一个论坛：http://miniblink.net/
