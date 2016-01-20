Attribute VB_Name = "SDL2"
Option Explicit

Const SIZEOF_VOIDP As Long = 8
Const HAVE_GCC_ATOMICS As Long = 1
Const HAVE_LIBC As Long = 1
Const HAVE_SYS_TYPES_H As Long = 1
Const HAVE_STDIO_H As Long = 1
Const STDC_HEADERS As Long = 1
Const HAVE_STDLIB_H As Long = 1
Const HAVE_STDARG_H As Long = 1
Const HAVE_MALLOC_H As Long = 1
Const HAVE_MEMORY_H As Long = 1
Const HAVE_STRING_H As Long = 1
Const HAVE_STRINGS_H As Long = 1
Const HAVE_INTTYPES_H As Long = 1
Const HAVE_STDINT_H As Long = 1
Const HAVE_CTYPE_H As Long = 1
Const HAVE_MATH_H As Long = 1
Const HAVE_ICONV_H As Long = 1
Const HAVE_SIGNAL_H As Long = 1
Const HAVE_MALLOC As Long = 1
Const HAVE_CALLOC As Long = 1
Const HAVE_REALLOC As Long = 1
Const HAVE_FREE As Long = 1
Const HAVE_GETENV As Long = 1
Const HAVE_PUTENV As Long = 1
Const HAVE_QSORT As Long = 1
Const HAVE_ABS As Long = 1
Const HAVE_MEMSET As Long = 1
Const HAVE_MEMCPY As Long = 1
Const HAVE_MEMMOVE As Long = 1
Const HAVE_MEMCMP As Long = 1
Const HAVE_STRLEN As Long = 1
Const HAVE_STRDUP As Long = 1
Const HAVE__STRREV As Long = 1
Const HAVE__STRUPR As Long = 1
Const HAVE__STRLWR As Long = 1
Const HAVE_STRCHR As Long = 1
Const HAVE_STRRCHR As Long = 1
Const HAVE_STRSTR As Long = 1
Const HAVE_ITOA As Long = 1
Const HAVE__LTOA As Long = 1
Const HAVE__ULTOA As Long = 1
Const HAVE_STRTOL As Long = 1
Const HAVE_STRTOUL As Long = 1
Const HAVE__I64TOA As Long = 1
Const HAVE__UI64TOA As Long = 1
Const HAVE_STRTOLL As Long = 1
Const HAVE_STRTOULL As Long = 1
Const HAVE_STRTOD As Long = 1
Const HAVE_ATOI As Long = 1
Const HAVE_ATOF As Long = 1
Const HAVE_STRCMP As Long = 1
Const HAVE_STRNCMP As Long = 1
Const HAVE__STRICMP As Long = 1
Const HAVE_STRCASECMP As Long = 1
Const HAVE__STRNICMP As Long = 1
Const HAVE_STRNCASECMP As Long = 1
Const HAVE_VSSCANF As Long = 1
Const HAVE_VSNPRINTF As Long = 1
Const HAVE_ATAN As Long = 1
Const HAVE_ATAN2 As Long = 1
Const HAVE_ACOS As Long = 1
Const HAVE_ASIN As Long = 1
Const HAVE_CEIL As Long = 1
Const HAVE_COPYSIGN As Long = 1
Const HAVE_COS As Long = 1
Const HAVE_COSF As Long = 1
Const HAVE_FABS As Long = 1
Const HAVE_FLOOR As Long = 1
Const HAVE_LOG As Long = 1
Const HAVE_POW As Long = 1
Const HAVE_SCALBN As Long = 1
Const HAVE_SIN As Long = 1
Const HAVE_SINF As Long = 1
Const HAVE_SQRT As Long = 1
Const HAVE_FSEEKO As Long = 1
Const HAVE_FSEEKO64 As Long = 1
Const HAVE_NANOSLEEP As Long = 1
Const SDL_AUDIO_DRIVER_DISK As Long = 1
Const SDL_AUDIO_DRIVER_DUMMY As Long = 1
Const SDL_AUDIO_DRIVER_DSOUND As Long = 1
Const SDL_AUDIO_DRIVER_WINMM As Long = 1
Const SDL_JOYSTICK_DINPUT As Long = 1
Const SDL_HAPTIC_DINPUT As Long = 1
Const SDL_LOADSO_WINDOWS As Long = 1
Const SDL_THREAD_WINDOWS As Long = 1
Const SDL_TIMER_WINDOWS As Long = 1
Const SDL_VIDEO_DRIVER_DUMMY As Long = 1
Const SDL_VIDEO_DRIVER_WINDOWS As Long = 1
Const SDL_VIDEO_RENDER_D3D As Long = 1
Const SDL_VIDEO_RENDER_OGL As Long = 1
Const SDL_VIDEO_OPENGL As Long = 1
Const SDL_VIDEO_OPENGL_WGL As Long = 1
Const SDL_POWER_WINDOWS As Long = 1
Const SDL_FILESYSTEM_WINDOWS As Long = 1
Const SDL_ASSEMBLY_ROUTINES As Long = 1
Const SEEK_SET As Long = 0
Const SEEK_CUR As Long = 1
Const SEEK_END As Long = 2
Const TMP_MAX As Long = 238328
Const FILENAME_MAX As Long = 4096
Const FOPEN_MAX As Long = 16
Const WNOHANG As Long = 1
Const WUNTRACED As Long = 2
Const WSTOPPED As Long = 2
Const WEXITED As Long = 4
Const WCONTINUED As Long = 8
Const WNOWAIT As Long = &H1000000
Const RAND_MAX As Long = 2147483647
Const EXIT_FAILURE As Long = 1
Const EXIT_SUCCESS As Long = 0
Const PRIX8 As String = "X"
Const PRIX16 As String = "X"
Const PRIX32 As String = "X"
Const PRIXLEAST8 As String = "X"
Const PRIXLEAST16 As String = "X"
Const PRIXLEAST32 As String = "X"
Const PRIXFAST8 As String = "X"
Const FP_NAN As Long = 0
Const FP_INFINITE As Long = 1
Const FP_ZERO As Long = 2
Const FP_SUBNORMAL As Long = 3
Const FP_NORMAL As Long = 4
Const MATH_ERRNO As Long = 1
Const MATH_ERREXCEPT As Long = 2
Const DOMAIN As Long = 1
Const SING As Long = 2
Const OVERFLOW As Long = 3
Const UNDERFLOW As Long = 4
Const TLOSS As Long = 5
Const PLOSS As Long = 6
'Const HUGE As Long = 3.40282347e+38F
Const M_E As Long = 2.71828182845905
Const M_LOG2E As Long = 1.44269504088896
Const M_LOG10E As Long = 0.434294481903252
Const M_LN2 As Long = 0.693147180559945
Const M_LN10 As Long = 2.30258509299405
Const M_PI As Long = 3.14159265358979
Const M_PI_2 As Long = 1.5707963267949
Const M_PI_4 As Long = 0.785398163397448
Const M_1_PI As Long = 0.318309886183791
Const M_2_PI As Long = 0.636619772367581
Const M_2_SQRTPI As Long = 1.12837916709551
Const M_SQRT2 As Long = 1.4142135623731
Const M_SQRT1_2 As Long = 0.707106781186548
Const SDL_ASSERT_LEVEL As Long = 2
Const SDL_LIL_ENDIAN As Long = 1234
Const SDL_BIG_ENDIAN As Long = 4321
Const SDL_MUTEX_TIMEDOUT As Long = 1
Const SDL_RWOPS_UNKNOWN As Long = 0
Const SDL_RWOPS_WINFILE As Long = 1
Const SDL_RWOPS_STDFILE As Long = 2
Const SDL_RWOPS_JNIFILE As Long = 3
Const SDL_RWOPS_MEMORY As Long = 4
Const SDL_RWOPS_MEMORY_RO As Long = 5
Const RW_SEEK_SET As Long = 0
Const RW_SEEK_CUR As Long = 1
Const RW_SEEK_END As Long = 2
Const AUDIO_U8 As Long = &H8
Const AUDIO_S8 As Long = &H8008
Const AUDIO_U16LSB As Long = &H10
Const AUDIO_S16LSB As Long = &H8010
Const AUDIO_U16MSB As Long = &H1010
Const AUDIO_S16MSB As Long = &H9010
Const AUDIO_S32LSB As Long = &H8020
Const AUDIO_S32MSB As Long = &H9020
Const AUDIO_F32LSB As Long = &H8120
Const AUDIO_F32MSB As Long = &H9120
Const SDL_AUDIO_ALLOW_FREQUENCY_CHANGE As Long = &H1
Const SDL_AUDIO_ALLOW_FORMAT_CHANGE As Long = &H2
Const SDL_AUDIO_ALLOW_CHANNELS_CHANGE As Long = &H4
Const SDL_MIX_MAXVOLUME As Long = 128
Const SDL_CACHELINE_SIZE As Long = 128
Const SDL_ALPHA_OPAQUE As Long = 255
Const SDL_ALPHA_TRANSPARENT As Long = 0
Const SDL_SWSURFACE As Long = 0
Const SDL_PREALLOC As Long = &H1
Const SDL_RLEACCEL As Long = &H2
Const SDL_DONTFREE As Long = &H4
Const SDL_WINDOWPOS_UNDEFINED_MASK As Long = &H1FFF0000
Const SDL_WINDOWPOS_CENTERED_MASK As Long = &H2FFF0000
Const SDL_BUTTON_LEFT As Long = 1
Const SDL_BUTTON_MIDDLE As Long = 2
Const SDL_BUTTON_RIGHT As Long = 3
Const SDL_BUTTON_X1 As Long = 4
Const SDL_BUTTON_X2 As Long = 5
Const SDL_HAT_CENTERED As Long = &H0
Const SDL_HAT_UP As Long = &H1
Const SDL_HAT_RIGHT As Long = &H2
Const SDL_HAT_DOWN As Long = &H4
Const SDL_HAT_LEFT As Long = &H8
Const SDL_RELEASED As Long = 0
Const SDL_PRESSED As Long = 1
Const SDL_IGNORE As Long = 0
Const SDL_DISABLE As Long = 0
Const SDL_ENABLE As Long = 1
Const SDL_HAPTIC_POLAR As Long = 0
Const SDL_HAPTIC_CARTESIAN As Long = 1
Const SDL_HAPTIC_SPHERICAL As Long = 2
'Const SDL_HAPTIC_INFINITY As Long = 4294967295U
Const SDL_HINT_FRAMEBUFFER_ACCELERATION As String = "SDL_FRAMEBUFFER_ACCELERATION"
Const SDL_HINT_RENDER_DRIVER As String = "SDL_RENDER_DRIVER"
Const SDL_HINT_RENDER_OPENGL_SHADERS As String = "SDL_RENDER_OPENGL_SHADERS"
Const SDL_HINT_RENDER_DIRECT3D_THREADSAFE As String = "SDL_RENDER_DIRECT3D_THREADSAFE"
Const SDL_HINT_RENDER_DIRECT3D11_DEBUG As String = "SDL_HINT_RENDER_DIRECT3D11_DEBUG"
Const SDL_HINT_RENDER_SCALE_QUALITY As String = "SDL_RENDER_SCALE_QUALITY"
Const SDL_HINT_RENDER_VSYNC As String = "SDL_RENDER_VSYNC"
Const SDL_HINT_VIDEO_ALLOW_SCREENSAVER As String = "SDL_VIDEO_ALLOW_SCREENSAVER"
Const SDL_HINT_VIDEO_X11_XVIDMODE As String = "SDL_VIDEO_X11_XVIDMODE"
Const SDL_HINT_VIDEO_X11_XINERAMA As String = "SDL_VIDEO_X11_XINERAMA"
Const SDL_HINT_VIDEO_X11_XRANDR As String = "SDL_VIDEO_X11_XRANDR"
Const SDL_HINT_GRAB_KEYBOARD As String = "SDL_GRAB_KEYBOARD"
Const SDL_HINT_MOUSE_RELATIVE_MODE_WARP As String = "SDL_MOUSE_RELATIVE_MODE_WARP"
Const SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS As String = "SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS"
Const SDL_HINT_IDLE_TIMER_DISABLED As String = "SDL_IOS_IDLE_TIMER_DISABLED"
Const SDL_HINT_ORIENTATIONS As String = "SDL_IOS_ORIENTATIONS"
Const SDL_HINT_ACCELEROMETER_AS_JOYSTICK As String = "SDL_ACCELEROMETER_AS_JOYSTICK"
Const SDL_HINT_XINPUT_ENABLED As String = "SDL_XINPUT_ENABLED"
Const SDL_HINT_GAMECONTROLLERCONFIG As String = "SDL_GAMECONTROLLERCONFIG"
Const SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS As String = "SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"
Const SDL_HINT_ALLOW_TOPMOST As String = "SDL_ALLOW_TOPMOST"
Const SDL_HINT_TIMER_RESOLUTION As String = "SDL_TIMER_RESOLUTION"
Const SDL_HINT_VIDEO_HIGHDPI_DISABLED As String = "SDL_VIDEO_HIGHDPI_DISABLED"
Const SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK As String = "SDL_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK"
Const SDL_HINT_VIDEO_WIN_D3DCOMPILER As String = "SDL_VIDEO_WIN_D3DCOMPILER"
Const SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT As String = "SDL_VIDEO_WINDOW_SHARE_PIXEL_FORMAT"
Const SDL_HINT_WINRT_PRIVACY_POLICY_URL As String = "SDL_HINT_WINRT_PRIVACY_POLICY_URL"
Const SDL_HINT_WINRT_PRIVACY_POLICY_LABEL As String = "SDL_HINT_WINRT_PRIVACY_POLICY_LABEL"
Const SDL_HINT_WINRT_HANDLE_BACK_BUTTON As String = "SDL_HINT_WINRT_HANDLE_BACK_BUTTON"
Const SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES As String = "SDL_VIDEO_MAC_FULLSCREEN_SPACES"
Const SDL_MAX_LOG_MESSAGE As Long = 4096
Const SDL_MAJOR_VERSION As Long = 2
Const SDL_MINOR_VERSION As Long = 0
Const SDL_PATCHLEVEL As Long = 3
Const SDL_INIT_TIMER As Long = &H1
Const SDL_INIT_AUDIO As Long = &H10
Const SDL_INIT_VIDEO As Long = &H20
Const SDL_INIT_JOYSTICK As Long = &H200
Const SDL_INIT_HAPTIC As Long = &H1000
Const SDL_INIT_GAMECONTROLLER As Long = &H2000
Const SDL_INIT_EVENTS As Long = &H4000
Const SDL_INIT_NOPARACHUTE As Long = &H100000

