;format PE GUI 4.0

include 'win32ax.inc'

; headers
include 'Common.h'
include 'HelloWindow.h'
include 'ChooseWindow.h'
include 'PlayWindow.h'

;------------------------------------------------------------------------------
; data section
.data
        include 'Common.d'
        include 'HelloWindow.d'
        include 'ChooseWindow.d'
        include 'PlayWindow.d'
        include 'Main.d'



        ;Globalwc              WNDCLASS <10 dup (0)>
        Globalwc              WNDCLASS 0,0,0,0,0,0,0,0,0,0

        msg             MSG     ?

;end of data section
;------------------------------------------------------------------------------

;code section
.code


startPoint:
        ;calculating scale
        invoke GetSystemMetrics, SM_CXSCREEN
        mov    [_CurrentScreenSize.width], eax
        invoke GetSystemMetrics, SM_CYSCREEN
        mov    [_CurrentScreenSize.height], eax
        stdcall CalculateScale, _ScreenScale, _DefScreenSize, _CurrentScreenSize

        ;loading cursors
        invoke LoadCursor, <invoke GetModuleHandle, NULL>, "IDC_PEN"
        mov     [PenCursorHandle], eax
        invoke LoadCursor, <invoke GetModuleHandle, NULL>, "IDC_HAND"
        mov     [HandCursorHandle], eax

        invoke GetModuleHandle, NULL
        mov     [Globalwc.hInstance], eax

        invoke LoadIcon, 0, IDI_APPLICATION
        mov     [Globalwc.hIcon], eax

        mov     eax, [PenCursorHandle]
        mov     [Globalwc.hCursor], eax

        mov     [Globalwc.style], CS_GLOBALCLASS
        mov     [Globalwc.lpfnWndProc], MainWindow.WindowProc
        mov     [Globalwc.hbrBackground], COLOR_BACKGROUND + 1
        mov     [Globalwc.lpszClassName], MainWindow.ClassName

        ;random init
        stdcall Random.Init

    ; registrating all the window classes ---------

        invoke RegisterClass, Globalwc
        cmp    eax, 0
        je     error


        stdcall HelloWindow.Register
        cmp    eax, 0
        je     error

        stdcall ChooseWindow.Register
        cmp    eax, 0
        je     error

        stdcall PlayWindow.Register
        cmp    eax, 0
        je     error



    ; end of registration -------------------------

    ; creating main window ------------------------


        invoke CreateWindowEx, 0, MainWindow.ClassName, MainWindow.WindowName,\
               WS_VISIBLE xor WS_MINIMIZEBOX xor WS_POPUP,\
               [_CurrentScreenSize.left], [_CurrentScreenSize.top], [_CurrentScreenSize.width], [_CurrentScreenSize.height],\
               0, 0, <invoke GetModuleHandle, NULL>, 0

        cmp    eax, 0
        je     error

        mov    [MainWindow.Handle], eax


    ; end of creation -----------------------------



;        invoke SetWindowLong, hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW

;        invoke ShowWindow, hwnd, SW_SHOW

    ;------ msg loop --------
msg_loop:
        invoke   GetMessage, msg, 0, 0, 0
        cmp      eax, 0
        je       end_loop

        invoke   TranslateMessage, msg
        invoke   DispatchMessage, msg
        jmp      msg_loop

error:
        invoke MessageBox, 0, "Error acured!", "Error", MB_ICONERROR + MB_OK

end_loop:
        invoke ExitProcess, [msg.wParam]

; end of code section
;------------------------------------------------------------------------------




;                              Procedures definitions




;------------------------------------------------------------------------------
;  main window procedure
proc MainWindow.WindowProc\
     uses ebx esi edi,\
     hwnd, umsg, wparam, lparam

        locals
                hbmTemp         dd      ?
        endl

     ; message case --------------------------

        .if [umsg] = WM_COMMAND

                mov     eax, 0

        .elseif [umsg] = _wmSwitchWindows
        ;wparam - handle of the window to be shown
        ;lparam - handle of the window to be hidden

                ; hiding window

                .if [lparam] <> 0
                        invoke  AnimateWindow, [lparam], AW_SPEED, AW_CURRENTANIMATION or AW_HIDE
                .endif

                ; showing window
                .if [wparam] <> 0
                        invoke  AnimateWindow, [wparam], AW_SPEED, AW_CURRENTANIMATION or AW_ACTIVATE
                        ;invoke  RedrawWindow, [wparam], NULL, NULL, RDW_INVALIDATE

                .endif



        .elseif [umsg] = WM_CREATE


                stdcall HelloWindow.Create, [hwnd]
                stdcall ChooseWindow.Create, [hwnd]
                stdcall PlayWindow.Create, [hwnd]
                invoke  PostMessage, [hwnd], _wmSwitchWindows, [HelloWindow.Handle], 0
;                invoke  PostMessage, [hwnd], _wmSwitchWindows, [PlayWindow.Handle], 0


                stdcall InitializeWindow, MainWindow

        .elseif ([umsg] = WM_PAINT) | ([umsg] = WM_PRINT)
                ;Preparing for real painting
                .if ([umsg] = WM_PAINT)
                        invoke  BeginPaint, [hwnd], MainWindow.PS
                        mov     esi, eax
                .else
                        mov     esi, [wparam]
                .endif

                ; Updating info about window size
                invoke  GetClientRect, [hwnd], MainWindow.RealRect

                ;Copying everything from BG DC to RealDC
                invoke  StretchBlt, esi, 0, 0, [MainWindow.RealRect.width], [MainWindow.RealRect.height],\
                        [MainWindow.BG.hdc], 0, 0, [MainWindow.BG.bm.bmWidth], [MainWindow.BG.bm.bmHeight], SRCCOPY

                ;ending real painting
                .if ([umsg] = WM_PAINT)
                        invoke EndPaint, [hwnd], MainWindow.PS
                .endif

                mov     eax, 0


        .elseif [umsg] = WM_DESTROY

                invoke  SendMessage, [hwnd], _wmSwitchWindows, 0, [hwnd]

                invoke  PostQuitMessage, 0
                mov     eax, 0


        .else

                invoke  DefWindowProc, [hwnd], [umsg], [wparam], [lparam]

        .endif

     ; endcase ------------------------------

        ret
endp
;------------------------------------------------------------------------------
include 'Common.c'
include 'HelloWindow.c'
include 'ChooseWindow.c'
include 'PlayWindow.c'

section '.rsrc' data resource from 'data.res' readable


.end startPoint
                              