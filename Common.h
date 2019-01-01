;params, wich are usualy initialized are placed at the top of the list

struct RECTWH
        left     dd     ?
        top      dd     ?
        width    dd     ?
        height   dd     ?
ends

struct OLDDEVICECONTEXT
        hOldBrush       dd      ?
        hOldPen         dd      ?
        hOldBitmap      dd      ?
        hOldFont        dd      ?
ends

struct BITMAPBUTTONINFO
        bmpResourceName db      _resourceNameLength dup (0)
        text            db      _buttonTextLength   dup (0)
        visible         dd      0
        originalRect    RECTWH
        realRect        RECTWH
        hwndParent      dd      ?
        hdc             dd      ?
        bm              BITMAP
        oldDC           OLDDEVICECONTEXT
ends

struct BGINFO
        resourceName    db      _resourceNameLength dup (0)
        hdc             dd      ?
        oldDC           OLDDEVICECONTEXT
        bm              BITMAP
ends

struct BUFFERINFO
        hdc             dd      ?
        oldDC           OLDDEVICECONTEXT
        rect            RECTWH
ends

struct FONTINFO
        height          dd      -54
        faceName        db      20 dup 0
        color           dd      $00000000
        width           dd      0
        escapement      dd      0
        orientation     dd      0
        weight          dd      FW_DONTCARE
        italic          dd      FALSE
        underline       dd      FALSE
        strikeOut       dd      FALSE
        charSet         dd      ANSI_CHARSET
        outPrecision    dd      OUT_DEFAULT_PRECIS
        clipPrecision   dd      CLIP_DEFAULT_PRECIS
        quality         dd      DEFAULT_QUALITY
        pitch           dd      DEFAULT_PITCH or FF_ROMAN

ends

struct WINDOWINFO
        ClassName       db      _wndNameLength dup (0)
        WindowName      db      _wndNameLength dup (0)
        BG              BGINFO
        RealRect        RECTWH
        Font            FONTINFO
        Handle          dd      ?
        Buffer          BUFFERINFO
        PS              PAINTSTRUCT
ends

;BOOLEANS
TRUE            =       1
FALSE           =       0

; window settings
_wndWidth               =       1024
_wndHeight              =       768
_wndX                   =       0
_wndY                   =       0
_wndNameLength          =       30

;resource settings
_resourceNameLength     =       30

;common sizes
_bigButtonWidth         =       330
_bigButtonHeight        =       130

_squareButtonWidth      =       130
_squareButtonHeight     =       160

_buttonTextLength       =       30

;program states
_psMainWindow   =       1001
_psChooseWindow =       1002
_psPlayWindow   =       1003

; messages
_wmSwitchWindows  =     WM_USER + 1

; animation
AW_SPEED                =       500             ;in miliseconds

; string buffer
_stringBufferSize       =       100

AW_CURRENTANIMATION     =       AW_SLIDE xor AW_VER_POSITIVE
AW_CURRENTANIMATION     =       AW_CENTER
AW_HOR_POSITIVE         =       0x00000001
AW_HOR_NEGATIVE         =       0x00000002
AW_VER_POSITIVE         =       0x00000004
AW_VER_NEGATIVE         =       0x00000008
AW_CENTER               =       0x00000010
AW_HIDE                 =       0x00010000
AW_ACTIVATE             =       0x00020000
AW_SLIDE                =       0x00040000
AW_BLEND                =       0x00080000

; Games Modes
_gmCharacter    =       5001
_gmScribble     =       5002
_gmCopy         =       5003
_gmEnvironment  =       5004
_gmMix          =       5005
_gmIncremental  =       5006

_gmMinIndex     =       _gmCharacter
_gmMaxIndex     =       _gmIncremental

;Colors
clrBlack        =       $00000000
clrWhite        =       $00FFFFFF
clrSome         =       $00baa9a5
