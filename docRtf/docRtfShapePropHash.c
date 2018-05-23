#   include <appUtilConfig.h>

#   include <utilJenkinsHash.h>

#   include "docRtfTagHash.h"

#   define ub4 UtilUint32
#   define ub2 UtilUint16
#   define ub1 unsigned char

#   define CHECKSTATE 8

/* smax= 512, blen= 256 */

/* small adjustments to _a_ to make values distinct */
static const ub2 tab[] = {
28,0,243,0,258,65,468,0,481,468,162,243,0,112,65,36,
243,404,0,0,230,0,258,65,154,481,324,28,65,0,154,324,
0,0,484,0,468,468,0,28,0,345,404,154,258,469,404,399,
258,471,337,258,258,28,319,404,36,404,230,0,6,154,469,258,
468,162,0,243,28,258,258,258,154,154,0,23,154,0,481,0,
0,468,0,481,258,0,0,243,258,0,0,65,258,0,258,0,
112,285,6,0,6,404,0,264,243,0,264,154,469,468,162,324,
319,462,395,243,258,404,469,0,157,28,258,28,468,462,0,28,
26,258,211,328,65,0,345,258,162,324,0,162,471,0,319,65,
230,0,324,211,258,481,328,154,319,258,65,162,468,0,0,469,
328,419,204,0,419,162,0,319,256,6,154,65,201,258,337,154,
154,337,395,65,162,404,258,243,378,174,28,379,468,131,146,0,
131,258,481,243,324,258,26,146,328,0,0,484,0,157,243,0,
258,319,256,0,0,258,157,157,338,345,258,180,162,0,324,243,
7,243,327,6,0,136,146,238,258,101,328,0,230,234,419,184,
258,337,0,0,419,154,468,238,292,359,154,39,234,0,0,183,
};

/* map back array */

