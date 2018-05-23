/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	<docObject.h>
#   include	<docObjectProperties.h>
#   include	"docObjectRect.h"
#   include	"docShape.h"

/************************************************************************/
/*									*/
/*  Find the twips rectangle on the page for an object.			*/
/*									*/
/************************************************************************/

void docObjectGetPageRect(	DocumentRectangle *		drDest,
				const InsertedObject *		io,
				int				x0Twips,
				int				baselineTwips )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	docPlaceRootShapeRect( drDest, &(io->ioDrawingShape->dsShapeProperties),
						x0Twips, baselineTwips );
	}
    else{
	int	twipsWide= ( pip->pipScaleXUsed* pip->pipTwipsWide )/ 100;
	int	twipsHigh= ( pip->pipScaleYUsed* pip->pipTwipsHigh )/ 100;
	
	drDest->drX0= x0Twips;
	drDest->drY1= baselineTwips;

	drDest->drX1= drDest->drX0+ twipsWide;
	drDest->drY0= drDest->drY1- twipsHigh;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the twips rectangle of the object itself.			*/
/*									*/
/************************************************************************/

void docObjectGetSourceRect(	DocumentRectangle *		drSrc,
				const PictureProperties *	pip )
    {
    drSrc->drX0= 0;
    drSrc->drY0= 0;
    drSrc->drX1= pip->pipTwipsWide- 1;
    drSrc->drY1= pip->pipTwipsHigh- 1;

    return;
    }
