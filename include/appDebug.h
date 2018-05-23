/************************************************************************/
/*									*/
/*  Debugging and diagnostic macros.					*/
/*									*/
/*  The mechanism is activated when the macro APP_DEBUG is defined.	*/
/*									*/
/************************************************************************/

#   ifndef APP_DEBUG_H
#   define APP_DEBUG_H

#   ifdef	APP_DEB
#   undef	APP_DEB
#   endif

# ifdef __cplusplus
extern "C"
    {
# endif

#ifdef	__GNUC__
extern int appDebug(	const char *	format, ... )
			__attribute__ (( format ( printf, 1, 2 ) ));
#else
extern int appDebug(	const char *	format, ... );
#endif

extern const int appDebugZero;
extern int appDebugSetFile(	const char *	filename,
				const char *	mode );

# ifdef APP_DEBUG
#	include <ctype.h>
#	define	APP_DEB(x)	(x)
# else
#	define	APP_DEB(x)
# endif

# define DEB0(sa)	((sa)?"\"":""), ((sa)?(sa):"0x0"), ((sa)?"\"":"")
# define DEBS(sa)	(((sa)+appDebugZero)?"\"":""), (((sa)+appDebugZero)?(sa):"0x0"), (((sa)+appDebugZero)?"\"":"")
# define DEBX(lo)	((unsigned long)(lo))
# define DEBP(pt)	((void *)(pt))
# define DEBO(lo)	((unsigned long)(lo))
# define DEBL(lo)	((long)(lo))
# define DEBD(do)	((double)(do))
# define DEBC(ch)	(ch)&0xff,isprint((ch))?'=':' ',isprint((ch))?(ch):' '

# define DFMS	" %s=%s%s%s"
# define DFMX	" %s=0x%lx"
# define DFMP	" %s=%p"
# define DFMO	" %s=0%lo"
# define DFML	" %s=%ld"
# define DFMD	" %s=%g"
# define DFMC	" %s=0x%02x%c%c"

# define SDEB(s1) APP_DEB(appDebug( "%s(%3d)"			\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1)			))

# define LDEB(l1) APP_DEB(appDebug( "%s(%3d)"			\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1)			))

# define ODEB(l1) APP_DEB(appDebug( "%s(%3d)"			\
			    DFMO				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBO(l1)			))

# define XDEB(l1) APP_DEB(appDebug( "%s(%3d)"			\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1)			))
# define PDEB(p1) APP_DEB(appDebug( "%s(%3d)"			\
			    DFMP				\
			    "\n", __FILE__, __LINE__,		\
			    #p1, DEBP(p1)			))

# define FDEB(d1) APP_DEB(appDebug( "%s(%3d)"			\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #d1, DEBD(d1)			))

# define CDEB(c1) APP_DEB(appDebug( "%s(%3d)"			\
			    DFMC				\
			    "\n", __FILE__, __LINE__,		\
			    #c1, DEBC(c1)			))

# define CLDEB(c1,l2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMC				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #c1, DEBC(c1),			\
			    #l2, DEBL(l2)			))

# define CSDEB(c1,s2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMC				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #c1, DEBC(c1),			\
			    #s2, DEBS(s2)			))

# define CLSDEB(c1,l2,s3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMC				\
			    DFML				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #c1, DEBC(c1),			\
			    #l2, DEBL(l2),			\
			    #s3, DEBS(s3)			))

# define CLLDEB(c1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMC				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #c1, DEBC(c1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3)			))

# define CLLLDEB(c1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMC				\
			    DFML				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #c1, DEBC(c1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define CCDEB(c1,c2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMC				\
			    DFMC				\
			    "\n", __FILE__, __LINE__,		\
			    #c1, DEBC(c1),			\
			    #c2, DEBC(c2)			))

# define LCCDEB(l1,c2,c3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMC				\
			    DFMC				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #c2, DEBC(c2),			\
			    #c3, DEBC(c3)			))

# define LCXDEB(l1,c2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMC				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #c2, DEBC(c2),			\
			    #l3, DEBX(l3)			))

# define CXLDEB(c1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMC				\
			    DFMX				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #c1, DEBC(c1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBL(l3)			))

