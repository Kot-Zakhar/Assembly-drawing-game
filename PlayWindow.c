;------------------------------------------------------------------------------
proc PlayWindow.Register \
     uses edx
        local wc: WNDCLASS

        mov     [wc.style], 0
        mov     [wc.lpfnWndProc], PlayWindow.WindowProc
        mov     [wc.cbClsExtra], 0
        mov     [wc.cbWndExtra], 0

        invoke GetModuleHandle, NULL
        mov     [wc.hInstance], eax

        invoke LoadIcon, 0, IDI_APPLICATION
        mov     [wc.hIcon], eax

        invoke LoadCursor, [wc.hInstance], "IDC_PEN"
        mov     [wc.hCursor], eax

        mov     [wc.hbrBackground], COLOR_HIGHLIGHT + 1
        mov     [wc.lpszMenuName], 0
        mov     [wc.lpszClassName], PlayWindow.ClassName

        invoke RegisterClass, addr wc

        ; registering gamewindows
        mov     [wc.lpfnWndProc], CharacterGame.WindowProc
        mov     [wc.lpszClassName], CharacterGame.ClassName
        invoke RegisterClass, addr wc
        mov     [wc.lpfnWndProc], ScribbleGame.WindowProc
        mov     [wc.lpszClassName], ScribbleGame.ClassName
        invoke RegisterClass, addr wc
        mov     [wc.lpfnWndProc], CopyGame.WindowProc
        mov     [wc.lpszClassName], CopyGame.ClassName
        invoke RegisterClass, addr wc
        mov     [wc.lpfnWndProc], EnvironmentGame.WindowProc
        mov     [wc.lpszClassName], EnvironmentGame.ClassName
        invoke RegisterClass, addr wc
        mov     [wc.lpfnWndProc], MixGame.WindowProc
        mov     [wc.lpszClassName], MixGame.ClassName
        invoke RegisterClass, addr wc
        mov     [wc.lpfnWndProc], IncrementalGame.WindowProc
        mov     [wc.lpszClassName], IncrementalGame.ClassName
        invoke RegisterClass, addr wc


        ret
endp ;returns status of registration in eax
;------------------------------------------------------------------------------
proc PlayWindow.Create, hwnd

        invoke  CreateWindowEx, 0, PlayWindow.ClassName, PlayWindow.WindowName,\
                WS_CHILD, \
                [_CurrentScreenSize.left], [_CurrentScreenSize.top], [_CurrentScreenSize.width], [_CurrentScreenSize.height],\
                [hwnd], _psPlayWindow, <invoke GetModuleHandle, NULL>, 0

        mov     [PlayWindow.Handle], eax

        ret
