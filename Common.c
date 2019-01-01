proc InitializeWindow\
     uses eax ebx esi edi,\
     pWindowInfo
        local   hbmTemp         dd      ?

        mov     edi, [pWindowInfo]

        invoke  GetDC, [edi + WINDOWINFO.Handle]
        mov     esi, eax

        ;Creating Buffer-hdc
        ;bitmap will be created
        invoke  CreateCompatibleDC, esi
        mov     [edi + WINDOWINFO.Buffer.hdc], eax
        mov     ebx, edi
        add     ebx, WINDOWINFO.Buffer.rect
        invoke  GetClientRect, [edi + WINDOWINFO.Handle], ebx
        invoke  CreateCompatibleBitmap, esi, [edi + WINDOWINFO.Buffer.rect.width], [edi + WINDOWINFO.Buffer.rect.height]
        invoke  SelectObject, [edi + WINDOWINFO.Buffer.hdc], eax
        mov     [edi + WINDOWINFO.Buffer.oldDC.hOldBitmap], eax

        invoke  CreateFontW, DWORD [Default.Font.height], DWORD [Default.Font.width],\
                             DWORD [Default.Font.escapement], DWORD [Default.Font.orientation],\
                             DWORD [Default.Font.weight], DWORD [Default.Font.italic],\
                             DWORD [Default.Font.underline], DWORD [Default.Font.strikeOut],\
                             DWORD [Default.Font.charSet], DWORD [Default.Font.outPrecision],\
                             DWORD [Default.Font.clipPrecision], DWORD [Default.Font.quality],\
                             DWORD [Default.Font.pitch], Default.Font.faceName

        invoke  SelectObject, [edi + WINDOWINFO.Buffer.hdc], eax
        mov     [edi + WINDOWINFO.Buffer.oldDC.hOldFont], eax
        invoke  SetTextColor, [edi + WINDOWINFO.Buffer.hdc], [Default.Font.color]
        invoke  SetBkMode, [edi + WINDOWINFO.Buffer.hdc], TRANSPARENT

        ;Creating BG-hdc with image from resource
        invoke  CreateCompatibleDC, esi
        mov     [edi + WINDOWINFO.BG.hdc], eax
        ;Loading image into BG-DC
        mov     ebx, edi
        add     ebx, WINDOWINFO.BG.resourceName
        invoke  LoadImageA, <invoke GetModuleHandle, NULL>, ebx, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR
        mov     [hbmTemp], eax
        .if ~eax
                invoke MessageBox, 0, ErrorMsg.ResourceError, ebx, MB_ICONERROR + MB_OK
                invoke ExitProcess, 0
        .endif
        mov     ebx, edi
        add     ebx, WINDOWINFO.BG.bm
        invoke  GetObject, [hbmTemp], sizeof.BITMAP, ebx
        invoke  SelectObject, [edi + WINDOWINFO.BG.hdc], [hbmTemp]
        mov     [edi + WINDOWINFO.BG.oldDC.hOldBitmap], eax

        invoke  ReleaseDC, [edi + WINDOWINFO.Handle], esi

        ret
endp
;------------------------------------------------------------------------------
proc MapPointOnScreen\
     uses ecx esi edi ebx,\
     fromPointAddr, toPointAddr, scalePointAddr
        lea   esi, [fromPointAddr]
        mov   esi, [esi]
        lea   edi, [toPointAddr]
        mov   edi, [edi]
        lea   ebx, [scalePointAddr]
        mov   ebx, [ebx]

        ; calculating x
        fild  dword [esi + POINT.x]
        fld   dword [ebx + POINT.x]
        fmulp
        fistp dword [edi + POINT.x]

        ; calculating y
        fild  dword [esi + POINT.y]
        fld   dword [ebx + POINT.y]
        fmulp
        fistp dword [edi + POINT.y]

        mov   eax, edi

        ret
