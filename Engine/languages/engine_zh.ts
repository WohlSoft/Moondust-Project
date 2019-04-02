<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name></name>
    <message id="MSGBOX_INFO">
        <location filename="../gui/pge_msgbox.cpp" line="242"/>
        <source>Information</source>
        <translation>信息</translation>
    </message>
    <message id="MSGBOX_WARN">
        <location filename="../gui/pge_msgbox.cpp" line="257"/>
        <source>Warning</source>
        <translation>警告</translation>
    </message>
    <message id="MSGBOX_ERROR">
        <location filename="../gui/pge_msgbox.cpp" line="272"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message id="MSGBOX_FATAL">
        <location filename="../gui/pge_msgbox.cpp" line="287"/>
        <source>Fatal error</source>
        <translation>错误</translation>
    </message>
    <message id="CONFIG_SELECT">
        <location filename="../data_configs/config_select_scene/scene_config_select.cpp" line="60"/>
        <source>Choose a game:</source>
        <translation>选择游戏：</translation>
    </message>
    <message id="ERR_CONFIG_DIR_MISSING_TTL">
        <location filename="../data_configs/config_manager.cpp" line="115"/>
        <source>Config pack directory error</source>
        <translation>配置包目录错误</translation>
    </message>
    <message id="ERR_CONFIG_DIR_MISSING">
        <location filename="../data_configs/config_manager.cpp" line="117"/>
        <source>Config directory is not found at:
%1</source>
        <translation>未找到配置目录：
%1</translation>
    </message>
    <message id="ERR_CONFIG_ERROR_MAININI_TTL">
        <location filename="../data_configs/config_manager.cpp" line="127"/>
        <source>Missing main.ini</source>
        <translation>未找到 main.ini</translation>
    </message>
    <message id="ERR_CONFIG_ERROR_MAININI_MSG">
        <location filename="../data_configs/config_manager.cpp" line="129"/>
        <source>Can&apos;t open the &apos;main.ini&apos; config file!</source>
        <translation>打开文件“main.ini”失败！</translation>
    </message>
    <message id="ERR_CONFIG_ERROR_DATADIR_TTL">
        <location filename="../data_configs/config_manager.cpp" line="150"/>
        <source>Missing config pack data directory</source>
        <translation>未找到配置包数据路径</translation>
    </message>
    <message id="ERR_CONFIG_ERROR_DATADIR_MSG">
        <location filename="../data_configs/config_manager.cpp" line="152"/>
        <source>Config pack data path not exists:
%1</source>
        <translation>配置包数据路径不存在：
%1</translation>
    </message>
    <message id="WARNING_LEGACY_CONFIG_PACK_TTL">
        <location filename="../data_configs/config_manager.cpp" line="183"/>
        <source>Legacy configuration package</source>
        <translation>配置包过旧</translation>
    </message>
    <message id="WARNING_LEGACY_CONFIG_PACK">
        <location filename="../data_configs/config_manager.cpp" line="190"/>
        <source>You have a legacy configuration package.
Game will be started, but you may have a some problems with gameplay.

Please download and install latest version of a configuration package:

Download: %1

Note: most of config packs are updates togeter with PGE,
therefore you can use same link to get updated version.</source>
        <translation>您的配置包版本过旧。
