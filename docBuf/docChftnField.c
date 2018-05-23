/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>
#   include	<stdio.h>

#   include	<utilBase26.h>
#   include	<utilRoman.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docDocumentNote.h>
#   include	<docNotes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Format the value of a 'chftn' field.				*/
/*									*/
/*  I.E. the text of the reference to the note in the body of the text,	*/
/*	or the identical sign at the begnning of the note.		*/
/*									*/
/************************************************************************/

int docFormatChftnField(	MemoryBuffer *			mbResult,
				const FootEndNotesProperties *	fep,
				const NoteProperties *		np,
				int				noteNumber )
    {
    char			scratch[100+1];
    const NotesProperties *	nsp;

    utilEmptyMemoryBuffer( mbResult );

    if  ( ! np->npAutoNumber )
	{
	if  ( utilCopyMemoryBuffer( mbResult, &(np->npFixedText) ) )
	    { LDEB(1); return -1;	}
	return 0;
	}

    switch( np->npTreeType )
	{
	case DOCinFOOTNOTE:
	    nsp= &(fep->fepFootnotesProps);
	    break;

	case DOCinENDNOTE:
	    nsp= &(fep->fepEndnotesProps);
	    break;

	default:
	    SDEB(docTreeTypeStr(np->npTreeType)); return -1;
	}

    if  ( nsp->npRestart == FTN_RST_CONTINUOUS )
	{ noteNumber += nsp->npStartNumber- 1;	}

    switch( nsp->npNumberStyle )
	{
	case FTNstyleNAR:
	    sprintf( scratch, "%d", noteNumber+ 1 );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FTNstyleNALC:
	    if  ( utilBase26String( scratch, sizeof(scratch)-1,
							noteNumber+ 1, 0 ) )
		{ LDEB(noteNumber); return -1;	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FTNstyleNAUC:
	    if  ( utilBase26String( scratch, sizeof(scratch)-1,
							noteNumber+ 1, 1 ) )
		{ LDEB(noteNumber); return -1;	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FTNstyleNRLC:
	    if  ( utilRomanString( scratch, sizeof(scratch)-1,
							noteNumber+ 1, 0 ) )
		{ sprintf( scratch, "NRLC:%d", noteNumber+ 1 );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FTNstyleNRUC:
	    if  ( utilRomanString( scratch, sizeof(scratch)-1,
							noteNumber+ 1, 1 ) )
		{ sprintf( scratch, "NRUC:%d", noteNumber+ 1 );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FTNstyleNCHI:
	default:
	    LDEB(nsp->npNumberStyle);
	    sprintf( scratch, "(%d)", noteNumber+ 1 );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

int docCalculateChftnFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		dfChftn,
				const RecalculateFields *	rf )
    {
    DocumentNote *		dn;

    dn= docGetNoteOfField( dfChftn, rf->rfDocument );
    if  ( dn )
	{
	const BufferItem *	bodySectNode;

	bodySectNode= docGetBodySectNode( dn->dnDocumentTree.dtRoot,
							    rf->rfDocument );

	if  ( docFormatChftnField( mbResult,
			    &(bodySectNode->biSectNotesProperties),
			    &(dn->dnNoteProperties),
			    dn->dnNoteNumber ) )
	    { LDEB(dn->dnNoteNumber); *pCalculated= 0; return 0; }

	*pCalculated= 1;
	return 0;
	}

    XDEB(dn);
    *pCalculated= 0; return 0;
    }

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

int docRecalculateParaChftnTextParticules(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				struct BufferItem *		paraBi,
				int				part,
				int				partCount,
				DocumentField *			df,
				const RecalculateFields *	rf )
    {
    int			ret;

    TextAttribute	taSet;
    PropertyMask	taSetMask;
    int			taChanged= 0;

    ret= docRecalculateParaStringTextParticules( pCalculated,
			    pPartShift, pStroffShift, paraBi, part, partCount,
			    df, rf );

    if  ( ret )
	{ LDEB(ret); return ret;	}

    docPlainTextAttribute( &taSet, rf->rfDocument );
    utilPropMaskClear( &taSetMask );

    taSet.taSuperSub= TEXTvaSUPERSCRIPT;
    PROPmaskADD( &taSetMask, TApropSUPERSUB );

    if  ( docChangeParticuleAttributes( &taChanged, (PropertyMask *)0,
					rf->rfDocument,
					paraBi, part+ 1,
					part+ 1+ partCount+ *pPartShift,
					&taSet, &taSetMask ) )
	{ LDEB(1);	}

    if  ( taChanged )
	{ *pCalculated= 1;	}

    return ret;
    }