endp
;------------------------------------------------------------------------------
proc PlayWindow.WindowProc\
     uses edx ebx ecx esi edi,\
     hwnd, umsg, wparam, lparam

        locals
                bm      BITMAP
                hbm     dd      ?
                hdcTemp dd      ?
                hbmOld  dd      ?
                timerRect       RECTWH
        endl

        .if [umsg] = WM_CREATE

                ;stdcall CreatePlayWindowButtons, [hwnd]
                stdcall CreateAllGameWindows, [hwnd]

                mov     esi, [hwnd]
                mov     [PlayWindow.Handle], esi
                stdcall InitializeWindow, PlayWindow

                ;Creating Return button
                mov     [PlayWindow.ReturnBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.ReturnBmpBtnInfo

                ;Creating Restart button
                mov     [PlayWindow.RestartBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.RestartBmpBtnInfo

                ;Creating Change-timer button
                mov     [PlayWindow.ChangeTimerBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.ChangeTimerBmpBtnInfo

                ;creating timer buttons
                mov     [PlayWindow.EndlessTimerBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.EndlessTimerBmpBtnInfo
                mov     [PlayWindow.1minTimerBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.1minTimerBmpBtnInfo
                mov     [PlayWindow.2minTimerBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.2minTimerBmpBtnInfo
                mov     [PlayWindow.5minTimerBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.5minTimerBmpBtnInfo
                mov     [PlayWindow.10minTimerBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.10minTimerBmpBtnInfo
                mov     [PlayWindow.20minTimerBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, PlayWindow.20minTimerBmpBtnInfo


                mov     eax, 0


        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)
                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], PlayWindow.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future)
                invoke  GetClientRect, [hwnd], PlayWindow.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [PlayWindow.Buffer.rect.width], [PlayWindow.Buffer.rect.height]
                invoke  SelectObject, [PlayWindow.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ; painting on a Buffer DC
                ; copying bg to buffer
                invoke  StretchBlt, [PlayWindow.Buffer.hdc], 0, 0, [PlayWindow.Buffer.rect.width], [PlayWindow.Buffer.rect.height],\
                        [PlayWindow.BG.hdc], 0, 0, [PlayWindow.BG.bm.bmWidth], [PlayWindow.BG.bm.bmHeight], SRCCOPY

                ;drawing return button
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.ReturnBmpBtnInfo, 1
                ;drawing Restart button
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.RestartBmpBtnInfo, 1
                ;drawing ChangeTimer button
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.ChangeTimerBmpBtnInfo, 1

                ;drawing timer buttons
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.EndlessTimerBmpBtnInfo, 1
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.1minTimerBmpBtnInfo, 1
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.2minTimerBmpBtnInfo, 1
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.5minTimerBmpBtnInfo, 1
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.10minTimerBmpBtnInfo, 1
                stdcall DrawBitmapButton, [PlayWindow.Buffer.hdc], PlayWindow.20minTimerBmpBtnInfo, 1

                ;drawing timer
                .if ([Game.IsBeingPlayed]) & ([Game.CurrentTimer] <> _biPlayEndless)

                        mov     [timerRect.left], 0
                        mov     [timerRect.top], 0
                        mov     ebx, [PlayWindow.Buffer.rect.height]
                        shr     ebx, 3
                        mov     [timerRect.height], ebx
                        mov     ebx, [PlayWindow.Buffer.rect.width]
                        mov     [timerRect.width], ebx


                        ; preparing strOut
                        .if [Timer.TimeLeft]
                                mov     edx, [Timer.TimeLeft]
                                mov     eax, [Timer.TimeLeft]
                                shr     edx, 16
                                mov     cx, 60000 ; minute
                                div     cx
                                movzx   edi, ax

                                movzx   edx, dx
                                mov     eax, edx
                                shr     edx, 16
                                mov     cx, 1000
                                div     cx
                                movzx   edx, ax


                                .if [Timer.Dots]
                                        cinvoke wsprintfA, Timer.strToPrint, Timer.FormatWithDots, edi, edx
                                .else
                                        cinvoke wsprintfA, Timer.strToPrint,Timer.FormatWithoutDots, edi, edx
                                .endif
                        .else
                                cinvoke  wsprintfA, Timer.strToPrint, Timer.FormatTimeOut, Timer.strTimeOut
                        .endif

                        invoke  DrawText, [PlayWindow.Buffer.hdc], Timer.strToPrint, -1, addr timerRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE
                .endif




                ;Copying everything from Back DC to RealDC
                invoke  BitBlt, esi, 0, 0, [PlayWindow.Buffer.rect.width], [PlayWindow.Buffer.rect.height],\
                        [PlayWindow.Buffer.hdc], 0, 0, SRCCOPY


                ;ending real painting
                .if ([umsg] = WM_PAINT)
                        invoke EndPaint, [hwnd], PlayWindow.PS
                .else

                        ;invoke  SendMessage, [CharacterGame.Handle], WM_PAINT, esi, 0

                .endif



        .elseif ([umsg] = WM_MOUSEMOVE) | ([umsg] = WM_LBUTTONDOWN)

                local   MousePoint      POINT
                stdcall GetPointFromLparam, [lparam], addr MousePoint

                mov     ebx, 0

                .if ~ebx ; Checking for point on Return button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.ReturnBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlayReturn
                        .endif
                .endif
                .if ~ebx ; Checking for point on Restart button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.RestartBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlayRestart
                        .endif
                .endif
                .if ~ebx ; Checking for point on ChangeTimer button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.ChangeTimerBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlayChangeTimer
                        .endif
                .endif
                .if ~ebx ; Checking for point on Timer button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.EndlessTimerBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlayEndless
                        .endif
                .endif
                .if ~ebx ; Checking for point on Timer button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.1minTimerBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlay1min
                        .endif
                .endif
                .if ~ebx ; Checking for point on Timer button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.2minTimerBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlay2min
                        .endif
                .endif
                .if ~ebx ; Checking for point on Timer button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.5minTimerBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlay5min
                        .endif
                .endif
                .if ~ebx ; Checking for point on Timer button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.10minTimerBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlay10min
                        .endif
                .endif
                .if ~ebx ; Checking for point on Timer button
                        stdcall IsPointOnButton, addr MousePoint, PlayWindow.20minTimerBmpBtnInfo
                        .if eax
                                mov     ebx, _biPlay20min
                        .endif
                .endif

                .if ebx ; if point is on any button
                        mov     esi, [HandCursorHandle]
                        ; if there was a click on a button:
                        .if [umsg] = WM_LBUTTONDOWN
                                ; if timer button was pressed:
                                .if (ebx >= _biPlayEndless) & (ebx <= _biPlay20min)
                                        mov     [Game.CurrentTimer], ebx ;saving that timer
                                        stdcall PlayWindow.StartGame ; starting game
                                ; if return was pressed
                                .elseif ebx = _biPlayReturn
                                        stdcall PlayWindow.ReturnClicked
                                ; if restart was pressed
                                .elseif ebx = _biPlayRestart
                                        stdcall PlayWindow.RestartClicked
                                ; if change timer was pressed
                                .elseif ebx = _biPlayChangeTimer
                                        stdcall PlayWindow.ChangeTimerClicked
                                .endif
                        .endif
                .else
                        mov     esi, [PenCursorHandle]
                .endif

                invoke  SetClassLongA, DWORD [hwnd], GCL_HCURSOR, esi

        .elseif [umsg] = WM_TIMER

                mov     [timerRect.left], 0
                mov     [timerRect.top], 0
                mov     ebx, [PlayWindow.Buffer.rect.height]
                shr     ebx, 3
                mov     [timerRect.height], ebx
                mov     ebx, [PlayWindow.Buffer.rect.width]
                mov     [timerRect.width], ebx

                .if [wparam] = _tickTimerIndex

                        .if [Timer.Dots]
                                mov     [Timer.Dots], FALSE
                        .else
                                mov     [Timer.Dots], TRUE
                        .endif
                        invoke  InvalidateRect, [hwnd], addr timerRect, FALSE
;                        invoke  RedrawWindow, [hwnd], NULL, NULL,  RDW_UPDATENOW

                .elseif [wparam] = _secondTimerIndex

                        .if [Timer.TimeLeft]
                                sub     [Timer.TimeLeft], _second
                        .endif
                        invoke  InvalidateRect, [hwnd], addr timerRect, FALSE
;                        invoke  RedrawWindow, [hwnd], NULL, NULL,  RDW_UPDATENOW

                .else
                        invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]
                .endif

        .elseif [umsg] = WM_ERASEBKGND
                mov     eax, 0
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]
        .endif

        ;mov     eax, 0

        ret

endp
;------------------------------------------------------------------------------
proc PlayWindow.ReturnClicked

        stdcall PlayWindow.StopGame
        invoke PostMessage, [MainWindow.Handle], _wmSwitchWindows, [ChooseWindow.Handle], [PlayWindow.Handle]

        ret
endp
;------------------------------------------------------------------------------
proc PlayWindow.RestartClicked
        invoke SendMessage, [MainWindow.Handle], _wmSwitchWindows, [CharacterGame.Handle], 0
        ret
endp
;------------------------------------------------------------------------------
proc PlayWindow.ChangeTimerClicked
        invoke SendMessage, [MainWindow.Handle], _wmSwitchWindows, 0, [CharacterGame.Handle]
        ret
endp
;------------------------------------------------------------------------------
proc PlayWindow.StartGame
        ; till this line Game.CurrentTimer, Game.CurrentHWindow and Game.CurrentMode are set up with proper data
        ; CurrentHWindow and CurrentMode are set after choosing a game mode on a choose menu
        ; CurrentTimer is set after clicking on a timer button on the play window
        .if [Game.CurrentTimer]
                .if [Game.CurrentTimer] = _biPlay1min
                        mov     [Timer.TimeLeft], _1min
                .elseif [Game.CurrentTimer] = _biPlay2min
                        mov     [Timer.TimeLeft], _2min
                .elseif [Game.CurrentTimer] = _biPlay5min
                        mov     [Timer.TimeLeft], _5min
                .elseif [Game.CurrentTimer] = _biPlay10min
                        mov     [Timer.TimeLeft], _10min
                .elseif [Game.CurrentTimer] = _biPlay20min
                        mov     [Timer.TimeLeft], _20min
                .endif
                mov     [Game.IsBeingPlayed], TRUE
                .if  ([Game.CurrentTimer] <> _biPlayEndless)
                        invoke  SetTimer, [PlayWindow.Handle], _mainTimerIndex, [Timer.TimeLeft], PlayWindow.TimeOut
                        invoke  SetTimer, [PlayWindow.Handle], _secondTimerIndex, _second, NULL
                        invoke  SetTimer, [PlayWindow.Handle], _tickTimerIndex, _tick, NULL
                .endif

;                mov     [PlayWindow.RestartBmpBtnInfo.visible], 1
;                invoke  InvalidateRect, [PlayWindow.Handle], PlayWindow.RestartBmpBtnInfo.realRect, NULL

                invoke  PostMessage, [Game.CurrentHWindow], _wmPrepareSelf, 0, 0

                invoke  PostMessage, [MainWindow.Handle], _wmSwitchWindows, [Game.CurrentHWindow], 0

        .endif

        ret
endp
;------------------------------------------------------------------------------
proc PlayWindow.TimeOut

        invoke  KillTimer, [PlayWindow.Handle], _mainTimerIndex
;        invoke  SetTimer, [PlayWindow.Handle], _mainTimerIndex, _timeAfterTimeout, PlayWindow.StopGame
        invoke  SetTimer, [PlayWindow.Handle], _mainTimerIndex, _timeAfterTimeout, PlayWindow.ReturnClicked

        ret
endp
;------------------------------------------------------------------------------
proc PlayWindow.StopGame

        .if ([Game.CurrentTimer]) & ([Game.CurrentTimer] <> _biPlayEndless)
                invoke  KillTimer, [PlayWindow.Handle], _secondTimerIndex
                invoke  KillTimer, [PlayWindow.Handle], _tickTimerIndex
                invoke  KillTimer, [PlayWindow.Handle], _mainTimerIndex
                mov     [Timer.TimeLeft], 0
                mov     [Game.IsBeingPlayed], FALSE
                mov     [Timer.strToPrint], 0
        .endif

        invoke  PostMessage, [Game.CurrentHWindow], _wmCleanSelf, 0, 0

        invoke  PostMessage, [MainWindow.Handle], _wmSwitchWindows, 0, [Game.CurrentHWindow]
        ;invoke  InvalidateRect, [MainWindow.Handle], 0, FALSE
        ret
endp
;------------------------------------------------------------------------------


;                               Games' windows


;------------------------------------------------------------------------------
proc CreateAllGameWindows\
     uses ebx edx esi,\
     hwnd
        local WindowScreenRect: RECTWH


        stdcall MapPointOnScreen, GameWindow.Rect.left, addr WindowScreenRect.left, _ScreenScale
        stdcall MapPointOnScreen, GameWindow.Rect.width, addr WindowScreenRect.width, _ScreenScale


        ; Creating character game window on the play window
        invoke  CreateWindowEx, 0, CharacterGame.ClassName, CharacterGame.WindowName,\
                WS_CHILD, \
                [WindowScreenRect.left], [WindowScreenRect.top], [WindowScreenRect.width], [WindowScreenRect.height],\
                [hwnd], _gmCharacter, <invoke GetModuleHandle, NULL>, 0
        mov     [CharacterGame.Handle], eax

        ; Creating Sctibble game window on the play window
        invoke  CreateWindowEx, 0, ScribbleGame.ClassName, ScribbleGame.WindowName,\
                WS_CHILD, \
                [WindowScreenRect.left], [WindowScreenRect.top], [WindowScreenRect.width], [WindowScreenRect.height],\
                [hwnd], _gmScribble, <invoke GetModuleHandle, NULL>, 0
        mov     [ScribbleGame.Handle], eax

        ; Creating Copy game window on the play window
        invoke  CreateWindowEx, 0, CopyGame.ClassName, CopyGame.WindowName,\
                WS_CHILD, \
                [WindowScreenRect.left], [WindowScreenRect.top], [WindowScreenRect.width], [WindowScreenRect.height],\
                [hwnd], _gmCopy, <invoke GetModuleHandle, NULL>, 0
        mov     [CopyGame.Handle], eax

        ; Creating Environment game window on the play window
        invoke  CreateWindowEx, 0, EnvironmentGame.ClassName, EnvironmentGame.WindowName,\
                WS_CHILD, \
                [WindowScreenRect.left], [WindowScreenRect.top], [WindowScreenRect.width], [WindowScreenRect.height],\
                [hwnd], _gmEnvironment, <invoke GetModuleHandle, NULL>, 0
        mov     [EnvironmentGame.Handle], eax

        ; Creating Mix game window on the play window
        invoke  CreateWindowEx, 0, MixGame.ClassName, MixGame.WindowName,\
                WS_CHILD, \
                [WindowScreenRect.left], [WindowScreenRect.top], [WindowScreenRect.width], [WindowScreenRect.height],\
                [hwnd], _gmMix, <invoke GetModuleHandle, NULL>, 0
        mov     [MixGame.Handle], eax

        ; Creating Incremental game window on the play window
        invoke  CreateWindowEx, 0, IncrementalGame.ClassName, IncrementalGame.WindowName,\
                WS_CHILD, \
                [WindowScreenRect.left], [WindowScreenRect.top], [WindowScreenRect.width], [WindowScreenRect.height],\
                [hwnd], _gmIncremental, <invoke GetModuleHandle, NULL>, 0
        mov     [IncrementalGame.Handle], eax


        ret
endp
;------------------------------------------------------------------------------


;                           Games's procedures


;------------------------------------------------------------------------------
proc CharacterGame.WindowProc\
     uses edx ebx esi edi,\
     hwnd, umsg, wparam, lparam

     locals
        strAmount       db      10 dup 0
        textRect        RECT
     endl
        .if [umsg] = WM_CREATE

                mov     esi, [hwnd]
                mov     [CharacterGame.Handle], esi
                stdcall InitializeWindow, CharacterGame

                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, _strResCharacter1, addr strAmount, 10
                invoke  StrToIntA, addr strAmount
                mov     [CharacterGame.Amount1], eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, _strResCharacter2, addr strAmount, 10
                invoke  StrToIntA, addr strAmount
                mov     [CharacterGame.Amount2], eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, _strResCharacter3, addr strAmount, 10
                invoke  StrToIntA, addr strAmount
                mov     [CharacterGame.Amount3], eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, _strResCharacter4, addr strAmount, 10
                invoke  StrToIntA, addr strAmount
                mov     [CharacterGame.Amount4], eax

        .elseif [umsg] = _wmPrepareSelf

                stdcall GetRandom, 1, [CharacterGame.Amount1]
                mov     esi, _strResCharacter1
                add     esi, eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, esi, CharacterGame.Buffer1, 100

                stdcall GetRandom, 1, [CharacterGame.Amount2]
                mov     esi, _strResCharacter2
                add     esi, eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, esi, CharacterGame.Buffer2, 100

                stdcall GetRandom, 1, [CharacterGame.Amount3]
                mov     esi, _strResCharacter3
                add     esi, eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, esi, CharacterGame.Buffer3, 100

                stdcall GetRandom, 1, [CharacterGame.Amount4]
                mov     esi, _strResCharacter4
                add     esi, eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, esi, CharacterGame.Buffer4, 100


        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)

                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], CharacterGame.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                mov     [textRect.left], 0
                mov     ebx, [CharacterGame.Buffer.rect.width]
                mov     [textRect.right], ebx

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future)
                invoke  GetClientRect, [hwnd], CharacterGame.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [CharacterGame.Buffer.rect.width], [CharacterGame.Buffer.rect.height]
                invoke  SelectObject, [CharacterGame.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ; painting on a Buffer DC
                ; copying bg to buffer
                invoke  StretchBlt, [CharacterGame.Buffer.hdc], 0, 0, [CharacterGame.Buffer.rect.width], [CharacterGame.Buffer.rect.height],\
                        [CharacterGame.BG.hdc], 0, 0, [CharacterGame.BG.bm.bmWidth], [CharacterGame.BG.bm.bmHeight], SRCCOPY

                ;drawing 4 lines
                mov     edi, [CharacterGame.Buffer.rect.height]
                shr     edi, 1
                mov     ebx, edi
                sub     ebx, [CharacterGame.LineHeight]
                sub     ebx, [CharacterGame.LineHeight]
                mov     [textRect.top], ebx
                add     ebx, [CharacterGame.LineHeight]
                mov     [textRect.bottom], ebx
                invoke  DrawText, [CharacterGame.Buffer.hdc], CharacterGame.Buffer1, -1, addr textRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE
                mov     ebx, edi
                sub     ebx, [CharacterGame.LineHeight]
                mov     [textRect.top], ebx
                add     ebx, [CharacterGame.LineHeight]
                mov     [textRect.bottom], ebx
                invoke  DrawText, [CharacterGame.Buffer.hdc], CharacterGame.Buffer2, -1, addr textRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE
                mov     ebx, edi
                mov     [textRect.top], ebx
                add     ebx, [CharacterGame.LineHeight]
                mov     [textRect.bottom], ebx
                invoke  DrawText, [CharacterGame.Buffer.hdc], CharacterGame.Buffer3, -1, addr textRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE
                mov     ebx, edi
                add     ebx, [CharacterGame.LineHeight]
                mov     [textRect.top], ebx
                add     ebx, [CharacterGame.LineHeight]
                mov     [textRect.bottom], ebx
                invoke  DrawText, [CharacterGame.Buffer.hdc], CharacterGame.Buffer4, -1, addr textRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE

                ;Copying everything from Back DC to RealDC
                invoke  BitBlt, esi, 0, 0, [CharacterGame.Buffer.rect.width], [CharacterGame.Buffer.rect.height],\
                        [CharacterGame.Buffer.hdc], 0, 0, SRCCOPY


                .if ([umsg] = WM_PAINT)
                        invoke  EndPaint, [hwnd], CharacterGame.PS
                .endif
        .elseif [umsg] = WM_ERASEBKGND
                mov     eax, 0
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]

        .endif
        ret
endp
;------------------------------------------------------------------------------
proc ScribbleGame.WindowProc\
     uses ebx esi edi,\
     hwnd, umsg, wparam, lparam

        .if [umsg] = WM_CREATE

                mov     esi, [hwnd]
                mov     [ScribbleGame.Handle], esi
                stdcall InitializeWindow, ScribbleGame

        .elseif [umsg] = _wmPrepareSelf
                mov     ebx, [ScribbleGame.Amount]
                sub     ebx, 1
                stdcall GetRandom, 0, ebx
                mov     [ScribbleGame.SourceNumber], '0'
                add     [ScribbleGame.SourceNumber], al
                invoke  LoadImageA, <invoke GetModuleHandle, NULL>, ScribbleGame.SourceName, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR
                mov     edi, eax
                invoke  GetObject, edi, sizeof.BITMAP, ScribbleGame.BG.bm
                invoke  SelectObject, [ScribbleGame.BG.hdc], edi
                invoke  DeleteObject, eax

        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)

                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], ScribbleGame.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future)
                invoke  GetClientRect, [hwnd], ScribbleGame.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [ScribbleGame.Buffer.rect.width], [ScribbleGame.Buffer.rect.height]
                invoke  SelectObject, [ScribbleGame.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ; painting on a Buffer DC
                ; copying bg to buffer
                invoke  StretchBlt, [ScribbleGame.Buffer.hdc], 0, 0, [ScribbleGame.Buffer.rect.width], [ScribbleGame.Buffer.rect.height],\
                        [ScribbleGame.BG.hdc], 0, 0, [ScribbleGame.BG.bm.bmWidth], [ScribbleGame.BG.bm.bmHeight], SRCCOPY

                ;Copying everything from Back DC to RealDC
                invoke  BitBlt, esi, 0, 0, [ScribbleGame.Buffer.rect.width], [ScribbleGame.Buffer.rect.height],\
                        [ScribbleGame.Buffer.hdc], 0, 0, SRCCOPY


                .if ([umsg] = WM_PAINT)
                        invoke  EndPaint, [hwnd], ScribbleGame.PS
                .endif
        .elseif [umsg] = WM_ERASEBKGND
                mov     eax, 0
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]

        .endif
        ret