&lt;br&gt;编辑器即将启动，但您会在对象和设置中遇到问题。
&lt;br&gt;
&lt;br&gt;请下载并安装最新版本的配置包：
&lt;br&gt;
&lt;br&gt;下载： %1
&lt;br&gt;注：部分配置包和PGE同步更新，
&lt;br&gt;确保可以在同一链接里下载到新版本。</translation>
    </message>
    <message id="ERROR_CONFIG_MAININI_FAILED_TTL">
        <location filename="../data_configs/config_manager.cpp" line="236"/>
        <source>main.ini is invalid</source>
        <translation>错误的 main.ini 文件</translation>
    </message>
    <message id="ERROR_CONFIG_MAININI_FAILED_MSG">
        <location filename="../data_configs/config_manager.cpp" line="238"/>
        <source>Error has occouped in main.ini in line %1.</source>
        <translation>main.ini 在第 %1 行出现错误。</translation>
    </message>
    <message id="CRASH_UNHEXC_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="290"/>
        <source>Unhandled exception!</source>
        <translation>未处理异常！</translation>
    </message>
    <message id="CRASH_UNHEXC_MSG">
        <location filename="../common_features/crash_handler.cpp" line="292"/>
        <source>Engine has crashed because accepted unhandled exception!</source>
        <translation>发生未处理异常，因此引擎已崩溃！</translation>
    </message>
    <message id="CRASH_OUT_OF_MEM_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="304"/>
        <source>Out of memory!</source>
        <translation>内存不足！</translation>
    </message>
    <message id="CRASH_OUT_OF_MEM_MSG">
        <location filename="../common_features/crash_handler.cpp" line="306"/>
        <source>Engine has crashed because out of memory! Try to close other applications and restart game.</source>
        <translation>由于内存不足，因此引擎已崩溃！请关闭其他应用再重新开始游戏。</translation>
    </message>
    <message id="CRASH_TIMEOUT_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="342"/>
        <source>Time out!</source>
        <translation>超时！</translation>
    </message>
    <message id="CRASH_TIMEOUT_MSG">
        <location filename="../common_features/crash_handler.cpp" line="344"/>
        <source>Engine has abourted because alarm() time out!</source>
        <translation>由于 alarm() 超时，因此引擎已关闭！</translation>
    </message>
    <message id="CRASH_BUS_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="389"/>
        <source>Physical memory address error!</source>
        <translation>物理内存地址错误！</translation>
    </message>
    <message id="CRASH_BUS_MSG">
        <location filename="../common_features/crash_handler.cpp" line="391"/>
        <source>Engine has crashed because a physical memory address error</source>
        <translation>由于物理内存地址错误，因此引擎已崩溃</translation>
    </message>
    <message id="CRASH_ILL_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="408"/>
        <source>Wrong CPU Instruction!</source>
        <translation>CPU 命令错误！</translation>
    </message>
    <message id="CRASH_ILL_MSG">
        <location filename="../common_features/crash_handler.cpp" line="410"/>
        <source>Engine has crashed because a wrong CPU instruction</source>
        <translation>由于 CPU 命令错误，因此引擎已崩溃</translation>
    </message>
    <message id="CRASH_FPE_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="465"/>
        <source>Wrong arithmetical operation</source>
        <translation>算法错误</translation>
    </message>
    <message id="CRASH_FPE_MSG">
        <location filename="../common_features/crash_handler.cpp" line="467"/>
        <source>Engine has crashed because of a wrong arithmetical operation!</source>
        <translation>由于算法错误，因此引擎已崩溃！</translation>
    </message>
    <message id="CRASH_ABORT_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="479"/>
        <source>Aborted</source>
        <translation>异常终止</translation>
    </message>
    <message id="CRASH_ABORT_TITLE.">
        <location filename="../common_features/crash_handler.cpp" line="481"/>
        <source>Engine has been aborted because critical error was occouped.</source>
        <translation>由于致命错误，因此引擎异常终止。</translation>
    </message>
    <message id="CRASH_SIGSEGV_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="526"/>
        <source>Segmentation fault</source>
        <translation>段落错误</translation>
    </message>
    <message id="CRASH_SIGSEGV_MSG.">
        <location filename="../common_features/crash_handler.cpp" line="530"/>
        <source>Engine has crashed because of a Segmentation fault.
Run debugging with a built in debug mode application
and retry your recent actions to get more detailed information.</source>
        <translation>由于段落错误，因此引擎已崩溃。