# define LCLDEB(l1,c2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMC				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #c2, DEBC(c2),			\
			    #l3, DEBL(l3)			))

# define LCSDEB(l1,c2,s3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMC				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #c2, DEBC(c2),			\
			    #s3, DEBS(s3)			))

# define LLCDEB(l1,l2,c3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFML				\
			    DFMC				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #c3, DEBC(c3)			))

# define SSDEB(s1,s2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2)			))

# define SCDEB(s1,c2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMC				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #c2, DEBC(c2)			))

# define LLDEB(l1,l2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2)			))

# define FLDEB(d1,l2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMD				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #d1, DEBD(d1),			\
			    #l2, DEBL(l2)			))

# define XXDEB(l1,l2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2)			))

# define PPDEB(p1,p2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMP				\
			    DFMP				\
			    "\n", __FILE__, __LINE__,		\
			    #p1, DEBP(p1),			\
			    #p2, DEBP(p2)			))

# define XLDEB(l1,l2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2)			))

# define LXDEB(l1,l2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2)			))
# define LPDEB(l1,p2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMP				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #p2, DEBP(p2)			))

# define SLDEB(s1,l2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2)			))

# define SFDEB(s1,d2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #d2, DEBD(d2)			))

# define LSDEB(l1,s2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2)			))

# define FSDEB(d1,s2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMD				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #d1, DEBD(d1),			\
			    #s2, DEBS(s2)			))

# define SXDEB(s1,l2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2)			))
# define SPDEB(s1,p2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMP				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #p2, DEBP(p2)			))

# define SXSDEB(s1,l2,s3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMX				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBX(l2),			\
			    #s3, DEBS(s3)			))

# define SLSDEB(s1,l2,s3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFML				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #s3, DEBS(s3)			))

# define SLSSDEB(s1,l2,s3,s4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFMS				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #s3, DEBS(s3),			\
			    #s4, DEBS(s4)			))

# define LLSSDEB(l1,l2,s3,s4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFMS				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #s3, DEBS(s3),			\
			    #s4, DEBS(s4)			))

# define XSDEB(l1,s2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2)			))

# define LFDEB(l1,d2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #d2, DEBD(d2)			))

# define LFLDEB(l1,d2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMD				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #d2, DEBD(d2),			\
			    #l3, DEBL(l3)			))

# define LFCDEB(l1,d2,c3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMD				\
			    DFMC				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #d2, DEBD(d2),			\
			    #c3, DEBC(c3)			))

# define FFDEB(d1,d2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #d1, DEBD(d1),			\
			    #d2, DEBD(d2)			))

# define FFLDEB(d1,d2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMD				\
			    DFMD				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #d1, DEBD(d1),			\
			    #d2, DEBD(d2),			\
			    #l3, DEBL(l3)			))

# define LCDEB(l1,c2) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMC				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #c2, DEBC(c2)			))

# define SLLDEB(s1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3)			))

# define SXLDEB(s1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMX				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBL(l3)			))

# define SLXDEB(s1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFML				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBX(l3)			))

# define SXXDEB(s1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMX				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBX(l3)			))

# define SFLDEB(s1,d2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMD				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #d2, DEBD(d2),			\
			    #l3, DEBL(l3)			))

# define SLFFDEB(s1,l2,d3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #d3, DEBD(d3),			\
			    #d4, DEBD(d4)			))

# define SLFDEB(s1,l2,d3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFML				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #d3, DEBD(d3)			))

# define SFFDEB(s1,d2,d3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #d2, DEBD(d2),			\
			    #d3, DEBD(d3)			))

# define SFFFDEB(s1,d2,d3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFMD				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #d2, DEBD(d2),			\
			    #d3, DEBD(d3),			\
			    #d4, DEBD(d4)			))

# define LSLDEB(l1,s2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMS				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBL(l3)			))

# define LSCDEB(l1,s2,c3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMS				\
			    DFMC				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #c3, DEBC(c3)			))

# define XSLDEB(l1,s2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFMS				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBL(l3)			))

# define LSXDEB(l1,s2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMS				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBX(l3)			))

# define LSFDEB(l1,s2,d3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMS				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #d3, DEBD(d3)			))