Public Type timespec
        tv_sec As Long
        tv_nsec As Long
End Type
Public Type timeval
        tv_sec As Long
        tv_usec As Long
End Type

Public Type random_data
        fptr As Long
        rptr As Long
        state As Long
        rand_type As Long
        rand_deg As Long
        rand_sep As Long
        end_ptr As Long
End Type

Public Type exception
        type As Long
        name As Long
        arg1 As Double
        arg2 As Double
        retval As Double
End Type

Public Type SDL_AudioSpec
        freq As Long
        format As Long
        channels As Long
        silence As Long
        samples As Long
        padding As Long
        size As Long
        callback As Long
        userdata As Long
End Type

Public Type SDL_Color
        r As Long
        g As Long
        b As Long
        a As Long
End Type
Public Type SDL_Palette
        ncolors As Long
        colors As Long
        version As Long
        refcount As Long
End Type

Public Type SDL_Point
        X As Long
        Y As Long
End Type
Public Type SDL_Rect
        X As Long
        w As Long
End Type
Public Type SDL_Surface
        flags As Long
        format As Long
        w As Long
        pitch As Long
        pixels As Long
        userdata As Long
        locked As Long
        lock_data As Long
        clip_rect As Long
        SDL_BlitMap As Long
        refcount As Long
End Type
Public Type SDL_Keysym
        scancode As Long
        sym As Long
        mod As Long
        unused As Long
