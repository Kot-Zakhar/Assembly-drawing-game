_DefScreenSize                  RECTWH  _wndX, _wndY, _wndWidth, _wndHeight
_CurrentScreenSize              RECTWH  0, 0, ?, ?
_ScreenScale                    POINT   ?, ?


;Common sizes
;_ReturnButtonText               db      'Return', 0
;_ReturnButtonHandle             dd      ?
;_ReturnButtonRect               RECTWH  0, 0, _squareButtonWidth, _squareButtonHeight

_StringBuffer                   db      _stringBufferSize dup 0

; Random data
Random.A                        dd      1103515245
Random.C                        dd      12345
Random.M                        dw      32768
Random.N                        dd      ?

;Erorrs
ErrorMsg.ResourceError          db      'Error: resource is not found!', 0


;Common images
HandCursorHandle                dd      ?
PenCursorHandle                 dd      ?

;Font settings
Default.Font                    FONTINFO        -100, <"Times New Roman", 0>, $00d923fe

;game data
Game.CurrentMode                dd      0       ; one of _gm... (or _biChooseCharacter..) constant to know, what the game was choosen
Game.CurrentTimer               dd      0       ; _biPlayEndless.._biPlay20min to know, what timer was choosen
Game.CurrentHWindow             dd      0       ; handle to game window, that is currently shown while playing game
Game.IsBeingPlayed              dd      FALSE   ; shows if the game is actualy being played at the moment