endp
;------------------------------------------------------------------------------
proc CalculateScale\
     uses ecx esi edi ebx ,\
     scalePointAddr, fromRectAddr, toRectAddr

        local temp:DWORD

        lea   esi, [fromRectAddr]
        mov   esi, [esi]
        lea   edi, [toRectAddr]
        mov   edi, [edi]
        lea   ebx, [scalePointAddr]
        mov   ebx, [ebx]

        ; calculating x-scale
        mov   ecx, dword [edi + RECT.right]
        sub   ecx, dword [edi + RECT.left]
        mov   [temp], ecx
        fild  dword [temp]
        mov   ecx, dword [esi + RECT.right]
        sub   ecx, dword [esi + RECT.left]
        mov   [temp], ecx
        fild  dword [temp]
        fdivp
        fstp  dword [ebx + POINT.x]

        ; calculating y-scale
        mov   ecx, dword [edi + RECT.bottom]
        sub   ecx, dword [edi + RECT.top]
        mov   [temp], ecx
        fild  dword [temp]
        mov   ecx, dword [esi + RECT.bottom]
        sub   ecx, dword [esi + RECT.top]
        mov   [temp], ecx
        fild  dword [temp]
        fdivp
        fstp  dword [ebx + POINT.y]

        mov   eax, edi

        ret
endp
;------------------------------------------------------------------------------
;function expects this values to be initialized:
;    hwndParent
;    pstrBmpResourceName
;    originalRect
;others values would be filled during the process
proc CreateBitmapButton\
     uses ebx esi edi,\
     pBmpBtnInfo
        local   hbmTemp         dd      ?

        mov     edi, [pBmpBtnInfo]

        mov     ebx, edi
        add     ebx, BITMAPBUTTONINFO.hwndParent
        invoke  GetDC, DWORD [ebx]
        mov     esi, eax

        invoke  CreateCompatibleDC, esi
        mov     ebx, edi
        add     ebx, BITMAPBUTTONINFO.hdc
        mov     [ebx], eax
        mov     ebx, edi
        add     ebx, BITMAPBUTTONINFO.bmpResourceName
        invoke  LoadImageA, <invoke GetModuleHandle, NULL>, ebx, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR
        mov     [hbmTemp], eax
;        invoke MessageBox, 0, ErrorMsg.ResourceError, HelloWindow.ExitBmpBtnInfo.bmpResourceName, MB_ICONERROR + MB_OK
        .if ~eax
                mov     ebx, edi
                add     ebx, BITMAPBUTTONINFO.bmpResourceName
                invoke MessageBox, 0, ErrorMsg.ResourceError, ebx, MB_ICONERROR + MB_OK
                ;invoke ExitProcess, 0
                jmp    .ProcEnd
        .endif
        mov     ebx, edi
        add     ebx, BITMAPBUTTONINFO.bm
        invoke  GetObject, DWORD [hbmTemp], sizeof.BITMAP, ebx
        mov     ebx, edi
        add     ebx, BITMAPBUTTONINFO.hdc
        invoke  SelectObject, DWORD [ebx], DWORD [hbmTemp]
        mov     ebx, edi
        add     ebx, BITMAPBUTTONINFO.oldDC.hOldBitmap
        mov     [ebx], eax

        ;invoke MessageBox, 0, ErrorMsg.ResourceError, HelloWindow.ExitBmpBtnInfo.bmpResourceName, MB_ICONERROR + MB_OK

 .ProcEnd:
        mov     ebx, edi
        add     ebx, BITMAPBUTTONINFO.hwndParent
        invoke  ReleaseDC, DWORD [ebx], esi
        ret