请使用内置调试器进行调试并对最近的操作
进行重试以获得更多的详细信息。</translation>
    </message>
    <message id="CRASH_INT_TITLE">
        <location filename="../common_features/crash_handler.cpp" line="539"/>
        <source>Interrupt</source>
        <translation>中断</translation>
    </message>
    <message id="CRASH_INT_MSG">
        <location filename="../common_features/crash_handler.cpp" line="541"/>
        <source>Engine has been interrupted</source>
        <translation>引擎已损坏</translation>
    </message>
    <message id="GAMEOVER_TITLE">
        <location filename="../scenes/scene_gameover.cpp" line="54"/>
        <source>Game Over!</source>
        <translation>游戏结束！</translation>
    </message>
    <message id="GAMEOVER_CONTINUE">
        <location filename="../scenes/scene_gameover.cpp" line="60"/>
        <source>Continue</source>
        <translation>继续</translation>
    </message>
    <message id="GAMEOVER_EXIT">
        <location filename="../scenes/scene_gameover.cpp" line="62"/>
        <source>Quit</source>
        <translation>退出</translation>
    </message>
    <message id="LVL_MENU_PAUSE_TTL">
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="31"/>
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="60"/>
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="87"/>
        <source>Pause</source>
        <translation>暂停</translation>
    </message>
    <message id="LVL_MENU_PAUSE_CONTINUE">
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="40"/>
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="69"/>
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="95"/>
        <source>Continue</source>
        <translation>继续</translation>
    </message>
    <message id="LVL_MENU_PAUSE_CONTINUESAVE">
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="42"/>
        <source>Save and continue</source>
        <translation>保存并继续</translation>
    </message>
    <message id="LVL_MENU_PAUSE_EXITSAVE">
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="44"/>
        <source>Save and quit</source>
        <translation>保存并退出</translation>
    </message>
    <message id="LVL_MENU_PAUSE_EXITNOSAVE">
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="46"/>
        <source>Exit without saving</source>
        <translation>退出而不保存</translation>
    </message>
    <message id="LVL_MENU_PAUSE_EXIT">
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="71"/>
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="99"/>
        <source>Quit</source>
        <translation>退出</translation>
    </message>
    <message id="LVL_MENU_PAUSE_REPLAY">
        <location filename="../scenes/level/lvl_scene_pausemenu.cpp" line="97"/>
        <source>Replay level</source>
        <translation>重新开始关卡</translation>
    </message>
    <message id="ERROR_LVL_UNKNOWN_PL_CHARACTER">
        <location filename="../scenes/level/lvl_scene_init.cpp" line="298"/>
        <location filename="../scenes/scene_world.cpp" line="550"/>
        <source>Invalid playable character ID</source>
        <translation>错误的人物编号</translation>
    </message>
    <message id="ERROR_LVL_UNKNOWN_PL_STATE">
        <location filename="../scenes/level/lvl_scene_init.cpp" line="307"/>
        <location filename="../scenes/scene_world.cpp" line="559"/>
        <source>Invalid playable character state ID</source>
        <translation>错误的人物形态编号</translation>
    </message>
    <message id="LVL_ERROR_NOSECTIONS">
        <location filename="../scenes/level/lvl_scene_init.cpp" line="432"/>
        <source>Fatal error: Impossible to find start section.
Did you placed player start point (or entrance warp point) too far off of the section(s)?</source>
        <translation>错误：无法找到起点。
