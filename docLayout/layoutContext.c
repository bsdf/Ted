#   include	"docLayoutConfig.h"
#   include	"layoutContext.h"

void layoutInitContext(		LayoutContext *	lc )
    {
    lc->lcDrawingSurface= (struct DrawingSurface *)0;
    lc->lcAttributeToScreenFont= (IndexMapping *)0;
    lc->lcPostScriptFontList= (const PostScriptFontList *)0;
    lc->lcDocument= (BufferDocument *)0;
    lc->lcCloseObject= (DOC_CLOSE_OBJECT)0;

    lc->lcOx= 0;
    lc->lcOy= 0;

    lc->lcGetFontForAttribute= (GetFontForAttribute)0;

    lc->lcPixelsPerTwip= 0;
    lc->lcPageGapPixels= 0;
    }