endp
;------------------------------------------------------------------------------
proc DrawBitmapButton\
     uses esi edi ebx ecx eax,\
     hdc, pButtonInfo, isText  ; to add screenScale

        local   realRect        RECTWH
        local   tempRect        RECT

        mov     esi, [pButtonInfo]
        mov     ebx, esi
        add     ebx, BITMAPBUTTONINFO.visible
        mov     ebx, [ebx]
        .if ebx ;if it's visible
                add     esi, BITMAPBUTTONINFO.originalRect.left
                stdcall MapPointOnScreen, esi, addr realRect.left, _ScreenScale
                add     esi, RECTWH.width
                stdcall MapPointOnScreen, esi, addr realRect.width, _ScreenScale

                mov     esi, [pButtonInfo]
                mov     edi, esi
                mov     ebx, esi
                add     esi, BITMAPBUTTONINFO.hdc
                add     edi, BITMAPBUTTONINFO.bm.bmWidth
                add     ebx, BITMAPBUTTONINFO.bm.bmHeight
                invoke  TransparentBlt, [hdc], [realRect.left], [realRect.top], [realRect.width], [realRect.height],\
                        DWORD [esi], 0, 0, DWORD [edi], DWORD [ebx], $00FFFFFF

                ;copying from local realRect to pButtonInfo->realRect
                mov     edi, [pButtonInfo]
                add     edi, BITMAPBUTTONINFO.realRect
                lea     esi, [realRect]
                mov     ecx, 4
                rep movsd

                .if [isText]
                        lea     esi, [realRect]
                        mov     eax, [esi + RECTWH.left]
                        mov     [tempRect.left], eax
                        add     eax, [esi + RECTWH.width]
                        mov     [tempRect.right], eax
                        mov     eax, [esi + RECTWH.top]
                        mov     [tempRect.top], eax
                        add     eax, [esi + RECTWH.height]
                        mov     [tempRect.bottom], eax
                        mov     esi, [pButtonInfo]
                        add     esi, BITMAPBUTTONINFO.text
                        invoke  DrawText, [hdc], esi, -1, addr tempRect, DT_CENTER or DT_VCENTER or DT_SINGLELINE
                .endif

        .endif
        ret
endp
;------------------------------------------------------------------------------
proc IsPointOnButton\
     uses edi esi,\
     pPoint, pBtnInfo
        local   ButtonRect      RECT

        mov     esi, [pBtnInfo]
        mov     edi, [pPoint]

        mov     eax, [esi + BITMAPBUTTONINFO.realRect.left]
        mov     [ButtonRect.left], eax
        add     eax, [esi + BITMAPBUTTONINFO.realRect.width]
        mov     [ButtonRect.right], eax
        mov     eax, [esi + BITMAPBUTTONINFO.realRect.top]
        mov     [ButtonRect.top], eax
        add     eax, [esi + BITMAPBUTTONINFO.realRect.height]
        mov     [ButtonRect.bottom], eax
        invoke PtInRect, addr ButtonRect, DWORD [edi + POINT.x], DWORD [edi + POINT.y]

        ret
endp
;------------------------------------------------------------------------------
proc GetPointFromLparam\
     uses eax ebx edi,\
     lparam, pPoint
        local   MousePoints     POINTS

        mov     edi, [pPoint]
        mov     eax, [lparam]
        mov     [MousePoints], eax
        mov     bx, [MousePoints.x]
        movzx   eax, bx
        mov     [edi + POINT.x], eax
        mov     bx, [MousePoints.y]
        movzx   eax, bx
        mov     [edi + POINT.y], eax

        ret
endp
;------------------------------------------------------------------------------
proc GetCleanStringBuffer\
     uses ecx ebx

        mov     ecx, _stringBufferSize
        .repeat
        mov     ebx, ecx
        mov     [_StringBuffer + ebx], 0
        .until  ecx = 0
        mov     eax, _StringBuffer

endp
;------------------------------------------------------------------------------
proc Random.Init\
     uses ecx edx

        invoke  GetTickCount
        xor     edx, edx
        movzx   ecx, [Random.M]
        div     ecx
        mov     [Random.N], edx

        ret
endp
;------------------------------------------------------------------------------
proc GetRandom\
     uses ecx edx,\
     min, max

        mov     eax, [Random.A]
        mov     ecx, [Random.N]
        mul     ecx
        add     eax, [Random.C]
        adc     edx, 0
        mov     ecx, 65536
        div     ecx              ; eax = next / 65536
        mov     edx, eax
        shr     edx, 16
        mov     cx, [Random.M]
        div     cx
        mov     eax, [max]
        sub     eax, [min]
        inc     eax
        mov     ecx, eax
        movzx   eax, dx          ; ax = Random value
        mov     [Random.N], eax
        xor     edx, edx
        div     cx
        movzx   eax, dx
        add     eax, [min]
        ret
endp