请问你将起点（或者传送入口）放置到了场景外的过大范围吗？</translation>
    </message>
    <message id="WLD_MENU_PAUSE_TTL">
        <location filename="../scenes/scene_world.cpp" line="611"/>
        <location filename="../scenes/scene_world.cpp" line="638"/>
        <source>Pause</source>
        <translation>暂停</translation>
    </message>
    <message id="WLD_MENU_PAUSE_CONTINUE">
        <location filename="../scenes/scene_world.cpp" line="619"/>
        <location filename="../scenes/scene_world.cpp" line="646"/>
        <source>Continue</source>
        <translation>继续</translation>
    </message>
    <message id="WLD_MENU_PAUSE_CONTINUESAVE">
        <location filename="../scenes/scene_world.cpp" line="621"/>
        <source>Save and continue</source>
        <translation>保存并继续</translation>
    </message>
    <message id="WLD_MENU_PAUSE_EXITSAVE">
        <location filename="../scenes/scene_world.cpp" line="623"/>
        <source>Save and quit</source>
        <translation>保存并退出</translation>
    </message>
    <message id="WLD_MENU_PAUSE_EXITNOSAVE">
        <location filename="../scenes/scene_world.cpp" line="625"/>
        <source>Exit without saving</source>
        <translation>退出而不保存</translation>
    </message>
    <message id="WLD_MENU_PAUSE_EXIT">
        <location filename="../scenes/scene_world.cpp" line="648"/>
        <source>Quit</source>
        <translation>退出</translation>
    </message>
    <message id="MAINMENU_1_PLAYER_GAME">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="360"/>
        <source>1 Player Game</source>
        <translation>单人游戏</translation>
    </message>
    <message id="MAINMENU_2_PLAYER_GAME">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="362"/>
        <source>2 Player Game</source>
        <translation>双人游戏</translation>
    </message>
    <message id="MAINMENU_PLAY_SINGLE_LEVEL">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="364"/>
        <source>Play level</source>
        <translation>关卡模式</translation>
    </message>
    <message id="MAINMENU_OPTIONS">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="366"/>
        <source>Options</source>
        <translation>设置</translation>
    </message>
    <message id="MAINMENU_EXIT">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="368"/>
        <source>Exit</source>
        <translation>退出游戏</translation>
    </message>
    <message id="MAINMENU_OPTIONS_TEST_SCRS">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="374"/>
        <source>Test of screens</source>
        <translation>屏幕测试</translation>
    </message>
    <message id="MAINMENU_OPTIONS_TEST_MSGS">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="376"/>
        <source>Test of message boxes</source>
        <translation>信息框测试</translation>
    </message>
    <message id="MAINMENU_OPTIONS_CONTROLLS">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="378"/>
        <source>Player controlling</source>
        <translation>玩家控制</translation>
    </message>
    <message id="MAINMENU_OPTIONS_VIDEO">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="380"/>
        <source>Video settings</source>
        <translation>图像设置</translation>
    </message>
    <message id="MAINMENU_OPTIONS_MUS_VOL">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="382"/>
        <source>Music volume</source>
        <translation>音乐音量</translation>
    </message>
    <message id="MAINMENU_OPTIONS_SND_VOL">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="385"/>
        <source>Sound volume</source>
        <translation>音效音量</translation>
    </message>
    <message id="MAINMENU_OPTIONS_FULLSCR">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="388"/>
        <source>Full Screen mode</source>
        <translation>全屏</translation>
    </message>
    <message id="TESTSCR_CREDITS">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="396"/>
        <source>Credits</source>
        <translation>作者</translation>
    </message>
    <message id="TESTSCR_LOADING_SCREEN">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="398"/>
        <source>Loading screen</source>
        <translation>加载界面</translation>
    </message>
    <message id="TESTSCR_GAMEOVER">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="400"/>
        <source>Game over screen</source>
        <translation>游戏结束界面</translation>
    </message>
    <message id="TEST_MSGBOX">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="406"/>
        <source>Message box</source>
        <translation>信息框</translation>
    </message>
    <message id="TEST_MENUBOX">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="408"/>
        <source>Menu box</source>
        <translation>菜单框</translation>
    </message>
    <message id="TEST_TEXTINPUTBOX">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="410"/>
        <source>Text Input box</source>
        <translation>文本输入框</translation>
    </message>
    <message id="TEST_QUESTION_BOX">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="412"/>
        <source>Question box</source>
        <translation>问号框</translation>
    </message>
    <message id="VIDEO_SHOW_DEBUG">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="418"/>
        <source>Show debug info</source>
        <translation>显示调试信息</translation>
    </message>
    <message id="VIDEO_ENABLE_FRSKIP">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="420"/>
        <source>Enable frame-skip</source>
        <translation>启用跳帧</translation>
    </message>
    <message id="VIDEO_ENABLE_VSYNC">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="422"/>
        <source>Enable V-Sync</source>
        <translation>启用垂直同步</translation>
    </message>
    <message id="VIDEO_FRAME_TIME">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="434"/>
        <source>Frame time (ms.)</source>
        <translation>帧数（毫秒）</translation>
    </message>
    <message id="CONTROLLS_SETUP_PLAYER1">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="452"/>
        <source>Player 1 controls</source>
        <translation>玩家 1 按键控制</translation>
    </message>
    <message id="CONTROLLS_SETUP_PLAYER2">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="454"/>
        <source>Player 2 controls</source>
        <translation>玩家 2 按键控制</translation>
    </message>
    <message id="PLAYER_CONTROLS_SETUP_KEYBOARD">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="504"/>
        <source>Keyboard</source>
        <translation>键盘</translation>
    </message>
    <message id="PLAYER_CONTROLS_SETUP_JOYSTICK">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="511"/>
        <source>Joystick: %1</source>
        <translation>手柄： %1</translation>
    </message>
    <message id="PLAYER_CONTROLS_SETUP_INPUT_TYPE">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="515"/>
        <source>Input:</source>
        <translation>输入：</translation>
    </message>
    <message id="MSG_PLEASEWAIT">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="545"/>
        <location filename="../scenes/title/ttl_main_menu.cpp" line="575"/>
        <source>Please wait...</source>
        <translation>请稍候……</translation>
    </message>
    <message id="MSG_EPISODES_NOT_FOUND">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="662"/>
        <source>&lt;episodes not found&gt;</source>
        <translation>&lt;未找到作品&gt;</translation>
    </message>
    <message id="MSG_LEVELS_NOT_FOUND">
        <location filename="../scenes/title/ttl_main_menu.cpp" line="713"/>
        <source>&lt;levels not found&gt;</source>
        <translation>&lt;未找到关卡&gt;</translation>
    </message>
    <message id="SDL_INIT_ERROR">
        <location filename="../main.cpp" line="116"/>
        <source>Unable to init SDL!</source>
        <translation>SDL 初始化失败！</translation>
    </message>
    <message id="ERROR_NO_CONFIG_PACKS_TTL">
        <location filename="../main.cpp" line="177"/>
        <source>Config packs not found</source>
        <translation>未找到配置包</translation>
    </message>
    <message id="ERROR_NO_CONFIG_PACKS_TEXT">
        <location filename="../main.cpp" line="180"/>
        <source>Can&apos;t start game, because available
