// Run command START_VTRAM_UPDATE
WriteAutoIncrement(0x0203, 0x1); // (val=1) / UI.CMD.DEBUG.COMMAND
WaitValueMaskEx(6, 0x0203, 0x0, 0xf, 1); // (val=0) UI.CMD.DEBUG.COMMAND