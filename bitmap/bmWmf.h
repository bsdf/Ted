/************************************************************************/
/*									*/
/*  Windows Meta File and GDI specific definitions.			*/
/*									*/
/************************************************************************/

#   ifndef		BM_WMF_H	/*	{{	*/
#   define		BM_WMF_H

#   include	"bmcolor.h"
#   include	<geo2DInteger.h>

typedef enum
    {
    META_EOF = 0x0000,
    META_REALIZEPALETTE = 0x0035,
    META_SETPALENTRIES = 0x0037,
    META_SETBKMODE = 0x0102,
    META_SETMAPMODE = 0x0103,
    META_SETROP2 = 0x0104,
    META_SETRELABS = 0x0105,
    META_SETPOLYFILLMODE = 0x0106,
    META_SETSTRETCHBLTMODE = 0x0107,
    META_SETTEXTCHAREXTRA = 0x0108,
    META_RESTOREDC = 0x0127,
    META_RESIZEPALETTE = 0x0139,
    META_DIBCREATEPATTERNBRUSH = 0x0142,
    META_SETLAYOUT = 0x0149,
    META_SETBKCOLOR = 0x0201,
    META_SETTEXTCOLOR = 0x0209,
    META_OFFSETVIEWPORTORG = 0x0211,
    META_LINETO = 0x0213,
    META_MOVETO = 0x0214,
    META_OFFSETCLIPRGN = 0x0220,
    META_FILLREGION = 0x0228,
    META_SETMAPPERFLAGS = 0x0231,
    META_SELECTPALETTE = 0x0234,
    META_POLYGON = 0x0324,
    META_POLYLINE = 0x0325,
    META_SETTEXTJUSTIFICATION = 0x020A,
    META_SETWINDOWORG = 0x020B,
    META_SETWINDOWEXT = 0x020C,
    META_SETVIEWPORTORG = 0x020D,
    META_SETVIEWPORTEXT = 0x020E,
    META_OFFSETWINDOWORG = 0x020F,
    META_SCALEWINDOWEXT = 0x0410,
    META_SCALEVIEWPORTEXT = 0x0412,
    META_EXCLUDECLIPRECT = 0x0415,
    META_INTERSECTCLIPRECT = 0x0416,
    META_ELLIPSE = 0x0418,
    META_FLOODFILL = 0x0419,
    META_FRAMEREGION = 0x0429,
    META_ANIMATEPALETTE = 0x0436,
    META_TEXTOUT = 0x0521,
    META_POLYPOLYGON = 0x0538,
    META_EXTFLOODFILL = 0x0548,
    META_RECTANGLE = 0x041B,
    META_SETPIXEL = 0x041F,
    META_ROUNDRECT = 0x061C,
    META_PATBLT = 0x061D,
    META_SAVEDC = 0x001E,
    META_PIE = 0x081A,
    META_STRETCHBLT = 0x0B23,
    META_ESCAPE = 0x0626,
    META_INVERTREGION = 0x012A,
    META_PAINTREGION = 0x012B,
    META_SELECTCLIPREGION = 0x012C,
    META_SELECTOBJECT = 0x012D,
    META_SETTEXTALIGN = 0x012E,
    META_ARC = 0x0817,
    META_CHORD = 0x0830,
    META_BITBLT = 0x0922,
    META_EXTTEXTOUT = 0x0a32,
    META_SETDIBTODEV = 0x0d33,
    META_DIBBITBLT = 0x0940,
    META_DIBSTRETCHBLT = 0x0b41,
    META_STRETCHDIB = 0x0f43,
    META_DELETEOBJECT = 0x01f0,
    META_CREATEPALETTE = 0x00f7,
    META_CREATEPATTERNBRUSH = 0x01F9,
    META_CREATEPENINDIRECT = 0x02FA,
    META_CREATEFONTINDIRECT = 0x02FB,
    META_CREATEBRUSHINDIRECT = 0x02FC,
    META_CREATEREGION = 0x06FF
    } WmfRecordType;

/************************************************************************/
/*									*/
/*  1)  Various MapModes: Refer to petzold for an explanation.		*/
/*  2)  StrechBlt() modes.						*/
/*  3)  Font family styles.						*/
/*  4)  Polygon Fill Modes.						*/
/*									*/
/************************************************************************/

/*  1  */
#   define	MM_TEXT				1
#   define	MM_LOMETRIC			2
#   define	MM_HIMETRIC			3
#   define	MM_LOENGLISH			4
#   define	MM_HIENGLISH			5
#   define	MM_TWIPS			6
#   define	MM_ISOTROPIC			7
#   define	MM_ANISOTROPIC			8

/*  2  */
#   define	STRETCH_BLT_BLACKONWHITE	1
#   define	STRETCH_BLT_WHITEONBLACK	2
#   define	STRETCH_BLT_COLORONCOLOR	3

/*  3  */
#   define	FF_DONTCARE			0x00
#   define	FF_ROMAN			0x10
#   define	FF_SWISS			0x20
#   define	FF_MODERN			0x30
#   define	FF_SCRIPT			0x40
#   define	FF_DECORATIVE			0x50

/*  4  */
#   define	ALTERNATE			1
#   define	WINDING				2

/*  5  */
#   define	TRANSPARENT			1
#   define	OPAQUE				2

/************************************************************************/
/*									*/
/*  Implementation of a metafile 'Object'				*/
/*									*/
/************************************************************************/

#   define TA_NOUPDATECP			0x0000
#   define TA_UPDATECP				0x0001
#   define TA_LEFT				0x0000
#   define TA_RIGHT				0x0002
#   define TA_CENTER				0x0006
#   define TA_TOP				0x0000
#   define TA_BOTTOM				0x0008
#   define TA_BASELINE				0x0018

typedef struct LogicalPen
    {
    int		lpStyle;
#		define		PS_SOLID	0
#		define		PS_DASH		1
#		define		PS_DOT		2
#		define		PS_DASHDOT	3
#		define		PS_DASHDOTDOT	4
#		define		PS_NULL		5
#		define		PS_INSIDEFRAME	6
    int		lpWidth;
    RGB8Color	lpColor;
    } LogicalPen;

typedef struct LogicalPalette
    {
    int		lpVersion;
    int		lpColorCount;
    } LogicalPalette;

#   define	MFtypeFREE		0
#   define	MFtypeBRUSH		1
#   define	MFtypePEN		2
#   define	MFtypeFONT		3
#   define	MFtypePALETTE		4
#   define	MFtypePATTERNBRUSH	5
#   define	MFtypeREGION		6

#   define	mfoLogicalBrush		mfoSpecific.mfsLogicalBrush
#   define	mfoLogicalPen		mfoSpecific.mfsLogicalPen
#   define	mfoLogicalFont		mfoSpecific.mfsLogicalFont
#   define	mfoPatternBrush		mfoSpecific.mfsPatternBrush

/************************************************************************/
/*									*/
/*  The header of a WMF metafile.					*/
/*									*/
/************************************************************************/

typedef struct WmfHeader
    {
    int				wh_fileType;
    int				wh_headerSize;
    int				wh_windowsVersion;
    long			wh_fileSize;
    int				wh_objectCount;
    long			wh_maxRecordSize;
    int				wh_parameterCount;
    } WmfHeader;

typedef struct WmfFileHeader
    {
    unsigned long		wfh_key;
    unsigned int		wfh_handle;
    DocumentRectangle		wfhBounds;
    unsigned int		wfhUnitsPerInch;

    unsigned long		wfh_reserved;
    unsigned int		wfh_checksum;
    } WmfFileHeader;

#   endif	/*	BM_WMF_H		}}	*/