static short mapBack[]=
    {
     350L,	/*     0: "lineColumnBackColorExtK" */
     265L,	/*     1: "pictureFillCrMod" */
       7L,	/*     2: "pibName" */
     457L,	/*     3: "lineTopType" */
      34L,	/*     4: "fHidden" */
     269L,	/*     5: "shapePath" */
     263L,	/*     6: "pictureBrightness" */
     414L,	/*     7: "lineRightDashing" */
     389L,	/*     8: "lineLeftEndArrowhead" */
     287L,	/*     9: "c3DExtrudePlane" */
     477L,	/*    10: "pctHoriz" */
     456L,	/*    11: "lineTopStyle" */
     131L,	/*    12: "fillToLeft" */
     104L,	/*    13: "lineBottomFillShape" */
     317L,	/*    14: "lineBottomBackColorExtCMY" */
     291L,	/*    15: "c3DOriginY" */
      82L,	/*    16: "lineJoinStyle" */
      13L,	/*    17: "posrelh" */
     197L,	/*    18: "fshadowObscured" */
     309L,	/*    19: "fillColorExtK" */
     374L,	/*    20: "lineColumnWidth" */
      17L,	/*    21: "fChangePage" */
     339L,	/*    22: "lineBottomStartArrowhead" */
      88L,	/*    23: "fColumnLineOK" */
      86L,	/*    24: "fLineOK" */
     134L,	/*    25: "fillToBottom" */
      70L,	/*    26: "lineFillHeight" */
      51L,	/*    27: "fPseudoInline" */
     461L,	/*    28: "shadowColorExtK" */
     448L,	/*    29: "lineTopFillDztype" */
     258L,	/*    30: "fCalloutDropAuto" */
     173L,	/*    31: "lidRegroup" */
     390L,	/*    32: "lineLeftEndCapStyle" */
     446L,	/*    33: "lineTopEndCapStyle" */
     459L,	/*    34: "shadowColorExt" */
      10L,	/*    35: "posv" */
     177L,	/*    36: "groupBottom" */
     431L,	/*    37: "lineTopBackColor" */
      52L,	/*    38: "dxTextLeft" */
     200L,	/*    39: "shadowColor" */
     419L,	/*    40: "lineRightFillBlipFlags" */
     111L,	/*    41: "lineBackColor" */
     171L,	/*    42: "relBottom" */
     241L,	/*    43: "c3DFillX" */
     321L,	/*    44: "lineBottomColorExt" */
     353L,	/*    45: "lineColumnColorExt" */
     371L,	/*    46: "lineColumnStartArrowhead" */
     363L,	/*    47: "lineColumnFillBlipFlags" */
      67L,	/*    48: "lineType" */
     253L,	/*    49: "dxyCalloutDropSpecified" */
     207L,	/*    50: "gtextFKern" */
     356L,	/*    51: "lineColumnColorExtMod" */
     292L,	/*    52: "c3DRenderMode" */
     410L,	/*    53: "lineRightColorExtCMY" */
     324L,	/*    54: "lineBottomColorExtMod" */
     304L,	/*    55: "c3DExtrusionColorExtCMY" */
      25L,	/*    56: "hspMaster" */
      68L,	/*    57: "lineFillBlipFlags" */
     328L,	/*    58: "lineBottomEndArrowWidth" */
     311L,	/*    59: "lineBackColorExt" */
     225L,	/*    60: "c3DExtrudeBackward" */
     439L,	/*    61: "lineTopColorExtK" */
     392L,	/*    62: "lineLeftFillDztype" */
     331L,	/*    63: "lineBottomFillBlipFlags" */
     393L,	/*    64: "lineLeftFillHeight" */
     340L,	/*    65: "lineBottomStyle" */
       1L,	/*    66: "pWrapPolygonVertices" */
     102L,	/*    67: "lineTopFillShape" */
      53L,	/*    68: "dyTextTop" */
     420L,	/*    69: "lineRightFillDztype" */
      73L,	/*    70: "lineDashing" */
     206L,	/*    71: "gtextFVertical" */
     351L,	/*    72: "lineColumnBackColorExtMod" */
     377L,	/*    73: "lineLeftBackColorExtCMY" */
     293L,	/*    74: "c3DRotationAngle" */
     262L,	/*    75: "pictureContrast" */
     191L,	/*    76: "shadowPerspectiveX" */
     300L,	/*    77: "c3DTolerance" */
      95L,	/*    78: "fLeftHitTestLine" */
     166L,	/*    79: "adjust9Value" */
     209L,	/*    80: "gtextFStretch" */
     375L,	/*    81: "lineLeftBackColor" */
     223L,	/*    82: "c3DEdgeThickness" */
     326L,	/*    83: "lineBottomDashing" */
      78L,	/*    84: "lineEndArrowWidth" */
     412L,	/*    85: "lineRightColorExtMod" */
     462L,	/*    86: "shadowColorExtMod" */
     254L,	/*    87: "dxyCalloutLengthSpecified" */
     267L,	/*    88: "fCalloutMinusX" */
     281L,	/*    89: "dzColMargin" */
       9L,	/*    90: "pib" */
     450L,	/*    91: "lineTopFillWidth" */
     230L,	/*    92: "c3DRotationAxisX" */
     237L,	/*    93: "c3DZViewpoint" */
     268L,	/*    94: "fCalloutMinusY" */
      57L,	/*    95: "lTxid" */
      87L,	/*    96: "fLineUseShapeAnchor" */
     249L,	/*    97: "spcot" */
      94L,	/*    98: "fColumnHitTestLine" */
     347L,	/*    99: "lineColumnBackColor" */
     147L,	/*   100: "fillShape" */
     201L,	/*   101: "shadowHighlight" */
     346L,	/*   102: "lineColorExtMod" */
     322L,	/*   103: "lineBottomColorExtCMY" */
      63L,	/*   104: "fRotateText" */
     397L,	/*   105: "lineLeftStartArrowLength" */
     211L,	/*   106: "gtextFBestFit" */
      43L,	/*   107: "fNoFillHitTest" */
     312L,	/*   108: "lineBackColorExtCMY" */
     470L,	/*   109: "fStandardHR" */
     180L,	/*   110: "fRelFlipV" */
     128L,	/*   111: "fillHeight" */
     435L,	/*   112: "lineTopBackColorExtMod" */
     376L,	/*   113: "lineLeftBackColorExt" */
     443L,	/*   114: "lineTopEndArrowLength" */
     332L,	/*   115: "lineBottomFillDztype" */
     344L,	/*   116: "lineColorExtCMY" */
     117L,	/*   117: "fLockVerticies" */
     156L,	/*   118: "geoRight" */
     169L,	/*   119: "relTop" */
     354L,	/*   120: "lineColumnColorExtCMY" */
     261L,	/*   121: "pictureActive" */
     357L,	/*   122: "lineColumnCrMod" */
      41L,	/*   123: "fHitTestLine" */
      55L,	/*   124: "dyTextBottom" */
      93L,	/*   125: "fBottomLine" */
     472L,	/*   126: "fHorizRule" */
     295L,	/*   127: "c3DRotationCenterY" */
     394L,	/*   128: "lineLeftFillWidth" */
     301L,	/*   129: "c3DXRotationAngle" */
     224L,	/*   130: "c3DExtrudeForward" */
     421L,	/*   131: "lineRightFillHeight" */
     476L,	/*   132: "sizerelv" */
     320L,	/*   133: "lineBottomColor" */
      97L,	/*   134: "fRightHitTestLine" */
     176L,	/*   135: "groupRight" */
     257L,	/*   136: "fCalloutTextBorder" */
      60L,	/*   137: "txflTextFlow" */
     110L,	/*   138: "lineColor" */
     428L,	/*   139: "lineRightStyle" */
     204L,	/*   140: "gtextAlign" */
     184L,	/*   141: "shadowOffsetY" */
      76L,	/*   142: "lineStartArrowWidth" */
     252L,	/*   143: "spcod" */
     182L,	/*   144: "shadowOpacity" */
     164L,	/*   145: "adjust7Value" */
     189L,	/*   146: "shadowScaleXToY" */
     154L,	/*   147: "geoLeft" */
      36L,	/*   148: "fArrowheadsOK" */
      77L,	/*   149: "lineStartArrowLength" */
     380L,	/*   150: "lineLeftColor" */
     288L,	/*   151: "c3DFillIntensity" */
      16L,	/*   152: "fAllowOverlap" */
     316L,	/*   153: "lineBottomBackColorExt" */
     277L,	/*   154: "fPreferRelativeResize" */
     463L,	/*   155: "shadowHighlightExt" */
     323L,	/*   156: "lineBottomColorExtK" */
     240L,	/*   157: "c3DKeyZ" */
     445L,	/*   158: "lineTopEndArrowhead" */
     133L,	/*   159: "fillToRight" */
     108L,	/*   160: "fRightInsetPen" */
     121L,	/*   161: "fLockShapeType" */
     417L,	/*   162: "lineRightEndArrowhead" */
     174L,	/*   163: "groupLeft" */
     256L,	/*   164: "fCalloutAccentBar" */
     405L,	/*   165: "lineRightBackColorExtCMY" */
     158L,	/*   166: "adjustValue" */
     167L,	/*   167: "adjust10Value" */
      98L,	/*   168: "fBottomHitTestLine" */
     438L,	/*   169: "lineTopColorExtCMY" */
     187L,	/*   170: "shadowScaleXToX" */
     345L,	/*   171: "lineColorExtK" */
     233L,	/*   172: "fC3DRotationCenterAut" */
      39L,	/*   173: "fEditedWrap" */
     116L,	/*   174: "fLockCropping" */
     314L,	/*   175: "lineBackColorExtMod" */
     408L,	/*   176: "lineRightColor" */
     336L,	/*   177: "lineBottomOpacity" */
      35L,	/*   178: "fReallyHidden" */
       3L,	/*   179: "wzName" */
     319L,	/*   180: "lineBottomBackColorExtMod" */
     153L,	/*   181: "fillCrMod" */
      72L,	/*   182: "lineStyle" */
     471L,	/*   183: "fNoshadeHR" */
     120L,	/*   184: "fLockAgainstGrouping" */
     188L,	/*   185: "shadowScaleYToX" */
     294L,	/*   186: "c3DRotationCenterX" */
     367L,	/*   187: "lineColumnMiterLimit" */
     391L,	/*   188: "lineLeftFillBlipFlags" */
      32L,	/*   189: "fBehindDocument" */
     348L,	/*   190: "lineColumnBackColorExt" */
     205L,	/*   191: "fGtext" */
      37L,	/*   192: "fBackground" */
      54L,	/*   193: "dxTextRight" */
     175L,	/*   194: "groupTop" */
      50L,	/*   195: "fPrint" */
     122L,	/*   196: "fLockPosition" */
      92L,	/*   197: "fRightLine" */
     441L,	/*   198: "lineTopCrMod" */
     144L,	/*   199: "fillShadePreset" */
     145L,	/*   200: "fillShadeType" */
     449L,	/*   201: "lineTopFillHeight" */
     242L,	/*   202: "c3DFillY" */
     106L,	/*   203: "fLeftInsetPen" */
     114L,	/*   204: "fLockAspectRatio" */
     250L,	/*   205: "dxyCalloutGap" */
     358L,	/*   206: "lineColumnDashing" */
      89L,	/*   207: "fColumnLine" */
     129L,	/*   208: "fillAngle" */
     369L,	/*   209: "lineColumnStartArrowLength" */
     162L,	/*   210: "adjust5Value" */
     418L,	/*   211: "lineRightEndCapStyle" */
     343L,	/*   212: "lineColorExt" */
     235L,	/*   213: "c3DXViewpoint" */
     274L,	/*   214: "cropFromBottom" */
     458L,	/*   215: "lineTopWidth" */
     264L,	/*   216: "pictureDblCrMod" */
     139L,	/*   217: "fillDztype" */
     327L,	/*   218: "lineBottomEndArrowLength" */
     222L,	/*   219: "pictureBiLevel" */
     212L,	/*   220: "gtextFNormalize" */
     337L,	/*   221: "lineBottomStartArrowLength" */
      64L,	/*   222: "fSelectText" */
     453L,	/*   223: "lineTopStartArrowLength" */
     140L,	/*   224: "fillRectLeft" */
      14L,	/*   225: "fUseShapeAnchor" */
     123L,	/*   226: "fillType" */
     232L,	/*   227: "c3DRotationAxisZ" */
     469L,	/*   228: "c3DCrMod" */
     218L,	/*   229: "gtextFSmallcaps" */
     228L,	/*   230: "fc3DUseExtrusionColor" */
     271L,	/*   231: "cropFromLeft" */
     259L,	/*   232: "fCalloutLengthSpecified" */
     355L,	/*   233: "lineColumnColorExtK" */
      85L,	/*   234: "fline" */
     465L,	/*   235: "shadowHighlightExtK" */
     289L,	/*   236: "c3DKeyIntensity" */
      44L,	/*   237: "fNoHitTestPicture" */
     143L,	/*   238: "fillRectBottom" */
     163L,	/*   239: "adjust6Value" */
     151L,	/*   240: "fillColor" */
     447L,	/*   241: "lineTopFillBlipFlags" */
     474L,	/*   242: "dhgt" */
     303L,	/*   243: "c3DExtrusionColorExt" */
      24L,	/*   244: "dyWrapDistBottom" */
     455L,	/*   245: "lineTopStartArrowhead" */
      59L,	/*   246: "anchorText" */
     279L,	/*   247: "dgmt" */
     385L,	/*   248: "lineLeftCrMod" */
     181L,	/*   249: "shadowType" */
     192L,	/*   250: "shadowPerspectiveY" */
     272L,	/*   251: "cropFromRight" */
     138L,	/*   252: "fillShapeOriginY" */
      15L,	/*   253: "fLayoutInCell" */
     135L,	/*   254: "fillOriginX" */
     399L,	/*   255: "lineLeftStartArrowhead" */
     406L,	/*   256: "lineRightBackColorExtK" */
     416L,	/*   257: "lineRightEndArrowWidth" */
     149L,	/*   258: "fFillOK" */
     124L,	/*   259: "fillOpacity" */
     239L,	/*   260: "c3DKeyY" */
     248L,	/*   261: "fc3DRotationCenterAuto" */
     183L,	/*   262: "shadowOffsetX" */
      21L,	/*   263: "dxWrapDistLeft" */
     329L,	/*   264: "lineBottomEndArrowhead" */
     366L,	/*   265: "lineColumnFillWidth" */
     381L,	/*   266: "lineLeftColorExt" */
     244L,	/*   267: "fc3DParallel" */
     226L,	/*   268: "f3D" */
     415L,	/*   269: "lineRightEndArrowLength" */
     334L,	/*   270: "lineBottomFillWidth" */
      96L,	/*   271: "fTopHitTestLine" */
      28L,	/*   272: "yLimo" */
     409L,	/*   273: "lineRightColorExt" */
     196L,	/*   274: "fShadow" */
     473L,	/*   275: "fUserDrawn" */
      61L,	/*   276: "cdirFont" */
     378L,	/*   277: "lineLeftBackColorExtK" */
     160L,	/*   278: "adjust3Value" */
      83L,	/*   279: "lineMiterLimit" */
     452L,	/*   280: "lineTopOpacity" */
     109L,	/*   281: "fBottomInsetPen" */
      66L,	/*   282: "fFitTextToShape" */
     208L,	/*   283: "gtextFTight" */
     382L,	/*   284: "lineLeftColorExtCMY" */
     199L,	/*   285: "shadowCrMod" */
     302L,	/*   286: "c3DYRotationAngle" */
     400L,	/*   287: "lineLeftStyle" */
     286L,	/*   288: "c3DDiffuseAmt" */
     165L,	/*   289: "adjust8Value" */
     186L,	/*   290: "shadowSecondOffsetY" */
     266L,	/*   291: "pictureLineCrMod" */
      31L,	/*   292: "fFlipH" */
     468L,	/*   293: "c3DExtrusionColor" */
     365L,	/*   294: "lineColumnFillHeight" */
     310L,	/*   295: "fillColorExtMod" */
     464L,	/*   296: "shadowHighlightExtCMY" */
     260L,	/*   297: "pictureId" */
     388L,	/*   298: "lineLeftEndArrowWidth" */
     404L,	/*   299: "lineRightBackColorExt" */
     148L,	/*   300: "fillUseRect" */
     401L,	/*   301: "lineLeftType" */
     125L,	/*   302: "fillBackOpacity" */
     221L,	/*   303: "pictureGray" */
     243L,	/*   304: "c3DFillZ" */
      38L,	/*   305: "fDeleteAttachedObject" */
     100L,	/*   306: "lineColumnFillShape" */
      62L,	/*   307: "fAutoTextMargin" */
     362L,	/*   308: "lineColumnEndCapStyle" */
      75L,	/*   309: "lineEndArrowhead" */
      84L,	/*   310: "fLine" */
     407L,	/*   311: "lineRightBackColorExtMod" */
     219L,	/*   312: "gtextFStrikethrough" */
     115L,	/*   313: "fLockAgainstSelect" */
     193L,	/*   314: "shadowWeight" */
     136L,	/*   315: "fillOriginY" */
     460L,	/*   316: "shadowColorExtCMY" */
     290L,	/*   317: "c3DOriginX" */
     384L,	/*   318: "lineLeftColorExtMod" */
      65L,	/*   319: "fFitShapeToText" */
      71L,	/*   320: "lineWidth" */
     246L,	/*   321: "fc3DFillHarsh" */
     190L,	/*   322: "shadowScaleYToY" */
     126L,	/*   323: "fillblipflags" */
     220L,	/*   324: "pibFlags" */
      42L,	/*   325: "fInitiator" */
     195L,	/*   326: "shadowOriginY" */
     442L,	/*   327: "lineTopDashing" */
     255L,	/*   328: "fCallout" */
     432L,	/*   329: "lineTopBackColorExt" */
      58L,	/*   330: "WrapText" */
     137L,	/*   331: "fillShapeOriginX" */
     341L,	/*   332: "lineBottomType" */
     275L,	/*   333: "bWMode" */
      91L,	/*   334: "fTopLine" */
      33L,	/*   335: "fIsButton" */
     335L,	/*   336: "lineBottomMiterLimit" */
     103L,	/*   337: "lineRightFillShape" */
     440L,	/*   338: "lineTopColorExtMod" */
     119L,	/*   339: "fLockAdjustHandles" */
     127L,	/*   340: "fillWidth" */
     284L,	/*   341: "fCameFromImgDummy" */
     423L,	/*   342: "lineRightMiterLimit" */
       8L,	/*   343: "fillBlipName" */
      29L,	/*   344: "fIsBullet" */
     338L,	/*   345: "lineBottomStartArrowWidth" */
      30L,	/*   346: "fFlipV" */
     210L,	/*   347: "gtextFShrinkFit" */
     118L,	/*   348: "fLockText" */
     276L,	/*   349: "f3DOK" */
     107L,	/*   350: "fTopInsetPen" */
     433L,	/*   351: "lineTopBackColorExtCMY" */
      46L,	/*   352: "fOleIcon" */
     278L,	/*   353: "fRecolorFillAsPicture" */
     466L,	/*   354: "shadowHighlightExtMod" */
     141L,	/*   355: "fillRectTop" */
     467L,	/*   356: "pictureTransparent" */
     179L,	/*   357: "fRelFlipH" */
      18L,	/*   358: "shapeType" */
     178L,	/*   359: "fRelChangePage" */
     236L,	/*   360: "c3DYViewpoint" */
       6L,	/*   361: "gtextFont" */
     426L,	/*   362: "lineRightStartArrowWidth" */
     157L,	/*   363: "geoBottom" */
     387L,	/*   364: "lineLeftEndArrowLength" */
     215L,	/*   365: "gtextFItalic" */
     113L,	/*   366: "fLockRotation" */
      74L,	/*   367: "lineStartArrowhead" */
     342L,	/*   368: "lineBottomWidth" */
      20L,	/*   369: "rotation" */
     172L,	/*   370: "relRotation" */
     155L,	/*   371: "geoTop" */
     307L,	/*   372: "fillColorExt" */
     142L,	/*   373: "fillRectRight" */
      12L,	/*   374: "posrelv" */
     373L,	/*   375: "lineColumnType" */
     270L,	/*   376: "lineOpacity" */
     273L,	/*   377: "cropFromTop" */
     315L,	/*   378: "lineBottomBackColor" */
     217L,	/*   379: "gtextFShadow" */
     422L,	/*   380: "lineRightFillWidth" */
     282L,	/*   381: "fScriptAnchor" */
      56L,	/*   382: "scaleText" */
     429L,	/*   383: "lineRightType" */
     395L,	/*   384: "lineLeftMiterLimit" */
      45L,	/*   385: "fNoLineDrawDash" */
     247L,	/*   386: "fc3DConstrainRotation" */
     202L,	/*   387: "cxk" */
     146L,	/*   388: "fFilled" */
     203L,	/*   389: "cxstyle" */
      48L,	/*   390: "fOneD" */
     161L,	/*   391: "adjust4Value" */
       0L,	/*   392: "pVerticies" */
     451L,	/*   393: "lineTopMiterLimit" */
     383L,	/*   394: "lineLeftColorExtK" */
     349L,	/*   395: "lineColumnBackColorExtCMY" */
     159L,	/*   396: "adjust2Value" */
     130L,	/*   397: "fillFocus" */
      26L,	/*   398: "hspNext" */
     283L,	/*   399: "fFakeMaster" */
     396L,	/*   400: "lineLeftOpacity" */
       5L,	/*   401: "gtextUNICODE" */
      22L,	/*   402: "dyWrapDistTop" */
     296L,	/*   403: "c3DShininess" */
     245L,	/*   404: "fc3DKeyHarsh" */
     168L,	/*   405: "relLeft" */
     216L,	/*   406: "gtextFUnderline" */
      27L,	/*   407: "xLimo" */
      79L,	/*   408: "lineEndArrowLength" */
     234L,	/*   409: "c3DRotationCenterZ" */
     368L,	/*   410: "lineColumnOpacity" */
      81L,	/*   411: "lineFillDztype" */
     306L,	/*   412: "c3DExtrusionColorExtMod" */
     213L,	/*   413: "gtextFDxMeasure" */
     198L,	/*   414: "fShadowOK" */
      40L,	/*   415: "fHitTestFill" */
      47L,	/*   416: "fOnDblClickNotify" */
     386L,	/*   417: "lineLeftDashing" */
     379L,	/*   418: "lineLeftBackColorExtMod" */
     425L,	/*   419: "lineRightStartArrowLength" */
      90L,	/*   420: "fLeftLine" */
     305L,	/*   421: "c3DExtrusionColorExtK" */
     411L,	/*   422: "lineRightColorExtK" */
     330L,	/*   423: "lineBottomEndCapStyle" */
     227L,	/*   424: "fc3DMetallic" */
     364L,	/*   425: "lineColumnFillDztype" */
     427L,	/*   426: "lineRightStartArrowhead" */
     436L,	/*   427: "lineTopColor" */
     238L,	/*   428: "c3DKeyX" */
     361L,	/*   429: "lineColumnEndArrowhead" */
     194L,	/*   430: "shadowOriginX" */
     214L,	/*   431: "gtextFBold" */
     454L,	/*   432: "lineTopStartArrowWidth" */
     105L,	/*   433: "fInsetPen" */
     101L,	/*   434: "lineLeftFillShape" */
      49L,	/*   435: "fPreferRelativeSize" */
      23L,	/*   436: "dxWrapDistRight" */
     251L,	/*   437: "spcoa" */
       2L,	/*   438: "pSegmentInfo" */
     430L,	/*   439: "lineRightWidth" */
     285L,	/*   440: "c3DAmbientIntensity" */
     398L,	/*   441: "lineLeftStartArrowWidth" */
     132L,	/*   442: "fillToTop" */
      19L,	/*   443: "ShapeType" */
     444L,	/*   444: "lineTopEndArrowWidth" */
     359L,	/*   445: "lineColumnEndArrowLength" */
      11L,	/*   446: "posh" */
     112L,	/*   447: "lineCrMod" */
     280L,	/*   448: "ccol" */
     150L,	/*   449: "fFillShadeShapeOK" */
     299L,	/*   450: "c3DSpecularAmt" */
       4L,	/*   451: "wzDescription" */
     402L,	/*   452: "lineLeftWidth" */
     318L,	/*   453: "lineBottomBackColorExtK" */
     185L,	/*   454: "shadowSecondOffsetX" */
     313L,	/*   455: "lineBackColorExtK" */
     170L,	/*   456: "relRight" */
     360L,	/*   457: "lineColumnEndArrowWidth" */
     297L,	/*   458: "c3DSkewAmount" */
     434L,	/*   459: "lineTopBackColorExtK" */
     231L,	/*   460: "c3DRotationAxisY" */
     370L,	/*   461: "lineColumnStartArrowWidth" */
     308L,	/*   462: "fillColorExtCMY" */
     298L,	/*   463: "c3DSkewAngle" */
     424L,	/*   464: "lineRightOpacity" */
      69L,	/*   465: "lineFillWidth" */
     229L,	/*   466: "fc3DLightFace" */
     437L,	/*   467: "lineTopColorExt" */
      99L,	/*   468: "lineFillShape" */
     413L,	/*   469: "lineRightCrMod" */
     475L,	/*   470: "sizerelh" */
      80L,	/*   471: "lineEndCapStyle" */
     372L,	/*   472: "lineColumnStyle" */
     152L,	/*   473: "fillBackColor" */
     403L,	/*   474: "lineRightBackColor" */
     325L,	/*   475: "lineBottomCrMod" */
     333L,	/*   476: "lineBottomFillHeight" */
     352L,	/*   477: "lineColumnColor" */
    };

/* The hash function */

unsigned long docRtfShapePropHash(const unsigned char * key, int len )
{
    const ub4 initlev= 0x7b82e09b;
    ub4 rsl= 0;
    ub4 val = utilJenkinsHash( key, len, initlev );
    rsl = ((val>>25)^tab[val&0xff]);
    return rsl;
}

int docRtfShapePropIndex( const unsigned char * key, int len )
{
    ub4 rsl= docRtfShapePropHash( key, len );

    if  ( rsl >= sizeof(mapBack)/sizeof(short) )
	{ return -1;	}

    return mapBack[rsl];
}