endp
;------------------------------------------------------------------------------
proc CopyGame.WindowProc\
     uses ebx esi edi,\
     hwnd, umsg, wparam, lparam

        .if [umsg] = WM_CREATE

                mov     esi, [hwnd]
                mov     [CopyGame.Handle], esi
                stdcall InitializeWindow, CopyGame

        .elseif [umsg] = _wmPrepareSelf
                mov     ebx, [CopyGame.Amount]
                sub     ebx, 1
                stdcall GetRandom, 0, ebx
                mov     [CopyGame.SourceNumber], '0'
                add     [CopyGame.SourceNumber], al
                invoke  LoadImageA, <invoke GetModuleHandle, NULL>, CopyGame.SourceName, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR
                mov     edi, eax
                invoke  GetObject, edi, sizeof.BITMAP, CopyGame.BG.bm
                invoke  SelectObject, [CopyGame.BG.hdc], edi
                invoke  DeleteObject, eax

        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)

                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], CopyGame.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future)
                invoke  GetClientRect, [hwnd], CopyGame.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [CopyGame.Buffer.rect.width], [CopyGame.Buffer.rect.height]
                invoke  SelectObject, [CopyGame.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ; painting on a Buffer DC
                ; copying bg to buffer
                invoke  StretchBlt, [CopyGame.Buffer.hdc], 0, 0, [CopyGame.Buffer.rect.width], [CopyGame.Buffer.rect.height],\
                        [CopyGame.BG.hdc], 0, 0, [CopyGame.BG.bm.bmWidth], [CopyGame.BG.bm.bmHeight], SRCCOPY

                ;Copying everything from Back DC to RealDC
                invoke  BitBlt, esi, 0, 0, [CopyGame.Buffer.rect.width], [CopyGame.Buffer.rect.height],\
                        [CopyGame.Buffer.hdc], 0, 0, SRCCOPY


                .if ([umsg] = WM_PAINT)
                        invoke  EndPaint, [hwnd], CopyGame.PS
                .endif
        .elseif [umsg] = WM_ERASEBKGND
                mov     eax, 0
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]

        .endif
        ret
