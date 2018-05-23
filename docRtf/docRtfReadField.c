/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*  Read fields, bookmarks.						*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	<docField.h>
#   include	<docParaParticules.h>
#   include	<docTreeType.h>
#   include	<docBookmarkField.h>

/************************************************************************/
/*									*/
/*  To ensure proper field/bookmark nesting.. A stack op active		*/
/*  is kept to be flushed at the appropriate transitions. (Field end,	*/
/*  field start, paragraph end.)					*/
/*									*/
/************************************************************************/

typedef struct RtfFieldStackLevel
    {
    DocumentField *		rfslField;
    SelectionScope		rfslSelectionScope;
    EditPosition		rfslStartPosition;
    int				rfslParticule;
    struct RtfFieldStackLevel *	rfslPrev;
    } RtfFieldStackLevel;

/************************************************************************/
/*									*/
/*  Consume a field.							*/
/*									*/
/************************************************************************/

static int docRtfReadFldrslt(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    int			res;

    res= docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfDocumentGroups,
				docRtfTextParticule, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

static int docRtfCommitFldinst(	const RtfControlWord *	rcw,
				RtfReader *		rrc )
    {
    RtfFieldStackLevel *	rfsl= rrc->rrcFieldStack;
    DocumentField *		df= rfsl->rfslField;
    int				kind;

    char *			text= (char *)0;
    int				size;
    const int			removeSemicolon= 0;

    if  ( ! rfsl )
	{ XDEB(rfsl); return 0;	}

    df= rfsl->rfslField;

    if  ( docRtfStoreSavedText( &text, &size, rrc, removeSemicolon ) )
	{ LDEB(1); return -1;	}
    if  ( docSetFieldInst( df, text, size ) )
	{ LDEB(size); return -1;	}

    if  ( text )
	{ free( text );		}

    kind= docFieldKindFromInstructions( df );
    if  ( kind >= 0 )
	{ df->dfKind= kind;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Move a bookmark inside the field instructions to the exterior.	*/
/*									*/
/************************************************************************/

# if 0
static int docRtfBkmkStartX(	const RtfControlWord *		rcw,
				int				arg,
				RtfReader *		rrc )
    {
    RtfFieldStackLevel *	here;
    RtfFieldStackLevel *	prev;
    RtfFieldStackLevel *	prpr;

    if  ( docRtfBkmkStart( rcw, arg, rrc ) )
	{ LDEB(1); return -1;	}

    here= rrc->rrcFieldStack;
    if  ( ! here )
	{ XDEB(here); return -1;	}
    prev= rrc->rrcFieldStack->rfslPrev;
    if  ( ! prev )
	{ XDEB(prev); return -1;	}
    prpr= rrc->rrcFieldStack->rfslPrev->rfslPrev;

    here->rfslStartPosition= prev->rfslStartPosition;
    here->rfslField->dfHeadPosition= here->rfslStartPosition;

    rrc->rrcFieldStack= prev;
    rrc->rrcFieldStack->rfslPrev= here;
    rrc->rrcFieldStack->rfslPrev->rfslPrev= prpr;

    return 0;
    }

static int docRtfReadFieldX(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int		res;

    res= docRtfReadField( rcw, arg, rrc );

    return res;
    }

# endif

static RtfControlWord	docRtfFldinstGroups[]=
    {
#	if 0
	RTF_DEST_XX( "bkmkstart",	FPpropFLDINST,	docRtfBkmkStartX ),
	RTF_DEST_XX( "field",		RTFidFIELD,	docRtfReadFieldX ),
#	endif

	{ (char *)0, 0, 0 }
    };

static int docRtfReadFldinst(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    int		res;

    res= docRtfReadGroup( rcw, 0, 0, rrc, docRtfFldinstGroups,
						docRtfSaveDocEncodedText,
						docRtfCommitFldinst );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

static RtfControlWord	docRtfFieldGroups[]=
{
    RTF_DEST_XX( "fldrslt",	FPpropFLDRSLT,	docRtfReadFldrslt ),
    RTF_DEST_XX( "fldinst",	FPpropFLDINST,	docRtfReadFldinst ),

    { (char *)0, 0, 0 }
};

static int docRtfReadPushField(		int *			pPart,
					DocumentField **	pField,
					int			kind,
					RtfReader *		rrc,
					const char *		fieldinst,
					int			size )
    {
    int				rval= 0;
    BufferDocument *		bd= rrc->rrDocument;
    BufferItem *		paraNode;
    int				part;

    RtfReadingState *		rrs= rrc->rrcState;
    RtfFieldStackLevel *	rfsl= (RtfFieldStackLevel *)0;

    paraNode= docRtfGetParaNode( rrc );
    if  ( ! paraNode )
	{ XDEB(paraNode); rval= -1; goto ready; }

    part= paraNode->biParaParticuleCount;

    rfsl= (RtfFieldStackLevel *)malloc( sizeof(RtfFieldStackLevel) );
    if  ( ! rfsl )
	{ XDEB(rfsl); rval= -1; goto ready;	}

    rfsl->rfslSelectionScope= rrc->rrcSelectionScope;
    rfsl->rfslStartPosition.epParaNr= docNumberOfParagraph( paraNode );
    rfsl->rfslStartPosition.epStroff= docParaStrlen( paraNode );
    rfsl->rfslParticule= paraNode->biParaParticuleCount;
    rfsl->rfslPrev= rrc->rrcFieldStack;

    rfsl->rfslField= docClaimField( &(bd->bdFieldList) );
    if  ( ! rfsl->rfslField )
	{ XDEB(rfsl->rfslField); rval= -1; goto ready;	}
    rfsl->rfslField->dfKind= kind;
    rfsl->rfslField->dfSelectionScope= rfsl->rfslSelectionScope;
    rfsl->rfslField->dfHeadPosition= rfsl->rfslStartPosition;

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rrc, rrs );	}

    if  ( docInsertAdminParticule( bd, paraNode,
					    rfsl->rfslParticule,
					    rfsl->rfslStartPosition.epStroff,
					    rfsl->rfslField->dfFieldNumber,
					    DOCkindFIELDHEAD,
					    &(rrs->rrsTextAttribute) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    rrc->rrcAfterNoteref= 0;
    rrc->rrAfterParaHeadField= 0;

    if  ( size > 0 )
	{
	if  ( docSetFieldInst( rfsl->rfslField, fieldinst, size ) )
	    { LDEB(size); rval= -1; goto ready;	}
	}

    *pField= rfsl->rfslField;
    *pPart= part;
    rrc->rrcFieldStack= rfsl; rfsl= (RtfFieldStackLevel *)0; /* steal */

  ready:

    if  ( rfsl )
	{ free( rfsl );	}

    return rval;
    }

static int docRtfTerminateField(	const RtfFieldStackLevel *	rfsl,
					RtfReader *			rrc )
    {
    BufferItem *	bi= rrc->rrcNode;
    RtfReadingState *	rrs= rrc->rrcState;
    BufferDocument *	bd= rrc->rrDocument;

    int			paraNr;
    int			stroff;
    int			part;

    EditPosition	epTail;

    if  ( ! bi )
	{ XDEB(bi); return -1;	}
    if  ( bi->biLevel != DOClevPARA )
	{
	DocumentPosition	dp;

	if  ( docTailPosition( &dp, bi ) )
	    { XDEB(bi); return -1;	}

	bi= dp.dpNode;
	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rrc, rrs );	}

    paraNr= docNumberOfParagraph( bi );
    stroff= docParaStrlen( bi );
    part= bi->biParaParticuleCount;

    if  ( docInsertAdminParticule( bd, bi, part, stroff,
					    rfsl->rfslField->dfFieldNumber,
					    DOCkindFIELDTAIL,
					    &(rrs->rrsTextAttribute) ) )
	{ LDEB(1); return -1;	}

    epTail.epParaNr= paraNr;
    epTail.epStroff= stroff;
    docSetFieldTail( rfsl->rfslField, &epTail );

    if  ( rfsl->rfslPrev )
	{
	if  ( docAddChildToField( rfsl->rfslField, rfsl->rfslPrev->rfslField ) )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docAddRootFieldToTree( rrc->rrcTree, rfsl->rfslField ) )
	    { LDEB(1); return -1;	}
	}

    rrc->rrcLastFieldNumber= rfsl->rfslField->dfFieldNumber;
    rrc->rrcAfterNoteref= 0;
    rrc->rrAfterParaHeadField= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Pop a particular field from the field stack.			*/
/*  Improper field/bookmark nesting may cause enclosed fields to be	*/
/*  closed and hence that the field that we try to close/pop has	*/
/*  already  been closed/popped.					*/
/*									*/
/************************************************************************/

static int docRtfPopFieldFromFieldStack(	DocumentField *		df,
						RtfReader *	rrc )
    {
    RtfFieldStackLevel *	rfsl;

    rfsl= rrc->rrcFieldStack;
    while( rfsl )
	{
	if  ( rfsl->rfslField == df )
	    { break;	}

	rfsl= rfsl->rfslPrev;
	}

    if  ( ! rfsl )
	{ return 0;	}

    rfsl= rrc->rrcFieldStack;
    while( rfsl )
	{
	int	last= ( rfsl->rfslField == df );

	if  ( docRtfTerminateField( rfsl, rrc ) )
	    { LDEB(1); return -1;	}

	rrc->rrcFieldStack= rfsl->rfslPrev;
	free( rfsl );

	if  ( last )
	    { break;	}

	rfsl= rrc->rrcFieldStack;
	}

    return 0;
    }

int docRtfPopScopeFromFieldStack(	RtfReader *	rrc )
    {
    RtfFieldStackLevel *	rfsl;

    rfsl= rrc->rrcFieldStack;
    while( rfsl )
	{
	if  ( ! docSelectionSameScope( &(rfsl->rfslSelectionScope),
						&(rrc->rrcSelectionScope) ) )
	    { break;	}

	if  ( docRtfTerminateField( rfsl, rrc ) )
	    { LDEB(1); return -1;	}

	rrc->rrcFieldStack= rfsl->rfslPrev;
	free( rfsl );
	rfsl= rrc->rrcFieldStack;
	}

    return 0;
    }

int docRtfPopParaFromFieldStack(	RtfReader *	rrc,
					int			paraNr )
    {
    RtfFieldStackLevel *	rfsl;

    rfsl= rrc->rrcFieldStack;
    while( rfsl )
	{
	const FieldKindInformation *	fki;

	if  ( ! docSelectionSameScope( &(rfsl->rfslSelectionScope),
						&(rrc->rrcSelectionScope) ) )
	    { break;	}

	if  ( rfsl->rfslField->dfHeadPosition.epParaNr != paraNr )
	    { break;	}

	if  ( rfsl->rfslField->dfKind >= DOC_FieldKindCount	)
	    { break;	}
	fki= DOC_FieldKinds+ rfsl->rfslField->dfKind;
	if  ( fki->fkiLevel != DOClevSPAN )
	    { break;	}

	if  ( docRtfTerminateField( rfsl, rrc ) )
	    { LDEB(1); return -1;	}

	rrc->rrcFieldStack= rfsl->rfslPrev;
	free( rfsl );
	rfsl= rrc->rrcFieldStack;
	}

    return 0;
    }

int docRtfReadField(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *	rrc )
    {
    int			res;

    int			startParticule;
    DocumentField *	df= (DocumentField *)0;

    /*  docRtfReadPushField() adjusts level */
    if  ( docRtfReadPushField( &startParticule, &df, DOCfkUNKNOWN, rrc,
							(char *)0, 0 ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfFieldGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfPopFieldFromFieldStack( df, rrc ) )
	{ LDEB(df->dfFieldNumber); return -1;	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Trick: Remember rtf for lookup words in field data.			*/
/*									*/
/************************************************************************/

int docRtfLookupWord(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    RtfFieldStackLevel *	rfsl= rrc->rrcFieldStack;
    char			scratch[40];

    int				startParticule;
    DocumentField *		df= (DocumentField *)0;

    const char *		fieldinst= (const char *)0;
    int				size= 0;

    switch( rcw->rcwID )
	{
	case RTFlookupTCF:
	case RTFlookupTCL:
	    while( rfsl					&&
		   rfsl->rfslField->dfKind != DOCfkTC	&&
		   rfsl->rfslField->dfKind != DOCfkTCN	)
		{ rfsl= rfsl->rfslPrev;	}
	    if  ( ! rfsl )
		{ SXDEB(rcw->rcwWord,rfsl); return 0;	}
	    break;

	case RTFlookupXEF:
	case RTFlookupBXE:
	case RTFlookupIXE:
	    while( rfsl					&&
		   rfsl->rfslField->dfKind != DOCfkXE	)
		{ rfsl= rfsl->rfslPrev;	}
	    if  ( ! rfsl )
		{ SXDEB(rcw->rcwWord,rfsl); return 0;	}
	    break;

	case RTFlookupTC:
	    /*  docRtfReadPushField() adjusts level */
	    if  ( docRtfReadPushField( &startParticule, &df, DOCfkTC, rrc,
							    fieldinst, size ) )
		{ LDEB(1); return -1;	}
	    return 0;

	case RTFlookupTCN:
	    /*  docRtfReadPushField() adjusts level */
	    if  ( docRtfReadPushField( &startParticule, &df, DOCfkTCN, rrc,
							    fieldinst, size ) )
		{ LDEB(1); return -1;	}
	    return 0;

	default:
	    SDEB(rcw->rcwWord); return 0;
	}

    switch( rcw->rcwID )
	{
	case RTFlookupXEF:
	case RTFlookupTCF:
	case RTFlookupTCL:
	    sprintf( scratch, "\\%s%d", rcw->rcwWord, arg );
	    if  ( docAddToFieldData( rfsl->rfslField,
						scratch, strlen( scratch ) ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	case RTFlookupBXE:
	case RTFlookupIXE:
	    sprintf( scratch, "\\%s", rcw->rcwWord );
	    if  ( docAddToFieldData( rfsl->rfslField,
						scratch, strlen( scratch ) ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    return 0;

	default:
	    SDEB(rcw->rcwWord); return 0;
	}
    }

int docRtfLookupEntry(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    int			res;

    int			fieldKind;

    int			startParticule;
    DocumentField *	df= (DocumentField *)0;

    const char *	fieldinst= (const char *)0;
    int			size= 0;

    switch( rcw->rcwID )
	{
	case DOCfkXE:
	    fieldKind= DOCfkXE;
	    break;

	case DOCfkTC:
	    fieldKind= DOCfkTC;
	    break;

	case DOCfkTCN:
	    fieldKind= DOCfkTCN;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    /*  docRtfReadPushField() adjusts level */
    if  ( docRtfReadPushField( &startParticule, &df, fieldKind, rrc,
							fieldinst, size ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rrc,
				(RtfControlWord *)0,
				docRtfTextParticule, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfPopFieldFromFieldStack( df, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Process bookmarks.							*/
/*									*/
/************************************************************************/

static int docRtfCommitBookmarkName(	const RtfControlWord *	rcw,
					RtfReader *		rrc )
    {
    if  ( docRtfMemoryBufferAppendText( &(rrc->rrcBookmark), rrc ) )
	{ LDEB(1); return -1;	}

    if  ( rrc->rrcBookmark.mbSize > DOCmaxBOOKMARK )
	{
	LLDEB(rrc->rrcBookmark.mbSize,DOCmaxBOOKMARK);
	utilMemoryBufferSetSize( &(rrc->rrcBookmark), DOCmaxBOOKMARK );
	}

    return 0;
    }

int docRtfBkmkStart(	const RtfControlWord *		rcw,
			int				arg,
			RtfReader *		rrc )
    {
    int			res;

    int			startParticule;
    DocumentField *	df= (DocumentField *)0;

    const char *	fieldinst= (const char *)0;
    int			size= 0;

    utilMemoryBufferSetSize( &(rrc->rrcBookmark), 0 );

    /*  docRtfReadPushField() adjusts level */
    if  ( docRtfReadPushField( &startParticule, &df, DOCfkBOOKMARK, rrc,
							    fieldinst, size ) )
	{ LDEB(1); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rrc,
						(RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						docRtfCommitBookmarkName );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res); return res;	}

    if  ( docSetBookmarkField( &(df->dfInstructions), &(rrc->rrcBookmark) ) )
	{ LDEB(1); return -1;	}
    df->dfKind= DOCfkBOOKMARK;

    /* Do not pop field stack */

    return res;
    }

int docRtfBkmkEnd(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *	rrc )
    {
    BufferDocument *	bd= rrc->rrDocument;

    int			res;
    int			fieldNumber;

    DocumentField *	df;

    /*  do not push the field */

    utilMemoryBufferSetSize( &(rrc->rrcBookmark), 0 );

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    res= docRtfReadGroup( rcw, 0, 0, rrc, (RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						docRtfCommitBookmarkName );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res); return res;	}

    fieldNumber= docFindBookmarkField( &df, &(bd->bdFieldList),
							&(rrc->rrcBookmark) );
    if  ( fieldNumber < 0 )
	{ /*SDEB((char *)rrc->rrcBookmarkName);*/ return 0;	}

    if  ( docRtfPopFieldFromFieldStack( df, rrc ) )
	{ LDEB(fieldNumber);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Translate a special character to the corresponding field type.	*/
/*									*/
/************************************************************************/

DocumentField * docRtfSpecialField(
				int			fieldKind,
				const char *		fieldinst,
				int			fieldsize,
				const char *		fieldRslt,
				RtfReader *	rrc )
    {
    int			startParticule;
    DocumentField *	df;

    /*  docRtfReadPushField() adjusts level */
    if  ( docRtfReadPushField( &startParticule, &df, fieldKind, rrc,
						    fieldinst, fieldsize ) )
	{ LDEB(1); return (DocumentField *)0;	}

    if  ( fieldRslt )
	{
	/*  fieldRslt is an ascii string from the Ted source code here,	*/
	/*  so its encoding is irrelevant.				*/

	if  ( docRtfTextParticule( rrc, fieldRslt, strlen( fieldRslt ) ) )
	    { LDEB(2); return (DocumentField *)0;	}
	}

    if  ( docRtfPopFieldFromFieldStack( df, rrc ) )
	{ LDEB(1); return (DocumentField *)0;	}

    return df;
    }

/************************************************************************/
/*									*/
/*  Handle an rtf control word that results in a special character in	*/
/*  the RTF spec, but that is implemented as a field in Ted.		*/
/*									*/
/************************************************************************/

int docRtfTextSpecialToField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    const char *		fieldinst= (const char *)"?";
    int				fieldsize= 1;
    const char *		fieldRslt= (const char *)"?";

    int				afterNoteref= 0;

    switch( rcw->rcwID )
	{
	case DOCfkPAGE:
	    fieldinst= " PAGE ";
	    fieldsize= 6;
	    break;
	case DOCfkDATE:
	    fieldinst= " DATE \\* MERGEFORMAT ";
	    fieldsize= 21;
	    break;
	case DOCfkTIME:
	    fieldinst= " TIME \\* MERGEFORMAT ";
	    fieldsize= 21;
	    break;
	case DOCfkSECTION:
	    fieldinst= " SECTION ";
	    fieldsize= 9;
	    break;

	case DOCfkCHFTN:
	    fieldinst= " -CHFTN ";
	    fieldsize= 8;
	    fieldRslt= (const char *)"1";
	    afterNoteref= 1;
	    break;
	case DOCfkCHATN:
	    fieldinst= " -CHATN ";
	    fieldsize= 8;
	    fieldRslt= (const char *)"!";
	    afterNoteref= 1;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    if  ( ! docRtfSpecialField( rcw->rcwID,
				    fieldinst, fieldsize, fieldRslt, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    rrc->rrcAfterNoteref= afterNoteref;
    rrc->rrAfterParaHeadField= afterNoteref != 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a group that results in a text field.				*/
/*									*/
/************************************************************************/

int docRtfReadTextField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int			res;

    int			startParticule;
    DocumentField *	df= (DocumentField *)0;

    const char *	fieldinst= (const char *)0;
    int			size= 0;

#   if 0
    /*  Will be added later on, as the table nesting is given inside */
    /*  the field, we have a problem if we start the paragraph now. */
    if  ( rcw->rcwID == DOCfkLISTTEXT )
	{
	rrc->rrAfterParaHeadField= 1;
	return docRtfSkipGroup( rcw, arg, rrc );
	}
#   endif

    /*  docRtfReadPushField() adjusts level */
    if  ( docRtfReadPushField( &startParticule, &df, rcw->rcwID, rrc,
							    fieldinst, size ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfDocumentGroups,
				docRtfTextParticule, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfPopFieldFromFieldStack( df, rrc ) )
	{ LDEB(1);	}

    if  ( rcw->rcwID == DOCfkLISTTEXT )
	{ rrc->rrAfterParaHeadField= 1;	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Handle a field property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberFieldProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    RtfFieldStackLevel *	rfsl= rrc->rrcFieldStack;

    if  ( ! rfsl )
	{ SXDEB(rcw->rcwWord,rrc->rrcFieldStack); return 0;	}

    if  ( docSetFieldProperty( rfsl->rfslField, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return 0;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a note property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberNoteProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    switch( rcw->rcwID )
	{
	case NOTEpropTREE_TYPE:
	    if  ( rrc->rrcSelectionScope.ssTreeType != DOCinFOOTNOTE )
		{ LLDEB(rrc->rrcSelectionScope.ssTreeType,DOCinFOOTNOTE); }
	    else{
		rrc->rrcSelectionScope.ssTreeType= rcw->rcwEnumValue;
		rrc->rrcNoteProperties.npTreeType= rcw->rcwEnumValue;
		}
	    break;

	case NOTEpropAUTO_NUMBER:
	    rrc->rrcNoteProperties.npAutoNumber= arg != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    PROPmaskADD( &(rrc->rrcNotePropertyMask), rcw->rcwID );

    return 0;
    }