End Type

Public Type SDL_Finger
        id As Long
        X As Long
        Y As Long
        pressure As Long
End Type
Public Type SDL_CommonEvent
        type As Long
        timestamp As Long
End Type
Public Type SDL_WindowEvent
        type As Long
        timestamp As Long
        windowID As Long
        event As Long
        padding1 As Long
        padding2 As Long
        padding3 As Long
        data1 As Long
        data2 As Long
End Type
Public Type SDL_KeyboardEvent
        type As Long
        timestamp As Long
        windowID As Long
        state As Long
        repeat As Long
        padding2 As Long
        padding3 As Long
        keysym As Long
End Type
Public Type SDL_MouseMotionEvent
        type As Long
        timestamp As Long
        windowID As Long
        which As Long
        state As Long
        X As Long
        Y As Long
        xrel As Long
        yrel As Long
End Type
Public Type SDL_MouseButtonEvent
        type As Long
        timestamp As Long
        windowID As Long
        which As Long
        button As Long
        state As Long
        clicks As Long
        padding1 As Long
        X As Long
        Y As Long
End Type
Public Type SDL_MouseWheelEvent
        type As Long
        timestamp As Long
        windowID As Long
        which As Long
        X As Long
        Y As Long
End Type
Public Type SDL_JoyAxisEvent
        type As Long
        timestamp As Long
        which As Long
        axis As Long
        padding1 As Long
        padding2 As Long
        padding3 As Long
        value As Long
        padding4 As Long