endp
;------------------------------------------------------------------------------
proc EnvironmentGame.WindowProc\
     uses ebx esi edi,\
     hwnd, umsg, wparam, lparam
     locals
        strAmount       db      10 dup 0
        textRect        RECT
     endl

        .if [umsg] = WM_CREATE

                mov     esi, [hwnd]
                mov     [EnvironmentGame.Handle], esi
                stdcall InitializeWindow, EnvironmentGame

                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, _strResEnvironment1, addr strAmount, 10
                invoke  StrToIntA, addr strAmount
                mov     [EnvironmentGame.Amount1], eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, _strResEnvironment2, addr strAmount, 10
                invoke  StrToIntA, addr strAmount
                mov     [EnvironmentGame.Amount2], eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, _strResEnvironment3, addr strAmount, 10
                invoke  StrToIntA, addr strAmount
                mov     [EnvironmentGame.Amount3], eax


        .elseif [umsg] = _wmPrepareSelf

                stdcall GetRandom, 1, [EnvironmentGame.Amount1]
                mov     esi, _strResEnvironment1
                add     esi, eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, esi, EnvironmentGame.Buffer1, 100

                stdcall GetRandom, 1, [EnvironmentGame.Amount2]
                mov     esi, _strResEnvironment2
                add     esi, eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, esi, EnvironmentGame.Buffer2, 100

                stdcall GetRandom, 1, [EnvironmentGame.Amount3]
                mov     esi, _strResEnvironment3
                add     esi, eax
                invoke  LoadStringA, <invoke GetModuleHandle, NULL>, esi, EnvironmentGame.Buffer3, 100

        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)

                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], EnvironmentGame.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                mov     [textRect.left], 0
                mov     ebx, [CharacterGame.Buffer.rect.width]
                mov     [textRect.right], ebx

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future)
                invoke  GetClientRect, [hwnd], EnvironmentGame.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [EnvironmentGame.Buffer.rect.width], [EnvironmentGame.Buffer.rect.height]
                invoke  SelectObject, [EnvironmentGame.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ; painting on a Buffer DC
                ; copying bg to buffer
                invoke  StretchBlt, [EnvironmentGame.Buffer.hdc], 0, 0, [EnvironmentGame.Buffer.rect.width], [EnvironmentGame.Buffer.rect.height],\
                        [EnvironmentGame.BG.hdc], 0, 0, [EnvironmentGame.BG.bm.bmWidth], [EnvironmentGame.BG.bm.bmHeight], SRCCOPY

                ;drawing 4 lines
                mov     edi, [EnvironmentGame.Buffer.rect.height]
                shr     edi, 1
                mov     ebx, edi
                sub     ebx, [EnvironmentGame.LineHeight]
                sub     ebx, [EnvironmentGame.LineHeight]
                mov     [textRect.top], ebx
                add     ebx, [EnvironmentGame.LineHeight]
                mov     [textRect.bottom], ebx
                invoke  DrawText, [EnvironmentGame.Buffer.hdc], EnvironmentGame.Buffer1, -1, addr textRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE
                mov     ebx, edi
                sub     ebx, [EnvironmentGame.LineHeight]
                mov     [textRect.top], ebx
                add     ebx, [EnvironmentGame.LineHeight]
                mov     [textRect.bottom], ebx
                invoke  DrawText, [EnvironmentGame.Buffer.hdc], EnvironmentGame.Buffer2, -1, addr textRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE
                mov     ebx, edi
                mov     [textRect.top], ebx
                add     ebx, [EnvironmentGame.LineHeight]
                mov     [textRect.bottom], ebx
                invoke  DrawText, [EnvironmentGame.Buffer.hdc], EnvironmentGame.Buffer3, -1, addr textRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE

                ;Copying everything from Back DC to RealDC
                invoke  BitBlt, esi, 0, 0, [EnvironmentGame.Buffer.rect.width], [EnvironmentGame.Buffer.rect.height],\
                        [EnvironmentGame.Buffer.hdc], 0, 0, SRCCOPY


                .if ([umsg] = WM_PAINT)
                        invoke  EndPaint, [hwnd], EnvironmentGame.PS
                .endif
        .elseif [umsg] = WM_ERASEBKGND
                mov     eax, 0
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]

        .endif
        ret
