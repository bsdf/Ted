/************************************************************************/
/*									*/
/*  Additional information for the layout of a line of text. Some of	*/
/*  the fields are used for scratch in the process.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARTICULE_DATA_H
#   define	DOC_PARTICULE_DATA_H

#   include	<psFontInfo.h>
#   include	<geo2DInteger.h>

typedef struct ParticuleData
    {
			    /************************************/
			    /*  Advance in X direction.		*/
			    /************************************/
    int			pdX0;
    int			pdTwipsWide;
    int			pdDecWidth;
			    /************************************/
			    /*  Actually visible.		*/
			    /*  Origin is in ( 0, 0 )		*/
			    /*  representing ( pdX0, baseline )	*/
			    /************************************/
    DocumentRectangle	pdVisibleBBox;
			    /************************************/
			    /*  Borders: Are inside the visible	*/
			    /*  bounding box.			*/
			    /************************************/
    int			pdLeftBorderWidth;
    int			pdRightBorderWidth;

    int			pdTabKind;
    int			pdTabNumber;
    int			pdTabPosition;
    const AfmFontInfo *	pdAfi;
    unsigned char	pdFlags;

    int			pdVisiblePixels;
    int			pdWhiteUnits;
    int			pdCorrectBy;
    } ParticuleData;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitParticuleData(		ParticuleData *	pd );

extern int docPsClaimParticuleData(
				int			count,
				ParticuleData **	pParticuleData );

#   endif	/*  DOC_PARTICULE_DATA_H  */