# define LSLLDEB(l1,s2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMS				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define LSFFDEB(l1,s2,d3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMS				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #d3, DEBD(d3),			\
			    #d4, DEBD(d4)			))

# define SLLLDEB(s1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define SLXXDEB(s1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFMX				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBX(l3),			\
			    #l4, DEBX(l4)			))

# define SXLLDEB(s1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFMX				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define SLLLLDEB(s1,l2,l3,l4,l5) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4),			\
			    #l5, DEBL(l5)			))

# define SLLLLLDEB(s1,l2,l3,l4,l5,l6) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4),			\
			    #l5, DEBL(l5),			\
			    #l6, DEBL(l6)			))

# define SLLLXDEB(s1,l2,l3,l4,l5) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4),			\
			    #l5, DEBX(l5)			))

# define SLLXDEB(s1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFML				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBX(l4)			))

# define LLLSDEB(l1,l2,l3,s4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #s4, DEBS(s4)			))

# define LLFFDEB(l1,l2,d3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #d3, DEBD(d3),			\
			    #d4, DEBD(d4)			))

# define SLLSDEB(s1,l2,l3,s4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFML				\
			    DFML				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #s4, DEBS(s4)			))

# define LLLDEB(l1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3)			))

# define LXLDEB(l1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMX				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBL(l3)			))

# define XXXDEB(l1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFMX				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBX(l3)			))

# define LXXXDEB(l1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMX				\
			    DFMX				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBX(l3),			\
			    #l4, DEBX(l4)			))

# define XLLDEB(l1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3)			))
# define PLLDEB(p1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMP				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #p1, DEBP(p1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3)			))

# define XLXDEB(l1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFML				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBX(l3)			))

# define LLLLDEB(l1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define LXXLDEB(l1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMX				\
			    DFMX				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBX(l3),			\
			    #l4, DEBL(l4)			))

# define LLLXDEB(l1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBX(l4)			))

# define LLLLXDEB(l1,l2,l3,l4,l5) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4),			\
			    #l5, DEBX(l5)			))

# define XXLLDEB(l1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMX				\
			    DFMX				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define LLXXDEB(l1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFMX				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBX(l3),			\
			    #l4, DEBX(l4)			))

# define FFLLDEB(d1,d2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMD				\
			    DFMD				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #d1, DEBD(d1),			\
			    #d2, DEBD(d2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define XXXXDEB(l1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMX				\
			    DFMX				\
			    DFMX				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBX(l3),			\
			    #l4, DEBX(l4)			))

# define XXXLDEB(l1,l2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMX				\
			    DFMX				\
			    DFMX				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBX(l3),			\
			    #l4, DEBL(l4)			))

# define LLLLLDEB(l1,l2,l3,l4,l5) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4),			\
			    #l5, DEBL(l5)			))

# define XXXLLDEB(l1,l2,l3,l4,l5) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMX				\
			    DFMX				\
			    DFMX				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBX(l3),			\
			    #l4, DEBL(l4),			\
			    #l5, DEBL(l5)			))

# define LLLLLLDEB(l1,l2,l3,l4,l5,l6) APP_DEB(appDebug( "%s(%3d)" \
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4),			\
			    #l5, DEBL(l5),			\
			    #l6, DEBL(l6)			))

# define LLLFDEB(l1,l2,l3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFML				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBL(l3),			\
			    #d4, DEBD(d4)			))

# define LXXDEB(l1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMX				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBX(l3)			))

# define XXLDEB(l1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFMX				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBX(l2),			\
			    #l3, DEBL(l3)			))

# define LLXDEB(l1,l2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFML				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #l3, DEBX(l3)			))

# define LLPDEB(l1,l2,p3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFML				\
			    DFMP				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #p3, DEBP(p3)			))

# define LFFDEB(l1,d2,d3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #d2, DEBD(d2),			\
			    #d3, DEBD(d3)			))

# define LFLLDEB(l1,d2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMD				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #d2, DEBD(d2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define LLFFFDEB(l1,l2,d3,d4,d5) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFMD				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #d3, DEBD(d3),			\
			    #d4, DEBD(d4),			\
			    #d5, DEBD(d5)			))

# define LFFFDEB(l1,d2,d3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMD				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #d2, DEBD(d2),			\
			    #d3, DEBD(d3),			\
			    #d4, DEBD(d4)			))

