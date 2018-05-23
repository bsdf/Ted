/************************************************************************/
/*									*/
/*  Windows Meta File specific definitions.				*/
/*									*/
/************************************************************************/

#   ifndef		DRAW_WIN_META_IMPL_H
#   define		DRAW_WIN_META_IMPL_H

#   include	<bitmap.h>
#   include	<utilDocFont.h>
#   include	<sioGeneral.h>

#   include	<bmWmf.h>
#   include	<utilMemoryBuffer.h>
#   include	<textConverterImpl.h>
#   include	<geo2DInteger.h>
#   include	<geoAffineTransform.h>
#   include	<utilDocFontList.h>
#   include	<utilMatchFont.h>
#   include	<psPostScriptFontList.h>
#   include	"drawMetafile.h"

struct DrawingSurface;

typedef struct LogicalBrush
    {
    int				lbStyle;
#				define		BS_SOLID	0
#				define		BS_NULL		1
#				define		BS_HOLLOW	BS_NULL
#				define		BS_HATCHED	2
#				define		BS_PATTERN	3
#				define		BS_INDEXED	4
#				define		BS_DIBPATTERN	5
    RGB8Color			lbColor;
    int				lbHatch;
#				define HS_HORIZONTAL       0
#				define HS_VERTICAL         1
#				define HS_FDIAGONAL        2
#				define HS_BDIAGONAL        3
#				define HS_CROSS            4
#				define HS_DIAGCROSS        5

    struct DrawingSurface *	lbTilePixmap;
    int				lbTileNumber;
    } LogicalBrush;

typedef struct PatternBrush
    {
    int				pbType;		/*  == 5	*/
    int				pbUsage;	/*  == 0	*/
    RasterImage *		pbAbi;
    struct DrawingSurface *	pbTilePixmap;
    int				pbTileNumber;
    } PatternBrush;

typedef struct LogicalFont
    {
    int			lfHeight;
    int			lfWidth;
    int			lfEscapement;
    int			lfOrientation;
    int			lfWeight;
    unsigned char	lfItalic;
    unsigned char	lfUnderline;
    unsigned char	lfStrikeOut;
    unsigned char	lfCharSet;
    unsigned char	lfOutPrecision;
    unsigned char	lfClipPrecision;
    unsigned char	lfQuality;
    unsigned char	lfPitchAndFamily;
#   define		LF_CHARS	32
#   define		LF_BYTES	(3*LF_CHARS)
    char		lfFaceNameUtf8[LF_BYTES+1];

    /************************************/
    /*  Implementation			*/
    /************************************/
    TextAttribute	lfTextAttribute;
    int			lfPrivateFont;
    } LogicalFont;

typedef union MetafileSpecific
    {
    LogicalBrush	mfsLogicalBrush;
    LogicalPen		mfsLogicalPen;
    LogicalFont		mfsLogicalFont;
    PatternBrush	mfsPatternBrush;
    } MetafileSpecific;

typedef struct MetaFileObject
    {
    int			mfoType;
    MetafileSpecific	mfoSpecific;
    } MetaFileObject;

typedef enum MacPictBrushHandle
    {
    MACPICThandlePAINT_BRUSH,
    MACPICThandleFILL_BRUSH,
    MACPICThandleERASE_BRUSH,
    MACPICThandleDUMMY_BRUSH,	/* dummy that does not paint */

    MACPICThandleDUMMY_PEN,	/* dummy that does not draw */
    MACPICThandleFRAME_PEN,

    MACPICThandleFONT,

    MACPICThandle_NONE,
    MACPICThandle_COUNT
    } MacPictBrushHandle;

/************************************************************************/

struct DeviceContext;

/************************************************************************/

typedef int (*MetaGetPoints)(		struct DeviceContext *	dc,
					int			count,
					SimpleInputStream *	sis );

/************************************************************************/

typedef int (*MetaDrawRasterImage)(
				struct DeviceContext *		dc,
				void *				through,
				const RasterImage *		abi,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest );

typedef int (*MetaCleanObject)(		struct DeviceContext *	dc,
					void *			through,
					MetaFileObject *	mfo );

typedef int (*MetaSelectPenObject)(	struct DeviceContext *	dc,
					void *			through,
					LogicalPen *		lp );

typedef int (*MetaSelectBrushObject)(	struct DeviceContext *	dc,
					void *			through,
					LogicalBrush  *		lb );

typedef int (*MetaSelectFontObject)(	struct DeviceContext *	dc,
					void *			through,
					LogicalFont  *		lf );

typedef int (*MetaSelectPatternBrushObject)(
					struct DeviceContext *	dc,
					void *			through,
					PatternBrush  *		pb );

typedef int (*MetaDrawPolyPolygon)(	struct DeviceContext *	dc,
					void *			through,
					int			polyCount,
					const int *		pointCounts,
					const Point2DI *	points,
					int			fillInside,
					int			drawBorder,
					int			closePath );
