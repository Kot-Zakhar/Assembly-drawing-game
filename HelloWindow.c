;------------------------------------------------------------------------------
proc HelloWindow.Register
        local wc: WNDCLASS

        mov     [wc.style], 0
        mov     [wc.lpfnWndProc], HelloWindow.WindowProc
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
        mov     [wc.lpszClassName], HelloWindow.ClassName

        invoke RegisterClass, addr wc

        ret
endp ;returns status of registration in eax
;------------------------------------------------------------------------------
proc HelloWindow.Create, hwnd

        invoke  CreateWindowEx, 0, HelloWindow.ClassName, HelloWindow.WindowName,\
                WS_CHILD, \
                [_CurrentScreenSize.left], [_CurrentScreenSize.top], [_CurrentScreenSize.width], [_CurrentScreenSize.height],\
                [hwnd], _psMainWindow, <invoke GetModuleHandle, NULL>, 0

        mov     [HelloWindow.Handle], eax

        ret
endp
;------------------------------------------------------------------------------
proc HelloWindow.WindowProc\
     uses ebx ecx esi edi,\
     hwnd, umsg, wparam, lparam

        ; case----------------------------------
        .if [umsg] = WM_CREATE


                mov     esi, [hwnd]
                mov     [HelloWindow.Handle], esi
                stdcall InitializeWindow, HelloWindow

                ;Creating Exit button
                mov     [HelloWindow.ExitBmpBtnInfo.hwndParent], eSi
                stdcall CreateBitmapButton, HelloWindow.ExitBmpBtnInfo
                ;Creating Start button
                mov     [HelloWindow.StartBmpBtnInfo.hwndParent], eSi
                stdcall CreateBitmapButton, HelloWindow.StartBmpBtnInfo

                mov     eax, 0


        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)
                ;Preparing for real painting
                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], HelloWindow.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                ;Clearing buffer-DC's bitmap
                ; creates new bitmap, because size of window can be changed (in future)
                invoke  GetClientRect, [hwnd], HelloWindow.Buffer.rect
                invoke  CreateCompatibleBitmap, esi, [HelloWindow.Buffer.rect.width], [HelloWindow.Buffer.rect.height]
                invoke  SelectObject, [HelloWindow.Buffer.hdc], eax
                invoke  DeleteObject, eax

                ;Painting on a buffer-DC
                ; copying bg to buffer
                invoke  StretchBlt, [HelloWindow.Buffer.hdc], 0, 0, [HelloWindow.Buffer.rect.width], [HelloWindow.Buffer.rect.height],\
                        [HelloWindow.BG.hdc], 0, 0, [HelloWindow.BG.bm.bmWidth], [HelloWindow.BG.bm.bmHeight], SRCCOPY
                ; drawing Exit button
                stdcall MapPointOnScreen, HelloWindow.ExitBmpBtnInfo.originalRect.left, HelloWindow.ExitBmpBtnInfo.realRect.left, _ScreenScale
                stdcall MapPointOnScreen, HelloWindow.ExitBmpBtnInfo.originalRect.width, HelloWindow.ExitBmpBtnInfo.realRect.width, _ScreenScale

                invoke  TransparentBlt, [HelloWindow.Buffer.hdc],\
                        [HelloWindow.ExitBmpBtnInfo.realRect.left], [HelloWindow.ExitBmpBtnInfo.realRect.top],\
                        [HelloWindow.ExitBmpBtnInfo.realRect.width], [HelloWindow.ExitBmpBtnInfo.realRect.height],\
                        [HelloWindow.ExitBmpBtnInfo.hdc], 0, 0, [HelloWindow.ExitBmpBtnInfo.bm.bmWidth], [HelloWindow.ExitBmpBtnInfo.bm.bmHeight], $00FFFFFF

                ; drawing Start button
                stdcall MapPointOnScreen, HelloWindow.StartBmpBtnInfo.originalRect.left, HelloWindow.StartBmpBtnInfo.realRect.left, _ScreenScale
                stdcall MapPointOnScreen, HelloWindow.StartBmpBtnInfo.originalRect.width, HelloWindow.StartBmpBtnInfo.realRect.width, _ScreenScale

                invoke  TransparentBlt, [HelloWindow.Buffer.hdc],\
                        [HelloWindow.StartBmpBtnInfo.realRect.left], [HelloWindow.StartBmpBtnInfo.realRect.top],\
                        [HelloWindow.StartBmpBtnInfo.realRect.width], [HelloWindow.StartBmpBtnInfo.realRect.height],\
                        [HelloWindow.StartBmpBtnInfo.hdc], 0, 0, [HelloWindow.StartBmpBtnInfo.bm.bmWidth], [HelloWindow.StartBmpBtnInfo.bm.bmHeight], $00FFFFFF



                ;Copying everything from Back DC to RealDC
                invoke  BitBlt, esi, 0, 0, [HelloWindow.Buffer.rect.width], [HelloWindow.Buffer.rect.height],\
                        [HelloWindow.Buffer.hdc], 0, 0, SRCCOPY

                ;ending real painting
                .if ([umsg] = WM_PAINT)
                        invoke EndPaint, [hwnd], HelloWindow.PS
                .endif

                mov     eax, 0

        .elseif ([umsg] = WM_MOUSEMOVE) | ([umsg] = WM_LBUTTONDOWN)

                local   MousePoint      POINT
                stdcall GetPointFromLparam, [lparam], addr MousePoint

                mov     ebx, 0

                .if ~ebx ; Checking for point on start button
                        stdcall IsPointOnButton, addr MousePoint, HelloWindow.StartBmpBtnInfo
                        .if eax
                                mov     ebx, _biHelloStart
                        .endif
                .endif

                .if ~ebx ; Checking for point on exit button
                        stdcall IsPointOnButton, addr MousePoint, HelloWindow.ExitBmpBtnInfo
                        .if eax
                                mov     ebx, _biHelloExit
                        .endif
                .endif


                .if ebx ; if point is on any button
                        mov     esi, [HandCursorHandle]
                        .if [umsg] = WM_LBUTTONDOWN   ; if was a click on a button
                                .if ebx = _biHelloStart    ; if clicked button is Start-button
                                        stdcall StartButtonClicked
                                .elseif ebx = _biHelloExit ; if clicked button is Exit-button
                                        stdcall ExitButtonClicked
                                .endif
                        .endif
                .else
                        mov     esi, [PenCursorHandle]
                .endif

                invoke  SetClassLongA, DWORD [hwnd], GCL_HCURSOR, esi
        .else
                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]
        .endif
        ; endcase ------------------------------
.ProcEnd:
        ret

endp
;------------------------------------------------------------------------------
proc ExitButtonClicked

        invoke PostMessage, [MainWindow.Handle], _wmSwitchWindows, 0, [HelloWindow.Handle]
        invoke PostMessage, [MainWindow.Handle], WM_DESTROY, 0, 0
        ret
endp
;------------------------------------------------------------------------------
proc StartButtonClicked
        invoke PostMessage, [MainWindow.Handle], _wmSwitchWindows, [ChooseWindow.Handle], [HelloWindow.Handle]
        ret
endp
;------------------------------------------------------------------------------