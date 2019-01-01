HelloWindow                             WINDOWINFO      <'HelloWindowClassName', 0>,\
                                                        <'HelloWindowWindowName', 0>,\
                                                        <<"BMP_BG_HELLO", 0>>

HelloWindow.ExitBmpBtnInfo              BITMAPBUTTONINFO        <"BMP_BTN_EXIT", 0>,\
                                                                <"Exit", 0>, 1,\
                                                                <_wndWidth - _squareButtonWidth, 0, _squareButtonWidth, _squareButtonHeight>

HelloWindow.StartBmpBtnInfo             BITMAPBUTTONINFO        <"BMP_BTN_START", 0>,\
                                                                <"Start", 0>, 1,\
                                                                <_wndWidth/2 - _squareButtonWidth/2, _wndHeight/2 - _squareButtonHeight/2, _squareButtonWidth, _squareButtonHeight>
