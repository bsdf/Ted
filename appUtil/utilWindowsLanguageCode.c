/************************************************************************/
/*									*/
/*  Translation back and forth between windows language code and LANG	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"
#   include	<string.h>

#   include	"utilWindowsLanguageCode.h"

#   define	GENERATE	0

typedef struct LanguageCode
    {
    int			lcCode;
    const char *	lcLang;

    int			lcCodeIndex;
#   if GENERATE
    int			lcSource;
#   endif
    } LanguageCode;

/************************************************************************/
/*									*/
/*  Language Codes sorted by string.					*/
/*									*/
/*  The lcCodeIndex field is an inverse sort index for the Code value	*/
/*									*/
/************************************************************************/

# if GENERATE
static LanguageCode LanguageCodes[]=
# else
static const LanguageCode LanguageCodes[]=
# endif
{
    {    54, "af",		   4 }, /* 0x0036 */
    {  1078, "af-ZA",		  34 }, /* 0x0436 */
    {    94, "am",		  50 }, /* 0x005e */
    {  1118, "am-ET",		 384 }, /* 0x045e */
    {     1, "ar",		  55 }, /* 0x0001 */
    { 14337, "ar-AE",		  59 }, /* 0x3801 */
    { 15361, "ar-BH",		  61 }, /* 0x3c01 */
    {  5121, "ar-DZ",		  71 }, /* 0x1401 */
    {  3073, "ar-EG",		  73 }, /* 0x0c01 */
    {  2049, "ar-IQ",		  92 }, /* 0x0801 */
    { 11265, "ar-JO",		 120 }, /* 0x2c01 */
    { 13313, "ar-KW",		 126 }, /* 0x3401 */
    { 12289, "ar-LB",		 161 }, /* 0x3001 */
    {  4097, "ar-LY",		 169 }, /* 0x1001 */
    {  6145, "ar-MA",		 180 }, /* 0x1801 */
    {  8193, "ar-OM",		 182 }, /* 0x2001 */
    { 16385, "ar-QA",		 190 }, /* 0x4001 */
    {  1025, "ar-SA",		 202 }, /* 0x0401 */
    { 10241, "ar-SY",		 244 }, /* 0x2801 */
    {  7169, "ar-TN",		 249 }, /* 0x1c01 */
    {  9217, "ar-YE",		 260 }, /* 0x2401 */
    {   122, "arn",		 267 }, /* 0x007a */
    {  1146, "arn-CL",		 276 }, /* 0x047a */
    {    77, "as",		 278 }, /* 0x004d */
    {  1101, "as-IN",		 281 }, /* 0x044d */
    {    44, "az",		 165 }, /* 0x002c */
    { 29740, "az-Cyrl",		 298 }, /* 0x742c */
    {  2092, "az-Cyrl-AZ",	 313 }, /* 0x082c */
    { 30764, "az-Latn",		 327 }, /* 0x782c */
    {  1068, "az-Latn-AZ",	 341 }, /* 0x042c */
    {   109, "ba",		 351 }, /* 0x006d */
    {  1133, "ba-RU",		 363 }, /* 0x046d */
    {    35, "be",		 174 }, /* 0x0023 */
    {  1059, "be-BY",		 361 }, /* 0x0423 */
    {     2, "bg",		  32 }, /* 0x0002 */
    {  1026, "bg-BG",		 300 }, /* 0x0402 */
    {  1126, "bin-NG",		 114 }, /* 0x0466 */
    {    69, "bn",		 215 }, /* 0x0045 */
    {  2117, "bn-BD",		 213 }, /* 0x0845 */
    {  1093, "bn-IN",		 338 }, /* 0x0445 */
    {    81, "bo",		 118 }, /* 0x0051 */
    {  2129, "bo-BT",		 372 }, /* 0x0851 */
    {  1105, "bo-CN",		 171 }, /* 0x0451 */
    {   126, "br",		  25 }, /* 0x007e */
    {  1150, "br-FR",		 116 }, /* 0x047e */
    { 30746, "bs",		 168 }, /* 0x781a */
    { 25626, "bs-Cyrl",		 219 }, /* 0x641a */
    {  8218, "bs-Cyrl-BA",	 349 }, /* 0x201a */
    { 26650, "bs-Latn",		 377 }, /* 0x681a */
    {  5146, "bs-Latn-BA",	 389 }, /* 0x141a */
    {     3, "ca",		   0 }, /* 0x0003 */
    {  1027, "ca-ES",		 192 }, /* 0x0403 */
    {  1116, "chr-US",		 124 }, /* 0x045c */
    {   131, "co",		 163 }, /* 0x0083 */
    {  1155, "co-FR",		 236 }, /* 0x0483 */
    {     5, "cs",		 292 }, /* 0x0005 */
    {  1029, "cs-CZ",		 145 }, /* 0x0405 */
    {    82, "cy",		 233 }, /* 0x0052 */
    {  1106, "cy-GB",		 194 }, /* 0x0452 */
    {     6, "da",		 207 }, /* 0x0006 */
    {  1030, "da-DK",		 330 }, /* 0x0406 */
    {     7, "de",		 345 }, /* 0x0007 */
    {  3079, "de-AT",		 366 }, /* 0x0c07 */
    {  2055, "de-CH",		 354 }, /* 0x0807 */
    {  1031, "de-DE",		  37 }, /* 0x0407 */
    {  5127, "de-LI",		 256 }, /* 0x1407 */
    {  4103, "de-LU",		 155 }, /* 0x1007 */
    { 31790, "dsb",		 254 }, /* 0x7c2e */
    {  2094, "dsb-DE",		 334 }, /* 0x082e */
    {   101, "dv",		 336 }, /* 0x0065 */
    {  1125, "dv-MV",		 200 }, /* 0x0465 */
    {     8, "el",		 221 }, /* 0x0008 */
    {  1032, "el-GR",		  23 }, /* 0x0408 */
    {     9, "en",		 231 }, /* 0x0009 */
    {  3081, "en-AU",		 286 }, /* 0x0c09 */
    { 10249, "en-BZ",		 223 }, /* 0x2809 */
    {  4105, "en-CA",		  40 }, /* 0x1009 */
    {  9225, "en-CB",		  57 }, /* 0x2409 */
    {  2057, "en-GB",		 198 }, /* 0x0809 */
    { 15369, "en-HK",		 211 }, /* 0x3c09 */
    { 14345, "en-ID",		 150 }, /* 0x3809 */
    {  6153, "en-IE",		 204 }, /* 0x1809 */
    { 16393, "en-IN",		 332 }, /* 0x4009 */
    {  8201, "en-JM",		 296 }, /* 0x2009 */
    { 17417, "en-MY",		 185 }, /* 0x4409 */
    {  5129, "en-NZ",		   2 }, /* 0x1409 */
    { 13321, "en-PH",		 356 }, /* 0x3409 */
    { 18441, "en-SG",		 241 }, /* 0x4809 */
    { 11273, "en-TT",		 143 }, /* 0x2c09 */
    {  1033, "en-US",		 265 }, /* 0x0409 */
    {  7177, "en-ZA",		 122 }, /* 0x1c09 */
    { 12297, "en-ZW",		  69 }, /* 0x3009 */
    {    10, "es",		 157 }, /* 0x000a */
    { 11274, "es-AR",		 379 }, /* 0x2c0a */
    { 16394, "es-BO",		 272 }, /* 0x400a */
    { 13322, "es-CL",		 250 }, /* 0x340a */
    {  9226, "es-CO",		  30 }, /* 0x240a */
    {  5130, "es-CR",		 209 }, /* 0x140a */
    {  7178, "es-DO",		 196 }, /* 0x1c0a */
    { 12298, "es-EC",		 176 }, /* 0x300a */
    {  3082, "es-ES",		 178 }, /* 0x0c0a */
    {  1034, "es-ES_tradnl",	  21 }, /* 0x040a */
    {  4106, "es-GT",		 229 }, /* 0x100a */
    { 18442, "es-HN",		  43 }, /* 0x480a */
    {  2058, "es-MX",		 359 }, /* 0x080a */
    { 19466, "es-NI",		 217 }, /* 0x4c0a */
    {  6154, "es-PA",		 252 }, /* 0x180a */
    { 10250, "es-PE",		  53 }, /* 0x280a */
    { 20490, "es-PR",		 153 }, /* 0x500a */
    { 15370, "es-PY",		 288 }, /* 0x3c0a */
    { 17418, "es-SV",		 270 }, /* 0x440a */
    { 21514, "es-US",		 284 }, /* 0x540a */
    { 14346, "es-UY",		 375 }, /* 0x380a */
    {  8202, "es-VE",		 263 }, /* 0x200a */
    {    37, "et",		 148 }, /* 0x0025 */
    {  1061, "et-EE",		  17 }, /* 0x0425 */
    {    45, "eu",		  35 }, /* 0x002d */
    {  1069, "eu-ES",		  51 }, /* 0x042d */
    {    41, "fa",		 388 }, /* 0x0029 */
    {  1065, "fa-IR",		  56 }, /* 0x0429 */
    {    11, "fi",		  60 }, /* 0x000b */
    {  1035, "fi-FI",		  64 }, /* 0x040b */
    {   100, "fil",		  72 }, /* 0x0064 */
    {  1124, "fil-PH",		  89 }, /* 0x0464 */
    {    56, "fo",		 101 }, /* 0x0038 */
    {  1080, "fo-FO",		 121 }, /* 0x0438 */
    {    12, "fr",		 133 }, /* 0x000c */
    {  2060, "fr-BE",		 162 }, /* 0x080c */
    {  3084, "fr-CA",		 170 }, /* 0x0c0c */
    {  9228, "fr-CG",		 181 }, /* 0x240c */
    {  4108, "fr-CH",		 184 }, /* 0x100c */
    { 12300, "fr-CI",		 191 }, /* 0x300c */
    { 11276, "fr-CM",		 203 }, /* 0x2c0c */
    {  1036, "fr-FR",		 246 }, /* 0x040c */
    { 15372, "fr-HT",		 240 }, /* 0x3c0c */
    {  5132, "fr-LU",		 261 }, /* 0x140c */
    { 14348, "fr-MA",		 268 }, /* 0x380c */
    {  6156, "fr-MC",		 277 }, /* 0x180c */
    { 13324, "fr-ML",		 280 }, /* 0x340c */
    {  8204, "fr-RE",		 283 }, /* 0x200c */
    { 10252, "fr-SN",		 167 }, /* 0x280c */
    {  7180, "fr-West Indies",	 299 }, /* 0x1c0c */
    {  1127, "fuv-NG",		 314 }, /* 0x0467 */
    {    98, "fy",		 329 }, /* 0x0062 */
    {  1122, "fy-NL",		 342 }, /* 0x0462 */
    {    60, "ga",		 352 }, /* 0x003c */
    {  2108, "ga-IE",		 365 }, /* 0x083c */
    {  1138, "gaz-ET",		 175 }, /* 0x0472 */
    {   145, "gd",		 362 }, /* 0x0091 */
    {  1169, "gd-GB",		  33 }, /* 0x0491 */
    {    86, "gl",		 301 }, /* 0x0056 */
    {  1110, "gl-ES",		 115 }, /* 0x0456 */
    {  1140, "gn-PY",		 216 }, /* 0x0474 */
    {   132, "gsw",		 214 }, /* 0x0084 */
    {  1156, "gsw-FR",		 340 }, /* 0x0484 */
    {    71, "gu",		 119 }, /* 0x0047 */
    {  1095, "gu-IN",		 373 }, /* 0x0447 */
    {   104, "ha",		 172 }, /* 0x0068 */
    { 31848, "ha-Latn",		  29 }, /* 0x7c68 */
    {  1128, "ha-Latn-NG",	 117 }, /* 0x0468 */
    {  1141, "haw-US",		 374 }, /* 0x0475 */
    {    13, "he",		 220 }, /* 0x000d */
    {  1037, "he-IL",		 326 }, /* 0x040d */
    {    57, "hi",		 353 }, /* 0x0039 */
    {  1081, "hi-IN",		 350 }, /* 0x0439 */
    {    26, "hr",		 371 }, /* 0x001a */
    {  4122, "hr-BA",		 378 }, /* 0x101a */
    {  1050, "hr-HR",		 390 }, /* 0x041a */
    {    46, "hsb",		   1 }, /* 0x002e */
    {    14, "hu",		 193 }, /* 0x000e */
    {  1038, "hu-HU",		 125 }, /* 0x040e */
    {    43, "hy",		 164 }, /* 0x002b */
    {  1067, "hy-AM",		 237 }, /* 0x042b */
    {  1129, "ibb-NG",		 294 }, /* 0x0469 */
    {    33, "id",		 235 }, /* 0x0021 */
    {  1057, "id-ID",		 195 }, /* 0x0421 */
    {   112, "ig",		 208 }, /* 0x0070 */
    {  1136, "ig-NG",		 331 }, /* 0x0470 */
    {   120, "ii",		 346 }, /* 0x0078 */
    {  1144, "ii-CN",		 370 }, /* 0x0478 */
    {    15, "is",		 355 }, /* 0x000f */
    {  1039, "is-IS",		  39 }, /* 0x040f */
    {    16, "it",		 257 }, /* 0x0010 */
    {  2064, "it-CH",		 156 }, /* 0x0810 */
    {  1040, "it-IT",		 255 }, /* 0x0410 */
    {    93, "iu",		 335 }, /* 0x005d */
    { 30813, "iu-Cans",		 337 }, /* 0x785d */
    {  1117, "iu-Cans-CA",	 201 }, /* 0x045d */
    { 31837, "iu-Latn",		 222 }, /* 0x7c5d */
    {  2141, "iu-Latn-CA",	  24 }, /* 0x085d */
    {    17, "ja",		 232 }, /* 0x0011 */
    {  1041, "ja-JP",		 287 }, /* 0x0411 */
    {    55, "ka",		 225 }, /* 0x0037 */
    {  1079, "ka-GE",		  42 }, /* 0x0437 */
    {    63, "kk",		  58 }, /* 0x003f */
    {  1087, "kk-KZ",		 199 }, /* 0x043f */
    {   111, "kl",		 212 }, /* 0x006f */
    {  1135, "kl-GL",		 238 }, /* 0x046f */
    {    83, "km",		 151 }, /* 0x0053 */
    {  1107, "km-KH",		 205 }, /* 0x0453 */
    {    75, "kn",		 228 }, /* 0x004b */
    {  1099, "kn-IN",		 290 }, /* 0x044b */
    {    18, "ko",		 333 }, /* 0x0012 */
    {  1042, "ko-KR",		 297 }, /* 0x0412 */
    {    87, "kok",		  52 }, /* 0x0057 */
    {  1111, "kok-IN",		 187 }, /* 0x0457 */
    {  1137, "kr-NG",		   3 }, /* 0x0471 */
    {    64, "ky",		 347 }, /* 0x0040 */
    {  1088, "ky-KG",		 243 }, /* 0x0440 */
    {   110, "lb",		 144 }, /* 0x006e */
    {  1134, "lb-LU",		 266 }, /* 0x046e */
    {    84, "lo",		 123 }, /* 0x0054 */
    {  1108, "lo-LA",		  70 }, /* 0x0454 */
    {    39, "lt",		  36 }, /* 0x0027 */
    {  1063, "lt-LT",		 142 }, /* 0x0427 */
    {    38, "lv",		 159 }, /* 0x0026 */
    {  1062, "lv-LV",		 173 }, /* 0x0426 */
    {   129, "mi",		 380 }, /* 0x0081 */
    {  1153, "mi-NZ",		 273 }, /* 0x0481 */
    {    47, "mk",		 251 }, /* 0x002f */
    {  1071, "mk-MK",		  31 }, /* 0x042f */
    {    76, "ml",		 210 }, /* 0x004c */
    {  1100, "ml-IN",		 197 }, /* 0x044c */
    {    80, "mn",		 177 }, /* 0x0050 */
    { 30800, "mn-Cyrl",		 206 }, /* 0x7850 */
    {  1104, "mn-MN",		 147 }, /* 0x0450 */
    { 31824, "mn-Mong",		 343 }, /* 0x7c50 */
    {  2128, "mn-Mong-CN",	 152 }, /* 0x0850 */
    {  1112, "mni",		 160 }, /* 0x0458 */
    {   124, "moh",		 312 }, /* 0x007c */
    {  1148, "moh-CA",		 179 }, /* 0x047c */
    {    78, "mr",		 259 }, /* 0x004e */
    {  1102, "mr-IN",		  22 }, /* 0x044e */
    {    62, "ms",		 230 }, /* 0x003e */
    {  2110, "ms-BN",		  44 }, /* 0x083e */
    {  1086, "ms-MY",		 360 }, /* 0x043e */
    {    58, "mt",		 218 }, /* 0x003a */
    {  1082, "mt-MT",		 253 }, /* 0x043a */
    {  1109, "my-MM",		  54 }, /* 0x0455 */
    { 31764, "nb",		 154 }, /* 0x7c14 */
    {  1044, "nb-NO",		 289 }, /* 0x0414 */
    {    97, "ne",		 271 }, /* 0x0061 */
    {  2145, "ne-IN",		 285 }, /* 0x0861 */
    {  1121, "ne-NP",		 376 }, /* 0x0461 */
    {    19, "nl",		 264 }, /* 0x0013 */
    {  2067, "nl-BE",		 262 }, /* 0x0813 */
    {  1043, "nl-NL",		 149 }, /* 0x0413 */
    { 30740, "nn",		   9 }, /* 0x7814 */
    {  2068, "nn-NO",		 382 }, /* 0x0814 */
    {    20, "no",		  63 }, /* 0x0014 */
    {   108, "nso",		  78 }, /* 0x006c */
    {  1132, "nso-ZA",		 104 }, /* 0x046c */
    {   130, "oc",		 127 }, /* 0x0082 */
    {  1154, "oc-FR",		 183 }, /* 0x0482 */
    {    72, "or",		 245 }, /* 0x0048 */
    {  1096, "or-IN",		 248 }, /* 0x0448 */
    {    70, "pa",		 269 }, /* 0x0046 */
    {  1094, "pa-IN",		 279 }, /* 0x0446 */
    {  2118, "pa-PK",		 282 }, /* 0x0846 */
    {  1145, "pap-AN",		 323 }, /* 0x0479 */
    {    21, "pl",		 328 }, /* 0x0015 */
    {  1045, "pl-PL",		 364 }, /* 0x0415 */
    {  1165, "plt-MG",		  27 }, /* 0x048d */
    {   140, "prs",		  68 }, /* 0x008c */
    {  1164, "prs-AF",		 295 }, /* 0x048c */
    {    99, "ps",		 146 }, /* 0x0063 */
    {  1123, "ps-AF",		 234 }, /* 0x0463 */
    {    22, "pt",		 368 }, /* 0x0016 */
    {  1046, "pt-BR",		  38 }, /* 0x0416 */
    {  2070, "pt-PT",		 258 }, /* 0x0816 */
    {   134, "qut",		 227 }, /* 0x0086 */
    {  1158, "qut-GT",		  41 }, /* 0x0486 */
    {   107, "quz",		 291 }, /* 0x006b */
    {  1131, "quz-BO",		 189 }, /* 0x046b */
    {  2155, "quz-EC",		 358 }, /* 0x086b */
    {  3179, "quz-PE",		 242 }, /* 0x0c6b */
    {    23, "rm",		 274 }, /* 0x0017 */
    {  1047, "rm-CH",		 344 }, /* 0x0417 */
    {    24, "ro",		   8 }, /* 0x0018 */
    {  2072, "ro-MO",		 383 }, /* 0x0818 */
    {  1048, "ro-RO",		  62 }, /* 0x0418 */
    {    25, "ru",		  74 }, /* 0x0019 */
    {  2073, "ru-MO",		 100 }, /* 0x0819 */
    {  1049, "ru-RU",		 128 }, /* 0x0419 */
    {   135, "rw",		 318 }, /* 0x0087 */
    {  1159, "rw-RW",		 293 }, /* 0x0487 */
    {    79, "sa",		 348 }, /* 0x004f */
    {  1103, "sa-IN",		 275 }, /* 0x044f */
    {   133, "sah",		  13 }, /* 0x0085 */
    {  1157, "sah-RU",		 387 }, /* 0x0485 */
    {  1113, "sd-IN",		  66 }, /* 0x0459 */
    {  2137, "sd-PK",		  76 }, /* 0x0859 */
    {    59, "se",		 102 }, /* 0x003b */
    {  3131, "se-FI",		 130 }, /* 0x0c3b */
    {  1083, "se-NO",		 166 }, /* 0x043b */
    {  2107, "se-SE",		 306 }, /* 0x083b */
    {    91, "si",		   7 }, /* 0x005b */
    {  1115, "si-LK",		 386 }, /* 0x045b */
    {    27, "sk",		  65 }, /* 0x001b */
    {  1051, "sk-SK",		  85 }, /* 0x041b */
    {    36, "sl",		  97 }, /* 0x0024 */
    {  1060, "sl-SI",		 135 }, /* 0x0424 */
    { 30779, "sma",		  49 }, /* 0x783b */
    {  6203, "sma-NO",		 307 }, /* 0x183b */
    {  7227, "sma-SE",		  14 }, /* 0x1c3b */
    { 31803, "smj",		  81 }, /* 0x7c3b */
    {  4155, "smj-NO",		 106 }, /* 0x103b */
    {  5179, "smj-SE",		 137 }, /* 0x143b */
    { 28731, "smn",		 322 }, /* 0x703b */
    {  9275, "smn-FI",		 303 }, /* 0x243b */
    { 29755, "sms",		  19 }, /* 0x743b */
    {  8251, "sms-FI",		  90 }, /* 0x203b */
    {  1143, "so-SO",		  98 }, /* 0x0477 */
    {    28, "sq",		 141 }, /* 0x001c */
    {  1052, "sq-AL",		 317 }, /* 0x041c */
    { 31770, "sr",		 304 }, /* 0x7c1a */
    { 27674, "sr-Cyrl",		  15 }, /* 0x6c1a */
    {  7194, "sr-Cyrl-BA",	  83 }, /* 0x1c1a */
    {  3098, "sr-Cyrl-CS",	 113 }, /* 0x0c1a */
    { 12314, "sr-Cyrl-ME",	 139 }, /* 0x301a */
    { 10266, "sr-Cyrl-RS",	  47 }, /* 0x281a */
    { 28698, "sr-Latn",		 311 }, /* 0x701a */
    {  6170, "sr-Latn-BA",	  20 }, /* 0x181a */
    {  2074, "sr-Latn-CS",	  77 }, /* 0x081a */
    { 11290, "sr-Latn-ME",	  96 }, /* 0x2c1a */
    {  9242, "sr-Latn-RS",	 129 }, /* 0x241a */
    {  1072, "st-ZA",		 325 }, /* 0x0430 */
    {    29, "sv",		 309 }, /* 0x001d */
    {  2077, "sv-FI",		  18 }, /* 0x081d */
    {  1053, "sv-SE",		  75 }, /* 0x041d */
    {    65, "sw",		 107 }, /* 0x0041 */
    {  1089, "sw-KE",		 140 }, /* 0x0441 */
    {    90, "syr",		 320 }, /* 0x005a */
    {  1114, "syr-SY",		  10 }, /* 0x045a */
    {    73, "ta",		  88 }, /* 0x0049 */
    {  1097, "ta-IN",		  93 }, /* 0x0449 */
    {    74, "te",		 132 }, /* 0x004a */
    {  1098, "te-IN",		 324 }, /* 0x044a */
    {    40, "tg",		  12 }, /* 0x0028 */
    { 31784, "tg-Cyrl",		  91 }, /* 0x7c28 */
    {  1064, "tg-Cyrl-TJ",	  99 }, /* 0x0428 */
    {    30, "th",		 131 }, /* 0x001e */
    {  1054, "th-TH",		 319 }, /* 0x041e */
    {  1139, "ti-ER",		  11 }, /* 0x0473 */
    {  2163, "ti-ET",		  86 }, /* 0x0873 */
    {    66, "tk",		  95 }, /* 0x0042 */
    {  1090, "tk-TM",		 138 }, /* 0x0442 */
    {  1119, "tmz",		   5 }, /* 0x045f */
    {  3167, "tmz-MA",		  80 }, /* 0x0c5f */
    {    50, "tn",		 112 }, /* 0x0032 */
    {  1074, "tn-ZA",		 136 }, /* 0x0432 */
    {    31, "tr",		   6 }, /* 0x001f */
    {  1055, "tr-TR",		  79 }, /* 0x041f */
    {  1073, "ts-ZA",		 109 }, /* 0x0431 */
    {    68, "tt",		 134 }, /* 0x0044 */
    {  1092, "tt-RU",		  16 }, /* 0x0444 */
    {    95, "tzm",		  82 }, /* 0x005f */
    { 31839, "tzm-Latn",	  94 }, /* 0x7c5f */
    {  2143, "tzm-Latn-DZ",	  84 }, /* 0x085f */
    {   128, "ug",		 110 }, /* 0x0080 */
    {  1152, "ug-CN",		  87 }, /* 0x0480 */
    {    34, "uk",		 103 }, /* 0x0022 */
    {  1058, "uk-UA",		 105 }, /* 0x0422 */
    {    32, "ur",		 108 }, /* 0x0020 */
    {  2080, "ur-IN",		 111 }, /* 0x0820 */
    {  1056, "ur-PK",		  46 }, /* 0x0420 */
    {    67, "uz",		  48 }, /* 0x0043 */
    { 30787, "uz-Cyrl",		 316 }, /* 0x7843 */
    {  2115, "uz-Cyrl-UZ",	 321 }, /* 0x0843 */
    { 31811, "uz-Latn",		 308 }, /* 0x7c43 */
    {  1091, "uz-Latn-UZ",	  26 }, /* 0x0443 */
    {  1075, "ven-ZA",		 310 }, /* 0x0433 */
    {    42, "vi",		 381 }, /* 0x002a */
    {  1066, "vi-VN",		 247 }, /* 0x042a */
    {  1070, "wen-DE",		  45 }, /* 0x042e */
    {   136, "wo",		  28 }, /* 0x0088 */
    {  1160, "wo-SN",		 302 }, /* 0x0488 */
    {    52, "xh",		 367 }, /* 0x0034 */
    {  1076, "xh-ZA",		 224 }, /* 0x0434 */
    {   106, "yo",		 186 }, /* 0x006a */
    {  1130, "yo-NG",		 385 }, /* 0x046a */
    { 30724, "zh",		 239 }, /* 0x7804 */
    {  2052, "zh-CN",		 315 }, /* 0x0804 */
    {  3076, "zh-HK",		 339 }, /* 0x0c04 */
    {     4, "zh-Hans",		  67 }, /* 0x0004 */
    { 31748, "zh-Hant",		 305 }, /* 0x7c04 */
    {  5124, "zh-MO",		 369 }, /* 0x1404 */
    {  4100, "zh-SG",		 226 }, /* 0x1004 */
    {  1028, "zh-TW",		 188 }, /* 0x0404 */
    {    53, "zu",		 357 }, /* 0x0035 */
    {  1077, "zu-ZA",		 158 }, /* 0x0435 */
};

