PlayWindow                      WINDOWINFO      <'PlayWindowClassName', 0>,\
                                                <'PlayWindowWindowName', 0>,\
                                                <<'BMP_BG_PLAY', 0>>

PlayWindow.ReturnBmpBtnInfo     BITMAPBUTTONINFO        <"BMP_BTN_RETURN", 0>,\
                                                        <"Return", 0>, 1,\
                                                        <0, 0, _squareButtonWidth, _squareButtonHeight / 2>

PlayWindow.RestartBmpBtnInfo    BITMAPBUTTONINFO        <"BMP_BTN_RESTART", 0>,\
                                                        <"Restart", 0>, 0,\
                                                        <0, _squareButtonHeight + 20, _squareButtonWidth, _squareButtonHeight / 2>

PlayWindow.ChangeTimerBmpBtnInfo BITMAPBUTTONINFO       <"BMP_BTN_CHANGE_TIMER", 0>,\
                                                        <"Change timer", 0>, 0,\
                                                        <0, (_squareButtonHeight + 20) * 2, _squareButtonWidth, _squareButtonHeight / 2>

PlayWindow.EndlessTimerBmpBtnInfo BITMAPBUTTONINFO      <"BMP_BTN_CHOOSE_TIMER", 0>,\
                                                        <"Endless", 0>, 1,\
                                                        <_gameWindowX + _gameWindowWidth / 2  - _timerButtonWidth - 10,\
                                                         _gameWindowY + _gameWindowHeight / 4 - _timerButtonHeight / 2,\
                                                         _timerButtonWidth, _timerButtonHeight>
PlayWindow.1minTimerBmpBtnInfo  BITMAPBUTTONINFO        <"BMP_BTN_CHOOSE_TIMER", 0>,\
                                                        <"1 min", 0>, 1,\
                                                        <_gameWindowX + _gameWindowWidth / 2 + 10,\
                                                         _gameWindowY + _gameWindowHeight / 4 - _timerButtonHeight / 2,\
                                                         _timerButtonWidth, _timerButtonHeight>
PlayWindow.2minTimerBmpBtnInfo  BITMAPBUTTONINFO        <"BMP_BTN_CHOOSE_TIMER", 0>,\
                                                        <"2 min", 0>, 1,\
                                                        <_gameWindowX + _gameWindowWidth / 2  - _timerButtonWidth - 10,\
                                                         _gameWindowY + _gameWindowHeight / 2 - _timerButtonHeight / 2,\
                                                         _timerButtonWidth, _timerButtonHeight>
PlayWindow.5minTimerBmpBtnInfo  BITMAPBUTTONINFO        <"BMP_BTN_CHOOSE_TIMER", 0>,\
                                                        <"5 min", 0>, 1,\
                                                        <_gameWindowX + _gameWindowWidth / 2 + 10,\
                                                         _gameWindowY + _gameWindowHeight / 2 - _timerButtonHeight / 2,\
                                                         _timerButtonWidth, _timerButtonHeight>
PlayWindow.10minTimerBmpBtnInfo BITMAPBUTTONINFO        <"BMP_BTN_CHOOSE_TIMER", 0>,\
                                                        <"10 min", 0>, 1,\
                                                        <_gameWindowX + _gameWindowWidth / 2  - _timerButtonWidth - 10,\
                                                         _gameWindowY + _gameWindowHeight / 4 * 3 - _timerButtonHeight / 2,\
                                                         _timerButtonWidth, _timerButtonHeight>
PlayWindow.20minTimerBmpBtnInfo BITMAPBUTTONINFO        <"BMP_BTN_CHOOSE_TIMER", 0>,\
                                                        <"20 min", 0>, 1,\
                                                        <_gameWindowX + _gameWindowWidth / 2 + 10,\
                                                         _gameWindowY + _gameWindowHeight / 4 * 3 - _timerButtonHeight / 2,\
                                                         _timerButtonWidth, _timerButtonHeight>

GameWindow.Rect                 RECTWH  _gameWindowX, _gameWindowY, _gameWindowWidth, _gameWindowHeight

CharacterGame                   WINDOWINFO      <'CharacterWindowClassName', 0>,\
                                                <'CharacterWindowWindowName', 0>,\
                                                <<'BMP_BG_GAME', 0>>
CharacterGame.Buffer1           db              100 dup 0
CharacterGame.Amount1           dd              0
CharacterGame.Buffer2           db              100 dup 0
CharacterGame.Amount2           dd              0
CharacterGame.Buffer3           db              100 dup 0
CharacterGame.Amount3           dd              0
CharacterGame.Buffer4           db              100 dup 0
CharacterGame.Amount4           dd              0
CharacterGame.LineHeight        dd              200

ScribbleGame                    WINDOWINFO      <'ScribbleWindowClassName', 0>,\
                                                <'ScribbleWindowWindowName', 0>,\
                                                <<'BMP_BG_GAME', 0>>

ScribbleGame.Amount             dd              10
ScribbleGame.SourceName         db              'BMP_SCRIBBLE_0'
ScribbleGame.SourceNumber       db              '0', 0

CopyGame                        WINDOWINFO      <'CopyWindowClassName', 0>,\
                                                <'CopyWindowWindowName', 0>,\
                                                <<'BMP_BG_GAME', 0>>

CopyGame.Amount                 dd              10
CopyGame.SourceName             db              'BMP_Copy_0'
CopyGame.SourceNumber           db              '0', 0

EnvironmentGame                 WINDOWINFO      <'EnvironmentWindowClassName', 0>,\
                                                <'EnvironmentWindowWindowName', 0>,\
                                                <<'BMP_BG_GAME', 0>>

EnvironmentGame.Buffer1         db              100 dup 0
EnvironmentGame.Amount1         dd              0
EnvironmentGame.Buffer2         db              100 dup 0
EnvironmentGame.Amount2         dd              0
EnvironmentGame.Buffer3         db              100 dup 0
EnvironmentGame.Amount3         dd              0
EnvironmentGame.LineHeight      dd              200


MixGame                         WINDOWINFO      <'MixWindowClassName', 0>,\
                                                <'MixWindowWindowName', 0>,\
                                                <<'BMP_BG_GAME', 0>>
IncrementalGame                 WINDOWINFO      <'IncrementalWindowClassName', 0>,\
                                                <'IncrementalWindowWindowName', 0>,\
                                                <<'BMP_BG_GAME', 0>>

Timer.Dots                      dd              TRUE
Timer.FormatWithDots            db              '%d:%d', 0
Timer.FormatWithoutDots         db              '%d %d', 0
Timer.FormatTimeOut             db              '%s', 0
Timer.strToPrint                db              15 dup 0
Timer.strTimeOut                db              'Time is out', 0
Timer.TimeLeft                  dd              0
Timer.Rect                      RECTWH          ?