# define LFFFFDEB(l1,d2,d3,d4,d5) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMD				\
			    DFMD				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #d2, DEBD(d2),			\
			    #d3, DEBD(d3),			\
			    #d4, DEBD(d4),			\
			    #d5, DEBD(d5)			))

# define LLFDEB(l1,l2,d3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFML				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #d3, DEBD(d3)			))

# define LLSDEB(l1,l2,s3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFML				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBL(l2),			\
			    #s3, DEBS(s3)			))

# define XLSDEB(l1,l2,s3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMX				\
			    DFML				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #l2, DEBL(l2),			\
			    #s3, DEBS(s3)			))

# define LXSDEB(l1,l2,s3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFML				\
			    DFMX				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #l2, DEBX(l2),			\
			    #s3, DEBS(s3)			))

# define SSXDEB(s1,s2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMS				\
			    DFMX				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBX(l3)			))
# define SSPDEB(s1,s2,p3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMS				\
			    DFMP				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #p3, DEBP(p3)			))

# define SSLDEB(s1,s2,l3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMS				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBL(l3)			))

# define SSLSDEB(s1,s2,l3,s4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFMS				\
			    DFML				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBL(l3),			\
			    #s4, DEBS(s4)			))

# define SSFDEB(s1,s2,d3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMS				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #d3, DEBD(d3)			))

# define SSSLDEB(s1,s2,s3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFMS				\
			    DFMS				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3),			\
			    #l4, DEBL(l4)			))

# define SSSFDEB(s1,s2,s3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFMS				\
			    DFMS				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3),			\
			    #d4, DEBD(d4)			))

# define SSSSDEB(s1,s2,s3,s4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFMS				\
			    DFMS				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3),			\
			    #s4, DEBS(s4)			))

# define SSLLDEB(s1,s2,l3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFMS				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBL(l3),			\
			    #l4, DEBL(l4)			))

# define SSLFDEB(s1,s2,l3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMS				\
			    DFMS				\
			    DFML				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #l3, DEBL(l3),			\
			    #d4, DEBD(d4)			))

# define LLSLDEB(l1,l2,s3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFML				\
			    DFMS				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBS(l1),			\
			    #l2, DEBS(l2),			\
			    #s3, DEBL(s3),			\
			    #l4, DEBL(l4)			))

# define SSSDEB(s1,s2,s3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMS				\
			    DFMS				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #s1, DEBS(s1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3)			))

# define LSSSDEB(l1,s2,s3,s4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMS				\
			    DFMS				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3),			\
			    #s4, DEBS(s4)			))

# define LSSLDEB(l1,s2,s3,l4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMS				\
			    DFMS				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3),			\
			    #l4, DEBL(l4)			))

# define LSSLLDEB(l1,s2,s3,l4,l5) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMS				\
			    DFMS				\
			    DFML				\
			    DFML				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3),			\
			    #l4, DEBL(l4),			\
			    #l5, DEBL(l5)			))

# define LSSDEB(l1,s2,s3) APP_DEB(appDebug( "%s(%3d)"	\
			    DFML				\
			    DFMS				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBL(l1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3)			))

# define XSSDEB(l1,s2,s3) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMX				\
			    DFMS				\
			    DFMS				\
			    "\n", __FILE__, __LINE__,		\
			    #l1, DEBX(l1),			\
			    #s2, DEBS(s2),			\
			    #s3, DEBS(s3)			))

# define FFFDEB(d1,d2,d3) APP_DEB(appDebug( "%s(%3d)"		\
			    DFMD				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #d1, DEBD(d1),			\
			    #d2, DEBD(d2),			\
			    #d3, DEBD(d3)			))

# define FFFFDEB(d1,d2,d3,d4) APP_DEB(appDebug( "%s(%3d)"	\
			    DFMD				\
			    DFMD				\
			    DFMD				\
			    DFMD				\
			    "\n", __FILE__, __LINE__,		\
			    #d1, DEBD(d1),			\
			    #d2, DEBD(d2),			\
			    #d3, DEBD(d3),			\
			    #d4, DEBD(d4)			))

# ifdef __cplusplus
    }
# endif

#   endif /* APP_DEBUG_H */