typedef int (*MetaDrawPolyBezier)(	struct DeviceContext *	dc,
					void *			through,
					int			pointCounts,
					const Point2DI *	points,
					int			hasStart,
					int			fillInside,
					int			drawBorder,
					int			closePath );

typedef int (*MetaDrawRectangle)(
				struct DeviceContext *		dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				fillInside,
				int				drawBorder );

typedef int (*MetaDrawRoundedRectangle)(
				struct DeviceContext *		dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				dx,
				int				dy,
				int				fillInside,
				int				drawBorder );

typedef int (*MetaDrawString)(		struct DeviceContext *	dc,
					void *			through,
					int			x,
					int			y,
					const MemoryBuffer *	text );

typedef int (*MetaPatBlt)(	struct DeviceContext *		dc,
				void *				through,
				long				rop,
				const DocumentRectangle *	drFill );

typedef int (*MetaDrawArc)(	struct DeviceContext *		dc,
				void *				through,
				const DocumentRectangle *	drArc,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder );

typedef int (*MetaSetColor)(		struct DeviceContext *	dc,
					void *			through,
					int			r,
					int			g,
					int			b );

typedef int (*MetaSetPolyFillMode)(	struct DeviceContext *	dc,
					void *			through,
					int			mode );

typedef int (*MetaBeginPath)(		struct DeviceContext *	dc,
					void *			through );

typedef int (*MetaClosePath)(		struct DeviceContext *	dc,
					void *			through );

typedef int (*MetaEndPath)(		struct DeviceContext *	dc,
					void *			through );

typedef int (*MetaStrokePath)(		struct DeviceContext *	dc,
					void *			through );

typedef int (*MetaFillPath)(		struct DeviceContext *	dc,
					void *			through );

/************************************************************************/
/*									*/
/*  Refer to Petzold, Charles: "Programming Windows 95", Microsoft	*/
/*  Press, Redmond, WA, 1996 pp138..					*/
/*									*/
/*  Terminology is extremely confusing: The window is the system of	*/
/*  'logical' user coordinates. The viewport is the sytem of 'hard'	*/
/*  device coordinates. Viewport=Device coordinates usually are in	*/
/*  pixels.								*/
/*									*/
/************************************************************************/

typedef struct DeviceCoordinateTransform
    {
    					/* The MS-Windows window */
    DocumentRectangle			dctDeviceRect;
    					/* Logical coordinates as used */
    DocumentRectangle			dctLogicalRect;
    					/* Output coordinates for X11 or PS */

    AffineTransform2D			dctDeviceToOutput;
    AffineTransform2D			dctLogicalToOutput;
    struct DeviceCoordinateTransform *	dctPushed;
    } DeviceCoordinateTransform;

# define DC_xViewport(x,y,dc) (AT2_X((x),(y),&((dc)->dcTransform.dctLogicalToOutput)))
# define DC_yViewport(x,y,dc) (AT2_Y((x),(y),&((dc)->dcTransform.dctLogicalToOutput)))

    /*HACK*/
# define DC_wViewport(w,dc) (DC_xViewport((w),0,(dc))-DC_xViewport(0,0,(dc)))
# define DC_hViewport(h,dc) (DC_yViewport(0,(h),(dc))-DC_yViewport(0,0,(dc)))

