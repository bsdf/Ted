/************************************************************************/
/*									*/
/*  Save notes properties to RTF.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"

int docRtfSaveNotesProperties(
		RtfWriter *			rw,
		const PropertyMask *		mask,
		const NotesProperties *		np,
		const int			propMap[NOTESprop_COUNT],
		const char *			startNrTag,
		const char * const *		justificationTags,
		int				justificationTagCount,
		const char * const *		placementTags,
		int				placementTagCount,
		const char * const *		restartTags,
		int				restartTagCount,
		const char * const *		styleTags,
		int				styleTagCount )
    {
    int		prop;

    for ( prop= 0; prop < NOTESprop_COUNT; prop++ )
	{
	const int	pprop= propMap[prop];
	int		val;

	if  ( pprop < 0				||
	      ! PROPmaskISSET( mask, pprop )	)
	    { continue;	}

	val= docGetNotesProperty( np, prop );
	switch( prop )
	    {
	    case NOTESpropSTARTNR:
		docRtfWriteArgTag( rw, startNrTag, val );
		break;
	    case NOTESpropJUSTIFICATION:
		docRtfWriteEnumTag( rw, justificationTags, val,
				justificationTagCount, FTNjustify_COUNT );
		break;
	    case NOTESpropPLACEMENT:
		docRtfWriteEnumTag( rw, placementTags, val,
				placementTagCount, FTNplace_COUNT );
		break;
	    case NOTESpropRESTART:
		docRtfWriteEnumTag( rw, restartTags, val,
				restartTagCount, FTN_RST__COUNT );
		break;
	    case NOTESpropSTYLE:
		docRtfWriteEnumTag( rw, styleTags, val,
				styleTagCount, FTNstyle_COUNT );
		break;
	    default:
		LDEB(prop); return -1;
	    }
	}

    return 0;
    }

