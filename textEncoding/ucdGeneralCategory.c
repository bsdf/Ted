#   include <stdio.h>

#   include "ucdIntern.h"
#   include "ucdGeneralCategory.h"

#   define Co	UCDgc_Co
#   define Lm	UCDgc_Lm
#   define Pd	UCDgc_Pd
#   define So	UCDgc_So
#   define Nd	UCDgc_Nd
#   define Cn	UCDgc_Cn
#   define Zp	UCDgc_Zp
#   define Po	UCDgc_Po
#   define No	UCDgc_No
#   define Mn	UCDgc_Mn
#   define Cf	UCDgc_Cf
#   define Zs	UCDgc_Zs
#   define Pe	UCDgc_Pe
#   define Pf	UCDgc_Pf
#   define Ps	UCDgc_Ps
#   define Cc	UCDgc_Cc
#   define Lu	UCDgc_Lu
#   define Mc	UCDgc_Mc
#   define Sm	UCDgc_Sm
#   define Cs	UCDgc_Cs
#   define Lt	UCDgc_Lt
#   define Pc	UCDgc_Pc
#   define Nl	UCDgc_Nl
#   define Lo	UCDgc_Lo
#   define Pi	UCDgc_Pi
#   define Me	UCDgc_Me
#   define Ll	UCDgc_Ll
#   define Zl	UCDgc_Zl
#   define Sk	UCDgc_Sk
#   define Sc	UCDgc_Sc