typedef struct DeviceContext
    {
    int					dcMapMode;
    int					dcDrawBorders;
    int					dcFillInsides;
    int					dcROP2;

    DocumentRectangle			dcClipRect;

    int					dcFillHatched;
    int					dcFillPattern;

    DocumentFontList			dcFontList;
    TextConverter			dcTextConverter;
    MemoryBuffer			dcCollectedText;

    LogicalPen				dcPen;
    LogicalBrush			dcBrush;
    LogicalFont				dcFont;
    int					dcCurrentBrushObject;

    RGB8Color				dcBkColor;
    RGB8Color				dcTextColor;
    int					dcBkMode;
    int					dcPolyFillMode;

    RGB8Color				dcColorSet;
    TextAttribute			dcTextAttributeSet;
    LogicalPen				dcPenSet;

    int					dcExtraTextSpacing;
    int					dcJustificationAmount;
    int					dcJustificationSpaces;
    int					dcTextAlignment;
    long				dcStretchBltMode;

    int					dcX;
    int					dcY;

    DocumentRectangle			dcOutputRect;
    DocumentRectangle			dcTwipsRect;
    DeviceCoordinateTransform		dcTransform;

    MetaFileObject *			dcObjects;
    int					dcObjectCount;

    Point2DI *				dcPoints;
    int					dcPointCount;
    int *				dcCounts;

    const AfmFontInfo *			dcAfi;
    int					dcFontEncoding;

    int					dcMacPictVersion;

    int					dcMacPictPenMode;
    int					dcMacPictPenIsSolid;
    int					dcMacPictFillIsSolid;
    int					dcMacPictBackIsSolid;

    unsigned char			dcMacPictPenPattern[8];
    unsigned char			dcMacPictFillPattern[8];
    unsigned char			dcMacPictBackPattern[8];

    RGB8Color				dcMacPictPenSolid;
    RGB8Color				dcMacPictFillSolid;
    RGB8Color				dcMacPictBackSolid;

    DocumentRectangle			dcMacPictOvalRect;
    DocumentRectangle			dcMacPictPolyRect;
    DocumentRectangle			dcMacPictRectRect;
    DocumentRectangle			dcMacPictRRectRect;
    DocumentRectangle			dcMacPictArcRect;

    int					dcMacPictRRectOvalWide;
    int					dcMacPictRRectOvalHigh;

    int					dcMacPictArcA0;
    int					dcMacPictArcA1;
    int					dcMacPictArcXs;
    int					dcMacPictArcYs;
    int					dcMacPictArcXe;
    int					dcMacPictArcYe;

    RGB8Color				dcMacPictForeColor;
    RGB8Color				dcMacPictBackColor;
    int					dcMacPictStippleSet;

    unsigned int			dcMacPictRedMax;
    unsigned int			dcMacPictGreenMax;
    unsigned int			dcMacPictBlueMax;

    const MetafilePlayer *		dcPlayer;

    MetaCleanObject			dcCleanObject;
    MetaDrawRasterImage			dcDrawRasterImage;
    MetaSelectPenObject			dcSelectPenObject;
    MetaSelectBrushObject		dcSelectBrushObject;
    MetaSelectFontObject		dcSelectFontObject;
    MetaSelectPatternBrushObject	dcSelectPatternBrushObject;
    MetaDrawPolyPolygon			dcDrawPolyPolygon;
    MetaDrawPolyBezier			dcDrawPolyBezier;
    MetaDrawRoundedRectangle		dcDrawRoundedRectangle;
    MetaDrawString			dcDrawString;
    MetaPatBlt				dcPatBlt;
    MetaDrawArc				dcDrawArc;
    MetaDrawArc				dcDrawPie;
    MetaDrawArc				dcDrawChord;
    MetaDrawRectangle			dcDrawEllipse;
    MetaSetColor			dcSetTextColor;
    MetaSetColor			dcSetBkColor;
    MetaSetPolyFillMode			dcSetPolyFillMode;

    MetaBeginPath			dcBeginPath;
    MetaClosePath			dcClosePath;
    MetaEndPath				dcEndPath;
    MetaStrokePath			dcStrokePath;
    MetaFillPath			dcFillPath;
    } DeviceContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appMetaCleanDeviceContext(	DeviceContext *		dc,
					void *			through );

int appMetaInitDeviceContext(	DeviceContext *			dc,
				const MetafilePlayer *		player,
				int				objectCount,
				const DocumentRectangle *	drLogical,
				int				mapMode,
				int				outputWide,
				int				outputHigh,
				int				twipsWide,
				int				twipsHigh );

extern void appMetaCleanObject( MetaFileObject *	mfo );

extern int appWinMetaRememberFontInList(DeviceContext *		dc,
					TextAttribute *		ta,
					const LogicalFont *	lf );

extern int appMetaReadBitmapImage(	int *			pSkip,
					RasterImage **	pAbi,
					SimpleInputStream *	sis,
					int			expectBytes );

extern int appMetaSetTextAlign(	DeviceContext *		dc,
				int			textAlignment );

extern int appMetaSaveDC(		DeviceContext *		dc );
extern int appMetaRestoreDC(		DeviceContext *		dc );

extern int appMetaExcludeClipRect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaIntersectClipRect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appWinMetaGetPoints16(	DeviceContext *		dc,
					int			count,
					SimpleInputStream *	sis );
extern int appWinMetaGetPoints32(	DeviceContext *		dc,
					int			count,
					SimpleInputStream *	sis );

extern int appMetaPlayWmf(		DeviceContext *		dc,
					void *			through );

extern int appMetaPlayEmf(		DeviceContext *		dc,
					void *			through );

extern int appMetaDrawRasterImage(
				SimpleInputStream *		sis,
				void *				through,
				int				expectBytes,
				DeviceContext *			dc,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest );

extern int appWinMetaSelectObject(	DeviceContext *		dc,
					void *			through,
					int			ob );

extern int appWinMetaDrawRectangle(
				DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				fillInside,
				int				drawBorder );

extern int appWinMetaDrawEllipse(
				DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				fillInside,
				int				drawBorder );

extern void appWinMetaTransformRectangle(
				DocumentRectangle *		drOutput,
				DeviceContext *			dc,
				const DocumentRectangle *	drLogical );

extern void appWinMetaUpdateTransform(	DeviceContext *		dc );

extern int appWinMetaOutputSize(	DeviceContext *		dc,
					int			logicalSize );

extern int appWinMetaReadUtf16Text(	DeviceContext *		dc,
					int			expectChars,
					SimpleInputStream *	sis );

extern int appWinMetaReadLegacyText(	DeviceContext *		dc,
					int			count,
					int			expectBytes,
					SimpleInputStream *	sis );

extern int appMacPictPlayPict(		DeviceContext *		dc,
					void *			through );

#   endif	/*	DRAW_WIN_META_IMPL_H	*/
