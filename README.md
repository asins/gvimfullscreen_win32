gvimfullscreen.dll是个相当全的东西，能让VIM全屏、透明、总在最前功能，在vimrc中设置如下则可使用

``` vim
" {{{ Win平台下窗口全屏组件 gvimfullscreen.dll
" Alt + Enter 全屏切换
" Shift + t 降低窗口透明度
" Shift + y 加大窗口透明度
" Shift + r 切换Vim是否总在最前面显示
" Vim启动的时候自动使用当前颜色的背景色以去除Vim的白色边框
if has('gui_running') && has('gui_win32') && has('libcall')
    let g:MyVimLib = 'gvimfullscreen.dll'
    function! ToggleFullScreen()
        call libcall(g:MyVimLib, 'ToggleFullScreen', 1)
    endfunction

    let g:VimAlpha = 245
    function! SetAlpha(alpha)
        let g:VimAlpha = g:VimAlpha + a:alpha
        if g:VimAlpha < 180
            let g:VimAlpha = 180
        endif
        if g:VimAlpha > 255
            let g:VimAlpha = 255
        endif
        call libcall(g:MyVimLib, 'SetAlpha', g:VimAlpha)
    endfunction

    let g:VimTopMost = 0
    function! SwitchVimTopMostMode()
        if g:VimTopMost == 0
            let g:VimTopMost = 1
        else
            let g:VimTopMost = 0
        endif
        call libcall(g:MyVimLib, 'EnableTopMost', g:VimTopMost)
    endfunction
    "映射 Alt+Enter 切换全屏vim
    map <a-enter> <esc>:call ToggleFullScreen()<cr>
    "切换Vim是否在最前面显示
    nmap <s-r> <esc>:call SwitchVimTopMostMode()<cr>
    "增加Vim窗体的不透明度
    nmap <s-t> <esc>:call SetAlpha(10)<cr>
    "增加Vim窗体的透明度
    nmap <s-y> <esc>:call SetAlpha(-10)<cr>
    " 默认设置透明
    autocmd GUIEnter * call libcallnr(g:MyVimLib, 'SetAlpha', g:VimAlpha)
endif
" }}}
```

## 更新记录

#### Update 2013-03-26

注释掉了112行 `SetWindowLong(hTextArea, GWL_EXSTYLE, GetWindowLong(hTextArea, GWL_EXSTYLE) | WS_EX_CLIENTEDGE);`，修改过的VIM源码不需要这设置。

**注意：**如果不使用修改过后的gvim.exe，在编译时请加上这一行！

### 感谢

源码的改进来自小秦努力，感谢！

gVimWin32FullScreen
                                     Derek McLoughlin

* What is this
  Put gvim into full screen in Microsoft Windows.
  This is a copy of Yasuhiro Matsumoto's VimTweak's EnableMaximize
  functionality	modified to deal with window borders and making the
  window overlap the task bar.

  Note: this works on dual monitor configurations where the primary monitor is the
  one on the left hand side. When the gvim window spans both monitors, it is maximised 
  to the monitor which contains most of the area of gvim's window.

* Compiling
  Compile the DLL using any version of Visual C++ for Windows
  Use the makefile - nmake
  copy gvimfullscreen.dll to the directory that has gvim.exe

* Usage
    Toggle
    :call libcallnr("gvimfullscreen.dll", "ToggleFullScreen", 0)

	For  example:
	map <F11> <Esc>:call libcallnr("gvimfullscreen.dll", "ToggleFullScreen", 0)<CR>
