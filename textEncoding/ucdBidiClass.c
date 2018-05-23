#   include <stdio.h>

#   include "ucdIntern.h"
#   include "ucdBidiClass.h"

#   define ET	UCDbidi_ET
#   define S	UCDbidi_S
#   define LRO	UCDbidi_LRO
#   define AL	UCDbidi_AL
#   define RLO	UCDbidi_RLO
#   define BN	UCDbidi_BN
#   define RLE	UCDbidi_RLE
#   define L	UCDbidi_L
#   define LRE	UCDbidi_LRE
#   define NSM	UCDbidi_NSM
#   define ON	UCDbidi_ON
#   define B	UCDbidi_B
#   define PDF	UCDbidi_PDF
#   define CS	UCDbidi_CS
#   define AN	UCDbidi_AN
#   define WS	UCDbidi_WS
#   define R	UCDbidi_R
#   define ES	UCDbidi_ES
#   define EN	UCDbidi_EN

static const unsigned char    UCD_BIDI_0000_000f[16]= {BN,BN,BN,BN,BN,BN,BN,BN,BN,S,B,S,WS,B,BN,BN,};
static const unsigned char    UCD_BIDI_0010_001f[16]= {BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,B,B,B,S,};
static const unsigned char    UCD_BIDI_0020_002f[16]= {WS,ON,ON,ET,ET,ET,ON,ON,ON,ON,ON,ES,CS,ES,CS,CS,};
static const unsigned char    UCD_BIDI_0030_003f[16]= {EN,EN,EN,EN,EN,EN,EN,EN,EN,EN,CS,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0040_004f[16]= {ON,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0050_005f[16]= {L,L,L,L,L,L,L,L,L,L,L,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0070_007f[16]= {L,L,L,L,L,L,L,L,L,L,L,ON,ON,ON,ON,BN,};
static const unsigned char    UCD_BIDI_0080_008f[16]= {BN,BN,BN,BN,BN,B,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,};
static const unsigned char    UCD_BIDI_0090_009f[16]= {BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,BN,};
static const unsigned char    UCD_BIDI_00a0_00af[16]= {CS,ON,ET,ET,ET,ET,ON,ON,ON,ON,L,ON,ON,BN,ON,ON,};
static const unsigned char    UCD_BIDI_00b0_00bf[16]= {ET,ET,EN,EN,ON,L,ON,ON,ON,EN,L,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_00c0_00cf[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_00d0_00df[16]= {L,L,L,L,L,L,L,ON,L,L,L,L,L,L,L,L,};
static const unsigned char * const UCD_BIDI_0000_00ff[16]= {
    UCD_BIDI_0000_000f,
    UCD_BIDI_0010_001f,
    UCD_BIDI_0020_002f,
    UCD_BIDI_0030_003f,
    UCD_BIDI_0040_004f,
    UCD_BIDI_0050_005f,
    UCD_BIDI_0040_004f, /*  UCD_BIDI_0060_006f  */
    UCD_BIDI_0070_007f,
    UCD_BIDI_0080_008f,
    UCD_BIDI_0090_009f,
    UCD_BIDI_00a0_00af,
    UCD_BIDI_00b0_00bf,
    UCD_BIDI_00c0_00cf,
    UCD_BIDI_00d0_00df,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_00e0_00ef  */
    UCD_BIDI_00d0_00df, /*  UCD_BIDI_00f0_00ff  */
};
static const unsigned char * const UCD_BIDI_0100_01ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0100_010f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0110_011f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0120_012f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0130_013f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0140_014f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0150_015f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0160_016f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0170_017f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0180_018f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0190_019f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_01a0_01af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_01b0_01bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_01c0_01cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_01d0_01df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_01e0_01ef  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_01f0_01ff  */
};
static const unsigned char    UCD_BIDI_02b0_02bf[16]= {L,L,L,L,L,L,L,L,L,ON,ON,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_02c0_02cf[16]= {L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_02e0_02ef[16]= {L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,L,ON,};
static const unsigned char    UCD_BIDI_02f0_02ff[16]= {ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_0200_02ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0200_020f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0210_021f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0220_022f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0230_023f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0240_024f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0250_025f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0260_026f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0270_027f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0280_028f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0290_029f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_02a0_02af  */
    UCD_BIDI_02b0_02bf,
    UCD_BIDI_02c0_02cf,
    UCD_BIDI_02c0_02cf, /*  UCD_BIDI_02d0_02df  */
    UCD_BIDI_02e0_02ef,
    UCD_BIDI_02f0_02ff,
};
static const unsigned char    UCD_BIDI_0300_030f[16]= {NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_0370_037f[16]= {L,L,L,L,ON,ON,L,L,ON,ON,L,L,L,L,ON,ON,};
static const unsigned char    UCD_BIDI_0380_038f[16]= {ON,ON,ON,ON,ON,ON,L,ON,L,L,L,ON,L,ON,L,L,};
static const unsigned char    UCD_BIDI_03a0_03af[16]= {L,L,ON,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_03f0_03ff[16]= {L,L,L,L,L,L,ON,L,L,L,L,L,L,L,L,L,};
static const unsigned char * const UCD_BIDI_0300_03ff[16]= {
    UCD_BIDI_0300_030f,
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0310_031f  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0320_032f  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0330_033f  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0340_034f  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0350_035f  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0360_036f  */
    UCD_BIDI_0370_037f,
    UCD_BIDI_0380_038f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0390_039f  */
    UCD_BIDI_03a0_03af,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_03b0_03bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_03c0_03cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_03d0_03df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_03e0_03ef  */
    UCD_BIDI_03f0_03ff,
};
static const unsigned char    UCD_BIDI_0480_048f[16]= {L,L,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,L,L,L,L,L,};
static const unsigned char * const UCD_BIDI_0400_04ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0400_040f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0410_041f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0420_042f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0430_043f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0440_044f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0450_045f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0460_046f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0470_047f  */
    UCD_BIDI_0480_048f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0490_049f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_04a0_04af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_04b0_04bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_04c0_04cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_04d0_04df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_04e0_04ef  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_04f0_04ff  */
};
static const unsigned char    UCD_BIDI_0520_052f[16]= {L,L,L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0550_055f[16]= {L,L,L,L,L,L,L,ON,ON,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0580_058f[16]= {L,L,L,L,L,L,L,L,ON,L,ON,ON,ON,ON,ON,ET,};
static const unsigned char    UCD_BIDI_0590_059f[16]= {ON,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_05b0_05bf[16]= {NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,R,NSM,};
static const unsigned char    UCD_BIDI_05c0_05cf[16]= {R,NSM,NSM,R,NSM,NSM,R,NSM,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_05d0_05df[16]= {R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,};
static const unsigned char    UCD_BIDI_05e0_05ef[16]= {R,R,R,R,R,R,R,R,R,R,R,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_05f0_05ff[16]= {R,R,R,R,R,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_0500_05ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0500_050f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0510_051f  */
    UCD_BIDI_0520_052f,
    UCD_BIDI_0040_004f, /*  UCD_BIDI_0530_053f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0540_054f  */
    UCD_BIDI_0550_055f,
    UCD_BIDI_0040_004f, /*  UCD_BIDI_0560_056f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0570_057f  */
    UCD_BIDI_0580_058f,
    UCD_BIDI_0590_059f,
    UCD_BIDI_0300_030f, /*  UCD_BIDI_05a0_05af  */
    UCD_BIDI_05b0_05bf,
    UCD_BIDI_05c0_05cf,
    UCD_BIDI_05d0_05df,
    UCD_BIDI_05e0_05ef,
    UCD_BIDI_05f0_05ff,
};
static const unsigned char    UCD_BIDI_0600_060f[16]= {AN,AN,AN,AN,AN,ON,ON,ON,AL,ET,ET,AL,CS,AL,ON,ON,};
static const unsigned char    UCD_BIDI_0610_061f[16]= {NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,AL,ON,ON,AL,AL,};
static const unsigned char    UCD_BIDI_0620_062f[16]= {AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,};
static const unsigned char    UCD_BIDI_0640_064f[16]= {AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_0660_066f[16]= {AN,AN,AN,AN,AN,AN,AN,AN,AN,AN,ET,AN,AN,AL,AL,AL,};
static const unsigned char    UCD_BIDI_0670_067f[16]= {NSM,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,};
static const unsigned char    UCD_BIDI_06d0_06df[16]= {AL,AL,AL,AL,AL,AL,NSM,NSM,NSM,NSM,NSM,NSM,NSM,AN,ON,NSM,};
static const unsigned char    UCD_BIDI_06e0_06ef[16]= {NSM,NSM,NSM,NSM,NSM,AL,AL,NSM,NSM,ON,NSM,NSM,NSM,NSM,AL,AL,};
static const unsigned char    UCD_BIDI_06f0_06ff[16]= {EN,EN,EN,EN,EN,EN,EN,EN,EN,EN,AL,AL,AL,AL,AL,AL,};
static const unsigned char * const UCD_BIDI_0600_06ff[16]= {
    UCD_BIDI_0600_060f,
    UCD_BIDI_0610_061f,
    UCD_BIDI_0620_062f,
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0630_063f  */
    UCD_BIDI_0640_064f,
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0650_065f  */
    UCD_BIDI_0660_066f,
    UCD_BIDI_0670_067f,
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0680_068f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0690_069f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_06a0_06af  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_06b0_06bf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_06c0_06cf  */
    UCD_BIDI_06d0_06df,
    UCD_BIDI_06e0_06ef,
    UCD_BIDI_06f0_06ff,
};
static const unsigned char    UCD_BIDI_0700_070f[16]= {AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,ON,AL,};
static const unsigned char    UCD_BIDI_0710_071f[16]= {AL,NSM,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,};
static const unsigned char    UCD_BIDI_0740_074f[16]= {NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,ON,AL,AL,AL,};
static const unsigned char    UCD_BIDI_07a0_07af[16]= {AL,AL,AL,AL,AL,AL,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_07b0_07bf[16]= {NSM,AL,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_07e0_07ef[16]= {R,R,R,R,R,R,R,R,R,R,R,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_07f0_07ff[16]= {NSM,NSM,NSM,NSM,R,R,ON,ON,ON,ON,R,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_0700_07ff[16]= {
    UCD_BIDI_0700_070f,
    UCD_BIDI_0710_071f,
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0720_072f  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0730_073f  */
    UCD_BIDI_0740_074f,
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0750_075f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0760_076f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0770_077f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0780_078f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_0790_079f  */
    UCD_BIDI_07a0_07af,
    UCD_BIDI_07b0_07bf,
    UCD_BIDI_05d0_05df, /*  UCD_BIDI_07c0_07cf  */
    UCD_BIDI_05d0_05df, /*  UCD_BIDI_07d0_07df  */
    UCD_BIDI_07e0_07ef,
    UCD_BIDI_07f0_07ff,
};
static const unsigned char    UCD_BIDI_0810_081f[16]= {R,R,R,R,R,R,NSM,NSM,NSM,NSM,R,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_0820_082f[16]= {NSM,NSM,NSM,NSM,R,NSM,NSM,NSM,R,NSM,NSM,NSM,NSM,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_0830_083f[16]= {R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,ON,};
static const unsigned char    UCD_BIDI_0850_085f[16]= {R,R,R,R,R,R,R,R,R,NSM,NSM,NSM,ON,ON,R,ON,};
static const unsigned char    UCD_BIDI_08a0_08af[16]= {AL,ON,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,ON,ON,ON,};
static const unsigned char    UCD_BIDI_08e0_08ef[16]= {ON,ON,ON,ON,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_08f0_08ff[16]= {NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,};
static const unsigned char * const UCD_BIDI_0800_08ff[16]= {
    UCD_BIDI_05d0_05df, /*  UCD_BIDI_0800_080f  */
    UCD_BIDI_0810_081f,
    UCD_BIDI_0820_082f,
    UCD_BIDI_0830_083f,
    UCD_BIDI_05d0_05df, /*  UCD_BIDI_0840_084f  */
    UCD_BIDI_0850_085f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0860_086f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0870_087f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0880_088f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0890_089f  */
    UCD_BIDI_08a0_08af,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_08b0_08bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_08c0_08cf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_08d0_08df  */
    UCD_BIDI_08e0_08ef,
    UCD_BIDI_08f0_08ff,
};
static const unsigned char    UCD_BIDI_0900_090f[16]= {NSM,NSM,NSM,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0930_093f[16]= {L,L,L,L,L,L,L,L,L,L,NSM,L,NSM,L,L,L,};
static const unsigned char    UCD_BIDI_0940_094f[16]= {L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,L,L,L,NSM,L,L,};
static const unsigned char    UCD_BIDI_0950_095f[16]= {L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0960_096f[16]= {L,L,NSM,NSM,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0970_097f[16]= {L,L,L,L,L,L,L,L,ON,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0980_098f[16]= {ON,NSM,L,L,ON,L,L,L,L,L,L,L,L,ON,ON,L,};
static const unsigned char    UCD_BIDI_0990_099f[16]= {L,ON,ON,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_09a0_09af[16]= {L,L,L,L,L,L,L,L,L,ON,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_09b0_09bf[16]= {L,ON,L,ON,ON,ON,L,L,L,L,ON,ON,NSM,L,L,L,};
static const unsigned char    UCD_BIDI_09c0_09cf[16]= {L,NSM,NSM,NSM,NSM,ON,ON,L,L,ON,ON,L,L,NSM,L,ON,};
static const unsigned char    UCD_BIDI_09d0_09df[16]= {ON,ON,ON,ON,ON,ON,ON,L,ON,ON,ON,ON,L,L,ON,L,};
static const unsigned char    UCD_BIDI_09e0_09ef[16]= {L,L,NSM,NSM,ON,ON,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_09f0_09ff[16]= {L,L,ET,ET,L,L,L,L,L,L,L,ET,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_0900_09ff[16]= {
    UCD_BIDI_0900_090f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0910_091f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0920_092f  */
    UCD_BIDI_0930_093f,
    UCD_BIDI_0940_094f,
    UCD_BIDI_0950_095f,
    UCD_BIDI_0960_096f,
    UCD_BIDI_0970_097f,
    UCD_BIDI_0980_098f,
    UCD_BIDI_0990_099f,
    UCD_BIDI_09a0_09af,
    UCD_BIDI_09b0_09bf,
    UCD_BIDI_09c0_09cf,
    UCD_BIDI_09d0_09df,
    UCD_BIDI_09e0_09ef,
    UCD_BIDI_09f0_09ff,
};
static const unsigned char    UCD_BIDI_0a00_0a0f[16]= {ON,NSM,NSM,L,ON,L,L,L,L,L,L,ON,ON,ON,ON,L,};
static const unsigned char    UCD_BIDI_0a30_0a3f[16]= {L,ON,L,L,ON,L,L,ON,L,L,ON,ON,NSM,ON,L,L,};
static const unsigned char    UCD_BIDI_0a40_0a4f[16]= {L,NSM,NSM,ON,ON,ON,ON,NSM,NSM,ON,ON,NSM,NSM,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_0a50_0a5f[16]= {ON,NSM,ON,ON,ON,ON,ON,ON,ON,L,L,L,L,ON,L,ON,};
static const unsigned char    UCD_BIDI_0a60_0a6f[16]= {ON,ON,ON,ON,ON,ON,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0a70_0a7f[16]= {NSM,NSM,L,L,L,NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0a80_0a8f[16]= {ON,NSM,NSM,L,ON,L,L,L,L,L,L,L,L,L,ON,L,};
static const unsigned char    UCD_BIDI_0ab0_0abf[16]= {L,ON,L,L,ON,L,L,L,L,L,ON,ON,NSM,L,L,L,};
static const unsigned char    UCD_BIDI_0ac0_0acf[16]= {L,NSM,NSM,NSM,NSM,NSM,ON,NSM,NSM,L,ON,L,L,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_0ad0_0adf[16]= {L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0af0_0aff[16]= {L,ET,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_0a00_0aff[16]= {
    UCD_BIDI_0a00_0a0f,
    UCD_BIDI_0990_099f, /*  UCD_BIDI_0a10_0a1f  */
    UCD_BIDI_09a0_09af, /*  UCD_BIDI_0a20_0a2f  */
    UCD_BIDI_0a30_0a3f,
    UCD_BIDI_0a40_0a4f,
    UCD_BIDI_0a50_0a5f,
    UCD_BIDI_0a60_0a6f,
    UCD_BIDI_0a70_0a7f,
    UCD_BIDI_0a80_0a8f,
    UCD_BIDI_03a0_03af, /*  UCD_BIDI_0a90_0a9f  */
    UCD_BIDI_09a0_09af, /*  UCD_BIDI_0aa0_0aaf  */
    UCD_BIDI_0ab0_0abf,
    UCD_BIDI_0ac0_0acf,
    UCD_BIDI_0ad0_0adf,
    UCD_BIDI_09e0_09ef, /*  UCD_BIDI_0ae0_0aef  */
    UCD_BIDI_0af0_0aff,
};
static const unsigned char    UCD_BIDI_0b30_0b3f[16]= {L,ON,L,L,ON,L,L,L,L,L,ON,ON,NSM,L,L,NSM,};
static const unsigned char    UCD_BIDI_0b40_0b4f[16]= {L,NSM,NSM,NSM,NSM,ON,ON,L,L,ON,ON,L,L,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_0b50_0b5f[16]= {ON,ON,ON,ON,ON,ON,NSM,L,ON,ON,ON,ON,L,L,ON,L,};
static const unsigned char    UCD_BIDI_0b80_0b8f[16]= {ON,ON,NSM,L,ON,L,L,L,L,L,L,ON,ON,ON,L,L,};
static const unsigned char    UCD_BIDI_0b90_0b9f[16]= {L,ON,L,L,L,L,ON,ON,ON,L,L,ON,L,ON,L,L,};
static const unsigned char    UCD_BIDI_0ba0_0baf[16]= {ON,ON,ON,L,L,ON,ON,ON,L,L,L,ON,ON,ON,L,L,};
static const unsigned char    UCD_BIDI_0bb0_0bbf[16]= {L,L,L,L,L,L,L,L,L,L,ON,ON,ON,ON,L,L,};
static const unsigned char    UCD_BIDI_0bc0_0bcf[16]= {NSM,L,L,ON,ON,ON,L,L,L,ON,L,L,L,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_0bd0_0bdf[16]= {L,ON,ON,ON,ON,ON,ON,L,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0bf0_0bff[16]= {L,L,L,ON,ON,ON,ON,ON,ON,ET,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_0b00_0bff[16]= {
    UCD_BIDI_0980_098f, /*  UCD_BIDI_0b00_0b0f  */
    UCD_BIDI_0990_099f, /*  UCD_BIDI_0b10_0b1f  */
    UCD_BIDI_09a0_09af, /*  UCD_BIDI_0b20_0b2f  */
    UCD_BIDI_0b30_0b3f,
    UCD_BIDI_0b40_0b4f,
    UCD_BIDI_0b50_0b5f,
    UCD_BIDI_09e0_09ef, /*  UCD_BIDI_0b60_0b6f  */
    UCD_BIDI_0520_052f, /*  UCD_BIDI_0b70_0b7f  */
    UCD_BIDI_0b80_0b8f,
    UCD_BIDI_0b90_0b9f,
    UCD_BIDI_0ba0_0baf,
    UCD_BIDI_0bb0_0bbf,
    UCD_BIDI_0bc0_0bcf,
    UCD_BIDI_0bd0_0bdf,
    UCD_BIDI_0a60_0a6f, /*  UCD_BIDI_0be0_0bef  */
    UCD_BIDI_0bf0_0bff,
};
static const unsigned char    UCD_BIDI_0c00_0c0f[16]= {ON,L,L,L,ON,L,L,L,L,L,L,L,L,ON,L,L,};
static const unsigned char    UCD_BIDI_0c10_0c1f[16]= {L,ON,L,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0c30_0c3f[16]= {L,L,L,L,ON,L,L,L,L,L,ON,ON,ON,L,NSM,NSM,};
static const unsigned char    UCD_BIDI_0c40_0c4f[16]= {NSM,L,L,L,L,ON,NSM,NSM,NSM,ON,NSM,NSM,NSM,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_0c50_0c5f[16]= {ON,ON,ON,ON,ON,NSM,NSM,ON,L,L,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0c70_0c7f[16]= {ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,L,};
static const unsigned char    UCD_BIDI_0c80_0c8f[16]= {ON,ON,L,L,ON,L,L,L,L,L,L,L,L,ON,L,L,};
static const unsigned char    UCD_BIDI_0cb0_0cbf[16]= {L,L,L,L,ON,L,L,L,L,L,ON,ON,NSM,L,L,L,};
static const unsigned char    UCD_BIDI_0cc0_0ccf[16]= {L,L,L,L,L,ON,L,L,L,ON,L,L,NSM,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_0cd0_0cdf[16]= {ON,ON,ON,ON,ON,L,L,ON,ON,ON,ON,ON,ON,ON,L,ON,};
static const unsigned char    UCD_BIDI_0cf0_0cff[16]= {ON,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_0c00_0cff[16]= {
    UCD_BIDI_0c00_0c0f,
    UCD_BIDI_0c10_0c1f,
    UCD_BIDI_09a0_09af, /*  UCD_BIDI_0c20_0c2f  */
    UCD_BIDI_0c30_0c3f,
    UCD_BIDI_0c40_0c4f,
    UCD_BIDI_0c50_0c5f,
    UCD_BIDI_09e0_09ef, /*  UCD_BIDI_0c60_0c6f  */
    UCD_BIDI_0c70_0c7f,
    UCD_BIDI_0c80_0c8f,
    UCD_BIDI_0c10_0c1f, /*  UCD_BIDI_0c90_0c9f  */
    UCD_BIDI_09a0_09af, /*  UCD_BIDI_0ca0_0caf  */
    UCD_BIDI_0cb0_0cbf,
    UCD_BIDI_0cc0_0ccf,
    UCD_BIDI_0cd0_0cdf,
    UCD_BIDI_09e0_09ef, /*  UCD_BIDI_0ce0_0cef  */
    UCD_BIDI_0cf0_0cff,
};
static const unsigned char    UCD_BIDI_0d30_0d3f[16]= {L,L,L,L,L,L,L,L,L,L,L,ON,ON,L,L,L,};
static const unsigned char    UCD_BIDI_0d40_0d4f[16]= {L,NSM,NSM,NSM,NSM,ON,L,L,L,ON,L,L,L,NSM,L,ON,};
static const unsigned char    UCD_BIDI_0d50_0d5f[16]= {ON,ON,ON,ON,ON,ON,ON,L,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0d70_0d7f[16]= {L,L,L,L,L,L,ON,ON,ON,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0d80_0d8f[16]= {ON,ON,L,L,ON,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0d90_0d9f[16]= {L,L,L,L,L,L,L,ON,ON,ON,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0db0_0dbf[16]= {L,L,ON,L,L,L,L,L,L,L,L,L,ON,L,ON,ON,};
static const unsigned char    UCD_BIDI_0dc0_0dcf[16]= {L,L,L,L,L,L,L,ON,ON,ON,NSM,ON,ON,ON,ON,L,};
static const unsigned char    UCD_BIDI_0dd0_0ddf[16]= {L,L,NSM,NSM,NSM,ON,NSM,ON,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0df0_0dff[16]= {ON,ON,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_0d00_0dff[16]= {
    UCD_BIDI_0c80_0c8f, /*  UCD_BIDI_0d00_0d0f  */
    UCD_BIDI_0c10_0c1f, /*  UCD_BIDI_0d10_0d1f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0d20_0d2f  */
    UCD_BIDI_0d30_0d3f,
    UCD_BIDI_0d40_0d4f,
    UCD_BIDI_0d50_0d5f,
    UCD_BIDI_09e0_09ef, /*  UCD_BIDI_0d60_0d6f  */
    UCD_BIDI_0d70_0d7f,
    UCD_BIDI_0d80_0d8f,
    UCD_BIDI_0d90_0d9f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0da0_0daf  */
    UCD_BIDI_0db0_0dbf,
    UCD_BIDI_0dc0_0dcf,
    UCD_BIDI_0dd0_0ddf,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0de0_0def  */
    UCD_BIDI_0df0_0dff,
};
static const unsigned char    UCD_BIDI_0e30_0e3f[16]= {L,NSM,L,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,ON,ON,ON,ET,};
static const unsigned char    UCD_BIDI_0e40_0e4f[16]= {L,L,L,L,L,L,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,};
static const unsigned char    UCD_BIDI_0e50_0e5f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0e80_0e8f[16]= {ON,L,L,ON,L,ON,ON,L,L,ON,L,ON,ON,L,ON,ON,};
static const unsigned char    UCD_BIDI_0e90_0e9f[16]= {ON,ON,ON,ON,L,L,L,L,ON,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0ea0_0eaf[16]= {ON,L,L,L,ON,L,ON,L,ON,ON,L,L,ON,L,L,L,};
static const unsigned char    UCD_BIDI_0eb0_0ebf[16]= {L,NSM,L,L,NSM,NSM,NSM,NSM,NSM,NSM,ON,NSM,NSM,L,ON,ON,};
static const unsigned char    UCD_BIDI_0ec0_0ecf[16]= {L,L,L,L,L,ON,L,ON,NSM,NSM,NSM,NSM,NSM,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_0ed0_0edf[16]= {L,L,L,L,L,L,L,L,L,L,ON,ON,L,L,L,L,};
static const unsigned char * const UCD_BIDI_0e00_0eff[16]= {
    UCD_BIDI_0040_004f, /*  UCD_BIDI_0e00_0e0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0e10_0e1f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0e20_0e2f  */
    UCD_BIDI_0e30_0e3f,
    UCD_BIDI_0e40_0e4f,
    UCD_BIDI_0e50_0e5f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0e60_0e6f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0e70_0e7f  */
    UCD_BIDI_0e80_0e8f,
    UCD_BIDI_0e90_0e9f,
    UCD_BIDI_0ea0_0eaf,
    UCD_BIDI_0eb0_0ebf,
    UCD_BIDI_0ec0_0ecf,
    UCD_BIDI_0ed0_0edf,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0ee0_0eef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0ef0_0eff  */
};
static const unsigned char    UCD_BIDI_0f10_0f1f[16]= {L,L,L,L,L,L,L,L,NSM,NSM,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_0f30_0f3f[16]= {L,L,L,L,L,NSM,L,NSM,L,NSM,ON,ON,ON,ON,L,L,};
static const unsigned char    UCD_BIDI_0f60_0f6f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,ON,ON,ON,};
static const unsigned char    UCD_BIDI_0f70_0f7f[16]= {ON,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,};
static const unsigned char    UCD_BIDI_0f80_0f8f[16]= {NSM,NSM,NSM,NSM,NSM,L,NSM,NSM,L,L,L,L,L,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_0f90_0f9f[16]= {NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,NSM,NSM,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_0fb0_0fbf[16]= {NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,L,L,};
static const unsigned char    UCD_BIDI_0fc0_0fcf[16]= {L,L,L,L,L,L,NSM,L,L,L,L,L,L,ON,L,L,};
static const unsigned char * const UCD_BIDI_0f00_0fff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0f00_0f0f  */
    UCD_BIDI_0f10_0f1f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0f20_0f2f  */
    UCD_BIDI_0f30_0f3f,
    UCD_BIDI_0970_097f, /*  UCD_BIDI_0f40_0f4f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_0f50_0f5f  */
    UCD_BIDI_0f60_0f6f,
    UCD_BIDI_0f70_0f7f,
    UCD_BIDI_0f80_0f8f,
    UCD_BIDI_0f90_0f9f,
    UCD_BIDI_0300_030f, /*  UCD_BIDI_0fa0_0faf  */
    UCD_BIDI_0fb0_0fbf,
    UCD_BIDI_0fc0_0fcf,
    UCD_BIDI_0050_005f, /*  UCD_BIDI_0fd0_0fdf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0fe0_0fef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_0ff0_0fff  */
};
static const unsigned char * const * const UCD_BIDI_0000_0fff[16]= {
    UCD_BIDI_0000_00ff,
    UCD_BIDI_0100_01ff,
    UCD_BIDI_0200_02ff,
    UCD_BIDI_0300_03ff,
    UCD_BIDI_0400_04ff,
    UCD_BIDI_0500_05ff,
    UCD_BIDI_0600_06ff,
    UCD_BIDI_0700_07ff,
    UCD_BIDI_0800_08ff,
    UCD_BIDI_0900_09ff,
    UCD_BIDI_0a00_0aff,
    UCD_BIDI_0b00_0bff,
    UCD_BIDI_0c00_0cff,
    UCD_BIDI_0d00_0dff,
    UCD_BIDI_0e00_0eff,
    UCD_BIDI_0f00_0fff,
};
static const unsigned char    UCD_BIDI_1020_102f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_1030_103f[16]= {NSM,L,NSM,NSM,NSM,NSM,NSM,NSM,L,NSM,NSM,L,L,NSM,NSM,L,};
static const unsigned char    UCD_BIDI_1050_105f[16]= {L,L,L,L,L,L,L,L,NSM,NSM,L,L,L,L,NSM,NSM,};
static const unsigned char    UCD_BIDI_1060_106f[16]= {NSM,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_1070_107f[16]= {L,NSM,NSM,NSM,NSM,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_1080_108f[16]= {L,L,NSM,L,L,NSM,NSM,L,L,L,L,L,L,NSM,L,L,};
static const unsigned char    UCD_BIDI_1090_109f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,NSM,L,L,};
static const unsigned char    UCD_BIDI_10c0_10cf[16]= {L,L,L,L,L,L,ON,L,ON,ON,ON,ON,ON,L,ON,ON,};
static const unsigned char * const UCD_BIDI_1000_10ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1000_100f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1010_101f  */
    UCD_BIDI_1020_102f,
    UCD_BIDI_1030_103f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1040_104f  */
    UCD_BIDI_1050_105f,
    UCD_BIDI_1060_106f,
    UCD_BIDI_1070_107f,
    UCD_BIDI_1080_108f,
    UCD_BIDI_1090_109f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_10a0_10af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_10b0_10bf  */
    UCD_BIDI_10c0_10cf,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_10d0_10df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_10e0_10ef  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_10f0_10ff  */
};
static const unsigned char    UCD_BIDI_1240_124f[16]= {L,L,L,L,L,L,L,L,L,ON,L,L,L,L,ON,ON,};
static const unsigned char    UCD_BIDI_1250_125f[16]= {L,L,L,L,L,L,L,ON,L,ON,L,L,L,L,ON,ON,};
static const unsigned char    UCD_BIDI_12b0_12bf[16]= {L,ON,L,L,L,L,ON,ON,L,L,L,L,L,L,L,ON,};
static const unsigned char    UCD_BIDI_12c0_12cf[16]= {L,ON,L,L,L,L,ON,ON,L,L,L,L,L,L,L,L,};
static const unsigned char * const UCD_BIDI_1200_12ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1200_120f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1210_121f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1220_122f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1230_123f  */
    UCD_BIDI_1240_124f,
    UCD_BIDI_1250_125f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1260_126f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1270_127f  */
    UCD_BIDI_1240_124f, /*  UCD_BIDI_1280_128f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1290_129f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_12a0_12af  */
    UCD_BIDI_12b0_12bf,
    UCD_BIDI_12c0_12cf,
    UCD_BIDI_00d0_00df, /*  UCD_BIDI_12d0_12df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_12e0_12ef  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_12f0_12ff  */
};
static const unsigned char    UCD_BIDI_1350_135f[16]= {L,L,L,L,L,L,L,L,L,L,L,ON,ON,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_13f0_13ff[16]= {L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_1300_13ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1300_130f  */
    UCD_BIDI_12c0_12cf, /*  UCD_BIDI_1310_131f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1320_132f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1330_133f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1340_134f  */
    UCD_BIDI_1350_135f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1360_136f  */
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_1370_137f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1380_138f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1390_139f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_13a0_13af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_13b0_13bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_13c0_13cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_13d0_13df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_13e0_13ef  */
    UCD_BIDI_13f0_13ff,
};
static const unsigned char * const UCD_BIDI_1400_14ff[16]= {
    UCD_BIDI_0040_004f, /*  UCD_BIDI_1400_140f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1410_141f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1420_142f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1430_143f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1440_144f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1450_145f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1460_146f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1470_147f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1480_148f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1490_149f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_14a0_14af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_14b0_14bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_14c0_14cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_14d0_14df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_14e0_14ef  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_14f0_14ff  */
};
static const unsigned char    UCD_BIDI_1680_168f[16]= {WS,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char * const UCD_BIDI_1600_16ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1600_160f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1610_161f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1620_162f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1630_163f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1640_164f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1650_165f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1660_166f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1670_167f  */
    UCD_BIDI_1680_168f,
    UCD_BIDI_0050_005f, /*  UCD_BIDI_1690_169f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_16a0_16af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_16b0_16bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_16c0_16cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_16d0_16df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_16e0_16ef  */
    UCD_BIDI_0ad0_0adf, /*  UCD_BIDI_16f0_16ff  */
};
static const unsigned char    UCD_BIDI_1700_170f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,ON,L,L,};
static const unsigned char    UCD_BIDI_1710_171f[16]= {L,L,NSM,NSM,NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1730_173f[16]= {L,L,NSM,NSM,NSM,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1750_175f[16]= {L,L,NSM,NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1770_177f[16]= {L,ON,NSM,NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_17b0_17bf[16]= {L,L,L,L,NSM,NSM,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,L,};
static const unsigned char    UCD_BIDI_17c0_17cf[16]= {L,L,L,L,L,L,NSM,L,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_17d0_17df[16]= {NSM,NSM,NSM,NSM,L,L,L,L,L,L,L,ET,L,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_17e0_17ef[16]= {L,L,L,L,L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_1700_17ff[16]= {
    UCD_BIDI_1700_170f,
    UCD_BIDI_1710_171f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1720_172f  */
    UCD_BIDI_1730_173f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1740_174f  */
    UCD_BIDI_1750_175f,
    UCD_BIDI_1700_170f, /*  UCD_BIDI_1760_176f  */
    UCD_BIDI_1770_177f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1780_178f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1790_179f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_17a0_17af  */
    UCD_BIDI_17b0_17bf,
    UCD_BIDI_17c0_17cf,
    UCD_BIDI_17d0_17df,
    UCD_BIDI_17e0_17ef,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_17f0_17ff  */
};
static const unsigned char    UCD_BIDI_1800_180f[16]= {ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,NSM,NSM,NSM,WS,ON,};
static const unsigned char    UCD_BIDI_18a0_18af[16]= {L,L,L,L,L,L,L,L,L,NSM,L,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_18f0_18ff[16]= {L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_1800_18ff[16]= {
    UCD_BIDI_1800_180f,
    UCD_BIDI_17e0_17ef, /*  UCD_BIDI_1810_181f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1820_182f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1830_183f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1840_184f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1850_185f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1860_186f  */
    UCD_BIDI_0520_052f, /*  UCD_BIDI_1870_187f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1880_188f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1890_189f  */
    UCD_BIDI_18a0_18af,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_18b0_18bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_18c0_18cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_18d0_18df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_18e0_18ef  */
    UCD_BIDI_18f0_18ff,
};
static const unsigned char    UCD_BIDI_1920_192f[16]= {NSM,NSM,NSM,L,L,L,L,NSM,NSM,L,L,L,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1930_193f[16]= {L,L,NSM,L,L,L,L,L,L,NSM,NSM,NSM,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1960_196f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,L,ON,ON,};
static const unsigned char * const UCD_BIDI_1900_19ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1900_190f  */
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_1910_191f  */
    UCD_BIDI_1920_192f,
    UCD_BIDI_1930_193f,
    UCD_BIDI_0a60_0a6f, /*  UCD_BIDI_1940_194f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1950_195f  */
    UCD_BIDI_1960_196f,
    UCD_BIDI_13f0_13ff, /*  UCD_BIDI_1970_197f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1980_198f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1990_199f  */
    UCD_BIDI_0e50_0e5f, /*  UCD_BIDI_19a0_19af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_19b0_19bf  */
    UCD_BIDI_17e0_17ef, /*  UCD_BIDI_19c0_19cf  */
    UCD_BIDI_0050_005f, /*  UCD_BIDI_19d0_19df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_19e0_19ef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_19f0_19ff  */
};
static const unsigned char    UCD_BIDI_1a10_1a1f[16]= {L,L,L,L,L,L,L,NSM,NSM,L,L,L,ON,ON,L,L,};
static const unsigned char    UCD_BIDI_1a50_1a5f[16]= {L,L,L,L,L,L,NSM,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,};
static const unsigned char    UCD_BIDI_1a60_1a6f[16]= {NSM,L,NSM,L,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,L,L,};
static const unsigned char    UCD_BIDI_1a70_1a7f[16]= {L,L,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,ON,NSM,};
static const unsigned char * const UCD_BIDI_1a00_1aff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1a00_1a0f  */
    UCD_BIDI_1a10_1a1f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1a20_1a2f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1a30_1a3f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1a40_1a4f  */
    UCD_BIDI_1a50_1a5f,
    UCD_BIDI_1a60_1a6f,
    UCD_BIDI_1a70_1a7f,
    UCD_BIDI_17e0_17ef, /*  UCD_BIDI_1a80_1a8f  */
    UCD_BIDI_17e0_17ef, /*  UCD_BIDI_1a90_1a9f  */
    UCD_BIDI_1960_196f, /*  UCD_BIDI_1aa0_1aaf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1ab0_1abf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1ac0_1acf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1ad0_1adf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1ae0_1aef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1af0_1aff  */
};
static const unsigned char    UCD_BIDI_1b00_1b0f[16]= {NSM,NSM,NSM,NSM,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_1b30_1b3f[16]= {L,L,L,L,NSM,L,NSM,NSM,NSM,NSM,NSM,L,NSM,L,L,L,};
static const unsigned char    UCD_BIDI_1b40_1b4f[16]= {L,L,NSM,L,L,L,L,L,L,L,L,L,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1b60_1b6f[16]= {L,L,L,L,L,L,L,L,L,L,L,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_1b70_1b7f[16]= {NSM,NSM,NSM,NSM,L,L,L,L,L,L,L,L,L,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1b80_1b8f[16]= {NSM,NSM,L,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_1ba0_1baf[16]= {L,L,NSM,NSM,NSM,NSM,L,L,NSM,NSM,L,NSM,L,L,L,L,};
static const unsigned char    UCD_BIDI_1be0_1bef[16]= {L,L,L,L,L,L,NSM,L,NSM,NSM,L,L,L,NSM,L,NSM,};
static const unsigned char    UCD_BIDI_1bf0_1bff[16]= {NSM,NSM,L,L,ON,ON,ON,ON,ON,ON,ON,ON,L,L,L,L,};
static const unsigned char * const UCD_BIDI_1b00_1bff[16]= {
    UCD_BIDI_1b00_1b0f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1b10_1b1f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1b20_1b2f  */
    UCD_BIDI_1b30_1b3f,
    UCD_BIDI_1b40_1b4f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1b50_1b5f  */
    UCD_BIDI_1b60_1b6f,
    UCD_BIDI_1b70_1b7f,
    UCD_BIDI_1b80_1b8f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1b90_1b9f  */
    UCD_BIDI_1ba0_1baf,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1bb0_1bbf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1bc0_1bcf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1bd0_1bdf  */
    UCD_BIDI_1be0_1bef,
    UCD_BIDI_1bf0_1bff,
};
static const unsigned char    UCD_BIDI_1c20_1c2f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_1c30_1c3f[16]= {NSM,NSM,NSM,NSM,L,L,NSM,NSM,ON,ON,ON,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_1c40_1c4f[16]= {L,L,L,L,L,L,L,L,L,L,ON,ON,ON,L,L,L,};
static const unsigned char    UCD_BIDI_1cd0_1cdf[16]= {NSM,NSM,NSM,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_1ce0_1cef[16]= {NSM,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,L,L,L,NSM,L,L,};
static const unsigned char    UCD_BIDI_1cf0_1cff[16]= {L,L,L,L,NSM,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_1c00_1cff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1c00_1c0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1c10_1c1f  */
    UCD_BIDI_1c20_1c2f,
    UCD_BIDI_1c30_1c3f,
    UCD_BIDI_1c40_1c4f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1c50_1c5f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1c60_1c6f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1c70_1c7f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1c80_1c8f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1c90_1c9f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1ca0_1caf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_1cb0_1cbf  */
    UCD_BIDI_0520_052f, /*  UCD_BIDI_1cc0_1ccf  */
    UCD_BIDI_1cd0_1cdf,
    UCD_BIDI_1ce0_1cef,
    UCD_BIDI_1cf0_1cff,
};
static const unsigned char    UCD_BIDI_1de0_1def[16]= {NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1df0_1dff[16]= {ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,NSM,NSM,NSM,NSM,};
static const unsigned char * const UCD_BIDI_1d00_1dff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d00_1d0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d10_1d1f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d20_1d2f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d30_1d3f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d40_1d4f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d50_1d5f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d60_1d6f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d70_1d7f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d80_1d8f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1d90_1d9f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1da0_1daf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1db0_1dbf  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_1dc0_1dcf  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_1dd0_1ddf  */
    UCD_BIDI_1de0_1def,
    UCD_BIDI_1df0_1dff,
};
static const unsigned char    UCD_BIDI_1f10_1f1f[16]= {L,L,L,L,L,L,ON,ON,L,L,L,L,L,L,ON,ON,};
static const unsigned char    UCD_BIDI_1f50_1f5f[16]= {L,L,L,L,L,L,L,L,ON,L,ON,L,ON,L,ON,L,};
static const unsigned char    UCD_BIDI_1fb0_1fbf[16]= {L,L,L,L,L,ON,L,L,L,L,L,L,L,ON,L,ON,};
static const unsigned char    UCD_BIDI_1fc0_1fcf[16]= {ON,ON,L,L,L,ON,L,L,L,L,L,L,L,ON,ON,ON,};
static const unsigned char    UCD_BIDI_1fd0_1fdf[16]= {L,L,L,L,ON,ON,L,L,L,L,L,L,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_1f00_1fff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1f00_1f0f  */
    UCD_BIDI_1f10_1f1f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1f20_1f2f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1f30_1f3f  */
    UCD_BIDI_1f10_1f1f, /*  UCD_BIDI_1f40_1f4f  */
    UCD_BIDI_1f50_1f5f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1f60_1f6f  */
    UCD_BIDI_1960_196f, /*  UCD_BIDI_1f70_1f7f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1f80_1f8f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1f90_1f9f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_1fa0_1faf  */
    UCD_BIDI_1fb0_1fbf,
    UCD_BIDI_1fc0_1fcf,
    UCD_BIDI_1fd0_1fdf,
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_1fe0_1fef  */
    UCD_BIDI_1fc0_1fcf, /*  UCD_BIDI_1ff0_1fff  */
};
static const unsigned char * const * const UCD_BIDI_1000_1fff[16]= {
    UCD_BIDI_1000_10ff,
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_1100_11ff  */
    UCD_BIDI_1200_12ff,
    UCD_BIDI_1300_13ff,
    UCD_BIDI_1400_14ff,
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_1500_15ff  */
    UCD_BIDI_1600_16ff,
    UCD_BIDI_1700_17ff,
    UCD_BIDI_1800_18ff,
    UCD_BIDI_1900_19ff,
    UCD_BIDI_1a00_1aff,
    UCD_BIDI_1b00_1bff,
    UCD_BIDI_1c00_1cff,
    UCD_BIDI_1d00_1dff,
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_1e00_1eff  */
    UCD_BIDI_1f00_1fff,
};
static const unsigned char    UCD_BIDI_2000_200f[16]= {WS,WS,WS,WS,WS,WS,WS,WS,WS,WS,WS,BN,BN,BN,L,R,};
static const unsigned char    UCD_BIDI_2020_202f[16]= {ON,ON,ON,ON,ON,ON,ON,ON,WS,B,LRE,RLE,PDF,LRO,RLO,CS,};
static const unsigned char    UCD_BIDI_2030_203f[16]= {ET,ET,ET,ET,ET,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_2040_204f[16]= {ON,ON,ON,ON,CS,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_2050_205f[16]= {ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,WS,};
static const unsigned char    UCD_BIDI_2060_206f[16]= {BN,BN,BN,BN,BN,ON,ON,ON,ON,ON,BN,BN,BN,BN,BN,BN,};
static const unsigned char    UCD_BIDI_2070_207f[16]= {EN,L,ON,ON,EN,EN,EN,EN,EN,EN,ES,ES,ON,ON,ON,L,};
static const unsigned char    UCD_BIDI_2080_208f[16]= {EN,EN,EN,EN,EN,EN,EN,EN,EN,EN,ES,ES,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_20a0_20af[16]= {ET,ET,ET,ET,ET,ET,ET,ET,ET,ET,ET,ET,ET,ET,ET,ET,};
static const unsigned char    UCD_BIDI_20b0_20bf[16]= {ET,ET,ET,ET,ET,ET,ET,ET,ET,ET,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_20f0_20ff[16]= {NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_2000_20ff[16]= {
    UCD_BIDI_2000_200f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2010_201f  */
    UCD_BIDI_2020_202f,
    UCD_BIDI_2030_203f,
    UCD_BIDI_2040_204f,
    UCD_BIDI_2050_205f,
    UCD_BIDI_2060_206f,
    UCD_BIDI_2070_207f,
    UCD_BIDI_2080_208f,
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_2090_209f  */
    UCD_BIDI_20a0_20af,
    UCD_BIDI_20b0_20bf,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_20c0_20cf  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_20d0_20df  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_20e0_20ef  */
    UCD_BIDI_20f0_20ff,
};
static const unsigned char    UCD_BIDI_2100_210f[16]= {ON,ON,L,ON,ON,ON,ON,L,ON,ON,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_2110_211f[16]= {L,L,L,L,ON,L,ON,ON,ON,L,L,L,L,L,ON,ON,};
static const unsigned char    UCD_BIDI_2120_212f[16]= {ON,ON,ON,ON,L,ON,L,ON,L,ON,L,L,L,L,ET,L,};
static const unsigned char    UCD_BIDI_2140_214f[16]= {ON,ON,ON,ON,ON,L,L,L,L,L,ON,ON,ON,ON,L,L,};
static const unsigned char    UCD_BIDI_2180_218f[16]= {L,L,L,L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_2100_21ff[16]= {
    UCD_BIDI_2100_210f,
    UCD_BIDI_2110_211f,
    UCD_BIDI_2120_212f,
    UCD_BIDI_0ed0_0edf, /*  UCD_BIDI_2130_213f  */
    UCD_BIDI_2140_214f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2150_215f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2160_216f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2170_217f  */
    UCD_BIDI_2180_218f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2190_219f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_21a0_21af  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_21b0_21bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_21c0_21cf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_21d0_21df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_21e0_21ef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_21f0_21ff  */
};
static const unsigned char    UCD_BIDI_2210_221f[16]= {ON,ON,ES,ET,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_2200_22ff[16]= {
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2200_220f  */
    UCD_BIDI_2210_221f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2220_222f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2230_223f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2240_224f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2250_225f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2260_226f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2270_227f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2280_228f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2290_229f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_22a0_22af  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_22b0_22bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_22c0_22cf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_22d0_22df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_22e0_22ef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_22f0_22ff  */
};
static const unsigned char    UCD_BIDI_2390_239f[16]= {ON,ON,ON,ON,ON,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_2300_23ff[16]= {
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2300_230f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2310_231f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2320_232f  */
    UCD_BIDI_0a60_0a6f, /*  UCD_BIDI_2330_233f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2340_234f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2350_235f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2360_236f  */
    UCD_BIDI_0050_005f, /*  UCD_BIDI_2370_237f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2380_238f  */
    UCD_BIDI_2390_239f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_23a0_23af  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_23b0_23bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_23c0_23cf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_23d0_23df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_23e0_23ef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_23f0_23ff  */
};
static const unsigned char    UCD_BIDI_2480_248f[16]= {ON,ON,ON,ON,ON,ON,ON,ON,EN,EN,EN,EN,EN,EN,EN,EN,};
static const unsigned char    UCD_BIDI_2490_249f[16]= {EN,EN,EN,EN,EN,EN,EN,EN,EN,EN,EN,EN,L,L,L,L,};
static const unsigned char * const UCD_BIDI_2400_24ff[16]= {
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2400_240f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2410_241f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2420_242f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2430_243f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2440_244f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2450_245f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2460_246f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2470_247f  */
    UCD_BIDI_2480_248f,
    UCD_BIDI_2490_249f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_24a0_24af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_24b0_24bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_24c0_24cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_24d0_24df  */
    UCD_BIDI_17e0_17ef, /*  UCD_BIDI_24e0_24ef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_24f0_24ff  */
};
static const unsigned char * const UCD_BIDI_2500_25ff[16]= {
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2500_250f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2510_251f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2520_252f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2530_253f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2540_254f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2550_255f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2560_256f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2570_257f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2580_258f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2590_259f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_25a0_25af  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_25b0_25bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_25c0_25cf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_25d0_25df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_25e0_25ef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_25f0_25ff  */
};
static const unsigned char    UCD_BIDI_26a0_26af[16]= {ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,L,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_2600_26ff[16]= {
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2600_260f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2610_261f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2620_262f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2630_263f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2640_264f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2650_265f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2660_266f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2670_267f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2680_268f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_2690_269f  */
    UCD_BIDI_26a0_26af,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_26b0_26bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_26c0_26cf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_26d0_26df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_26e0_26ef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_26f0_26ff  */
};
static const unsigned char    UCD_BIDI_2c20_2c2f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,ON,};
static const unsigned char    UCD_BIDI_2ce0_2cef[16]= {L,L,L,L,L,ON,ON,ON,ON,ON,ON,L,L,L,L,NSM,};
static const unsigned char    UCD_BIDI_2cf0_2cff[16]= {NSM,NSM,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_2c00_2cff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2c00_2c0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2c10_2c1f  */
    UCD_BIDI_2c20_2c2f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2c30_2c3f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2c40_2c4f  */
    UCD_BIDI_2c20_2c2f, /*  UCD_BIDI_2c50_2c5f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2c60_2c6f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2c70_2c7f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2c80_2c8f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2c90_2c9f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2ca0_2caf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2cb0_2cbf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2cc0_2ccf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2cd0_2cdf  */
    UCD_BIDI_2ce0_2cef,
    UCD_BIDI_2cf0_2cff,
};
static const unsigned char    UCD_BIDI_2d60_2d6f[16]= {L,L,L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,L,};
static const unsigned char    UCD_BIDI_2d70_2d7f[16]= {L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,NSM,};
static const unsigned char    UCD_BIDI_2d90_2d9f[16]= {L,L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_2da0_2daf[16]= {L,L,L,L,L,L,L,ON,L,L,L,L,L,L,L,ON,};
static const unsigned char * const UCD_BIDI_2d00_2dff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2d00_2d0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2d10_2d1f  */
    UCD_BIDI_10c0_10cf, /*  UCD_BIDI_2d20_2d2f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2d30_2d3f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2d40_2d4f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2d50_2d5f  */
    UCD_BIDI_2d60_2d6f,
    UCD_BIDI_2d70_2d7f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_2d80_2d8f  */
    UCD_BIDI_2d90_2d9f,
    UCD_BIDI_2da0_2daf,
    UCD_BIDI_2da0_2daf, /*  UCD_BIDI_2db0_2dbf  */
    UCD_BIDI_2da0_2daf, /*  UCD_BIDI_2dc0_2dcf  */
    UCD_BIDI_2da0_2daf, /*  UCD_BIDI_2dd0_2ddf  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_2de0_2def  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_2df0_2dff  */
};
static const unsigned char * const * const UCD_BIDI_2000_2fff[16]= {
    UCD_BIDI_2000_20ff,
    UCD_BIDI_2100_21ff,
    UCD_BIDI_2200_22ff,
    UCD_BIDI_2300_23ff,
    UCD_BIDI_2400_24ff,
    UCD_BIDI_2500_25ff,
    UCD_BIDI_2600_26ff,
    UCD_BIDI_2500_25ff, /*  UCD_BIDI_2700_27ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_2800_28ff  */
    UCD_BIDI_2500_25ff, /*  UCD_BIDI_2900_29ff  */
    UCD_BIDI_2500_25ff, /*  UCD_BIDI_2a00_2aff  */
    UCD_BIDI_2500_25ff, /*  UCD_BIDI_2b00_2bff  */
    UCD_BIDI_2c00_2cff,
    UCD_BIDI_2d00_2dff,
    UCD_BIDI_2500_25ff, /*  UCD_BIDI_2e00_2eff  */
    UCD_BIDI_2500_25ff, /*  UCD_BIDI_2f00_2fff  */
};
static const unsigned char    UCD_BIDI_3000_300f[16]= {WS,ON,ON,ON,ON,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_3020_302f[16]= {ON,L,L,L,L,L,L,L,L,L,NSM,NSM,NSM,NSM,L,L,};
static const unsigned char    UCD_BIDI_3030_303f[16]= {ON,L,L,L,L,L,ON,ON,L,L,L,L,L,ON,ON,ON,};
static const unsigned char    UCD_BIDI_3090_309f[16]= {L,L,L,L,L,L,L,ON,ON,NSM,NSM,ON,ON,L,L,L,};
static const unsigned char    UCD_BIDI_30f0_30ff[16]= {L,L,L,L,L,L,L,L,L,L,L,ON,L,L,L,L,};
static const unsigned char * const UCD_BIDI_3000_30ff[16]= {
    UCD_BIDI_3000_300f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_3010_301f  */
    UCD_BIDI_3020_302f,
    UCD_BIDI_3030_303f,
    UCD_BIDI_0040_004f, /*  UCD_BIDI_3040_304f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3050_305f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3060_306f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3070_307f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3080_308f  */
    UCD_BIDI_3090_309f,
    UCD_BIDI_0040_004f, /*  UCD_BIDI_30a0_30af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_30b0_30bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_30c0_30cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_30d0_30df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_30e0_30ef  */
    UCD_BIDI_30f0_30ff,
};
static const unsigned char    UCD_BIDI_3100_310f[16]= {ON,ON,ON,ON,ON,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char * const UCD_BIDI_3100_31ff[16]= {
    UCD_BIDI_3100_310f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3110_311f  */
    UCD_BIDI_1960_196f, /*  UCD_BIDI_3120_312f  */
    UCD_BIDI_0040_004f, /*  UCD_BIDI_3130_313f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3140_314f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3150_315f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3160_316f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3170_317f  */
    UCD_BIDI_2c20_2c2f, /*  UCD_BIDI_3180_318f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3190_319f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_31a0_31af  */
    UCD_BIDI_0050_005f, /*  UCD_BIDI_31b0_31bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_31c0_31cf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_31d0_31df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_31e0_31ef  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_31f0_31ff  */
};
static const unsigned char    UCD_BIDI_3270_327f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,ON,ON,ON,L,};
static const unsigned char * const UCD_BIDI_3200_32ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3200_320f  */
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_3210_321f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3220_322f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3230_323f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3240_324f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_3250_325f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3260_326f  */
    UCD_BIDI_3270_327f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3280_328f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3290_329f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_32a0_32af  */
    UCD_BIDI_0ad0_0adf, /*  UCD_BIDI_32b0_32bf  */
    UCD_BIDI_0e50_0e5f, /*  UCD_BIDI_32c0_32cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_32d0_32df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_32e0_32ef  */
    UCD_BIDI_2c20_2c2f, /*  UCD_BIDI_32f0_32ff  */
};
static const unsigned char    UCD_BIDI_3370_337f[16]= {L,L,L,L,L,L,L,ON,ON,ON,ON,L,L,L,L,L,};
static const unsigned char * const UCD_BIDI_3300_33ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3300_330f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3310_331f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3320_332f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3330_333f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3340_334f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3350_335f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3360_336f  */
    UCD_BIDI_3370_337f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3380_338f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_3390_339f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_33a0_33af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_33b0_33bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_33c0_33cf  */
    UCD_BIDI_1960_196f, /*  UCD_BIDI_33d0_33df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_33e0_33ef  */
    UCD_BIDI_2c20_2c2f, /*  UCD_BIDI_33f0_33ff  */
};
static const unsigned char * const * const UCD_BIDI_3000_3fff[16]= {
    UCD_BIDI_3000_30ff,
    UCD_BIDI_3100_31ff,
    UCD_BIDI_3200_32ff,
    UCD_BIDI_3300_33ff,
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3400_34ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3500_35ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3600_36ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3700_37ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3800_38ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3900_39ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3a00_3aff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3b00_3bff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3c00_3cff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3d00_3dff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3e00_3eff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_3f00_3fff  */
};
static const unsigned char * const UCD_BIDI_4d00_4dff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d00_4d0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d10_4d1f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d20_4d2f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d30_4d3f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d40_4d4f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d50_4d5f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d60_4d6f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d70_4d7f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d80_4d8f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4d90_4d9f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_4da0_4daf  */
    UCD_BIDI_18f0_18ff, /*  UCD_BIDI_4db0_4dbf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_4dc0_4dcf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_4dd0_4ddf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_4de0_4def  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_4df0_4dff  */
};
static const unsigned char * const * const UCD_BIDI_4000_4fff[16]= {
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4000_40ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4100_41ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4200_42ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4300_43ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4400_44ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4500_45ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4600_46ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4700_47ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4800_48ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4900_49ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4a00_4aff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4b00_4bff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4c00_4cff  */
    UCD_BIDI_4d00_4dff,
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4e00_4eff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_4f00_4fff  */
};
static const unsigned char * const * const UCD_BIDI_5000_5fff[16]= {
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5000_50ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5100_51ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5200_52ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5300_53ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5400_54ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5500_55ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5600_56ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5700_57ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5800_58ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5900_59ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5a00_5aff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5b00_5bff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5c00_5cff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5d00_5dff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5e00_5eff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_5f00_5fff  */
};
static const unsigned char * const UCD_BIDI_9f00_9fff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f00_9f0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f10_9f1f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f20_9f2f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f30_9f3f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f40_9f4f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f50_9f5f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f60_9f6f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f70_9f7f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f80_9f8f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9f90_9f9f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9fa0_9faf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_9fb0_9fbf  */
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_9fc0_9fcf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_9fd0_9fdf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_9fe0_9fef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_9ff0_9fff  */
};
static const unsigned char * const * const UCD_BIDI_9000_9fff[16]= {
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9000_90ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9100_91ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9200_92ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9300_93ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9400_94ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9500_95ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9600_96ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9700_97ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9800_98ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9900_99ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9a00_9aff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9b00_9bff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9c00_9cff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9d00_9dff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_9e00_9eff  */
    UCD_BIDI_9f00_9fff,
};
static const unsigned char * const UCD_BIDI_a400_a4ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a400_a40f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a410_a41f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a420_a42f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a430_a43f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a440_a44f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a450_a45f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a460_a46f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a470_a47f  */
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_a480_a48f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a490_a49f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a4a0_a4af  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a4b0_a4bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a4c0_a4cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a4d0_a4df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a4e0_a4ef  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a4f0_a4ff  */
};
static const unsigned char    UCD_BIDI_a660_a66f[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,NSM,};
static const unsigned char    UCD_BIDI_a670_a67f[16]= {NSM,NSM,NSM,ON,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,ON,ON,};
static const unsigned char    UCD_BIDI_a690_a69f[16]= {L,L,L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,NSM,};
static const unsigned char    UCD_BIDI_a6f0_a6ff[16]= {NSM,NSM,L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_a600_a6ff[16]= {
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_a600_a60f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a610_a61f  */
    UCD_BIDI_0e50_0e5f, /*  UCD_BIDI_a620_a62f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a630_a63f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a640_a64f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a650_a65f  */
    UCD_BIDI_a660_a66f,
    UCD_BIDI_a670_a67f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a680_a68f  */
    UCD_BIDI_a690_a69f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a6a0_a6af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a6b0_a6bf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a6c0_a6cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a6d0_a6df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a6e0_a6ef  */
    UCD_BIDI_a6f0_a6ff,
};
static const unsigned char    UCD_BIDI_a720_a72f[16]= {ON,ON,L,L,L,L,L,L,L,L,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_a780_a78f[16]= {L,L,L,L,L,L,L,L,ON,L,L,L,L,L,L,ON,};
static const unsigned char    UCD_BIDI_a790_a79f[16]= {L,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_a7f0_a7ff[16]= {ON,ON,ON,ON,ON,ON,ON,ON,L,L,L,L,L,L,L,L,};
static const unsigned char * const UCD_BIDI_a700_a7ff[16]= {
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a700_a70f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a710_a71f  */
    UCD_BIDI_a720_a72f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a730_a73f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a740_a74f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a750_a75f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a760_a76f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a770_a77f  */
    UCD_BIDI_a780_a78f,
    UCD_BIDI_a790_a79f,
    UCD_BIDI_0050_005f, /*  UCD_BIDI_a7a0_a7af  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a7b0_a7bf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a7c0_a7cf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a7d0_a7df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a7e0_a7ef  */
    UCD_BIDI_a7f0_a7ff,
};
static const unsigned char    UCD_BIDI_a800_a80f[16]= {L,L,NSM,L,L,L,NSM,L,L,L,L,NSM,L,L,L,L,};
static const unsigned char    UCD_BIDI_a820_a82f[16]= {L,L,L,L,L,NSM,NSM,L,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_a830_a83f[16]= {L,L,L,L,L,L,L,L,ET,ET,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_a8c0_a8cf[16]= {L,L,L,L,NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,L,L,};
static const unsigned char    UCD_BIDI_a8f0_a8ff[16]= {NSM,NSM,L,L,L,L,L,L,L,L,L,L,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_a800_a8ff[16]= {
    UCD_BIDI_a800_a80f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a810_a81f  */
    UCD_BIDI_a820_a82f,
    UCD_BIDI_a830_a83f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a840_a84f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a850_a85f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a860_a86f  */
    UCD_BIDI_a790_a79f, /*  UCD_BIDI_a870_a87f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a880_a88f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a890_a89f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a8a0_a8af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a8b0_a8bf  */
    UCD_BIDI_a8c0_a8cf,
    UCD_BIDI_17e0_17ef, /*  UCD_BIDI_a8d0_a8df  */
    UCD_BIDI_0300_030f, /*  UCD_BIDI_a8e0_a8ef  */
    UCD_BIDI_a8f0_a8ff,
};
static const unsigned char    UCD_BIDI_a920_a92f[16]= {L,L,L,L,L,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,L,L,};
static const unsigned char    UCD_BIDI_a940_a94f[16]= {L,L,L,L,L,L,L,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,NSM,};
static const unsigned char    UCD_BIDI_a950_a95f[16]= {NSM,NSM,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,L,};
static const unsigned char    UCD_BIDI_a9b0_a9bf[16]= {L,L,L,NSM,L,L,NSM,NSM,NSM,NSM,L,L,NSM,L,L,L,};
static const unsigned char    UCD_BIDI_a9c0_a9cf[16]= {L,L,L,L,L,L,L,L,L,L,L,L,L,L,ON,L,};
static const unsigned char * const UCD_BIDI_a900_a9ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a900_a90f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a910_a91f  */
    UCD_BIDI_a920_a92f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a930_a93f  */
    UCD_BIDI_a940_a94f,
    UCD_BIDI_a950_a95f,
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a960_a96f  */
    UCD_BIDI_0f60_0f6f, /*  UCD_BIDI_a970_a97f  */
    UCD_BIDI_0900_090f, /*  UCD_BIDI_a980_a98f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a990_a99f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_a9a0_a9af  */
    UCD_BIDI_a9b0_a9bf,
    UCD_BIDI_a9c0_a9cf,
    UCD_BIDI_0bb0_0bbf, /*  UCD_BIDI_a9d0_a9df  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a9e0_a9ef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_a9f0_a9ff  */
};
static const unsigned char    UCD_BIDI_aa20_aa2f[16]= {L,L,L,L,L,L,L,L,L,NSM,NSM,NSM,NSM,NSM,NSM,L,};
static const unsigned char    UCD_BIDI_aa30_aa3f[16]= {L,NSM,NSM,L,L,NSM,NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_aa40_aa4f[16]= {L,L,L,NSM,L,L,L,L,L,L,L,L,NSM,L,ON,ON,};
static const unsigned char    UCD_BIDI_aab0_aabf[16]= {NSM,L,NSM,NSM,NSM,L,L,NSM,NSM,L,L,L,L,L,NSM,NSM,};
static const unsigned char    UCD_BIDI_aac0_aacf[16]= {L,NSM,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_aad0_aadf[16]= {ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_aae0_aaef[16]= {L,L,L,L,L,L,L,L,L,L,L,L,NSM,NSM,L,L,};
static const unsigned char    UCD_BIDI_aaf0_aaff[16]= {L,L,L,L,L,L,NSM,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_aa00_aaff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_aa00_aa0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_aa10_aa1f  */
    UCD_BIDI_aa20_aa2f,
    UCD_BIDI_aa30_aa3f,
    UCD_BIDI_aa40_aa4f,
    UCD_BIDI_0ed0_0edf, /*  UCD_BIDI_aa50_aa5f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_aa60_aa6f  */
    UCD_BIDI_0e50_0e5f, /*  UCD_BIDI_aa70_aa7f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_aa80_aa8f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_aa90_aa9f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_aaa0_aaaf  */
    UCD_BIDI_aab0_aabf,
    UCD_BIDI_aac0_aacf,
    UCD_BIDI_aad0_aadf,
    UCD_BIDI_aae0_aaef,
    UCD_BIDI_aaf0_aaff,
};
static const unsigned char    UCD_BIDI_ab00_ab0f[16]= {ON,L,L,L,L,L,L,ON,ON,L,L,L,L,L,L,ON,};
static const unsigned char    UCD_BIDI_ab10_ab1f[16]= {ON,L,L,L,L,L,L,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_abe0_abef[16]= {L,L,L,L,L,NSM,L,L,NSM,L,L,L,L,NSM,ON,ON,};
static const unsigned char * const UCD_BIDI_ab00_abff[16]= {
    UCD_BIDI_ab00_ab0f,
    UCD_BIDI_ab10_ab1f,
    UCD_BIDI_2da0_2daf, /*  UCD_BIDI_ab20_ab2f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_ab30_ab3f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_ab40_ab4f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_ab50_ab5f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_ab60_ab6f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_ab70_ab7f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_ab80_ab8f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_ab90_ab9f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_aba0_abaf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_abb0_abbf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_abc0_abcf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_abd0_abdf  */
    UCD_BIDI_abe0_abef,
    UCD_BIDI_17e0_17ef, /*  UCD_BIDI_abf0_abff  */
};
static const unsigned char * const * const UCD_BIDI_a000_afff[16]= {
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_a000_a0ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_a100_a1ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_a200_a2ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_a300_a3ff  */
    UCD_BIDI_a400_a4ff,
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_a500_a5ff  */
    UCD_BIDI_a600_a6ff,
    UCD_BIDI_a700_a7ff,
    UCD_BIDI_a800_a8ff,
    UCD_BIDI_a900_a9ff,
    UCD_BIDI_aa00_aaff,
    UCD_BIDI_ab00_abff,
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_ac00_acff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_ad00_adff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_ae00_aeff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_af00_afff  */
};
static const unsigned char * const UCD_BIDI_d700_d7ff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d700_d70f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d710_d71f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d720_d72f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d730_d73f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d740_d74f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d750_d75f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d760_d76f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d770_d77f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d780_d78f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d790_d79f  */
    UCD_BIDI_a790_a79f, /*  UCD_BIDI_d7a0_d7af  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d7b0_d7bf  */
    UCD_BIDI_3370_337f, /*  UCD_BIDI_d7c0_d7cf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d7d0_d7df  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_d7e0_d7ef  */
    UCD_BIDI_0e50_0e5f, /*  UCD_BIDI_d7f0_d7ff  */
};
static const unsigned char * const * const UCD_BIDI_d000_dfff[16]= {
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d000_d0ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d100_d1ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d200_d2ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d300_d3ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d400_d4ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d500_d5ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d600_d6ff  */
    UCD_BIDI_d700_d7ff,
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d800_d8ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_d900_d9ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_da00_daff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_db00_dbff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_dc00_dcff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_dd00_ddff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_de00_deff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_df00_dfff  */
};
static const unsigned char * const UCD_BIDI_fa00_faff[16]= {
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa00_fa0f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa10_fa1f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa20_fa2f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa30_fa3f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa40_fa4f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa50_fa5f  */
    UCD_BIDI_1960_196f, /*  UCD_BIDI_fa60_fa6f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa70_fa7f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa80_fa8f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fa90_fa9f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_faa0_faaf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fab0_fabf  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_fac0_facf  */
    UCD_BIDI_17e0_17ef, /*  UCD_BIDI_fad0_fadf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_fae0_faef  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_faf0_faff  */
};
static const unsigned char    UCD_BIDI_fb10_fb1f[16]= {ON,ON,ON,L,L,L,L,L,ON,ON,ON,ON,ON,R,NSM,R,};
static const unsigned char    UCD_BIDI_fb20_fb2f[16]= {R,R,R,R,R,R,R,R,R,ES,R,R,R,R,R,R,};
static const unsigned char    UCD_BIDI_fb30_fb3f[16]= {R,R,R,R,R,R,R,ON,R,R,R,R,R,ON,R,ON,};
static const unsigned char    UCD_BIDI_fb40_fb4f[16]= {R,R,ON,R,R,ON,R,R,R,R,R,R,R,R,R,R,};
static const unsigned char    UCD_BIDI_fbc0_fbcf[16]= {AL,AL,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_fbd0_fbdf[16]= {ON,ON,ON,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,};
static const unsigned char * const UCD_BIDI_fb00_fbff[16]= {
    UCD_BIDI_2d90_2d9f, /*  UCD_BIDI_fb00_fb0f  */
    UCD_BIDI_fb10_fb1f,
    UCD_BIDI_fb20_fb2f,
    UCD_BIDI_fb30_fb3f,
    UCD_BIDI_fb40_fb4f,
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fb50_fb5f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fb60_fb6f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fb70_fb7f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fb80_fb8f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fb90_fb9f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fba0_fbaf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fbb0_fbbf  */
    UCD_BIDI_fbc0_fbcf,
    UCD_BIDI_fbd0_fbdf,
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fbe0_fbef  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fbf0_fbff  */
};
static const unsigned char * const UCD_BIDI_fc00_fcff[16]= {
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc00_fc0f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc10_fc1f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc20_fc2f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc30_fc3f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc40_fc4f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc50_fc5f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc60_fc6f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc70_fc7f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc80_fc8f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fc90_fc9f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fca0_fcaf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fcb0_fcbf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fcc0_fccf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fcd0_fcdf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fce0_fcef  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fcf0_fcff  */
};
static const unsigned char    UCD_BIDI_fd30_fd3f[16]= {AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,ON,ON,};
static const unsigned char    UCD_BIDI_fd90_fd9f[16]= {ON,ON,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,};
static const unsigned char    UCD_BIDI_fdc0_fdcf[16]= {AL,AL,AL,AL,AL,AL,AL,AL,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_fdf0_fdff[16]= {AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_fd00_fdff[16]= {
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fd00_fd0f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fd10_fd1f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fd20_fd2f  */
    UCD_BIDI_fd30_fd3f,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_fd40_fd4f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fd50_fd5f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fd60_fd6f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fd70_fd7f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fd80_fd8f  */
    UCD_BIDI_fd90_fd9f,
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fda0_fdaf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fdb0_fdbf  */
    UCD_BIDI_fdc0_fdcf,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_fdd0_fddf  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_fde0_fdef  */
    UCD_BIDI_fdf0_fdff,
};
static const unsigned char    UCD_BIDI_fe50_fe5f[16]= {CS,ON,CS,ON,ON,CS,ON,ON,ON,ON,ON,ON,ON,ON,ON,ET,};
static const unsigned char    UCD_BIDI_fe60_fe6f[16]= {ON,ON,ES,ES,ON,ON,ON,ON,ON,ET,ET,ON,ON,ON,ON,ON,};
static const unsigned char    UCD_BIDI_fe70_fe7f[16]= {AL,AL,AL,AL,AL,ON,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,};
static const unsigned char    UCD_BIDI_fef0_feff[16]= {AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,AL,ON,ON,BN,};
static const unsigned char * const UCD_BIDI_fe00_feff[16]= {
    UCD_BIDI_0300_030f, /*  UCD_BIDI_fe00_fe0f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_fe10_fe1f  */
    UCD_BIDI_1de0_1def, /*  UCD_BIDI_fe20_fe2f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_fe30_fe3f  */
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_fe40_fe4f  */
    UCD_BIDI_fe50_fe5f,
    UCD_BIDI_fe60_fe6f,
    UCD_BIDI_fe70_fe7f,
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fe80_fe8f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fe90_fe9f  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fea0_feaf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_feb0_febf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fec0_fecf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fed0_fedf  */
    UCD_BIDI_0620_062f, /*  UCD_BIDI_fee0_feef  */
    UCD_BIDI_fef0_feff,
};
static const unsigned char    UCD_BIDI_ff00_ff0f[16]= {ON,ON,ON,ET,ET,ET,ON,ON,ON,ON,ON,ES,CS,ES,CS,CS,};
static const unsigned char    UCD_BIDI_ffc0_ffcf[16]= {ON,ON,L,L,L,L,L,L,ON,ON,L,L,L,L,L,L,};
static const unsigned char    UCD_BIDI_ffd0_ffdf[16]= {ON,ON,L,L,L,L,L,L,ON,ON,L,L,L,ON,ON,ON,};
static const unsigned char    UCD_BIDI_ffe0_ffef[16]= {ET,ET,ON,ON,ON,ET,ET,ON,ON,ON,ON,ON,ON,ON,ON,ON,};
static const unsigned char * const UCD_BIDI_ff00_ffff[16]= {
    UCD_BIDI_ff00_ff0f,
    UCD_BIDI_0030_003f, /*  UCD_BIDI_ff10_ff1f  */
    UCD_BIDI_0040_004f, /*  UCD_BIDI_ff20_ff2f  */
    UCD_BIDI_0050_005f, /*  UCD_BIDI_ff30_ff3f  */
    UCD_BIDI_0040_004f, /*  UCD_BIDI_ff40_ff4f  */
    UCD_BIDI_0050_005f, /*  UCD_BIDI_ff50_ff5f  */
    UCD_BIDI_0a60_0a6f, /*  UCD_BIDI_ff60_ff6f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_ff70_ff7f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_ff80_ff8f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_ff90_ff9f  */
    UCD_BIDI_00c0_00cf, /*  UCD_BIDI_ffa0_ffaf  */
    UCD_BIDI_2c20_2c2f, /*  UCD_BIDI_ffb0_ffbf  */
    UCD_BIDI_ffc0_ffcf,
    UCD_BIDI_ffd0_ffdf,
    UCD_BIDI_ffe0_ffef,
    UCD_BIDI_02f0_02ff, /*  UCD_BIDI_fff0_ffff  */
};
static const unsigned char * const * const UCD_BIDI_f000_ffff[16]= {
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f000_f0ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f100_f1ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f200_f2ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f300_f3ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f400_f4ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f500_f5ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f600_f6ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f700_f7ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f800_f8ff  */
    UCD_BIDI_0100_01ff, /*  UCD_BIDI_f900_f9ff  */
    UCD_BIDI_fa00_faff,
    UCD_BIDI_fb00_fbff,
    UCD_BIDI_fc00_fcff,
    UCD_BIDI_fd00_fdff,
    UCD_BIDI_fe00_feff,
    UCD_BIDI_ff00_ffff,
};
static const unsigned char * const * const * const UCD_BIDI[16]={
    UCD_BIDI_0000_0fff,
    UCD_BIDI_1000_1fff,
    UCD_BIDI_2000_2fff,
    UCD_BIDI_3000_3fff,
    UCD_BIDI_4000_4fff,
    UCD_BIDI_5000_5fff,
    UCD_BIDI_5000_5fff, /*  UCD_BIDI_6000_6fff  */
    UCD_BIDI_5000_5fff, /*  UCD_BIDI_7000_7fff  */
    UCD_BIDI_5000_5fff, /*  UCD_BIDI_8000_8fff  */
    UCD_BIDI_9000_9fff,
    UCD_BIDI_a000_afff,
    UCD_BIDI_5000_5fff, /*  UCD_BIDI_b000_bfff  */
    UCD_BIDI_5000_5fff, /*  UCD_BIDI_c000_cfff  */
    UCD_BIDI_d000_dfff,
    UCD_BIDI_5000_5fff, /*  UCD_BIDI_e000_efff  */
    UCD_BIDI_f000_ffff,
};

int ucdBidiClass( int sym ) {
    return ucdSymbolClass( sym, UCD_BIDI, ON );
}
const char * ucdBidiClassStr( int sym ) {
    static char scratch[20];
    switch( sym )
	{
	case ET:	return "ET";
	case S:	return "S";
	case LRO:	return "LRO";
	case AL:	return "AL";
	case RLO:	return "RLO";
	case BN:	return "BN";
	case RLE:	return "RLE";
	case L:	return "L";
	case LRE:	return "LRE";
	case NSM:	return "NSM";
	case ON:	return "ON";
	case B:	return "B";
	case PDF:	return "PDF";
	case CS:	return "CS";
	case AN:	return "AN";
	case WS:	return "WS";
	case R:	return "R";
	case ES:	return "ES";
	case EN:	return "EN";
	default:
	    sprintf( scratch, "%d", sym );
	    return scratch;
	}
}
