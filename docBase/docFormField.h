/************************************************************************/
/*									*/
/*  Formfield								*/
/*									*/
/************************************************************************/

typedef struct FormField
    {
    unsigned char		ffType;		/*  fftype		*/
    unsigned char		ffOwnHelp;	/*  ffownhelpN		*/
    unsigned char		ffOwnStatus;	/*  ffownstatN		*/
    unsigned char		ffProtected;	/*  ffprotN		*/
    unsigned char		ffSize;		/*  ffsizeN		*/
    unsigned char		ffTextType;	/*  fftypetxtN		*/
    unsigned char		ffRecalculate;	/*  ffrecalcN		*/
    unsigned char		ffHasListbox;	/*  ffhaslistboxN	*/

    short			ffMaxlen;	/*  ffmaxlen		*/
    short			ffCheckboxSize;	/*  ffhpsN		*/
    char *			ffName;		/*  ffname		*/
    char *			ffDefaultText;	/*  ffdeftext		*/
    short			ffDefaultIndex;	/*  ffdefres		*/
    char *			ffFormat;	/*  ffformat		*/
    char *			ffHelpText;	/*  ffhelptext		*/
    char *			ffStatusText;	/*  ffstattext		*/
    char *			ffEnterMacro;	/*  ffentrymcr		*/
    char *			ffExitMacro;	/*  ffexitmcr		*/
    char **			ffListValues;	/*  ffl			*/
    short			ffListValue;	/*  ffresN		*/
    } FormField;