End Type
Public Type SDL_JoyBallEvent
        type As Long
        timestamp As Long
        which As Long
        ball As Long
        padding1 As Long
        padding2 As Long
        padding3 As Long
        xrel As Long
        yrel As Long
End Type
Public Type SDL_JoyHatEvent
        type As Long
        timestamp As Long
        which As Long
        hat As Long
        value As Long
        padding1 As Long
        padding2 As Long
End Type
Public Type SDL_JoyButtonEvent
        type As Long
        timestamp As Long
        which As Long
        button As Long
        state As Long
        padding1 As Long
        padding2 As Long
End Type
Public Type SDL_JoyDeviceEvent
        type As Long
        timestamp As Long
        which As Long
End Type
Public Type SDL_ControllerAxisEvent
        type As Long
        timestamp As Long
        which As Long
        axis As Long
        padding1 As Long
        padding2 As Long
        padding3 As Long
        value As Long
        padding4 As Long
End Type
Public Type SDL_ControllerButtonEvent
        type As Long
        timestamp As Long
        which As Long
        button As Long
        state As Long
        padding1 As Long
        padding2 As Long
End Type
Public Type SDL_ControllerDeviceEvent
        type As Long
        timestamp As Long
        which As Long
End Type
Public Type SDL_TouchFingerEvent
        type As Long
        timestamp As Long
        touchId As Long
        fingerId As Long
        X As Long
        Y As Long
        dx As Long
        dy As Long
        pressure As Long
