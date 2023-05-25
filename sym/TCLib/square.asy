Version 4
SymbolType CELL
LINE Normal 0 -25 -16 -25
LINE Normal 0 8 0 -25
LINE Normal 16 8 0 8
LINE Normal 0 -40 0 -48
LINE Normal 0 32 0 24
LINE Normal 16 -38 16 -48
LINE Normal 11 -43 21 -43
LINE Normal -16 8 -16 -25
LINE Normal -21 8 -16 8
LINE Normal 16 -24 16 8
LINE Normal 21 -24 16 -24
CIRCLE Normal 32 -40 -32 24
WINDOW 38 85 -22 Center 2
WINDOW 3 228 6 Center 2
WINDOW 0 58 -40 Center 0
SYMATTR SpiceModel Square
SYMATTR Value Vpp=1 Freq=1k Voff=0 DC=0.5
SYMATTR Prefix X
SYMATTR Value2 trise=1n tfall=1n Nper=1k
SYMATTR ModelFile TCLib/square.lib
PIN 0 -48 NONE 8
PINATTR PinName V+
PINATTR SpiceOrder 1
PIN 0 32 NONE 8
PINATTR PinName V-
PINATTR SpiceOrder 2
