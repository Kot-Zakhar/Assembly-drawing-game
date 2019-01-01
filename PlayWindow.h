; Button indexes
_biPlayReturn           =       3001
_biPlayRestart          =       3002
_biPlayChangeTimer      =       3003

_biPlayEndless          =       3004
_biPlay1min             =       3005
_biPlay2min             =       3006
_biPlay5min             =       3007
_biPlay10min            =       3008
_biPlay20min            =       3009

_gameWindowX            =       320
_gameWindowY            =       110
_gameWindowWidth        =       550
_gameWindowHeight       =       530

_timerButtonWidth       =       200
_timerButtonHeight      =       100

_mainTimerIndex         =       1       ;main timer, that actualy count time
_tickTimerIndex         =       2       ;timer, that send message each half second to tick-tack (show-hide) ":"
_secondTimerIndex       =       3       ;timer, that send message each second

;that type of message tells a game window to be ready to be shown:
; prepare all images and strings randomly
_wmPrepareSelf          =       WM_USER + 10
_wmCleanSelf            =       WM_USER + 11

;time in ms
;_testTime               =       5000
_1min                   =       60000
_2min                   =       120000
_5min                   =       300000
_10min                  =       600000
_20min                  =       1200000
_timeAfterTimeout       =       2000

_tick                   =       500
_second                 =       1000

;string resources
; start indexes
; strings with following id's contain amount of defined resources
; strings starting from id+1 till id+[id] - strings with data
_strResCharacter1       =       1000
_strResCharacter2       =       2000
_strResCharacter3       =       3000
_strResCharacter4       =       4000
_strResEnvironment1     =       5000
_strResEnvironment2     =       6000
_strResEnvironment3     =       7000