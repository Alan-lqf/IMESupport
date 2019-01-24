## Sublime Text Build 3186

从 BUILD 3186 开始，ST 官方支持 IME 了，此插件或许可以被标注 Deprecated 了。

## IMESupport

Sublime Text 中文输入法支持，解决输入法候选框无法跟随 st 光标 (cursor) 的问题。

## 须知

### 原理

由于 st 默认不支持 IME (Input Method Editor)，导致在使用微软拼音、搜狗拼音等输入法输入中文的时候，候选框会固定在屏幕的一个角上，不会跟随光标的移动而移动。这个插件就是利用 st 的 api 接口和 Windows 的 hook (钩子) 机制，用 hackable (一种无奈且并不优雅) 的方式让 st 支持 IME (或者说 IMM/Input Method Manager)。

插件的主要操作分为两步:

1. 在光标位置改变时，通过 st 内置的事件监听接口触发获取光标位置操作（现在 st 内置 api 支持获取光标位置），然后将位置信息传递给加载的钩子处理。

2. hook 中的主要操作使用 Windows Api (ImmSetCompositionWindow) 设置输入法候选框的位置。

### Sublime Text 版本

由于旧版本的 st 没有获取光标相对于 st 窗口 (window) 的 api，最开始插件获取光标位置是通过获取当光标相对于 st 布局 (layout) 的位置，再配合侧边栏的宽度、手动配置调整参数等信息手动计算的，这样导致光标的位置极其不准确（换个主题可能侧边栏的宽度等就不同了），候选框跟随定位准确也就无从说起了。

从 BUILD 3143 起，st 有了可以获取光标（cursor）相对于窗口位置的 api（View.text_to_window()), 并鉴于以前手动计算问题太多、太不可靠，所有对代码做了精简，直接使用新的 api，而不再支持手动计算方式，所以，此插件支持 Sublime Text 3 Build 3143+（注意：跟 3143 比较临近的版本 api 实现并不是很好，如光标在搜索等输入框时位置也无法获取，so，尽量用最新版本吧）。

### Windows 系统版本和高清屏

鉴于 Windows 对于高清屏（缩放率大于 100%）的支持并不是很好，从 Windows 10 起这种状况才有所改观，并且某些针对缩放率等信息的 Windows api 从 Windows 10 起才开始支持，所以插件支持最好的 Windows 版本是 10。 如果 Windows 系统版本低于 Windows 10， 该插件也可以使用，唯一的区别就是无法自动获取显示的缩放率，如果使用缩放率大于 100，且小于 Windows 10 版本的 Windows，可以通过设置 ime_support_screen_scaling 手动配置一下。

缩放率 100% 的默认配置:
```json
{
  "imesupport_screen_scaling": 100
}
```

目前，这是该插件唯一需要手动设置的，以前的配置信息（offset 之类的）已经不再需要。

## 安装

注意：这是 [chikatoike/IMESupport](https://github.com/chikatoike/IMESupport) 的
fork 版本，所以不要使用 package_control 安装。

直接将该项目克隆或是下载到 Sublime_Text_3_Install_Path/Data/Packages 目录下就好，不知道该目录在哪的同学可以使用菜单 Perference > Browse Packages 打开。

## 已知问题

### 第一次输入定位不准

目前测试来看，这个问题主要原因还是插件未加载完成造成的，所以刚打开 st 的时候，稍等等，等插件加载完就好了。

## ChangeLog

### 2018-11-25
* 简化代码，放弃支持 Sublime Text 老版本 (Sublime Text 3.0+ 起)
* 兼容 Windows 7 +
* 兼容 搜狗拼音、QQ 拼音、手心输入法、小狼毫输入法

### 2017-07-31
* 新版本 (dev build 3142) 可以使用 View.text_to_view 获取光标的位置。
* 使用 Visual Studio 2017 Community 重新编译 dll 文件。
