// Run command END_VTRAM_UPDATE
WriteAutoIncrement(0x0203, 0x2); // (val=2) / UI.CMD.DEBUG.COMMAND
WaitValueMaskEx(6, 0x0203, 0x0, 0xf, 1); // (val=0) UI.CMD.DEBUG.COMMAND
Wait(2);