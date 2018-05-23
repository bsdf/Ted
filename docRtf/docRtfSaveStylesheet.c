/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"

/************************************************************************/
/*									*/
/*  Write a Style Sheet.						*/
/*									*/
/************************************************************************/

void docRtfWriteStyleSheet(	RtfWriter *		rwc,
				const DocumentStyleSheet *	dss )
    {
    int				i;

    docRtfWriteDestinationBegin( rwc, "stylesheet" );
    docRtfWriteNextLine( rwc );

    for ( i= 0; i < dss->dssStyleCount; i++ )
	{
	const DocumentStyle *	ds= dss->dssStyles[i];

	if  ( ! ds || ds->dsStyleNumber < 0 )
	    { continue;	}

	switch( ds->dsLevel )
	    {
	    case DOClevSPAN:
		docRtfWriteArgDestinationBegin( rwc, "*\\cs",
							ds->dsStyleNumber );
		docRtfSaveTextAttribute( rwc,
				&(ds->dsTextMask), &(ds->dsTextAttribute) );
		break;

	    case DOClevPARA:
		if  ( ds->dsStyleNumber > 0 )
		    {
		    docRtfWriteArgDestinationBegin( rwc, "s",
							ds->dsStyleNumber );
		    }
		else{
		    docRtfWriteDestinationBegin( rwc, "" );
		    }

		docRtfSaveParagraphProperties( rwc, &(ds->dsParaMask),
						&(ds->dsParaProps) );
		docRtfSaveTextAttribute( rwc,
				&(ds->dsTextMask), &(ds->dsTextAttribute) );
		break;

	    case DOClevSECT:
		docRtfWriteArgDestinationBegin( rwc, "ds",
							ds->dsStyleNumber );

		docRtfSaveSectionProperties( rwc,
			    &(ds->dsSectMask), &(ds->dsSectProps) );
		docRtfSaveParagraphProperties( rwc,
			    &(ds->dsParaMask), &(ds->dsParaProps) );
		docRtfSaveTextAttribute( rwc,
			    &(ds->dsTextMask), &(ds->dsTextAttribute) );
		break;

	    case DOClevROW:
		docRtfWriteArgDestinationBegin( rwc, "*\\ts",
							ds->dsStyleNumber );
		break;

	    default:
		LDEB(ds->dsLevel); continue;
	    }

	if  ( ds->dsBasedOn >= 0 )
	    { docRtfWriteArgTag( rwc, "sbasedon", ds->dsBasedOn ); }
	if  ( ds->dsNext >= 0 )
	    { docRtfWriteArgTag( rwc, "snext", ds->dsNext ); }
	if  ( ds->dsLink >= 0 )
	    { docRtfWriteTag( rwc, "slink" ); }

	if  ( ds->dsAdditive )
	    { docRtfWriteTag( rwc, "additive" ); }
	if  ( ds->dsAutoupd )
	    { docRtfWriteTag( rwc, "sautoupd" ); }
	if  ( ds->dsHidden )
	    { docRtfWriteTag( rwc, "shidden" ); }
	if  ( ds->dsLocked )
	    { docRtfWriteTag( rwc, "slocked" ); }
	if  ( ds->dsSemiHidden )
	    { docRtfWriteTag( rwc, "ssemihidden" ); }
	if  ( ds->dsPersonal )
	    { docRtfWriteTag( rwc, "spersonal" ); }

	docRtfWriteDocEncodedString( rwc, ds->dsName, strlen( ds->dsName ) );
	docRtfWriteSemicolon( rwc );

	docRtfWriteDestinationEnd( rwc );
	docRtfWriteNextLine( rwc );
	}

    docRtfWriteDestinationEnd( rwc );
    docRtfWriteNextLine( rwc );

    return;
    }

