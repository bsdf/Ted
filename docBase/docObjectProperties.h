/************************************************************************/
/*									*/
/*  Properies of inserted objects.					*/
/*  Objects are independent entities that are embedded in the text.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_OBJECT_PROPERTIES_H
#   define	DOC_OBJECT_PROPERTIES_H

/************************************************************************/
/*									*/
/*  Kinds of object.							*/
/*									*/
/************************************************************************/

typedef enum ObjectKind
    {
    DOCokUNKNOWN= 0,

    DOCokPICTWMETAFILE,
    DOCokPICTPNGBLIP,
    DOCokPICTJPEGBLIP,
    DOCokPICTEMFBLIP,
    DOCokMACPICT,
    DOCokPMMETAFILE,
    DOCokDIBITMAP,
    DOCokWBITMAP,
    DOCokOLEOBJECT,

    DOCokDRAWING_SHAPE,

    DOCokEPS_FILE,
	    /************************************************************/
	    /*  To include pictures. In Particular EPS pictures. As	*/
	    /*  this is different from what 'Word' does when you	*/
	    /*  include a picture: they are not	saved in the RTF file.	*/
	    /************************************************************/
    DOCokGIF_FILE,

    DOCok__COUNT
    } ObjectKind;

typedef enum ResultKind
    {
    RESULTkindUNKNOWN= 0,

    RESULTkindRTF,
    RESULTkindTXT,
    RESULTkindPICT,
    RESULTkindBMP,
    RESULTkindHTML,

    RESULTkind_COUNT
    } ResultKind;

typedef enum EmbeddedKind
    {
    EMBEDkindOBJEMB= 0,
    EMBEDkindOBJLINK,
    EMBEDkindOBJAUTLINK,
    EMBEDkindOBJSUB,
    EMBEDkindOBJPUB,
    EMBEDkindOBJICEMB,
    EMBEDkindOBJHTML,
    EMBEDkindOBJOCX,

    EMBEDkind_COUNT
    } EmbeddedKind;

typedef enum InsertedObjectProperty
    {
    IOpropRESULT_KIND,
    IOpropEMBED_KIND,

    IOpropOBJTWIPS_WIDE,
    IOpropOBJTWIPS_HIGH,

    IOpropOBJSCALE_X,
    IOpropOBJSCALE_Y,

    IOpropOBJCROP_TOP,
    IOpropOBJCROP_BOTTOM,
    IOpropOBJCROP_LEFT,
    IOpropOBJCROP_RIGHT,

    IOprop_UNSUPPORTED,

    IOprop_COUNT
    } InsertedObjectProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  DOC_OBJECT_PROPERTIES_H	*/