static const int LanguageCodeCount= sizeof(LanguageCodes)/sizeof(LanguageCode);

const char * utilLangForLanguageCode(		int		code )
    {
    int		l= 0, r= LanguageCodeCount, m= ( l+ r )/ 2;

    while( l < m )
	{
	if  ( LanguageCodes[LanguageCodes[m].lcCodeIndex].lcCode <= code )
	    { l= m; }
	else{ r= m; }

	m= ( l+ r )/ 2;
	}

    if  ( code != LanguageCodes[LanguageCodes[m].lcCodeIndex].lcCode )
	{ return (const char *)0;					}
    else{ return LanguageCodes[LanguageCodes[m].lcCodeIndex].lcLang;	}
    }

int utilLanguageCodeForLang(		const char *	lang )
    {
    int		l= 0, r= LanguageCodeCount, m= ( l+ r )/ 2;

    while( l < m )
	{
	if  ( strcmp( LanguageCodes[m].lcLang, lang ) <= 0 )
	    { l= m; }
	else{ r= m; }

	m= ( l+ r )/ 2;
	}

    if  ( strcmp( lang, LanguageCodes[m].lcLang ) != 0 )
	{ return -1;				}
    else{ return LanguageCodes[m].lcCode;	}
    }

#   if GENERATE