End Type
Public Type SDL_MultiGestureEvent
        type As Long
        timestamp As Long
        touchId As Long
        dTheta As Long
        dDist As Long
        X As Long
        Y As Long
        numFingers As Long
        padding As Long
End Type
Public Type SDL_DollarGestureEvent
        type As Long
        timestamp As Long
        touchId As Long
        gestureId As Long
        numFingers As Long
        error As Long
        X As Long
        Y As Long
End Type
Public Type SDL_DropEvent
        type As Long
        timestamp As Long
        file As Long
End Type
Public Type SDL_QuitEvent
        type As Long
        timestamp As Long
End Type
Public Type SDL_OSEvent
        type As Long
        timestamp As Long
End Type
Public Type SDL_UserEvent
        type As Long
        timestamp As Long
        windowID As Long
        code As Long
        data1 As Long
        data2 As Long
End Type
Public Type SDL_HapticConstant
        type As Long
        direction As Long
        length As Long
        delay As Long
        button As Long
        interval As Long
        level As Long
        attack_length As Long
        attack_level As Long
        fade_length As Long
        fade_level As Long
End Type
Public Type SDL_HapticPeriodic
        type As Long
        direction As Long
        length As Long
        delay As Long
        button As Long
        interval As Long
        period As Long
        magnitude As Long
        offset As Long
        phase As Long
        attack_length As Long
        attack_level As Long
        fade_length As Long
        fade_level As Long
End Type
Public Type SDL_HapticRamp
        type As Long
        direction As Long
        length As Long
        delay As Long
        button As Long
        interval As Long
        start As Long
        end As Long
        attack_length As Long
        attack_level As Long
        fade_length As Long
        fade_level As Long
End Type
Public Type SDL_HapticLeftRight
        type As Long
        length As Long
        large_magnitude As Long
        small_magnitude As Long
End Type
Public Type SDL_HapticCustom
        type As Long
        direction As Long
        length As Long
        delay As Long
        button As Long
        interval As Long
        channels As Long
        period As Long
        samples As Long
        data As Long
        attack_length As Long
        attack_level As Long
        fade_length As Long
        fade_level As Long
End Type

Public Declare Function SDL_Init Lib "SDL2.dll" (ByVal flags As Long) As Long

Public Declare Function SDL_Quit Lib "SDL2.dll" ()

Public Declare Function SDL_GetError Lib "SDL2.dll" () As String

'SDL_RWops * SDL_RWFromFile(const char *file, const char *mode);
Public Declare Function SDL_RWFromFile Lib "SDL2.dll" (ByVal file As String, ByVal mode As String) As Long

