## IMESupport

![sublime.png](img/sublime.png)

Sublime Text 中文输入法支持，解决输入法候选框无法光标跟随问题。

## 须知

### Sublime Text 版本

现在代码精简，光标位置的获得严重依赖 ST 的 api: text_to_window()，所以请使用 ST
最新版本（3.0+ BUILD 3143+)

### Windows 系统版本和高清屏

Windows 10 支持最好（Windows 新的 API 支持获取高清屏的缩放比例），非 Windows 10
系统也支持，但是如果屏幕缩放不是 100%，需要手动设置: imesupport_screen_scaling
的值.

## 安装

注意：这是 [chikatoike/IMESupport](https://github.com/chikatoike/IMESupport) 的
fork 版本，所以不要使用 package_control 安装。

直接将该项目克隆或是下载到 Sublime_Text_3_Install_Path/Data/Packages 目录下 (或
者使用菜单 Perference > Browse Packages 打开)。


## ChangeLog

### 2018-11-25
* 简化代码，放弃支持 Sublime Text 老版本 (Sublime Text 3.0+ 起)
* 兼容 Windows 7 +
* 兼容 搜狗拼音、QQ 拼音

### 2017-07-31
* 新版本 (dev build 3142) 可以使用 View.text_to_view 获取光标的位置。
* 使用 Visual Studio 2017 Community 重新编译 dll 文件。
