;------------------------------------------------------------------------------
proc ChooseWindow.Register
        local wc: WNDCLASS

        mov     [wc.style], 0
        mov     [wc.lpfnWndProc], ChooseWindow.WindowProc
        mov     [wc.cbClsExtra], 0
        mov     [wc.cbWndExtra], 0

        invoke GetModuleHandle, NULL
        mov     [wc.hInstance], eax

        invoke LoadIcon, 0, IDI_APPLICATION
        mov     [wc.hIcon], eax

        invoke LoadCursor, [wc.hInstance], "IDC_PEN"
        mov     [wc.hCursor], eax

        mov     [wc.hbrBackground], COLOR_SCROLLBAR + 1
        mov     [wc.lpszMenuName], 0
        mov     [wc.lpszClassName], ChooseWindow.ClassName

        invoke RegisterClass, addr wc

        ret
endp ;returns status of registration in eax
;------------------------------------------------------------------------------
proc ChooseWindow.Create, hwnd

        invoke  CreateWindowEx, 0, ChooseWindow.ClassName, ChooseWindow.WindowName,\
                WS_CHILD, \
                [_CurrentScreenSize.left], [_CurrentScreenSize.top], [_CurrentScreenSize.width], [_CurrentScreenSize.height],\
                [hwnd], _psChooseWindow, <invoke GetModuleHandle, NULL>, 0

        mov     [ChooseWindow.Handle], eax

        ret
endp
;------------------------------------------------------------------------------
proc ChooseWindow.WindowProc\
     uses ebx ecx esi edi,\
     hwnd, umsg, wparam, lparam
        local   hbmTemp         dd      ?

        .if [umsg] = WM_CREATE

                ;stdcall CreateChooseWindowButtons, [hwnd]

                mov     esi, [hwnd]
                mov     [ChooseWindow.Handle], esi
                stdcall InitializeWindow, ChooseWindow

                ;Creating Character button
                mov     [ChooseWindow.CharacterBmpBtnInfo.hwndParent], eSi
                stdcall CreateBitmapButton, ChooseWindow.CharacterBmpBtnInfo
                ;Creating scribble button
                mov     [ChooseWindow.CharacterBmpBtnInfo.hwndParent], eSi
                stdcall CreateBitmapButton, ChooseWindow.ScribbleBmpBtnInfo
                ;Creating copy button
                mov     [ChooseWindow.CharacterBmpBtnInfo.hwndParent], eSi
                stdcall CreateBitmapButton, ChooseWindow.CopyBmpBtnInfo
                ;Creating environment button
                mov     [ChooseWindow.CharacterBmpBtnInfo.hwndParent], eSi
                stdcall CreateBitmapButton, ChooseWindow.EnvironmentBmpBtnInfo
                ;Creating Mix button
                mov     [ChooseWindow.CharacterBmpBtnInfo.hwndParent], eSi
                stdcall CreateBitmapButton, ChooseWindow.MixBmpBtnInfo
                ;Creating incremental button
                mov     [ChooseWindow.CharacterBmpBtnInfo.hwndParent], eSi
                stdcall CreateBitmapButton, ChooseWindow.IncrementalBmpBtnInfo

                ;Creating Return button
                mov     [ChooseWindow.ReturnBmpBtnInfo.hwndParent], esi
                stdcall CreateBitmapButton, ChooseWindow.ReturnBmpBtnInfo

                mov     eax, 0


        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)
                ;Preparing for real painting
                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], ChooseWindow.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future(someday))
                invoke  GetClientRect, [hwnd], ChooseWindow.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [ChooseWindow.Buffer.rect.width], [ChooseWindow.Buffer.rect.height]
                invoke  SelectObject, [ChooseWindow.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ;Painting on a buffer-DC
                ; copying bg to buffer
                invoke  StretchBlt, [ChooseWindow.Buffer.hdc], 0, 0, [ChooseWindow.Buffer.rect.width], [ChooseWindow.Buffer.rect.height],\
                        [ChooseWindow.BG.hdc], 0, 0, [ChooseWindow.BG.bm.bmWidth], [ChooseWindow.BG.bm.bmHeight], SRCCOPY

                ; drawing buttons
                mov     edi, [ChooseWindow.Buffer.hdc]
                stdcall DrawBitmapButton, edi, ChooseWindow.CharacterBmpBtnInfo, 1
                stdcall DrawBitmapButton, edi, ChooseWindow.ScribbleBmpBtnInfo, 1
                stdcall DrawBitmapButton, edi, ChooseWindow.CopyBmpBtnInfo, 1
                stdcall DrawBitmapButton, edi, ChooseWindow.EnvironmentBmpBtnInfo, 1
                stdcall DrawBitmapButton, edi, ChooseWindow.MixBmpBtnInfo, 1
                stdcall DrawBitmapButton, edi, ChooseWindow.IncrementalBmpBtnInfo, 1

                stdcall DrawBitmapButton, edi, ChooseWindow.ReturnBmpBtnInfo, 1


                ;Copying everything from Buffer DC to RealDC
                invoke  BitBlt, esi, 0, 0, [ChooseWindow.Buffer.rect.width], [ChooseWindow.Buffer.rect.height],\
                        [ChooseWindow.Buffer.hdc], 0, 0, SRCCOPY

                ;ending real painting
                .if ([umsg] = WM_PAINT)
                        invoke EndPaint, [hwnd], ChooseWindow.PS
                .endif

                mov     eax, 0

        .elseif ([umsg] = WM_MOUSEMOVE) | ([umsg] = WM_LBUTTONDOWN)

                local   MousePoint      POINT
                stdcall GetPointFromLparam, [lparam], addr MousePoint

                mov     ebx, 0

                .if ~ebx ; Checking for point on Character button
                        stdcall IsPointOnButton, addr MousePoint, ChooseWindow.CharacterBmpBtnInfo
                        .if eax
                                mov     ebx, _biChooseCharacter
                        .endif
                .endif

                .if ~ebx ; Checking for point on Scribble button
                        stdcall IsPointOnButton, addr MousePoint, ChooseWindow.ScribbleBmpBtnInfo
                        .if eax
                                mov     ebx, _biChooseScribble
                        .endif
                .endif

                .if ~ebx ; Checking for point on Copy button
                        stdcall IsPointOnButton, addr MousePoint, ChooseWindow.CopyBmpBtnInfo
                        .if eax
                                mov     ebx, _biChooseCopy
                        .endif
                .endif

                .if ~ebx ; Checking for point on Environment button
                        stdcall IsPointOnButton, addr MousePoint, ChooseWindow.EnvironmentBmpBtnInfo
                        .if eax
                                mov     ebx, _biChooseEnvironment
                        .endif
                .endif

                .if ~ebx ; Checking for point on Mix button
                        stdcall IsPointOnButton, addr MousePoint, ChooseWindow.MixBmpBtnInfo
                        .if eax
                                mov     ebx, _biChooseMix
                        .endif
                .endif


                .if ~ebx ; Checking for point on Incremental button
                        stdcall IsPointOnButton, addr MousePoint, ChooseWindow.IncrementalBmpBtnInfo
                        .if eax
                                mov     ebx, _biChooseIncremental
                        .endif
                .endif

                .if ~ebx ; Checking for point on Return button
                        stdcall IsPointOnButton, addr MousePoint, ChooseWindow.ReturnBmpBtnInfo
                        .if eax
                                mov     ebx, _biChooseReturn
                        .endif
                .endif

                .if ebx ; if point is on any button
                        mov     esi, [HandCursorHandle]
                        .if [umsg] = WM_LBUTTONDOWN   ; if there was a click on a button
                                .if (ebx >= _biChooseCharacter) & (ebx <= _biChooseIncremental)
                                ;        stdcall StartButtonClicked
                                        stdcall ChooseWindow.PlayClicked, ebx
                                .elseif ebx = _biChooseReturn
                                ;        stdcall ExitButtonClicked
                                        stdcall ChooseWindow.ReturnClicked
                                .endif
                        .endif
                .else
                        mov     esi, [PenCursorHandle]
                .endif

                invoke  SetClassLongA, DWORD [hwnd], GCL_HCURSOR, esi
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]
        .endif
        ret