static int utilCompareLanguageCodesByNumber(	const void *	v1,
						const void *	v2 )
    {
    const LanguageCode *	lc1= (const LanguageCode *)v1;
    const LanguageCode *	lc2= (const LanguageCode *)v2;

    return lc1->lcCode- lc2->lcCode;
    }

static int utilCompareLanguageCodesByString(	const void *	v1,
						const void *	v2 )
    {
    const LanguageCode *	lc1= (const LanguageCode *)v1;
    const LanguageCode *	lc2= (const LanguageCode *)v2;

    return strcmp( lc1->lcLang, lc2->lcLang );
    }

int main( int argc, char ** argv )
    {
    int 		i;
    const char *	tabs= "\t\t\t\t";

    qsort( LanguageCodes, LanguageCodeCount, sizeof(LanguageCode),
					utilCompareLanguageCodesByNumber );

    for ( i= 0; i < sizeof(LanguageCodes)/sizeof(LanguageCode); i++ )
	{
	LanguageCode *	lc= &(LanguageCodes[i]);

	lc->lcSource= i;
	lc->lcCodeIndex= i;
	}

    qsort( LanguageCodes, LanguageCodeCount, sizeof(LanguageCode),
					utilCompareLanguageCodesByString );

    for ( i= 0; i < sizeof(LanguageCodes)/sizeof(LanguageCode); i++ )
	{
	LanguageCode *	lc= &(LanguageCodes[i]);

	LanguageCodes[lc->lcSource].lcCodeIndex= i;
	}

    for ( i= 0; i < LanguageCodeCount; i++ )
	{
	const LanguageCode *	lc= &(LanguageCodes[i]);
	const char *		lang;
	int			code;

	lang= utilLangForLanguageCode( lc->lcCode );
	code= utilLanguageCodeForLang( lc->lcLang );

	if  ( code != lc->lcCode )
	    {
	    fprintf( stderr, "@%3d: %d != %d\n", i, code, lc->lcCode );
	    abort();
	    }

	if  ( ! lang )
	    {
	    fprintf( stderr, "@%3d: \"%s\" -> NULL\n", i, lc->lcLang );
	    abort();
	    }

	if  ( strcmp( lang, lc->lcLang ) )
	    {
	    fprintf( stderr, "@ %3d: \"%s\" != \"%s\"\n",
						i, lang, lc->lcLang );
	    abort();
	    }
	}

    if  ( utilLanguageCodeForLang( "AA" ) >= 0 )
	{ fprintf( stderr, "\"AA\"\n" ); abort();	}
    if  ( utilLanguageCodeForLang( "zzz" ) >= 0 )
	{ fprintf( stderr, "\"zzz\"\n" ); abort();	}
    if  ( utilLanguageCodeForLang( "ukz" ) >= 0 )
	{ fprintf( stderr, "\"ukz\"\n" ); abort();	}

    if  ( utilLangForLanguageCode( 0 ) )
	{ fprintf( stderr, "0\n" ); abort();	}
    if  ( utilLangForLanguageCode( 32767 ) )
	{ fprintf( stderr, "32767\n" ); abort();	}
    if  ( utilLangForLanguageCode( 1000 ) )
	{ fprintf( stderr, "1000\n" ); abort();	}

    for ( i= 0; i < LanguageCodeCount; i++ )
	{
	int			col;
	const LanguageCode *	lc= &(LanguageCodes[i]);
	const LanguageCode *	lcOut= lc;

	/* lcOut= &(LanguageCodes[lc->lcCodeIndex]); */

	col= 4+ 2+ 5+ 3+ strlen( lcOut->lcLang )+ 2;

	col= col / 8;

	printf( "    { %5d, \"%s\",%s%4d }, /* 0x%04x */\n",
	    lcOut->lcCode,
	    lcOut->lcLang,
	    tabs+ col,
	    lcOut->lcCodeIndex,
	    LanguageCodes[i].lcCode);
	}

    return 0;
    }

#   endif
