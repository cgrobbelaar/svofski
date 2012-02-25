// =====================================================================
//	  Look-Up Tables
//		DIV: 1/(x+1)
//
//	Exported by Cearn's excellut v0.9
//	(comments, kudos, flames to daytshen@hotmail.com)
//
// =====================================================================

#include "divlut.h"

// -----------------------------------------------------------------------
// DIV: a 160 long LUT of 32bit values in 16.16 format
// 1/(x+1)
const signed int DIV[160]=
{
	0x00010000,0x00008000,0x00005555,0x00004000,0x00003333,0x00002AAB,0x00002492,0x00002000,
	0x00001C72,0x0000199A,0x00001746,0x00001555,0x000013B1,0x00001249,0x00001111,0x00001000,
	0x00000F0F,0x00000E39,0x00000D79,0x00000CCD,0x00000C31,0x00000BA3,0x00000B21,0x00000AAB,
	0x00000A3D,0x000009D9,0x0000097B,0x00000925,0x000008D4,0x00000889,0x00000842,0x00000800,
	0x000007C2,0x00000788,0x00000750,0x0000071C,0x000006EB,0x000006BD,0x00000690,0x00000666,
	0x0000063E,0x00000618,0x000005F4,0x000005D1,0x000005B0,0x00000591,0x00000572,0x00000555,
	0x00000539,0x0000051F,0x00000505,0x000004EC,0x000004D5,0x000004BE,0x000004A8,0x00000492,
	0x0000047E,0x0000046A,0x00000457,0x00000444,0x00000432,0x00000421,0x00000410,0x00000400,

	0x000003F0,0x000003E1,0x000003D2,0x000003C4,0x000003B6,0x000003A8,0x0000039B,0x0000038E,
	0x00000382,0x00000376,0x0000036A,0x0000035E,0x00000353,0x00000348,0x0000033E,0x00000333,
	0x00000329,0x0000031F,0x00000316,0x0000030C,0x00000303,0x000002FA,0x000002F1,0x000002E9,
	0x000002E0,0x000002D8,0x000002D0,0x000002C8,0x000002C1,0x000002B9,0x000002B2,0x000002AB,
	0x000002A4,0x0000029D,0x00000296,0x0000028F,0x00000289,0x00000283,0x0000027C,0x00000276,
	0x00000270,0x0000026A,0x00000264,0x0000025F,0x00000259,0x00000254,0x0000024E,0x00000249,
	0x00000244,0x0000023F,0x0000023A,0x00000235,0x00000230,0x0000022B,0x00000227,0x00000222,
	0x0000021E,0x00000219,0x00000215,0x00000211,0x0000020C,0x00000208,0x00000204,0x00000200,

	0x000001FC,0x000001F8,0x000001F4,0x000001F0,0x000001ED,0x000001E9,0x000001E5,0x000001E2,
	0x000001DE,0x000001DB,0x000001D7,0x000001D4,0x000001D1,0x000001CE,0x000001CA,0x000001C7,
	0x000001C4,0x000001C1,0x000001BE,0x000001BB,0x000001B8,0x000001B5,0x000001B2,0x000001AF,
	0x000001AC,0x000001AA,0x000001A7,0x000001A4,0x000001A1,0x0000019F,0x0000019C,0x0000019A,
};