endp
;------------------------------------------------------------------------------
proc ChooseWindow.ReturnClicked

        invoke PostMessage, [MainWindow.Handle], _wmSwitchWindows, [HelloWindow.Handle], [ChooseWindow.Handle]
        mov    [Game.CurrentHWindow], 0
        ret
endp
;------------------------------------------------------------------------------
proc ChooseWindow.PlayClicked\
     uses esi,\
     ButtonIndex

        mov     esi, [ButtonIndex]
        mov     [Game.CurrentMode], esi

        .if [ButtonIndex] = _biChooseCharacter
                mov     esi, [CharacterGame.Handle]
        .elseif [ButtonIndex] = _biChooseScribble
                mov     esi, [ScribbleGame.Handle]
        .elseif [ButtonIndex] = _biChooseCopy
                mov     esi, [CopyGame.Handle]
        .elseif [ButtonIndex] = _biChooseEnvironment
                mov     esi, [EnvironmentGame.Handle]
        .elseif [ButtonIndex] = _biChooseMix
                mov     esi, [MixGame.Handle]
        .elseif [ButtonIndex] = _biChooseIncremental
                mov     esi, [IncrementalGame.Handle]
        .endif
        mov     [Game.CurrentHWindow], esi

        invoke  PostMessage, [MainWindow.Handle], _wmSwitchWindows, [PlayWindow.Handle], [ChooseWindow.Handle]

        ret
endp
;------------------------------------------------------------------------------