static const unsigned char    UCD_GC_0000_000f[16]= {Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,Cc,};
static const unsigned char    UCD_GC_0020_002f[16]= {Zs,Po,Po,Po,Sc,Po,Po,Po,Ps,Pe,Po,Sm,Po,Pd,Po,Po,};
static const unsigned char    UCD_GC_0030_003f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Po,Po,Sm,Sm,Sm,Po,};
static const unsigned char    UCD_GC_0040_004f[16]= {Po,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,};
static const unsigned char    UCD_GC_0050_005f[16]= {Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Ps,Po,Pe,Sk,Pc,};
static const unsigned char    UCD_GC_0060_006f[16]= {Sk,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,};
static const unsigned char    UCD_GC_0070_007f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ps,Sm,Pe,Sm,Cc,};
static const unsigned char    UCD_GC_00a0_00af[16]= {Zs,Po,Sc,Sc,Sc,Sc,So,Po,Sk,So,Lo,Pi,Sm,Cf,So,Sk,};
static const unsigned char    UCD_GC_00b0_00bf[16]= {So,Sm,No,No,Sk,Ll,Po,Po,Sk,No,Lo,Pf,No,No,No,Po,};
static const unsigned char    UCD_GC_00c0_00cf[16]= {Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,};
static const unsigned char    UCD_GC_00d0_00df[16]= {Lu,Lu,Lu,Lu,Lu,Lu,Lu,Sm,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Ll,};
static const unsigned char    UCD_GC_00e0_00ef[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,};
static const unsigned char    UCD_GC_00f0_00ff[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Sm,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,};
static const unsigned char * const UCD_GC_0000_00ff[16]= {
    UCD_GC_0000_000f,
    UCD_GC_0000_000f, /*  UCD_GC_0010_001f  */
    UCD_GC_0020_002f,
    UCD_GC_0030_003f,
    UCD_GC_0040_004f,
    UCD_GC_0050_005f,
    UCD_GC_0060_006f,
    UCD_GC_0070_007f,
    UCD_GC_0000_000f, /*  UCD_GC_0080_008f  */
    UCD_GC_0000_000f, /*  UCD_GC_0090_009f  */
    UCD_GC_00a0_00af,
    UCD_GC_00b0_00bf,
    UCD_GC_00c0_00cf,
    UCD_GC_00d0_00df,
    UCD_GC_00e0_00ef,
    UCD_GC_00f0_00ff,
};
static const unsigned char    UCD_GC_0100_010f[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,};
static const unsigned char    UCD_GC_0130_013f[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,};
static const unsigned char    UCD_GC_0140_014f[16]= {Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Ll,Lu,Ll,Lu,Ll,Lu,Ll,};
static const unsigned char    UCD_GC_0170_017f[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Lu,Ll,Lu,Ll,Lu,Ll,Ll,};
static const unsigned char    UCD_GC_0180_018f[16]= {Ll,Lu,Lu,Ll,Lu,Ll,Lu,Lu,Ll,Lu,Lu,Lu,Ll,Ll,Lu,Lu,};
static const unsigned char    UCD_GC_0190_019f[16]= {Lu,Lu,Ll,Lu,Lu,Ll,Lu,Lu,Lu,Ll,Ll,Ll,Lu,Lu,Ll,Lu,};
static const unsigned char    UCD_GC_01a0_01af[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Lu,Ll,Lu,Ll,Ll,Lu,Ll,Lu,Lu,};
static const unsigned char    UCD_GC_01b0_01bf[16]= {Ll,Lu,Lu,Lu,Ll,Lu,Ll,Lu,Lu,Ll,Ll,Lo,Lu,Ll,Ll,Ll,};
static const unsigned char    UCD_GC_01c0_01cf[16]= {Lo,Lo,Lo,Lo,Lu,Lt,Ll,Lu,Lt,Ll,Lu,Lt,Ll,Lu,Ll,Lu,};
static const unsigned char    UCD_GC_01d0_01df[16]= {Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Ll,Lu,Ll,};
static const unsigned char    UCD_GC_01f0_01ff[16]= {Ll,Lu,Lt,Ll,Lu,Ll,Lu,Lu,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,};
static const unsigned char * const UCD_GC_0100_01ff[16]= {
    UCD_GC_0100_010f,
    UCD_GC_0100_010f, /*  UCD_GC_0110_011f  */
    UCD_GC_0100_010f, /*  UCD_GC_0120_012f  */
    UCD_GC_0130_013f,
    UCD_GC_0140_014f,
    UCD_GC_0100_010f, /*  UCD_GC_0150_015f  */
    UCD_GC_0100_010f, /*  UCD_GC_0160_016f  */
    UCD_GC_0170_017f,
    UCD_GC_0180_018f,
    UCD_GC_0190_019f,
    UCD_GC_01a0_01af,
    UCD_GC_01b0_01bf,
    UCD_GC_01c0_01cf,
    UCD_GC_01d0_01df,
    UCD_GC_0100_010f, /*  UCD_GC_01e0_01ef  */
    UCD_GC_01f0_01ff,
};
static const unsigned char    UCD_GC_0230_023f[16]= {Lu,Ll,Lu,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lu,Lu,Ll,Lu,Lu,Ll,};
static const unsigned char    UCD_GC_0240_024f[16]= {Ll,Lu,Ll,Lu,Lu,Lu,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,};
static const unsigned char    UCD_GC_0290_029f[16]= {Ll,Ll,Ll,Ll,Lo,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,};
static const unsigned char    UCD_GC_02b0_02bf[16]= {Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,};
static const unsigned char    UCD_GC_02c0_02cf[16]= {Lm,Lm,Sk,Sk,Sk,Sk,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,};
static const unsigned char    UCD_GC_02d0_02df[16]= {Lm,Lm,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,};
static const unsigned char    UCD_GC_02e0_02ef[16]= {Lm,Lm,Lm,Lm,Lm,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Lm,Sk,Lm,Sk,};
static const unsigned char    UCD_GC_02f0_02ff[16]= {Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,};
static const unsigned char * const UCD_GC_0200_02ff[16]= {
    UCD_GC_0100_010f, /*  UCD_GC_0200_020f  */
    UCD_GC_0100_010f, /*  UCD_GC_0210_021f  */
    UCD_GC_0100_010f, /*  UCD_GC_0220_022f  */
    UCD_GC_0230_023f,
    UCD_GC_0240_024f,
    UCD_GC_00e0_00ef, /*  UCD_GC_0250_025f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_0260_026f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_0270_027f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_0280_028f  */
    UCD_GC_0290_029f,
    UCD_GC_00e0_00ef, /*  UCD_GC_02a0_02af  */
    UCD_GC_02b0_02bf,
    UCD_GC_02c0_02cf,
    UCD_GC_02d0_02df,
    UCD_GC_02e0_02ef,
    UCD_GC_02f0_02ff,
};
static const unsigned char    UCD_GC_0300_030f[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_0370_037f[16]= {Lu,Ll,Lu,Ll,Lm,Sk,Lu,Ll,Cn,Cn,Lm,Ll,Ll,Ll,Po,Cn,};
static const unsigned char    UCD_GC_0380_038f[16]= {Cn,Cn,Cn,Cn,Sk,Sk,Lu,Po,Lu,Lu,Lu,Cn,Lu,Cn,Lu,Lu,};
static const unsigned char    UCD_GC_0390_039f[16]= {Ll,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,};
static const unsigned char    UCD_GC_03a0_03af[16]= {Lu,Lu,Cn,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Ll,Ll,Ll,Ll,};
static const unsigned char    UCD_GC_03c0_03cf[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lu,};
static const unsigned char    UCD_GC_03d0_03df[16]= {Ll,Ll,Lu,Lu,Lu,Ll,Ll,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,};
static const unsigned char    UCD_GC_03f0_03ff[16]= {Ll,Ll,Ll,Ll,Lu,Ll,Sm,Lu,Ll,Lu,Lu,Ll,Ll,Lu,Lu,Lu,};
static const unsigned char * const UCD_GC_0300_03ff[16]= {
    UCD_GC_0300_030f,
    UCD_GC_0300_030f, /*  UCD_GC_0310_031f  */
    UCD_GC_0300_030f, /*  UCD_GC_0320_032f  */
    UCD_GC_0300_030f, /*  UCD_GC_0330_033f  */
    UCD_GC_0300_030f, /*  UCD_GC_0340_034f  */
    UCD_GC_0300_030f, /*  UCD_GC_0350_035f  */
    UCD_GC_0300_030f, /*  UCD_GC_0360_036f  */
    UCD_GC_0370_037f,
    UCD_GC_0380_038f,
    UCD_GC_0390_039f,
    UCD_GC_03a0_03af,
    UCD_GC_00e0_00ef, /*  UCD_GC_03b0_03bf  */
    UCD_GC_03c0_03cf,
    UCD_GC_03d0_03df,
    UCD_GC_0100_010f, /*  UCD_GC_03e0_03ef  */
    UCD_GC_03f0_03ff,
};
static const unsigned char    UCD_GC_0480_048f[16]= {Lu,Ll,So,Mn,Mn,Mn,Mn,Mn,Me,Me,Lu,Ll,Lu,Ll,Lu,Ll,};
static const unsigned char    UCD_GC_04c0_04cf[16]= {Lu,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Ll,};
static const unsigned char * const UCD_GC_0400_04ff[16]= {
    UCD_GC_00c0_00cf, /*  UCD_GC_0400_040f  */
    UCD_GC_00c0_00cf, /*  UCD_GC_0410_041f  */
    UCD_GC_00c0_00cf, /*  UCD_GC_0420_042f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_0430_043f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_0440_044f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_0450_045f  */
    UCD_GC_0100_010f, /*  UCD_GC_0460_046f  */
    UCD_GC_0100_010f, /*  UCD_GC_0470_047f  */
    UCD_GC_0480_048f,
    UCD_GC_0100_010f, /*  UCD_GC_0490_049f  */
    UCD_GC_0100_010f, /*  UCD_GC_04a0_04af  */
    UCD_GC_0100_010f, /*  UCD_GC_04b0_04bf  */
    UCD_GC_04c0_04cf,
    UCD_GC_0100_010f, /*  UCD_GC_04d0_04df  */
    UCD_GC_0100_010f, /*  UCD_GC_04e0_04ef  */
    UCD_GC_0100_010f, /*  UCD_GC_04f0_04ff  */
};
static const unsigned char    UCD_GC_0520_052f[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0530_053f[16]= {Cn,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,};
static const unsigned char    UCD_GC_0550_055f[16]= {Lu,Lu,Lu,Lu,Lu,Lu,Lu,Cn,Cn,Lm,Po,Po,Po,Po,Po,Po,};
static const unsigned char    UCD_GC_0560_056f[16]= {Cn,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,};
static const unsigned char    UCD_GC_0580_058f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Cn,Po,Pd,Cn,Cn,Cn,Cn,Sc,};
static const unsigned char    UCD_GC_0590_059f[16]= {Cn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_05b0_05bf[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Pd,Mn,};
static const unsigned char    UCD_GC_05c0_05cf[16]= {Po,Mn,Mn,Po,Mn,Mn,Po,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_05d0_05df[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_05e0_05ef[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_05f0_05ff[16]= {Lo,Lo,Lo,Po,Po,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_0500_05ff[16]= {
    UCD_GC_0100_010f, /*  UCD_GC_0500_050f  */
    UCD_GC_0100_010f, /*  UCD_GC_0510_051f  */
    UCD_GC_0520_052f,
    UCD_GC_0530_053f,
    UCD_GC_00c0_00cf, /*  UCD_GC_0540_054f  */
    UCD_GC_0550_055f,
    UCD_GC_0560_056f,
    UCD_GC_00e0_00ef, /*  UCD_GC_0570_057f  */
    UCD_GC_0580_058f,
    UCD_GC_0590_059f,
    UCD_GC_0300_030f, /*  UCD_GC_05a0_05af  */
    UCD_GC_05b0_05bf,
    UCD_GC_05c0_05cf,
    UCD_GC_05d0_05df,
    UCD_GC_05e0_05ef,
    UCD_GC_05f0_05ff,
};
static const unsigned char    UCD_GC_0600_060f[16]= {Cf,Cf,Cf,Cf,Cf,Cn,Sm,Sm,Sm,Po,Po,Sc,Po,Po,So,So,};
static const unsigned char    UCD_GC_0610_061f[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Po,Cn,Cn,Po,Po,};
static const unsigned char    UCD_GC_0640_064f[16]= {Lm,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_0660_066f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Po,Po,Po,Po,Lo,Lo,};
static const unsigned char    UCD_GC_0670_067f[16]= {Mn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_06d0_06df[16]= {Lo,Lo,Lo,Lo,Po,Lo,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cf,So,Mn,};
static const unsigned char    UCD_GC_06e0_06ef[16]= {Mn,Mn,Mn,Mn,Mn,Lm,Lm,Mn,Mn,So,Mn,Mn,Mn,Mn,Lo,Lo,};
static const unsigned char    UCD_GC_06f0_06ff[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Lo,Lo,Lo,So,So,Lo,};
static const unsigned char * const UCD_GC_0600_06ff[16]= {
    UCD_GC_0600_060f,
    UCD_GC_0610_061f,
    UCD_GC_05d0_05df, /*  UCD_GC_0620_062f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0630_063f  */
    UCD_GC_0640_064f,
    UCD_GC_0300_030f, /*  UCD_GC_0650_065f  */
    UCD_GC_0660_066f,
    UCD_GC_0670_067f,
    UCD_GC_05d0_05df, /*  UCD_GC_0680_068f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0690_069f  */
    UCD_GC_05d0_05df, /*  UCD_GC_06a0_06af  */
    UCD_GC_05d0_05df, /*  UCD_GC_06b0_06bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_06c0_06cf  */
    UCD_GC_06d0_06df,
    UCD_GC_06e0_06ef,
    UCD_GC_06f0_06ff,
};
static const unsigned char    UCD_GC_0700_070f[16]= {Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Cn,Cf,};
static const unsigned char    UCD_GC_0710_071f[16]= {Lo,Mn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0740_074f[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,Cn,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_07a0_07af[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_07b0_07bf[16]= {Mn,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_07c0_07cf[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_07e0_07ef[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_07f0_07ff[16]= {Mn,Mn,Mn,Mn,Lm,Lm,So,Po,Po,Po,Lm,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_0700_07ff[16]= {
    UCD_GC_0700_070f,
    UCD_GC_0710_071f,
    UCD_GC_05d0_05df, /*  UCD_GC_0720_072f  */
    UCD_GC_0300_030f, /*  UCD_GC_0730_073f  */
    UCD_GC_0740_074f,
    UCD_GC_05d0_05df, /*  UCD_GC_0750_075f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0760_076f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0770_077f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0780_078f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0790_079f  */
    UCD_GC_07a0_07af,
    UCD_GC_07b0_07bf,
    UCD_GC_07c0_07cf,
    UCD_GC_05d0_05df, /*  UCD_GC_07d0_07df  */
    UCD_GC_07e0_07ef,
    UCD_GC_07f0_07ff,
};
static const unsigned char    UCD_GC_0810_081f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,Mn,Lm,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_0820_082f[16]= {Mn,Mn,Mn,Mn,Lm,Mn,Mn,Mn,Lm,Mn,Mn,Mn,Mn,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_0830_083f[16]= {Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Cn,};
static const unsigned char    UCD_GC_0850_085f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,Cn,Cn,Po,Cn,};
static const unsigned char    UCD_GC_0860_086f[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_08a0_08af[16]= {Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_08e0_08ef[16]= {Cn,Cn,Cn,Cn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_08f0_08ff[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,};
static const unsigned char * const UCD_GC_0800_08ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_0800_080f  */
    UCD_GC_0810_081f,
    UCD_GC_0820_082f,
    UCD_GC_0830_083f,
    UCD_GC_05d0_05df, /*  UCD_GC_0840_084f  */
    UCD_GC_0850_085f,
    UCD_GC_0860_086f,
    UCD_GC_0860_086f, /*  UCD_GC_0870_087f  */
    UCD_GC_0860_086f, /*  UCD_GC_0880_088f  */
    UCD_GC_0860_086f, /*  UCD_GC_0890_089f  */
    UCD_GC_08a0_08af,
    UCD_GC_0860_086f, /*  UCD_GC_08b0_08bf  */
    UCD_GC_0860_086f, /*  UCD_GC_08c0_08cf  */
    UCD_GC_0860_086f, /*  UCD_GC_08d0_08df  */
    UCD_GC_08e0_08ef,
    UCD_GC_08f0_08ff,
};
static const unsigned char    UCD_GC_0900_090f[16]= {Mn,Mn,Mn,Mc,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0930_093f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mc,Mn,Lo,Mc,Mc,};
static const unsigned char    UCD_GC_0940_094f[16]= {Mc,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mc,Mc,Mc,Mc,Mn,Mc,Mc,};
static const unsigned char    UCD_GC_0950_095f[16]= {Lo,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0960_096f[16]= {Lo,Lo,Mn,Mn,Po,Po,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,};
static const unsigned char    UCD_GC_0970_097f[16]= {Po,Lm,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0980_098f[16]= {Cn,Mn,Mc,Mc,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Lo,};
static const unsigned char    UCD_GC_0990_099f[16]= {Lo,Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_09a0_09af[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_09b0_09bf[16]= {Lo,Cn,Lo,Cn,Cn,Cn,Lo,Lo,Lo,Lo,Cn,Cn,Mn,Lo,Mc,Mc,};
static const unsigned char    UCD_GC_09c0_09cf[16]= {Mc,Mn,Mn,Mn,Mn,Cn,Cn,Mc,Mc,Cn,Cn,Mc,Mc,Mn,Lo,Cn,};
static const unsigned char    UCD_GC_09d0_09df[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Cn,Mc,Cn,Cn,Cn,Cn,Lo,Lo,Cn,Lo,};
static const unsigned char    UCD_GC_09e0_09ef[16]= {Lo,Lo,Mn,Mn,Cn,Cn,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,};
static const unsigned char    UCD_GC_09f0_09ff[16]= {Lo,Lo,Sc,Sc,No,No,No,No,No,No,So,Sc,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_0900_09ff[16]= {
    UCD_GC_0900_090f,
    UCD_GC_05d0_05df, /*  UCD_GC_0910_091f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0920_092f  */
    UCD_GC_0930_093f,
    UCD_GC_0940_094f,
    UCD_GC_0950_095f,
    UCD_GC_0960_096f,
    UCD_GC_0970_097f,
    UCD_GC_0980_098f,
    UCD_GC_0990_099f,
    UCD_GC_09a0_09af,
    UCD_GC_09b0_09bf,
    UCD_GC_09c0_09cf,
    UCD_GC_09d0_09df,
    UCD_GC_09e0_09ef,
    UCD_GC_09f0_09ff,
};
static const unsigned char    UCD_GC_0a00_0a0f[16]= {Cn,Mn,Mn,Mc,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Lo,};
static const unsigned char    UCD_GC_0a30_0a3f[16]= {Lo,Cn,Lo,Lo,Cn,Lo,Lo,Cn,Lo,Lo,Cn,Cn,Mn,Cn,Mc,Mc,};
static const unsigned char    UCD_GC_0a40_0a4f[16]= {Mc,Mn,Mn,Cn,Cn,Cn,Cn,Mn,Mn,Cn,Cn,Mn,Mn,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_0a50_0a5f[16]= {Cn,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Lo,Lo,Lo,Lo,Cn,Lo,Cn,};
static const unsigned char    UCD_GC_0a60_0a6f[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,};
static const unsigned char    UCD_GC_0a70_0a7f[16]= {Mn,Mn,Lo,Lo,Lo,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0a80_0a8f[16]= {Cn,Mn,Mn,Mc,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,};
static const unsigned char    UCD_GC_0a90_0a9f[16]= {Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0ab0_0abf[16]= {Lo,Cn,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Mn,Lo,Mc,Mc,};
static const unsigned char    UCD_GC_0ac0_0acf[16]= {Mc,Mn,Mn,Mn,Mn,Mn,Cn,Mn,Mn,Mc,Cn,Mc,Mc,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_0ad0_0adf[16]= {Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0af0_0aff[16]= {Po,Sc,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_0a00_0aff[16]= {
    UCD_GC_0a00_0a0f,
    UCD_GC_0990_099f, /*  UCD_GC_0a10_0a1f  */
    UCD_GC_09a0_09af, /*  UCD_GC_0a20_0a2f  */
    UCD_GC_0a30_0a3f,
    UCD_GC_0a40_0a4f,
    UCD_GC_0a50_0a5f,
    UCD_GC_0a60_0a6f,
    UCD_GC_0a70_0a7f,
    UCD_GC_0a80_0a8f,
    UCD_GC_0a90_0a9f,
    UCD_GC_09a0_09af, /*  UCD_GC_0aa0_0aaf  */
    UCD_GC_0ab0_0abf,
    UCD_GC_0ac0_0acf,
    UCD_GC_0ad0_0adf,
    UCD_GC_09e0_09ef, /*  UCD_GC_0ae0_0aef  */
    UCD_GC_0af0_0aff,
};
static const unsigned char    UCD_GC_0b30_0b3f[16]= {Lo,Cn,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Mn,Lo,Mc,Mn,};
static const unsigned char    UCD_GC_0b40_0b4f[16]= {Mc,Mn,Mn,Mn,Mn,Cn,Cn,Mc,Mc,Cn,Cn,Mc,Mc,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_0b50_0b5f[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Mn,Mc,Cn,Cn,Cn,Cn,Lo,Lo,Cn,Lo,};
static const unsigned char    UCD_GC_0b70_0b7f[16]= {So,Lo,No,No,No,No,No,No,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0b80_0b8f[16]= {Cn,Cn,Mn,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Lo,Lo,};
static const unsigned char    UCD_GC_0b90_0b9f[16]= {Lo,Cn,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Lo,Lo,Cn,Lo,Cn,Lo,Lo,};
static const unsigned char    UCD_GC_0ba0_0baf[16]= {Cn,Cn,Cn,Lo,Lo,Cn,Cn,Cn,Lo,Lo,Lo,Cn,Cn,Cn,Lo,Lo,};
static const unsigned char    UCD_GC_0bb0_0bbf[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Mc,Mc,};
static const unsigned char    UCD_GC_0bc0_0bcf[16]= {Mn,Mc,Mc,Cn,Cn,Cn,Mc,Mc,Mc,Cn,Mc,Mc,Mc,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_0bd0_0bdf[16]= {Lo,Cn,Cn,Cn,Cn,Cn,Cn,Mc,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0bf0_0bff[16]= {No,No,No,So,So,So,So,So,So,Sc,So,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_0b00_0bff[16]= {
    UCD_GC_0980_098f, /*  UCD_GC_0b00_0b0f  */
    UCD_GC_0990_099f, /*  UCD_GC_0b10_0b1f  */
    UCD_GC_09a0_09af, /*  UCD_GC_0b20_0b2f  */
    UCD_GC_0b30_0b3f,
    UCD_GC_0b40_0b4f,
    UCD_GC_0b50_0b5f,
    UCD_GC_09e0_09ef, /*  UCD_GC_0b60_0b6f  */
    UCD_GC_0b70_0b7f,
    UCD_GC_0b80_0b8f,
    UCD_GC_0b90_0b9f,
    UCD_GC_0ba0_0baf,
    UCD_GC_0bb0_0bbf,
    UCD_GC_0bc0_0bcf,
    UCD_GC_0bd0_0bdf,
    UCD_GC_0a60_0a6f, /*  UCD_GC_0be0_0bef  */
    UCD_GC_0bf0_0bff,
};
static const unsigned char    UCD_GC_0c00_0c0f[16]= {Cn,Mc,Mc,Mc,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,};
static const unsigned char    UCD_GC_0c10_0c1f[16]= {Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0c30_0c3f[16]= {Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Lo,Mn,Mn,};
static const unsigned char    UCD_GC_0c40_0c4f[16]= {Mn,Mc,Mc,Mc,Mc,Cn,Mn,Mn,Mn,Cn,Mn,Mn,Mn,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_0c50_0c5f[16]= {Cn,Cn,Cn,Cn,Cn,Mn,Mn,Cn,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0c70_0c7f[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,No,No,No,No,No,No,No,So,};
static const unsigned char    UCD_GC_0c80_0c8f[16]= {Cn,Cn,Mc,Mc,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,};
static const unsigned char    UCD_GC_0cb0_0cbf[16]= {Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Mn,Lo,Mc,Mn,};
static const unsigned char    UCD_GC_0cc0_0ccf[16]= {Mc,Mc,Mc,Mc,Mc,Cn,Mn,Mc,Mc,Cn,Mc,Mc,Mn,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_0cd0_0cdf[16]= {Cn,Cn,Cn,Cn,Cn,Mc,Mc,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Lo,Cn,};
static const unsigned char    UCD_GC_0cf0_0cff[16]= {Cn,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_0c00_0cff[16]= {
    UCD_GC_0c00_0c0f,
    UCD_GC_0c10_0c1f,
    UCD_GC_09a0_09af, /*  UCD_GC_0c20_0c2f  */
    UCD_GC_0c30_0c3f,
    UCD_GC_0c40_0c4f,
    UCD_GC_0c50_0c5f,
    UCD_GC_09e0_09ef, /*  UCD_GC_0c60_0c6f  */
    UCD_GC_0c70_0c7f,
    UCD_GC_0c80_0c8f,
    UCD_GC_0c10_0c1f, /*  UCD_GC_0c90_0c9f  */
    UCD_GC_09a0_09af, /*  UCD_GC_0ca0_0caf  */
    UCD_GC_0cb0_0cbf,
    UCD_GC_0cc0_0ccf,
    UCD_GC_0cd0_0cdf,
    UCD_GC_09e0_09ef, /*  UCD_GC_0ce0_0cef  */
    UCD_GC_0cf0_0cff,
};
static const unsigned char    UCD_GC_0d30_0d3f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Lo,Mc,Mc,};
static const unsigned char    UCD_GC_0d40_0d4f[16]= {Mc,Mn,Mn,Mn,Mn,Cn,Mc,Mc,Mc,Cn,Mc,Mc,Mc,Mn,Lo,Cn,};
static const unsigned char    UCD_GC_0d50_0d5f[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Cn,Mc,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0d70_0d7f[16]= {No,No,No,No,No,No,Cn,Cn,Cn,So,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0d80_0d8f[16]= {Cn,Cn,Mc,Mc,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0d90_0d9f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0db0_0dbf[16]= {Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Cn,Cn,};
static const unsigned char    UCD_GC_0dc0_0dcf[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Mn,Cn,Cn,Cn,Cn,Mc,};
static const unsigned char    UCD_GC_0dd0_0ddf[16]= {Mc,Mc,Mn,Mn,Mn,Cn,Mn,Cn,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,};
static const unsigned char    UCD_GC_0df0_0dff[16]= {Cn,Cn,Mc,Mc,Po,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_0d00_0dff[16]= {
    UCD_GC_0c80_0c8f, /*  UCD_GC_0d00_0d0f  */
    UCD_GC_0c10_0c1f, /*  UCD_GC_0d10_0d1f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0d20_0d2f  */
    UCD_GC_0d30_0d3f,
    UCD_GC_0d40_0d4f,
    UCD_GC_0d50_0d5f,
    UCD_GC_09e0_09ef, /*  UCD_GC_0d60_0d6f  */
    UCD_GC_0d70_0d7f,
    UCD_GC_0d80_0d8f,
    UCD_GC_0d90_0d9f,
    UCD_GC_05d0_05df, /*  UCD_GC_0da0_0daf  */
    UCD_GC_0db0_0dbf,
    UCD_GC_0dc0_0dcf,
    UCD_GC_0dd0_0ddf,
    UCD_GC_0860_086f, /*  UCD_GC_0de0_0def  */
    UCD_GC_0df0_0dff,
};
static const unsigned char    UCD_GC_0e00_0e0f[16]= {Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0e30_0e3f[16]= {Lo,Mn,Lo,Lo,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,Cn,Cn,Cn,Sc,};
static const unsigned char    UCD_GC_0e40_0e4f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lm,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Po,};
static const unsigned char    UCD_GC_0e50_0e5f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Po,Po,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0e80_0e8f[16]= {Cn,Lo,Lo,Cn,Lo,Cn,Cn,Lo,Lo,Cn,Lo,Cn,Cn,Lo,Cn,Cn,};
static const unsigned char    UCD_GC_0e90_0e9f[16]= {Cn,Cn,Cn,Cn,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0ea0_0eaf[16]= {Cn,Lo,Lo,Lo,Cn,Lo,Cn,Lo,Cn,Cn,Lo,Lo,Cn,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0eb0_0ebf[16]= {Lo,Mn,Lo,Lo,Mn,Mn,Mn,Mn,Mn,Mn,Cn,Mn,Mn,Lo,Cn,Cn,};
static const unsigned char    UCD_GC_0ec0_0ecf[16]= {Lo,Lo,Lo,Lo,Lo,Cn,Lm,Cn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_0ed0_0edf[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Cn,Cn,Lo,Lo,Lo,Lo,};
static const unsigned char * const UCD_GC_0e00_0eff[16]= {
    UCD_GC_0e00_0e0f,
    UCD_GC_05d0_05df, /*  UCD_GC_0e10_0e1f  */
    UCD_GC_05d0_05df, /*  UCD_GC_0e20_0e2f  */
    UCD_GC_0e30_0e3f,
    UCD_GC_0e40_0e4f,
    UCD_GC_0e50_0e5f,
    UCD_GC_0860_086f, /*  UCD_GC_0e60_0e6f  */
    UCD_GC_0860_086f, /*  UCD_GC_0e70_0e7f  */
    UCD_GC_0e80_0e8f,
    UCD_GC_0e90_0e9f,
    UCD_GC_0ea0_0eaf,
    UCD_GC_0eb0_0ebf,
    UCD_GC_0ec0_0ecf,
    UCD_GC_0ed0_0edf,
    UCD_GC_0860_086f, /*  UCD_GC_0ee0_0eef  */
    UCD_GC_0860_086f, /*  UCD_GC_0ef0_0eff  */
};
static const unsigned char    UCD_GC_0f00_0f0f[16]= {Lo,So,So,So,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,};
static const unsigned char    UCD_GC_0f10_0f1f[16]= {Po,Po,Po,So,Po,So,So,So,Mn,Mn,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_0f20_0f2f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,No,No,No,No,No,No,};
static const unsigned char    UCD_GC_0f30_0f3f[16]= {No,No,No,No,So,Mn,So,Mn,So,Mn,Ps,Pe,Ps,Pe,Mc,Mc,};
static const unsigned char    UCD_GC_0f40_0f4f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_0f60_0f6f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_0f70_0f7f[16]= {Cn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mc,};
static const unsigned char    UCD_GC_0f80_0f8f[16]= {Mn,Mn,Mn,Mn,Mn,Po,Mn,Mn,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_0f90_0f9f[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_0fb0_0fbf[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,So,So,};
static const unsigned char    UCD_GC_0fc0_0fcf[16]= {So,So,So,So,So,So,Mn,So,So,So,So,So,So,Cn,So,So,};
static const unsigned char    UCD_GC_0fd0_0fdf[16]= {Po,Po,Po,Po,Po,So,So,So,So,Po,Po,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_0f00_0fff[16]= {
    UCD_GC_0f00_0f0f,
    UCD_GC_0f10_0f1f,
    UCD_GC_0f20_0f2f,
    UCD_GC_0f30_0f3f,
    UCD_GC_0f40_0f4f,
    UCD_GC_05d0_05df, /*  UCD_GC_0f50_0f5f  */
    UCD_GC_0f60_0f6f,
    UCD_GC_0f70_0f7f,
    UCD_GC_0f80_0f8f,
    UCD_GC_0f90_0f9f,
    UCD_GC_0300_030f, /*  UCD_GC_0fa0_0faf  */
    UCD_GC_0fb0_0fbf,
    UCD_GC_0fc0_0fcf,
    UCD_GC_0fd0_0fdf,
    UCD_GC_0860_086f, /*  UCD_GC_0fe0_0fef  */
    UCD_GC_0860_086f, /*  UCD_GC_0ff0_0fff  */
};
static const unsigned char * const * const UCD_GC_0000_0fff[16]= {
    UCD_GC_0000_00ff,
    UCD_GC_0100_01ff,
    UCD_GC_0200_02ff,
    UCD_GC_0300_03ff,
    UCD_GC_0400_04ff,
    UCD_GC_0500_05ff,
    UCD_GC_0600_06ff,
    UCD_GC_0700_07ff,
    UCD_GC_0800_08ff,
    UCD_GC_0900_09ff,
    UCD_GC_0a00_0aff,
    UCD_GC_0b00_0bff,
    UCD_GC_0c00_0cff,
    UCD_GC_0d00_0dff,
    UCD_GC_0e00_0eff,
    UCD_GC_0f00_0fff,
};
static const unsigned char    UCD_GC_1020_102f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mc,Mc,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_1030_103f[16]= {Mn,Mc,Mn,Mn,Mn,Mn,Mn,Mn,Mc,Mn,Mn,Mc,Mc,Mn,Mn,Lo,};
static const unsigned char    UCD_GC_1040_104f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Po,Po,Po,Po,Po,Po,};
static const unsigned char    UCD_GC_1050_105f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Mc,Mc,Mn,Mn,Lo,Lo,Lo,Lo,Mn,Mn,};
static const unsigned char    UCD_GC_1060_106f[16]= {Mn,Lo,Mc,Mc,Mc,Lo,Lo,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Lo,Lo,};
static const unsigned char    UCD_GC_1070_107f[16]= {Lo,Mn,Mn,Mn,Mn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_1080_108f[16]= {Lo,Lo,Mn,Mc,Mc,Mn,Mn,Mc,Mc,Mc,Mc,Mc,Mc,Mn,Lo,Mc,};
static const unsigned char    UCD_GC_1090_109f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Mc,Mc,Mc,Mn,So,So,};
static const unsigned char    UCD_GC_10c0_10cf[16]= {Lu,Lu,Lu,Lu,Lu,Lu,Cn,Lu,Cn,Cn,Cn,Cn,Cn,Lu,Cn,Cn,};
static const unsigned char    UCD_GC_10f0_10ff[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Po,Lm,Lo,Lo,Lo,};
static const unsigned char * const UCD_GC_1000_10ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_1000_100f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1010_101f  */
    UCD_GC_1020_102f,
    UCD_GC_1030_103f,
    UCD_GC_1040_104f,
    UCD_GC_1050_105f,
    UCD_GC_1060_106f,
    UCD_GC_1070_107f,
    UCD_GC_1080_108f,
    UCD_GC_1090_109f,
    UCD_GC_00c0_00cf, /*  UCD_GC_10a0_10af  */
    UCD_GC_00c0_00cf, /*  UCD_GC_10b0_10bf  */
    UCD_GC_10c0_10cf,
    UCD_GC_05d0_05df, /*  UCD_GC_10d0_10df  */
    UCD_GC_05d0_05df, /*  UCD_GC_10e0_10ef  */
    UCD_GC_10f0_10ff,
};
static const unsigned char * const UCD_GC_1100_11ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_1100_110f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1110_111f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1120_112f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1130_113f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1140_114f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1150_115f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1160_116f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1170_117f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1180_118f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1190_119f  */
    UCD_GC_05d0_05df, /*  UCD_GC_11a0_11af  */
    UCD_GC_05d0_05df, /*  UCD_GC_11b0_11bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_11c0_11cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_11d0_11df  */
    UCD_GC_05d0_05df, /*  UCD_GC_11e0_11ef  */
    UCD_GC_05d0_05df, /*  UCD_GC_11f0_11ff  */
};
static const unsigned char    UCD_GC_1240_124f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Cn,Cn,};
static const unsigned char    UCD_GC_1250_125f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Cn,Lo,Lo,Lo,Lo,Cn,Cn,};
static const unsigned char    UCD_GC_12b0_12bf[16]= {Lo,Cn,Lo,Lo,Lo,Lo,Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,};
static const unsigned char    UCD_GC_12c0_12cf[16]= {Lo,Cn,Lo,Lo,Lo,Lo,Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_12d0_12df[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char * const UCD_GC_1200_12ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_1200_120f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1210_121f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1220_122f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1230_123f  */
    UCD_GC_1240_124f,
    UCD_GC_1250_125f,
    UCD_GC_05d0_05df, /*  UCD_GC_1260_126f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1270_127f  */
    UCD_GC_1240_124f, /*  UCD_GC_1280_128f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1290_129f  */
    UCD_GC_05d0_05df, /*  UCD_GC_12a0_12af  */
    UCD_GC_12b0_12bf,
    UCD_GC_12c0_12cf,
    UCD_GC_12d0_12df,
    UCD_GC_05d0_05df, /*  UCD_GC_12e0_12ef  */
    UCD_GC_05d0_05df, /*  UCD_GC_12f0_12ff  */
};
static const unsigned char    UCD_GC_1350_135f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_1360_136f[16]= {Po,Po,Po,Po,Po,Po,Po,Po,Po,No,No,No,No,No,No,No,};
static const unsigned char    UCD_GC_1370_137f[16]= {No,No,No,No,No,No,No,No,No,No,No,No,No,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1390_139f[16]= {So,So,So,So,So,So,So,So,So,So,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_13f0_13ff[16]= {Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_1300_13ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_1300_130f  */
    UCD_GC_12c0_12cf, /*  UCD_GC_1310_131f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1320_132f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1330_133f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1340_134f  */
    UCD_GC_1350_135f,
    UCD_GC_1360_136f,
    UCD_GC_1370_137f,
    UCD_GC_05d0_05df, /*  UCD_GC_1380_138f  */
    UCD_GC_1390_139f,
    UCD_GC_05d0_05df, /*  UCD_GC_13a0_13af  */
    UCD_GC_05d0_05df, /*  UCD_GC_13b0_13bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_13c0_13cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_13d0_13df  */
    UCD_GC_05d0_05df, /*  UCD_GC_13e0_13ef  */
    UCD_GC_13f0_13ff,
};
static const unsigned char    UCD_GC_1400_140f[16]= {Pd,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char * const UCD_GC_1400_14ff[16]= {
    UCD_GC_1400_140f,
    UCD_GC_05d0_05df, /*  UCD_GC_1410_141f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1420_142f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1430_143f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1440_144f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1450_145f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1460_146f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1470_147f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1480_148f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1490_149f  */
    UCD_GC_05d0_05df, /*  UCD_GC_14a0_14af  */
    UCD_GC_05d0_05df, /*  UCD_GC_14b0_14bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_14c0_14cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_14d0_14df  */
    UCD_GC_05d0_05df, /*  UCD_GC_14e0_14ef  */
    UCD_GC_05d0_05df, /*  UCD_GC_14f0_14ff  */
};
static const unsigned char    UCD_GC_1660_166f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Po,Po,Lo,};
static const unsigned char    UCD_GC_1680_168f[16]= {Zs,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_1690_169f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Ps,Pe,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_16e0_16ef[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Po,Po,Po,Nl,Nl,};
static const unsigned char    UCD_GC_16f0_16ff[16]= {Nl,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_1600_16ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_1600_160f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1610_161f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1620_162f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1630_163f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1640_164f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1650_165f  */
    UCD_GC_1660_166f,
    UCD_GC_05d0_05df, /*  UCD_GC_1670_167f  */
    UCD_GC_1680_168f,
    UCD_GC_1690_169f,
    UCD_GC_05d0_05df, /*  UCD_GC_16a0_16af  */
    UCD_GC_05d0_05df, /*  UCD_GC_16b0_16bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_16c0_16cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_16d0_16df  */
    UCD_GC_16e0_16ef,
    UCD_GC_16f0_16ff,
};
static const unsigned char    UCD_GC_1700_170f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,};
static const unsigned char    UCD_GC_1710_171f[16]= {Lo,Lo,Mn,Mn,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1730_173f[16]= {Lo,Lo,Mn,Mn,Mn,Po,Po,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1750_175f[16]= {Lo,Lo,Mn,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1770_177f[16]= {Lo,Cn,Mn,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_17b0_17bf[16]= {Lo,Lo,Lo,Lo,Mn,Mn,Mc,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mc,Mc,};
static const unsigned char    UCD_GC_17c0_17cf[16]= {Mc,Mc,Mc,Mc,Mc,Mc,Mn,Mc,Mc,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_17d0_17df[16]= {Mn,Mn,Mn,Mn,Po,Po,Po,Lm,Po,Po,Po,Sc,Lo,Mn,Cn,Cn,};
static const unsigned char    UCD_GC_17e0_17ef[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_17f0_17ff[16]= {No,No,No,No,No,No,No,No,No,No,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_1700_17ff[16]= {
    UCD_GC_1700_170f,
    UCD_GC_1710_171f,
    UCD_GC_05d0_05df, /*  UCD_GC_1720_172f  */
    UCD_GC_1730_173f,
    UCD_GC_05d0_05df, /*  UCD_GC_1740_174f  */
    UCD_GC_1750_175f,
    UCD_GC_1700_170f, /*  UCD_GC_1760_176f  */
    UCD_GC_1770_177f,
    UCD_GC_05d0_05df, /*  UCD_GC_1780_178f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1790_179f  */
    UCD_GC_05d0_05df, /*  UCD_GC_17a0_17af  */
    UCD_GC_17b0_17bf,
    UCD_GC_17c0_17cf,
    UCD_GC_17d0_17df,
    UCD_GC_17e0_17ef,
    UCD_GC_17f0_17ff,
};
static const unsigned char    UCD_GC_1800_180f[16]= {Po,Po,Po,Po,Po,Po,Pd,Po,Po,Po,Po,Mn,Mn,Mn,Zs,Cn,};
static const unsigned char    UCD_GC_1840_184f[16]= {Lo,Lo,Lo,Lm,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_1870_187f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_18a0_18af[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Lo,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_18f0_18ff[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_1800_18ff[16]= {
    UCD_GC_1800_180f,
    UCD_GC_17e0_17ef, /*  UCD_GC_1810_181f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1820_182f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1830_183f  */
    UCD_GC_1840_184f,
    UCD_GC_05d0_05df, /*  UCD_GC_1850_185f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1860_186f  */
    UCD_GC_1870_187f,
    UCD_GC_05d0_05df, /*  UCD_GC_1880_188f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1890_189f  */
    UCD_GC_18a0_18af,
    UCD_GC_05d0_05df, /*  UCD_GC_18b0_18bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_18c0_18cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_18d0_18df  */
    UCD_GC_05d0_05df, /*  UCD_GC_18e0_18ef  */
    UCD_GC_18f0_18ff,
};
static const unsigned char    UCD_GC_1920_192f[16]= {Mn,Mn,Mn,Mc,Mc,Mc,Mc,Mn,Mn,Mc,Mc,Mc,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1930_193f[16]= {Mc,Mc,Mn,Mc,Mc,Mc,Mc,Mc,Mc,Mn,Mn,Mn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1940_194f[16]= {So,Cn,Cn,Cn,Po,Po,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,};
static const unsigned char    UCD_GC_1960_196f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,};
static const unsigned char    UCD_GC_19a0_19af[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_19b0_19bf[16]= {Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,};
static const unsigned char    UCD_GC_19c0_19cf[16]= {Mc,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mc,Mc,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_19d0_19df[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,No,Cn,Cn,Cn,So,So,};
static const unsigned char    UCD_GC_19e0_19ef[16]= {So,So,So,So,So,So,So,So,So,So,So,So,So,So,So,So,};
static const unsigned char * const UCD_GC_1900_19ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_1900_190f  */
    UCD_GC_0f60_0f6f, /*  UCD_GC_1910_191f  */
    UCD_GC_1920_192f,
    UCD_GC_1930_193f,
    UCD_GC_1940_194f,
    UCD_GC_05d0_05df, /*  UCD_GC_1950_195f  */
    UCD_GC_1960_196f,
    UCD_GC_13f0_13ff, /*  UCD_GC_1970_197f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1980_198f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1990_199f  */
    UCD_GC_19a0_19af,
    UCD_GC_19b0_19bf,
    UCD_GC_19c0_19cf,
    UCD_GC_19d0_19df,
    UCD_GC_19e0_19ef,
    UCD_GC_19e0_19ef, /*  UCD_GC_19f0_19ff  */
};
static const unsigned char    UCD_GC_1a10_1a1f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mc,Mc,Mc,Cn,Cn,Po,Po,};
static const unsigned char    UCD_GC_1a50_1a5f[16]= {Lo,Lo,Lo,Lo,Lo,Mc,Mn,Mc,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,};
static const unsigned char    UCD_GC_1a60_1a6f[16]= {Mn,Mc,Mn,Mc,Mc,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mc,Mc,Mc,};
static const unsigned char    UCD_GC_1a70_1a7f[16]= {Mc,Mc,Mc,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,Cn,Mn,};
static const unsigned char    UCD_GC_1aa0_1aaf[16]= {Po,Po,Po,Po,Po,Po,Po,Lm,Po,Po,Po,Po,Po,Po,Cn,Cn,};
static const unsigned char * const UCD_GC_1a00_1aff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_1a00_1a0f  */
    UCD_GC_1a10_1a1f,
    UCD_GC_05d0_05df, /*  UCD_GC_1a20_1a2f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1a30_1a3f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1a40_1a4f  */
    UCD_GC_1a50_1a5f,
    UCD_GC_1a60_1a6f,
    UCD_GC_1a70_1a7f,
    UCD_GC_17e0_17ef, /*  UCD_GC_1a80_1a8f  */
    UCD_GC_17e0_17ef, /*  UCD_GC_1a90_1a9f  */
    UCD_GC_1aa0_1aaf,
    UCD_GC_0860_086f, /*  UCD_GC_1ab0_1abf  */
    UCD_GC_0860_086f, /*  UCD_GC_1ac0_1acf  */
    UCD_GC_0860_086f, /*  UCD_GC_1ad0_1adf  */
    UCD_GC_0860_086f, /*  UCD_GC_1ae0_1aef  */
    UCD_GC_0860_086f, /*  UCD_GC_1af0_1aff  */
};
static const unsigned char    UCD_GC_1b00_1b0f[16]= {Mn,Mn,Mn,Mn,Mc,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_1b30_1b3f[16]= {Lo,Lo,Lo,Lo,Mn,Mc,Mn,Mn,Mn,Mn,Mn,Mc,Mn,Mc,Mc,Mc,};
static const unsigned char    UCD_GC_1b40_1b4f[16]= {Mc,Mc,Mn,Mc,Mc,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1b60_1b6f[16]= {Po,So,So,So,So,So,So,So,So,So,So,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_1b70_1b7f[16]= {Mn,Mn,Mn,Mn,So,So,So,So,So,So,So,So,So,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1b80_1b8f[16]= {Mn,Mn,Mc,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_1ba0_1baf[16]= {Lo,Mc,Mn,Mn,Mn,Mn,Mc,Mc,Mn,Mn,Mc,Mn,Mc,Mc,Lo,Lo,};
static const unsigned char    UCD_GC_1be0_1bef[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mc,Mn,Mn,Mc,Mc,Mc,Mn,Mc,Mn,};
static const unsigned char    UCD_GC_1bf0_1bff[16]= {Mn,Mn,Mc,Mc,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Po,Po,Po,Po,};
static const unsigned char * const UCD_GC_1b00_1bff[16]= {
    UCD_GC_1b00_1b0f,
    UCD_GC_05d0_05df, /*  UCD_GC_1b10_1b1f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1b20_1b2f  */
    UCD_GC_1b30_1b3f,
    UCD_GC_1b40_1b4f,
    UCD_GC_1040_104f, /*  UCD_GC_1b50_1b5f  */
    UCD_GC_1b60_1b6f,
    UCD_GC_1b70_1b7f,
    UCD_GC_1b80_1b8f,
    UCD_GC_05d0_05df, /*  UCD_GC_1b90_1b9f  */
    UCD_GC_1ba0_1baf,
    UCD_GC_07c0_07cf, /*  UCD_GC_1bb0_1bbf  */
    UCD_GC_05d0_05df, /*  UCD_GC_1bc0_1bcf  */
    UCD_GC_05d0_05df, /*  UCD_GC_1bd0_1bdf  */
    UCD_GC_1be0_1bef,
    UCD_GC_1bf0_1bff,
};
static const unsigned char    UCD_GC_1c20_1c2f[16]= {Lo,Lo,Lo,Lo,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_1c30_1c3f[16]= {Mn,Mn,Mn,Mn,Mc,Mc,Mn,Mn,Cn,Cn,Cn,Po,Po,Po,Po,Po,};
static const unsigned char    UCD_GC_1c40_1c4f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Cn,Cn,Cn,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_1c70_1c7f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lm,Lm,Lm,Lm,Lm,Lm,Po,Po,};
static const unsigned char    UCD_GC_1cc0_1ccf[16]= {Po,Po,Po,Po,Po,Po,Po,Po,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1cd0_1cdf[16]= {Mn,Mn,Mn,Po,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_1ce0_1cef[16]= {Mn,Mc,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Lo,Lo,Lo,Lo,Mn,Lo,Lo,};
static const unsigned char    UCD_GC_1cf0_1cff[16]= {Lo,Lo,Mc,Mc,Mn,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_1c00_1cff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_1c00_1c0f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1c10_1c1f  */
    UCD_GC_1c20_1c2f,
    UCD_GC_1c30_1c3f,
    UCD_GC_1c40_1c4f,
    UCD_GC_07c0_07cf, /*  UCD_GC_1c50_1c5f  */
    UCD_GC_05d0_05df, /*  UCD_GC_1c60_1c6f  */
    UCD_GC_1c70_1c7f,
    UCD_GC_0860_086f, /*  UCD_GC_1c80_1c8f  */
    UCD_GC_0860_086f, /*  UCD_GC_1c90_1c9f  */
    UCD_GC_0860_086f, /*  UCD_GC_1ca0_1caf  */
    UCD_GC_0860_086f, /*  UCD_GC_1cb0_1cbf  */
    UCD_GC_1cc0_1ccf,
    UCD_GC_1cd0_1cdf,
    UCD_GC_1ce0_1cef,
    UCD_GC_1cf0_1cff,
};
static const unsigned char    UCD_GC_1d20_1d2f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lm,Lm,Lm,Lm,};
static const unsigned char    UCD_GC_1d60_1d6f[16]= {Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Ll,Ll,Ll,Ll,Ll,};
static const unsigned char    UCD_GC_1d70_1d7f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lm,Ll,Ll,Ll,Ll,Ll,Ll,Ll,};
static const unsigned char    UCD_GC_1d90_1d9f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lm,Lm,Lm,Lm,Lm,};
static const unsigned char    UCD_GC_1de0_1def[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_1df0_1dff[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Mn,Mn,Mn,Mn,};
static const unsigned char * const UCD_GC_1d00_1dff[16]= {
    UCD_GC_00e0_00ef, /*  UCD_GC_1d00_1d0f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_1d10_1d1f  */
    UCD_GC_1d20_1d2f,
    UCD_GC_02b0_02bf, /*  UCD_GC_1d30_1d3f  */
    UCD_GC_02b0_02bf, /*  UCD_GC_1d40_1d4f  */
    UCD_GC_02b0_02bf, /*  UCD_GC_1d50_1d5f  */
    UCD_GC_1d60_1d6f,
    UCD_GC_1d70_1d7f,
    UCD_GC_00e0_00ef, /*  UCD_GC_1d80_1d8f  */
    UCD_GC_1d90_1d9f,
    UCD_GC_02b0_02bf, /*  UCD_GC_1da0_1daf  */
    UCD_GC_02b0_02bf, /*  UCD_GC_1db0_1dbf  */
    UCD_GC_0300_030f, /*  UCD_GC_1dc0_1dcf  */
    UCD_GC_0300_030f, /*  UCD_GC_1dd0_1ddf  */
    UCD_GC_1de0_1def,
    UCD_GC_1df0_1dff,
};
static const unsigned char    UCD_GC_1e90_1e9f[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lu,Ll,};
static const unsigned char * const UCD_GC_1e00_1eff[16]= {
    UCD_GC_0100_010f, /*  UCD_GC_1e00_1e0f  */
    UCD_GC_0100_010f, /*  UCD_GC_1e10_1e1f  */
    UCD_GC_0100_010f, /*  UCD_GC_1e20_1e2f  */
    UCD_GC_0100_010f, /*  UCD_GC_1e30_1e3f  */
    UCD_GC_0100_010f, /*  UCD_GC_1e40_1e4f  */
    UCD_GC_0100_010f, /*  UCD_GC_1e50_1e5f  */
    UCD_GC_0100_010f, /*  UCD_GC_1e60_1e6f  */
    UCD_GC_0100_010f, /*  UCD_GC_1e70_1e7f  */
    UCD_GC_0100_010f, /*  UCD_GC_1e80_1e8f  */
    UCD_GC_1e90_1e9f,
    UCD_GC_0100_010f, /*  UCD_GC_1ea0_1eaf  */
    UCD_GC_0100_010f, /*  UCD_GC_1eb0_1ebf  */
    UCD_GC_0100_010f, /*  UCD_GC_1ec0_1ecf  */
    UCD_GC_0100_010f, /*  UCD_GC_1ed0_1edf  */
    UCD_GC_0100_010f, /*  UCD_GC_1ee0_1eef  */
    UCD_GC_0100_010f, /*  UCD_GC_1ef0_1eff  */
};
static const unsigned char    UCD_GC_1f00_1f0f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,};
static const unsigned char    UCD_GC_1f10_1f1f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Cn,Cn,Lu,Lu,Lu,Lu,Lu,Lu,Cn,Cn,};
static const unsigned char    UCD_GC_1f50_1f5f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Cn,Lu,Cn,Lu,Cn,Lu,Cn,Lu,};
static const unsigned char    UCD_GC_1f70_1f7f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Cn,Cn,};
static const unsigned char    UCD_GC_1f80_1f8f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lt,Lt,Lt,Lt,Lt,Lt,Lt,Lt,};
static const unsigned char    UCD_GC_1fb0_1fbf[16]= {Ll,Ll,Ll,Ll,Ll,Cn,Ll,Ll,Lu,Lu,Lu,Lu,Lt,Sk,Ll,Sk,};
static const unsigned char    UCD_GC_1fc0_1fcf[16]= {Sk,Sk,Ll,Ll,Ll,Cn,Ll,Ll,Lu,Lu,Lu,Lu,Lt,Sk,Sk,Sk,};
static const unsigned char    UCD_GC_1fd0_1fdf[16]= {Ll,Ll,Ll,Ll,Cn,Cn,Ll,Ll,Lu,Lu,Lu,Lu,Cn,Sk,Sk,Sk,};
static const unsigned char    UCD_GC_1fe0_1fef[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lu,Lu,Lu,Lu,Lu,Sk,Sk,Sk,};
static const unsigned char    UCD_GC_1ff0_1fff[16]= {Cn,Cn,Ll,Ll,Ll,Cn,Ll,Ll,Lu,Lu,Lu,Lu,Lt,Sk,Sk,Cn,};
static const unsigned char * const UCD_GC_1f00_1fff[16]= {
    UCD_GC_1f00_1f0f,
    UCD_GC_1f10_1f1f,
    UCD_GC_1f00_1f0f, /*  UCD_GC_1f20_1f2f  */
    UCD_GC_1f00_1f0f, /*  UCD_GC_1f30_1f3f  */
    UCD_GC_1f10_1f1f, /*  UCD_GC_1f40_1f4f  */
    UCD_GC_1f50_1f5f,
    UCD_GC_1f00_1f0f, /*  UCD_GC_1f60_1f6f  */
    UCD_GC_1f70_1f7f,
    UCD_GC_1f80_1f8f,
    UCD_GC_1f80_1f8f, /*  UCD_GC_1f90_1f9f  */
    UCD_GC_1f80_1f8f, /*  UCD_GC_1fa0_1faf  */
    UCD_GC_1fb0_1fbf,
    UCD_GC_1fc0_1fcf,
    UCD_GC_1fd0_1fdf,
    UCD_GC_1fe0_1fef,
    UCD_GC_1ff0_1fff,
};
static const unsigned char * const * const UCD_GC_1000_1fff[16]= {
    UCD_GC_1000_10ff,
    UCD_GC_1100_11ff,
    UCD_GC_1200_12ff,
    UCD_GC_1300_13ff,
    UCD_GC_1400_14ff,
    UCD_GC_1100_11ff, /*  UCD_GC_1500_15ff  */
    UCD_GC_1600_16ff,
    UCD_GC_1700_17ff,
    UCD_GC_1800_18ff,
    UCD_GC_1900_19ff,
    UCD_GC_1a00_1aff,
    UCD_GC_1b00_1bff,
    UCD_GC_1c00_1cff,
    UCD_GC_1d00_1dff,
    UCD_GC_1e00_1eff,
    UCD_GC_1f00_1fff,
};
static const unsigned char    UCD_GC_2000_200f[16]= {Zs,Zs,Zs,Zs,Zs,Zs,Zs,Zs,Zs,Zs,Zs,Cf,Cf,Cf,Cf,Cf,};
static const unsigned char    UCD_GC_2010_201f[16]= {Pd,Pd,Pd,Pd,Pd,Pd,Po,Po,Pi,Pf,Ps,Pi,Pi,Pf,Ps,Pi,};
static const unsigned char    UCD_GC_2020_202f[16]= {Po,Po,Po,Po,Po,Po,Po,Po,Zl,Zp,Cf,Cf,Cf,Cf,Cf,Zs,};
static const unsigned char    UCD_GC_2030_203f[16]= {Po,Po,Po,Po,Po,Po,Po,Po,Po,Pi,Pf,Po,Po,Po,Po,Pc,};
static const unsigned char    UCD_GC_2040_204f[16]= {Pc,Po,Po,Po,Sm,Ps,Pe,Po,Po,Po,Po,Po,Po,Po,Po,Po,};
static const unsigned char    UCD_GC_2050_205f[16]= {Po,Po,Sm,Po,Pc,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Zs,};
static const unsigned char    UCD_GC_2060_206f[16]= {Cf,Cf,Cf,Cf,Cf,Cn,Cn,Cn,Cn,Cn,Cf,Cf,Cf,Cf,Cf,Cf,};
static const unsigned char    UCD_GC_2070_207f[16]= {No,Lm,Cn,Cn,No,No,No,No,No,No,Sm,Sm,Sm,Ps,Pe,Lm,};
static const unsigned char    UCD_GC_2080_208f[16]= {No,No,No,No,No,No,No,No,No,No,Sm,Sm,Sm,Ps,Pe,Cn,};
static const unsigned char    UCD_GC_2090_209f[16]= {Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_20a0_20af[16]= {Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,};
static const unsigned char    UCD_GC_20b0_20bf[16]= {Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Sc,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_20d0_20df[16]= {Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Me,Me,Me,};
static const unsigned char    UCD_GC_20e0_20ef[16]= {Me,Mn,Me,Me,Me,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_20f0_20ff[16]= {Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_2000_20ff[16]= {
    UCD_GC_2000_200f,
    UCD_GC_2010_201f,
    UCD_GC_2020_202f,
    UCD_GC_2030_203f,
    UCD_GC_2040_204f,
    UCD_GC_2050_205f,
    UCD_GC_2060_206f,
    UCD_GC_2070_207f,
    UCD_GC_2080_208f,
    UCD_GC_2090_209f,
    UCD_GC_20a0_20af,
    UCD_GC_20b0_20bf,
    UCD_GC_0860_086f, /*  UCD_GC_20c0_20cf  */
    UCD_GC_20d0_20df,
    UCD_GC_20e0_20ef,
    UCD_GC_20f0_20ff,
};
static const unsigned char    UCD_GC_2100_210f[16]= {So,So,Lu,So,So,So,So,Lu,So,So,Ll,Lu,Lu,Lu,Ll,Ll,};
static const unsigned char    UCD_GC_2110_211f[16]= {Lu,Lu,Lu,Ll,So,Lu,So,So,Sm,Lu,Lu,Lu,Lu,Lu,So,So,};
static const unsigned char    UCD_GC_2120_212f[16]= {So,So,So,So,Lu,So,Lu,So,Lu,So,Lu,Lu,Lu,Lu,So,Ll,};
static const unsigned char    UCD_GC_2130_213f[16]= {Lu,Lu,Lu,Lu,Ll,Lo,Lo,Lo,Lo,Ll,So,So,Ll,Ll,Lu,Lu,};
static const unsigned char    UCD_GC_2140_214f[16]= {Sm,Sm,Sm,Sm,Sm,Lu,Ll,Ll,Ll,Ll,So,Sm,So,So,Ll,So,};
static const unsigned char    UCD_GC_2150_215f[16]= {No,No,No,No,No,No,No,No,No,No,No,No,No,No,No,No,};
static const unsigned char    UCD_GC_2160_216f[16]= {Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,};
static const unsigned char    UCD_GC_2180_218f[16]= {Nl,Nl,Nl,Lu,Ll,Nl,Nl,Nl,Nl,No,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_2190_219f[16]= {Sm,Sm,Sm,Sm,Sm,So,So,So,So,So,Sm,Sm,So,So,So,So,};
static const unsigned char    UCD_GC_21a0_21af[16]= {Sm,So,So,Sm,So,So,Sm,So,So,So,So,So,So,So,Sm,So,};
static const unsigned char    UCD_GC_21c0_21cf[16]= {So,So,So,So,So,So,So,So,So,So,So,So,So,So,Sm,Sm,};
static const unsigned char    UCD_GC_21d0_21df[16]= {So,So,Sm,So,Sm,So,So,So,So,So,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_21f0_21ff[16]= {So,So,So,So,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,};
static const unsigned char * const UCD_GC_2100_21ff[16]= {
    UCD_GC_2100_210f,
    UCD_GC_2110_211f,
    UCD_GC_2120_212f,
    UCD_GC_2130_213f,
    UCD_GC_2140_214f,
    UCD_GC_2150_215f,
    UCD_GC_2160_216f,
    UCD_GC_2160_216f, /*  UCD_GC_2170_217f  */
    UCD_GC_2180_218f,
    UCD_GC_2190_219f,
    UCD_GC_21a0_21af,
    UCD_GC_19e0_19ef, /*  UCD_GC_21b0_21bf  */
    UCD_GC_21c0_21cf,
    UCD_GC_21d0_21df,
    UCD_GC_19e0_19ef, /*  UCD_GC_21e0_21ef  */
    UCD_GC_21f0_21ff,
};
static const unsigned char    UCD_GC_2200_220f[16]= {Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,};
static const unsigned char * const UCD_GC_2200_22ff[16]= {
    UCD_GC_2200_220f,
    UCD_GC_2200_220f, /*  UCD_GC_2210_221f  */
    UCD_GC_2200_220f, /*  UCD_GC_2220_222f  */
    UCD_GC_2200_220f, /*  UCD_GC_2230_223f  */
    UCD_GC_2200_220f, /*  UCD_GC_2240_224f  */
    UCD_GC_2200_220f, /*  UCD_GC_2250_225f  */
    UCD_GC_2200_220f, /*  UCD_GC_2260_226f  */
    UCD_GC_2200_220f, /*  UCD_GC_2270_227f  */
    UCD_GC_2200_220f, /*  UCD_GC_2280_228f  */
    UCD_GC_2200_220f, /*  UCD_GC_2290_229f  */
    UCD_GC_2200_220f, /*  UCD_GC_22a0_22af  */
    UCD_GC_2200_220f, /*  UCD_GC_22b0_22bf  */
    UCD_GC_2200_220f, /*  UCD_GC_22c0_22cf  */
    UCD_GC_2200_220f, /*  UCD_GC_22d0_22df  */
    UCD_GC_2200_220f, /*  UCD_GC_22e0_22ef  */
    UCD_GC_2200_220f, /*  UCD_GC_22f0_22ff  */
};
static const unsigned char    UCD_GC_2300_230f[16]= {So,So,So,So,So,So,So,So,Sm,Sm,Sm,Sm,So,So,So,So,};
static const unsigned char    UCD_GC_2320_232f[16]= {Sm,Sm,So,So,So,So,So,So,So,Ps,Pe,So,So,So,So,So,};
static const unsigned char    UCD_GC_2370_237f[16]= {So,So,So,So,So,So,So,So,So,So,So,So,Sm,So,So,So,};
static const unsigned char    UCD_GC_2390_239f[16]= {So,So,So,So,So,So,So,So,So,So,So,Sm,Sm,Sm,Sm,Sm,};
static const unsigned char    UCD_GC_23b0_23bf[16]= {Sm,Sm,Sm,Sm,So,So,So,So,So,So,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_23d0_23df[16]= {So,So,So,So,So,So,So,So,So,So,So,So,Sm,Sm,Sm,Sm,};
static const unsigned char    UCD_GC_23e0_23ef[16]= {Sm,Sm,So,So,So,So,So,So,So,So,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_23f0_23ff[16]= {So,So,So,So,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_2300_23ff[16]= {
    UCD_GC_2300_230f,
    UCD_GC_19e0_19ef, /*  UCD_GC_2310_231f  */
    UCD_GC_2320_232f,
    UCD_GC_19e0_19ef, /*  UCD_GC_2330_233f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2340_234f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2350_235f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2360_236f  */
    UCD_GC_2370_237f,
    UCD_GC_19e0_19ef, /*  UCD_GC_2380_238f  */
    UCD_GC_2390_239f,
    UCD_GC_2200_220f, /*  UCD_GC_23a0_23af  */
    UCD_GC_23b0_23bf,
    UCD_GC_19e0_19ef, /*  UCD_GC_23c0_23cf  */
    UCD_GC_23d0_23df,
    UCD_GC_23e0_23ef,
    UCD_GC_23f0_23ff,
};
static const unsigned char    UCD_GC_2420_242f[16]= {So,So,So,So,So,So,So,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_2440_244f[16]= {So,So,So,So,So,So,So,So,So,So,So,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_2490_249f[16]= {No,No,No,No,No,No,No,No,No,No,No,No,So,So,So,So,};
static const unsigned char    UCD_GC_24e0_24ef[16]= {So,So,So,So,So,So,So,So,So,So,No,No,No,No,No,No,};
static const unsigned char * const UCD_GC_2400_24ff[16]= {
    UCD_GC_19e0_19ef, /*  UCD_GC_2400_240f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2410_241f  */
    UCD_GC_2420_242f,
    UCD_GC_0860_086f, /*  UCD_GC_2430_243f  */
    UCD_GC_2440_244f,
    UCD_GC_0860_086f, /*  UCD_GC_2450_245f  */
    UCD_GC_2150_215f, /*  UCD_GC_2460_246f  */
    UCD_GC_2150_215f, /*  UCD_GC_2470_247f  */
    UCD_GC_2150_215f, /*  UCD_GC_2480_248f  */
    UCD_GC_2490_249f,
    UCD_GC_19e0_19ef, /*  UCD_GC_24a0_24af  */
    UCD_GC_19e0_19ef, /*  UCD_GC_24b0_24bf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_24c0_24cf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_24d0_24df  */
    UCD_GC_24e0_24ef,
    UCD_GC_2150_215f, /*  UCD_GC_24f0_24ff  */
};
static const unsigned char    UCD_GC_25b0_25bf[16]= {So,So,So,So,So,So,So,Sm,So,So,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_25c0_25cf[16]= {So,Sm,So,So,So,So,So,So,So,So,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_25f0_25ff[16]= {So,So,So,So,So,So,So,So,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,};
static const unsigned char * const UCD_GC_2500_25ff[16]= {
    UCD_GC_19e0_19ef, /*  UCD_GC_2500_250f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2510_251f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2520_252f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2530_253f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2540_254f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2550_255f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2560_256f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2570_257f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2580_258f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2590_259f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_25a0_25af  */
    UCD_GC_25b0_25bf,
    UCD_GC_25c0_25cf,
    UCD_GC_19e0_19ef, /*  UCD_GC_25d0_25df  */
    UCD_GC_19e0_19ef, /*  UCD_GC_25e0_25ef  */
    UCD_GC_25f0_25ff,
};
static const unsigned char    UCD_GC_2660_266f[16]= {So,So,So,So,So,So,So,So,So,So,So,So,So,So,So,Sm,};
static const unsigned char * const UCD_GC_2600_26ff[16]= {
    UCD_GC_19e0_19ef, /*  UCD_GC_2600_260f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2610_261f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2620_262f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2630_263f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2640_264f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2650_265f  */
    UCD_GC_2660_266f,
    UCD_GC_19e0_19ef, /*  UCD_GC_2670_267f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2680_268f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2690_269f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_26a0_26af  */
    UCD_GC_19e0_19ef, /*  UCD_GC_26b0_26bf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_26c0_26cf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_26d0_26df  */
    UCD_GC_19e0_19ef, /*  UCD_GC_26e0_26ef  */
    UCD_GC_19e0_19ef, /*  UCD_GC_26f0_26ff  */
};
static const unsigned char    UCD_GC_2700_270f[16]= {Cn,So,So,So,So,So,So,So,So,So,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_2760_276f[16]= {So,So,So,So,So,So,So,So,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,};
static const unsigned char    UCD_GC_2770_277f[16]= {Ps,Pe,Ps,Pe,Ps,Pe,No,No,No,No,No,No,No,No,No,No,};
static const unsigned char    UCD_GC_2790_279f[16]= {No,No,No,No,So,So,So,So,So,So,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_27c0_27cf[16]= {Sm,Sm,Sm,Sm,Sm,Ps,Pe,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,};
static const unsigned char    UCD_GC_27e0_27ef[16]= {Sm,Sm,Sm,Sm,Sm,Sm,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,};
static const unsigned char * const UCD_GC_2700_27ff[16]= {
    UCD_GC_2700_270f,
    UCD_GC_19e0_19ef, /*  UCD_GC_2710_271f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2720_272f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2730_273f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2740_274f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2750_275f  */
    UCD_GC_2760_276f,
    UCD_GC_2770_277f,
    UCD_GC_2150_215f, /*  UCD_GC_2780_278f  */
    UCD_GC_2790_279f,
    UCD_GC_19e0_19ef, /*  UCD_GC_27a0_27af  */
    UCD_GC_19e0_19ef, /*  UCD_GC_27b0_27bf  */
    UCD_GC_27c0_27cf,
    UCD_GC_2200_220f, /*  UCD_GC_27d0_27df  */
    UCD_GC_27e0_27ef,
    UCD_GC_2200_220f, /*  UCD_GC_27f0_27ff  */
};
static const unsigned char * const UCD_GC_2800_28ff[16]= {
    UCD_GC_19e0_19ef, /*  UCD_GC_2800_280f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2810_281f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2820_282f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2830_283f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2840_284f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2850_285f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2860_286f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2870_287f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2880_288f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2890_289f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_28a0_28af  */
    UCD_GC_19e0_19ef, /*  UCD_GC_28b0_28bf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_28c0_28cf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_28d0_28df  */
    UCD_GC_19e0_19ef, /*  UCD_GC_28e0_28ef  */
    UCD_GC_19e0_19ef, /*  UCD_GC_28f0_28ff  */
};
static const unsigned char    UCD_GC_2980_298f[16]= {Sm,Sm,Sm,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Ps,};
static const unsigned char    UCD_GC_2990_299f[16]= {Pe,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Sm,Sm,Sm,Sm,Sm,Sm,Sm,};
static const unsigned char    UCD_GC_29d0_29df[16]= {Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Ps,Pe,Ps,Pe,Sm,Sm,Sm,Sm,};
static const unsigned char    UCD_GC_29f0_29ff[16]= {Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Sm,Ps,Pe,Sm,Sm,};
static const unsigned char * const UCD_GC_2900_29ff[16]= {
    UCD_GC_2200_220f, /*  UCD_GC_2900_290f  */
    UCD_GC_2200_220f, /*  UCD_GC_2910_291f  */
    UCD_GC_2200_220f, /*  UCD_GC_2920_292f  */
    UCD_GC_2200_220f, /*  UCD_GC_2930_293f  */
    UCD_GC_2200_220f, /*  UCD_GC_2940_294f  */
    UCD_GC_2200_220f, /*  UCD_GC_2950_295f  */
    UCD_GC_2200_220f, /*  UCD_GC_2960_296f  */
    UCD_GC_2200_220f, /*  UCD_GC_2970_297f  */
    UCD_GC_2980_298f,
    UCD_GC_2990_299f,
    UCD_GC_2200_220f, /*  UCD_GC_29a0_29af  */
    UCD_GC_2200_220f, /*  UCD_GC_29b0_29bf  */
    UCD_GC_2200_220f, /*  UCD_GC_29c0_29cf  */
    UCD_GC_29d0_29df,
    UCD_GC_2200_220f, /*  UCD_GC_29e0_29ef  */
    UCD_GC_29f0_29ff,
};
static const unsigned char    UCD_GC_2b40_2b4f[16]= {Sm,Sm,Sm,Sm,Sm,So,So,Sm,Sm,Sm,Sm,Sm,Sm,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_2b00_2bff[16]= {
    UCD_GC_19e0_19ef, /*  UCD_GC_2b00_2b0f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2b10_2b1f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2b20_2b2f  */
    UCD_GC_2200_220f, /*  UCD_GC_2b30_2b3f  */
    UCD_GC_2b40_2b4f,
    UCD_GC_1390_139f, /*  UCD_GC_2b50_2b5f  */
    UCD_GC_0860_086f, /*  UCD_GC_2b60_2b6f  */
    UCD_GC_0860_086f, /*  UCD_GC_2b70_2b7f  */
    UCD_GC_0860_086f, /*  UCD_GC_2b80_2b8f  */
    UCD_GC_0860_086f, /*  UCD_GC_2b90_2b9f  */
    UCD_GC_0860_086f, /*  UCD_GC_2ba0_2baf  */
    UCD_GC_0860_086f, /*  UCD_GC_2bb0_2bbf  */
    UCD_GC_0860_086f, /*  UCD_GC_2bc0_2bcf  */
    UCD_GC_0860_086f, /*  UCD_GC_2bd0_2bdf  */
    UCD_GC_0860_086f, /*  UCD_GC_2be0_2bef  */
    UCD_GC_0860_086f, /*  UCD_GC_2bf0_2bff  */
};
static const unsigned char    UCD_GC_2c20_2c2f[16]= {Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Lu,Cn,};
static const unsigned char    UCD_GC_2c50_2c5f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Cn,};
static const unsigned char    UCD_GC_2c60_2c6f[16]= {Lu,Ll,Lu,Lu,Lu,Ll,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Lu,Lu,};
static const unsigned char    UCD_GC_2c70_2c7f[16]= {Lu,Ll,Lu,Ll,Ll,Lu,Ll,Ll,Ll,Ll,Ll,Ll,Lm,Lm,Lu,Lu,};
static const unsigned char    UCD_GC_2ce0_2cef[16]= {Lu,Ll,Lu,Ll,Ll,So,So,So,So,So,So,Lu,Ll,Lu,Ll,Mn,};
static const unsigned char    UCD_GC_2cf0_2cff[16]= {Mn,Mn,Lu,Ll,Cn,Cn,Cn,Cn,Cn,Po,Po,Po,Po,No,Po,Po,};
static const unsigned char * const UCD_GC_2c00_2cff[16]= {
    UCD_GC_00c0_00cf, /*  UCD_GC_2c00_2c0f  */
    UCD_GC_00c0_00cf, /*  UCD_GC_2c10_2c1f  */
    UCD_GC_2c20_2c2f,
    UCD_GC_00e0_00ef, /*  UCD_GC_2c30_2c3f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_2c40_2c4f  */
    UCD_GC_2c50_2c5f,
    UCD_GC_2c60_2c6f,
    UCD_GC_2c70_2c7f,
    UCD_GC_0100_010f, /*  UCD_GC_2c80_2c8f  */
    UCD_GC_0100_010f, /*  UCD_GC_2c90_2c9f  */
    UCD_GC_0100_010f, /*  UCD_GC_2ca0_2caf  */
    UCD_GC_0100_010f, /*  UCD_GC_2cb0_2cbf  */
    UCD_GC_0100_010f, /*  UCD_GC_2cc0_2ccf  */
    UCD_GC_0100_010f, /*  UCD_GC_2cd0_2cdf  */
    UCD_GC_2ce0_2cef,
    UCD_GC_2cf0_2cff,
};
static const unsigned char    UCD_GC_2d20_2d2f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Cn,Ll,Cn,Cn,Cn,Cn,Cn,Ll,Cn,Cn,};
static const unsigned char    UCD_GC_2d60_2d6f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Lm,};
static const unsigned char    UCD_GC_2d70_2d7f[16]= {Po,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Mn,};
static const unsigned char    UCD_GC_2d90_2d9f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_2da0_2daf[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,};
static const unsigned char * const UCD_GC_2d00_2dff[16]= {
    UCD_GC_00e0_00ef, /*  UCD_GC_2d00_2d0f  */
    UCD_GC_00e0_00ef, /*  UCD_GC_2d10_2d1f  */
    UCD_GC_2d20_2d2f,
    UCD_GC_05d0_05df, /*  UCD_GC_2d30_2d3f  */
    UCD_GC_05d0_05df, /*  UCD_GC_2d40_2d4f  */
    UCD_GC_05d0_05df, /*  UCD_GC_2d50_2d5f  */
    UCD_GC_2d60_2d6f,
    UCD_GC_2d70_2d7f,
    UCD_GC_05d0_05df, /*  UCD_GC_2d80_2d8f  */
    UCD_GC_2d90_2d9f,
    UCD_GC_2da0_2daf,
    UCD_GC_2da0_2daf, /*  UCD_GC_2db0_2dbf  */
    UCD_GC_2da0_2daf, /*  UCD_GC_2dc0_2dcf  */
    UCD_GC_2da0_2daf, /*  UCD_GC_2dd0_2ddf  */
    UCD_GC_0300_030f, /*  UCD_GC_2de0_2def  */
    UCD_GC_0300_030f, /*  UCD_GC_2df0_2dff  */
};
static const unsigned char    UCD_GC_2e00_2e0f[16]= {Po,Po,Pi,Pf,Pi,Pf,Po,Po,Po,Pi,Pf,Po,Pi,Pf,Po,Po,};
static const unsigned char    UCD_GC_2e10_2e1f[16]= {Po,Po,Po,Po,Po,Po,Po,Pd,Po,Po,Pd,Po,Pi,Pf,Po,Po,};
static const unsigned char    UCD_GC_2e20_2e2f[16]= {Pi,Pf,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Po,Po,Po,Po,Po,Lm,};
static const unsigned char    UCD_GC_2e30_2e3f[16]= {Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Pd,Pd,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_2e90_2e9f[16]= {So,So,So,So,So,So,So,So,So,So,Cn,So,So,So,So,So,};
static const unsigned char * const UCD_GC_2e00_2eff[16]= {
    UCD_GC_2e00_2e0f,
    UCD_GC_2e10_2e1f,
    UCD_GC_2e20_2e2f,
    UCD_GC_2e30_2e3f,
    UCD_GC_0860_086f, /*  UCD_GC_2e40_2e4f  */
    UCD_GC_0860_086f, /*  UCD_GC_2e50_2e5f  */
    UCD_GC_0860_086f, /*  UCD_GC_2e60_2e6f  */
    UCD_GC_0860_086f, /*  UCD_GC_2e70_2e7f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2e80_2e8f  */
    UCD_GC_2e90_2e9f,
    UCD_GC_19e0_19ef, /*  UCD_GC_2ea0_2eaf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2eb0_2ebf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2ec0_2ecf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2ed0_2edf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2ee0_2eef  */
    UCD_GC_23f0_23ff, /*  UCD_GC_2ef0_2eff  */
};
static const unsigned char    UCD_GC_2fd0_2fdf[16]= {So,So,So,So,So,So,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_2ff0_2fff[16]= {So,So,So,So,So,So,So,So,So,So,So,So,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_2f00_2fff[16]= {
    UCD_GC_19e0_19ef, /*  UCD_GC_2f00_2f0f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f10_2f1f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f20_2f2f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f30_2f3f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f40_2f4f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f50_2f5f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f60_2f6f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f70_2f7f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f80_2f8f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2f90_2f9f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2fa0_2faf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2fb0_2fbf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_2fc0_2fcf  */
    UCD_GC_2fd0_2fdf,
    UCD_GC_0860_086f, /*  UCD_GC_2fe0_2fef  */
    UCD_GC_2ff0_2fff,
};
static const unsigned char * const * const UCD_GC_2000_2fff[16]= {
    UCD_GC_2000_20ff,
    UCD_GC_2100_21ff,
    UCD_GC_2200_22ff,
    UCD_GC_2300_23ff,
    UCD_GC_2400_24ff,
    UCD_GC_2500_25ff,
    UCD_GC_2600_26ff,
    UCD_GC_2700_27ff,
    UCD_GC_2800_28ff,
    UCD_GC_2900_29ff,
    UCD_GC_2200_22ff, /*  UCD_GC_2a00_2aff  */
    UCD_GC_2b00_2bff,
    UCD_GC_2c00_2cff,
    UCD_GC_2d00_2dff,
    UCD_GC_2e00_2eff,
    UCD_GC_2f00_2fff,
};
static const unsigned char    UCD_GC_3000_300f[16]= {Zs,Po,Po,Po,So,Lm,Lo,Nl,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,};
static const unsigned char    UCD_GC_3010_301f[16]= {Ps,Pe,So,So,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Pd,Ps,Pe,Pe,};
static const unsigned char    UCD_GC_3020_302f[16]= {So,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Mn,Mn,Mn,Mn,Mc,Mc,};
static const unsigned char    UCD_GC_3030_303f[16]= {Pd,Lm,Lm,Lm,Lm,Lm,So,So,Nl,Nl,Nl,Lm,Lo,Po,So,So,};
static const unsigned char    UCD_GC_3090_309f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Mn,Mn,Sk,Sk,Lm,Lm,Lo,};
static const unsigned char    UCD_GC_30f0_30ff[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Po,Lm,Lm,Lm,Lo,};
static const unsigned char * const UCD_GC_3000_30ff[16]= {
    UCD_GC_3000_300f,
    UCD_GC_3010_301f,
    UCD_GC_3020_302f,
    UCD_GC_3030_303f,
    UCD_GC_0e00_0e0f, /*  UCD_GC_3040_304f  */
    UCD_GC_05d0_05df, /*  UCD_GC_3050_305f  */
    UCD_GC_05d0_05df, /*  UCD_GC_3060_306f  */
    UCD_GC_05d0_05df, /*  UCD_GC_3070_307f  */
    UCD_GC_05d0_05df, /*  UCD_GC_3080_308f  */
    UCD_GC_3090_309f,
    UCD_GC_1400_140f, /*  UCD_GC_30a0_30af  */
    UCD_GC_05d0_05df, /*  UCD_GC_30b0_30bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_30c0_30cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_30d0_30df  */
    UCD_GC_05d0_05df, /*  UCD_GC_30e0_30ef  */
    UCD_GC_30f0_30ff,
};
static const unsigned char    UCD_GC_3100_310f[16]= {Cn,Cn,Cn,Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_3180_318f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,};
static const unsigned char    UCD_GC_3190_319f[16]= {So,So,No,No,No,No,So,So,So,So,So,So,So,So,So,So,};
static const unsigned char * const UCD_GC_3100_31ff[16]= {
    UCD_GC_3100_310f,
    UCD_GC_05d0_05df, /*  UCD_GC_3110_311f  */
    UCD_GC_1960_196f, /*  UCD_GC_3120_312f  */
    UCD_GC_0e00_0e0f, /*  UCD_GC_3130_313f  */
    UCD_GC_05d0_05df, /*  UCD_GC_3140_314f  */
    UCD_GC_05d0_05df, /*  UCD_GC_3150_315f  */
    UCD_GC_05d0_05df, /*  UCD_GC_3160_316f  */
    UCD_GC_05d0_05df, /*  UCD_GC_3170_317f  */
    UCD_GC_3180_318f,
    UCD_GC_3190_319f,
    UCD_GC_05d0_05df, /*  UCD_GC_31a0_31af  */
    UCD_GC_05e0_05ef, /*  UCD_GC_31b0_31bf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_31c0_31cf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_31d0_31df  */
    UCD_GC_23f0_23ff, /*  UCD_GC_31e0_31ef  */
    UCD_GC_05d0_05df, /*  UCD_GC_31f0_31ff  */
};
static const unsigned char    UCD_GC_3210_321f[16]= {So,So,So,So,So,So,So,So,So,So,So,So,So,So,So,Cn,};
static const unsigned char    UCD_GC_3220_322f[16]= {No,No,No,No,No,No,No,No,No,No,So,So,So,So,So,So,};
static const unsigned char    UCD_GC_3240_324f[16]= {So,So,So,So,So,So,So,So,No,No,No,No,No,No,No,No,};
static const unsigned char    UCD_GC_3250_325f[16]= {So,No,No,No,No,No,No,No,No,No,No,No,No,No,No,No,};
static const unsigned char * const UCD_GC_3200_32ff[16]= {
    UCD_GC_19e0_19ef, /*  UCD_GC_3200_320f  */
    UCD_GC_3210_321f,
    UCD_GC_3220_322f,
    UCD_GC_19e0_19ef, /*  UCD_GC_3230_323f  */
    UCD_GC_3240_324f,
    UCD_GC_3250_325f,
    UCD_GC_19e0_19ef, /*  UCD_GC_3260_326f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_3270_327f  */
    UCD_GC_3220_322f, /*  UCD_GC_3280_328f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_3290_329f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_32a0_32af  */
    UCD_GC_3250_325f, /*  UCD_GC_32b0_32bf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_32c0_32cf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_32d0_32df  */
    UCD_GC_19e0_19ef, /*  UCD_GC_32e0_32ef  */
    UCD_GC_3210_321f, /*  UCD_GC_32f0_32ff  */
};
static const unsigned char * const * const UCD_GC_3000_3fff[16]= {
    UCD_GC_3000_30ff,
    UCD_GC_3100_31ff,
    UCD_GC_3200_32ff,
    UCD_GC_2800_28ff, /*  UCD_GC_3300_33ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3400_34ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3500_35ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3600_36ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3700_37ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3800_38ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3900_39ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3a00_3aff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3b00_3bff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3c00_3cff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3d00_3dff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3e00_3eff  */
    UCD_GC_1100_11ff, /*  UCD_GC_3f00_3fff  */
};
static const unsigned char * const UCD_GC_4d00_4dff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_4d00_4d0f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d10_4d1f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d20_4d2f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d30_4d3f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d40_4d4f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d50_4d5f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d60_4d6f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d70_4d7f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d80_4d8f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4d90_4d9f  */
    UCD_GC_05d0_05df, /*  UCD_GC_4da0_4daf  */
    UCD_GC_18f0_18ff, /*  UCD_GC_4db0_4dbf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_4dc0_4dcf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_4dd0_4ddf  */
    UCD_GC_19e0_19ef, /*  UCD_GC_4de0_4def  */
    UCD_GC_19e0_19ef, /*  UCD_GC_4df0_4dff  */
};
static const unsigned char * const * const UCD_GC_4000_4fff[16]= {
    UCD_GC_1100_11ff, /*  UCD_GC_4000_40ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4100_41ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4200_42ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4300_43ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4400_44ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4500_45ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4600_46ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4700_47ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4800_48ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4900_49ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4a00_4aff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4b00_4bff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4c00_4cff  */
    UCD_GC_4d00_4dff,
    UCD_GC_1100_11ff, /*  UCD_GC_4e00_4eff  */
    UCD_GC_1100_11ff, /*  UCD_GC_4f00_4fff  */
};
static const unsigned char * const * const UCD_GC_5000_5fff[16]= {
    UCD_GC_1100_11ff, /*  UCD_GC_5000_50ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5100_51ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5200_52ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5300_53ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5400_54ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5500_55ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5600_56ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5700_57ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5800_58ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5900_59ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5a00_5aff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5b00_5bff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5c00_5cff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5d00_5dff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5e00_5eff  */
    UCD_GC_1100_11ff, /*  UCD_GC_5f00_5fff  */
};
static const unsigned char * const UCD_GC_9f00_9fff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_9f00_9f0f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f10_9f1f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f20_9f2f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f30_9f3f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f40_9f4f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f50_9f5f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f60_9f6f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f70_9f7f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f80_9f8f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9f90_9f9f  */
    UCD_GC_05d0_05df, /*  UCD_GC_9fa0_9faf  */
    UCD_GC_05d0_05df, /*  UCD_GC_9fb0_9fbf  */
    UCD_GC_0f60_0f6f, /*  UCD_GC_9fc0_9fcf  */
    UCD_GC_0860_086f, /*  UCD_GC_9fd0_9fdf  */
    UCD_GC_0860_086f, /*  UCD_GC_9fe0_9fef  */
    UCD_GC_0860_086f, /*  UCD_GC_9ff0_9fff  */
};
static const unsigned char * const * const UCD_GC_9000_9fff[16]= {
    UCD_GC_1100_11ff, /*  UCD_GC_9000_90ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9100_91ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9200_92ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9300_93ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9400_94ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9500_95ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9600_96ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9700_97ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9800_98ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9900_99ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9a00_9aff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9b00_9bff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9c00_9cff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9d00_9dff  */
    UCD_GC_1100_11ff, /*  UCD_GC_9e00_9eff  */
    UCD_GC_9f00_9fff,
};
static const unsigned char    UCD_GC_a010_a01f[16]= {Lo,Lo,Lo,Lo,Lo,Lm,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char * const UCD_GC_a000_a0ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_a000_a00f  */
    UCD_GC_a010_a01f,
    UCD_GC_05d0_05df, /*  UCD_GC_a020_a02f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a030_a03f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a040_a04f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a050_a05f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a060_a06f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a070_a07f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a080_a08f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a090_a09f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a0a0_a0af  */
    UCD_GC_05d0_05df, /*  UCD_GC_a0b0_a0bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_a0c0_a0cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_a0d0_a0df  */
    UCD_GC_05d0_05df, /*  UCD_GC_a0e0_a0ef  */
    UCD_GC_05d0_05df, /*  UCD_GC_a0f0_a0ff  */
};
static const unsigned char * const UCD_GC_a400_a4ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_a400_a40f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a410_a41f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a420_a42f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a430_a43f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a440_a44f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a450_a45f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a460_a46f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a470_a47f  */
    UCD_GC_0f60_0f6f, /*  UCD_GC_a480_a48f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_a490_a49f  */
    UCD_GC_19e0_19ef, /*  UCD_GC_a4a0_a4af  */
    UCD_GC_19e0_19ef, /*  UCD_GC_a4b0_a4bf  */
    UCD_GC_2420_242f, /*  UCD_GC_a4c0_a4cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_a4d0_a4df  */
    UCD_GC_05d0_05df, /*  UCD_GC_a4e0_a4ef  */
    UCD_GC_1c70_1c7f, /*  UCD_GC_a4f0_a4ff  */
};
static const unsigned char    UCD_GC_a600_a60f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lm,Po,Po,Po,};
static const unsigned char    UCD_GC_a620_a62f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Lo,Lo,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_a660_a66f[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lo,Mn,};
static const unsigned char    UCD_GC_a670_a67f[16]= {Me,Me,Me,Po,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Po,Lm,};
static const unsigned char    UCD_GC_a690_a69f[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Mn,};
static const unsigned char    UCD_GC_a6e0_a6ef[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,};
static const unsigned char    UCD_GC_a6f0_a6ff[16]= {Mn,Mn,Po,Po,Po,Po,Po,Po,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_a600_a6ff[16]= {
    UCD_GC_a600_a60f,
    UCD_GC_05d0_05df, /*  UCD_GC_a610_a61f  */
    UCD_GC_a620_a62f,
    UCD_GC_0860_086f, /*  UCD_GC_a630_a63f  */
    UCD_GC_0100_010f, /*  UCD_GC_a640_a64f  */
    UCD_GC_0100_010f, /*  UCD_GC_a650_a65f  */
    UCD_GC_a660_a66f,
    UCD_GC_a670_a67f,
    UCD_GC_0100_010f, /*  UCD_GC_a680_a68f  */
    UCD_GC_a690_a69f,
    UCD_GC_05d0_05df, /*  UCD_GC_a6a0_a6af  */
    UCD_GC_05d0_05df, /*  UCD_GC_a6b0_a6bf  */
    UCD_GC_05d0_05df, /*  UCD_GC_a6c0_a6cf  */
    UCD_GC_05d0_05df, /*  UCD_GC_a6d0_a6df  */
    UCD_GC_a6e0_a6ef,
    UCD_GC_a6f0_a6ff,
};
static const unsigned char    UCD_GC_a710_a71f[16]= {Sk,Sk,Sk,Sk,Sk,Sk,Sk,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,Lm,};
static const unsigned char    UCD_GC_a720_a72f[16]= {Sk,Sk,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,};
static const unsigned char    UCD_GC_a730_a73f[16]= {Ll,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,};
static const unsigned char    UCD_GC_a770_a77f[16]= {Lm,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Lu,Ll,Lu,Ll,Lu,Lu,Ll,};
static const unsigned char    UCD_GC_a780_a78f[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lm,Sk,Sk,Lu,Ll,Lu,Ll,Cn,};
static const unsigned char    UCD_GC_a790_a79f[16]= {Lu,Ll,Lu,Ll,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_a7a0_a7af[16]= {Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Ll,Lu,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_a7f0_a7ff[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Lm,Lm,Ll,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char * const UCD_GC_a700_a7ff[16]= {
    UCD_GC_02f0_02ff, /*  UCD_GC_a700_a70f  */
    UCD_GC_a710_a71f,
    UCD_GC_a720_a72f,
    UCD_GC_a730_a73f,
    UCD_GC_0100_010f, /*  UCD_GC_a740_a74f  */
    UCD_GC_0100_010f, /*  UCD_GC_a750_a75f  */
    UCD_GC_0100_010f, /*  UCD_GC_a760_a76f  */
    UCD_GC_a770_a77f,
    UCD_GC_a780_a78f,
    UCD_GC_a790_a79f,
    UCD_GC_a7a0_a7af,
    UCD_GC_0860_086f, /*  UCD_GC_a7b0_a7bf  */
    UCD_GC_0860_086f, /*  UCD_GC_a7c0_a7cf  */
    UCD_GC_0860_086f, /*  UCD_GC_a7d0_a7df  */
    UCD_GC_0860_086f, /*  UCD_GC_a7e0_a7ef  */
    UCD_GC_a7f0_a7ff,
};
static const unsigned char    UCD_GC_a800_a80f[16]= {Lo,Lo,Mn,Lo,Lo,Lo,Mn,Lo,Lo,Lo,Lo,Mn,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_a820_a82f[16]= {Lo,Lo,Lo,Mc,Mc,Mn,Mn,Mc,So,So,So,So,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_a830_a83f[16]= {No,No,No,No,No,No,So,So,Sc,So,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_a870_a87f[16]= {Lo,Lo,Lo,Lo,Po,Po,Po,Po,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_a880_a88f[16]= {Mc,Mc,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_a8b0_a8bf[16]= {Lo,Lo,Lo,Lo,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,Mc,};
static const unsigned char    UCD_GC_a8c0_a8cf[16]= {Mc,Mc,Mc,Mc,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Po,Po,};
static const unsigned char    UCD_GC_a8f0_a8ff[16]= {Mn,Mn,Lo,Lo,Lo,Lo,Lo,Lo,Po,Po,Po,Lo,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_a800_a8ff[16]= {
    UCD_GC_a800_a80f,
    UCD_GC_05d0_05df, /*  UCD_GC_a810_a81f  */
    UCD_GC_a820_a82f,
    UCD_GC_a830_a83f,
    UCD_GC_05d0_05df, /*  UCD_GC_a840_a84f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a850_a85f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a860_a86f  */
    UCD_GC_a870_a87f,
    UCD_GC_a880_a88f,
    UCD_GC_05d0_05df, /*  UCD_GC_a890_a89f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a8a0_a8af  */
    UCD_GC_a8b0_a8bf,
    UCD_GC_a8c0_a8cf,
    UCD_GC_17e0_17ef, /*  UCD_GC_a8d0_a8df  */
    UCD_GC_0300_030f, /*  UCD_GC_a8e0_a8ef  */
    UCD_GC_a8f0_a8ff,
};
static const unsigned char    UCD_GC_a920_a92f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Po,Po,};
static const unsigned char    UCD_GC_a940_a94f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,Mn,};
static const unsigned char    UCD_GC_a950_a95f[16]= {Mn,Mn,Mc,Mc,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Po,};
static const unsigned char    UCD_GC_a9b0_a9bf[16]= {Lo,Lo,Lo,Mn,Mc,Mc,Mn,Mn,Mn,Mn,Mc,Mc,Mn,Mc,Mc,Mc,};
static const unsigned char    UCD_GC_a9c0_a9cf[16]= {Mc,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Po,Cn,Lm,};
static const unsigned char    UCD_GC_a9d0_a9df[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Cn,Cn,Cn,Cn,Po,Po,};
static const unsigned char * const UCD_GC_a900_a9ff[16]= {
    UCD_GC_07c0_07cf, /*  UCD_GC_a900_a90f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a910_a91f  */
    UCD_GC_a920_a92f,
    UCD_GC_05d0_05df, /*  UCD_GC_a930_a93f  */
    UCD_GC_a940_a94f,
    UCD_GC_a950_a95f,
    UCD_GC_05d0_05df, /*  UCD_GC_a960_a96f  */
    UCD_GC_0f60_0f6f, /*  UCD_GC_a970_a97f  */
    UCD_GC_0900_090f, /*  UCD_GC_a980_a98f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a990_a99f  */
    UCD_GC_05d0_05df, /*  UCD_GC_a9a0_a9af  */
    UCD_GC_a9b0_a9bf,
    UCD_GC_a9c0_a9cf,
    UCD_GC_a9d0_a9df,
    UCD_GC_0860_086f, /*  UCD_GC_a9e0_a9ef  */
    UCD_GC_0860_086f, /*  UCD_GC_a9f0_a9ff  */
};
static const unsigned char    UCD_GC_aa20_aa2f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mn,Mn,Mn,Mn,Mn,Mc,};
static const unsigned char    UCD_GC_aa30_aa3f[16]= {Mc,Mn,Mn,Mc,Mc,Mn,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_aa40_aa4f[16]= {Lo,Lo,Lo,Mn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mn,Mc,Cn,Cn,};
static const unsigned char    UCD_GC_aa50_aa5f[16]= {Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Nd,Cn,Cn,Po,Po,Po,Po,};
static const unsigned char    UCD_GC_aa70_aa7f[16]= {Lm,Lo,Lo,Lo,Lo,Lo,Lo,So,So,So,Lo,Mc,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_aab0_aabf[16]= {Mn,Lo,Mn,Mn,Mn,Lo,Lo,Mn,Mn,Lo,Lo,Lo,Lo,Lo,Mn,Mn,};
static const unsigned char    UCD_GC_aac0_aacf[16]= {Lo,Mn,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_aad0_aadf[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Lo,Lo,Lm,Po,Po,};
static const unsigned char    UCD_GC_aae0_aaef[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Mc,Mn,Mn,Mc,Mc,};
static const unsigned char    UCD_GC_aaf0_aaff[16]= {Po,Po,Lo,Lm,Lm,Mc,Mn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_aa00_aaff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_aa00_aa0f  */
    UCD_GC_05d0_05df, /*  UCD_GC_aa10_aa1f  */
    UCD_GC_aa20_aa2f,
    UCD_GC_aa30_aa3f,
    UCD_GC_aa40_aa4f,
    UCD_GC_aa50_aa5f,
    UCD_GC_05d0_05df, /*  UCD_GC_aa60_aa6f  */
    UCD_GC_aa70_aa7f,
    UCD_GC_05d0_05df, /*  UCD_GC_aa80_aa8f  */
    UCD_GC_05d0_05df, /*  UCD_GC_aa90_aa9f  */
    UCD_GC_05d0_05df, /*  UCD_GC_aaa0_aaaf  */
    UCD_GC_aab0_aabf,
    UCD_GC_aac0_aacf,
    UCD_GC_aad0_aadf,
    UCD_GC_aae0_aaef,
    UCD_GC_aaf0_aaff,
};
static const unsigned char    UCD_GC_ab00_ab0f[16]= {Cn,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Cn,};
static const unsigned char    UCD_GC_ab10_ab1f[16]= {Cn,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_abe0_abef[16]= {Lo,Lo,Lo,Mc,Mc,Mn,Mc,Mc,Mn,Mc,Mc,Po,Mc,Mn,Cn,Cn,};
static const unsigned char * const UCD_GC_ab00_abff[16]= {
    UCD_GC_ab00_ab0f,
    UCD_GC_ab10_ab1f,
    UCD_GC_2da0_2daf, /*  UCD_GC_ab20_ab2f  */
    UCD_GC_0860_086f, /*  UCD_GC_ab30_ab3f  */
    UCD_GC_0860_086f, /*  UCD_GC_ab40_ab4f  */
    UCD_GC_0860_086f, /*  UCD_GC_ab50_ab5f  */
    UCD_GC_0860_086f, /*  UCD_GC_ab60_ab6f  */
    UCD_GC_0860_086f, /*  UCD_GC_ab70_ab7f  */
    UCD_GC_0860_086f, /*  UCD_GC_ab80_ab8f  */
    UCD_GC_0860_086f, /*  UCD_GC_ab90_ab9f  */
    UCD_GC_0860_086f, /*  UCD_GC_aba0_abaf  */
    UCD_GC_0860_086f, /*  UCD_GC_abb0_abbf  */
    UCD_GC_05d0_05df, /*  UCD_GC_abc0_abcf  */
    UCD_GC_05d0_05df, /*  UCD_GC_abd0_abdf  */
    UCD_GC_abe0_abef,
    UCD_GC_17e0_17ef, /*  UCD_GC_abf0_abff  */
};
static const unsigned char * const * const UCD_GC_a000_afff[16]= {
    UCD_GC_a000_a0ff,
    UCD_GC_1100_11ff, /*  UCD_GC_a100_a1ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_a200_a2ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_a300_a3ff  */
    UCD_GC_a400_a4ff,
    UCD_GC_1100_11ff, /*  UCD_GC_a500_a5ff  */
    UCD_GC_a600_a6ff,
    UCD_GC_a700_a7ff,
    UCD_GC_a800_a8ff,
    UCD_GC_a900_a9ff,
    UCD_GC_aa00_aaff,
    UCD_GC_ab00_abff,
    UCD_GC_1100_11ff, /*  UCD_GC_ac00_acff  */
    UCD_GC_1100_11ff, /*  UCD_GC_ad00_adff  */
    UCD_GC_1100_11ff, /*  UCD_GC_ae00_aeff  */
    UCD_GC_1100_11ff, /*  UCD_GC_af00_afff  */
};
static const unsigned char    UCD_GC_d7a0_d7af[16]= {Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_d7c0_d7cf[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char * const UCD_GC_d700_d7ff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_d700_d70f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d710_d71f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d720_d72f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d730_d73f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d740_d74f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d750_d75f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d760_d76f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d770_d77f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d780_d78f  */
    UCD_GC_05d0_05df, /*  UCD_GC_d790_d79f  */
    UCD_GC_d7a0_d7af,
    UCD_GC_05d0_05df, /*  UCD_GC_d7b0_d7bf  */
    UCD_GC_d7c0_d7cf,
    UCD_GC_05d0_05df, /*  UCD_GC_d7d0_d7df  */
    UCD_GC_05d0_05df, /*  UCD_GC_d7e0_d7ef  */
    UCD_GC_19a0_19af, /*  UCD_GC_d7f0_d7ff  */
};
static const unsigned char    UCD_GC_d800_d80f[16]= {Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,Cs,};
static const unsigned char * const UCD_GC_d800_d8ff[16]= {
    UCD_GC_d800_d80f,
    UCD_GC_d800_d80f, /*  UCD_GC_d810_d81f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d820_d82f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d830_d83f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d840_d84f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d850_d85f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d860_d86f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d870_d87f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d880_d88f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d890_d89f  */
    UCD_GC_d800_d80f, /*  UCD_GC_d8a0_d8af  */
    UCD_GC_d800_d80f, /*  UCD_GC_d8b0_d8bf  */
    UCD_GC_d800_d80f, /*  UCD_GC_d8c0_d8cf  */
    UCD_GC_d800_d80f, /*  UCD_GC_d8d0_d8df  */
    UCD_GC_d800_d80f, /*  UCD_GC_d8e0_d8ef  */
    UCD_GC_d800_d80f, /*  UCD_GC_d8f0_d8ff  */
};
static const unsigned char * const * const UCD_GC_d000_dfff[16]= {
    UCD_GC_1100_11ff, /*  UCD_GC_d000_d0ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_d100_d1ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_d200_d2ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_d300_d3ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_d400_d4ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_d500_d5ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_d600_d6ff  */
    UCD_GC_d700_d7ff,
    UCD_GC_d800_d8ff,
    UCD_GC_d800_d8ff, /*  UCD_GC_d900_d9ff  */
    UCD_GC_d800_d8ff, /*  UCD_GC_da00_daff  */
    UCD_GC_d800_d8ff, /*  UCD_GC_db00_dbff  */
    UCD_GC_d800_d8ff, /*  UCD_GC_dc00_dcff  */
    UCD_GC_d800_d8ff, /*  UCD_GC_dd00_ddff  */
    UCD_GC_d800_d8ff, /*  UCD_GC_de00_deff  */
    UCD_GC_d800_d8ff, /*  UCD_GC_df00_dfff  */
};
static const unsigned char    UCD_GC_e000_e00f[16]= {Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,};
static const unsigned char * const UCD_GC_e000_e0ff[16]= {
    UCD_GC_e000_e00f,
    UCD_GC_e000_e00f, /*  UCD_GC_e010_e01f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e020_e02f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e030_e03f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e040_e04f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e050_e05f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e060_e06f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e070_e07f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e080_e08f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e090_e09f  */
    UCD_GC_e000_e00f, /*  UCD_GC_e0a0_e0af  */
    UCD_GC_e000_e00f, /*  UCD_GC_e0b0_e0bf  */
    UCD_GC_e000_e00f, /*  UCD_GC_e0c0_e0cf  */
    UCD_GC_e000_e00f, /*  UCD_GC_e0d0_e0df  */
    UCD_GC_e000_e00f, /*  UCD_GC_e0e0_e0ef  */
    UCD_GC_e000_e00f, /*  UCD_GC_e0f0_e0ff  */
};
static const unsigned char * const * const UCD_GC_e000_efff[16]= {
    UCD_GC_e000_e0ff,
    UCD_GC_e000_e0ff, /*  UCD_GC_e100_e1ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_e200_e2ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_e300_e3ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_e400_e4ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_e500_e5ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_e600_e6ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_e700_e7ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_e800_e8ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_e900_e9ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_ea00_eaff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_eb00_ebff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_ec00_ecff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_ed00_edff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_ee00_eeff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_ef00_efff  */
};
static const unsigned char    UCD_GC_fad0_fadf[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char * const UCD_GC_fa00_faff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_fa00_fa0f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fa10_fa1f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fa20_fa2f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fa30_fa3f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fa40_fa4f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fa50_fa5f  */
    UCD_GC_1960_196f, /*  UCD_GC_fa60_fa6f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fa70_fa7f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fa80_fa8f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fa90_fa9f  */
    UCD_GC_05d0_05df, /*  UCD_GC_faa0_faaf  */
    UCD_GC_05d0_05df, /*  UCD_GC_fab0_fabf  */
    UCD_GC_05d0_05df, /*  UCD_GC_fac0_facf  */
    UCD_GC_fad0_fadf,
    UCD_GC_0860_086f, /*  UCD_GC_fae0_faef  */
    UCD_GC_0860_086f, /*  UCD_GC_faf0_faff  */
};
static const unsigned char    UCD_GC_fb00_fb0f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_fb10_fb1f[16]= {Cn,Cn,Cn,Ll,Ll,Ll,Ll,Ll,Cn,Cn,Cn,Cn,Cn,Lo,Mn,Lo,};
static const unsigned char    UCD_GC_fb20_fb2f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Sm,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_fb30_fb3f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Cn,Lo,Cn,};
static const unsigned char    UCD_GC_fb40_fb4f[16]= {Lo,Lo,Cn,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_fbb0_fbbf[16]= {Lo,Lo,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,Sk,};
static const unsigned char    UCD_GC_fbc0_fbcf[16]= {Sk,Sk,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_fbd0_fbdf[16]= {Cn,Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char * const UCD_GC_fb00_fbff[16]= {
    UCD_GC_fb00_fb0f,
    UCD_GC_fb10_fb1f,
    UCD_GC_fb20_fb2f,
    UCD_GC_fb30_fb3f,
    UCD_GC_fb40_fb4f,
    UCD_GC_05d0_05df, /*  UCD_GC_fb50_fb5f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fb60_fb6f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fb70_fb7f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fb80_fb8f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fb90_fb9f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fba0_fbaf  */
    UCD_GC_fbb0_fbbf,
    UCD_GC_fbc0_fbcf,
    UCD_GC_fbd0_fbdf,
    UCD_GC_05d0_05df, /*  UCD_GC_fbe0_fbef  */
    UCD_GC_05d0_05df, /*  UCD_GC_fbf0_fbff  */
};
static const unsigned char    UCD_GC_fd30_fd3f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Ps,Pe,};
static const unsigned char    UCD_GC_fd90_fd9f[16]= {Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_fdf0_fdff[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Sc,So,Cn,Cn,};
static const unsigned char * const UCD_GC_fd00_fdff[16]= {
    UCD_GC_05d0_05df, /*  UCD_GC_fd00_fd0f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fd10_fd1f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fd20_fd2f  */
    UCD_GC_fd30_fd3f,
    UCD_GC_0860_086f, /*  UCD_GC_fd40_fd4f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fd50_fd5f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fd60_fd6f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fd70_fd7f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fd80_fd8f  */
    UCD_GC_fd90_fd9f,
    UCD_GC_05d0_05df, /*  UCD_GC_fda0_fdaf  */
    UCD_GC_05d0_05df, /*  UCD_GC_fdb0_fdbf  */
    UCD_GC_1870_187f, /*  UCD_GC_fdc0_fdcf  */
    UCD_GC_0860_086f, /*  UCD_GC_fdd0_fddf  */
    UCD_GC_0860_086f, /*  UCD_GC_fde0_fdef  */
    UCD_GC_fdf0_fdff,
};
static const unsigned char    UCD_GC_fe10_fe1f[16]= {Po,Po,Po,Po,Po,Po,Po,Ps,Pe,Po,Cn,Cn,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_fe30_fe3f[16]= {Po,Pd,Pd,Pc,Pc,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Ps,Pe,Ps,};
static const unsigned char    UCD_GC_fe40_fe4f[16]= {Pe,Ps,Pe,Ps,Pe,Po,Po,Ps,Pe,Po,Po,Po,Po,Pc,Pc,Pc,};
static const unsigned char    UCD_GC_fe50_fe5f[16]= {Po,Po,Po,Cn,Po,Po,Po,Po,Pd,Ps,Pe,Ps,Pe,Ps,Pe,Po,};
static const unsigned char    UCD_GC_fe60_fe6f[16]= {Po,Po,Sm,Pd,Sm,Sm,Sm,Cn,Po,Sc,Po,Po,Cn,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_fe70_fe7f[16]= {Lo,Lo,Lo,Lo,Lo,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_fef0_feff[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Cf,};
static const unsigned char * const UCD_GC_fe00_feff[16]= {
    UCD_GC_0300_030f, /*  UCD_GC_fe00_fe0f  */
    UCD_GC_fe10_fe1f,
    UCD_GC_1de0_1def, /*  UCD_GC_fe20_fe2f  */
    UCD_GC_fe30_fe3f,
    UCD_GC_fe40_fe4f,
    UCD_GC_fe50_fe5f,
    UCD_GC_fe60_fe6f,
    UCD_GC_fe70_fe7f,
    UCD_GC_05d0_05df, /*  UCD_GC_fe80_fe8f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fe90_fe9f  */
    UCD_GC_05d0_05df, /*  UCD_GC_fea0_feaf  */
    UCD_GC_05d0_05df, /*  UCD_GC_feb0_febf  */
    UCD_GC_05d0_05df, /*  UCD_GC_fec0_fecf  */
    UCD_GC_05d0_05df, /*  UCD_GC_fed0_fedf  */
    UCD_GC_05d0_05df, /*  UCD_GC_fee0_feef  */
    UCD_GC_fef0_feff,
};
static const unsigned char    UCD_GC_ff00_ff0f[16]= {Cn,Po,Po,Po,Sc,Po,Po,Po,Ps,Pe,Po,Sm,Po,Pd,Po,Po,};
static const unsigned char    UCD_GC_ff50_ff5f[16]= {Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ll,Ps,Sm,Pe,Sm,Ps,};
static const unsigned char    UCD_GC_ff60_ff6f[16]= {Pe,Po,Ps,Pe,Po,Po,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_ff70_ff7f[16]= {Lm,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_ff90_ff9f[16]= {Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lo,Lm,Lm,};
static const unsigned char    UCD_GC_ffc0_ffcf[16]= {Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,};
static const unsigned char    UCD_GC_ffd0_ffdf[16]= {Cn,Cn,Lo,Lo,Lo,Lo,Lo,Lo,Cn,Cn,Lo,Lo,Lo,Cn,Cn,Cn,};
static const unsigned char    UCD_GC_ffe0_ffef[16]= {Sc,Sc,Sm,Sk,So,Sc,Sc,Cn,So,Sm,Sm,Sm,Sm,So,So,Cn,};
static const unsigned char    UCD_GC_fff0_ffff[16]= {Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cn,Cf,Cf,Cf,So,So,Cn,Cn,};
static const unsigned char * const UCD_GC_ff00_ffff[16]= {
    UCD_GC_ff00_ff0f,
    UCD_GC_0030_003f, /*  UCD_GC_ff10_ff1f  */
    UCD_GC_0040_004f, /*  UCD_GC_ff20_ff2f  */
    UCD_GC_0050_005f, /*  UCD_GC_ff30_ff3f  */
    UCD_GC_0060_006f, /*  UCD_GC_ff40_ff4f  */
    UCD_GC_ff50_ff5f,
    UCD_GC_ff60_ff6f,
    UCD_GC_ff70_ff7f,
    UCD_GC_05d0_05df, /*  UCD_GC_ff80_ff8f  */
    UCD_GC_ff90_ff9f,
    UCD_GC_05d0_05df, /*  UCD_GC_ffa0_ffaf  */
    UCD_GC_3180_318f, /*  UCD_GC_ffb0_ffbf  */
    UCD_GC_ffc0_ffcf,
    UCD_GC_ffd0_ffdf,
    UCD_GC_ffe0_ffef,
    UCD_GC_fff0_ffff,
};
static const unsigned char * const * const UCD_GC_f000_ffff[16]= {
    UCD_GC_e000_e0ff, /*  UCD_GC_f000_f0ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_f100_f1ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_f200_f2ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_f300_f3ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_f400_f4ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_f500_f5ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_f600_f6ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_f700_f7ff  */
    UCD_GC_e000_e0ff, /*  UCD_GC_f800_f8ff  */
    UCD_GC_1100_11ff, /*  UCD_GC_f900_f9ff  */
    UCD_GC_fa00_faff,
    UCD_GC_fb00_fbff,
    UCD_GC_1100_11ff, /*  UCD_GC_fc00_fcff  */
    UCD_GC_fd00_fdff,
    UCD_GC_fe00_feff,
    UCD_GC_ff00_ffff,
};
static const unsigned char * const * const * const UCD_GC[16]={
    UCD_GC_0000_0fff,
    UCD_GC_1000_1fff,
    UCD_GC_2000_2fff,
    UCD_GC_3000_3fff,
    UCD_GC_4000_4fff,
    UCD_GC_5000_5fff,
    UCD_GC_5000_5fff, /*  UCD_GC_6000_6fff  */
    UCD_GC_5000_5fff, /*  UCD_GC_7000_7fff  */
    UCD_GC_5000_5fff, /*  UCD_GC_8000_8fff  */
    UCD_GC_9000_9fff,
    UCD_GC_a000_afff,
    UCD_GC_5000_5fff, /*  UCD_GC_b000_bfff  */
    UCD_GC_5000_5fff, /*  UCD_GC_c000_cfff  */
    UCD_GC_d000_dfff,
    UCD_GC_e000_efff,
    UCD_GC_f000_ffff,
};

int ucdGeneralCategory( int sym ) {
    return ucdSymbolClass( sym, UCD_GC, Cn );
}
const char * ucdGeneralCategoryStr( int sym ) {
    static char scratch[20];
    switch( sym )
	{
	case Co:	return "Co";
	case Lm:	return "Lm";
	case Pd:	return "Pd";
	case So:	return "So";
	case Nd:	return "Nd";
	case Cn:	return "Cn";
	case Zp:	return "Zp";
	case Po:	return "Po";
	case No:	return "No";
	case Mn:	return "Mn";
	case Cf:	return "Cf";
	case Zs:	return "Zs";
	case Pe:	return "Pe";
	case Pf:	return "Pf";
	case Ps:	return "Ps";
	case Cc:	return "Cc";
	case Lu:	return "Lu";
	case Mc:	return "Mc";
	case Sm:	return "Sm";
	case Cs:	return "Cs";
	case Lt:	return "Lt";
	case Pc:	return "Pc";
	case Nl:	return "Nl";
	case Lo:	return "Lo";
	case Pi:	return "Pi";
	case Me:	return "Me";
	case Ll:	return "Ll";
	case Zl:	return "Zl";
	case Sk:	return "Sk";
	case Sc:	return "Sc";
	default:
	    sprintf( scratch, "%d", sym );
	    return scratch;
	}
}
