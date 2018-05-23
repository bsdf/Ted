/************************************************************************/
/*									*/
/*  Icons and pictures for 'Ted'.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<appIcons.h>
#   include	"tedApp.h"

#   include	"tedmain.xpm"
#   include	"tedabout.xpm"

static NamedPicture	TEDPictures[]=
    {
	{ "tedmain",	tedmain_xpm },
	{ "tedabout",	tedabout_xpm },
    };

void tedGetNamedPictures(	EditApplication *	ea )
    {
    ea->eaNamedPictures= TEDPictures;
    ea->eaNamedPictureCount= sizeof(TEDPictures)/sizeof(NamedPicture);
    }
