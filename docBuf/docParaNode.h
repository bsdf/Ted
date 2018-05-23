/************************************************************************/
/*									*/
/*  Text Editor Buffer structure of a paragraph.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_NODE_H
#   define	DOC_PARA_NODE_H

#   include	<docParaProperties.h>
#   include	<docObject.h>

/************************************************************************/
/*									*/
/*  A Paragraph.							*/
/*									*/
/************************************************************************/

typedef struct ParaNode
    {
    MemoryBuffer		btStringBuffer;

    int				btParticuleCount;
    struct TextParticule *	btParticules;

    int				btLineCount;
    struct TextLine *		btLines;

    ParagraphProperties		btProperties;

    int				btMajorityFontAscY0;
    int				btMajorityFontDescY1;
    int				btMajorityFontSize;
					/********************************/
					/*  Also used to find out	*/
					/*  whether the line extents	*/
					/*  must be recalculated.	*/
					/********************************/

    int				btBorderNrAbove;
    int				btBorderNrBelow;
    int				btTopInset;
    int				btBottomInset;
    } ParaNode;

/************************************************************************/
/*									*/
/*  Shortcut defines through the BufferItem union.			*/
/*									*/
/************************************************************************/

#	define	biParaString		BIU.biuPara.btStringBuffer.mbBytes
#	define	docParaStrlen( bi )	((bi)->BIU.biuPara.btStringBuffer.mbSize+0)
#	define	biParaStringBuffer      BIU.biuPara.btStringBuffer
#	define	docParaSetStrlen( bi, l ) \
	    utilMemoryBufferSetSize( &((bi)->BIU.biuPara.btStringBuffer), (l) );
#	define	docParaString( bi, fr )	((bi)->BIU.biuPara.btStringBuffer.mbBytes+(fr))

#	define	biParaParticuleCount	BIU.biuPara.btParticuleCount
#	define	biParaParticules	BIU.biuPara.btParticules
#	define	biParaLineCount		BIU.biuPara.btLineCount
#	define	biParaLines		BIU.biuPara.btLines

#	define	biParaMajorityFontAscY0		BIU.biuPara.btMajorityFontAscY0
#	define	biParaMajorityFontDescY1	BIU.biuPara.btMajorityFontDescY1
#	define	biParaMajorityFontSize	BIU.biuPara.btMajorityFontSize

#	define	biParaTopInset		BIU.biuPara.btTopInset
#	define	biParaBottomInset	BIU.biuPara.btBottomInset
#	define	biParaFlags		BIU.biuPara.btFlags
					/********************************/
					/*  Also used to find out	*/
					/*  whether the line extents	*/
					/*  must be recalculated.	*/
					/********************************/

#	define	biParaBorderNrAbove	BIU.biuPara.btBorderNrAbove
#	define	biParaBorderNrBelow	BIU.biuPara.btBorderNrBelow

#	define	biParaProperties	BIU.biuPara.btProperties

#	define	biParaTableNesting	biParaProperties.ppTableNesting
#	define	biParaLineSpacingIsMultiple \
					biParaProperties.ppLineSpacingIsMultiple
#	define	biParaBreakKind		biParaProperties.ppBreakKind
#	define	biParaFirstIndentTwips	biParaProperties.ppFirstIndentTwips
#	define	biParaLeftIndentTwips	biParaProperties.ppLeftIndentTwips
#	define	biParaRightIndentTwips	biParaProperties.ppRightIndentTwips
#	define	biParaSpaceBeforeTwips	biParaProperties.ppSpaceBeforeTwips
#	define	biParaSpaceAfterTwips	biParaProperties.ppSpaceAfterTwips
#	define	biParaLineSpacingTwips	biParaProperties.ppLineSpacingTwips
#	define	biParaAlignment		biParaProperties.ppAlignment
#	define	biParaTabStopListNumber	biParaProperties.ppTabStopListNumber
#	define	biParaKeepOnPage	biParaProperties.ppKeepOnPage
#	define	biParaKeepWithNext	biParaProperties.ppKeepWithNext
#	define	biParaShadingNumber	biParaProperties.ppShadingNumber
#	define	biParaFrameNumber	biParaProperties.ppFrameNumber
#	define	biParaOutlineLevel	biParaProperties.ppOutlineLevel
#	define	biParaListLevel		biParaProperties.ppListLevel
#	define	biParaTopBorderNumber	biParaProperties.ppTopBorderNumber
#	define	biParaBottomBorderNumber \
					biParaProperties.ppBottomBorderNumber
#	define	biParaLeftBorderNumber	biParaProperties.ppLeftBorderNumber
#	define	biParaRightBorderNumber	biParaProperties.ppRightBorderNumber
#	define	biParaBetweenBorderNumber \
					biParaProperties.ppBetweenBorderNumber
#	define	biParaBarNumber		biParaProperties.ppBarNumber
#	define	biParaStyle		biParaProperties.ppStyle
#	define	biParaListOverride	biParaProperties.ppListOverride
#	define	biParaWidowControl	biParaProperties.ppWidowControl

#   endif
