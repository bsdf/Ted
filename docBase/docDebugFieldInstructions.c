#   include	"docBaseConfig.h"

#   include	<appDebugon.h>
#   include	"docFieldInstructions.h"

void docListFieldInstructions(		int				indent,
					const FieldInstructions *	fi )
    {
    const InstructionsComponent *	ic= fi->fiComponents;
    int					i;

    for ( i= 0; i < fi->fiComponentCount; ic++, i++ )
	{
	appDebug( "%*s %2d: q=%s f=%s \"%s\"\n",
		    indent, "", i,
		    ic->icIsQuoted?"Y":"N",
		    ic->icIsFlag?"Y":"N",
		    utilMemoryBufferGetString( &(ic->icBuffer) ) );
	}

    return;
    }
