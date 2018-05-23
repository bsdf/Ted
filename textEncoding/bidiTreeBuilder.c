/************************************************************************/
/*									*/
/*  Build trees to represent the hierarchy defined by the unicode	*/
/*  bidirectionals algorithm.						*/
/*  See: http://unicode.org/reports/tr9/				*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	"bidiTreeBuilder.h"
#   include	"bidiEmbedding.h"

#   include	<appDebugon.h>

void bidiInitTreeBuilder(	BidiTreeBuilder *	btb )
    {
    btb->btbBottomNode= (BidiNode *)0;
    btb->btbExplicitNode= (BidiNode *)0;

    btb->btbOffset= 0;

    return;
    }

void bidiStartTreeBuilder(	BidiTreeBuilder *	btb,
				BidiNode *		bottom,
				int			embedding,
				int			level )
    {
    btb->btbBottomNode= bottom;
    btb->btbExplicitNode= bottom;

    btb->btbOffset= 0;

    bottom->bnFrom= 0;
    bottom->bnUpto= 0;
    bottom->bnEmbedding= embedding;
    bottom->bnLevel= level;

    btb->btbHighestLevel= level;

    return;
    }

int bidiTreeBuilderAddRun(	BidiTreeBuilder *	btb,
				int			embedding,
				int			level,
				int			from,
				int			upto )
    {
    if  ( from > upto )
	{ LLDEB(from,upto); return -1;	}
    if  ( ! btb->btbExplicitNode )
	{ XDEB(btb->btbExplicitNode); return -1;	}

    switch( embedding )
	{
	BidiNode *	fresh;

	case BIDIembedPDF:
	    if  ( from < btb->btbOffset -1 )
		{ LLDEB(from,btb->btbOffset); return -1;	}

	    if  ( bidiStretchNode( btb->btbExplicitNode,  upto ) )
		{ LDEB(upto); return -1;	}

	    btb->btbExplicitNode= btb->btbExplicitNode->bnParent;
	    btb->btbOffset= from;
	    break;

	case BIDIembedLRE:
	case BIDIembedRLE:
	case BIDIembedLRO:
	case BIDIembedRLO:
	    if  ( from < btb->btbOffset -1 )
		{ LLDEB(from,btb->btbOffset); return -1;	}

	    fresh= bidiNodeAddChild( btb->btbExplicitNode,
						embedding, level, from, upto );
	    if  ( ! fresh )
		{ LLLDEB(level,from,upto); return -1;	}

	    btb->btbExplicitNode= fresh;
	    btb->btbOffset= from;

	    if  ( btb->btbHighestLevel < level )
		{ btb->btbHighestLevel=  level;	}

	    break;

	case BIDIembedEMBEDDING:
	    if  ( from >= upto )
		{ LLDEB(from,upto); return -1;	}
	    if  ( from < btb->btbOffset )
		{ LLDEB(from,btb->btbOffset); return -1;	}

	    if  ( bidiStretchNode( btb->btbExplicitNode,  upto ) )
		{ LDEB(upto); return -1;	}

	    btb->btbOffset= upto;
	    break;

	case BIDIembedIMPLICIT:
	    if  ( from >= upto )
		{ LLDEB(from,upto); return -1;	}
	    if  ( from < btb->btbOffset )
		{ LLDEB(from,btb->btbOffset); return -1;	}

	    fresh= bidiNodeAddChild( btb->btbExplicitNode,
						embedding, level, from, upto );
	    if  ( ! fresh )
		{ LLLDEB(level,from,upto); return -1;	}

	    btb->btbOffset= upto;
	    if  ( btb->btbExplicitNode->bnUpto < upto )
		{ btb->btbExplicitNode->bnUpto= upto;	}

	    if  ( btb->btbHighestLevel < level )
		{ btb->btbHighestLevel=  level;	}

	    break;

	case BIDIembedREMOVED:
	case BIDIembedREJECTED:
	    /*  Could also have failed.. but ignoring these allows	*/
	    /*  the consumer to pass all call-backs from the bidi algo	*/
	    /*  through to the tree builder without any intermediate	*/
	    /*  logic.							*/
	    return 0;

	default:
	    CDEB(embedding); return -1;
	}

    return 0;
    }

