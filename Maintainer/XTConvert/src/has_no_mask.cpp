#include <array>

namespace XTConvert
{

static std::array<bool, 371> BlockHasNoMask;
static std::array<bool, 191> BackgroundHasNoMask;

void init_mask_arrays()
{
    // init BackgroundHasNoMask
    BackgroundHasNoMask[187] = true;
    BackgroundHasNoMask[188] = true;
    BackgroundHasNoMask[189] = true;
    BackgroundHasNoMask[190] = true;

    BackgroundHasNoMask[172] = true;
    BackgroundHasNoMask[167] = true;
    BackgroundHasNoMask[164] = true;
    BackgroundHasNoMask[165] = true;
    BackgroundHasNoMask[158] = true;
    BackgroundHasNoMask[146] = true;
    BackgroundHasNoMask[12] = true;
    BackgroundHasNoMask[14] = true;
    BackgroundHasNoMask[15] = true;
    BackgroundHasNoMask[22] = true;
    BackgroundHasNoMask[30] = true;
    BackgroundHasNoMask[39] = true;
    BackgroundHasNoMask[40] = true;
    BackgroundHasNoMask[41] = true;
    BackgroundHasNoMask[42] = true;
    BackgroundHasNoMask[43] = true;
    BackgroundHasNoMask[44] = true;
    BackgroundHasNoMask[47] = true;
    BackgroundHasNoMask[52] = true;
    BackgroundHasNoMask[53] = true;
    BackgroundHasNoMask[55] = true;
    BackgroundHasNoMask[56] = true;
    BackgroundHasNoMask[60] = true;
    BackgroundHasNoMask[61] = true;
    BackgroundHasNoMask[64] = true;
    BackgroundHasNoMask[75] = true;
    BackgroundHasNoMask[76] = true;
    BackgroundHasNoMask[77] = true;
    BackgroundHasNoMask[78] = true;
    BackgroundHasNoMask[79] = true;
    BackgroundHasNoMask[83] = true;
    BackgroundHasNoMask[87] = true;
    BackgroundHasNoMask[88] = true;
    BackgroundHasNoMask[91] = true;
    BackgroundHasNoMask[98] = true;
    BackgroundHasNoMask[99] = true;
    BackgroundHasNoMask[107] = true;
    BackgroundHasNoMask[115] = true;
    BackgroundHasNoMask[116] = true;
    BackgroundHasNoMask[117] = true;
    BackgroundHasNoMask[118] = true;
    BackgroundHasNoMask[119] = true;
    BackgroundHasNoMask[122] = true;
    BackgroundHasNoMask[123] = true;
    BackgroundHasNoMask[124] = true;

    BackgroundHasNoMask[139] = true;
    BackgroundHasNoMask[140] = true;
    BackgroundHasNoMask[141] = true;
    BackgroundHasNoMask[144] = true;
    BackgroundHasNoMask[145] = true;

    BackgroundHasNoMask[169] = true;


    // init BlockHasNoMask
    BlockHasNoMask[336] = true;
    BlockHasNoMask[337] = true;
    BlockHasNoMask[338] = true;
    BlockHasNoMask[339] = true;
    BlockHasNoMask[303] = true;
    BlockHasNoMask[304] = true;
    BlockHasNoMask[348] = true;
    BlockHasNoMask[353] = true;
    BlockHasNoMask[354] = true;
    BlockHasNoMask[355] = true;
    BlockHasNoMask[356] = true;
    BlockHasNoMask[3] = true;
    BlockHasNoMask[4] = true;
    BlockHasNoMask[13] = true;
    BlockHasNoMask[15] = true;
    BlockHasNoMask[16] = true;
    BlockHasNoMask[17] = true;
    BlockHasNoMask[19] = true;
    BlockHasNoMask[21] = true;
    BlockHasNoMask[34] = true;
    BlockHasNoMask[36] = true;
    BlockHasNoMask[23] = true;
    BlockHasNoMask[29] = true;
    BlockHasNoMask[40] = true;
    BlockHasNoMask[43] = true;
    BlockHasNoMask[46] = true;
    BlockHasNoMask[47] = true;
    BlockHasNoMask[48] = true;
    BlockHasNoMask[49] = true;
    BlockHasNoMask[50] = true;
    BlockHasNoMask[51] = true;
    BlockHasNoMask[52] = true;
    BlockHasNoMask[53] = true;
    BlockHasNoMask[54] = true;
    BlockHasNoMask[59] = true;
    BlockHasNoMask[60] = true;
    BlockHasNoMask[61] = true;
    BlockHasNoMask[63] = true;
    BlockHasNoMask[65] = true;
    BlockHasNoMask[67] = true;
    BlockHasNoMask[70] = true;
    BlockHasNoMask[71] = true;
    BlockHasNoMask[72] = true;
    BlockHasNoMask[73] = true;
    BlockHasNoMask[74] = true;
    BlockHasNoMask[75] = true;
    BlockHasNoMask[76] = true;
    BlockHasNoMask[81] = true;
    BlockHasNoMask[83] = true;
    BlockHasNoMask[84] = true;
    BlockHasNoMask[85] = true;
    BlockHasNoMask[86] = true;
    BlockHasNoMask[87] = true;
    BlockHasNoMask[91] = true;
    BlockHasNoMask[93] = true;
    BlockHasNoMask[94] = true;
    BlockHasNoMask[95] = true;
    BlockHasNoMask[96] = true;
    BlockHasNoMask[97] = true;
    BlockHasNoMask[98] = true;
    BlockHasNoMask[99] = true;
    BlockHasNoMask[100] = true;
    BlockHasNoMask[101] = true;
    BlockHasNoMask[102] = true;
    BlockHasNoMask[103] = true;
    BlockHasNoMask[111] = true;
    BlockHasNoMask[118] = true;
    BlockHasNoMask[119] = true;
    BlockHasNoMask[120] = true;
    BlockHasNoMask[121] = true;
    BlockHasNoMask[122] = true;
    BlockHasNoMask[123] = true;
    BlockHasNoMask[124] = true;
    BlockHasNoMask[125] = true;
    BlockHasNoMask[126] = true;
    BlockHasNoMask[127] = true;
    BlockHasNoMask[131] = true;
    BlockHasNoMask[134] = true;
    BlockHasNoMask[136] = true;
    BlockHasNoMask[159] = true;
    BlockHasNoMask[160] = true;
    BlockHasNoMask[166] = true;
    BlockHasNoMask[183] = true;
    BlockHasNoMask[184] = true;
    BlockHasNoMask[186] = true;
    BlockHasNoMask[187] = true;
    BlockHasNoMask[188] = true;
    BlockHasNoMask[190] = true;
    BlockHasNoMask[198] = true;
    BlockHasNoMask[199] = true;
    BlockHasNoMask[200] = true;
    BlockHasNoMask[201] = true;
    BlockHasNoMask[202] = true;
    BlockHasNoMask[203] = true;
    BlockHasNoMask[204] = true;
    BlockHasNoMask[205] = true;
    BlockHasNoMask[206] = true;
    BlockHasNoMask[216] = true;
    BlockHasNoMask[217] = true;
    BlockHasNoMask[218] = true;
    BlockHasNoMask[223] = true;
    BlockHasNoMask[226] = true;
    BlockHasNoMask[227] = true;
    BlockHasNoMask[228] = true;
    BlockHasNoMask[229] = true;
    BlockHasNoMask[230] = true;
    BlockHasNoMask[231] = true;
    BlockHasNoMask[232] = true;
    BlockHasNoMask[233] = true;
    BlockHasNoMask[234] = true;
    BlockHasNoMask[235] = true;
    BlockHasNoMask[236] = true;
    BlockHasNoMask[237] = true;
    BlockHasNoMask[238] = true;
    BlockHasNoMask[239] = true;
    BlockHasNoMask[250] = true;
    BlockHasNoMask[251] = true;
    BlockHasNoMask[252] = true;
    BlockHasNoMask[253] = true;
    BlockHasNoMask[254] = true;
    BlockHasNoMask[255] = true;
    BlockHasNoMask[256] = true;
    BlockHasNoMask[257] = true;
    BlockHasNoMask[258] = true;
    // BlockHasNoMask[261] = true; // invalid, Block 261 is sizable and this flag is never read for sizable blocks in SMBX 1.3
    BlockHasNoMask[262] = true;
    BlockHasNoMask[264] = true;
    BlockHasNoMask[263] = true;
    BlockHasNoMask[273] = true;
    BlockHasNoMask[272] = true;
    BlockHasNoMask[276] = true;
    BlockHasNoMask[281] = true;
    BlockHasNoMask[282] = true;
    BlockHasNoMask[283] = true;
    BlockHasNoMask[291] = true;
    BlockHasNoMask[292] = true;
    BlockHasNoMask[320] = true;
    BlockHasNoMask[322] = true;
    BlockHasNoMask[323] = true;
    BlockHasNoMask[330] = true;
    BlockHasNoMask[331] = true;
    BlockHasNoMask[369] = true;
    BlockHasNoMask[370] = true;
}

bool check_BlockHasNoMask(unsigned int index)
{
    if(index < BlockHasNoMask.size())
        return BlockHasNoMask[index];

    return false;
}

bool check_BackgroundHasNoMask(unsigned int index)
{
    if(index < BackgroundHasNoMask.size())
        return BackgroundHasNoMask[index];

    return false;
}

} // namespace XTConvert