configuration packages are not found!</source>
        <translation>没有可用的配置包，因此无法启动游戏！</translation>
    </message>
    <message id="CONFIG_SELECT_TEST">
        <location filename="../main.cpp" line="190"/>
        <source>Choose a game to test:</source>
        <translation>选择游戏进行测试：</translation>
    </message>
    <message id="ERROR_NO_OPEN_FILES_MSG">
        <location filename="../main.cpp" line="417"/>
        <location filename="../main.cpp" line="561"/>
        <source>No opened files</source>
        <translation>无已打开文件</translation>
    </message>
    <message id="ERROR_FAIL_START_WLD">
        <location filename="../main.cpp" line="432"/>
        <source>ERROR:
Fail to start world map

%1</source>
        <translation>错误：
无法启动大地图。
%1</translation>
    </message>
    <message id="WLD_ERROR_LVLCLOSED">
        <location filename="../main.cpp" line="450"/>
        <source>World map was closed with error.
%1</source>
        <translation>由于大地图有错误，因此大地图已关闭。
%1</translation>
    </message>
    <message id="MSG_START_LEVEL">
        <location filename="../main.cpp" line="467"/>
        <source>Start level
%1</source>
        <translation>启动关卡
%1</translation>
    </message>
    <message id="MSG_WLDTEST_EXIT_CODE">
        <location filename="../main.cpp" line="469"/>
        <source>Type an exit code (signed integer)</source>
        <translation>输入出口代码：（整数）</translation>
    </message>
    <message id="LVL_EXIT_WARP_INFO">
        <location filename="../main.cpp" line="639"/>
        <source>Warp exit

Exit into:
%1

Entrance point: %2</source>
        <translation>传送出口

出口到：
%1

入口点 #：%2</translation>
    </message>
    <message id="LVL_ERROR_LVLCLOSED">
        <location filename="../main.cpp" line="698"/>
        <source>Level was closed with error.
%1</source>
        <translation>由于关卡有错误，因此关卡已关闭。
%1</translation>
    </message>
    <message id="VSYNC_ERROR_INFO">
        <location filename="../graphics/vsync_validator.cpp" line="70"/>
        <source>Detected framerate is too high. V-Sync will be disabled to prevent problematic results. Please enable V-Sync in your video driver.</source>
        <translation>检测到帧率过高，为防止出现问题，垂直同步已关闭。请在你的显卡上开启垂直同步。</translation>
    </message>
    <message id="RENDERER_NO_OPENGL_ERROR">
        <location filename="../graphics/window.cpp" line="167"/>
        <source>Unable to find OpenGL support!
Software renderer will be started.</source>
        <translation>您的显卡不支持 OpenGL！
将使用软件渲染。</translation>
    </message>
    <message id="NO_RENDERER_ERROR">
        <location filename="../graphics/window.cpp" line="189"/>
        <source>Renderer is not selected!</source>
        <translation>未选择渲染环境！</translation>
    </message>
    <message id="WINDOW_CREATE_ERROR">
        <location filename="../graphics/window.cpp" line="221"/>
        <location filename="../graphics/window.cpp" line="229"/>
        <source>Unable to create window!</source>
        <translation>创建窗口失败！</translation>
    </message>
    <message id="WINDOW_ICON_INIT_ERROR">
        <location filename="../graphics/window.cpp" line="251"/>
        <location filename="../graphics/window.cpp" line="258"/>
        <location filename="../graphics/window.cpp" line="283"/>
        <source>Unable to setup window icon!</source>
        <translation>设置窗口图标失败！</translation>
    </message>
    <message id="RENDERER_CONTEXT_INIT_ERROR">
        <location filename="../graphics/window.cpp" line="296"/>
        <source>Unable to initialize renderer context!</source>
        <translation>渲染环境初始化失败！</translation>
    </message>
</context>
</TS>
