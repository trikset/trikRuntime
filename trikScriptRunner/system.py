import os
from enum import IntEnum

class KeysEnum(IntEnum):
    Left = 105
    Up = 103
    Down = 108
    Enter = 28
    Right = 106
    Power = 116
    Esc = 1

class Events(IntEnum):
    Sync = 0
    Key = 1
    Relative = 2
    Absolute = 3
    Misc = 4

class MouseEventCodes(IntEnum):
    X = 0
    Y = 1
    Wheel = 8

    LeftBtn = 272
    RightBtn = 273
    MiddleBtn = 274

class PadEventCodes(IntEnum):
    BtnA = 304
    BtnB = 305
    BtnC = 306
    BtnX = 307
    BtnY = 308
    BtnZ = 309

    BtnTL = 310
    BtnTR = 311
    BtnTL2 = 312
    BtnTR2 = 313
    BtnSelect = 314
    BtnStart = 315

    X = 0
    Y = 1
    Z = 2
    Rx = 3
    Ry = 4
    Rz = 5

    Throttle = 6
    Rudder = 7
    Wheel = 8
    Gas = 9
    Break = 10

    Hat0X = 16
    Hat0Y = 17
    Hat1X = 18
    Hat1Y = 19


script.repeat = lambda mscs, f : script.timer(mscs).connect(f)

brick.smile = lambda : brick.display().showImage('media/trik_smile_normal.png')

brick.sadSmile = lambda : brick.display().showImage('media/trik_smile_sad.png')

include = lambda f : eval(script.readAll(f).join(" "))
