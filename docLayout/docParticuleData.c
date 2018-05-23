/************************************************************************/
/*									*/
/*  Additional information for the layout of a line of text. Some of	*/
/*  the fields are used for scratch in the process.			*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stdlib.h>

#   include	"docParticuleData.h"

#   include	<appDebugon.h>

void docInitParticuleData(		ParticuleData *	pd )
    {
    pd->pdX0= 0;
    pd->pdTwipsWide= 0;
    pd->pdDecWidth= 0;

    geoInitRectangle( &(pd->pdVisibleBBox) );

    pd->pdLeftBorderWidth= 0;
    pd->pdRightBorderWidth= 0;

    pd->pdTabKind= -1;
    pd->pdTabNumber= -1;
    pd->pdTabPosition= 0;
    pd->pdAfi= (const AfmFontInfo *)0;
    pd->pdFlags= 0x0;

    pd->pdVisiblePixels= 0;
    pd->pdWhiteUnits= 0;
    pd->pdCorrectBy= 0;
    }

int docPsClaimParticuleData(	int			count,
				ParticuleData **	pParticuleData )
    {
    static ParticuleData *	PSPrintGeometry;
    static int			PSPrintGeometryCount;

    if  ( count > PSPrintGeometryCount )
	{
	ParticuleData *	fresh;

	fresh= (ParticuleData *)realloc( PSPrintGeometry,
				2* count* sizeof( ParticuleData ) );
	if  ( ! fresh )
	    { LXDEB(count,fresh); return -1;	}

	PSPrintGeometry= fresh;
	}

    *pParticuleData= PSPrintGeometry; return 0;
    }