endp
;------------------------------------------------------------------------------
proc MixGame.WindowProc\
     uses ebx esi edi,\
     hwnd, umsg, wparam, lparam

        .if [umsg] = WM_CREATE

                mov     esi, [hwnd]
                mov     [MixGame.Handle], esi
                stdcall InitializeWindow, MixGame



        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)

                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], MixGame.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future)
                invoke  GetClientRect, [hwnd], MixGame.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [MixGame.Buffer.rect.width], [MixGame.Buffer.rect.height]
                invoke  SelectObject, [MixGame.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ; painting on a Buffer DC
                ; copying bg to buffer
                invoke  StretchBlt, [MixGame.Buffer.hdc], 0, 0, [MixGame.Buffer.rect.width], [MixGame.Buffer.rect.height],\
                        [MixGame.BG.hdc], 0, 0, [MixGame.BG.bm.bmWidth], [MixGame.BG.bm.bmHeight], SRCCOPY
                ;invoke  Rectangle, [CharacterGame.Buffer.hdc], 0, 0, [CharacterGame.Buffer.rect.width], [CharacterGame.Buffer.rect.height]

                ;Copying everything from Back DC to RealDC
                invoke  BitBlt, esi, 0, 0, [MixGame.Buffer.rect.width], [MixGame.Buffer.rect.height],\
                        [MixGame.Buffer.hdc], 0, 0, SRCCOPY


                .if ([umsg] = WM_PAINT)
                        invoke  EndPaint, [hwnd], MixGame.PS
                .endif
        .elseif [umsg] = WM_ERASEBKGND
                mov     eax, 0
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]

        .endif
        ret
