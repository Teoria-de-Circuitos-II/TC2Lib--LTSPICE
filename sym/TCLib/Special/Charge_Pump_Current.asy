Version 4
SymbolType CELL
LINE Normal 32 0 64 0
LINE Normal 32 -12 32 0
LINE Normal 32 0 32 12
LINE Normal 32 -16 32 -32
LINE Normal 34 -22 32 -16
LINE Normal 30 -22 32 -16
LINE Normal 32 32 32 16
LINE Normal 34 26 32 32
LINE Normal 30 26 32 32
LINE Normal 32 -45 32 -36
LINE Normal 32 45 32 36
RECTANGLE Normal 64 -64 -64 65
CIRCLE Normal 44 -12 20 -36
CIRCLE Normal 44 36 20 12
CIRCLE Normal 35 -45 29 -51
CIRCLE Normal 29 45 35 51
TEXT -92 21 Left 2 Vi
TEXT -23 94 Bottom 2 Vf
TEXT 98 20 Right 2 Vo
TEXT -5 51 Left 2 -
TEXT -54 -2 Left 2 +
TEXT -50 -76 Left 1 Charge Pump
WINDOW 3 21 76 Left 0
WINDOW 123 21 92 Left 0
SYMATTR Value Vcc=5
SYMATTR Value2 Current=1m
SYMATTR Prefix X
SYMATTR SpiceModel Charge_Pump
SYMATTR Description Charge_Pump
SYMATTR ModelFile TCLib/Charge_Pump.lib
PIN -64 0 NONE 8
PINATTR PinName Vi
PINATTR SpiceOrder 1
PIN 0 64 NONE 8
PINATTR PinName Vf
PINATTR SpiceOrder 2
PIN 64 0 NONE 8
PINATTR PinName Vf1
PINATTR SpiceOrder 3