endp
;------------------------------------------------------------------------------
proc IncrementalGame.WindowProc\
     uses ebx esi edi,\
     hwnd, umsg, wparam, lparam

        .if [umsg] = WM_CREATE

                mov     esi, [hwnd]
                mov     [IncrementalGame.Handle], esi
                stdcall InitializeWindow, IncrementalGame



        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)

                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], IncrementalGame.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future)
                invoke  GetClientRect, [hwnd], IncrementalGame.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [IncrementalGame.Buffer.rect.width], [IncrementalGame.Buffer.rect.height]
                invoke  SelectObject, [IncrementalGame.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ; painting on a Buffer DC
                ; copying bg to buffer
                invoke  StretchBlt, [IncrementalGame.Buffer.hdc], 0, 0, [IncrementalGame.Buffer.rect.width], [IncrementalGame.Buffer.rect.height],\
                        [IncrementalGame.BG.hdc], 0, 0, [IncrementalGame.BG.bm.bmWidth], [IncrementalGame.BG.bm.bmHeight], SRCCOPY

                ;Copying everything from Back DC to RealDC
                invoke  BitBlt, esi, 0, 0, [IncrementalGame.Buffer.rect.width], [IncrementalGame.Buffer.rect.height],\
                        [IncrementalGame.Buffer.hdc], 0, 0, SRCCOPY


                .if ([umsg] = WM_PAINT)
                        invoke  EndPaint, [hwnd], IncrementalGame.PS
                .endif
        .elseif [umsg] = WM_ERASEBKGND
                mov     eax, 0
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]

        .endif
        ret
endp
;------------------------------------------------------------------------------
