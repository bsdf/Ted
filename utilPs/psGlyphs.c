/************************************************************************/
/*									*/
/*  Font Encodings							*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stdio.h>
#   include	<string.h>

#   include	"psGlyphs.h"
#   include	"psFontInfo.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Indirection for variable names.					*/
/*									*/
/************************************************************************/

static const char GLYPH_A[]= "A";
static const char GLYPH_AE[]= "AE";
static const char GLYPH_AEacute[]= "AEacute";
static const char GLYPH_Aacute[]= "Aacute";
static const char GLYPH_Abreve[]= "Abreve";
static const char GLYPH_Acircumflex[]= "Acircumflex";
static const char GLYPH_Adieresis[]= "Adieresis";
static const char GLYPH_Agrave[]= "Agrave";
static const char GLYPH_Alpha[]= "Alpha";
static const char GLYPH_Alphatonos[]= "Alphatonos";
static const char GLYPH_Amacron[]= "Amacron";
static const char GLYPH_Aogonek[]= "Aogonek";
static const char GLYPH_Aring[]= "Aring";
static const char GLYPH_Aringacute[]= "Aringacute";
static const char GLYPH_Atilde[]= "Atilde";
static const char GLYPH_B[]= "B";
static const char GLYPH_Beta[]= "Beta";
static const char GLYPH_C[]= "C";
static const char GLYPH_Cacute[]= "Cacute";
static const char GLYPH_Ccaron[]= "Ccaron";
static const char GLYPH_Ccedilla[]= "Ccedilla";
static const char GLYPH_Ccircumflex[]= "Ccircumflex";
static const char GLYPH_Cdotaccent[]= "Cdotaccent";
static const char GLYPH_Chi[]= "Chi";
static const char GLYPH_D[]= "D";
static const char GLYPH_Dcaron[]= "Dcaron";
static const char GLYPH_Dcroat[]= "Dcroat";
static const char GLYPH_Delta[]= "Delta";
static const char GLYPH_E[]= "E";
static const char GLYPH_Eacute[]= "Eacute";
static const char GLYPH_Ebreve[]= "Ebreve";
static const char GLYPH_Ecaron[]= "Ecaron";
static const char GLYPH_Ecircumflex[]= "Ecircumflex";
static const char GLYPH_Edieresis[]= "Edieresis";
static const char GLYPH_Edotaccent[]= "Edotaccent";
static const char GLYPH_Egrave[]= "Egrave";
static const char GLYPH_Emacron[]= "Emacron";
static const char GLYPH_Eng[]= "Eng";
static const char GLYPH_Eogonek[]= "Eogonek";
static const char GLYPH_Epsilon[]= "Epsilon";
static const char GLYPH_Epsilontonos[]= "Epsilontonos";
static const char GLYPH_Eta[]= "Eta";
static const char GLYPH_Etatonos[]= "Etatonos";
static const char GLYPH_Eth[]= "Eth";
static const char GLYPH_Euro[]= "Euro";
static const char GLYPH_F[]= "F";
static const char GLYPH_G[]= "G";
static const char GLYPH_Gamma[]= "Gamma";
static const char GLYPH_Gbreve[]= "Gbreve";
static const char GLYPH_Gcaron[]= "Gcaron";
static const char GLYPH_Gcircumflex[]= "Gcircumflex";
static const char GLYPH_Gcommaaccent[]= "Gcommaaccent";
static const char GLYPH_Gdotaccent[]= "Gdotaccent";
static const char GLYPH_H[]= "H";
static const char GLYPH_H18533[]= "H18533";
static const char GLYPH_H18543[]= "H18543";
static const char GLYPH_H18551[]= "H18551";
static const char GLYPH_H22073[]= "H22073";
static const char GLYPH_Hbar[]= "Hbar";
static const char GLYPH_Hcircumflex[]= "Hcircumflex";
static const char GLYPH_I[]= "I";
static const char GLYPH_IJ[]= "IJ";
static const char GLYPH_Iacute[]= "Iacute";
static const char GLYPH_Ibreve[]= "Ibreve";
static const char GLYPH_Icircumflex[]= "Icircumflex";
static const char GLYPH_Idieresis[]= "Idieresis";
static const char GLYPH_Idotaccent[]= "Idotaccent";
static const char GLYPH_Ifraktur[]= "Ifraktur";
static const char GLYPH_Igrave[]= "Igrave";
static const char GLYPH_Imacron[]= "Imacron";
static const char GLYPH_Iogonek[]= "Iogonek";
static const char GLYPH_Iota[]= "Iota";
static const char GLYPH_Iotadieresis[]= "Iotadieresis";
static const char GLYPH_Iotatonos[]= "Iotatonos";
static const char GLYPH_Itilde[]= "Itilde";
static const char GLYPH_J[]= "J";
static const char GLYPH_Jcircumflex[]= "Jcircumflex";
static const char GLYPH_K[]= "K";
static const char GLYPH_Kappa[]= "Kappa";
static const char GLYPH_Kcommaaccent[]= "Kcommaaccent";
static const char GLYPH_L[]= "L";
static const char GLYPH_Lacute[]= "Lacute";
static const char GLYPH_Lambda[]= "Lambda";
static const char GLYPH_Lcaron[]= "Lcaron";
static const char GLYPH_Lcommaaccent[]= "Lcommaaccent";
static const char GLYPH_Ldot[]= "Ldot";
static const char GLYPH_Lslash[]= "Lslash";
static const char GLYPH_M[]= "M";
static const char GLYPH_Mu[]= "Mu";
static const char GLYPH_N[]= "N";
static const char GLYPH_Nacute[]= "Nacute";
static const char GLYPH_Ncaron[]= "Ncaron";
static const char GLYPH_Ncommaaccent[]= "Ncommaaccent";
static const char GLYPH_Ntilde[]= "Ntilde";
static const char GLYPH_Nu[]= "Nu";
static const char GLYPH_O[]= "O";
static const char GLYPH_OE[]= "OE";
static const char GLYPH_Oacute[]= "Oacute";
static const char GLYPH_Obreve[]= "Obreve";
static const char GLYPH_Ocircumflex[]= "Ocircumflex";
static const char GLYPH_Odieresis[]= "Odieresis";
static const char GLYPH_Ograve[]= "Ograve";
static const char GLYPH_Ohorn[]= "Ohorn";
static const char GLYPH_Ohungarumlaut[]= "Ohungarumlaut";
static const char GLYPH_Omacron[]= "Omacron";
static const char GLYPH_Omega[]= "Omega";
static const char GLYPH_Omegatonos[]= "Omegatonos";
static const char GLYPH_Omicron[]= "Omicron";
static const char GLYPH_Omicrontonos[]= "Omicrontonos";
static const char GLYPH_Oslash[]= "Oslash";
static const char GLYPH_Oslashacute[]= "Oslashacute";
static const char GLYPH_Otilde[]= "Otilde";
static const char GLYPH_P[]= "P";
static const char GLYPH_Phi[]= "Phi";
static const char GLYPH_Pi[]= "Pi";
static const char GLYPH_Psi[]= "Psi";
static const char GLYPH_Q[]= "Q";
static const char GLYPH_R[]= "R";
static const char GLYPH_Racute[]= "Racute";
static const char GLYPH_Rcaron[]= "Rcaron";
static const char GLYPH_Rcommaaccent[]= "Rcommaaccent";
static const char GLYPH_Rfraktur[]= "Rfraktur";
static const char GLYPH_Rho[]= "Rho";
static const char GLYPH_S[]= "S";
static const char GLYPH_SF010000[]= "SF010000";
static const char GLYPH_SF020000[]= "SF020000";
static const char GLYPH_SF030000[]= "SF030000";
static const char GLYPH_SF040000[]= "SF040000";
static const char GLYPH_SF050000[]= "SF050000";
static const char GLYPH_SF060000[]= "SF060000";
static const char GLYPH_SF070000[]= "SF070000";
static const char GLYPH_SF080000[]= "SF080000";
static const char GLYPH_SF090000[]= "SF090000";
static const char GLYPH_SF100000[]= "SF100000";
static const char GLYPH_SF110000[]= "SF110000";
static const char GLYPH_SF190000[]= "SF190000";
static const char GLYPH_SF200000[]= "SF200000";
static const char GLYPH_SF210000[]= "SF210000";
static const char GLYPH_SF220000[]= "SF220000";
static const char GLYPH_SF230000[]= "SF230000";
static const char GLYPH_SF240000[]= "SF240000";
static const char GLYPH_SF250000[]= "SF250000";
static const char GLYPH_SF260000[]= "SF260000";
static const char GLYPH_SF270000[]= "SF270000";
static const char GLYPH_SF280000[]= "SF280000";
static const char GLYPH_SF360000[]= "SF360000";
static const char GLYPH_SF370000[]= "SF370000";
static const char GLYPH_SF380000[]= "SF380000";
static const char GLYPH_SF390000[]= "SF390000";
static const char GLYPH_SF400000[]= "SF400000";
static const char GLYPH_SF410000[]= "SF410000";
static const char GLYPH_SF420000[]= "SF420000";
static const char GLYPH_SF430000[]= "SF430000";
static const char GLYPH_SF440000[]= "SF440000";
static const char GLYPH_SF450000[]= "SF450000";
static const char GLYPH_SF460000[]= "SF460000";
static const char GLYPH_SF470000[]= "SF470000";
static const char GLYPH_SF480000[]= "SF480000";
static const char GLYPH_SF490000[]= "SF490000";
static const char GLYPH_SF500000[]= "SF500000";
static const char GLYPH_SF510000[]= "SF510000";
static const char GLYPH_SF520000[]= "SF520000";
static const char GLYPH_SF530000[]= "SF530000";
static const char GLYPH_SF540000[]= "SF540000";
static const char GLYPH_Sacute[]= "Sacute";
static const char GLYPH_Scaron[]= "Scaron";
static const char GLYPH_Scedilla[]= "Scedilla";
static const char GLYPH_Scircumflex[]= "Scircumflex";
static const char GLYPH_Scommaaccent[]= "Scommaaccent";
static const char GLYPH_Sigma[]= "Sigma";
static const char GLYPH_T[]= "T";
static const char GLYPH_Tau[]= "Tau";
static const char GLYPH_Tbar[]= "Tbar";
static const char GLYPH_Tcaron[]= "Tcaron";
static const char GLYPH_Tcommaaccent[]= "Tcommaaccent";
static const char GLYPH_Theta[]= "Theta";
static const char GLYPH_Thorn[]= "Thorn";
static const char GLYPH_U[]= "U";
static const char GLYPH_Uacute[]= "Uacute";
static const char GLYPH_Ubreve[]= "Ubreve";
static const char GLYPH_Ucircumflex[]= "Ucircumflex";
static const char GLYPH_Udieresis[]= "Udieresis";
static const char GLYPH_Ugrave[]= "Ugrave";
static const char GLYPH_Uhorn[]= "Uhorn";
static const char GLYPH_Uhungarumlaut[]= "Uhungarumlaut";
static const char GLYPH_Umacron[]= "Umacron";
static const char GLYPH_Uogonek[]= "Uogonek";
static const char GLYPH_Upsilon[]= "Upsilon";
static const char GLYPH_Upsilon1[]= "Upsilon1";
static const char GLYPH_Upsilondieresis[]= "Upsilondieresis";
static const char GLYPH_Upsilontonos[]= "Upsilontonos";
static const char GLYPH_Uring[]= "Uring";
static const char GLYPH_Utilde[]= "Utilde";
static const char GLYPH_V[]= "V";
static const char GLYPH_W[]= "W";
static const char GLYPH_Wacute[]= "Wacute";
static const char GLYPH_Wcircumflex[]= "Wcircumflex";
static const char GLYPH_Wdieresis[]= "Wdieresis";
static const char GLYPH_Wgrave[]= "Wgrave";
static const char GLYPH_X[]= "X";
static const char GLYPH_Xi[]= "Xi";
static const char GLYPH_Y[]= "Y";
static const char GLYPH_Yacute[]= "Yacute";
static const char GLYPH_Ycircumflex[]= "Ycircumflex";
static const char GLYPH_Ydieresis[]= "Ydieresis";
static const char GLYPH_Ygrave[]= "Ygrave";
static const char GLYPH_Z[]= "Z";
static const char GLYPH_Zacute[]= "Zacute";
static const char GLYPH_Zcaron[]= "Zcaron";
static const char GLYPH_Zdotaccent[]= "Zdotaccent";
static const char GLYPH_Zeta[]= "Zeta";
static const char GLYPH_a[]= "a";
static const char GLYPH_aacute[]= "aacute";
static const char GLYPH_abreve[]= "abreve";
static const char GLYPH_acircumflex[]= "acircumflex";
static const char GLYPH_acute[]= "acute";
static const char GLYPH_acutecomb[]= "acutecomb";
static const char GLYPH_adieresis[]= "adieresis";
static const char GLYPH_ae[]= "ae";
static const char GLYPH_aeacute[]= "aeacute";
static const char GLYPH_afii00208[]= "afii00208";
static const char GLYPH_afii10017[]= "afii10017";
static const char GLYPH_afii10018[]= "afii10018";
static const char GLYPH_afii10019[]= "afii10019";
static const char GLYPH_afii10020[]= "afii10020";
static const char GLYPH_afii10021[]= "afii10021";
static const char GLYPH_afii10022[]= "afii10022";
static const char GLYPH_afii10023[]= "afii10023";
static const char GLYPH_afii10024[]= "afii10024";
static const char GLYPH_afii10025[]= "afii10025";
static const char GLYPH_afii10026[]= "afii10026";
static const char GLYPH_afii10027[]= "afii10027";
static const char GLYPH_afii10028[]= "afii10028";
static const char GLYPH_afii10029[]= "afii10029";
static const char GLYPH_afii10030[]= "afii10030";
static const char GLYPH_afii10031[]= "afii10031";
static const char GLYPH_afii10032[]= "afii10032";
static const char GLYPH_afii10033[]= "afii10033";
static const char GLYPH_afii10034[]= "afii10034";
static const char GLYPH_afii10035[]= "afii10035";
static const char GLYPH_afii10036[]= "afii10036";
static const char GLYPH_afii10037[]= "afii10037";
static const char GLYPH_afii10038[]= "afii10038";
static const char GLYPH_afii10039[]= "afii10039";
static const char GLYPH_afii10040[]= "afii10040";
static const char GLYPH_afii10041[]= "afii10041";
static const char GLYPH_afii10042[]= "afii10042";
static const char GLYPH_afii10043[]= "afii10043";
static const char GLYPH_afii10044[]= "afii10044";
static const char GLYPH_afii10045[]= "afii10045";
static const char GLYPH_afii10046[]= "afii10046";
static const char GLYPH_afii10047[]= "afii10047";
static const char GLYPH_afii10048[]= "afii10048";
static const char GLYPH_afii10049[]= "afii10049";
static const char GLYPH_afii10050[]= "afii10050";
static const char GLYPH_afii10051[]= "afii10051";
static const char GLYPH_afii10052[]= "afii10052";
static const char GLYPH_afii10053[]= "afii10053";
static const char GLYPH_afii10054[]= "afii10054";
static const char GLYPH_afii10055[]= "afii10055";
static const char GLYPH_afii10056[]= "afii10056";
static const char GLYPH_afii10057[]= "afii10057";
static const char GLYPH_afii10058[]= "afii10058";
static const char GLYPH_afii10059[]= "afii10059";
static const char GLYPH_afii10060[]= "afii10060";
static const char GLYPH_afii10061[]= "afii10061";
static const char GLYPH_afii10062[]= "afii10062";
static const char GLYPH_afii10065[]= "afii10065";
static const char GLYPH_afii10066[]= "afii10066";
static const char GLYPH_afii10067[]= "afii10067";
static const char GLYPH_afii10068[]= "afii10068";
static const char GLYPH_afii10069[]= "afii10069";
static const char GLYPH_afii10070[]= "afii10070";
static const char GLYPH_afii10071[]= "afii10071";
static const char GLYPH_afii10072[]= "afii10072";
static const char GLYPH_afii10073[]= "afii10073";
static const char GLYPH_afii10074[]= "afii10074";
static const char GLYPH_afii10075[]= "afii10075";
static const char GLYPH_afii10076[]= "afii10076";
static const char GLYPH_afii10077[]= "afii10077";
static const char GLYPH_afii10078[]= "afii10078";
static const char GLYPH_afii10079[]= "afii10079";
static const char GLYPH_afii10080[]= "afii10080";
static const char GLYPH_afii10081[]= "afii10081";
static const char GLYPH_afii10082[]= "afii10082";
static const char GLYPH_afii10083[]= "afii10083";
static const char GLYPH_afii10084[]= "afii10084";
static const char GLYPH_afii10085[]= "afii10085";
static const char GLYPH_afii10086[]= "afii10086";
static const char GLYPH_afii10087[]= "afii10087";
static const char GLYPH_afii10088[]= "afii10088";
static const char GLYPH_afii10089[]= "afii10089";
static const char GLYPH_afii10090[]= "afii10090";
static const char GLYPH_afii10091[]= "afii10091";
static const char GLYPH_afii10092[]= "afii10092";
static const char GLYPH_afii10093[]= "afii10093";
static const char GLYPH_afii10094[]= "afii10094";
static const char GLYPH_afii10095[]= "afii10095";
static const char GLYPH_afii10096[]= "afii10096";
static const char GLYPH_afii10097[]= "afii10097";
static const char GLYPH_afii10098[]= "afii10098";
static const char GLYPH_afii10099[]= "afii10099";
static const char GLYPH_afii10100[]= "afii10100";
static const char GLYPH_afii10101[]= "afii10101";
static const char GLYPH_afii10102[]= "afii10102";
static const char GLYPH_afii10103[]= "afii10103";
static const char GLYPH_afii10104[]= "afii10104";
static const char GLYPH_afii10105[]= "afii10105";
static const char GLYPH_afii10106[]= "afii10106";
static const char GLYPH_afii10107[]= "afii10107";
static const char GLYPH_afii10108[]= "afii10108";
static const char GLYPH_afii10109[]= "afii10109";
static const char GLYPH_afii10110[]= "afii10110";
static const char GLYPH_afii10145[]= "afii10145";
static const char GLYPH_afii10146[]= "afii10146";
static const char GLYPH_afii10147[]= "afii10147";
static const char GLYPH_afii10148[]= "afii10148";
static const char GLYPH_afii10193[]= "afii10193";
static const char GLYPH_afii10194[]= "afii10194";
static const char GLYPH_afii10195[]= "afii10195";
static const char GLYPH_afii10196[]= "afii10196";
static const char GLYPH_afii10846[]= "afii10846";
static const char GLYPH_afii299[]= "afii299";
static const char GLYPH_afii300[]= "afii300";
static const char GLYPH_afii301[]= "afii301";
static const char GLYPH_afii57381[]= "afii57381";
static const char GLYPH_afii57388[]= "afii57388";
static const char GLYPH_afii57392[]= "afii57392";
static const char GLYPH_afii57393[]= "afii57393";
static const char GLYPH_afii57394[]= "afii57394";
static const char GLYPH_afii57395[]= "afii57395";
static const char GLYPH_afii57396[]= "afii57396";
static const char GLYPH_afii57397[]= "afii57397";
static const char GLYPH_afii57398[]= "afii57398";
static const char GLYPH_afii57399[]= "afii57399";
static const char GLYPH_afii57400[]= "afii57400";
static const char GLYPH_afii57401[]= "afii57401";
static const char GLYPH_afii57403[]= "afii57403";
static const char GLYPH_afii57407[]= "afii57407";
static const char GLYPH_afii57409[]= "afii57409";
static const char GLYPH_afii57410[]= "afii57410";
static const char GLYPH_afii57411[]= "afii57411";
static const char GLYPH_afii57412[]= "afii57412";
static const char GLYPH_afii57413[]= "afii57413";
static const char GLYPH_afii57414[]= "afii57414";
static const char GLYPH_afii57415[]= "afii57415";
static const char GLYPH_afii57416[]= "afii57416";
static const char GLYPH_afii57417[]= "afii57417";
static const char GLYPH_afii57418[]= "afii57418";
static const char GLYPH_afii57419[]= "afii57419";
static const char GLYPH_afii57420[]= "afii57420";
static const char GLYPH_afii57421[]= "afii57421";
static const char GLYPH_afii57422[]= "afii57422";
static const char GLYPH_afii57423[]= "afii57423";
static const char GLYPH_afii57424[]= "afii57424";
static const char GLYPH_afii57425[]= "afii57425";
static const char GLYPH_afii57426[]= "afii57426";
static const char GLYPH_afii57427[]= "afii57427";
static const char GLYPH_afii57428[]= "afii57428";
static const char GLYPH_afii57429[]= "afii57429";
static const char GLYPH_afii57430[]= "afii57430";
static const char GLYPH_afii57431[]= "afii57431";
static const char GLYPH_afii57432[]= "afii57432";
static const char GLYPH_afii57433[]= "afii57433";
static const char GLYPH_afii57434[]= "afii57434";
static const char GLYPH_afii57440[]= "afii57440";
static const char GLYPH_afii57441[]= "afii57441";
static const char GLYPH_afii57442[]= "afii57442";
static const char GLYPH_afii57443[]= "afii57443";
static const char GLYPH_afii57444[]= "afii57444";
static const char GLYPH_afii57445[]= "afii57445";
static const char GLYPH_afii57446[]= "afii57446";
static const char GLYPH_afii57448[]= "afii57448";
static const char GLYPH_afii57449[]= "afii57449";
static const char GLYPH_afii57450[]= "afii57450";
static const char GLYPH_afii57451[]= "afii57451";
static const char GLYPH_afii57452[]= "afii57452";
static const char GLYPH_afii57453[]= "afii57453";
static const char GLYPH_afii57454[]= "afii57454";
static const char GLYPH_afii57455[]= "afii57455";
static const char GLYPH_afii57456[]= "afii57456";
static const char GLYPH_afii57457[]= "afii57457";
static const char GLYPH_afii57458[]= "afii57458";
static const char GLYPH_afii57470[]= "afii57470";
static const char GLYPH_afii57505[]= "afii57505";
static const char GLYPH_afii57506[]= "afii57506";
static const char GLYPH_afii57507[]= "afii57507";
static const char GLYPH_afii57508[]= "afii57508";
static const char GLYPH_afii57509[]= "afii57509";
static const char GLYPH_afii57511[]= "afii57511";
static const char GLYPH_afii57512[]= "afii57512";
static const char GLYPH_afii57513[]= "afii57513";
static const char GLYPH_afii57514[]= "afii57514";
static const char GLYPH_afii57519[]= "afii57519";
static const char GLYPH_afii57534[]= "afii57534";
static const char GLYPH_afii57636[]= "afii57636";
static const char GLYPH_afii57645[]= "afii57645";
static const char GLYPH_afii57658[]= "afii57658";
static const char GLYPH_afii57664[]= "afii57664";
static const char GLYPH_afii57665[]= "afii57665";
static const char GLYPH_afii57666[]= "afii57666";
static const char GLYPH_afii57667[]= "afii57667";
static const char GLYPH_afii57668[]= "afii57668";
static const char GLYPH_afii57669[]= "afii57669";
static const char GLYPH_afii57670[]= "afii57670";
static const char GLYPH_afii57671[]= "afii57671";
static const char GLYPH_afii57672[]= "afii57672";
static const char GLYPH_afii57673[]= "afii57673";
static const char GLYPH_afii57674[]= "afii57674";
static const char GLYPH_afii57675[]= "afii57675";
static const char GLYPH_afii57676[]= "afii57676";
static const char GLYPH_afii57677[]= "afii57677";
static const char GLYPH_afii57678[]= "afii57678";
static const char GLYPH_afii57679[]= "afii57679";
static const char GLYPH_afii57680[]= "afii57680";
static const char GLYPH_afii57681[]= "afii57681";
static const char GLYPH_afii57682[]= "afii57682";
static const char GLYPH_afii57683[]= "afii57683";
static const char GLYPH_afii57684[]= "afii57684";
static const char GLYPH_afii57685[]= "afii57685";
static const char GLYPH_afii57686[]= "afii57686";
static const char GLYPH_afii57687[]= "afii57687";
static const char GLYPH_afii57688[]= "afii57688";
static const char GLYPH_afii57689[]= "afii57689";
static const char GLYPH_afii57690[]= "afii57690";
static const char GLYPH_afii57716[]= "afii57716";
static const char GLYPH_afii57717[]= "afii57717";
static const char GLYPH_afii57718[]= "afii57718";
static const char GLYPH_afii57793[]= "afii57793";
static const char GLYPH_afii57794[]= "afii57794";
static const char GLYPH_afii57795[]= "afii57795";
static const char GLYPH_afii57796[]= "afii57796";
static const char GLYPH_afii57797[]= "afii57797";
static const char GLYPH_afii57798[]= "afii57798";
static const char GLYPH_afii57799[]= "afii57799";
static const char GLYPH_afii57800[]= "afii57800";
static const char GLYPH_afii57801[]= "afii57801";
static const char GLYPH_afii57802[]= "afii57802";
static const char GLYPH_afii57803[]= "afii57803";
static const char GLYPH_afii57804[]= "afii57804";
static const char GLYPH_afii57806[]= "afii57806";
static const char GLYPH_afii57807[]= "afii57807";
static const char GLYPH_afii57839[]= "afii57839";
static const char GLYPH_afii57841[]= "afii57841";
static const char GLYPH_afii57842[]= "afii57842";
static const char GLYPH_afii57929[]= "afii57929";
static const char GLYPH_afii61248[]= "afii61248";
static const char GLYPH_afii61289[]= "afii61289";
static const char GLYPH_afii61352[]= "afii61352";
static const char GLYPH_afii61573[]= "afii61573";
static const char GLYPH_afii61574[]= "afii61574";
static const char GLYPH_afii61575[]= "afii61575";
static const char GLYPH_afii61664[]= "afii61664";
static const char GLYPH_afii63167[]= "afii63167";
static const char GLYPH_afii64937[]= "afii64937";
static const char GLYPH_agrave[]= "agrave";
static const char GLYPH_aleph[]= "aleph";
static const char GLYPH_alpha[]= "alpha";
static const char GLYPH_alphatonos[]= "alphatonos";
static const char GLYPH_amacron[]= "amacron";
static const char GLYPH_ampersand[]= "ampersand";
static const char GLYPH_angle[]= "angle";
static const char GLYPH_angleleft[]= "angleleft";
static const char GLYPH_angleright[]= "angleright";
static const char GLYPH_anoteleia[]= "anoteleia";
static const char GLYPH_aogonek[]= "aogonek";
static const char GLYPH_approxequal[]= "approxequal";
static const char GLYPH_aring[]= "aring";
static const char GLYPH_aringacute[]= "aringacute";
static const char GLYPH_arrowboth[]= "arrowboth";
static const char GLYPH_arrowdblboth[]= "arrowdblboth";
static const char GLYPH_arrowdbldown[]= "arrowdbldown";
static const char GLYPH_arrowdblleft[]= "arrowdblleft";
static const char GLYPH_arrowdblright[]= "arrowdblright";
static const char GLYPH_arrowdblup[]= "arrowdblup";
static const char GLYPH_arrowdown[]= "arrowdown";
static const char GLYPH_arrowleft[]= "arrowleft";
static const char GLYPH_arrowright[]= "arrowright";
static const char GLYPH_arrowup[]= "arrowup";
static const char GLYPH_arrowupdn[]= "arrowupdn";
static const char GLYPH_arrowupdnbse[]= "arrowupdnbse";
static const char GLYPH_asciicircum[]= "asciicircum";
static const char GLYPH_asciitilde[]= "asciitilde";
static const char GLYPH_asterisk[]= "asterisk";
static const char GLYPH_asteriskmath[]= "asteriskmath";
static const char GLYPH_at[]= "at";
static const char GLYPH_atilde[]= "atilde";
static const char GLYPH_b[]= "b";
static const char GLYPH_backslash[]= "backslash";
static const char GLYPH_bar[]= "bar";
static const char GLYPH_beta[]= "beta";
static const char GLYPH_block[]= "block";
static const char GLYPH_braceleft[]= "braceleft";
static const char GLYPH_braceright[]= "braceright";
static const char GLYPH_bracketleft[]= "bracketleft";
static const char GLYPH_bracketright[]= "bracketright";
static const char GLYPH_breve[]= "breve";
static const char GLYPH_brokenbar[]= "brokenbar";
static const char GLYPH_bullet[]= "bullet";
static const char GLYPH_c[]= "c";
static const char GLYPH_cacute[]= "cacute";
static const char GLYPH_caron[]= "caron";
static const char GLYPH_carriagereturn[]= "carriagereturn";
static const char GLYPH_ccaron[]= "ccaron";
static const char GLYPH_ccedilla[]= "ccedilla";
static const char GLYPH_ccircumflex[]= "ccircumflex";
static const char GLYPH_cdotaccent[]= "cdotaccent";
static const char GLYPH_cedilla[]= "cedilla";
static const char GLYPH_cent[]= "cent";
static const char GLYPH_chi[]= "chi";
static const char GLYPH_circle[]= "circle";
static const char GLYPH_circlemultiply[]= "circlemultiply";
static const char GLYPH_circleplus[]= "circleplus";
static const char GLYPH_circumflex[]= "circumflex";
static const char GLYPH_club[]= "club";
static const char GLYPH_colon[]= "colon";
static const char GLYPH_colonmonetary[]= "colonmonetary";
static const char GLYPH_comma[]= "comma";
static const char GLYPH_congruent[]= "congruent";
static const char GLYPH_copyright[]= "copyright";
static const char GLYPH_currency[]= "currency";
static const char GLYPH_d[]= "d";
static const char GLYPH_dagger[]= "dagger";
static const char GLYPH_daggerdbl[]= "daggerdbl";
static const char GLYPH_dcaron[]= "dcaron";
static const char GLYPH_dcroat[]= "dcroat";
static const char GLYPH_degree[]= "degree";
static const char GLYPH_delta[]= "delta";
static const char GLYPH_diamond[]= "diamond";
static const char GLYPH_dieresis[]= "dieresis";
static const char GLYPH_dieresistonos[]= "dieresistonos";
static const char GLYPH_divide[]= "divide";
static const char GLYPH_dkshade[]= "dkshade";
static const char GLYPH_dnblock[]= "dnblock";
static const char GLYPH_dollar[]= "dollar";
static const char GLYPH_dong[]= "dong";
static const char GLYPH_dotaccent[]= "dotaccent";
static const char GLYPH_dotbelowcomb[]= "dotbelowcomb";
static const char GLYPH_dotlessi[]= "dotlessi";
static const char GLYPH_dotmath[]= "dotmath";
static const char GLYPH_e[]= "e";
static const char GLYPH_eacute[]= "eacute";
static const char GLYPH_ebreve[]= "ebreve";
static const char GLYPH_ecaron[]= "ecaron";
static const char GLYPH_ecircumflex[]= "ecircumflex";
static const char GLYPH_edieresis[]= "edieresis";
static const char GLYPH_edotaccent[]= "edotaccent";
static const char GLYPH_egrave[]= "egrave";
static const char GLYPH_eight[]= "eight";
static const char GLYPH_element[]= "element";
static const char GLYPH_ellipsis[]= "ellipsis";
static const char GLYPH_emacron[]= "emacron";
static const char GLYPH_emdash[]= "emdash";
static const char GLYPH_emptyset[]= "emptyset";
static const char GLYPH_endash[]= "endash";
static const char GLYPH_eng[]= "eng";
static const char GLYPH_eogonek[]= "eogonek";
static const char GLYPH_epsilon[]= "epsilon";
static const char GLYPH_epsilontonos[]= "epsilontonos";
static const char GLYPH_equal[]= "equal";
static const char GLYPH_equivalence[]= "equivalence";
static const char GLYPH_estimated[]= "estimated";
static const char GLYPH_eta[]= "eta";
static const char GLYPH_etatonos[]= "etatonos";
static const char GLYPH_eth[]= "eth";
static const char GLYPH_exclam[]= "exclam";
static const char GLYPH_exclamdbl[]= "exclamdbl";
static const char GLYPH_exclamdown[]= "exclamdown";
static const char GLYPH_existential[]= "existential";
static const char GLYPH_f[]= "f";
static const char GLYPH_female[]= "female";
static const char GLYPH_figuredash[]= "figuredash";
static const char GLYPH_filledbox[]= "filledbox";
static const char GLYPH_filledrect[]= "filledrect";
static const char GLYPH_five[]= "five";
static const char GLYPH_fiveeighths[]= "fiveeighths";
static const char GLYPH_florin[]= "florin";
static const char GLYPH_four[]= "four";
static const char GLYPH_fraction[]= "fraction";
static const char GLYPH_franc[]= "franc";
static const char GLYPH_g[]= "g";
static const char GLYPH_gamma[]= "gamma";
static const char GLYPH_gbreve[]= "gbreve";
static const char GLYPH_gcaron[]= "gcaron";
static const char GLYPH_gcircumflex[]= "gcircumflex";
static const char GLYPH_gcommaaccent[]= "gcommaaccent";
static const char GLYPH_gdotaccent[]= "gdotaccent";
static const char GLYPH_germandbls[]= "germandbls";
static const char GLYPH_gradient[]= "gradient";
static const char GLYPH_grave[]= "grave";
static const char GLYPH_gravecomb[]= "gravecomb";
static const char GLYPH_greater[]= "greater";
static const char GLYPH_greaterequal[]= "greaterequal";
static const char GLYPH_guillemotleft[]= "guillemotleft";
static const char GLYPH_guillemotright[]= "guillemotright";
static const char GLYPH_guilsinglleft[]= "guilsinglleft";
static const char GLYPH_guilsinglright[]= "guilsinglright";
static const char GLYPH_h[]= "h";
static const char GLYPH_hbar[]= "hbar";
static const char GLYPH_hcircumflex[]= "hcircumflex";
static const char GLYPH_heart[]= "heart";
static const char GLYPH_hookabovecomb[]= "hookabovecomb";
static const char GLYPH_house[]= "house";
static const char GLYPH_hungarumlaut[]= "hungarumlaut";
static const char GLYPH_hyphen[]= "hyphen";
static const char GLYPH_i[]= "i";
static const char GLYPH_iacute[]= "iacute";
static const char GLYPH_ibreve[]= "ibreve";
static const char GLYPH_icircumflex[]= "icircumflex";
static const char GLYPH_idieresis[]= "idieresis";
static const char GLYPH_igrave[]= "igrave";
static const char GLYPH_ij[]= "ij";
static const char GLYPH_imacron[]= "imacron";
static const char GLYPH_infinity[]= "infinity";
static const char GLYPH_integral[]= "integral";
static const char GLYPH_integralbt[]= "integralbt";
static const char GLYPH_integraltp[]= "integraltp";
static const char GLYPH_intersection[]= "intersection";
static const char GLYPH_invbullet[]= "invbullet";
static const char GLYPH_invcircle[]= "invcircle";
static const char GLYPH_invsmileface[]= "invsmileface";
static const char GLYPH_iogonek[]= "iogonek";
static const char GLYPH_iota[]= "iota";
static const char GLYPH_iotadieresis[]= "iotadieresis";
static const char GLYPH_iotadieresistonos[]= "iotadieresistonos";
static const char GLYPH_iotatonos[]= "iotatonos";
static const char GLYPH_itilde[]= "itilde";
static const char GLYPH_j[]= "j";
static const char GLYPH_jcircumflex[]= "jcircumflex";
static const char GLYPH_k[]= "k";
static const char GLYPH_kappa[]= "kappa";
static const char GLYPH_kcommaaccent[]= "kcommaaccent";
static const char GLYPH_kgreenlandic[]= "kgreenlandic";
static const char GLYPH_l[]= "l";
static const char GLYPH_lacute[]= "lacute";
static const char GLYPH_lambda[]= "lambda";
static const char GLYPH_lcaron[]= "lcaron";
static const char GLYPH_lcommaaccent[]= "lcommaaccent";
static const char GLYPH_ldot[]= "ldot";
static const char GLYPH_less[]= "less";
static const char GLYPH_lessequal[]= "lessequal";
static const char GLYPH_lfblock[]= "lfblock";
static const char GLYPH_lira[]= "lira";
static const char GLYPH_logicaland[]= "logicaland";
static const char GLYPH_logicalnot[]= "logicalnot";
static const char GLYPH_logicalor[]= "logicalor";
static const char GLYPH_longs[]= "longs";
static const char GLYPH_lozenge[]= "lozenge";
static const char GLYPH_lslash[]= "lslash";
static const char GLYPH_ltshade[]= "ltshade";
static const char GLYPH_m[]= "m";
static const char GLYPH_macron[]= "macron";
static const char GLYPH_male[]= "male";
static const char GLYPH_micro[]= "micro";
static const char GLYPH_minus[]= "minus";
static const char GLYPH_minute[]= "minute";
static const char GLYPH_mu[]= "mu";
static const char GLYPH_multiply[]= "multiply";
static const char GLYPH_musicalnote[]= "musicalnote";
static const char GLYPH_musicalnotedbl[]= "musicalnotedbl";
static const char GLYPH_n[]= "n";
static const char GLYPH_nacute[]= "nacute";
static const char GLYPH_napostrophe[]= "napostrophe";
static const char GLYPH_ncaron[]= "ncaron";
static const char GLYPH_ncommaaccent[]= "ncommaaccent";
static const char GLYPH_nine[]= "nine";
static const char GLYPH_notelement[]= "notelement";
static const char GLYPH_notequal[]= "notequal";
static const char GLYPH_notsubset[]= "notsubset";
static const char GLYPH_ntilde[]= "ntilde";
static const char GLYPH_nu[]= "nu";
static const char GLYPH_numbersign[]= "numbersign";
static const char GLYPH_o[]= "o";
static const char GLYPH_oacute[]= "oacute";
static const char GLYPH_obreve[]= "obreve";
static const char GLYPH_ocircumflex[]= "ocircumflex";
static const char GLYPH_odieresis[]= "odieresis";
static const char GLYPH_oe[]= "oe";
static const char GLYPH_ogonek[]= "ogonek";
static const char GLYPH_ograve[]= "ograve";
static const char GLYPH_ohorn[]= "ohorn";
static const char GLYPH_ohungarumlaut[]= "ohungarumlaut";
static const char GLYPH_omacron[]= "omacron";
static const char GLYPH_omega[]= "omega";
static const char GLYPH_omega1[]= "omega1";
static const char GLYPH_omegatonos[]= "omegatonos";
static const char GLYPH_omicron[]= "omicron";
static const char GLYPH_omicrontonos[]= "omicrontonos";
static const char GLYPH_one[]= "one";
static const char GLYPH_onedotenleader[]= "onedotenleader";
static const char GLYPH_oneeighth[]= "oneeighth";
static const char GLYPH_onehalf[]= "onehalf";
static const char GLYPH_onequarter[]= "onequarter";
static const char GLYPH_onethird[]= "onethird";
static const char GLYPH_openbullet[]= "openbullet";
static const char GLYPH_ordfeminine[]= "ordfeminine";
static const char GLYPH_ordmasculine[]= "ordmasculine";
static const char GLYPH_orthogonal[]= "orthogonal";
static const char GLYPH_oslash[]= "oslash";
static const char GLYPH_oslashacute[]= "oslashacute";
static const char GLYPH_otilde[]= "otilde";
static const char GLYPH_p[]= "p";
static const char GLYPH_paragraph[]= "paragraph";
static const char GLYPH_parenleft[]= "parenleft";
static const char GLYPH_parenright[]= "parenright";
static const char GLYPH_partialdiff[]= "partialdiff";
static const char GLYPH_percent[]= "percent";
static const char GLYPH_period[]= "period";
static const char GLYPH_periodcentered[]= "periodcentered";
static const char GLYPH_perpendicular[]= "perpendicular";
static const char GLYPH_perthousand[]= "perthousand";
static const char GLYPH_peseta[]= "peseta";
static const char GLYPH_phi[]= "phi";
static const char GLYPH_phi1[]= "phi1";
static const char GLYPH_pi[]= "pi";
static const char GLYPH_plus[]= "plus";
static const char GLYPH_plusminus[]= "plusminus";
static const char GLYPH_prescription[]= "prescription";
static const char GLYPH_product[]= "product";
static const char GLYPH_propersubset[]= "propersubset";
static const char GLYPH_propersuperset[]= "propersuperset";
static const char GLYPH_proportional[]= "proportional";
static const char GLYPH_psi[]= "psi";
static const char GLYPH_q[]= "q";
static const char GLYPH_question[]= "question";
static const char GLYPH_questiondown[]= "questiondown";
static const char GLYPH_quotedbl[]= "quotedbl";
static const char GLYPH_quotedblbase[]= "quotedblbase";
static const char GLYPH_quotedblleft[]= "quotedblleft";
static const char GLYPH_quotedblright[]= "quotedblright";
static const char GLYPH_quoteleft[]= "quoteleft";
static const char GLYPH_quotereversed[]= "quotereversed";
static const char GLYPH_quoteright[]= "quoteright";
static const char GLYPH_quotesinglbase[]= "quotesinglbase";
static const char GLYPH_quotesingle[]= "quotesingle";
static const char GLYPH_r[]= "r";
static const char GLYPH_racute[]= "racute";
static const char GLYPH_radical[]= "radical";
static const char GLYPH_rcaron[]= "rcaron";
static const char GLYPH_rcommaaccent[]= "rcommaaccent";
static const char GLYPH_reflexsubset[]= "reflexsubset";
static const char GLYPH_reflexsuperset[]= "reflexsuperset";
static const char GLYPH_registered[]= "registered";
static const char GLYPH_revlogicalnot[]= "revlogicalnot";
static const char GLYPH_rho[]= "rho";
static const char GLYPH_ring[]= "ring";
static const char GLYPH_rtblock[]= "rtblock";
static const char GLYPH_s[]= "s";
static const char GLYPH_sacute[]= "sacute";
static const char GLYPH_scaron[]= "scaron";
static const char GLYPH_scedilla[]= "scedilla";
static const char GLYPH_scircumflex[]= "scircumflex";
static const char GLYPH_scommaaccent[]= "scommaaccent";
static const char GLYPH_second[]= "second";
static const char GLYPH_section[]= "section";
static const char GLYPH_semicolon[]= "semicolon";
static const char GLYPH_seven[]= "seven";
static const char GLYPH_seveneighths[]= "seveneighths";
static const char GLYPH_shade[]= "shade";
static const char GLYPH_sigma[]= "sigma";
static const char GLYPH_sigma1[]= "sigma1";
static const char GLYPH_similar[]= "similar";
static const char GLYPH_six[]= "six";
static const char GLYPH_slash[]= "slash";
static const char GLYPH_smileface[]= "smileface";
static const char GLYPH_space[]= "space";
static const char GLYPH_spade[]= "spade";
static const char GLYPH_sterling[]= "sterling";
static const char GLYPH_suchthat[]= "suchthat";
static const char GLYPH_summation[]= "summation";
static const char GLYPH_sun[]= "sun";
static const char GLYPH_t[]= "t";
static const char GLYPH_tau[]= "tau";
static const char GLYPH_tbar[]= "tbar";
static const char GLYPH_tcaron[]= "tcaron";
static const char GLYPH_tcommaaccent[]= "tcommaaccent";
static const char GLYPH_therefore[]= "therefore";
static const char GLYPH_theta[]= "theta";
static const char GLYPH_theta1[]= "theta1";
static const char GLYPH_thorn[]= "thorn";
static const char GLYPH_three[]= "three";
static const char GLYPH_threeeighths[]= "threeeighths";
static const char GLYPH_threequarters[]= "threequarters";
static const char GLYPH_tilde[]= "tilde";
static const char GLYPH_tildecomb[]= "tildecomb";
static const char GLYPH_tonos[]= "tonos";
static const char GLYPH_trademark[]= "trademark";
static const char GLYPH_triagdn[]= "triagdn";
static const char GLYPH_triaglf[]= "triaglf";
static const char GLYPH_triagrt[]= "triagrt";
static const char GLYPH_triagup[]= "triagup";
static const char GLYPH_two[]= "two";
static const char GLYPH_twodotenleader[]= "twodotenleader";
static const char GLYPH_twothirds[]= "twothirds";
static const char GLYPH_u[]= "u";
static const char GLYPH_uacute[]= "uacute";
static const char GLYPH_ubreve[]= "ubreve";
static const char GLYPH_ucircumflex[]= "ucircumflex";
static const char GLYPH_udieresis[]= "udieresis";
static const char GLYPH_ugrave[]= "ugrave";
static const char GLYPH_uhorn[]= "uhorn";
static const char GLYPH_uhungarumlaut[]= "uhungarumlaut";
static const char GLYPH_umacron[]= "umacron";
static const char GLYPH_underscore[]= "underscore";
static const char GLYPH_underscoredbl[]= "underscoredbl";
static const char GLYPH_union[]= "union";
static const char GLYPH_universal[]= "universal";
static const char GLYPH_uogonek[]= "uogonek";
static const char GLYPH_upblock[]= "upblock";
static const char GLYPH_upsilon[]= "upsilon";
static const char GLYPH_upsilondieresis[]= "upsilondieresis";
static const char GLYPH_upsilondieresistonos[]= "upsilondieresistonos";
static const char GLYPH_upsilontonos[]= "upsilontonos";
static const char GLYPH_uring[]= "uring";
static const char GLYPH_utilde[]= "utilde";
static const char GLYPH_v[]= "v";
static const char GLYPH_w[]= "w";
static const char GLYPH_wacute[]= "wacute";
static const char GLYPH_wcircumflex[]= "wcircumflex";
static const char GLYPH_wdieresis[]= "wdieresis";
static const char GLYPH_weierstrass[]= "weierstrass";
static const char GLYPH_wgrave[]= "wgrave";
static const char GLYPH_x[]= "x";
static const char GLYPH_xi[]= "xi";
static const char GLYPH_y[]= "y";
static const char GLYPH_yacute[]= "yacute";
static const char GLYPH_ycircumflex[]= "ycircumflex";
static const char GLYPH_ydieresis[]= "ydieresis";
static const char GLYPH_yen[]= "yen";
static const char GLYPH_ygrave[]= "ygrave";
static const char GLYPH_ypogegrammeni[]= "ypogegrammeni";
static const char GLYPH_z[]= "z";
static const char GLYPH_zacute[]= "zacute";
static const char GLYPH_zcaron[]= "zcaron";
static const char GLYPH_zdotaccent[]= "zdotaccent";
static const char GLYPH_zero[]= "zero";
static const char GLYPH_zeta[]= "zeta";

/*EXTRA*/

static const char GLYPH_onesuperior[]=		"onesuperior";
static const char GLYPH_twosuperior[]=		"twosuperior";
static const char GLYPH_threesuperior[]=	"threesuperior";
static const char GLYPH_radicalex[]=		"radicalex";
static const char GLYPH_arrowvertex[]=		"arrowvertex";
static const char GLYPH_arrowhorizex[]=		"arrowhorizex";
static const char GLYPH_registerserif[]=	"registerserif";
static const char GLYPH_copyrightserif[]=	"copyrightserif";
static const char GLYPH_trademarkserif[]=	"trademarkserif";
static const char GLYPH_registersans[]=		"registersans";
static const char GLYPH_copyrightsans[]=	"copyrightsans";
static const char GLYPH_trademarksans[]=	"trademarksans";
static const char GLYPH_parenlefttp[]=		"parenlefttp";
static const char GLYPH_parenleftex[]=		"parenleftex";
static const char GLYPH_parenleftbt[]=		"parenleftbt";
static const char GLYPH_bracketlefttp[]=	"bracketlefttp";
static const char GLYPH_bracketleftex[]=	"bracketleftex";
static const char GLYPH_bracketleftbt[]=	"bracketleftbt";
static const char GLYPH_bracelefttp[]=		"bracelefttp";
static const char GLYPH_braceleftmid[]=		"braceleftmid";
static const char GLYPH_braceleftbt[]=		"braceleftbt";
static const char GLYPH_braceex[]=		"braceex";
static const char GLYPH_integralex[]=		"integralex";
static const char GLYPH_parenrighttp[]=		"parenrighttp";
static const char GLYPH_parenrightex[]=		"parenrightex";
static const char GLYPH_parenrightbt[]=		"parenrightbt";
static const char GLYPH_bracketrighttp[]=	"bracketrighttp";
static const char GLYPH_bracketrightex[]=	"bracketrightex";
static const char GLYPH_bracketrightbt[]=	"bracketrightbt";
static const char GLYPH_bracerighttp[]=		"bracerighttp";
static const char GLYPH_bracerightmid[]=	"bracerightmid";
static const char GLYPH_bracerightbt[]=		"bracerightbt";

static const char GLYPH_nobreakspace[]=		"nobreakspace";

static const char GLYPH_a1[]=			"a1";
static const char GLYPH_a2[]=			"a2";
static const char GLYPH_a202[]=			"a202";
static const char GLYPH_a3[]=			"a3";
static const char GLYPH_a4[]=			"a4";
static const char GLYPH_a5[]=			"a5";
static const char GLYPH_a119[]=			"a119";
static const char GLYPH_a118[]=			"a118";
static const char GLYPH_a117[]=			"a117";
static const char GLYPH_a11[]=			"a11";
static const char GLYPH_a12[]=			"a12";
static const char GLYPH_a13[]=			"a13";
static const char GLYPH_a14[]=			"a14";
static const char GLYPH_a15[]=			"a15";
static const char GLYPH_a16[]=			"a16";
static const char GLYPH_a105[]=			"a105";
static const char GLYPH_a17[]=			"a17";
static const char GLYPH_a18[]=			"a18";
static const char GLYPH_a19[]=			"a19";
static const char GLYPH_a20[]=			"a20";
static const char GLYPH_a21[]=			"a21";
static const char GLYPH_a22[]=			"a22";
static const char GLYPH_a23[]=			"a23";
static const char GLYPH_a24[]=			"a24";
static const char GLYPH_a25[]=			"a25";
static const char GLYPH_a26[]=			"a26";
static const char GLYPH_a27[]=			"a27";
static const char GLYPH_a28[]=			"a28";
static const char GLYPH_a6[]=			"a6";
static const char GLYPH_a7[]=			"a7";
static const char GLYPH_a8[]=			"a8";
static const char GLYPH_a9[]=			"a9";
static const char GLYPH_a10[]=			"a10";
static const char GLYPH_a29[]=			"a29";
static const char GLYPH_a30[]=			"a30";
static const char GLYPH_a31[]=			"a31";
static const char GLYPH_a32[]=			"a32";
static const char GLYPH_a33[]=			"a33";
static const char GLYPH_a34[]=			"a34";
static const char GLYPH_a35[]=			"a35";
static const char GLYPH_a36[]=			"a36";
static const char GLYPH_a37[]=			"a37";
static const char GLYPH_a38[]=			"a38";
static const char GLYPH_a39[]=			"a39";
static const char GLYPH_a40[]=			"a40";
static const char GLYPH_a41[]=			"a41";
static const char GLYPH_a42[]=			"a42";
static const char GLYPH_a43[]=			"a43";
static const char GLYPH_a44[]=			"a44";
static const char GLYPH_a45[]=			"a45";
static const char GLYPH_a46[]=			"a46";
static const char GLYPH_a47[]=			"a47";
static const char GLYPH_a48[]=			"a48";
static const char GLYPH_a49[]=			"a49";
static const char GLYPH_a50[]=			"a50";
static const char GLYPH_a51[]=			"a51";
static const char GLYPH_a52[]=			"a52";
static const char GLYPH_a53[]=			"a53";
static const char GLYPH_a54[]=			"a54";
static const char GLYPH_a55[]=			"a55";
static const char GLYPH_a56[]=			"a56";
static const char GLYPH_a57[]=			"a57";
static const char GLYPH_a58[]=			"a58";
static const char GLYPH_a59[]=			"a59";
static const char GLYPH_a60[]=			"a60";
static const char GLYPH_a61[]=			"a61";
static const char GLYPH_a62[]=			"a62";
static const char GLYPH_a63[]=			"a63";
static const char GLYPH_a64[]=			"a64";
static const char GLYPH_a65[]=			"a65";
static const char GLYPH_a66[]=			"a66";
static const char GLYPH_a67[]=			"a67";
static const char GLYPH_a68[]=			"a68";
static const char GLYPH_a69[]=			"a69";
static const char GLYPH_a70[]=			"a70";
static const char GLYPH_a71[]=			"a71";
static const char GLYPH_a72[]=			"a72";
static const char GLYPH_a73[]=			"a73";
static const char GLYPH_a74[]=			"a74";
static const char GLYPH_a203[]=			"a203";
static const char GLYPH_a75[]=			"a75";
static const char GLYPH_a204[]=			"a204";
static const char GLYPH_a76[]=			"a76";
static const char GLYPH_a77[]=			"a77";
static const char GLYPH_a78[]=			"a78";
static const char GLYPH_a79[]=			"a79";
static const char GLYPH_a81[]=			"a81";
static const char GLYPH_a82[]=			"a82";
static const char GLYPH_a83[]=			"a83";
static const char GLYPH_a84[]=			"a84";
static const char GLYPH_a97[]=			"a97";
static const char GLYPH_a98[]=			"a98";
static const char GLYPH_a99[]=			"a99";
static const char GLYPH_a100[]=			"a100";
static const char GLYPH_a101[]=			"a101";
static const char GLYPH_a102[]=			"a102";
static const char GLYPH_a103[]=			"a103";
static const char GLYPH_a104[]=			"a104";
static const char GLYPH_a106[]=			"a106";
static const char GLYPH_a107[]=			"a107";
static const char GLYPH_a108[]=			"a108";
static const char GLYPH_a112[]=			"a112";
static const char GLYPH_a111[]=			"a111";
static const char GLYPH_a110[]=			"a110";
static const char GLYPH_a109[]=			"a109";
static const char GLYPH_a120[]=			"a120";
static const char GLYPH_a121[]=			"a121";
static const char GLYPH_a122[]=			"a122";
static const char GLYPH_a123[]=			"a123";
static const char GLYPH_a124[]=			"a124";
static const char GLYPH_a125[]=			"a125";
static const char GLYPH_a126[]=			"a126";
static const char GLYPH_a127[]=			"a127";
static const char GLYPH_a128[]=			"a128";
static const char GLYPH_a129[]=			"a129";
static const char GLYPH_a130[]=			"a130";
static const char GLYPH_a131[]=			"a131";
static const char GLYPH_a132[]=			"a132";
static const char GLYPH_a133[]=			"a133";
static const char GLYPH_a134[]=			"a134";
static const char GLYPH_a135[]=			"a135";
static const char GLYPH_a136[]=			"a136";
static const char GLYPH_a137[]=			"a137";
static const char GLYPH_a138[]=			"a138";
static const char GLYPH_a139[]=			"a139";
static const char GLYPH_a140[]=			"a140";
static const char GLYPH_a141[]=			"a141";
static const char GLYPH_a142[]=			"a142";
static const char GLYPH_a143[]=			"a143";
static const char GLYPH_a144[]=			"a144";
static const char GLYPH_a145[]=			"a145";
static const char GLYPH_a146[]=			"a146";
static const char GLYPH_a147[]=			"a147";
static const char GLYPH_a148[]=			"a148";
static const char GLYPH_a149[]=			"a149";
static const char GLYPH_a150[]=			"a150";
static const char GLYPH_a151[]=			"a151";
static const char GLYPH_a152[]=			"a152";
static const char GLYPH_a153[]=			"a153";
static const char GLYPH_a154[]=			"a154";
static const char GLYPH_a155[]=			"a155";
static const char GLYPH_a156[]=			"a156";
static const char GLYPH_a157[]=			"a157";
static const char GLYPH_a158[]=			"a158";
static const char GLYPH_a159[]=			"a159";
static const char GLYPH_a160[]=			"a160";
static const char GLYPH_a161[]=			"a161";
static const char GLYPH_a163[]=			"a163";
static const char GLYPH_a164[]=			"a164";
static const char GLYPH_a196[]=			"a196";
static const char GLYPH_a165[]=			"a165";
static const char GLYPH_a192[]=			"a192";
static const char GLYPH_a166[]=			"a166";
static const char GLYPH_a167[]=			"a167";
static const char GLYPH_a168[]=			"a168";
static const char GLYPH_a169[]=			"a169";
static const char GLYPH_a170[]=			"a170";
static const char GLYPH_a171[]=			"a171";
static const char GLYPH_a172[]=			"a172";
static const char GLYPH_a173[]=			"a173";
static const char GLYPH_a162[]=			"a162";
static const char GLYPH_a174[]=			"a174";
static const char GLYPH_a175[]=			"a175";
static const char GLYPH_a176[]=			"a176";
static const char GLYPH_a177[]=			"a177";
static const char GLYPH_a178[]=			"a178";
static const char GLYPH_a179[]=			"a179";
static const char GLYPH_a193[]=			"a193";
static const char GLYPH_a180[]=			"a180";
static const char GLYPH_a199[]=			"a199";
static const char GLYPH_a181[]=			"a181";
static const char GLYPH_a200[]=			"a200";
static const char GLYPH_a182[]=			"a182";
static const char GLYPH_a201[]=			"a201";
static const char GLYPH_a183[]=			"a183";
static const char GLYPH_a184[]=			"a184";
static const char GLYPH_a197[]=			"a197";
static const char GLYPH_a185[]=			"a185";
static const char GLYPH_a194[]=			"a194";
static const char GLYPH_a198[]=			"a198";
static const char GLYPH_a186[]=			"a186";
static const char GLYPH_a195[]=			"a195";
static const char GLYPH_a187[]=			"a187";
static const char GLYPH_a188[]=			"a188";
static const char GLYPH_a189[]=			"a189";
static const char GLYPH_a190[]=			"a190";
static const char GLYPH_a191[]=			"a191";

/************************************************************************/
static const char GLYPH_AEmacron[]= "AEmacron";
static const char GLYPH_AEsmall[]= "AEsmall";
static const char GLYPH_Aacutesmall[]= "Aacutesmall";
static const char GLYPH_Abreveacute[]= "Abreveacute";
static const char GLYPH_Abrevecyrillic[]= "Abrevecyrillic";
static const char GLYPH_Abrevedotbelow[]= "Abrevedotbelow";
static const char GLYPH_Abrevegrave[]= "Abrevegrave";
static const char GLYPH_Abrevehookabove[]= "Abrevehookabove";
static const char GLYPH_Abrevetilde[]= "Abrevetilde";
static const char GLYPH_Acaron[]= "Acaron";
static const char GLYPH_Acircle[]= "Acircle";
static const char GLYPH_Acircumflexacute[]= "Acircumflexacute";
static const char GLYPH_Acircumflexdotbelow[]= "Acircumflexdotbelow";
static const char GLYPH_Acircumflexgrave[]= "Acircumflexgrave";
static const char GLYPH_Acircumflexhookabove[]= "Acircumflexhookabove";
static const char GLYPH_Acircumflexsmall[]= "Acircumflexsmall";
static const char GLYPH_Acircumflextilde[]= "Acircumflextilde";
static const char GLYPH_Acute[]= "Acute";
static const char GLYPH_Acutesmall[]= "Acutesmall";
static const char GLYPH_Adblgrave[]= "Adblgrave";
static const char GLYPH_Adieresiscyrillic[]= "Adieresiscyrillic";
static const char GLYPH_Adieresismacron[]= "Adieresismacron";
static const char GLYPH_Adieresissmall[]= "Adieresissmall";
static const char GLYPH_Adotbelow[]= "Adotbelow";
static const char GLYPH_Adotmacron[]= "Adotmacron";
static const char GLYPH_Agravesmall[]= "Agravesmall";
static const char GLYPH_Ahookabove[]= "Ahookabove";
static const char GLYPH_Aiecyrillic[]= "Aiecyrillic";
static const char GLYPH_Ainvertedbreve[]= "Ainvertedbreve";
static const char GLYPH_Amonospace[]= "Amonospace";
static const char GLYPH_Aringbelow[]= "Aringbelow";
static const char GLYPH_Aringsmall[]= "Aringsmall";
static const char GLYPH_Asmall[]= "Asmall";
static const char GLYPH_Atildesmall[]= "Atildesmall";
static const char GLYPH_Aybarmenian[]= "Aybarmenian";
static const char GLYPH_Bcircle[]= "Bcircle";
static const char GLYPH_Bdotaccent[]= "Bdotaccent";
static const char GLYPH_Bdotbelow[]= "Bdotbelow";
static const char GLYPH_Benarmenian[]= "Benarmenian";
static const char GLYPH_Bhook[]= "Bhook";
static const char GLYPH_Blinebelow[]= "Blinebelow";
static const char GLYPH_Bmonospace[]= "Bmonospace";
static const char GLYPH_Brevesmall[]= "Brevesmall";
static const char GLYPH_Bsmall[]= "Bsmall";
static const char GLYPH_Btopbar[]= "Btopbar";
static const char GLYPH_Caarmenian[]= "Caarmenian";
static const char GLYPH_Caron[]= "Caron";
static const char GLYPH_Caronsmall[]= "Caronsmall";
static const char GLYPH_Ccedillaacute[]= "Ccedillaacute";
static const char GLYPH_Ccedillasmall[]= "Ccedillasmall";
static const char GLYPH_Ccircle[]= "Ccircle";
static const char GLYPH_Cedillasmall[]= "Cedillasmall";
static const char GLYPH_Chaarmenian[]= "Chaarmenian";
static const char GLYPH_Cheabkhasiancyrillic[]= "Cheabkhasiancyrillic";
static const char GLYPH_Chedescenderabkhasiancyrillic[]= "Chedescenderabkhasiancyrillic";
static const char GLYPH_Chedescendercyrillic[]= "Chedescendercyrillic";
static const char GLYPH_Chedieresiscyrillic[]= "Chedieresiscyrillic";
static const char GLYPH_Cheharmenian[]= "Cheharmenian";
static const char GLYPH_Chekhakassiancyrillic[]= "Chekhakassiancyrillic";
static const char GLYPH_Cheverticalstrokecyrillic[]= "Cheverticalstrokecyrillic";
static const char GLYPH_Chook[]= "Chook";
static const char GLYPH_Circumflexsmall[]= "Circumflexsmall";
static const char GLYPH_Cmonospace[]= "Cmonospace";
static const char GLYPH_Coarmenian[]= "Coarmenian";
static const char GLYPH_Csmall[]= "Csmall";
static const char GLYPH_DZ[]= "DZ";
static const char GLYPH_DZcaron[]= "DZcaron";
static const char GLYPH_Daarmenian[]= "Daarmenian";
static const char GLYPH_Dafrican[]= "Dafrican";
static const char GLYPH_Dcedilla[]= "Dcedilla";
static const char GLYPH_Dcircle[]= "Dcircle";
static const char GLYPH_Dcircumflexbelow[]= "Dcircumflexbelow";
static const char GLYPH_Ddotaccent[]= "Ddotaccent";
static const char GLYPH_Ddotbelow[]= "Ddotbelow";
static const char GLYPH_Deicoptic[]= "Deicoptic";
static const char GLYPH_Deltagreek[]= "Deltagreek";
static const char GLYPH_Dhook[]= "Dhook";
static const char GLYPH_Dieresis[]= "Dieresis";
static const char GLYPH_DieresisAcute[]= "DieresisAcute";
static const char GLYPH_DieresisGrave[]= "DieresisGrave";
static const char GLYPH_Dieresissmall[]= "Dieresissmall";
static const char GLYPH_Digammagreek[]= "Digammagreek";
static const char GLYPH_Dlinebelow[]= "Dlinebelow";
static const char GLYPH_Dmonospace[]= "Dmonospace";
static const char GLYPH_Dotaccentsmall[]= "Dotaccentsmall";
static const char GLYPH_Dsmall[]= "Dsmall";
static const char GLYPH_Dtopbar[]= "Dtopbar";
static const char GLYPH_Dz[]= "Dz";
static const char GLYPH_Dzcaron[]= "Dzcaron";
static const char GLYPH_Dzeabkhasiancyrillic[]= "Dzeabkhasiancyrillic";
static const char GLYPH_Eacutesmall[]= "Eacutesmall";
static const char GLYPH_Ecedillabreve[]= "Ecedillabreve";
static const char GLYPH_Echarmenian[]= "Echarmenian";
static const char GLYPH_Ecircle[]= "Ecircle";
static const char GLYPH_Ecircumflexacute[]= "Ecircumflexacute";
static const char GLYPH_Ecircumflexbelow[]= "Ecircumflexbelow";
static const char GLYPH_Ecircumflexdotbelow[]= "Ecircumflexdotbelow";
static const char GLYPH_Ecircumflexgrave[]= "Ecircumflexgrave";
static const char GLYPH_Ecircumflexhookabove[]= "Ecircumflexhookabove";
static const char GLYPH_Ecircumflexsmall[]= "Ecircumflexsmall";
static const char GLYPH_Ecircumflextilde[]= "Ecircumflextilde";
static const char GLYPH_Edblgrave[]= "Edblgrave";
static const char GLYPH_Edieresissmall[]= "Edieresissmall";
static const char GLYPH_Edotbelow[]= "Edotbelow";
static const char GLYPH_Egravesmall[]= "Egravesmall";
static const char GLYPH_Eharmenian[]= "Eharmenian";
static const char GLYPH_Ehookabove[]= "Ehookabove";
static const char GLYPH_Eightroman[]= "Eightroman";
static const char GLYPH_Einvertedbreve[]= "Einvertedbreve";
static const char GLYPH_Eiotifiedcyrillic[]= "Eiotifiedcyrillic";
static const char GLYPH_Elevenroman[]= "Elevenroman";
static const char GLYPH_Emacronacute[]= "Emacronacute";
static const char GLYPH_Emacrongrave[]= "Emacrongrave";
static const char GLYPH_Emonospace[]= "Emonospace";
static const char GLYPH_Endescendercyrillic[]= "Endescendercyrillic";
static const char GLYPH_Enghecyrillic[]= "Enghecyrillic";
static const char GLYPH_Enhookcyrillic[]= "Enhookcyrillic";
static const char GLYPH_Eopen[]= "Eopen";
static const char GLYPH_Ereversed[]= "Ereversed";
static const char GLYPH_Esdescendercyrillic[]= "Esdescendercyrillic";
static const char GLYPH_Esh[]= "Esh";
static const char GLYPH_Esmall[]= "Esmall";
static const char GLYPH_Etarmenian[]= "Etarmenian";
static const char GLYPH_Ethsmall[]= "Ethsmall";
static const char GLYPH_Etilde[]= "Etilde";
static const char GLYPH_Etildebelow[]= "Etildebelow";
static const char GLYPH_Ezh[]= "Ezh";
static const char GLYPH_Ezhcaron[]= "Ezhcaron";
static const char GLYPH_Ezhreversed[]= "Ezhreversed";
static const char GLYPH_Fcircle[]= "Fcircle";
static const char GLYPH_Fdotaccent[]= "Fdotaccent";
static const char GLYPH_Feharmenian[]= "Feharmenian";
static const char GLYPH_Feicoptic[]= "Feicoptic";
static const char GLYPH_Fhook[]= "Fhook";
static const char GLYPH_Fiveroman[]= "Fiveroman";
static const char GLYPH_Fmonospace[]= "Fmonospace";
static const char GLYPH_Fourroman[]= "Fourroman";
static const char GLYPH_Fsmall[]= "Fsmall";
static const char GLYPH_GBsquare[]= "GBsquare";
static const char GLYPH_Gacute[]= "Gacute";
static const char GLYPH_Gammaafrican[]= "Gammaafrican";
static const char GLYPH_Gangiacoptic[]= "Gangiacoptic";
static const char GLYPH_Gcircle[]= "Gcircle";
static const char GLYPH_Ghadarmenian[]= "Ghadarmenian";
static const char GLYPH_Ghemiddlehookcyrillic[]= "Ghemiddlehookcyrillic";
static const char GLYPH_Ghestrokecyrillic[]= "Ghestrokecyrillic";
static const char GLYPH_Ghook[]= "Ghook";
static const char GLYPH_Gimarmenian[]= "Gimarmenian";
static const char GLYPH_Gmacron[]= "Gmacron";
static const char GLYPH_Gmonospace[]= "Gmonospace";
static const char GLYPH_Grave[]= "Grave";
static const char GLYPH_Gravesmall[]= "Gravesmall";
static const char GLYPH_Gsmall[]= "Gsmall";
static const char GLYPH_Gsmallhook[]= "Gsmallhook";
static const char GLYPH_Gstroke[]= "Gstroke";
static const char GLYPH_HPsquare[]= "HPsquare";
static const char GLYPH_Haabkhasiancyrillic[]= "Haabkhasiancyrillic";
static const char GLYPH_Hadescendercyrillic[]= "Hadescendercyrillic";
static const char GLYPH_Hbrevebelow[]= "Hbrevebelow";
static const char GLYPH_Hcedilla[]= "Hcedilla";
static const char GLYPH_Hcircle[]= "Hcircle";
static const char GLYPH_Hdieresis[]= "Hdieresis";
static const char GLYPH_Hdotaccent[]= "Hdotaccent";
static const char GLYPH_Hdotbelow[]= "Hdotbelow";
static const char GLYPH_Hmonospace[]= "Hmonospace";
static const char GLYPH_Hoarmenian[]= "Hoarmenian";
static const char GLYPH_Horicoptic[]= "Horicoptic";
static const char GLYPH_Hsmall[]= "Hsmall";
static const char GLYPH_Hungarumlaut[]= "Hungarumlaut";
static const char GLYPH_Hungarumlautsmall[]= "Hungarumlautsmall";
static const char GLYPH_Hzsquare[]= "Hzsquare";
static const char GLYPH_Iacutesmall[]= "Iacutesmall";
static const char GLYPH_Icaron[]= "Icaron";
static const char GLYPH_Icircle[]= "Icircle";
static const char GLYPH_Icircumflexsmall[]= "Icircumflexsmall";
static const char GLYPH_Idblgrave[]= "Idblgrave";
static const char GLYPH_Idieresisacute[]= "Idieresisacute";
static const char GLYPH_Idieresiscyrillic[]= "Idieresiscyrillic";
static const char GLYPH_Idieresissmall[]= "Idieresissmall";
static const char GLYPH_Idotbelow[]= "Idotbelow";
static const char GLYPH_Iebrevecyrillic[]= "Iebrevecyrillic";
static const char GLYPH_Igravesmall[]= "Igravesmall";
static const char GLYPH_Ihookabove[]= "Ihookabove";
static const char GLYPH_Iinvertedbreve[]= "Iinvertedbreve";
static const char GLYPH_Imacroncyrillic[]= "Imacroncyrillic";
static const char GLYPH_Imonospace[]= "Imonospace";
static const char GLYPH_Iniarmenian[]= "Iniarmenian";
static const char GLYPH_Iotaafrican[]= "Iotaafrican";
static const char GLYPH_Ismall[]= "Ismall";
static const char GLYPH_Istroke[]= "Istroke";
static const char GLYPH_Itildebelow[]= "Itildebelow";
static const char GLYPH_Izhitsadblgravecyrillic[]= "Izhitsadblgravecyrillic";
static const char GLYPH_Jaarmenian[]= "Jaarmenian";
static const char GLYPH_Jcircle[]= "Jcircle";
static const char GLYPH_Jheharmenian[]= "Jheharmenian";
static const char GLYPH_Jmonospace[]= "Jmonospace";
static const char GLYPH_Jsmall[]= "Jsmall";
static const char GLYPH_KBsquare[]= "KBsquare";
static const char GLYPH_KKsquare[]= "KKsquare";
static const char GLYPH_Kabashkircyrillic[]= "Kabashkircyrillic";
static const char GLYPH_Kacute[]= "Kacute";
static const char GLYPH_Kadescendercyrillic[]= "Kadescendercyrillic";
static const char GLYPH_Kahookcyrillic[]= "Kahookcyrillic";
static const char GLYPH_Kastrokecyrillic[]= "Kastrokecyrillic";
static const char GLYPH_Kaverticalstrokecyrillic[]= "Kaverticalstrokecyrillic";
static const char GLYPH_Kcaron[]= "Kcaron";
static const char GLYPH_Kcircle[]= "Kcircle";
static const char GLYPH_Kdotbelow[]= "Kdotbelow";
static const char GLYPH_Keharmenian[]= "Keharmenian";
static const char GLYPH_Kenarmenian[]= "Kenarmenian";
static const char GLYPH_Kheicoptic[]= "Kheicoptic";
static const char GLYPH_Khook[]= "Khook";
static const char GLYPH_Klinebelow[]= "Klinebelow";
static const char GLYPH_Kmonospace[]= "Kmonospace";
static const char GLYPH_Koppacyrillic[]= "Koppacyrillic";
static const char GLYPH_Koppagreek[]= "Koppagreek";
static const char GLYPH_Ksicyrillic[]= "Ksicyrillic";
static const char GLYPH_Ksmall[]= "Ksmall";
static const char GLYPH_LJ[]= "LJ";
static const char GLYPH_LL[]= "LL";
static const char GLYPH_Lcircle[]= "Lcircle";
static const char GLYPH_Lcircumflexbelow[]= "Lcircumflexbelow";
static const char GLYPH_Ldotbelow[]= "Ldotbelow";
static const char GLYPH_Ldotbelowmacron[]= "Ldotbelowmacron";
static const char GLYPH_Liwnarmenian[]= "Liwnarmenian";
static const char GLYPH_Lj[]= "Lj";
static const char GLYPH_Llinebelow[]= "Llinebelow";
static const char GLYPH_Lmonospace[]= "Lmonospace";
static const char GLYPH_Lslashsmall[]= "Lslashsmall";
static const char GLYPH_Lsmall[]= "Lsmall";
static const char GLYPH_MBsquare[]= "MBsquare";
static const char GLYPH_Macron[]= "Macron";
static const char GLYPH_Macronsmall[]= "Macronsmall";
static const char GLYPH_Macute[]= "Macute";
static const char GLYPH_Mcircle[]= "Mcircle";
static const char GLYPH_Mdotaccent[]= "Mdotaccent";
static const char GLYPH_Mdotbelow[]= "Mdotbelow";
static const char GLYPH_Menarmenian[]= "Menarmenian";
static const char GLYPH_Mmonospace[]= "Mmonospace";
static const char GLYPH_Msmall[]= "Msmall";
static const char GLYPH_Mturned[]= "Mturned";
static const char GLYPH_NJ[]= "NJ";
static const char GLYPH_Ncircle[]= "Ncircle";
static const char GLYPH_Ncircumflexbelow[]= "Ncircumflexbelow";
static const char GLYPH_Ndotaccent[]= "Ndotaccent";
static const char GLYPH_Ndotbelow[]= "Ndotbelow";
static const char GLYPH_Nhookleft[]= "Nhookleft";
static const char GLYPH_Nineroman[]= "Nineroman";
static const char GLYPH_Nj[]= "Nj";
static const char GLYPH_Nlinebelow[]= "Nlinebelow";
static const char GLYPH_Nmonospace[]= "Nmonospace";
static const char GLYPH_Nowarmenian[]= "Nowarmenian";
static const char GLYPH_Nsmall[]= "Nsmall";
static const char GLYPH_Ntildesmall[]= "Ntildesmall";
static const char GLYPH_OEsmall[]= "OEsmall";
static const char GLYPH_Oacutesmall[]= "Oacutesmall";
static const char GLYPH_Obarredcyrillic[]= "Obarredcyrillic";
static const char GLYPH_Obarreddieresiscyrillic[]= "Obarreddieresiscyrillic";
static const char GLYPH_Ocaron[]= "Ocaron";
static const char GLYPH_Ocenteredtilde[]= "Ocenteredtilde";
static const char GLYPH_Ocircle[]= "Ocircle";
static const char GLYPH_Ocircumflexacute[]= "Ocircumflexacute";
static const char GLYPH_Ocircumflexdotbelow[]= "Ocircumflexdotbelow";
static const char GLYPH_Ocircumflexgrave[]= "Ocircumflexgrave";
static const char GLYPH_Ocircumflexhookabove[]= "Ocircumflexhookabove";
static const char GLYPH_Ocircumflexsmall[]= "Ocircumflexsmall";
static const char GLYPH_Ocircumflextilde[]= "Ocircumflextilde";
static const char GLYPH_Odblgrave[]= "Odblgrave";
static const char GLYPH_Odieresiscyrillic[]= "Odieresiscyrillic";
static const char GLYPH_Odieresissmall[]= "Odieresissmall";
static const char GLYPH_Odotbelow[]= "Odotbelow";
static const char GLYPH_Ogoneksmall[]= "Ogoneksmall";
static const char GLYPH_Ogravesmall[]= "Ogravesmall";
static const char GLYPH_Oharmenian[]= "Oharmenian";
static const char GLYPH_Ohookabove[]= "Ohookabove";
static const char GLYPH_Ohornacute[]= "Ohornacute";
static const char GLYPH_Ohorndotbelow[]= "Ohorndotbelow";
static const char GLYPH_Ohorngrave[]= "Ohorngrave";
static const char GLYPH_Ohornhookabove[]= "Ohornhookabove";
static const char GLYPH_Ohorntilde[]= "Ohorntilde";
static const char GLYPH_Oi[]= "Oi";
static const char GLYPH_Oinvertedbreve[]= "Oinvertedbreve";
static const char GLYPH_Omacronacute[]= "Omacronacute";
static const char GLYPH_Omacrongrave[]= "Omacrongrave";
static const char GLYPH_Omegacyrillic[]= "Omegacyrillic";
static const char GLYPH_Omegagreek[]= "Omegagreek";
static const char GLYPH_Omegaroundcyrillic[]= "Omegaroundcyrillic";
static const char GLYPH_Omegatitlocyrillic[]= "Omegatitlocyrillic";
static const char GLYPH_Omonospace[]= "Omonospace";
static const char GLYPH_Oneroman[]= "Oneroman";
static const char GLYPH_Oogonek[]= "Oogonek";
static const char GLYPH_Oogonekmacron[]= "Oogonekmacron";
static const char GLYPH_Oopen[]= "Oopen";
static const char GLYPH_Oslashsmall[]= "Oslashsmall";
static const char GLYPH_Osmall[]= "Osmall";
static const char GLYPH_Otcyrillic[]= "Otcyrillic";
static const char GLYPH_Otildeacute[]= "Otildeacute";
static const char GLYPH_Otildedieresis[]= "Otildedieresis";
static const char GLYPH_Otildesmall[]= "Otildesmall";
static const char GLYPH_Pacute[]= "Pacute";
static const char GLYPH_Pcircle[]= "Pcircle";
static const char GLYPH_Pdotaccent[]= "Pdotaccent";
static const char GLYPH_Peharmenian[]= "Peharmenian";
static const char GLYPH_Pemiddlehookcyrillic[]= "Pemiddlehookcyrillic";
static const char GLYPH_Phook[]= "Phook";
static const char GLYPH_Piwrarmenian[]= "Piwrarmenian";
static const char GLYPH_Pmonospace[]= "Pmonospace";
static const char GLYPH_Psicyrillic[]= "Psicyrillic";
static const char GLYPH_Psmall[]= "Psmall";
static const char GLYPH_Qcircle[]= "Qcircle";
static const char GLYPH_Qmonospace[]= "Qmonospace";
static const char GLYPH_Qsmall[]= "Qsmall";
static const char GLYPH_Raarmenian[]= "Raarmenian";
static const char GLYPH_Rcircle[]= "Rcircle";
static const char GLYPH_Rdblgrave[]= "Rdblgrave";
static const char GLYPH_Rdotaccent[]= "Rdotaccent";
static const char GLYPH_Rdotbelow[]= "Rdotbelow";
static const char GLYPH_Rdotbelowmacron[]= "Rdotbelowmacron";
static const char GLYPH_Reharmenian[]= "Reharmenian";
static const char GLYPH_Ringsmall[]= "Ringsmall";
static const char GLYPH_Rinvertedbreve[]= "Rinvertedbreve";
static const char GLYPH_Rlinebelow[]= "Rlinebelow";
static const char GLYPH_Rmonospace[]= "Rmonospace";
static const char GLYPH_Rsmall[]= "Rsmall";
static const char GLYPH_Rsmallinverted[]= "Rsmallinverted";
static const char GLYPH_Rsmallinvertedsuperior[]= "Rsmallinvertedsuperior";
static const char GLYPH_Sacutedotaccent[]= "Sacutedotaccent";
static const char GLYPH_Sampigreek[]= "Sampigreek";
static const char GLYPH_Scarondotaccent[]= "Scarondotaccent";
static const char GLYPH_Scaronsmall[]= "Scaronsmall";
static const char GLYPH_Schwa[]= "Schwa";
static const char GLYPH_Schwacyrillic[]= "Schwacyrillic";
static const char GLYPH_Schwadieresiscyrillic[]= "Schwadieresiscyrillic";
static const char GLYPH_Scircle[]= "Scircle";
static const char GLYPH_Sdotaccent[]= "Sdotaccent";
static const char GLYPH_Sdotbelow[]= "Sdotbelow";
static const char GLYPH_Sdotbelowdotaccent[]= "Sdotbelowdotaccent";
static const char GLYPH_Seharmenian[]= "Seharmenian";
static const char GLYPH_Sevenroman[]= "Sevenroman";
static const char GLYPH_Shaarmenian[]= "Shaarmenian";
static const char GLYPH_Sheicoptic[]= "Sheicoptic";
static const char GLYPH_Shhacyrillic[]= "Shhacyrillic";
static const char GLYPH_Shimacoptic[]= "Shimacoptic";
static const char GLYPH_Sixroman[]= "Sixroman";
static const char GLYPH_Smonospace[]= "Smonospace";
static const char GLYPH_Ssmall[]= "Ssmall";
static const char GLYPH_Stigmagreek[]= "Stigmagreek";
static const char GLYPH_Tcircle[]= "Tcircle";
static const char GLYPH_Tcircumflexbelow[]= "Tcircumflexbelow";
static const char GLYPH_Tdotaccent[]= "Tdotaccent";
static const char GLYPH_Tdotbelow[]= "Tdotbelow";
static const char GLYPH_Tedescendercyrillic[]= "Tedescendercyrillic";
static const char GLYPH_Tenroman[]= "Tenroman";
static const char GLYPH_Tetsecyrillic[]= "Tetsecyrillic";
static const char GLYPH_Thook[]= "Thook";
static const char GLYPH_Thornsmall[]= "Thornsmall";
static const char GLYPH_Threeroman[]= "Threeroman";
static const char GLYPH_Tildesmall[]= "Tildesmall";
static const char GLYPH_Tiwnarmenian[]= "Tiwnarmenian";
static const char GLYPH_Tlinebelow[]= "Tlinebelow";
static const char GLYPH_Tmonospace[]= "Tmonospace";
static const char GLYPH_Toarmenian[]= "Toarmenian";
static const char GLYPH_Tonefive[]= "Tonefive";
static const char GLYPH_Tonesix[]= "Tonesix";
static const char GLYPH_Tonetwo[]= "Tonetwo";
static const char GLYPH_Tretroflexhook[]= "Tretroflexhook";
static const char GLYPH_Tsmall[]= "Tsmall";
static const char GLYPH_Twelveroman[]= "Twelveroman";
static const char GLYPH_Tworoman[]= "Tworoman";
static const char GLYPH_Uacutesmall[]= "Uacutesmall";
static const char GLYPH_Ucaron[]= "Ucaron";
static const char GLYPH_Ucircle[]= "Ucircle";
static const char GLYPH_Ucircumflexbelow[]= "Ucircumflexbelow";
static const char GLYPH_Ucircumflexsmall[]= "Ucircumflexsmall";
static const char GLYPH_Udblgrave[]= "Udblgrave";
static const char GLYPH_Udieresisacute[]= "Udieresisacute";
static const char GLYPH_Udieresisbelow[]= "Udieresisbelow";
static const char GLYPH_Udieresiscaron[]= "Udieresiscaron";
static const char GLYPH_Udieresiscyrillic[]= "Udieresiscyrillic";
static const char GLYPH_Udieresisgrave[]= "Udieresisgrave";
static const char GLYPH_Udieresismacron[]= "Udieresismacron";
static const char GLYPH_Udieresissmall[]= "Udieresissmall";
static const char GLYPH_Udotbelow[]= "Udotbelow";
static const char GLYPH_Ugravesmall[]= "Ugravesmall";
static const char GLYPH_Uhookabove[]= "Uhookabove";
static const char GLYPH_Uhornacute[]= "Uhornacute";
static const char GLYPH_Uhorndotbelow[]= "Uhorndotbelow";
static const char GLYPH_Uhorngrave[]= "Uhorngrave";
static const char GLYPH_Uhornhookabove[]= "Uhornhookabove";
static const char GLYPH_Uhorntilde[]= "Uhorntilde";
static const char GLYPH_Uhungarumlautcyrillic[]= "Uhungarumlautcyrillic";
static const char GLYPH_Uinvertedbreve[]= "Uinvertedbreve";
static const char GLYPH_Ukcyrillic[]= "Ukcyrillic";
static const char GLYPH_Umacroncyrillic[]= "Umacroncyrillic";
static const char GLYPH_Umacrondieresis[]= "Umacrondieresis";
static const char GLYPH_Umonospace[]= "Umonospace";
static const char GLYPH_Upsilonacutehooksymbolgreek[]= "Upsilonacutehooksymbolgreek";
static const char GLYPH_Upsilonafrican[]= "Upsilonafrican";
static const char GLYPH_Upsilondieresishooksymbolgreek[]= "Upsilondieresishooksymbolgreek";
static const char GLYPH_Usmall[]= "Usmall";
static const char GLYPH_Ustraightcyrillic[]= "Ustraightcyrillic";
static const char GLYPH_Ustraightstrokecyrillic[]= "Ustraightstrokecyrillic";
static const char GLYPH_Utildeacute[]= "Utildeacute";
static const char GLYPH_Utildebelow[]= "Utildebelow";
static const char GLYPH_Vcircle[]= "Vcircle";
static const char GLYPH_Vdotbelow[]= "Vdotbelow";
static const char GLYPH_Vewarmenian[]= "Vewarmenian";
static const char GLYPH_Vhook[]= "Vhook";
static const char GLYPH_Vmonospace[]= "Vmonospace";
static const char GLYPH_Voarmenian[]= "Voarmenian";
static const char GLYPH_Vsmall[]= "Vsmall";
static const char GLYPH_Vtilde[]= "Vtilde";
static const char GLYPH_Wcircle[]= "Wcircle";
static const char GLYPH_Wdotaccent[]= "Wdotaccent";
static const char GLYPH_Wdotbelow[]= "Wdotbelow";
static const char GLYPH_Wmonospace[]= "Wmonospace";
static const char GLYPH_Wsmall[]= "Wsmall";
static const char GLYPH_Xcircle[]= "Xcircle";
static const char GLYPH_Xdieresis[]= "Xdieresis";
static const char GLYPH_Xdotaccent[]= "Xdotaccent";
static const char GLYPH_Xeharmenian[]= "Xeharmenian";
static const char GLYPH_Xmonospace[]= "Xmonospace";
static const char GLYPH_Xsmall[]= "Xsmall";
static const char GLYPH_Yacutesmall[]= "Yacutesmall";
static const char GLYPH_Ycircle[]= "Ycircle";
static const char GLYPH_Ydieresissmall[]= "Ydieresissmall";
static const char GLYPH_Ydotaccent[]= "Ydotaccent";
static const char GLYPH_Ydotbelow[]= "Ydotbelow";
static const char GLYPH_Yerudieresiscyrillic[]= "Yerudieresiscyrillic";
static const char GLYPH_Yhook[]= "Yhook";
static const char GLYPH_Yhookabove[]= "Yhookabove";
static const char GLYPH_Yiarmenian[]= "Yiarmenian";
static const char GLYPH_Yiwnarmenian[]= "Yiwnarmenian";
static const char GLYPH_Ymonospace[]= "Ymonospace";
static const char GLYPH_Ysmall[]= "Ysmall";
static const char GLYPH_Ytilde[]= "Ytilde";
static const char GLYPH_Yusbigcyrillic[]= "Yusbigcyrillic";
static const char GLYPH_Yusbigiotifiedcyrillic[]= "Yusbigiotifiedcyrillic";
static const char GLYPH_Yuslittlecyrillic[]= "Yuslittlecyrillic";
static const char GLYPH_Yuslittleiotifiedcyrillic[]= "Yuslittleiotifiedcyrillic";
static const char GLYPH_Zaarmenian[]= "Zaarmenian";
static const char GLYPH_Zcaronsmall[]= "Zcaronsmall";
static const char GLYPH_Zcircle[]= "Zcircle";
static const char GLYPH_Zcircumflex[]= "Zcircumflex";
static const char GLYPH_Zdotbelow[]= "Zdotbelow";
static const char GLYPH_Zedescendercyrillic[]= "Zedescendercyrillic";
static const char GLYPH_Zedieresiscyrillic[]= "Zedieresiscyrillic";
static const char GLYPH_Zhearmenian[]= "Zhearmenian";
static const char GLYPH_Zhebrevecyrillic[]= "Zhebrevecyrillic";
static const char GLYPH_Zhedescendercyrillic[]= "Zhedescendercyrillic";
static const char GLYPH_Zhedieresiscyrillic[]= "Zhedieresiscyrillic";
static const char GLYPH_Zlinebelow[]= "Zlinebelow";
static const char GLYPH_Zmonospace[]= "Zmonospace";
static const char GLYPH_Zsmall[]= "Zsmall";
static const char GLYPH_Zstroke[]= "Zstroke";
static const char GLYPH_aabengali[]= "aabengali";
static const char GLYPH_aadeva[]= "aadeva";
static const char GLYPH_aagujarati[]= "aagujarati";
static const char GLYPH_aagurmukhi[]= "aagurmukhi";
static const char GLYPH_aamatragurmukhi[]= "aamatragurmukhi";
static const char GLYPH_aarusquare[]= "aarusquare";
static const char GLYPH_aavowelsignbengali[]= "aavowelsignbengali";
static const char GLYPH_aavowelsigndeva[]= "aavowelsigndeva";
static const char GLYPH_aavowelsigngujarati[]= "aavowelsigngujarati";
static const char GLYPH_abbreviationmarkarmenian[]= "abbreviationmarkarmenian";
static const char GLYPH_abbreviationsigndeva[]= "abbreviationsigndeva";
static const char GLYPH_abengali[]= "abengali";
static const char GLYPH_abopomofo[]= "abopomofo";
static const char GLYPH_abreveacute[]= "abreveacute";
static const char GLYPH_abrevecyrillic[]= "abrevecyrillic";
static const char GLYPH_abrevedotbelow[]= "abrevedotbelow";
static const char GLYPH_abrevegrave[]= "abrevegrave";
static const char GLYPH_abrevehookabove[]= "abrevehookabove";
static const char GLYPH_abrevetilde[]= "abrevetilde";
static const char GLYPH_acaron[]= "acaron";
static const char GLYPH_acircle[]= "acircle";
static const char GLYPH_acircumflexacute[]= "acircumflexacute";
static const char GLYPH_acircumflexdotbelow[]= "acircumflexdotbelow";
static const char GLYPH_acircumflexgrave[]= "acircumflexgrave";
static const char GLYPH_acircumflexhookabove[]= "acircumflexhookabove";
static const char GLYPH_acircumflextilde[]= "acircumflextilde";
static const char GLYPH_acutebelowcmb[]= "acutebelowcmb";
static const char GLYPH_acutedeva[]= "acutedeva";
static const char GLYPH_acutelowmod[]= "acutelowmod";
static const char GLYPH_acutetonecmb[]= "acutetonecmb";
static const char GLYPH_adblgrave[]= "adblgrave";
static const char GLYPH_addakgurmukhi[]= "addakgurmukhi";
static const char GLYPH_adeva[]= "adeva";
static const char GLYPH_adieresiscyrillic[]= "adieresiscyrillic";
static const char GLYPH_adieresismacron[]= "adieresismacron";
static const char GLYPH_adotbelow[]= "adotbelow";
static const char GLYPH_adotmacron[]= "adotmacron";
static const char GLYPH_aekorean[]= "aekorean";
static const char GLYPH_aemacron[]= "aemacron";
static const char GLYPH_afii10063[]= "afii10063";
static const char GLYPH_afii10064[]= "afii10064";
static const char GLYPH_afii10192[]= "afii10192";
static const char GLYPH_afii10831[]= "afii10831";
static const char GLYPH_afii10832[]= "afii10832";
static const char GLYPH_afii57694[]= "afii57694";
static const char GLYPH_afii57695[]= "afii57695";
static const char GLYPH_afii57700[]= "afii57700";
static const char GLYPH_afii57705[]= "afii57705";
static const char GLYPH_afii57723[]= "afii57723";
static const char GLYPH_agujarati[]= "agujarati";
static const char GLYPH_agurmukhi[]= "agurmukhi";
static const char GLYPH_ahiragana[]= "ahiragana";
static const char GLYPH_ahookabove[]= "ahookabove";
static const char GLYPH_aibengali[]= "aibengali";
static const char GLYPH_aibopomofo[]= "aibopomofo";
static const char GLYPH_aideva[]= "aideva";
static const char GLYPH_aiecyrillic[]= "aiecyrillic";
static const char GLYPH_aigujarati[]= "aigujarati";
static const char GLYPH_aigurmukhi[]= "aigurmukhi";
static const char GLYPH_aimatragurmukhi[]= "aimatragurmukhi";
static const char GLYPH_ainfinalarabic[]= "ainfinalarabic";
static const char GLYPH_aininitialarabic[]= "aininitialarabic";
static const char GLYPH_ainmedialarabic[]= "ainmedialarabic";
static const char GLYPH_ainvertedbreve[]= "ainvertedbreve";
static const char GLYPH_aivowelsignbengali[]= "aivowelsignbengali";
static const char GLYPH_aivowelsigndeva[]= "aivowelsigndeva";
static const char GLYPH_aivowelsigngujarati[]= "aivowelsigngujarati";
static const char GLYPH_akatakana[]= "akatakana";
static const char GLYPH_akatakanahalfwidth[]= "akatakanahalfwidth";
static const char GLYPH_akorean[]= "akorean";
static const char GLYPH_alefdageshhebrew[]= "alefdageshhebrew";
static const char GLYPH_aleffinalarabic[]= "aleffinalarabic";
static const char GLYPH_alefhamzaabovefinalarabic[]= "alefhamzaabovefinalarabic";
static const char GLYPH_alefhamzabelowfinalarabic[]= "alefhamzabelowfinalarabic";
static const char GLYPH_aleflamedhebrew[]= "aleflamedhebrew";
static const char GLYPH_alefmaddaabovefinalarabic[]= "alefmaddaabovefinalarabic";
static const char GLYPH_alefmaksurafinalarabic[]= "alefmaksurafinalarabic";
static const char GLYPH_alefmaksurainitialarabic[]= "alefmaksurainitialarabic";
static const char GLYPH_alefmaksuramedialarabic[]= "alefmaksuramedialarabic";
static const char GLYPH_alefpatahhebrew[]= "alefpatahhebrew";
static const char GLYPH_alefqamatshebrew[]= "alefqamatshebrew";
static const char GLYPH_allequal[]= "allequal";
static const char GLYPH_amonospace[]= "amonospace";
static const char GLYPH_ampersandmonospace[]= "ampersandmonospace";
static const char GLYPH_ampersandsmall[]= "ampersandsmall";
static const char GLYPH_amsquare[]= "amsquare";
static const char GLYPH_anbopomofo[]= "anbopomofo";
static const char GLYPH_angbopomofo[]= "angbopomofo";
static const char GLYPH_angkhankhuthai[]= "angkhankhuthai";
static const char GLYPH_anglebracketleft[]= "anglebracketleft";
static const char GLYPH_anglebracketleftvertical[]= "anglebracketleftvertical";
static const char GLYPH_anglebracketright[]= "anglebracketright";
static const char GLYPH_anglebracketrightvertical[]= "anglebracketrightvertical";
static const char GLYPH_angstrom[]= "angstrom";
static const char GLYPH_anudattadeva[]= "anudattadeva";
static const char GLYPH_anusvarabengali[]= "anusvarabengali";
static const char GLYPH_anusvaradeva[]= "anusvaradeva";
static const char GLYPH_anusvaragujarati[]= "anusvaragujarati";
static const char GLYPH_apaatosquare[]= "apaatosquare";
static const char GLYPH_aparen[]= "aparen";
static const char GLYPH_apostrophearmenian[]= "apostrophearmenian";
static const char GLYPH_apple[]= "apple";
static const char GLYPH_approaches[]= "approaches";
static const char GLYPH_approxequalorimage[]= "approxequalorimage";
static const char GLYPH_araeaekorean[]= "araeaekorean";
static const char GLYPH_araeakorean[]= "araeakorean";
static const char GLYPH_arc[]= "arc";
static const char GLYPH_arighthalfring[]= "arighthalfring";
static const char GLYPH_aringbelow[]= "aringbelow";
static const char GLYPH_arrowdashdown[]= "arrowdashdown";
static const char GLYPH_arrowdashleft[]= "arrowdashleft";
static const char GLYPH_arrowdashright[]= "arrowdashright";
static const char GLYPH_arrowdashup[]= "arrowdashup";
static const char GLYPH_arrowdownleft[]= "arrowdownleft";
static const char GLYPH_arrowdownright[]= "arrowdownright";
static const char GLYPH_arrowdownwhite[]= "arrowdownwhite";
static const char GLYPH_arrowheaddownmod[]= "arrowheaddownmod";
static const char GLYPH_arrowheadleftmod[]= "arrowheadleftmod";
static const char GLYPH_arrowheadrightmod[]= "arrowheadrightmod";
static const char GLYPH_arrowheadupmod[]= "arrowheadupmod";
static const char GLYPH_arrowleftdblstroke[]= "arrowleftdblstroke";
static const char GLYPH_arrowleftoverright[]= "arrowleftoverright";
static const char GLYPH_arrowleftwhite[]= "arrowleftwhite";
static const char GLYPH_arrowrightdblstroke[]= "arrowrightdblstroke";
static const char GLYPH_arrowrightheavy[]= "arrowrightheavy";
static const char GLYPH_arrowrightoverleft[]= "arrowrightoverleft";
static const char GLYPH_arrowrightwhite[]= "arrowrightwhite";
static const char GLYPH_arrowtableft[]= "arrowtableft";
static const char GLYPH_arrowtabright[]= "arrowtabright";
static const char GLYPH_arrowupleft[]= "arrowupleft";
static const char GLYPH_arrowupleftofdown[]= "arrowupleftofdown";
static const char GLYPH_arrowupright[]= "arrowupright";
static const char GLYPH_arrowupwhite[]= "arrowupwhite";
static const char GLYPH_asciicircummonospace[]= "asciicircummonospace";
static const char GLYPH_asciitildemonospace[]= "asciitildemonospace";
static const char GLYPH_ascript[]= "ascript";
static const char GLYPH_ascriptturned[]= "ascriptturned";
static const char GLYPH_asmallhiragana[]= "asmallhiragana";
static const char GLYPH_asmallkatakana[]= "asmallkatakana";
static const char GLYPH_asmallkatakanahalfwidth[]= "asmallkatakanahalfwidth";
static const char GLYPH_asteriskmonospace[]= "asteriskmonospace";
static const char GLYPH_asterisksmall[]= "asterisksmall";
static const char GLYPH_asterism[]= "asterism";
static const char GLYPH_asuperior[]= "asuperior";
static const char GLYPH_asymptoticallyequal[]= "asymptoticallyequal";
static const char GLYPH_atmonospace[]= "atmonospace";
static const char GLYPH_atsmall[]= "atsmall";
static const char GLYPH_aturned[]= "aturned";
static const char GLYPH_aubengali[]= "aubengali";
static const char GLYPH_aubopomofo[]= "aubopomofo";
static const char GLYPH_audeva[]= "audeva";
static const char GLYPH_augujarati[]= "augujarati";
static const char GLYPH_augurmukhi[]= "augurmukhi";
static const char GLYPH_aulengthmarkbengali[]= "aulengthmarkbengali";
static const char GLYPH_aumatragurmukhi[]= "aumatragurmukhi";
static const char GLYPH_auvowelsignbengali[]= "auvowelsignbengali";
static const char GLYPH_auvowelsigndeva[]= "auvowelsigndeva";
static const char GLYPH_auvowelsigngujarati[]= "auvowelsigngujarati";
static const char GLYPH_avagrahadeva[]= "avagrahadeva";
static const char GLYPH_aybarmenian[]= "aybarmenian";
static const char GLYPH_ayinaltonehebrew[]= "ayinaltonehebrew";
static const char GLYPH_babengali[]= "babengali";
static const char GLYPH_backslashmonospace[]= "backslashmonospace";
static const char GLYPH_badeva[]= "badeva";
static const char GLYPH_bagujarati[]= "bagujarati";
static const char GLYPH_bagurmukhi[]= "bagurmukhi";
static const char GLYPH_bahiragana[]= "bahiragana";
static const char GLYPH_bahtthai[]= "bahtthai";
static const char GLYPH_bakatakana[]= "bakatakana";
static const char GLYPH_barmonospace[]= "barmonospace";
static const char GLYPH_bbopomofo[]= "bbopomofo";
static const char GLYPH_bcircle[]= "bcircle";
static const char GLYPH_bdotaccent[]= "bdotaccent";
static const char GLYPH_bdotbelow[]= "bdotbelow";
static const char GLYPH_beamedsixteenthnotes[]= "beamedsixteenthnotes";
static const char GLYPH_because[]= "because";
static const char GLYPH_behfinalarabic[]= "behfinalarabic";
static const char GLYPH_behinitialarabic[]= "behinitialarabic";
static const char GLYPH_behiragana[]= "behiragana";
static const char GLYPH_behmedialarabic[]= "behmedialarabic";
static const char GLYPH_behmeeminitialarabic[]= "behmeeminitialarabic";
static const char GLYPH_behmeemisolatedarabic[]= "behmeemisolatedarabic";
static const char GLYPH_behnoonfinalarabic[]= "behnoonfinalarabic";
static const char GLYPH_bekatakana[]= "bekatakana";
static const char GLYPH_benarmenian[]= "benarmenian";
static const char GLYPH_betasymbolgreek[]= "betasymbolgreek";
static const char GLYPH_betdagesh[]= "betdagesh";
static const char GLYPH_betdageshhebrew[]= "betdageshhebrew";
static const char GLYPH_betrafehebrew[]= "betrafehebrew";
static const char GLYPH_bhabengali[]= "bhabengali";
static const char GLYPH_bhadeva[]= "bhadeva";
static const char GLYPH_bhagujarati[]= "bhagujarati";
static const char GLYPH_bhagurmukhi[]= "bhagurmukhi";
static const char GLYPH_bhook[]= "bhook";
static const char GLYPH_bihiragana[]= "bihiragana";
static const char GLYPH_bikatakana[]= "bikatakana";
static const char GLYPH_bilabialclick[]= "bilabialclick";
static const char GLYPH_bindigurmukhi[]= "bindigurmukhi";
static const char GLYPH_birusquare[]= "birusquare";
static const char GLYPH_blackdiamond[]= "blackdiamond";
static const char GLYPH_blackleftpointingtriangle[]= "blackleftpointingtriangle";
static const char GLYPH_blacklenticularbracketleft[]= "blacklenticularbracketleft";
static const char GLYPH_blacklenticularbracketleftvertical[]= "blacklenticularbracketleftvertical";
static const char GLYPH_blacklenticularbracketright[]= "blacklenticularbracketright";
static const char GLYPH_blacklenticularbracketrightvertical[]= "blacklenticularbracketrightvertical";
static const char GLYPH_blacklowerlefttriangle[]= "blacklowerlefttriangle";
static const char GLYPH_blacklowerrighttriangle[]= "blacklowerrighttriangle";
static const char GLYPH_blackrightpointingtriangle[]= "blackrightpointingtriangle";
static const char GLYPH_blackstar[]= "blackstar";
static const char GLYPH_blackupperlefttriangle[]= "blackupperlefttriangle";
static const char GLYPH_blackupperrighttriangle[]= "blackupperrighttriangle";
static const char GLYPH_blackuppointingsmalltriangle[]= "blackuppointingsmalltriangle";
static const char GLYPH_blank[]= "blank";
static const char GLYPH_blinebelow[]= "blinebelow";
static const char GLYPH_bmonospace[]= "bmonospace";
static const char GLYPH_bobaimaithai[]= "bobaimaithai";
static const char GLYPH_bohiragana[]= "bohiragana";
static const char GLYPH_bokatakana[]= "bokatakana";
static const char GLYPH_bparen[]= "bparen";
static const char GLYPH_bqsquare[]= "bqsquare";
static const char GLYPH_braceleftmonospace[]= "braceleftmonospace";
static const char GLYPH_braceleftsmall[]= "braceleftsmall";
static const char GLYPH_braceleftvertical[]= "braceleftvertical";
static const char GLYPH_bracerightmonospace[]= "bracerightmonospace";
static const char GLYPH_bracerightsmall[]= "bracerightsmall";
static const char GLYPH_bracerightvertical[]= "bracerightvertical";
static const char GLYPH_bracketleftmonospace[]= "bracketleftmonospace";
static const char GLYPH_bracketrightmonospace[]= "bracketrightmonospace";
static const char GLYPH_brevebelowcmb[]= "brevebelowcmb";
static const char GLYPH_brevecmb[]= "brevecmb";
static const char GLYPH_breveinvertedbelowcmb[]= "breveinvertedbelowcmb";
static const char GLYPH_breveinvertedcmb[]= "breveinvertedcmb";
static const char GLYPH_breveinverteddoublecmb[]= "breveinverteddoublecmb";
static const char GLYPH_bridgebelowcmb[]= "bridgebelowcmb";
static const char GLYPH_bridgeinvertedbelowcmb[]= "bridgeinvertedbelowcmb";
static const char GLYPH_bstroke[]= "bstroke";
static const char GLYPH_bsuperior[]= "bsuperior";
static const char GLYPH_btopbar[]= "btopbar";
static const char GLYPH_buhiragana[]= "buhiragana";
static const char GLYPH_bukatakana[]= "bukatakana";
static const char GLYPH_bulletoperator[]= "bulletoperator";
static const char GLYPH_bullseye[]= "bullseye";
static const char GLYPH_caarmenian[]= "caarmenian";
static const char GLYPH_cabengali[]= "cabengali";
static const char GLYPH_cadeva[]= "cadeva";
static const char GLYPH_cagujarati[]= "cagujarati";
static const char GLYPH_cagurmukhi[]= "cagurmukhi";
static const char GLYPH_calsquare[]= "calsquare";
static const char GLYPH_candrabindubengali[]= "candrabindubengali";
static const char GLYPH_candrabinducmb[]= "candrabinducmb";
static const char GLYPH_candrabindudeva[]= "candrabindudeva";
static const char GLYPH_candrabindugujarati[]= "candrabindugujarati";
static const char GLYPH_capslock[]= "capslock";
static const char GLYPH_caronbelowcmb[]= "caronbelowcmb";
static const char GLYPH_caroncmb[]= "caroncmb";
static const char GLYPH_cbopomofo[]= "cbopomofo";
static const char GLYPH_ccedillaacute[]= "ccedillaacute";
static const char GLYPH_ccircle[]= "ccircle";
static const char GLYPH_ccurl[]= "ccurl";
static const char GLYPH_cdsquare[]= "cdsquare";
static const char GLYPH_cedillacmb[]= "cedillacmb";
static const char GLYPH_centigrade[]= "centigrade";
static const char GLYPH_centinferior[]= "centinferior";
static const char GLYPH_centmonospace[]= "centmonospace";
static const char GLYPH_centoldstyle[]= "centoldstyle";
static const char GLYPH_centsuperior[]= "centsuperior";
static const char GLYPH_chaarmenian[]= "chaarmenian";
static const char GLYPH_chabengali[]= "chabengali";
static const char GLYPH_chadeva[]= "chadeva";
static const char GLYPH_chagujarati[]= "chagujarati";
static const char GLYPH_chagurmukhi[]= "chagurmukhi";
static const char GLYPH_chbopomofo[]= "chbopomofo";
static const char GLYPH_cheabkhasiancyrillic[]= "cheabkhasiancyrillic";
static const char GLYPH_checkmark[]= "checkmark";
static const char GLYPH_chedescenderabkhasiancyrillic[]= "chedescenderabkhasiancyrillic";
static const char GLYPH_chedescendercyrillic[]= "chedescendercyrillic";
static const char GLYPH_chedieresiscyrillic[]= "chedieresiscyrillic";
static const char GLYPH_cheharmenian[]= "cheharmenian";
static const char GLYPH_chekhakassiancyrillic[]= "chekhakassiancyrillic";
static const char GLYPH_cheverticalstrokecyrillic[]= "cheverticalstrokecyrillic";
static const char GLYPH_chieuchacirclekorean[]= "chieuchacirclekorean";
static const char GLYPH_chieuchaparenkorean[]= "chieuchaparenkorean";
static const char GLYPH_chieuchcirclekorean[]= "chieuchcirclekorean";
static const char GLYPH_chieuchkorean[]= "chieuchkorean";
static const char GLYPH_chieuchparenkorean[]= "chieuchparenkorean";
static const char GLYPH_chochangthai[]= "chochangthai";
static const char GLYPH_chochanthai[]= "chochanthai";
static const char GLYPH_chochingthai[]= "chochingthai";
static const char GLYPH_chochoethai[]= "chochoethai";
static const char GLYPH_chook[]= "chook";
static const char GLYPH_cieucacirclekorean[]= "cieucacirclekorean";
static const char GLYPH_cieucaparenkorean[]= "cieucaparenkorean";
static const char GLYPH_cieuccirclekorean[]= "cieuccirclekorean";
static const char GLYPH_cieuckorean[]= "cieuckorean";
static const char GLYPH_cieucparenkorean[]= "cieucparenkorean";
static const char GLYPH_cieucuparenkorean[]= "cieucuparenkorean";
static const char GLYPH_circleot[]= "circleot";
static const char GLYPH_circlepostalmark[]= "circlepostalmark";
static const char GLYPH_circlewithlefthalfblack[]= "circlewithlefthalfblack";
static const char GLYPH_circlewithrighthalfblack[]= "circlewithrighthalfblack";
static const char GLYPH_circumflexbelowcmb[]= "circumflexbelowcmb";
static const char GLYPH_circumflexcmb[]= "circumflexcmb";
static const char GLYPH_clear[]= "clear";
static const char GLYPH_clickalveolar[]= "clickalveolar";
static const char GLYPH_clickdental[]= "clickdental";
static const char GLYPH_clicklateral[]= "clicklateral";
static const char GLYPH_clickretroflex[]= "clickretroflex";
static const char GLYPH_clubsuitwhite[]= "clubsuitwhite";
static const char GLYPH_cmcubedsquare[]= "cmcubedsquare";
static const char GLYPH_cmonospace[]= "cmonospace";
static const char GLYPH_cmsquaredsquare[]= "cmsquaredsquare";
static const char GLYPH_coarmenian[]= "coarmenian";
static const char GLYPH_colonmonospace[]= "colonmonospace";
static const char GLYPH_colonsmall[]= "colonsmall";
static const char GLYPH_colontriangularhalfmod[]= "colontriangularhalfmod";
static const char GLYPH_colontriangularmod[]= "colontriangularmod";
static const char GLYPH_commaabovecmb[]= "commaabovecmb";
static const char GLYPH_commaaboverightcmb[]= "commaaboverightcmb";
static const char GLYPH_commaarmenian[]= "commaarmenian";
static const char GLYPH_commainferior[]= "commainferior";
static const char GLYPH_commamonospace[]= "commamonospace";
static const char GLYPH_commareversedabovecmb[]= "commareversedabovecmb";
static const char GLYPH_commasmall[]= "commasmall";
static const char GLYPH_commasuperior[]= "commasuperior";
static const char GLYPH_commaturnedabovecmb[]= "commaturnedabovecmb";
static const char GLYPH_commaturnedmod[]= "commaturnedmod";
static const char GLYPH_contourintegral[]= "contourintegral";
static const char GLYPH_control[]= "control";
static const char GLYPH_controlACK[]= "controlACK";
static const char GLYPH_controlBEL[]= "controlBEL";
static const char GLYPH_controlBS[]= "controlBS";
static const char GLYPH_controlCAN[]= "controlCAN";
static const char GLYPH_controlCR[]= "controlCR";
static const char GLYPH_controlDC1[]= "controlDC1";
static const char GLYPH_controlDC2[]= "controlDC2";
static const char GLYPH_controlDC3[]= "controlDC3";
static const char GLYPH_controlDC4[]= "controlDC4";
static const char GLYPH_controlDEL[]= "controlDEL";
static const char GLYPH_controlDLE[]= "controlDLE";
static const char GLYPH_controlEM[]= "controlEM";
static const char GLYPH_controlENQ[]= "controlENQ";
static const char GLYPH_controlEOT[]= "controlEOT";
static const char GLYPH_controlESC[]= "controlESC";
static const char GLYPH_controlETB[]= "controlETB";
static const char GLYPH_controlETX[]= "controlETX";
static const char GLYPH_controlFF[]= "controlFF";
static const char GLYPH_controlFS[]= "controlFS";
static const char GLYPH_controlGS[]= "controlGS";
static const char GLYPH_controlHT[]= "controlHT";
static const char GLYPH_controlLF[]= "controlLF";
static const char GLYPH_controlNAK[]= "controlNAK";
static const char GLYPH_controlRS[]= "controlRS";
static const char GLYPH_controlSI[]= "controlSI";
static const char GLYPH_controlSO[]= "controlSO";
static const char GLYPH_controlSOT[]= "controlSOT";
static const char GLYPH_controlSTX[]= "controlSTX";
static const char GLYPH_controlSUB[]= "controlSUB";
static const char GLYPH_controlSYN[]= "controlSYN";
static const char GLYPH_controlUS[]= "controlUS";
static const char GLYPH_controlVT[]= "controlVT";
static const char GLYPH_cornerbracketleft[]= "cornerbracketleft";
static const char GLYPH_cornerbracketlefthalfwidth[]= "cornerbracketlefthalfwidth";
static const char GLYPH_cornerbracketleftvertical[]= "cornerbracketleftvertical";
static const char GLYPH_cornerbracketright[]= "cornerbracketright";
static const char GLYPH_cornerbracketrighthalfwidth[]= "cornerbracketrighthalfwidth";
static const char GLYPH_cornerbracketrightvertical[]= "cornerbracketrightvertical";
static const char GLYPH_corporationsquare[]= "corporationsquare";
static const char GLYPH_cosquare[]= "cosquare";
static const char GLYPH_coverkgsquare[]= "coverkgsquare";
static const char GLYPH_cparen[]= "cparen";
static const char GLYPH_cruzeiro[]= "cruzeiro";
static const char GLYPH_cstretched[]= "cstretched";
static const char GLYPH_curlyand[]= "curlyand";
static const char GLYPH_curlyor[]= "curlyor";
static const char GLYPH_cyrBreve[]= "cyrBreve";
static const char GLYPH_cyrFlex[]= "cyrFlex";
static const char GLYPH_cyrbreve[]= "cyrbreve";
static const char GLYPH_cyrflex[]= "cyrflex";
static const char GLYPH_daarmenian[]= "daarmenian";
static const char GLYPH_dabengali[]= "dabengali";
static const char GLYPH_dadeva[]= "dadeva";
static const char GLYPH_dadfinalarabic[]= "dadfinalarabic";
static const char GLYPH_dadinitialarabic[]= "dadinitialarabic";
static const char GLYPH_dadmedialarabic[]= "dadmedialarabic";
static const char GLYPH_dagujarati[]= "dagujarati";
static const char GLYPH_dagurmukhi[]= "dagurmukhi";
static const char GLYPH_dahiragana[]= "dahiragana";
static const char GLYPH_dakatakana[]= "dakatakana";
static const char GLYPH_daletdagesh[]= "daletdagesh";
static const char GLYPH_daletdageshhebrew[]= "daletdageshhebrew";
static const char GLYPH_dalfinalarabic[]= "dalfinalarabic";
static const char GLYPH_danda[]= "danda";
static const char GLYPH_dargahebrew[]= "dargahebrew";
static const char GLYPH_dargalefthebrew[]= "dargalefthebrew";
static const char GLYPH_dasiapneumatacyrilliccmb[]= "dasiapneumatacyrilliccmb";
static const char GLYPH_dblGrave[]= "dblGrave";
static const char GLYPH_dblanglebracketleft[]= "dblanglebracketleft";
static const char GLYPH_dblanglebracketleftvertical[]= "dblanglebracketleftvertical";
static const char GLYPH_dblanglebracketright[]= "dblanglebracketright";
static const char GLYPH_dblanglebracketrightvertical[]= "dblanglebracketrightvertical";
static const char GLYPH_dblarchinvertedbelowcmb[]= "dblarchinvertedbelowcmb";
static const char GLYPH_dbldanda[]= "dbldanda";
static const char GLYPH_dblgrave[]= "dblgrave";
static const char GLYPH_dblgravecmb[]= "dblgravecmb";
static const char GLYPH_dblintegral[]= "dblintegral";
static const char GLYPH_dbllowlinecmb[]= "dbllowlinecmb";
static const char GLYPH_dbloverlinecmb[]= "dbloverlinecmb";
static const char GLYPH_dblprimemod[]= "dblprimemod";
static const char GLYPH_dblverticalbar[]= "dblverticalbar";
static const char GLYPH_dblverticallineabovecmb[]= "dblverticallineabovecmb";
static const char GLYPH_dbopomofo[]= "dbopomofo";
static const char GLYPH_dbsquare[]= "dbsquare";
static const char GLYPH_dcedilla[]= "dcedilla";
static const char GLYPH_dcircle[]= "dcircle";
static const char GLYPH_dcircumflexbelow[]= "dcircumflexbelow";
static const char GLYPH_ddabengali[]= "ddabengali";
static const char GLYPH_ddadeva[]= "ddadeva";
static const char GLYPH_ddagujarati[]= "ddagujarati";
static const char GLYPH_ddagurmukhi[]= "ddagurmukhi";
static const char GLYPH_ddalfinalarabic[]= "ddalfinalarabic";
static const char GLYPH_dddhadeva[]= "dddhadeva";
static const char GLYPH_ddhabengali[]= "ddhabengali";
static const char GLYPH_ddhadeva[]= "ddhadeva";
static const char GLYPH_ddhagujarati[]= "ddhagujarati";
static const char GLYPH_ddhagurmukhi[]= "ddhagurmukhi";
static const char GLYPH_ddotaccent[]= "ddotaccent";
static const char GLYPH_ddotbelow[]= "ddotbelow";
static const char GLYPH_decimalseparatorarabic[]= "decimalseparatorarabic";
static const char GLYPH_decimalseparatorpersian[]= "decimalseparatorpersian";
static const char GLYPH_dehihebrew[]= "dehihebrew";
static const char GLYPH_dehiragana[]= "dehiragana";
static const char GLYPH_deicoptic[]= "deicoptic";
static const char GLYPH_dekatakana[]= "dekatakana";
static const char GLYPH_deleteleft[]= "deleteleft";
static const char GLYPH_deleteright[]= "deleteright";
static const char GLYPH_deltaturned[]= "deltaturned";
static const char GLYPH_denominatorminusonenumeratorbengali[]= "denominatorminusonenumeratorbengali";
static const char GLYPH_dezh[]= "dezh";
static const char GLYPH_dhabengali[]= "dhabengali";
static const char GLYPH_dhadeva[]= "dhadeva";
static const char GLYPH_dhagujarati[]= "dhagujarati";
static const char GLYPH_dhagurmukhi[]= "dhagurmukhi";
static const char GLYPH_dhook[]= "dhook";
static const char GLYPH_dialytikatonoscmb[]= "dialytikatonoscmb";
static const char GLYPH_diamondsuitwhite[]= "diamondsuitwhite";
static const char GLYPH_dieresisbelowcmb[]= "dieresisbelowcmb";
static const char GLYPH_dieresiscmb[]= "dieresiscmb";
static const char GLYPH_dieresisgrave[]= "dieresisgrave";
static const char GLYPH_dihiragana[]= "dihiragana";
static const char GLYPH_dikatakana[]= "dikatakana";
static const char GLYPH_dittomark[]= "dittomark";
static const char GLYPH_divides[]= "divides";
static const char GLYPH_divisionslash[]= "divisionslash";
static const char GLYPH_dlinebelow[]= "dlinebelow";
static const char GLYPH_dlsquare[]= "dlsquare";
static const char GLYPH_dmonospace[]= "dmonospace";
static const char GLYPH_dochadathai[]= "dochadathai";
static const char GLYPH_dodekthai[]= "dodekthai";
static const char GLYPH_dohiragana[]= "dohiragana";
static const char GLYPH_dokatakana[]= "dokatakana";
static const char GLYPH_dollarinferior[]= "dollarinferior";
static const char GLYPH_dollarmonospace[]= "dollarmonospace";
static const char GLYPH_dollaroldstyle[]= "dollaroldstyle";
static const char GLYPH_dollarsmall[]= "dollarsmall";
static const char GLYPH_dollarsuperior[]= "dollarsuperior";
static const char GLYPH_dorusquare[]= "dorusquare";
static const char GLYPH_dotaccentcmb[]= "dotaccentcmb";
static const char GLYPH_dotkatakana[]= "dotkatakana";
static const char GLYPH_dotlessj[]= "dotlessj";
static const char GLYPH_dotlessjstrokehook[]= "dotlessjstrokehook";
static const char GLYPH_dottedcircle[]= "dottedcircle";
static const char GLYPH_doubleyodpatah[]= "doubleyodpatah";
static const char GLYPH_doubleyodpatahhebrew[]= "doubleyodpatahhebrew";
static const char GLYPH_downtackbelowcmb[]= "downtackbelowcmb";
static const char GLYPH_downtackmod[]= "downtackmod";
static const char GLYPH_dparen[]= "dparen";
static const char GLYPH_dsuperior[]= "dsuperior";
static const char GLYPH_dtail[]= "dtail";
static const char GLYPH_dtopbar[]= "dtopbar";
static const char GLYPH_duhiragana[]= "duhiragana";
static const char GLYPH_dukatakana[]= "dukatakana";
static const char GLYPH_dz[]= "dz";
static const char GLYPH_dzaltone[]= "dzaltone";
static const char GLYPH_dzcaron[]= "dzcaron";
static const char GLYPH_dzcurl[]= "dzcurl";
static const char GLYPH_dzeabkhasiancyrillic[]= "dzeabkhasiancyrillic";
static const char GLYPH_earth[]= "earth";
static const char GLYPH_ebengali[]= "ebengali";
static const char GLYPH_ebopomofo[]= "ebopomofo";
static const char GLYPH_ecandradeva[]= "ecandradeva";
static const char GLYPH_ecandragujarati[]= "ecandragujarati";
static const char GLYPH_ecandravowelsigndeva[]= "ecandravowelsigndeva";
static const char GLYPH_ecandravowelsigngujarati[]= "ecandravowelsigngujarati";
static const char GLYPH_ecedillabreve[]= "ecedillabreve";
static const char GLYPH_echarmenian[]= "echarmenian";
static const char GLYPH_echyiwnarmenian[]= "echyiwnarmenian";
static const char GLYPH_ecircle[]= "ecircle";
static const char GLYPH_ecircumflexacute[]= "ecircumflexacute";
static const char GLYPH_ecircumflexbelow[]= "ecircumflexbelow";
static const char GLYPH_ecircumflexdotbelow[]= "ecircumflexdotbelow";
static const char GLYPH_ecircumflexgrave[]= "ecircumflexgrave";
static const char GLYPH_ecircumflexhookabove[]= "ecircumflexhookabove";
static const char GLYPH_ecircumflextilde[]= "ecircumflextilde";
static const char GLYPH_edblgrave[]= "edblgrave";
static const char GLYPH_edeva[]= "edeva";
static const char GLYPH_edotbelow[]= "edotbelow";
static const char GLYPH_eegurmukhi[]= "eegurmukhi";
static const char GLYPH_eematragurmukhi[]= "eematragurmukhi";
static const char GLYPH_egujarati[]= "egujarati";
static const char GLYPH_eharmenian[]= "eharmenian";
static const char GLYPH_ehbopomofo[]= "ehbopomofo";
static const char GLYPH_ehiragana[]= "ehiragana";
static const char GLYPH_ehookabove[]= "ehookabove";
static const char GLYPH_eibopomofo[]= "eibopomofo";
static const char GLYPH_eightbengali[]= "eightbengali";
static const char GLYPH_eightcircle[]= "eightcircle";
static const char GLYPH_eightcircleinversesansserif[]= "eightcircleinversesansserif";
static const char GLYPH_eightdeva[]= "eightdeva";
static const char GLYPH_eighteencircle[]= "eighteencircle";
static const char GLYPH_eighteenparen[]= "eighteenparen";
static const char GLYPH_eighteenperiod[]= "eighteenperiod";
static const char GLYPH_eightgujarati[]= "eightgujarati";
static const char GLYPH_eightgurmukhi[]= "eightgurmukhi";
static const char GLYPH_eighthangzhou[]= "eighthangzhou";
static const char GLYPH_eightideographicparen[]= "eightideographicparen";
static const char GLYPH_eightinferior[]= "eightinferior";
static const char GLYPH_eightmonospace[]= "eightmonospace";
static const char GLYPH_eightoldstyle[]= "eightoldstyle";
static const char GLYPH_eightparen[]= "eightparen";
static const char GLYPH_eightperiod[]= "eightperiod";
static const char GLYPH_eightpersian[]= "eightpersian";
static const char GLYPH_eightroman[]= "eightroman";
static const char GLYPH_eightsuperior[]= "eightsuperior";
static const char GLYPH_eightthai[]= "eightthai";
static const char GLYPH_einvertedbreve[]= "einvertedbreve";
static const char GLYPH_eiotifiedcyrillic[]= "eiotifiedcyrillic";
static const char GLYPH_ekatakana[]= "ekatakana";
static const char GLYPH_ekatakanahalfwidth[]= "ekatakanahalfwidth";
static const char GLYPH_ekonkargurmukhi[]= "ekonkargurmukhi";
static const char GLYPH_ekorean[]= "ekorean";
static const char GLYPH_elevencircle[]= "elevencircle";
static const char GLYPH_elevenparen[]= "elevenparen";
static const char GLYPH_elevenperiod[]= "elevenperiod";
static const char GLYPH_elevenroman[]= "elevenroman";
static const char GLYPH_ellipsisvertical[]= "ellipsisvertical";
static const char GLYPH_emacronacute[]= "emacronacute";
static const char GLYPH_emacrongrave[]= "emacrongrave";
static const char GLYPH_emdashvertical[]= "emdashvertical";
static const char GLYPH_emonospace[]= "emonospace";
static const char GLYPH_emphasismarkarmenian[]= "emphasismarkarmenian";
static const char GLYPH_enbopomofo[]= "enbopomofo";
static const char GLYPH_endashvertical[]= "endashvertical";
static const char GLYPH_endescendercyrillic[]= "endescendercyrillic";
static const char GLYPH_engbopomofo[]= "engbopomofo";
static const char GLYPH_enghecyrillic[]= "enghecyrillic";
static const char GLYPH_enhookcyrillic[]= "enhookcyrillic";
static const char GLYPH_enspace[]= "enspace";
static const char GLYPH_eokorean[]= "eokorean";
static const char GLYPH_eopen[]= "eopen";
static const char GLYPH_eopenclosed[]= "eopenclosed";
static const char GLYPH_eopenreversed[]= "eopenreversed";
static const char GLYPH_eopenreversedclosed[]= "eopenreversedclosed";
static const char GLYPH_eopenreversedhook[]= "eopenreversedhook";
static const char GLYPH_eparen[]= "eparen";
static const char GLYPH_equalmonospace[]= "equalmonospace";
static const char GLYPH_equalsmall[]= "equalsmall";
static const char GLYPH_equalsuperior[]= "equalsuperior";
static const char GLYPH_erbopomofo[]= "erbopomofo";
static const char GLYPH_ereversed[]= "ereversed";
static const char GLYPH_esdescendercyrillic[]= "esdescendercyrillic";
static const char GLYPH_esh[]= "esh";
static const char GLYPH_eshcurl[]= "eshcurl";
static const char GLYPH_eshortdeva[]= "eshortdeva";
static const char GLYPH_eshortvowelsigndeva[]= "eshortvowelsigndeva";
static const char GLYPH_eshreversedloop[]= "eshreversedloop";
static const char GLYPH_eshsquatreversed[]= "eshsquatreversed";
static const char GLYPH_esmallhiragana[]= "esmallhiragana";
static const char GLYPH_esmallkatakana[]= "esmallkatakana";
static const char GLYPH_esmallkatakanahalfwidth[]= "esmallkatakanahalfwidth";
static const char GLYPH_esuperior[]= "esuperior";
static const char GLYPH_etarmenian[]= "etarmenian";
static const char GLYPH_etilde[]= "etilde";
static const char GLYPH_etildebelow[]= "etildebelow";
static const char GLYPH_etnahtafoukhhebrew[]= "etnahtafoukhhebrew";
static const char GLYPH_etnahtafoukhlefthebrew[]= "etnahtafoukhlefthebrew";
static const char GLYPH_etnahtahebrew[]= "etnahtahebrew";
static const char GLYPH_etnahtalefthebrew[]= "etnahtalefthebrew";
static const char GLYPH_eturned[]= "eturned";
static const char GLYPH_eukorean[]= "eukorean";
static const char GLYPH_evowelsignbengali[]= "evowelsignbengali";
static const char GLYPH_evowelsigndeva[]= "evowelsigndeva";
static const char GLYPH_evowelsigngujarati[]= "evowelsigngujarati";
static const char GLYPH_exclamarmenian[]= "exclamarmenian";
static const char GLYPH_exclamdownsmall[]= "exclamdownsmall";
static const char GLYPH_exclammonospace[]= "exclammonospace";
static const char GLYPH_exclamsmall[]= "exclamsmall";
static const char GLYPH_ezh[]= "ezh";
static const char GLYPH_ezhcaron[]= "ezhcaron";
static const char GLYPH_ezhcurl[]= "ezhcurl";
static const char GLYPH_ezhreversed[]= "ezhreversed";
static const char GLYPH_ezhtail[]= "ezhtail";
static const char GLYPH_fadeva[]= "fadeva";
static const char GLYPH_fagurmukhi[]= "fagurmukhi";
static const char GLYPH_fahrenheit[]= "fahrenheit";
static const char GLYPH_fbopomofo[]= "fbopomofo";
static const char GLYPH_fcircle[]= "fcircle";
static const char GLYPH_fdotaccent[]= "fdotaccent";
static const char GLYPH_feharmenian[]= "feharmenian";
static const char GLYPH_fehfinalarabic[]= "fehfinalarabic";
static const char GLYPH_fehinitialarabic[]= "fehinitialarabic";
static const char GLYPH_fehmedialarabic[]= "fehmedialarabic";
static const char GLYPH_feicoptic[]= "feicoptic";
static const char GLYPH_ff[]= "ff";
static const char GLYPH_ffi[]= "ffi";
static const char GLYPH_ffl[]= "ffl";
static const char GLYPH_fi[]= "fi";
static const char GLYPH_fifteencircle[]= "fifteencircle";
static const char GLYPH_fifteenparen[]= "fifteenparen";
static const char GLYPH_fifteenperiod[]= "fifteenperiod";
static const char GLYPH_finalkafdagesh[]= "finalkafdagesh";
static const char GLYPH_finalkafdageshhebrew[]= "finalkafdageshhebrew";
static const char GLYPH_firsttonechinese[]= "firsttonechinese";
static const char GLYPH_fisheye[]= "fisheye";
static const char GLYPH_fivebengali[]= "fivebengali";
static const char GLYPH_fivecircle[]= "fivecircle";
static const char GLYPH_fivecircleinversesansserif[]= "fivecircleinversesansserif";
static const char GLYPH_fivedeva[]= "fivedeva";
static const char GLYPH_fivegujarati[]= "fivegujarati";
static const char GLYPH_fivegurmukhi[]= "fivegurmukhi";
static const char GLYPH_fivehangzhou[]= "fivehangzhou";
static const char GLYPH_fiveideographicparen[]= "fiveideographicparen";
static const char GLYPH_fiveinferior[]= "fiveinferior";
static const char GLYPH_fivemonospace[]= "fivemonospace";
static const char GLYPH_fiveoldstyle[]= "fiveoldstyle";
static const char GLYPH_fiveparen[]= "fiveparen";
static const char GLYPH_fiveperiod[]= "fiveperiod";
static const char GLYPH_fivepersian[]= "fivepersian";
static const char GLYPH_fiveroman[]= "fiveroman";
static const char GLYPH_fivesuperior[]= "fivesuperior";
static const char GLYPH_fivethai[]= "fivethai";
static const char GLYPH_fl[]= "fl";
static const char GLYPH_fmonospace[]= "fmonospace";
static const char GLYPH_fmsquare[]= "fmsquare";
static const char GLYPH_fofanthai[]= "fofanthai";
static const char GLYPH_fofathai[]= "fofathai";
static const char GLYPH_fongmanthai[]= "fongmanthai";
static const char GLYPH_fourbengali[]= "fourbengali";
static const char GLYPH_fourcircle[]= "fourcircle";
static const char GLYPH_fourcircleinversesansserif[]= "fourcircleinversesansserif";
static const char GLYPH_fourdeva[]= "fourdeva";
static const char GLYPH_fourgujarati[]= "fourgujarati";
static const char GLYPH_fourgurmukhi[]= "fourgurmukhi";
static const char GLYPH_fourhangzhou[]= "fourhangzhou";
static const char GLYPH_fourideographicparen[]= "fourideographicparen";
static const char GLYPH_fourinferior[]= "fourinferior";
static const char GLYPH_fourmonospace[]= "fourmonospace";
static const char GLYPH_fournumeratorbengali[]= "fournumeratorbengali";
static const char GLYPH_fouroldstyle[]= "fouroldstyle";
static const char GLYPH_fourparen[]= "fourparen";
static const char GLYPH_fourperiod[]= "fourperiod";
static const char GLYPH_fourpersian[]= "fourpersian";
static const char GLYPH_fourroman[]= "fourroman";
static const char GLYPH_foursuperior[]= "foursuperior";
static const char GLYPH_fourteencircle[]= "fourteencircle";
static const char GLYPH_fourteenparen[]= "fourteenparen";
static const char GLYPH_fourteenperiod[]= "fourteenperiod";
static const char GLYPH_fourthai[]= "fourthai";
static const char GLYPH_fourthtonechinese[]= "fourthtonechinese";
static const char GLYPH_fparen[]= "fparen";
static const char GLYPH_gabengali[]= "gabengali";
static const char GLYPH_gacute[]= "gacute";
static const char GLYPH_gadeva[]= "gadeva";
static const char GLYPH_gaffinalarabic[]= "gaffinalarabic";
static const char GLYPH_gafinitialarabic[]= "gafinitialarabic";
static const char GLYPH_gafmedialarabic[]= "gafmedialarabic";
static const char GLYPH_gagujarati[]= "gagujarati";
static const char GLYPH_gagurmukhi[]= "gagurmukhi";
static const char GLYPH_gahiragana[]= "gahiragana";
static const char GLYPH_gakatakana[]= "gakatakana";
static const char GLYPH_gammalatinsmall[]= "gammalatinsmall";
static const char GLYPH_gammasuperior[]= "gammasuperior";
static const char GLYPH_gangiacoptic[]= "gangiacoptic";
static const char GLYPH_gbopomofo[]= "gbopomofo";
static const char GLYPH_gcircle[]= "gcircle";
static const char GLYPH_gehiragana[]= "gehiragana";
static const char GLYPH_gekatakana[]= "gekatakana";
static const char GLYPH_geometricallyequal[]= "geometricallyequal";
static const char GLYPH_gereshaccenthebrew[]= "gereshaccenthebrew";
static const char GLYPH_gereshhebrew[]= "gereshhebrew";
static const char GLYPH_gereshmuqdamhebrew[]= "gereshmuqdamhebrew";
static const char GLYPH_gershayimaccenthebrew[]= "gershayimaccenthebrew";
static const char GLYPH_gershayimhebrew[]= "gershayimhebrew";
static const char GLYPH_getamark[]= "getamark";
static const char GLYPH_ghabengali[]= "ghabengali";
static const char GLYPH_ghadarmenian[]= "ghadarmenian";
static const char GLYPH_ghadeva[]= "ghadeva";
static const char GLYPH_ghagujarati[]= "ghagujarati";
static const char GLYPH_ghagurmukhi[]= "ghagurmukhi";
static const char GLYPH_ghainfinalarabic[]= "ghainfinalarabic";
static const char GLYPH_ghaininitialarabic[]= "ghaininitialarabic";
static const char GLYPH_ghainmedialarabic[]= "ghainmedialarabic";
static const char GLYPH_ghemiddlehookcyrillic[]= "ghemiddlehookcyrillic";
static const char GLYPH_ghestrokecyrillic[]= "ghestrokecyrillic";
static const char GLYPH_ghhadeva[]= "ghhadeva";
static const char GLYPH_ghhagurmukhi[]= "ghhagurmukhi";
static const char GLYPH_ghook[]= "ghook";
static const char GLYPH_ghzsquare[]= "ghzsquare";
static const char GLYPH_gihiragana[]= "gihiragana";
static const char GLYPH_gikatakana[]= "gikatakana";
static const char GLYPH_gimarmenian[]= "gimarmenian";
static const char GLYPH_gimeldagesh[]= "gimeldagesh";
static const char GLYPH_gimeldageshhebrew[]= "gimeldageshhebrew";
static const char GLYPH_glottalinvertedstroke[]= "glottalinvertedstroke";
static const char GLYPH_glottalstop[]= "glottalstop";
static const char GLYPH_glottalstopinverted[]= "glottalstopinverted";
static const char GLYPH_glottalstopmod[]= "glottalstopmod";
static const char GLYPH_glottalstopreversed[]= "glottalstopreversed";
static const char GLYPH_glottalstopreversedmod[]= "glottalstopreversedmod";
static const char GLYPH_glottalstopreversedsuperior[]= "glottalstopreversedsuperior";
static const char GLYPH_glottalstopstroke[]= "glottalstopstroke";
static const char GLYPH_glottalstopstrokereversed[]= "glottalstopstrokereversed";
static const char GLYPH_gmacron[]= "gmacron";
static const char GLYPH_gmonospace[]= "gmonospace";
static const char GLYPH_gohiragana[]= "gohiragana";
static const char GLYPH_gokatakana[]= "gokatakana";
static const char GLYPH_gparen[]= "gparen";
static const char GLYPH_gpasquare[]= "gpasquare";
static const char GLYPH_gravebelowcmb[]= "gravebelowcmb";
static const char GLYPH_gravedeva[]= "gravedeva";
static const char GLYPH_gravelowmod[]= "gravelowmod";
static const char GLYPH_gravemonospace[]= "gravemonospace";
static const char GLYPH_gravetonecmb[]= "gravetonecmb";
static const char GLYPH_greaterequalorless[]= "greaterequalorless";
static const char GLYPH_greatermonospace[]= "greatermonospace";
static const char GLYPH_greaterorequivalent[]= "greaterorequivalent";
static const char GLYPH_greaterorless[]= "greaterorless";
static const char GLYPH_greateroverequal[]= "greateroverequal";
static const char GLYPH_greatersmall[]= "greatersmall";
static const char GLYPH_gscript[]= "gscript";
static const char GLYPH_gstroke[]= "gstroke";
static const char GLYPH_guhiragana[]= "guhiragana";
static const char GLYPH_gukatakana[]= "gukatakana";
static const char GLYPH_guramusquare[]= "guramusquare";
static const char GLYPH_gysquare[]= "gysquare";
static const char GLYPH_haabkhasiancyrillic[]= "haabkhasiancyrillic";
static const char GLYPH_haaltonearabic[]= "haaltonearabic";
static const char GLYPH_habengali[]= "habengali";
static const char GLYPH_hadescendercyrillic[]= "hadescendercyrillic";
static const char GLYPH_hadeva[]= "hadeva";
static const char GLYPH_hagujarati[]= "hagujarati";
static const char GLYPH_hagurmukhi[]= "hagurmukhi";
static const char GLYPH_hahfinalarabic[]= "hahfinalarabic";
static const char GLYPH_hahinitialarabic[]= "hahinitialarabic";
static const char GLYPH_hahiragana[]= "hahiragana";
static const char GLYPH_hahmedialarabic[]= "hahmedialarabic";
static const char GLYPH_haitusquare[]= "haitusquare";
static const char GLYPH_hakatakana[]= "hakatakana";
static const char GLYPH_hakatakanahalfwidth[]= "hakatakanahalfwidth";
static const char GLYPH_halantgurmukhi[]= "halantgurmukhi";
static const char GLYPH_hangulfiller[]= "hangulfiller";
static const char GLYPH_harpoonleftbarbup[]= "harpoonleftbarbup";
static const char GLYPH_harpoonrightbarbup[]= "harpoonrightbarbup";
static const char GLYPH_hasquare[]= "hasquare";
static const char GLYPH_hbopomofo[]= "hbopomofo";
static const char GLYPH_hbrevebelow[]= "hbrevebelow";
static const char GLYPH_hcedilla[]= "hcedilla";
static const char GLYPH_hcircle[]= "hcircle";
static const char GLYPH_hdieresis[]= "hdieresis";
static const char GLYPH_hdotaccent[]= "hdotaccent";
static const char GLYPH_hdotbelow[]= "hdotbelow";
static const char GLYPH_heartsuitwhite[]= "heartsuitwhite";
static const char GLYPH_hedagesh[]= "hedagesh";
static const char GLYPH_hedageshhebrew[]= "hedageshhebrew";
static const char GLYPH_hehaltonearabic[]= "hehaltonearabic";
static const char GLYPH_hehfinalaltonearabic[]= "hehfinalaltonearabic";
static const char GLYPH_hehfinalalttwoarabic[]= "hehfinalalttwoarabic";
static const char GLYPH_hehfinalarabic[]= "hehfinalarabic";
static const char GLYPH_hehhamzaabovefinalarabic[]= "hehhamzaabovefinalarabic";
static const char GLYPH_hehhamzaaboveisolatedarabic[]= "hehhamzaaboveisolatedarabic";
static const char GLYPH_hehinitialaltonearabic[]= "hehinitialaltonearabic";
static const char GLYPH_hehinitialarabic[]= "hehinitialarabic";
static const char GLYPH_hehiragana[]= "hehiragana";
static const char GLYPH_hehmedialaltonearabic[]= "hehmedialaltonearabic";
static const char GLYPH_hehmedialarabic[]= "hehmedialarabic";
static const char GLYPH_heiseierasquare[]= "heiseierasquare";
static const char GLYPH_hekatakana[]= "hekatakana";
static const char GLYPH_hekatakanahalfwidth[]= "hekatakanahalfwidth";
static const char GLYPH_hekutaarusquare[]= "hekutaarusquare";
static const char GLYPH_henghook[]= "henghook";
static const char GLYPH_herutusquare[]= "herutusquare";
static const char GLYPH_hhook[]= "hhook";
static const char GLYPH_hhooksuperior[]= "hhooksuperior";
static const char GLYPH_hieuhacirclekorean[]= "hieuhacirclekorean";
static const char GLYPH_hieuhaparenkorean[]= "hieuhaparenkorean";
static const char GLYPH_hieuhcirclekorean[]= "hieuhcirclekorean";
static const char GLYPH_hieuhkorean[]= "hieuhkorean";
static const char GLYPH_hieuhparenkorean[]= "hieuhparenkorean";
static const char GLYPH_hihiragana[]= "hihiragana";
static const char GLYPH_hikatakana[]= "hikatakana";
static const char GLYPH_hikatakanahalfwidth[]= "hikatakanahalfwidth";
static const char GLYPH_hlinebelow[]= "hlinebelow";
static const char GLYPH_hmonospace[]= "hmonospace";
static const char GLYPH_hoarmenian[]= "hoarmenian";
static const char GLYPH_hohipthai[]= "hohipthai";
static const char GLYPH_hohiragana[]= "hohiragana";
static const char GLYPH_hokatakana[]= "hokatakana";
static const char GLYPH_hokatakanahalfwidth[]= "hokatakanahalfwidth";
static const char GLYPH_honokhukthai[]= "honokhukthai";
static const char GLYPH_hookpalatalizedbelowcmb[]= "hookpalatalizedbelowcmb";
static const char GLYPH_hookretroflexbelowcmb[]= "hookretroflexbelowcmb";
static const char GLYPH_hoonsquare[]= "hoonsquare";
static const char GLYPH_horicoptic[]= "horicoptic";
static const char GLYPH_horncmb[]= "horncmb";
static const char GLYPH_hotsprings[]= "hotsprings";
static const char GLYPH_hparen[]= "hparen";
static const char GLYPH_hsuperior[]= "hsuperior";
static const char GLYPH_hturned[]= "hturned";
static const char GLYPH_huhiragana[]= "huhiragana";
static const char GLYPH_huiitosquare[]= "huiitosquare";
static const char GLYPH_hukatakana[]= "hukatakana";
static const char GLYPH_hukatakanahalfwidth[]= "hukatakanahalfwidth";
static const char GLYPH_hungarumlautcmb[]= "hungarumlautcmb";
static const char GLYPH_hv[]= "hv";
static const char GLYPH_hypheninferior[]= "hypheninferior";
static const char GLYPH_hyphenmonospace[]= "hyphenmonospace";
static const char GLYPH_hyphensmall[]= "hyphensmall";
static const char GLYPH_hyphensuperior[]= "hyphensuperior";
static const char GLYPH_hyphentwo[]= "hyphentwo";
static const char GLYPH_ibengali[]= "ibengali";
static const char GLYPH_ibopomofo[]= "ibopomofo";
static const char GLYPH_icaron[]= "icaron";
static const char GLYPH_icircle[]= "icircle";
static const char GLYPH_idblgrave[]= "idblgrave";
static const char GLYPH_ideographearthcircle[]= "ideographearthcircle";
static const char GLYPH_ideographfirecircle[]= "ideographfirecircle";
static const char GLYPH_ideographicallianceparen[]= "ideographicallianceparen";
static const char GLYPH_ideographiccallparen[]= "ideographiccallparen";
static const char GLYPH_ideographiccentrecircle[]= "ideographiccentrecircle";
static const char GLYPH_ideographicclose[]= "ideographicclose";
static const char GLYPH_ideographiccomma[]= "ideographiccomma";
static const char GLYPH_ideographiccommaleft[]= "ideographiccommaleft";
static const char GLYPH_ideographiccongratulationparen[]= "ideographiccongratulationparen";
static const char GLYPH_ideographiccorrectcircle[]= "ideographiccorrectcircle";
static const char GLYPH_ideographicearthparen[]= "ideographicearthparen";
static const char GLYPH_ideographicenterpriseparen[]= "ideographicenterpriseparen";
static const char GLYPH_ideographicexcellentcircle[]= "ideographicexcellentcircle";
static const char GLYPH_ideographicfestivalparen[]= "ideographicfestivalparen";
static const char GLYPH_ideographicfinancialcircle[]= "ideographicfinancialcircle";
static const char GLYPH_ideographicfinancialparen[]= "ideographicfinancialparen";
static const char GLYPH_ideographicfireparen[]= "ideographicfireparen";
static const char GLYPH_ideographichaveparen[]= "ideographichaveparen";
static const char GLYPH_ideographichighcircle[]= "ideographichighcircle";
static const char GLYPH_ideographiciterationmark[]= "ideographiciterationmark";
static const char GLYPH_ideographiclaborcircle[]= "ideographiclaborcircle";
static const char GLYPH_ideographiclaborparen[]= "ideographiclaborparen";
static const char GLYPH_ideographicleftcircle[]= "ideographicleftcircle";
static const char GLYPH_ideographiclowcircle[]= "ideographiclowcircle";
static const char GLYPH_ideographicmedicinecircle[]= "ideographicmedicinecircle";
static const char GLYPH_ideographicmetalparen[]= "ideographicmetalparen";
static const char GLYPH_ideographicmoonparen[]= "ideographicmoonparen";
static const char GLYPH_ideographicnameparen[]= "ideographicnameparen";
static const char GLYPH_ideographicperiod[]= "ideographicperiod";
static const char GLYPH_ideographicprintcircle[]= "ideographicprintcircle";
static const char GLYPH_ideographicreachparen[]= "ideographicreachparen";
static const char GLYPH_ideographicrepresentparen[]= "ideographicrepresentparen";
static const char GLYPH_ideographicresourceparen[]= "ideographicresourceparen";
static const char GLYPH_ideographicrightcircle[]= "ideographicrightcircle";
static const char GLYPH_ideographicsecretcircle[]= "ideographicsecretcircle";
static const char GLYPH_ideographicselfparen[]= "ideographicselfparen";
static const char GLYPH_ideographicsocietyparen[]= "ideographicsocietyparen";
static const char GLYPH_ideographicspace[]= "ideographicspace";
static const char GLYPH_ideographicspecialparen[]= "ideographicspecialparen";
static const char GLYPH_ideographicstockparen[]= "ideographicstockparen";
static const char GLYPH_ideographicstudyparen[]= "ideographicstudyparen";
static const char GLYPH_ideographicsunparen[]= "ideographicsunparen";
static const char GLYPH_ideographicsuperviseparen[]= "ideographicsuperviseparen";
static const char GLYPH_ideographicwaterparen[]= "ideographicwaterparen";
static const char GLYPH_ideographicwoodparen[]= "ideographicwoodparen";
static const char GLYPH_ideographiczero[]= "ideographiczero";
static const char GLYPH_ideographmetalcircle[]= "ideographmetalcircle";
static const char GLYPH_ideographmooncircle[]= "ideographmooncircle";
static const char GLYPH_ideographnamecircle[]= "ideographnamecircle";
static const char GLYPH_ideographsuncircle[]= "ideographsuncircle";
static const char GLYPH_ideographwatercircle[]= "ideographwatercircle";
static const char GLYPH_ideographwoodcircle[]= "ideographwoodcircle";
static const char GLYPH_ideva[]= "ideva";
static const char GLYPH_idieresisacute[]= "idieresisacute";
static const char GLYPH_idieresiscyrillic[]= "idieresiscyrillic";
static const char GLYPH_idotbelow[]= "idotbelow";
static const char GLYPH_iebrevecyrillic[]= "iebrevecyrillic";
static const char GLYPH_ieungacirclekorean[]= "ieungacirclekorean";
static const char GLYPH_ieungaparenkorean[]= "ieungaparenkorean";
static const char GLYPH_ieungcirclekorean[]= "ieungcirclekorean";
static const char GLYPH_ieungkorean[]= "ieungkorean";
static const char GLYPH_ieungparenkorean[]= "ieungparenkorean";
static const char GLYPH_igujarati[]= "igujarati";
static const char GLYPH_igurmukhi[]= "igurmukhi";
static const char GLYPH_ihiragana[]= "ihiragana";
static const char GLYPH_ihookabove[]= "ihookabove";
static const char GLYPH_iibengali[]= "iibengali";
static const char GLYPH_iideva[]= "iideva";
static const char GLYPH_iigujarati[]= "iigujarati";
static const char GLYPH_iigurmukhi[]= "iigurmukhi";
static const char GLYPH_iimatragurmukhi[]= "iimatragurmukhi";
static const char GLYPH_iinvertedbreve[]= "iinvertedbreve";
static const char GLYPH_iivowelsignbengali[]= "iivowelsignbengali";
static const char GLYPH_iivowelsigndeva[]= "iivowelsigndeva";
static const char GLYPH_iivowelsigngujarati[]= "iivowelsigngujarati";
static const char GLYPH_ikatakana[]= "ikatakana";
static const char GLYPH_ikatakanahalfwidth[]= "ikatakanahalfwidth";
static const char GLYPH_ikorean[]= "ikorean";
static const char GLYPH_iluyhebrew[]= "iluyhebrew";
static const char GLYPH_imacroncyrillic[]= "imacroncyrillic";
static const char GLYPH_imageorapproximatelyequal[]= "imageorapproximatelyequal";
static const char GLYPH_imatragurmukhi[]= "imatragurmukhi";
static const char GLYPH_imonospace[]= "imonospace";
static const char GLYPH_iniarmenian[]= "iniarmenian";
static const char GLYPH_intisquare[]= "intisquare";
static const char GLYPH_iotalatin[]= "iotalatin";
static const char GLYPH_iparen[]= "iparen";
static const char GLYPH_irigurmukhi[]= "irigurmukhi";
static const char GLYPH_ismallhiragana[]= "ismallhiragana";
static const char GLYPH_ismallkatakana[]= "ismallkatakana";
static const char GLYPH_ismallkatakanahalfwidth[]= "ismallkatakanahalfwidth";
static const char GLYPH_issharbengali[]= "issharbengali";
static const char GLYPH_istroke[]= "istroke";
static const char GLYPH_isuperior[]= "isuperior";
static const char GLYPH_iterationhiragana[]= "iterationhiragana";
static const char GLYPH_iterationkatakana[]= "iterationkatakana";
static const char GLYPH_itildebelow[]= "itildebelow";
static const char GLYPH_iubopomofo[]= "iubopomofo";
static const char GLYPH_ivowelsignbengali[]= "ivowelsignbengali";
static const char GLYPH_ivowelsigndeva[]= "ivowelsigndeva";
static const char GLYPH_ivowelsigngujarati[]= "ivowelsigngujarati";
static const char GLYPH_izhitsadblgravecyrillic[]= "izhitsadblgravecyrillic";
static const char GLYPH_jaarmenian[]= "jaarmenian";
static const char GLYPH_jabengali[]= "jabengali";
static const char GLYPH_jadeva[]= "jadeva";
static const char GLYPH_jagujarati[]= "jagujarati";
static const char GLYPH_jagurmukhi[]= "jagurmukhi";
static const char GLYPH_jbopomofo[]= "jbopomofo";
static const char GLYPH_jcaron[]= "jcaron";
static const char GLYPH_jcircle[]= "jcircle";
static const char GLYPH_jcrossedtail[]= "jcrossedtail";
static const char GLYPH_jdotlessstroke[]= "jdotlessstroke";
static const char GLYPH_jeemfinalarabic[]= "jeemfinalarabic";
static const char GLYPH_jeeminitialarabic[]= "jeeminitialarabic";
static const char GLYPH_jeemmedialarabic[]= "jeemmedialarabic";
static const char GLYPH_jehfinalarabic[]= "jehfinalarabic";
static const char GLYPH_jhabengali[]= "jhabengali";
static const char GLYPH_jhadeva[]= "jhadeva";
static const char GLYPH_jhagujarati[]= "jhagujarati";
static const char GLYPH_jhagurmukhi[]= "jhagurmukhi";
static const char GLYPH_jheharmenian[]= "jheharmenian";
static const char GLYPH_jis[]= "jis";
static const char GLYPH_jmonospace[]= "jmonospace";
static const char GLYPH_jparen[]= "jparen";
static const char GLYPH_jsuperior[]= "jsuperior";
static const char GLYPH_kabashkircyrillic[]= "kabashkircyrillic";
static const char GLYPH_kabengali[]= "kabengali";
static const char GLYPH_kacute[]= "kacute";
static const char GLYPH_kadescendercyrillic[]= "kadescendercyrillic";
static const char GLYPH_kadeva[]= "kadeva";
static const char GLYPH_kafdagesh[]= "kafdagesh";
static const char GLYPH_kafdageshhebrew[]= "kafdageshhebrew";
static const char GLYPH_kaffinalarabic[]= "kaffinalarabic";
static const char GLYPH_kafinitialarabic[]= "kafinitialarabic";
static const char GLYPH_kafmedialarabic[]= "kafmedialarabic";
static const char GLYPH_kafrafehebrew[]= "kafrafehebrew";
static const char GLYPH_kagujarati[]= "kagujarati";
static const char GLYPH_kagurmukhi[]= "kagurmukhi";
static const char GLYPH_kahiragana[]= "kahiragana";
static const char GLYPH_kahookcyrillic[]= "kahookcyrillic";
static const char GLYPH_kakatakana[]= "kakatakana";
static const char GLYPH_kakatakanahalfwidth[]= "kakatakanahalfwidth";
static const char GLYPH_kappasymbolgreek[]= "kappasymbolgreek";
static const char GLYPH_kapyeounmieumkorean[]= "kapyeounmieumkorean";
static const char GLYPH_kapyeounphieuphkorean[]= "kapyeounphieuphkorean";
static const char GLYPH_kapyeounpieupkorean[]= "kapyeounpieupkorean";
static const char GLYPH_kapyeounssangpieupkorean[]= "kapyeounssangpieupkorean";
static const char GLYPH_karoriisquare[]= "karoriisquare";
static const char GLYPH_kasmallkatakana[]= "kasmallkatakana";
static const char GLYPH_kasquare[]= "kasquare";
static const char GLYPH_kastrokecyrillic[]= "kastrokecyrillic";
static const char GLYPH_katahiraprolongmarkhalfwidth[]= "katahiraprolongmarkhalfwidth";
static const char GLYPH_kaverticalstrokecyrillic[]= "kaverticalstrokecyrillic";
static const char GLYPH_kbopomofo[]= "kbopomofo";
static const char GLYPH_kcalsquare[]= "kcalsquare";
static const char GLYPH_kcaron[]= "kcaron";
static const char GLYPH_kcircle[]= "kcircle";
static const char GLYPH_kdotbelow[]= "kdotbelow";
static const char GLYPH_keharmenian[]= "keharmenian";
static const char GLYPH_kehiragana[]= "kehiragana";
static const char GLYPH_kekatakana[]= "kekatakana";
static const char GLYPH_kekatakanahalfwidth[]= "kekatakanahalfwidth";
static const char GLYPH_kenarmenian[]= "kenarmenian";
static const char GLYPH_kesmallkatakana[]= "kesmallkatakana";
static const char GLYPH_khabengali[]= "khabengali";
static const char GLYPH_khadeva[]= "khadeva";
static const char GLYPH_khagujarati[]= "khagujarati";
static const char GLYPH_khagurmukhi[]= "khagurmukhi";
static const char GLYPH_khahfinalarabic[]= "khahfinalarabic";
static const char GLYPH_khahinitialarabic[]= "khahinitialarabic";
static const char GLYPH_khahmedialarabic[]= "khahmedialarabic";
static const char GLYPH_kheicoptic[]= "kheicoptic";
static const char GLYPH_khhadeva[]= "khhadeva";
static const char GLYPH_khhagurmukhi[]= "khhagurmukhi";
static const char GLYPH_khieukhacirclekorean[]= "khieukhacirclekorean";
static const char GLYPH_khieukhaparenkorean[]= "khieukhaparenkorean";
static const char GLYPH_khieukhcirclekorean[]= "khieukhcirclekorean";
static const char GLYPH_khieukhkorean[]= "khieukhkorean";
static const char GLYPH_khieukhparenkorean[]= "khieukhparenkorean";
static const char GLYPH_khokhaithai[]= "khokhaithai";
static const char GLYPH_khokhonthai[]= "khokhonthai";
static const char GLYPH_khokhuatthai[]= "khokhuatthai";
static const char GLYPH_khokhwaithai[]= "khokhwaithai";
static const char GLYPH_khomutthai[]= "khomutthai";
static const char GLYPH_khook[]= "khook";
static const char GLYPH_khorakhangthai[]= "khorakhangthai";
static const char GLYPH_khzsquare[]= "khzsquare";
static const char GLYPH_kihiragana[]= "kihiragana";
static const char GLYPH_kikatakana[]= "kikatakana";
static const char GLYPH_kikatakanahalfwidth[]= "kikatakanahalfwidth";
static const char GLYPH_kiroguramusquare[]= "kiroguramusquare";
static const char GLYPH_kiromeetorusquare[]= "kiromeetorusquare";
static const char GLYPH_kirosquare[]= "kirosquare";
static const char GLYPH_kiyeokacirclekorean[]= "kiyeokacirclekorean";
static const char GLYPH_kiyeokaparenkorean[]= "kiyeokaparenkorean";
static const char GLYPH_kiyeokcirclekorean[]= "kiyeokcirclekorean";
static const char GLYPH_kiyeokkorean[]= "kiyeokkorean";
static const char GLYPH_kiyeokparenkorean[]= "kiyeokparenkorean";
static const char GLYPH_kiyeoksioskorean[]= "kiyeoksioskorean";
static const char GLYPH_klinebelow[]= "klinebelow";
static const char GLYPH_klsquare[]= "klsquare";
static const char GLYPH_kmcubedsquare[]= "kmcubedsquare";
static const char GLYPH_kmonospace[]= "kmonospace";
static const char GLYPH_kmsquaredsquare[]= "kmsquaredsquare";
static const char GLYPH_kohiragana[]= "kohiragana";
static const char GLYPH_kohmsquare[]= "kohmsquare";
static const char GLYPH_kokaithai[]= "kokaithai";
static const char GLYPH_kokatakana[]= "kokatakana";
static const char GLYPH_kokatakanahalfwidth[]= "kokatakanahalfwidth";
static const char GLYPH_kooposquare[]= "kooposquare";
static const char GLYPH_koppacyrillic[]= "koppacyrillic";
static const char GLYPH_koreanstandardsymbol[]= "koreanstandardsymbol";
static const char GLYPH_koroniscmb[]= "koroniscmb";
static const char GLYPH_kparen[]= "kparen";
static const char GLYPH_kpasquare[]= "kpasquare";
static const char GLYPH_ksicyrillic[]= "ksicyrillic";
static const char GLYPH_ktsquare[]= "ktsquare";
static const char GLYPH_kturned[]= "kturned";
static const char GLYPH_kuhiragana[]= "kuhiragana";
static const char GLYPH_kukatakana[]= "kukatakana";
static const char GLYPH_kukatakanahalfwidth[]= "kukatakanahalfwidth";
static const char GLYPH_kvsquare[]= "kvsquare";
static const char GLYPH_kwsquare[]= "kwsquare";
static const char GLYPH_labengali[]= "labengali";
static const char GLYPH_ladeva[]= "ladeva";
static const char GLYPH_lagujarati[]= "lagujarati";
static const char GLYPH_lagurmukhi[]= "lagurmukhi";
static const char GLYPH_lakkhangyaothai[]= "lakkhangyaothai";
static const char GLYPH_lamaleffinalarabic[]= "lamaleffinalarabic";
static const char GLYPH_lamalefhamzaabovefinalarabic[]= "lamalefhamzaabovefinalarabic";
static const char GLYPH_lamalefhamzaaboveisolatedarabic[]= "lamalefhamzaaboveisolatedarabic";
static const char GLYPH_lamalefhamzabelowfinalarabic[]= "lamalefhamzabelowfinalarabic";
static const char GLYPH_lamalefhamzabelowisolatedarabic[]= "lamalefhamzabelowisolatedarabic";
static const char GLYPH_lamalefisolatedarabic[]= "lamalefisolatedarabic";
static const char GLYPH_lamalefmaddaabovefinalarabic[]= "lamalefmaddaabovefinalarabic";
static const char GLYPH_lamalefmaddaaboveisolatedarabic[]= "lamalefmaddaaboveisolatedarabic";
static const char GLYPH_lambdastroke[]= "lambdastroke";
static const char GLYPH_lameddagesh[]= "lameddagesh";
static const char GLYPH_lameddageshhebrew[]= "lameddageshhebrew";
static const char GLYPH_lamfinalarabic[]= "lamfinalarabic";
static const char GLYPH_lamhahinitialarabic[]= "lamhahinitialarabic";
static const char GLYPH_laminitialarabic[]= "laminitialarabic";
static const char GLYPH_lamjeeminitialarabic[]= "lamjeeminitialarabic";
static const char GLYPH_lamkhahinitialarabic[]= "lamkhahinitialarabic";
static const char GLYPH_lamlamhehisolatedarabic[]= "lamlamhehisolatedarabic";
static const char GLYPH_lammedialarabic[]= "lammedialarabic";
static const char GLYPH_lammeemhahinitialarabic[]= "lammeemhahinitialarabic";
static const char GLYPH_lammeeminitialarabic[]= "lammeeminitialarabic";
static const char GLYPH_largecircle[]= "largecircle";
static const char GLYPH_lbar[]= "lbar";
static const char GLYPH_lbelt[]= "lbelt";
static const char GLYPH_lbopomofo[]= "lbopomofo";
static const char GLYPH_lcircle[]= "lcircle";
static const char GLYPH_lcircumflexbelow[]= "lcircumflexbelow";
static const char GLYPH_ldotbelow[]= "ldotbelow";
static const char GLYPH_ldotbelowmacron[]= "ldotbelowmacron";
static const char GLYPH_leftangleabovecmb[]= "leftangleabovecmb";
static const char GLYPH_lefttackbelowcmb[]= "lefttackbelowcmb";
static const char GLYPH_lessequalorgreater[]= "lessequalorgreater";
static const char GLYPH_lessmonospace[]= "lessmonospace";
static const char GLYPH_lessorequivalent[]= "lessorequivalent";
static const char GLYPH_lessorgreater[]= "lessorgreater";
static const char GLYPH_lessoverequal[]= "lessoverequal";
static const char GLYPH_lesssmall[]= "lesssmall";
static const char GLYPH_lezh[]= "lezh";
static const char GLYPH_lhookretroflex[]= "lhookretroflex";
static const char GLYPH_liwnarmenian[]= "liwnarmenian";
static const char GLYPH_lj[]= "lj";
static const char GLYPH_ll[]= "ll";
static const char GLYPH_lladeva[]= "lladeva";
static const char GLYPH_llagujarati[]= "llagujarati";
static const char GLYPH_llinebelow[]= "llinebelow";
static const char GLYPH_llladeva[]= "llladeva";
static const char GLYPH_llvocalicbengali[]= "llvocalicbengali";
static const char GLYPH_llvocalicdeva[]= "llvocalicdeva";
static const char GLYPH_llvocalicvowelsignbengali[]= "llvocalicvowelsignbengali";
static const char GLYPH_llvocalicvowelsigndeva[]= "llvocalicvowelsigndeva";
static const char GLYPH_lmiddletilde[]= "lmiddletilde";
static const char GLYPH_lmonospace[]= "lmonospace";
static const char GLYPH_lmsquare[]= "lmsquare";
static const char GLYPH_lochulathai[]= "lochulathai";
static const char GLYPH_lolingthai[]= "lolingthai";
static const char GLYPH_lowlinecenterline[]= "lowlinecenterline";
static const char GLYPH_lowlinecmb[]= "lowlinecmb";
static const char GLYPH_lowlinedashed[]= "lowlinedashed";
static const char GLYPH_lparen[]= "lparen";
static const char GLYPH_lsuperior[]= "lsuperior";
static const char GLYPH_luthai[]= "luthai";
static const char GLYPH_lvocalicbengali[]= "lvocalicbengali";
static const char GLYPH_lvocalicdeva[]= "lvocalicdeva";
static const char GLYPH_lvocalicvowelsignbengali[]= "lvocalicvowelsignbengali";
static const char GLYPH_lvocalicvowelsigndeva[]= "lvocalicvowelsigndeva";
static const char GLYPH_lxsquare[]= "lxsquare";
static const char GLYPH_mabengali[]= "mabengali";
static const char GLYPH_macronbelowcmb[]= "macronbelowcmb";
static const char GLYPH_macroncmb[]= "macroncmb";
static const char GLYPH_macronlowmod[]= "macronlowmod";
static const char GLYPH_macronmonospace[]= "macronmonospace";
static const char GLYPH_macute[]= "macute";
static const char GLYPH_madeva[]= "madeva";
static const char GLYPH_magujarati[]= "magujarati";
static const char GLYPH_magurmukhi[]= "magurmukhi";
static const char GLYPH_mahapakhhebrew[]= "mahapakhhebrew";
static const char GLYPH_mahapakhlefthebrew[]= "mahapakhlefthebrew";
static const char GLYPH_mahiragana[]= "mahiragana";
static const char GLYPH_maichattawalowleftthai[]= "maichattawalowleftthai";
static const char GLYPH_maichattawalowrightthai[]= "maichattawalowrightthai";
static const char GLYPH_maichattawathai[]= "maichattawathai";
static const char GLYPH_maichattawaupperleftthai[]= "maichattawaupperleftthai";
static const char GLYPH_maieklowleftthai[]= "maieklowleftthai";
static const char GLYPH_maieklowrightthai[]= "maieklowrightthai";
static const char GLYPH_maiekthai[]= "maiekthai";
static const char GLYPH_maiekupperleftthai[]= "maiekupperleftthai";
static const char GLYPH_maihanakatleftthai[]= "maihanakatleftthai";
static const char GLYPH_maihanakatthai[]= "maihanakatthai";
static const char GLYPH_maitaikhuleftthai[]= "maitaikhuleftthai";
static const char GLYPH_maitaikhuthai[]= "maitaikhuthai";
static const char GLYPH_maitholowleftthai[]= "maitholowleftthai";
static const char GLYPH_maitholowrightthai[]= "maitholowrightthai";
static const char GLYPH_maithothai[]= "maithothai";
static const char GLYPH_maithoupperleftthai[]= "maithoupperleftthai";
static const char GLYPH_maitrilowleftthai[]= "maitrilowleftthai";
static const char GLYPH_maitrilowrightthai[]= "maitrilowrightthai";
static const char GLYPH_maitrithai[]= "maitrithai";
static const char GLYPH_maitriupperleftthai[]= "maitriupperleftthai";
static const char GLYPH_maiyamokthai[]= "maiyamokthai";
static const char GLYPH_makatakana[]= "makatakana";
static const char GLYPH_makatakanahalfwidth[]= "makatakanahalfwidth";
static const char GLYPH_mansyonsquare[]= "mansyonsquare";
static const char GLYPH_masoracirclehebrew[]= "masoracirclehebrew";
static const char GLYPH_masquare[]= "masquare";
static const char GLYPH_mbopomofo[]= "mbopomofo";
static const char GLYPH_mbsquare[]= "mbsquare";
static const char GLYPH_mcircle[]= "mcircle";
static const char GLYPH_mcubedsquare[]= "mcubedsquare";
static const char GLYPH_mdotaccent[]= "mdotaccent";
static const char GLYPH_mdotbelow[]= "mdotbelow";
static const char GLYPH_meemfinalarabic[]= "meemfinalarabic";
static const char GLYPH_meeminitialarabic[]= "meeminitialarabic";
static const char GLYPH_meemmedialarabic[]= "meemmedialarabic";
static const char GLYPH_meemmeeminitialarabic[]= "meemmeeminitialarabic";
static const char GLYPH_meemmeemisolatedarabic[]= "meemmeemisolatedarabic";
static const char GLYPH_meetorusquare[]= "meetorusquare";
static const char GLYPH_mehiragana[]= "mehiragana";
static const char GLYPH_meizierasquare[]= "meizierasquare";
static const char GLYPH_mekatakana[]= "mekatakana";
static const char GLYPH_mekatakanahalfwidth[]= "mekatakanahalfwidth";
static const char GLYPH_memdagesh[]= "memdagesh";
static const char GLYPH_memdageshhebrew[]= "memdageshhebrew";
static const char GLYPH_menarmenian[]= "menarmenian";
static const char GLYPH_merkhahebrew[]= "merkhahebrew";
static const char GLYPH_merkhakefulahebrew[]= "merkhakefulahebrew";
static const char GLYPH_merkhakefulalefthebrew[]= "merkhakefulalefthebrew";
static const char GLYPH_merkhalefthebrew[]= "merkhalefthebrew";
static const char GLYPH_mhook[]= "mhook";
static const char GLYPH_mhzsquare[]= "mhzsquare";
static const char GLYPH_middledotkatakanahalfwidth[]= "middledotkatakanahalfwidth";
static const char GLYPH_mieumacirclekorean[]= "mieumacirclekorean";
static const char GLYPH_mieumaparenkorean[]= "mieumaparenkorean";
static const char GLYPH_mieumcirclekorean[]= "mieumcirclekorean";
static const char GLYPH_mieumkorean[]= "mieumkorean";
static const char GLYPH_mieumpansioskorean[]= "mieumpansioskorean";
static const char GLYPH_mieumparenkorean[]= "mieumparenkorean";
static const char GLYPH_mieumpieupkorean[]= "mieumpieupkorean";
static const char GLYPH_mieumsioskorean[]= "mieumsioskorean";
static const char GLYPH_mihiragana[]= "mihiragana";
static const char GLYPH_mikatakana[]= "mikatakana";
static const char GLYPH_mikatakanahalfwidth[]= "mikatakanahalfwidth";
static const char GLYPH_minusbelowcmb[]= "minusbelowcmb";
static const char GLYPH_minuscircle[]= "minuscircle";
static const char GLYPH_minusmod[]= "minusmod";
static const char GLYPH_minusplus[]= "minusplus";
static const char GLYPH_miribaarusquare[]= "miribaarusquare";
static const char GLYPH_mirisquare[]= "mirisquare";
static const char GLYPH_mlonglegturned[]= "mlonglegturned";
static const char GLYPH_mlsquare[]= "mlsquare";
static const char GLYPH_mmcubedsquare[]= "mmcubedsquare";
static const char GLYPH_mmonospace[]= "mmonospace";
static const char GLYPH_mmsquaredsquare[]= "mmsquaredsquare";
static const char GLYPH_mohiragana[]= "mohiragana";
static const char GLYPH_mohmsquare[]= "mohmsquare";
static const char GLYPH_mokatakana[]= "mokatakana";
static const char GLYPH_mokatakanahalfwidth[]= "mokatakanahalfwidth";
static const char GLYPH_molsquare[]= "molsquare";
static const char GLYPH_momathai[]= "momathai";
static const char GLYPH_moverssquare[]= "moverssquare";
static const char GLYPH_moverssquaredsquare[]= "moverssquaredsquare";
static const char GLYPH_mparen[]= "mparen";
static const char GLYPH_mpasquare[]= "mpasquare";
static const char GLYPH_mssquare[]= "mssquare";
static const char GLYPH_msuperior[]= "msuperior";
static const char GLYPH_mturned[]= "mturned";
static const char GLYPH_muasquare[]= "muasquare";
static const char GLYPH_muchgreater[]= "muchgreater";
static const char GLYPH_muchless[]= "muchless";
static const char GLYPH_mufsquare[]= "mufsquare";
static const char GLYPH_mugreek[]= "mugreek";
static const char GLYPH_mugsquare[]= "mugsquare";
static const char GLYPH_muhiragana[]= "muhiragana";
static const char GLYPH_mukatakana[]= "mukatakana";
static const char GLYPH_mukatakanahalfwidth[]= "mukatakanahalfwidth";
static const char GLYPH_mulsquare[]= "mulsquare";
static const char GLYPH_mumsquare[]= "mumsquare";
static const char GLYPH_munahhebrew[]= "munahhebrew";
static const char GLYPH_munahlefthebrew[]= "munahlefthebrew";
static const char GLYPH_musicflatsign[]= "musicflatsign";
static const char GLYPH_musicsharpsign[]= "musicsharpsign";
static const char GLYPH_mussquare[]= "mussquare";
static const char GLYPH_muvsquare[]= "muvsquare";
static const char GLYPH_muwsquare[]= "muwsquare";
static const char GLYPH_mvmegasquare[]= "mvmegasquare";
static const char GLYPH_mvsquare[]= "mvsquare";
static const char GLYPH_mwmegasquare[]= "mwmegasquare";
static const char GLYPH_mwsquare[]= "mwsquare";
static const char GLYPH_nabengali[]= "nabengali";
static const char GLYPH_nadeva[]= "nadeva";
static const char GLYPH_nagujarati[]= "nagujarati";
static const char GLYPH_nagurmukhi[]= "nagurmukhi";
static const char GLYPH_nahiragana[]= "nahiragana";
static const char GLYPH_nakatakana[]= "nakatakana";
static const char GLYPH_nakatakanahalfwidth[]= "nakatakanahalfwidth";
static const char GLYPH_nasquare[]= "nasquare";
static const char GLYPH_nbopomofo[]= "nbopomofo";
static const char GLYPH_nbspace[]= "nbspace";
static const char GLYPH_ncircle[]= "ncircle";
static const char GLYPH_ncircumflexbelow[]= "ncircumflexbelow";
static const char GLYPH_ndotaccent[]= "ndotaccent";
static const char GLYPH_ndotbelow[]= "ndotbelow";
static const char GLYPH_nehiragana[]= "nehiragana";
static const char GLYPH_nekatakana[]= "nekatakana";
static const char GLYPH_nekatakanahalfwidth[]= "nekatakanahalfwidth";
static const char GLYPH_nfsquare[]= "nfsquare";
static const char GLYPH_ngabengali[]= "ngabengali";
static const char GLYPH_ngadeva[]= "ngadeva";
static const char GLYPH_ngagujarati[]= "ngagujarati";
static const char GLYPH_ngagurmukhi[]= "ngagurmukhi";
static const char GLYPH_ngonguthai[]= "ngonguthai";
static const char GLYPH_nhiragana[]= "nhiragana";
static const char GLYPH_nhookleft[]= "nhookleft";
static const char GLYPH_nhookretroflex[]= "nhookretroflex";
static const char GLYPH_nieunacirclekorean[]= "nieunacirclekorean";
static const char GLYPH_nieunaparenkorean[]= "nieunaparenkorean";
static const char GLYPH_nieuncieuckorean[]= "nieuncieuckorean";
static const char GLYPH_nieuncirclekorean[]= "nieuncirclekorean";
static const char GLYPH_nieunhieuhkorean[]= "nieunhieuhkorean";
static const char GLYPH_nieunkorean[]= "nieunkorean";
static const char GLYPH_nieunpansioskorean[]= "nieunpansioskorean";
static const char GLYPH_nieunparenkorean[]= "nieunparenkorean";
static const char GLYPH_nieunsioskorean[]= "nieunsioskorean";
static const char GLYPH_nieuntikeutkorean[]= "nieuntikeutkorean";
static const char GLYPH_nihiragana[]= "nihiragana";
static const char GLYPH_nikatakana[]= "nikatakana";
static const char GLYPH_nikatakanahalfwidth[]= "nikatakanahalfwidth";
static const char GLYPH_nikhahitleftthai[]= "nikhahitleftthai";
static const char GLYPH_nikhahitthai[]= "nikhahitthai";
static const char GLYPH_ninebengali[]= "ninebengali";
static const char GLYPH_ninecircle[]= "ninecircle";
static const char GLYPH_ninecircleinversesansserif[]= "ninecircleinversesansserif";
static const char GLYPH_ninedeva[]= "ninedeva";
static const char GLYPH_ninegujarati[]= "ninegujarati";
static const char GLYPH_ninegurmukhi[]= "ninegurmukhi";
static const char GLYPH_ninehangzhou[]= "ninehangzhou";
static const char GLYPH_nineideographicparen[]= "nineideographicparen";
static const char GLYPH_nineinferior[]= "nineinferior";
static const char GLYPH_ninemonospace[]= "ninemonospace";
static const char GLYPH_nineoldstyle[]= "nineoldstyle";
static const char GLYPH_nineparen[]= "nineparen";
static const char GLYPH_nineperiod[]= "nineperiod";
static const char GLYPH_ninepersian[]= "ninepersian";
static const char GLYPH_nineroman[]= "nineroman";
static const char GLYPH_ninesuperior[]= "ninesuperior";
static const char GLYPH_nineteencircle[]= "nineteencircle";
static const char GLYPH_nineteenparen[]= "nineteenparen";
static const char GLYPH_nineteenperiod[]= "nineteenperiod";
static const char GLYPH_ninethai[]= "ninethai";
static const char GLYPH_nj[]= "nj";
static const char GLYPH_nkatakana[]= "nkatakana";
static const char GLYPH_nkatakanahalfwidth[]= "nkatakanahalfwidth";
static const char GLYPH_nlegrightlong[]= "nlegrightlong";
static const char GLYPH_nlinebelow[]= "nlinebelow";
static const char GLYPH_nmonospace[]= "nmonospace";
static const char GLYPH_nmsquare[]= "nmsquare";
static const char GLYPH_nnabengali[]= "nnabengali";
static const char GLYPH_nnadeva[]= "nnadeva";
static const char GLYPH_nnagujarati[]= "nnagujarati";
static const char GLYPH_nnagurmukhi[]= "nnagurmukhi";
static const char GLYPH_nnnadeva[]= "nnnadeva";
static const char GLYPH_nohiragana[]= "nohiragana";
static const char GLYPH_nokatakana[]= "nokatakana";
static const char GLYPH_nokatakanahalfwidth[]= "nokatakanahalfwidth";
static const char GLYPH_nonenthai[]= "nonenthai";
static const char GLYPH_nonuthai[]= "nonuthai";
static const char GLYPH_noonfinalarabic[]= "noonfinalarabic";
static const char GLYPH_noonghunnafinalarabic[]= "noonghunnafinalarabic";
static const char GLYPH_nooninitialarabic[]= "nooninitialarabic";
static const char GLYPH_noonjeeminitialarabic[]= "noonjeeminitialarabic";
static const char GLYPH_noonjeemisolatedarabic[]= "noonjeemisolatedarabic";
static const char GLYPH_noonmedialarabic[]= "noonmedialarabic";
static const char GLYPH_noonmeeminitialarabic[]= "noonmeeminitialarabic";
static const char GLYPH_noonmeemisolatedarabic[]= "noonmeemisolatedarabic";
static const char GLYPH_noonnoonfinalarabic[]= "noonnoonfinalarabic";
static const char GLYPH_notcontains[]= "notcontains";
static const char GLYPH_notgreater[]= "notgreater";
static const char GLYPH_notgreaternorequal[]= "notgreaternorequal";
static const char GLYPH_notgreaternorless[]= "notgreaternorless";
static const char GLYPH_notidentical[]= "notidentical";
static const char GLYPH_notless[]= "notless";
static const char GLYPH_notlessnorequal[]= "notlessnorequal";
static const char GLYPH_notparallel[]= "notparallel";
static const char GLYPH_notprecedes[]= "notprecedes";
static const char GLYPH_notsucceeds[]= "notsucceeds";
static const char GLYPH_notsuperset[]= "notsuperset";
static const char GLYPH_nowarmenian[]= "nowarmenian";
static const char GLYPH_nparen[]= "nparen";
static const char GLYPH_nssquare[]= "nssquare";
static const char GLYPH_nsuperior[]= "nsuperior";
static const char GLYPH_nuhiragana[]= "nuhiragana";
static const char GLYPH_nukatakana[]= "nukatakana";
static const char GLYPH_nukatakanahalfwidth[]= "nukatakanahalfwidth";
static const char GLYPH_nuktabengali[]= "nuktabengali";
static const char GLYPH_nuktadeva[]= "nuktadeva";
static const char GLYPH_nuktagujarati[]= "nuktagujarati";
static const char GLYPH_nuktagurmukhi[]= "nuktagurmukhi";
static const char GLYPH_numbersignmonospace[]= "numbersignmonospace";
static const char GLYPH_numbersignsmall[]= "numbersignsmall";
static const char GLYPH_numeralsigngreek[]= "numeralsigngreek";
static const char GLYPH_numeralsignlowergreek[]= "numeralsignlowergreek";
static const char GLYPH_nundagesh[]= "nundagesh";
static const char GLYPH_nundageshhebrew[]= "nundageshhebrew";
static const char GLYPH_nvsquare[]= "nvsquare";
static const char GLYPH_nwsquare[]= "nwsquare";
static const char GLYPH_nyabengali[]= "nyabengali";
static const char GLYPH_nyadeva[]= "nyadeva";
static const char GLYPH_nyagujarati[]= "nyagujarati";
static const char GLYPH_nyagurmukhi[]= "nyagurmukhi";
static const char GLYPH_oangthai[]= "oangthai";
static const char GLYPH_obarred[]= "obarred";
static const char GLYPH_obarredcyrillic[]= "obarredcyrillic";
static const char GLYPH_obarreddieresiscyrillic[]= "obarreddieresiscyrillic";
static const char GLYPH_obengali[]= "obengali";
static const char GLYPH_obopomofo[]= "obopomofo";
static const char GLYPH_ocandradeva[]= "ocandradeva";
static const char GLYPH_ocandragujarati[]= "ocandragujarati";
static const char GLYPH_ocandravowelsigndeva[]= "ocandravowelsigndeva";
static const char GLYPH_ocandravowelsigngujarati[]= "ocandravowelsigngujarati";
static const char GLYPH_ocaron[]= "ocaron";
static const char GLYPH_ocircle[]= "ocircle";
static const char GLYPH_ocircumflexacute[]= "ocircumflexacute";
static const char GLYPH_ocircumflexdotbelow[]= "ocircumflexdotbelow";
static const char GLYPH_ocircumflexgrave[]= "ocircumflexgrave";
static const char GLYPH_ocircumflexhookabove[]= "ocircumflexhookabove";
static const char GLYPH_ocircumflextilde[]= "ocircumflextilde";
static const char GLYPH_odblgrave[]= "odblgrave";
static const char GLYPH_odeva[]= "odeva";
static const char GLYPH_odieresiscyrillic[]= "odieresiscyrillic";
static const char GLYPH_odotbelow[]= "odotbelow";
static const char GLYPH_oekorean[]= "oekorean";
static const char GLYPH_ogonekcmb[]= "ogonekcmb";
static const char GLYPH_ogujarati[]= "ogujarati";
static const char GLYPH_oharmenian[]= "oharmenian";
static const char GLYPH_ohiragana[]= "ohiragana";
static const char GLYPH_ohookabove[]= "ohookabove";
static const char GLYPH_ohornacute[]= "ohornacute";
static const char GLYPH_ohorndotbelow[]= "ohorndotbelow";
static const char GLYPH_ohorngrave[]= "ohorngrave";
static const char GLYPH_ohornhookabove[]= "ohornhookabove";
static const char GLYPH_ohorntilde[]= "ohorntilde";
static const char GLYPH_oi[]= "oi";
static const char GLYPH_oinvertedbreve[]= "oinvertedbreve";
static const char GLYPH_okatakana[]= "okatakana";
static const char GLYPH_okatakanahalfwidth[]= "okatakanahalfwidth";
static const char GLYPH_okorean[]= "okorean";
static const char GLYPH_olehebrew[]= "olehebrew";
static const char GLYPH_omacronacute[]= "omacronacute";
static const char GLYPH_omacrongrave[]= "omacrongrave";
static const char GLYPH_omdeva[]= "omdeva";
static const char GLYPH_omegacyrillic[]= "omegacyrillic";
static const char GLYPH_omegalatinclosed[]= "omegalatinclosed";
static const char GLYPH_omegaroundcyrillic[]= "omegaroundcyrillic";
static const char GLYPH_omegatitlocyrillic[]= "omegatitlocyrillic";
static const char GLYPH_omgujarati[]= "omgujarati";
static const char GLYPH_omonospace[]= "omonospace";
static const char GLYPH_onebengali[]= "onebengali";
static const char GLYPH_onecircle[]= "onecircle";
static const char GLYPH_onecircleinversesansserif[]= "onecircleinversesansserif";
static const char GLYPH_onedeva[]= "onedeva";
static const char GLYPH_onefitted[]= "onefitted";
static const char GLYPH_onegujarati[]= "onegujarati";
static const char GLYPH_onegurmukhi[]= "onegurmukhi";
static const char GLYPH_onehangzhou[]= "onehangzhou";
static const char GLYPH_oneideographicparen[]= "oneideographicparen";
static const char GLYPH_oneinferior[]= "oneinferior";
static const char GLYPH_onemonospace[]= "onemonospace";
static const char GLYPH_onenumeratorbengali[]= "onenumeratorbengali";
static const char GLYPH_oneoldstyle[]= "oneoldstyle";
static const char GLYPH_oneparen[]= "oneparen";
static const char GLYPH_oneperiod[]= "oneperiod";
static const char GLYPH_onepersian[]= "onepersian";
static const char GLYPH_oneroman[]= "oneroman";
static const char GLYPH_onethai[]= "onethai";
static const char GLYPH_oogonek[]= "oogonek";
static const char GLYPH_oogonekmacron[]= "oogonekmacron";
static const char GLYPH_oogurmukhi[]= "oogurmukhi";
static const char GLYPH_oomatragurmukhi[]= "oomatragurmukhi";
static const char GLYPH_oopen[]= "oopen";
static const char GLYPH_oparen[]= "oparen";
static const char GLYPH_option[]= "option";
static const char GLYPH_oshortdeva[]= "oshortdeva";
static const char GLYPH_oshortvowelsigndeva[]= "oshortvowelsigndeva";
static const char GLYPH_osmallhiragana[]= "osmallhiragana";
static const char GLYPH_osmallkatakana[]= "osmallkatakana";
static const char GLYPH_osmallkatakanahalfwidth[]= "osmallkatakanahalfwidth";
static const char GLYPH_osuperior[]= "osuperior";
static const char GLYPH_otcyrillic[]= "otcyrillic";
static const char GLYPH_otildeacute[]= "otildeacute";
static const char GLYPH_otildedieresis[]= "otildedieresis";
static const char GLYPH_oubopomofo[]= "oubopomofo";
static const char GLYPH_overline[]= "overline";
static const char GLYPH_overlinecenterline[]= "overlinecenterline";
static const char GLYPH_overlinecmb[]= "overlinecmb";
static const char GLYPH_overlinedashed[]= "overlinedashed";
static const char GLYPH_overlinedblwavy[]= "overlinedblwavy";
static const char GLYPH_overlinewavy[]= "overlinewavy";
static const char GLYPH_ovowelsignbengali[]= "ovowelsignbengali";
static const char GLYPH_ovowelsigndeva[]= "ovowelsigndeva";
static const char GLYPH_ovowelsigngujarati[]= "ovowelsigngujarati";
static const char GLYPH_paampssquare[]= "paampssquare";
static const char GLYPH_paasentosquare[]= "paasentosquare";
static const char GLYPH_pabengali[]= "pabengali";
static const char GLYPH_pacute[]= "pacute";
static const char GLYPH_padeva[]= "padeva";
static const char GLYPH_pagedown[]= "pagedown";
static const char GLYPH_pageup[]= "pageup";
static const char GLYPH_pagujarati[]= "pagujarati";
static const char GLYPH_pagurmukhi[]= "pagurmukhi";
static const char GLYPH_pahiragana[]= "pahiragana";
static const char GLYPH_paiyannoithai[]= "paiyannoithai";
static const char GLYPH_pakatakana[]= "pakatakana";
static const char GLYPH_palatalizationcyrilliccmb[]= "palatalizationcyrilliccmb";
static const char GLYPH_palochkacyrillic[]= "palochkacyrillic";
static const char GLYPH_pansioskorean[]= "pansioskorean";
static const char GLYPH_parallel[]= "parallel";
static const char GLYPH_parenleftaltonearabic[]= "parenleftaltonearabic";
static const char GLYPH_parenleftinferior[]= "parenleftinferior";
static const char GLYPH_parenleftmonospace[]= "parenleftmonospace";
static const char GLYPH_parenleftsmall[]= "parenleftsmall";
static const char GLYPH_parenleftsuperior[]= "parenleftsuperior";
static const char GLYPH_parenleftvertical[]= "parenleftvertical";
static const char GLYPH_parenrightaltonearabic[]= "parenrightaltonearabic";
static const char GLYPH_parenrightinferior[]= "parenrightinferior";
static const char GLYPH_parenrightmonospace[]= "parenrightmonospace";
static const char GLYPH_parenrightsmall[]= "parenrightsmall";
static const char GLYPH_parenrightsuperior[]= "parenrightsuperior";
static const char GLYPH_parenrightvertical[]= "parenrightvertical";
static const char GLYPH_pashtahebrew[]= "pashtahebrew";
static const char GLYPH_pasquare[]= "pasquare";
static const char GLYPH_pazerhebrew[]= "pazerhebrew";
static const char GLYPH_pbopomofo[]= "pbopomofo";
static const char GLYPH_pcircle[]= "pcircle";
static const char GLYPH_pdotaccent[]= "pdotaccent";
static const char GLYPH_pedagesh[]= "pedagesh";
static const char GLYPH_pedageshhebrew[]= "pedageshhebrew";
static const char GLYPH_peezisquare[]= "peezisquare";
static const char GLYPH_pefinaldageshhebrew[]= "pefinaldageshhebrew";
static const char GLYPH_peharmenian[]= "peharmenian";
static const char GLYPH_pehfinalarabic[]= "pehfinalarabic";
static const char GLYPH_pehinitialarabic[]= "pehinitialarabic";
static const char GLYPH_pehiragana[]= "pehiragana";
static const char GLYPH_pehmedialarabic[]= "pehmedialarabic";
static const char GLYPH_pekatakana[]= "pekatakana";
static const char GLYPH_pemiddlehookcyrillic[]= "pemiddlehookcyrillic";
static const char GLYPH_perafehebrew[]= "perafehebrew";
static const char GLYPH_percentmonospace[]= "percentmonospace";
static const char GLYPH_percentsmall[]= "percentsmall";
static const char GLYPH_periodarmenian[]= "periodarmenian";
static const char GLYPH_periodhalfwidth[]= "periodhalfwidth";
static const char GLYPH_periodinferior[]= "periodinferior";
static const char GLYPH_periodmonospace[]= "periodmonospace";
static const char GLYPH_periodsmall[]= "periodsmall";
static const char GLYPH_periodsuperior[]= "periodsuperior";
static const char GLYPH_perispomenigreekcmb[]= "perispomenigreekcmb";
static const char GLYPH_pfsquare[]= "pfsquare";
static const char GLYPH_phabengali[]= "phabengali";
static const char GLYPH_phadeva[]= "phadeva";
static const char GLYPH_phagujarati[]= "phagujarati";
static const char GLYPH_phagurmukhi[]= "phagurmukhi";
static const char GLYPH_phieuphacirclekorean[]= "phieuphacirclekorean";
static const char GLYPH_phieuphaparenkorean[]= "phieuphaparenkorean";
static const char GLYPH_phieuphcirclekorean[]= "phieuphcirclekorean";
static const char GLYPH_phieuphkorean[]= "phieuphkorean";
static const char GLYPH_phieuphparenkorean[]= "phieuphparenkorean";
static const char GLYPH_philatin[]= "philatin";
static const char GLYPH_phinthuthai[]= "phinthuthai";
static const char GLYPH_phook[]= "phook";
static const char GLYPH_phophanthai[]= "phophanthai";
static const char GLYPH_phophungthai[]= "phophungthai";
static const char GLYPH_phosamphaothai[]= "phosamphaothai";
static const char GLYPH_pieupacirclekorean[]= "pieupacirclekorean";
static const char GLYPH_pieupaparenkorean[]= "pieupaparenkorean";
static const char GLYPH_pieupcieuckorean[]= "pieupcieuckorean";
static const char GLYPH_pieupcirclekorean[]= "pieupcirclekorean";
static const char GLYPH_pieupkiyeokkorean[]= "pieupkiyeokkorean";
static const char GLYPH_pieupkorean[]= "pieupkorean";
static const char GLYPH_pieupparenkorean[]= "pieupparenkorean";
static const char GLYPH_pieupsioskiyeokkorean[]= "pieupsioskiyeokkorean";
static const char GLYPH_pieupsioskorean[]= "pieupsioskorean";
static const char GLYPH_pieupsiostikeutkorean[]= "pieupsiostikeutkorean";
static const char GLYPH_pieupthieuthkorean[]= "pieupthieuthkorean";
static const char GLYPH_pieuptikeutkorean[]= "pieuptikeutkorean";
static const char GLYPH_pihiragana[]= "pihiragana";
static const char GLYPH_pikatakana[]= "pikatakana";
static const char GLYPH_piwrarmenian[]= "piwrarmenian";
static const char GLYPH_plusbelowcmb[]= "plusbelowcmb";
static const char GLYPH_plusmod[]= "plusmod";
static const char GLYPH_plusmonospace[]= "plusmonospace";
static const char GLYPH_plussmall[]= "plussmall";
static const char GLYPH_plussuperior[]= "plussuperior";
static const char GLYPH_pmonospace[]= "pmonospace";
static const char GLYPH_pmsquare[]= "pmsquare";
static const char GLYPH_pohiragana[]= "pohiragana";
static const char GLYPH_pointingindexdownwhite[]= "pointingindexdownwhite";
static const char GLYPH_pointingindexleftwhite[]= "pointingindexleftwhite";
static const char GLYPH_pointingindexrightwhite[]= "pointingindexrightwhite";
static const char GLYPH_pointingindexupwhite[]= "pointingindexupwhite";
static const char GLYPH_pokatakana[]= "pokatakana";
static const char GLYPH_poplathai[]= "poplathai";
static const char GLYPH_postalmark[]= "postalmark";
static const char GLYPH_postalmarkface[]= "postalmarkface";
static const char GLYPH_pparen[]= "pparen";
static const char GLYPH_precedes[]= "precedes";
static const char GLYPH_primemod[]= "primemod";
static const char GLYPH_primereversed[]= "primereversed";
static const char GLYPH_projective[]= "projective";
static const char GLYPH_prolongedkana[]= "prolongedkana";
static const char GLYPH_propellor[]= "propellor";
static const char GLYPH_proportion[]= "proportion";
static const char GLYPH_psicyrillic[]= "psicyrillic";
static const char GLYPH_psilipneumatacyrilliccmb[]= "psilipneumatacyrilliccmb";
static const char GLYPH_pssquare[]= "pssquare";
static const char GLYPH_puhiragana[]= "puhiragana";
static const char GLYPH_pukatakana[]= "pukatakana";
static const char GLYPH_pvsquare[]= "pvsquare";
static const char GLYPH_pwsquare[]= "pwsquare";
static const char GLYPH_qadeva[]= "qadeva";
static const char GLYPH_qadmahebrew[]= "qadmahebrew";
static const char GLYPH_qaffinalarabic[]= "qaffinalarabic";
static const char GLYPH_qafinitialarabic[]= "qafinitialarabic";
static const char GLYPH_qafmedialarabic[]= "qafmedialarabic";
static const char GLYPH_qarneyparahebrew[]= "qarneyparahebrew";
static const char GLYPH_qbopomofo[]= "qbopomofo";
static const char GLYPH_qcircle[]= "qcircle";
static const char GLYPH_qhook[]= "qhook";
static const char GLYPH_qmonospace[]= "qmonospace";
static const char GLYPH_qofdagesh[]= "qofdagesh";
static const char GLYPH_qofdageshhebrew[]= "qofdageshhebrew";
static const char GLYPH_qparen[]= "qparen";
static const char GLYPH_quarternote[]= "quarternote";
static const char GLYPH_questionarmenian[]= "questionarmenian";
static const char GLYPH_questiondownsmall[]= "questiondownsmall";
static const char GLYPH_questiongreek[]= "questiongreek";
static const char GLYPH_questionmonospace[]= "questionmonospace";
static const char GLYPH_questionsmall[]= "questionsmall";
static const char GLYPH_quotedblmonospace[]= "quotedblmonospace";
static const char GLYPH_quotedblprime[]= "quotedblprime";
static const char GLYPH_quotedblprimereversed[]= "quotedblprimereversed";
static const char GLYPH_quotesinglemonospace[]= "quotesinglemonospace";
static const char GLYPH_raarmenian[]= "raarmenian";
static const char GLYPH_rabengali[]= "rabengali";
static const char GLYPH_radeva[]= "radeva";
static const char GLYPH_radoverssquare[]= "radoverssquare";
static const char GLYPH_radoverssquaredsquare[]= "radoverssquaredsquare";
static const char GLYPH_radsquare[]= "radsquare";
static const char GLYPH_ragujarati[]= "ragujarati";
static const char GLYPH_ragurmukhi[]= "ragurmukhi";
static const char GLYPH_rahiragana[]= "rahiragana";
static const char GLYPH_rakatakana[]= "rakatakana";
static const char GLYPH_rakatakanahalfwidth[]= "rakatakanahalfwidth";
static const char GLYPH_ralowerdiagonalbengali[]= "ralowerdiagonalbengali";
static const char GLYPH_ramiddlediagonalbengali[]= "ramiddlediagonalbengali";
static const char GLYPH_ramshorn[]= "ramshorn";
static const char GLYPH_ratio[]= "ratio";
static const char GLYPH_rbopomofo[]= "rbopomofo";
static const char GLYPH_rcircle[]= "rcircle";
static const char GLYPH_rdblgrave[]= "rdblgrave";
static const char GLYPH_rdotaccent[]= "rdotaccent";
static const char GLYPH_rdotbelow[]= "rdotbelow";
static const char GLYPH_rdotbelowmacron[]= "rdotbelowmacron";
static const char GLYPH_referencemark[]= "referencemark";
static const char GLYPH_reharmenian[]= "reharmenian";
static const char GLYPH_rehfinalarabic[]= "rehfinalarabic";
static const char GLYPH_rehiragana[]= "rehiragana";
static const char GLYPH_rekatakana[]= "rekatakana";
static const char GLYPH_rekatakanahalfwidth[]= "rekatakanahalfwidth";
static const char GLYPH_reshdageshhebrew[]= "reshdageshhebrew";
static const char GLYPH_reversedtilde[]= "reversedtilde";
static const char GLYPH_reviahebrew[]= "reviahebrew";
static const char GLYPH_reviamugrashhebrew[]= "reviamugrashhebrew";
static const char GLYPH_rfishhook[]= "rfishhook";
static const char GLYPH_rfishhookreversed[]= "rfishhookreversed";
static const char GLYPH_rhabengali[]= "rhabengali";
static const char GLYPH_rhadeva[]= "rhadeva";
static const char GLYPH_rhook[]= "rhook";
static const char GLYPH_rhookturned[]= "rhookturned";
static const char GLYPH_rhookturnedsuperior[]= "rhookturnedsuperior";
static const char GLYPH_rhosymbolgreek[]= "rhosymbolgreek";
static const char GLYPH_rhotichookmod[]= "rhotichookmod";
static const char GLYPH_rieulacirclekorean[]= "rieulacirclekorean";
static const char GLYPH_rieulaparenkorean[]= "rieulaparenkorean";
static const char GLYPH_rieulcirclekorean[]= "rieulcirclekorean";
static const char GLYPH_rieulhieuhkorean[]= "rieulhieuhkorean";
static const char GLYPH_rieulkiyeokkorean[]= "rieulkiyeokkorean";
static const char GLYPH_rieulkiyeoksioskorean[]= "rieulkiyeoksioskorean";
static const char GLYPH_rieulkorean[]= "rieulkorean";
static const char GLYPH_rieulmieumkorean[]= "rieulmieumkorean";
static const char GLYPH_rieulpansioskorean[]= "rieulpansioskorean";
static const char GLYPH_rieulparenkorean[]= "rieulparenkorean";
static const char GLYPH_rieulphieuphkorean[]= "rieulphieuphkorean";
static const char GLYPH_rieulpieupkorean[]= "rieulpieupkorean";
static const char GLYPH_rieulpieupsioskorean[]= "rieulpieupsioskorean";
static const char GLYPH_rieulsioskorean[]= "rieulsioskorean";
static const char GLYPH_rieulthieuthkorean[]= "rieulthieuthkorean";
static const char GLYPH_rieultikeutkorean[]= "rieultikeutkorean";
static const char GLYPH_rieulyeorinhieuhkorean[]= "rieulyeorinhieuhkorean";
static const char GLYPH_righttackbelowcmb[]= "righttackbelowcmb";
static const char GLYPH_righttriangle[]= "righttriangle";
static const char GLYPH_rihiragana[]= "rihiragana";
static const char GLYPH_rikatakana[]= "rikatakana";
static const char GLYPH_rikatakanahalfwidth[]= "rikatakanahalfwidth";
static const char GLYPH_ringbelowcmb[]= "ringbelowcmb";
static const char GLYPH_ringcmb[]= "ringcmb";
static const char GLYPH_ringhalfleft[]= "ringhalfleft";
static const char GLYPH_ringhalfleftarmenian[]= "ringhalfleftarmenian";
static const char GLYPH_ringhalfleftbelowcmb[]= "ringhalfleftbelowcmb";
static const char GLYPH_ringhalfleftcentered[]= "ringhalfleftcentered";
static const char GLYPH_ringhalfright[]= "ringhalfright";
static const char GLYPH_ringhalfrightbelowcmb[]= "ringhalfrightbelowcmb";
static const char GLYPH_ringhalfrightcentered[]= "ringhalfrightcentered";
static const char GLYPH_rinvertedbreve[]= "rinvertedbreve";
static const char GLYPH_rittorusquare[]= "rittorusquare";
static const char GLYPH_rlinebelow[]= "rlinebelow";
static const char GLYPH_rlongleg[]= "rlongleg";
static const char GLYPH_rlonglegturned[]= "rlonglegturned";
static const char GLYPH_rmonospace[]= "rmonospace";
static const char GLYPH_rohiragana[]= "rohiragana";
static const char GLYPH_rokatakana[]= "rokatakana";
static const char GLYPH_rokatakanahalfwidth[]= "rokatakanahalfwidth";
static const char GLYPH_roruathai[]= "roruathai";
static const char GLYPH_rparen[]= "rparen";
static const char GLYPH_rrabengali[]= "rrabengali";
static const char GLYPH_rradeva[]= "rradeva";
static const char GLYPH_rragurmukhi[]= "rragurmukhi";
static const char GLYPH_rrehfinalarabic[]= "rrehfinalarabic";
static const char GLYPH_rrvocalicbengali[]= "rrvocalicbengali";
static const char GLYPH_rrvocalicdeva[]= "rrvocalicdeva";
static const char GLYPH_rrvocalicgujarati[]= "rrvocalicgujarati";
static const char GLYPH_rrvocalicvowelsignbengali[]= "rrvocalicvowelsignbengali";
static const char GLYPH_rrvocalicvowelsigndeva[]= "rrvocalicvowelsigndeva";
static const char GLYPH_rrvocalicvowelsigngujarati[]= "rrvocalicvowelsigngujarati";
static const char GLYPH_rsuperior[]= "rsuperior";
static const char GLYPH_rturned[]= "rturned";
static const char GLYPH_rturnedsuperior[]= "rturnedsuperior";
static const char GLYPH_ruhiragana[]= "ruhiragana";
static const char GLYPH_rukatakana[]= "rukatakana";
static const char GLYPH_rukatakanahalfwidth[]= "rukatakanahalfwidth";
static const char GLYPH_rupeemarkbengali[]= "rupeemarkbengali";
static const char GLYPH_rupeesignbengali[]= "rupeesignbengali";
static const char GLYPH_rupiah[]= "rupiah";
static const char GLYPH_ruthai[]= "ruthai";
static const char GLYPH_rvocalicbengali[]= "rvocalicbengali";
static const char GLYPH_rvocalicdeva[]= "rvocalicdeva";
static const char GLYPH_rvocalicgujarati[]= "rvocalicgujarati";
static const char GLYPH_rvocalicvowelsignbengali[]= "rvocalicvowelsignbengali";
static const char GLYPH_rvocalicvowelsigndeva[]= "rvocalicvowelsigndeva";
static const char GLYPH_rvocalicvowelsigngujarati[]= "rvocalicvowelsigngujarati";
static const char GLYPH_sabengali[]= "sabengali";
static const char GLYPH_sacutedotaccent[]= "sacutedotaccent";
static const char GLYPH_sadeva[]= "sadeva";
static const char GLYPH_sadfinalarabic[]= "sadfinalarabic";
static const char GLYPH_sadinitialarabic[]= "sadinitialarabic";
static const char GLYPH_sadmedialarabic[]= "sadmedialarabic";
static const char GLYPH_sagujarati[]= "sagujarati";
static const char GLYPH_sagurmukhi[]= "sagurmukhi";
static const char GLYPH_sahiragana[]= "sahiragana";
static const char GLYPH_sakatakana[]= "sakatakana";
static const char GLYPH_sakatakanahalfwidth[]= "sakatakanahalfwidth";
static const char GLYPH_sallallahoualayhewasallamarabic[]= "sallallahoualayhewasallamarabic";
static const char GLYPH_samekhdagesh[]= "samekhdagesh";
static const char GLYPH_samekhdageshhebrew[]= "samekhdageshhebrew";
static const char GLYPH_saraaathai[]= "saraaathai";
static const char GLYPH_saraaethai[]= "saraaethai";
static const char GLYPH_saraaimaimalaithai[]= "saraaimaimalaithai";
static const char GLYPH_saraaimaimuanthai[]= "saraaimaimuanthai";
static const char GLYPH_saraamthai[]= "saraamthai";
static const char GLYPH_saraathai[]= "saraathai";
static const char GLYPH_saraethai[]= "saraethai";
static const char GLYPH_saraiileftthai[]= "saraiileftthai";
static const char GLYPH_saraiithai[]= "saraiithai";
static const char GLYPH_saraileftthai[]= "saraileftthai";
static const char GLYPH_saraithai[]= "saraithai";
static const char GLYPH_saraothai[]= "saraothai";
static const char GLYPH_saraueeleftthai[]= "saraueeleftthai";
static const char GLYPH_saraueethai[]= "saraueethai";
static const char GLYPH_saraueleftthai[]= "saraueleftthai";
static const char GLYPH_sarauethai[]= "sarauethai";
static const char GLYPH_sarauthai[]= "sarauthai";
static const char GLYPH_sarauuthai[]= "sarauuthai";
static const char GLYPH_sbopomofo[]= "sbopomofo";
static const char GLYPH_scarondotaccent[]= "scarondotaccent";
static const char GLYPH_schwa[]= "schwa";
static const char GLYPH_schwadieresiscyrillic[]= "schwadieresiscyrillic";
static const char GLYPH_schwahook[]= "schwahook";
static const char GLYPH_scircle[]= "scircle";
static const char GLYPH_sdotaccent[]= "sdotaccent";
static const char GLYPH_sdotbelow[]= "sdotbelow";
static const char GLYPH_sdotbelowdotaccent[]= "sdotbelowdotaccent";
static const char GLYPH_seagullbelowcmb[]= "seagullbelowcmb";
static const char GLYPH_secondtonechinese[]= "secondtonechinese";
static const char GLYPH_seenfinalarabic[]= "seenfinalarabic";
static const char GLYPH_seeninitialarabic[]= "seeninitialarabic";
static const char GLYPH_seenmedialarabic[]= "seenmedialarabic";
static const char GLYPH_segoltahebrew[]= "segoltahebrew";
static const char GLYPH_seharmenian[]= "seharmenian";
static const char GLYPH_sehiragana[]= "sehiragana";
static const char GLYPH_sekatakana[]= "sekatakana";
static const char GLYPH_sekatakanahalfwidth[]= "sekatakanahalfwidth";
static const char GLYPH_semicolonmonospace[]= "semicolonmonospace";
static const char GLYPH_semicolonsmall[]= "semicolonsmall";
static const char GLYPH_semivoicedmarkkana[]= "semivoicedmarkkana";
static const char GLYPH_semivoicedmarkkanahalfwidth[]= "semivoicedmarkkanahalfwidth";
static const char GLYPH_sentisquare[]= "sentisquare";
static const char GLYPH_sentosquare[]= "sentosquare";
static const char GLYPH_sevenbengali[]= "sevenbengali";
static const char GLYPH_sevencircle[]= "sevencircle";
static const char GLYPH_sevencircleinversesansserif[]= "sevencircleinversesansserif";
static const char GLYPH_sevendeva[]= "sevendeva";
static const char GLYPH_sevengujarati[]= "sevengujarati";
static const char GLYPH_sevengurmukhi[]= "sevengurmukhi";
static const char GLYPH_sevenhangzhou[]= "sevenhangzhou";
static const char GLYPH_sevenideographicparen[]= "sevenideographicparen";
static const char GLYPH_seveninferior[]= "seveninferior";
static const char GLYPH_sevenmonospace[]= "sevenmonospace";
static const char GLYPH_sevenoldstyle[]= "sevenoldstyle";
static const char GLYPH_sevenparen[]= "sevenparen";
static const char GLYPH_sevenperiod[]= "sevenperiod";
static const char GLYPH_sevenpersian[]= "sevenpersian";
static const char GLYPH_sevenroman[]= "sevenroman";
static const char GLYPH_sevensuperior[]= "sevensuperior";
static const char GLYPH_seventeencircle[]= "seventeencircle";
static const char GLYPH_seventeenparen[]= "seventeenparen";
static const char GLYPH_seventeenperiod[]= "seventeenperiod";
static const char GLYPH_seventhai[]= "seventhai";
static const char GLYPH_shaarmenian[]= "shaarmenian";
static const char GLYPH_shabengali[]= "shabengali";
static const char GLYPH_shaddadammaarabic[]= "shaddadammaarabic";
static const char GLYPH_shaddadammatanarabic[]= "shaddadammatanarabic";
static const char GLYPH_shaddafathaarabic[]= "shaddafathaarabic";
static const char GLYPH_shaddakasraarabic[]= "shaddakasraarabic";
static const char GLYPH_shaddakasratanarabic[]= "shaddakasratanarabic";
static const char GLYPH_shadeva[]= "shadeva";
static const char GLYPH_shagujarati[]= "shagujarati";
static const char GLYPH_shagurmukhi[]= "shagurmukhi";
static const char GLYPH_shalshelethebrew[]= "shalshelethebrew";
static const char GLYPH_shbopomofo[]= "shbopomofo";
static const char GLYPH_sheenfinalarabic[]= "sheenfinalarabic";
static const char GLYPH_sheeninitialarabic[]= "sheeninitialarabic";
static const char GLYPH_sheenmedialarabic[]= "sheenmedialarabic";
static const char GLYPH_sheicoptic[]= "sheicoptic";
static const char GLYPH_shhacyrillic[]= "shhacyrillic";
static const char GLYPH_shimacoptic[]= "shimacoptic";
static const char GLYPH_shindagesh[]= "shindagesh";
static const char GLYPH_shindageshhebrew[]= "shindageshhebrew";
static const char GLYPH_shindageshshindot[]= "shindageshshindot";
static const char GLYPH_shindageshshindothebrew[]= "shindageshshindothebrew";
static const char GLYPH_shindageshsindot[]= "shindageshsindot";
static const char GLYPH_shindageshsindothebrew[]= "shindageshsindothebrew";
static const char GLYPH_shinshindot[]= "shinshindot";
static const char GLYPH_shinshindothebrew[]= "shinshindothebrew";
static const char GLYPH_shinsindot[]= "shinsindot";
static const char GLYPH_shinsindothebrew[]= "shinsindothebrew";
static const char GLYPH_shook[]= "shook";
static const char GLYPH_sigmalunatesymbolgreek[]= "sigmalunatesymbolgreek";
static const char GLYPH_sihiragana[]= "sihiragana";
static const char GLYPH_sikatakana[]= "sikatakana";
static const char GLYPH_sikatakanahalfwidth[]= "sikatakanahalfwidth";
static const char GLYPH_siosacirclekorean[]= "siosacirclekorean";
static const char GLYPH_siosaparenkorean[]= "siosaparenkorean";
static const char GLYPH_sioscieuckorean[]= "sioscieuckorean";
static const char GLYPH_sioscirclekorean[]= "sioscirclekorean";
static const char GLYPH_sioskiyeokkorean[]= "sioskiyeokkorean";
static const char GLYPH_sioskorean[]= "sioskorean";
static const char GLYPH_siosnieunkorean[]= "siosnieunkorean";
static const char GLYPH_siosparenkorean[]= "siosparenkorean";
static const char GLYPH_siospieupkorean[]= "siospieupkorean";
static const char GLYPH_siostikeutkorean[]= "siostikeutkorean";
static const char GLYPH_sixbengali[]= "sixbengali";
static const char GLYPH_sixcircle[]= "sixcircle";
static const char GLYPH_sixcircleinversesansserif[]= "sixcircleinversesansserif";
static const char GLYPH_sixdeva[]= "sixdeva";
static const char GLYPH_sixgujarati[]= "sixgujarati";
static const char GLYPH_sixgurmukhi[]= "sixgurmukhi";
static const char GLYPH_sixhangzhou[]= "sixhangzhou";
static const char GLYPH_sixideographicparen[]= "sixideographicparen";
static const char GLYPH_sixinferior[]= "sixinferior";
static const char GLYPH_sixmonospace[]= "sixmonospace";
static const char GLYPH_sixoldstyle[]= "sixoldstyle";
static const char GLYPH_sixparen[]= "sixparen";
static const char GLYPH_sixperiod[]= "sixperiod";
static const char GLYPH_sixpersian[]= "sixpersian";
static const char GLYPH_sixroman[]= "sixroman";
static const char GLYPH_sixsuperior[]= "sixsuperior";
static const char GLYPH_sixteencircle[]= "sixteencircle";
static const char GLYPH_sixteencurrencydenominatorbengali[]= "sixteencurrencydenominatorbengali";
static const char GLYPH_sixteenparen[]= "sixteenparen";
static const char GLYPH_sixteenperiod[]= "sixteenperiod";
static const char GLYPH_sixthai[]= "sixthai";
static const char GLYPH_slashmonospace[]= "slashmonospace";
static const char GLYPH_slongdotaccent[]= "slongdotaccent";
static const char GLYPH_smonospace[]= "smonospace";
static const char GLYPH_softhyphen[]= "softhyphen";
static const char GLYPH_sohiragana[]= "sohiragana";
static const char GLYPH_sokatakana[]= "sokatakana";
static const char GLYPH_sokatakanahalfwidth[]= "sokatakanahalfwidth";
static const char GLYPH_soliduslongoverlaycmb[]= "soliduslongoverlaycmb";
static const char GLYPH_solidusshortoverlaycmb[]= "solidusshortoverlaycmb";
static const char GLYPH_sorusithai[]= "sorusithai";
static const char GLYPH_sosalathai[]= "sosalathai";
static const char GLYPH_sosothai[]= "sosothai";
static const char GLYPH_sosuathai[]= "sosuathai";
static const char GLYPH_spadesuitwhite[]= "spadesuitwhite";
static const char GLYPH_sparen[]= "sparen";
static const char GLYPH_squarebelowcmb[]= "squarebelowcmb";
static const char GLYPH_squarecc[]= "squarecc";
static const char GLYPH_squarecm[]= "squarecm";
static const char GLYPH_squarediagonalcrosshatchfill[]= "squarediagonalcrosshatchfill";
static const char GLYPH_squarehorizontalfill[]= "squarehorizontalfill";
static const char GLYPH_squarekg[]= "squarekg";
static const char GLYPH_squarekm[]= "squarekm";
static const char GLYPH_squarekmcapital[]= "squarekmcapital";
static const char GLYPH_squareln[]= "squareln";
static const char GLYPH_squarelog[]= "squarelog";
static const char GLYPH_squaremg[]= "squaremg";
static const char GLYPH_squaremil[]= "squaremil";
static const char GLYPH_squaremm[]= "squaremm";
static const char GLYPH_squaremsquared[]= "squaremsquared";
static const char GLYPH_squareorthogonalcrosshatchfill[]= "squareorthogonalcrosshatchfill";
static const char GLYPH_squareupperlefttolowerrightfill[]= "squareupperlefttolowerrightfill";
static const char GLYPH_squareupperrighttolowerleftfill[]= "squareupperrighttolowerleftfill";
static const char GLYPH_squareverticalfill[]= "squareverticalfill";
static const char GLYPH_squarewhitewithsmallblack[]= "squarewhitewithsmallblack";
static const char GLYPH_srsquare[]= "srsquare";
static const char GLYPH_ssabengali[]= "ssabengali";
static const char GLYPH_ssadeva[]= "ssadeva";
static const char GLYPH_ssagujarati[]= "ssagujarati";
static const char GLYPH_ssangcieuckorean[]= "ssangcieuckorean";
static const char GLYPH_ssanghieuhkorean[]= "ssanghieuhkorean";
static const char GLYPH_ssangieungkorean[]= "ssangieungkorean";
static const char GLYPH_ssangkiyeokkorean[]= "ssangkiyeokkorean";
static const char GLYPH_ssangnieunkorean[]= "ssangnieunkorean";
static const char GLYPH_ssangpieupkorean[]= "ssangpieupkorean";
static const char GLYPH_ssangsioskorean[]= "ssangsioskorean";
static const char GLYPH_ssangtikeutkorean[]= "ssangtikeutkorean";
static const char GLYPH_ssuperior[]= "ssuperior";
static const char GLYPH_sterlingmonospace[]= "sterlingmonospace";
static const char GLYPH_strokelongoverlaycmb[]= "strokelongoverlaycmb";
static const char GLYPH_strokeshortoverlaycmb[]= "strokeshortoverlaycmb";
static const char GLYPH_subsetnotequal[]= "subsetnotequal";
static const char GLYPH_succeeds[]= "succeeds";
static const char GLYPH_suhiragana[]= "suhiragana";
static const char GLYPH_sukatakana[]= "sukatakana";
static const char GLYPH_sukatakanahalfwidth[]= "sukatakanahalfwidth";
static const char GLYPH_supersetnotequal[]= "supersetnotequal";
static const char GLYPH_svsquare[]= "svsquare";
static const char GLYPH_syouwaerasquare[]= "syouwaerasquare";
static const char GLYPH_tabengali[]= "tabengali";
static const char GLYPH_tackdown[]= "tackdown";
static const char GLYPH_tackleft[]= "tackleft";
static const char GLYPH_tadeva[]= "tadeva";
static const char GLYPH_tagujarati[]= "tagujarati";
static const char GLYPH_tagurmukhi[]= "tagurmukhi";
static const char GLYPH_tahfinalarabic[]= "tahfinalarabic";
static const char GLYPH_tahinitialarabic[]= "tahinitialarabic";
static const char GLYPH_tahiragana[]= "tahiragana";
static const char GLYPH_tahmedialarabic[]= "tahmedialarabic";
static const char GLYPH_taisyouerasquare[]= "taisyouerasquare";
static const char GLYPH_takatakana[]= "takatakana";
static const char GLYPH_takatakanahalfwidth[]= "takatakanahalfwidth";
static const char GLYPH_tavdages[]= "tavdages";
static const char GLYPH_tavdagesh[]= "tavdagesh";
static const char GLYPH_tavdageshhebrew[]= "tavdageshhebrew";
static const char GLYPH_tbopomofo[]= "tbopomofo";
static const char GLYPH_tccurl[]= "tccurl";
static const char GLYPH_tchehfinalarabic[]= "tchehfinalarabic";
static const char GLYPH_tchehinitialarabic[]= "tchehinitialarabic";
static const char GLYPH_tchehmedialarabic[]= "tchehmedialarabic";
static const char GLYPH_tcircle[]= "tcircle";
static const char GLYPH_tcircumflexbelow[]= "tcircumflexbelow";
static const char GLYPH_tdieresis[]= "tdieresis";
static const char GLYPH_tdotaccent[]= "tdotaccent";
static const char GLYPH_tdotbelow[]= "tdotbelow";
static const char GLYPH_tedescendercyrillic[]= "tedescendercyrillic";
static const char GLYPH_tehfinalarabic[]= "tehfinalarabic";
static const char GLYPH_tehhahinitialarabic[]= "tehhahinitialarabic";
static const char GLYPH_tehhahisolatedarabic[]= "tehhahisolatedarabic";
static const char GLYPH_tehinitialarabic[]= "tehinitialarabic";
static const char GLYPH_tehiragana[]= "tehiragana";
static const char GLYPH_tehjeeminitialarabic[]= "tehjeeminitialarabic";
static const char GLYPH_tehjeemisolatedarabic[]= "tehjeemisolatedarabic";
static const char GLYPH_tehmarbutafinalarabic[]= "tehmarbutafinalarabic";
static const char GLYPH_tehmedialarabic[]= "tehmedialarabic";
static const char GLYPH_tehmeeminitialarabic[]= "tehmeeminitialarabic";
static const char GLYPH_tehmeemisolatedarabic[]= "tehmeemisolatedarabic";
static const char GLYPH_tehnoonfinalarabic[]= "tehnoonfinalarabic";
static const char GLYPH_tekatakana[]= "tekatakana";
static const char GLYPH_tekatakanahalfwidth[]= "tekatakanahalfwidth";
static const char GLYPH_telephone[]= "telephone";
static const char GLYPH_telephoneblack[]= "telephoneblack";
static const char GLYPH_telishagedolahebrew[]= "telishagedolahebrew";
static const char GLYPH_telishaqetanahebrew[]= "telishaqetanahebrew";
static const char GLYPH_tencircle[]= "tencircle";
static const char GLYPH_tenideographicparen[]= "tenideographicparen";
static const char GLYPH_tenparen[]= "tenparen";
static const char GLYPH_tenperiod[]= "tenperiod";
static const char GLYPH_tenroman[]= "tenroman";
static const char GLYPH_tesh[]= "tesh";
static const char GLYPH_tetdagesh[]= "tetdagesh";
static const char GLYPH_tetdageshhebrew[]= "tetdageshhebrew";
static const char GLYPH_tetsecyrillic[]= "tetsecyrillic";
static const char GLYPH_tevirhebrew[]= "tevirhebrew";
static const char GLYPH_tevirlefthebrew[]= "tevirlefthebrew";
static const char GLYPH_thabengali[]= "thabengali";
static const char GLYPH_thadeva[]= "thadeva";
static const char GLYPH_thagujarati[]= "thagujarati";
static const char GLYPH_thagurmukhi[]= "thagurmukhi";
static const char GLYPH_thalfinalarabic[]= "thalfinalarabic";
static const char GLYPH_thanthakhatlowleftthai[]= "thanthakhatlowleftthai";
static const char GLYPH_thanthakhatlowrightthai[]= "thanthakhatlowrightthai";
static const char GLYPH_thanthakhatthai[]= "thanthakhatthai";
static const char GLYPH_thanthakhatupperleftthai[]= "thanthakhatupperleftthai";
static const char GLYPH_thehfinalarabic[]= "thehfinalarabic";
static const char GLYPH_thehinitialarabic[]= "thehinitialarabic";
static const char GLYPH_thehmedialarabic[]= "thehmedialarabic";
static const char GLYPH_thieuthacirclekorean[]= "thieuthacirclekorean";
static const char GLYPH_thieuthaparenkorean[]= "thieuthaparenkorean";
static const char GLYPH_thieuthcirclekorean[]= "thieuthcirclekorean";
static const char GLYPH_thieuthkorean[]= "thieuthkorean";
static const char GLYPH_thieuthparenkorean[]= "thieuthparenkorean";
static const char GLYPH_thirteencircle[]= "thirteencircle";
static const char GLYPH_thirteenparen[]= "thirteenparen";
static const char GLYPH_thirteenperiod[]= "thirteenperiod";
static const char GLYPH_thonangmonthothai[]= "thonangmonthothai";
static const char GLYPH_thook[]= "thook";
static const char GLYPH_thophuthaothai[]= "thophuthaothai";
static const char GLYPH_thothahanthai[]= "thothahanthai";
static const char GLYPH_thothanthai[]= "thothanthai";
static const char GLYPH_thothongthai[]= "thothongthai";
static const char GLYPH_thothungthai[]= "thothungthai";
static const char GLYPH_thousandcyrillic[]= "thousandcyrillic";
static const char GLYPH_thousandsseparatorarabic[]= "thousandsseparatorarabic";
static const char GLYPH_thousandsseparatorpersian[]= "thousandsseparatorpersian";
static const char GLYPH_threebengali[]= "threebengali";
static const char GLYPH_threecircle[]= "threecircle";
static const char GLYPH_threecircleinversesansserif[]= "threecircleinversesansserif";
static const char GLYPH_threedeva[]= "threedeva";
static const char GLYPH_threegujarati[]= "threegujarati";
static const char GLYPH_threegurmukhi[]= "threegurmukhi";
static const char GLYPH_threehangzhou[]= "threehangzhou";
static const char GLYPH_threeideographicparen[]= "threeideographicparen";
static const char GLYPH_threeinferior[]= "threeinferior";
static const char GLYPH_threemonospace[]= "threemonospace";
static const char GLYPH_threenumeratorbengali[]= "threenumeratorbengali";
static const char GLYPH_threeoldstyle[]= "threeoldstyle";
static const char GLYPH_threeparen[]= "threeparen";
static const char GLYPH_threeperiod[]= "threeperiod";
static const char GLYPH_threepersian[]= "threepersian";
static const char GLYPH_threequartersemdash[]= "threequartersemdash";
static const char GLYPH_threeroman[]= "threeroman";
static const char GLYPH_threethai[]= "threethai";
static const char GLYPH_thzsquare[]= "thzsquare";
static const char GLYPH_tihiragana[]= "tihiragana";
static const char GLYPH_tikatakana[]= "tikatakana";
static const char GLYPH_tikatakanahalfwidth[]= "tikatakanahalfwidth";
static const char GLYPH_tikeutacirclekorean[]= "tikeutacirclekorean";
static const char GLYPH_tikeutaparenkorean[]= "tikeutaparenkorean";
static const char GLYPH_tikeutcirclekorean[]= "tikeutcirclekorean";
static const char GLYPH_tikeutkorean[]= "tikeutkorean";
static const char GLYPH_tikeutparenkorean[]= "tikeutparenkorean";
static const char GLYPH_tildebelowcmb[]= "tildebelowcmb";
static const char GLYPH_tildedoublecmb[]= "tildedoublecmb";
static const char GLYPH_tildeoverlaycmb[]= "tildeoverlaycmb";
static const char GLYPH_tildeverticalcmb[]= "tildeverticalcmb";
static const char GLYPH_tipehahebrew[]= "tipehahebrew";
static const char GLYPH_tipehalefthebrew[]= "tipehalefthebrew";
static const char GLYPH_tippigurmukhi[]= "tippigurmukhi";
static const char GLYPH_titlocyrilliccmb[]= "titlocyrilliccmb";
static const char GLYPH_tiwnarmenian[]= "tiwnarmenian";
static const char GLYPH_tlinebelow[]= "tlinebelow";
static const char GLYPH_tmonospace[]= "tmonospace";
static const char GLYPH_toarmenian[]= "toarmenian";
static const char GLYPH_tohiragana[]= "tohiragana";
static const char GLYPH_tokatakana[]= "tokatakana";
static const char GLYPH_tokatakanahalfwidth[]= "tokatakanahalfwidth";
static const char GLYPH_tonebarextrahighmod[]= "tonebarextrahighmod";
static const char GLYPH_tonebarextralowmod[]= "tonebarextralowmod";
static const char GLYPH_tonebarhighmod[]= "tonebarhighmod";
static const char GLYPH_tonebarlowmod[]= "tonebarlowmod";
static const char GLYPH_tonebarmidmod[]= "tonebarmidmod";
static const char GLYPH_tonefive[]= "tonefive";
static const char GLYPH_tonesix[]= "tonesix";
static const char GLYPH_tonetwo[]= "tonetwo";
static const char GLYPH_tonsquare[]= "tonsquare";
static const char GLYPH_topatakthai[]= "topatakthai";
static const char GLYPH_tortoiseshellbracketleft[]= "tortoiseshellbracketleft";
static const char GLYPH_tortoiseshellbracketleftsmall[]= "tortoiseshellbracketleftsmall";
static const char GLYPH_tortoiseshellbracketleftvertical[]= "tortoiseshellbracketleftvertical";
static const char GLYPH_tortoiseshellbracketright[]= "tortoiseshellbracketright";
static const char GLYPH_tortoiseshellbracketrightsmall[]= "tortoiseshellbracketrightsmall";
static const char GLYPH_tortoiseshellbracketrightvertical[]= "tortoiseshellbracketrightvertical";
static const char GLYPH_totaothai[]= "totaothai";
static const char GLYPH_tpalatalhook[]= "tpalatalhook";
static const char GLYPH_tparen[]= "tparen";
static const char GLYPH_tretroflexhook[]= "tretroflexhook";
static const char GLYPH_ts[]= "ts";
static const char GLYPH_tsadidagesh[]= "tsadidagesh";
static const char GLYPH_tsadidageshhebrew[]= "tsadidageshhebrew";
static const char GLYPH_tsuperior[]= "tsuperior";
static const char GLYPH_ttabengali[]= "ttabengali";
static const char GLYPH_ttadeva[]= "ttadeva";
static const char GLYPH_ttagujarati[]= "ttagujarati";
static const char GLYPH_ttagurmukhi[]= "ttagurmukhi";
static const char GLYPH_ttehfinalarabic[]= "ttehfinalarabic";
static const char GLYPH_ttehinitialarabic[]= "ttehinitialarabic";
static const char GLYPH_ttehmedialarabic[]= "ttehmedialarabic";
static const char GLYPH_tthabengali[]= "tthabengali";
static const char GLYPH_tthadeva[]= "tthadeva";
static const char GLYPH_tthagujarati[]= "tthagujarati";
static const char GLYPH_tthagurmukhi[]= "tthagurmukhi";
static const char GLYPH_tturned[]= "tturned";
static const char GLYPH_tuhiragana[]= "tuhiragana";
static const char GLYPH_tukatakana[]= "tukatakana";
static const char GLYPH_tukatakanahalfwidth[]= "tukatakanahalfwidth";
static const char GLYPH_tusmallhiragana[]= "tusmallhiragana";
static const char GLYPH_tusmallkatakana[]= "tusmallkatakana";
static const char GLYPH_tusmallkatakanahalfwidth[]= "tusmallkatakanahalfwidth";
static const char GLYPH_twelvecircle[]= "twelvecircle";
static const char GLYPH_twelveparen[]= "twelveparen";
static const char GLYPH_twelveperiod[]= "twelveperiod";
static const char GLYPH_twelveroman[]= "twelveroman";
static const char GLYPH_twentycircle[]= "twentycircle";
static const char GLYPH_twentyhangzhou[]= "twentyhangzhou";
static const char GLYPH_twentyparen[]= "twentyparen";
static const char GLYPH_twentyperiod[]= "twentyperiod";
static const char GLYPH_twobengali[]= "twobengali";
static const char GLYPH_twocircle[]= "twocircle";
static const char GLYPH_twocircleinversesansserif[]= "twocircleinversesansserif";
static const char GLYPH_twodeva[]= "twodeva";
static const char GLYPH_twodotleadervertical[]= "twodotleadervertical";
static const char GLYPH_twogujarati[]= "twogujarati";
static const char GLYPH_twogurmukhi[]= "twogurmukhi";
static const char GLYPH_twohangzhou[]= "twohangzhou";
static const char GLYPH_twoideographicparen[]= "twoideographicparen";
static const char GLYPH_twoinferior[]= "twoinferior";
static const char GLYPH_twomonospace[]= "twomonospace";
static const char GLYPH_twonumeratorbengali[]= "twonumeratorbengali";
static const char GLYPH_twooldstyle[]= "twooldstyle";
static const char GLYPH_twoparen[]= "twoparen";
static const char GLYPH_twoperiod[]= "twoperiod";
static const char GLYPH_twopersian[]= "twopersian";
static const char GLYPH_tworoman[]= "tworoman";
static const char GLYPH_twostroke[]= "twostroke";
static const char GLYPH_twothai[]= "twothai";
static const char GLYPH_ubar[]= "ubar";
static const char GLYPH_ubengali[]= "ubengali";
static const char GLYPH_ubopomofo[]= "ubopomofo";
static const char GLYPH_ucaron[]= "ucaron";
static const char GLYPH_ucircle[]= "ucircle";
static const char GLYPH_ucircumflexbelow[]= "ucircumflexbelow";
static const char GLYPH_udattadeva[]= "udattadeva";
static const char GLYPH_udblgrave[]= "udblgrave";
static const char GLYPH_udeva[]= "udeva";
static const char GLYPH_udieresisacute[]= "udieresisacute";
static const char GLYPH_udieresisbelow[]= "udieresisbelow";
static const char GLYPH_udieresiscaron[]= "udieresiscaron";
static const char GLYPH_udieresiscyrillic[]= "udieresiscyrillic";
static const char GLYPH_udieresisgrave[]= "udieresisgrave";
static const char GLYPH_udieresismacron[]= "udieresismacron";
static const char GLYPH_udotbelow[]= "udotbelow";
static const char GLYPH_ugujarati[]= "ugujarati";
static const char GLYPH_ugurmukhi[]= "ugurmukhi";
static const char GLYPH_uhiragana[]= "uhiragana";
static const char GLYPH_uhookabove[]= "uhookabove";
static const char GLYPH_uhornacute[]= "uhornacute";
static const char GLYPH_uhorndotbelow[]= "uhorndotbelow";
static const char GLYPH_uhorngrave[]= "uhorngrave";
static const char GLYPH_uhornhookabove[]= "uhornhookabove";
static const char GLYPH_uhorntilde[]= "uhorntilde";
static const char GLYPH_uhungarumlautcyrillic[]= "uhungarumlautcyrillic";
static const char GLYPH_uinvertedbreve[]= "uinvertedbreve";
static const char GLYPH_ukatakana[]= "ukatakana";
static const char GLYPH_ukatakanahalfwidth[]= "ukatakanahalfwidth";
static const char GLYPH_ukcyrillic[]= "ukcyrillic";
static const char GLYPH_ukorean[]= "ukorean";
static const char GLYPH_umacroncyrillic[]= "umacroncyrillic";
static const char GLYPH_umacrondieresis[]= "umacrondieresis";
static const char GLYPH_umatragurmukhi[]= "umatragurmukhi";
static const char GLYPH_umonospace[]= "umonospace";
static const char GLYPH_underscoremonospace[]= "underscoremonospace";
static const char GLYPH_underscorevertical[]= "underscorevertical";
static const char GLYPH_underscorewavy[]= "underscorewavy";
static const char GLYPH_uparen[]= "uparen";
static const char GLYPH_upperdothebrew[]= "upperdothebrew";
static const char GLYPH_upsilonlatin[]= "upsilonlatin";
static const char GLYPH_uptackbelowcmb[]= "uptackbelowcmb";
static const char GLYPH_uptackmod[]= "uptackmod";
static const char GLYPH_uragurmukhi[]= "uragurmukhi";
static const char GLYPH_usmallhiragana[]= "usmallhiragana";
static const char GLYPH_usmallkatakana[]= "usmallkatakana";
static const char GLYPH_usmallkatakanahalfwidth[]= "usmallkatakanahalfwidth";
static const char GLYPH_ustraightcyrillic[]= "ustraightcyrillic";
static const char GLYPH_ustraightstrokecyrillic[]= "ustraightstrokecyrillic";
static const char GLYPH_utildeacute[]= "utildeacute";
static const char GLYPH_utildebelow[]= "utildebelow";
static const char GLYPH_uubengali[]= "uubengali";
static const char GLYPH_uudeva[]= "uudeva";
static const char GLYPH_uugujarati[]= "uugujarati";
static const char GLYPH_uugurmukhi[]= "uugurmukhi";
static const char GLYPH_uumatragurmukhi[]= "uumatragurmukhi";
static const char GLYPH_uuvowelsignbengali[]= "uuvowelsignbengali";
static const char GLYPH_uuvowelsigndeva[]= "uuvowelsigndeva";
static const char GLYPH_uuvowelsigngujarati[]= "uuvowelsigngujarati";
static const char GLYPH_uvowelsignbengali[]= "uvowelsignbengali";
static const char GLYPH_uvowelsigndeva[]= "uvowelsigndeva";
static const char GLYPH_uvowelsigngujarati[]= "uvowelsigngujarati";
static const char GLYPH_vadeva[]= "vadeva";
static const char GLYPH_vagujarati[]= "vagujarati";
static const char GLYPH_vagurmukhi[]= "vagurmukhi";
static const char GLYPH_vakatakana[]= "vakatakana";
static const char GLYPH_vavdagesh[]= "vavdagesh";
static const char GLYPH_vavdagesh65[]= "vavdagesh65";
static const char GLYPH_vavdageshhebrew[]= "vavdageshhebrew";
static const char GLYPH_vavholam[]= "vavholam";
static const char GLYPH_vavholamhebrew[]= "vavholamhebrew";
static const char GLYPH_vcircle[]= "vcircle";
static const char GLYPH_vdotbelow[]= "vdotbelow";
static const char GLYPH_vehfinalarabic[]= "vehfinalarabic";
static const char GLYPH_vehinitialarabic[]= "vehinitialarabic";
static const char GLYPH_vehmedialarabic[]= "vehmedialarabic";
static const char GLYPH_vekatakana[]= "vekatakana";
static const char GLYPH_verticallineabovecmb[]= "verticallineabovecmb";
static const char GLYPH_verticallinebelowcmb[]= "verticallinebelowcmb";
static const char GLYPH_verticallinelowmod[]= "verticallinelowmod";
static const char GLYPH_verticallinemod[]= "verticallinemod";
static const char GLYPH_vewarmenian[]= "vewarmenian";
static const char GLYPH_vhook[]= "vhook";
static const char GLYPH_vikatakana[]= "vikatakana";
static const char GLYPH_viramabengali[]= "viramabengali";
static const char GLYPH_viramadeva[]= "viramadeva";
static const char GLYPH_viramagujarati[]= "viramagujarati";
static const char GLYPH_visargabengali[]= "visargabengali";
static const char GLYPH_visargadeva[]= "visargadeva";
static const char GLYPH_visargagujarati[]= "visargagujarati";
static const char GLYPH_vmonospace[]= "vmonospace";
static const char GLYPH_voarmenian[]= "voarmenian";
static const char GLYPH_voicediterationhiragana[]= "voicediterationhiragana";
static const char GLYPH_voicediterationkatakana[]= "voicediterationkatakana";
static const char GLYPH_voicedmarkkana[]= "voicedmarkkana";
static const char GLYPH_voicedmarkkanahalfwidth[]= "voicedmarkkanahalfwidth";
static const char GLYPH_vokatakana[]= "vokatakana";
static const char GLYPH_vparen[]= "vparen";
static const char GLYPH_vtilde[]= "vtilde";
static const char GLYPH_vturned[]= "vturned";
static const char GLYPH_vuhiragana[]= "vuhiragana";
static const char GLYPH_vukatakana[]= "vukatakana";
static const char GLYPH_waekorean[]= "waekorean";
static const char GLYPH_wahiragana[]= "wahiragana";
static const char GLYPH_wakatakana[]= "wakatakana";
static const char GLYPH_wakatakanahalfwidth[]= "wakatakanahalfwidth";
static const char GLYPH_wakorean[]= "wakorean";
static const char GLYPH_wasmallhiragana[]= "wasmallhiragana";
static const char GLYPH_wasmallkatakana[]= "wasmallkatakana";
static const char GLYPH_wattosquare[]= "wattosquare";
static const char GLYPH_wavedash[]= "wavedash";
static const char GLYPH_wavyunderscorevertical[]= "wavyunderscorevertical";
static const char GLYPH_wawfinalarabic[]= "wawfinalarabic";
static const char GLYPH_wawhamzaabovefinalarabic[]= "wawhamzaabovefinalarabic";
static const char GLYPH_wbsquare[]= "wbsquare";
static const char GLYPH_wcircle[]= "wcircle";
static const char GLYPH_wdotaccent[]= "wdotaccent";
static const char GLYPH_wdotbelow[]= "wdotbelow";
static const char GLYPH_wehiragana[]= "wehiragana";
static const char GLYPH_wekatakana[]= "wekatakana";
static const char GLYPH_wekorean[]= "wekorean";
static const char GLYPH_weokorean[]= "weokorean";
static const char GLYPH_whitecornerbracketleft[]= "whitecornerbracketleft";
static const char GLYPH_whitecornerbracketleftvertical[]= "whitecornerbracketleftvertical";
static const char GLYPH_whitecornerbracketright[]= "whitecornerbracketright";
static const char GLYPH_whitecornerbracketrightvertical[]= "whitecornerbracketrightvertical";
static const char GLYPH_whitediamond[]= "whitediamond";
static const char GLYPH_whitediamondcontainingblacksmalldiamond[]= "whitediamondcontainingblacksmalldiamond";
static const char GLYPH_whitedownpointingsmalltriangle[]= "whitedownpointingsmalltriangle";
static const char GLYPH_whitedownpointingtriangle[]= "whitedownpointingtriangle";
static const char GLYPH_whiteleftpointingsmalltriangle[]= "whiteleftpointingsmalltriangle";
static const char GLYPH_whiteleftpointingtriangle[]= "whiteleftpointingtriangle";
static const char GLYPH_whitelenticularbracketleft[]= "whitelenticularbracketleft";
static const char GLYPH_whitelenticularbracketright[]= "whitelenticularbracketright";
static const char GLYPH_whiterightpointingsmalltriangle[]= "whiterightpointingsmalltriangle";
static const char GLYPH_whiterightpointingtriangle[]= "whiterightpointingtriangle";
static const char GLYPH_whitestar[]= "whitestar";
static const char GLYPH_whitetelephone[]= "whitetelephone";
static const char GLYPH_whitetortoiseshellbracketleft[]= "whitetortoiseshellbracketleft";
static const char GLYPH_whitetortoiseshellbracketright[]= "whitetortoiseshellbracketright";
static const char GLYPH_whiteuppointingsmalltriangle[]= "whiteuppointingsmalltriangle";
static const char GLYPH_whiteuppointingtriangle[]= "whiteuppointingtriangle";
static const char GLYPH_wihiragana[]= "wihiragana";
static const char GLYPH_wikatakana[]= "wikatakana";
static const char GLYPH_wikorean[]= "wikorean";
static const char GLYPH_wmonospace[]= "wmonospace";
static const char GLYPH_wohiragana[]= "wohiragana";
static const char GLYPH_wokatakana[]= "wokatakana";
static const char GLYPH_wokatakanahalfwidth[]= "wokatakanahalfwidth";
static const char GLYPH_won[]= "won";
static const char GLYPH_wonmonospace[]= "wonmonospace";
static const char GLYPH_wowaenthai[]= "wowaenthai";
static const char GLYPH_wparen[]= "wparen";
static const char GLYPH_wring[]= "wring";
static const char GLYPH_wsuperior[]= "wsuperior";
static const char GLYPH_wturned[]= "wturned";
static const char GLYPH_wynn[]= "wynn";
static const char GLYPH_xabovecmb[]= "xabovecmb";
static const char GLYPH_xbopomofo[]= "xbopomofo";
static const char GLYPH_xcircle[]= "xcircle";
static const char GLYPH_xdieresis[]= "xdieresis";
static const char GLYPH_xdotaccent[]= "xdotaccent";
static const char GLYPH_xeharmenian[]= "xeharmenian";
static const char GLYPH_xmonospace[]= "xmonospace";
static const char GLYPH_xparen[]= "xparen";
static const char GLYPH_xsuperior[]= "xsuperior";
static const char GLYPH_yaadosquare[]= "yaadosquare";
static const char GLYPH_yabengali[]= "yabengali";
static const char GLYPH_yadeva[]= "yadeva";
static const char GLYPH_yaekorean[]= "yaekorean";
static const char GLYPH_yagujarati[]= "yagujarati";
static const char GLYPH_yagurmukhi[]= "yagurmukhi";
static const char GLYPH_yahiragana[]= "yahiragana";
static const char GLYPH_yakatakana[]= "yakatakana";
static const char GLYPH_yakatakanahalfwidth[]= "yakatakanahalfwidth";
static const char GLYPH_yakorean[]= "yakorean";
static const char GLYPH_yamakkanthai[]= "yamakkanthai";
static const char GLYPH_yasmallhiragana[]= "yasmallhiragana";
static const char GLYPH_yasmallkatakana[]= "yasmallkatakana";
static const char GLYPH_yasmallkatakanahalfwidth[]= "yasmallkatakanahalfwidth";
static const char GLYPH_ycircle[]= "ycircle";
static const char GLYPH_ydotaccent[]= "ydotaccent";
static const char GLYPH_ydotbelow[]= "ydotbelow";
static const char GLYPH_yehbarreefinalarabic[]= "yehbarreefinalarabic";
static const char GLYPH_yehfinalarabic[]= "yehfinalarabic";
static const char GLYPH_yehhamzaabovefinalarabic[]= "yehhamzaabovefinalarabic";
static const char GLYPH_yehhamzaaboveinitialarabic[]= "yehhamzaaboveinitialarabic";
static const char GLYPH_yehhamzaabovemedialarabic[]= "yehhamzaabovemedialarabic";
static const char GLYPH_yehinitialarabic[]= "yehinitialarabic";
static const char GLYPH_yehmedialarabic[]= "yehmedialarabic";
static const char GLYPH_yehmeeminitialarabic[]= "yehmeeminitialarabic";
static const char GLYPH_yehmeemisolatedarabic[]= "yehmeemisolatedarabic";
static const char GLYPH_yehnoonfinalarabic[]= "yehnoonfinalarabic";
static const char GLYPH_yehthreedotsbelowarabic[]= "yehthreedotsbelowarabic";
static const char GLYPH_yekorean[]= "yekorean";
static const char GLYPH_yenmonospace[]= "yenmonospace";
static const char GLYPH_yeokorean[]= "yeokorean";
static const char GLYPH_yeorinhieuhkorean[]= "yeorinhieuhkorean";
static const char GLYPH_yerahbenyomohebrew[]= "yerahbenyomohebrew";
static const char GLYPH_yerahbenyomolefthebrew[]= "yerahbenyomolefthebrew";
static const char GLYPH_yerudieresiscyrillic[]= "yerudieresiscyrillic";
static const char GLYPH_yesieungkorean[]= "yesieungkorean";
static const char GLYPH_yesieungpansioskorean[]= "yesieungpansioskorean";
static const char GLYPH_yesieungsioskorean[]= "yesieungsioskorean";
static const char GLYPH_yetivhebrew[]= "yetivhebrew";
static const char GLYPH_yhook[]= "yhook";
static const char GLYPH_yhookabove[]= "yhookabove";
static const char GLYPH_yiarmenian[]= "yiarmenian";
static const char GLYPH_yikorean[]= "yikorean";
static const char GLYPH_yinyang[]= "yinyang";
static const char GLYPH_yiwnarmenian[]= "yiwnarmenian";
static const char GLYPH_ymonospace[]= "ymonospace";
static const char GLYPH_yoddagesh[]= "yoddagesh";
static const char GLYPH_yoddageshhebrew[]= "yoddageshhebrew";
static const char GLYPH_yodyodpatahhebrew[]= "yodyodpatahhebrew";
static const char GLYPH_yohiragana[]= "yohiragana";
static const char GLYPH_yoikorean[]= "yoikorean";
static const char GLYPH_yokatakana[]= "yokatakana";
static const char GLYPH_yokatakanahalfwidth[]= "yokatakanahalfwidth";
static const char GLYPH_yokorean[]= "yokorean";
static const char GLYPH_yosmallhiragana[]= "yosmallhiragana";
static const char GLYPH_yosmallkatakana[]= "yosmallkatakana";
static const char GLYPH_yosmallkatakanahalfwidth[]= "yosmallkatakanahalfwidth";
static const char GLYPH_yotgreek[]= "yotgreek";
static const char GLYPH_yoyaekorean[]= "yoyaekorean";
static const char GLYPH_yoyakorean[]= "yoyakorean";
static const char GLYPH_yoyakthai[]= "yoyakthai";
static const char GLYPH_yoyingthai[]= "yoyingthai";
static const char GLYPH_yparen[]= "yparen";
static const char GLYPH_ypogegrammenigreekcmb[]= "ypogegrammenigreekcmb";
static const char GLYPH_yr[]= "yr";
static const char GLYPH_yring[]= "yring";
static const char GLYPH_ysuperior[]= "ysuperior";
static const char GLYPH_ytilde[]= "ytilde";
static const char GLYPH_yturned[]= "yturned";
static const char GLYPH_yuhiragana[]= "yuhiragana";
static const char GLYPH_yuikorean[]= "yuikorean";
static const char GLYPH_yukatakana[]= "yukatakana";
static const char GLYPH_yukatakanahalfwidth[]= "yukatakanahalfwidth";
static const char GLYPH_yukorean[]= "yukorean";
static const char GLYPH_yusbigcyrillic[]= "yusbigcyrillic";
static const char GLYPH_yusbigiotifiedcyrillic[]= "yusbigiotifiedcyrillic";
static const char GLYPH_yuslittlecyrillic[]= "yuslittlecyrillic";
static const char GLYPH_yuslittleiotifiedcyrillic[]= "yuslittleiotifiedcyrillic";
static const char GLYPH_yusmallhiragana[]= "yusmallhiragana";
static const char GLYPH_yusmallkatakana[]= "yusmallkatakana";
static const char GLYPH_yusmallkatakanahalfwidth[]= "yusmallkatakanahalfwidth";
static const char GLYPH_yuyekorean[]= "yuyekorean";
static const char GLYPH_yuyeokorean[]= "yuyeokorean";
static const char GLYPH_yyabengali[]= "yyabengali";
static const char GLYPH_yyadeva[]= "yyadeva";
static const char GLYPH_zaarmenian[]= "zaarmenian";
static const char GLYPH_zadeva[]= "zadeva";
static const char GLYPH_zagurmukhi[]= "zagurmukhi";
static const char GLYPH_zahfinalarabic[]= "zahfinalarabic";
static const char GLYPH_zahinitialarabic[]= "zahinitialarabic";
static const char GLYPH_zahiragana[]= "zahiragana";
static const char GLYPH_zahmedialarabic[]= "zahmedialarabic";
static const char GLYPH_zainfinalarabic[]= "zainfinalarabic";
static const char GLYPH_zakatakana[]= "zakatakana";
static const char GLYPH_zaqefgadolhebrew[]= "zaqefgadolhebrew";
static const char GLYPH_zaqefqatanhebrew[]= "zaqefqatanhebrew";
static const char GLYPH_zarqahebrew[]= "zarqahebrew";
static const char GLYPH_zayindagesh[]= "zayindagesh";
static const char GLYPH_zayindageshhebrew[]= "zayindageshhebrew";
static const char GLYPH_zbopomofo[]= "zbopomofo";
static const char GLYPH_zcircle[]= "zcircle";
static const char GLYPH_zcircumflex[]= "zcircumflex";
static const char GLYPH_zcurl[]= "zcurl";
static const char GLYPH_zdotbelow[]= "zdotbelow";
static const char GLYPH_zedescendercyrillic[]= "zedescendercyrillic";
static const char GLYPH_zedieresiscyrillic[]= "zedieresiscyrillic";
static const char GLYPH_zehiragana[]= "zehiragana";
static const char GLYPH_zekatakana[]= "zekatakana";
static const char GLYPH_zerobengali[]= "zerobengali";
static const char GLYPH_zerodeva[]= "zerodeva";
static const char GLYPH_zerogujarati[]= "zerogujarati";
static const char GLYPH_zerogurmukhi[]= "zerogurmukhi";
static const char GLYPH_zeroinferior[]= "zeroinferior";
static const char GLYPH_zeromonospace[]= "zeromonospace";
static const char GLYPH_zerooldstyle[]= "zerooldstyle";
static const char GLYPH_zeropersian[]= "zeropersian";
static const char GLYPH_zerosuperior[]= "zerosuperior";
static const char GLYPH_zerothai[]= "zerothai";
static const char GLYPH_zerowidthjoiner[]= "zerowidthjoiner";
static const char GLYPH_zerowidthspace[]= "zerowidthspace";
static const char GLYPH_zhbopomofo[]= "zhbopomofo";
static const char GLYPH_zhearmenian[]= "zhearmenian";
static const char GLYPH_zhebrevecyrillic[]= "zhebrevecyrillic";
static const char GLYPH_zhedescendercyrillic[]= "zhedescendercyrillic";
static const char GLYPH_zhedieresiscyrillic[]= "zhedieresiscyrillic";
static const char GLYPH_zihiragana[]= "zihiragana";
static const char GLYPH_zikatakana[]= "zikatakana";
static const char GLYPH_zinorhebrew[]= "zinorhebrew";
static const char GLYPH_zlinebelow[]= "zlinebelow";
static const char GLYPH_zmonospace[]= "zmonospace";
static const char GLYPH_zohiragana[]= "zohiragana";
static const char GLYPH_zokatakana[]= "zokatakana";
static const char GLYPH_zparen[]= "zparen";
static const char GLYPH_zretroflexhook[]= "zretroflexhook";
static const char GLYPH_zstroke[]= "zstroke";
static const char GLYPH_zuhiragana[]= "zuhiragana";
static const char GLYPH_zukatakana[]= "zukatakana";
/************************************************************************/

/************************************************************************/
/*									*/
/*  Alternate glyphs or glyph names as I have encontered them in	*/
/*  actual afm files.							*/
/*									*/
/************************************************************************/

static const char GLYPH_Cdot[]= "Cdot";
static const char GLYPH_Dslash[]= "Dslash";
static const char GLYPH_increment[]= "increment";
static const char GLYPH_Edot[]= "Edot";
static const char GLYPH_euro[]= "euro";
static const char GLYPH_Gcedilla[]= "Gcedilla";
static const char GLYPH_Gdot[]= "Gdot";
static const char GLYPH_blackcircle[]= "blackcircle";
static const char GLYPH_blacksmallsquare[]= "blacksmallsquare";
static const char GLYPH_whitesmallsquare[]= "whitesmallsquare";
static const char GLYPH_whitesquare[]= "whitesquare";
static const char GLYPH_Idot[]= "Idot";
static const char GLYPH_Kcedilla[]= "Kcedilla";
static const char GLYPH_Lcedilla[]= "Lcedilla";
static const char GLYPH_Ldotaccent[]= "Ldotaccent";
static const char GLYPH_Ncedilla[]= "Ncedilla";
static const char GLYPH_Odblacute[]= "Odblacute";
static const char GLYPH_Ohm[]= "Ohm";
static const char GLYPH_Ostrokeacute[]= "Ostrokeacute";
static const char GLYPH_Rcedilla[]= "Rcedilla";
static const char GLYPH_Tcedilla[]= "Tcedilla";
static const char GLYPH_Udblacute[]= "Udblacute";
static const char GLYPH_Upsilonhooksymbol[]= "Upsilonhooksymbol";
static const char GLYPH_Zdot[]= "Zdot";
static const char GLYPH_acutecmb[]= "acutecmb";
static const char GLYPH_horizontalbar[]= "horizontalbar";
static const char GLYPH_Acyrillic[]= "Acyrillic";
static const char GLYPH_Becyrillic[]= "Becyrillic";
static const char GLYPH_Vecyrillic[]= "Vecyrillic";
static const char GLYPH_Gecyrillic[]= "Gecyrillic";
static const char GLYPH_Decyrillic[]= "Decyrillic";
static const char GLYPH_Iecyrillic[]= "Iecyrillic";
static const char GLYPH_Iocyrillic[]= "Iocyrillic";
static const char GLYPH_Zhecyrillic[]= "Zhecyrillic";
static const char GLYPH_Zecyrillic[]= "Zecyrillic";
static const char GLYPH_Iicyrillic[]= "Iicyrillic";
static const char GLYPH_Iishortcyrillic[]= "Iishortcyrillic";
static const char GLYPH_Kacyrillic[]= "Kacyrillic";
static const char GLYPH_Elcyrillic[]= "Elcyrillic";
static const char GLYPH_Emcyrillic[]= "Emcyrillic";
static const char GLYPH_Encyrillic[]= "Encyrillic";
static const char GLYPH_Ocyrillic[]= "Ocyrillic";
static const char GLYPH_Pecyrillic[]= "Pecyrillic";
static const char GLYPH_Ercyrillic[]= "Ercyrillic";
static const char GLYPH_Escyrillic[]= "Escyrillic";
static const char GLYPH_Tecyrillic[]= "Tecyrillic";
static const char GLYPH_Ucyrillic[]= "Ucyrillic";
static const char GLYPH_Efcyrillic[]= "Efcyrillic";
static const char GLYPH_Khacyrillic[]= "Khacyrillic";
static const char GLYPH_Tsecyrillic[]= "Tsecyrillic";
static const char GLYPH_Checyrillic[]= "Checyrillic";
static const char GLYPH_Shacyrillic[]= "Shacyrillic";
static const char GLYPH_Shchacyrillic[]= "Shchacyrillic";
static const char GLYPH_Hardsigncyrillic[]= "Hardsigncyrillic";
static const char GLYPH_Yericyrillic[]= "Yericyrillic";
static const char GLYPH_Softsigncyrillic[]= "Softsigncyrillic";
static const char GLYPH_Ereversedcyrillic[]= "Ereversedcyrillic";
static const char GLYPH_IUcyrillic[]= "IUcyrillic";
static const char GLYPH_IAcyrillic[]= "IAcyrillic";
static const char GLYPH_Gheupturncyrillic[]= "Gheupturncyrillic";
static const char GLYPH_Djecyrillic[]= "Djecyrillic";
static const char GLYPH_Gjecyrillic[]= "Gjecyrillic";
static const char GLYPH_Ecyrillic[]= "Ecyrillic";
static const char GLYPH_Dzecyrillic[]= "Dzecyrillic";
static const char GLYPH_Icyrillic[]= "Icyrillic";
static const char GLYPH_Yicyrillic[]= "Yicyrillic";
static const char GLYPH_Jecyrillic[]= "Jecyrillic";
static const char GLYPH_Ljecyrillic[]= "Ljecyrillic";
static const char GLYPH_Njecyrillic[]= "Njecyrillic";
static const char GLYPH_Tshecyrillic[]= "Tshecyrillic";
static const char GLYPH_Kjecyrillic[]= "Kjecyrillic";
static const char GLYPH_Ushortcyrillic[]= "Ushortcyrillic";
static const char GLYPH_acyrillic[]= "acyrillic";
static const char GLYPH_becyrillic[]= "becyrillic";
static const char GLYPH_vecyrillic[]= "vecyrillic";
static const char GLYPH_gecyrillic[]= "gecyrillic";
static const char GLYPH_decyrillic[]= "decyrillic";
static const char GLYPH_iecyrillic[]= "iecyrillic";
static const char GLYPH_iocyrillic[]= "iocyrillic";
static const char GLYPH_zhecyrillic[]= "zhecyrillic";
static const char GLYPH_zecyrillic[]= "zecyrillic";
static const char GLYPH_iicyrillic[]= "iicyrillic";
static const char GLYPH_iishortcyrillic[]= "iishortcyrillic";
static const char GLYPH_kacyrillic[]= "kacyrillic";
static const char GLYPH_elcyrillic[]= "elcyrillic";
static const char GLYPH_emcyrillic[]= "emcyrillic";
static const char GLYPH_encyrillic[]= "encyrillic";
static const char GLYPH_ocyrillic[]= "ocyrillic";
static const char GLYPH_pecyrillic[]= "pecyrillic";
static const char GLYPH_ercyrillic[]= "ercyrillic";
static const char GLYPH_escyrillic[]= "escyrillic";
static const char GLYPH_tecyrillic[]= "tecyrillic";
static const char GLYPH_ucyrillic[]= "ucyrillic";
static const char GLYPH_efcyrillic[]= "efcyrillic";
static const char GLYPH_khacyrillic[]= "khacyrillic";
static const char GLYPH_tsecyrillic[]= "tsecyrillic";
static const char GLYPH_checyrillic[]= "checyrillic";
static const char GLYPH_shacyrillic[]= "shacyrillic";
static const char GLYPH_shchacyrillic[]= "shchacyrillic";
static const char GLYPH_hardsigncyrillic[]= "hardsigncyrillic";
static const char GLYPH_yericyrillic[]= "yericyrillic";
static const char GLYPH_softsigncyrillic[]= "softsigncyrillic";
static const char GLYPH_ereversedcyrillic[]= "ereversedcyrillic";
static const char GLYPH_iucyrillic[]= "iucyrillic";
static const char GLYPH_iacyrillic[]= "iacyrillic";
static const char GLYPH_gheupturncyrillic[]= "gheupturncyrillic";
static const char GLYPH_djecyrillic[]= "djecyrillic";
static const char GLYPH_gjecyrillic[]= "gjecyrillic";
static const char GLYPH_ecyrillic[]= "ecyrillic";
static const char GLYPH_dzecyrillic[]= "dzecyrillic";
static const char GLYPH_icyrillic[]= "icyrillic";
static const char GLYPH_yicyrillic[]= "yicyrillic";
static const char GLYPH_jecyrillic[]= "jecyrillic";
static const char GLYPH_ljecyrillic[]= "ljecyrillic";
static const char GLYPH_njecyrillic[]= "njecyrillic";
static const char GLYPH_tshecyrillic[]= "tshecyrillic";
static const char GLYPH_kjecyrillic[]= "kjecyrillic";
static const char GLYPH_ushortcyrillic[]= "ushortcyrillic";
static const char GLYPH_Dzhecyrillic[]= "Dzhecyrillic";
static const char GLYPH_Yatcyrillic[]= "Yatcyrillic";
static const char GLYPH_Fitacyrillic[]= "Fitacyrillic";
static const char GLYPH_Izhitsacyrillic[]= "Izhitsacyrillic";
static const char GLYPH_dzhecyrillic[]= "dzhecyrillic";
static const char GLYPH_yatcyrillic[]= "yatcyrillic";
static const char GLYPH_fitacyrillic[]= "fitacyrillic";
static const char GLYPH_izhitsacyrillic[]= "izhitsacyrillic";
static const char GLYPH_schwacyrillic[]= "schwacyrillic";
static const char GLYPH_percentarabic[]= "percentarabic";
static const char GLYPH_commaarabic[]= "commaarabic";
static const char GLYPH_zeroarabic[]= "zeroarabic";
static const char GLYPH_zerohackarabic[]= "zerohackarabic";
static const char GLYPH_onearabic[]= "onearabic";
static const char GLYPH_onehackarabic[]= "onehackarabic";
static const char GLYPH_twoarabic[]= "twoarabic";
static const char GLYPH_twohackarabic[]= "twohackarabic";
static const char GLYPH_threearabic[]= "threearabic";
static const char GLYPH_threehackarabic[]= "threehackarabic";
static const char GLYPH_fourarabic[]= "fourarabic";
static const char GLYPH_fourhackarabic[]= "fourhackarabic";
static const char GLYPH_fivearabic[]= "fivearabic";
static const char GLYPH_fivehackarabic[]= "fivehackarabic";
static const char GLYPH_sixarabic[]= "sixarabic";
static const char GLYPH_sixhackarabic[]= "sixhackarabic";
static const char GLYPH_sevenarabic[]= "sevenarabic";
static const char GLYPH_sevenhackarabic[]= "sevenhackarabic";
static const char GLYPH_eightarabic[]= "eightarabic";
static const char GLYPH_eighthackarabic[]= "eighthackarabic";
static const char GLYPH_ninearabic[]= "ninearabic";
static const char GLYPH_ninehackarabic[]= "ninehackarabic";
static const char GLYPH_semicolonarabic[]= "semicolonarabic";
static const char GLYPH_questionarabic[]= "questionarabic";
static const char GLYPH_hamzaarabic[]= "hamzaarabic";
static const char GLYPH_hamzalowarabic[]= "hamzalowarabic";
static const char GLYPH_alefmaddaabovearabic[]= "alefmaddaabovearabic";
static const char GLYPH_alefhamzaabovearabic[]= "alefhamzaabovearabic";
static const char GLYPH_wawhamzaabovearabic[]= "wawhamzaabovearabic";
static const char GLYPH_alefhamzabelowarabic[]= "alefhamzabelowarabic";
static const char GLYPH_yehhamzaabovearabic[]= "yehhamzaabovearabic";
static const char GLYPH_alefarabic[]= "alefarabic";
static const char GLYPH_beharabic[]= "beharabic";
static const char GLYPH_tehmarbutaarabic[]= "tehmarbutaarabic";
static const char GLYPH_teharabic[]= "teharabic";
static const char GLYPH_theharabic[]= "theharabic";
static const char GLYPH_jeemarabic[]= "jeemarabic";
static const char GLYPH_haharabic[]= "haharabic";
static const char GLYPH_khaharabic[]= "khaharabic";
static const char GLYPH_dalarabic[]= "dalarabic";
static const char GLYPH_thalarabic[]= "thalarabic";
static const char GLYPH_reharabic[]= "reharabic";
static const char GLYPH_zainarabic[]= "zainarabic";
static const char GLYPH_seenarabic[]= "seenarabic";
static const char GLYPH_sheenarabic[]= "sheenarabic";
static const char GLYPH_sadarabic[]= "sadarabic";
static const char GLYPH_dadarabic[]= "dadarabic";
static const char GLYPH_taharabic[]= "taharabic";
static const char GLYPH_zaharabic[]= "zaharabic";
static const char GLYPH_ainarabic[]= "ainarabic";
static const char GLYPH_ghainarabic[]= "ghainarabic";
static const char GLYPH_kashidaautoarabic[]= "kashidaautoarabic";
static const char GLYPH_kashidaautonosidebearingarabic[]= "kashidaautonosidebearingarabic";
static const char GLYPH_tatweelarabic[]= "tatweelarabic";
static const char GLYPH_feharabic[]= "feharabic";
static const char GLYPH_qafarabic[]= "qafarabic";
static const char GLYPH_kafarabic[]= "kafarabic";
static const char GLYPH_lamarabic[]= "lamarabic";
static const char GLYPH_meemarabic[]= "meemarabic";
static const char GLYPH_noonarabic[]= "noonarabic";
static const char GLYPH_wawarabic[]= "wawarabic";
static const char GLYPH_alefmaksuraarabic[]= "alefmaksuraarabic";
static const char GLYPH_yeharabic[]= "yeharabic";
static const char GLYPH_fathatanarabic[]= "fathatanarabic";
static const char GLYPH_dammatanaltonearabic[]= "dammatanaltonearabic";
static const char GLYPH_dammatanarabic[]= "dammatanarabic";
static const char GLYPH_kasratanarabic[]= "kasratanarabic";
static const char GLYPH_fathaarabic[]= "fathaarabic";
static const char GLYPH_fathalowarabic[]= "fathalowarabic";
static const char GLYPH_dammaarabic[]= "dammaarabic";
static const char GLYPH_dammalowarabic[]= "dammalowarabic";
static const char GLYPH_kasraarabic[]= "kasraarabic";
static const char GLYPH_shaddaarabic[]= "shaddaarabic";
static const char GLYPH_sukunarabic[]= "sukunarabic";
static const char GLYPH_heharabic[]= "heharabic";
static const char GLYPH_veharabic[]= "veharabic";
static const char GLYPH_peharabic[]= "peharabic";
static const char GLYPH_tcheharabic[]= "tcheharabic";
static const char GLYPH_jeharabic[]= "jeharabic";
static const char GLYPH_gafarabic[]= "gafarabic";
static const char GLYPH_tteharabic[]= "tteharabic";
static const char GLYPH_ddalarabic[]= "ddalarabic";
static const char GLYPH_rreharabic[]= "rreharabic";
static const char GLYPH_noonghunnaarabic[]= "noonghunnaarabic";
static const char GLYPH_yehbarreearabic[]= "yehbarreearabic";
static const char GLYPH_newsheqelsign[]= "newsheqelsign";
static const char GLYPH_sheqel[]= "sheqel";
static const char GLYPH_sheqelhebrew[]= "sheqelhebrew";
static const char GLYPH_maqafhebrew[]= "maqafhebrew";
static const char GLYPH_sofpasuqhebrew[]= "sofpasuqhebrew";
static const char GLYPH_alef[]= "alef";
static const char GLYPH_alefhebrew[]= "alefhebrew";
static const char GLYPH_bet[]= "bet";
static const char GLYPH_bethebrew[]= "bethebrew";
static const char GLYPH_gimel[]= "gimel";
static const char GLYPH_gimelhebrew[]= "gimelhebrew";
static const char GLYPH_dalet[]= "dalet";
static const char GLYPH_dalethebrew[]= "dalethebrew";
static const char GLYPH_he[]= "he";
static const char GLYPH_hehebrew[]= "hehebrew";
static const char GLYPH_vav[]= "vav";
static const char GLYPH_vavhebrew[]= "vavhebrew";
static const char GLYPH_zayin[]= "zayin";
static const char GLYPH_zayinhebrew[]= "zayinhebrew";
static const char GLYPH_het[]= "het";
static const char GLYPH_hethebrew[]= "hethebrew";
static const char GLYPH_tet[]= "tet";
static const char GLYPH_tethebrew[]= "tethebrew";
static const char GLYPH_yod[]= "yod";
static const char GLYPH_yodhebrew[]= "yodhebrew";
static const char GLYPH_finalkaf[]= "finalkaf";
static const char GLYPH_finalkafhebrew[]= "finalkafhebrew";
static const char GLYPH_kaf[]= "kaf";
static const char GLYPH_kafhebrew[]= "kafhebrew";
static const char GLYPH_lamed[]= "lamed";
static const char GLYPH_lamedhebrew[]= "lamedhebrew";
static const char GLYPH_finalmem[]= "finalmem";
static const char GLYPH_finalmemhebrew[]= "finalmemhebrew";
static const char GLYPH_mem[]= "mem";
static const char GLYPH_memhebrew[]= "memhebrew";
static const char GLYPH_finalnun[]= "finalnun";
static const char GLYPH_finalnunhebrew[]= "finalnunhebrew";
static const char GLYPH_nun[]= "nun";
static const char GLYPH_nunhebrew[]= "nunhebrew";
static const char GLYPH_samekh[]= "samekh";
static const char GLYPH_samekhhebrew[]= "samekhhebrew";
static const char GLYPH_ayin[]= "ayin";
static const char GLYPH_ayinhebrew[]= "ayinhebrew";
static const char GLYPH_finalpe[]= "finalpe";
static const char GLYPH_finalpehebrew[]= "finalpehebrew";
static const char GLYPH_pe[]= "pe";
static const char GLYPH_pehebrew[]= "pehebrew";
static const char GLYPH_finaltsadi[]= "finaltsadi";
static const char GLYPH_finaltsadihebrew[]= "finaltsadihebrew";
static const char GLYPH_tsadi[]= "tsadi";
static const char GLYPH_tsadihebrew[]= "tsadihebrew";
static const char GLYPH_qof[]= "qof";
static const char GLYPH_qofhebrew[]= "qofhebrew";
static const char GLYPH_resh[]= "resh";
static const char GLYPH_reshhebrew[]= "reshhebrew";
static const char GLYPH_shin[]= "shin";
static const char GLYPH_shinhebrew[]= "shinhebrew";
static const char GLYPH_tav[]= "tav";
static const char GLYPH_tavhebrew[]= "tavhebrew";
static const char GLYPH_vavvavhebrew[]= "vavvavhebrew";
static const char GLYPH_vavyodhebrew[]= "vavyodhebrew";
static const char GLYPH_yodyodhebrew[]= "yodyodhebrew";
static const char GLYPH_hiriq[]= "hiriq";
static const char GLYPH_hiriq14[]= "hiriq14";
static const char GLYPH_hiriq21[]= "hiriq21";
static const char GLYPH_hiriq2d[]= "hiriq2d";
static const char GLYPH_hiriqhebrew[]= "hiriqhebrew";
static const char GLYPH_hiriqnarrowhebrew[]= "hiriqnarrowhebrew";
static const char GLYPH_hiriqquarterhebrew[]= "hiriqquarterhebrew";
static const char GLYPH_hiriqwidehebrew[]= "hiriqwidehebrew";
static const char GLYPH_tsere[]= "tsere";
static const char GLYPH_tsere12[]= "tsere12";
static const char GLYPH_tsere1e[]= "tsere1e";
static const char GLYPH_tsere2b[]= "tsere2b";
static const char GLYPH_tserehebrew[]= "tserehebrew";
static const char GLYPH_tserenarrowhebrew[]= "tserenarrowhebrew";
static const char GLYPH_tserequarterhebrew[]= "tserequarterhebrew";
static const char GLYPH_tserewidehebrew[]= "tserewidehebrew";
static const char GLYPH_segol[]= "segol";
static const char GLYPH_segol13[]= "segol13";
static const char GLYPH_segol1f[]= "segol1f";
static const char GLYPH_segol2c[]= "segol2c";
static const char GLYPH_segolhebrew[]= "segolhebrew";
static const char GLYPH_segolnarrowhebrew[]= "segolnarrowhebrew";
static const char GLYPH_segolquarterhebrew[]= "segolquarterhebrew";
static const char GLYPH_segolwidehebrew[]= "segolwidehebrew";
static const char GLYPH_qubuts[]= "qubuts";
static const char GLYPH_qubuts18[]= "qubuts18";
static const char GLYPH_qubuts25[]= "qubuts25";
static const char GLYPH_qubuts31[]= "qubuts31";
static const char GLYPH_qubutshebrew[]= "qubutshebrew";
static const char GLYPH_qubutsnarrowhebrew[]= "qubutsnarrowhebrew";
static const char GLYPH_qubutsquarterhebrew[]= "qubutsquarterhebrew";
static const char GLYPH_qubutswidehebrew[]= "qubutswidehebrew";
static const char GLYPH_qamats[]= "qamats";
static const char GLYPH_qamats10[]= "qamats10";
static const char GLYPH_qamats1a[]= "qamats1a";
static const char GLYPH_qamats1c[]= "qamats1c";
static const char GLYPH_qamats27[]= "qamats27";
static const char GLYPH_qamats29[]= "qamats29";
static const char GLYPH_qamats33[]= "qamats33";
static const char GLYPH_qamatsde[]= "qamatsde";
static const char GLYPH_qamatshebrew[]= "qamatshebrew";
static const char GLYPH_qamatsnarrowhebrew[]= "qamatsnarrowhebrew";
static const char GLYPH_qamatsqatanhebrew[]= "qamatsqatanhebrew";
static const char GLYPH_qamatsqatannarrowhebrew[]= "qamatsqatannarrowhebrew";
static const char GLYPH_qamatsqatanquarterhebrew[]= "qamatsqatanquarterhebrew";
static const char GLYPH_qamatsqatanwidehebrew[]= "qamatsqatanwidehebrew";
static const char GLYPH_qamatsquarterhebrew[]= "qamatsquarterhebrew";
static const char GLYPH_qamatswidehebrew[]= "qamatswidehebrew";
static const char GLYPH_patah[]= "patah";
static const char GLYPH_patah11[]= "patah11";
static const char GLYPH_patah1d[]= "patah1d";
static const char GLYPH_patah2a[]= "patah2a";
static const char GLYPH_patahhebrew[]= "patahhebrew";
static const char GLYPH_patahnarrowhebrew[]= "patahnarrowhebrew";
static const char GLYPH_patahquarterhebrew[]= "patahquarterhebrew";
static const char GLYPH_patahwidehebrew[]= "patahwidehebrew";
static const char GLYPH_sheva[]= "sheva";
static const char GLYPH_sheva115[]= "sheva115";
static const char GLYPH_sheva15[]= "sheva15";
static const char GLYPH_sheva22[]= "sheva22";
static const char GLYPH_sheva2e[]= "sheva2e";
static const char GLYPH_shevahebrew[]= "shevahebrew";
static const char GLYPH_shevanarrowhebrew[]= "shevanarrowhebrew";
static const char GLYPH_shevaquarterhebrew[]= "shevaquarterhebrew";
static const char GLYPH_shevawidehebrew[]= "shevawidehebrew";
static const char GLYPH_hatafpatah[]= "hatafpatah";
static const char GLYPH_hatafpatah16[]= "hatafpatah16";
static const char GLYPH_hatafpatah23[]= "hatafpatah23";
static const char GLYPH_hatafpatah2f[]= "hatafpatah2f";
static const char GLYPH_hatafpatahhebrew[]= "hatafpatahhebrew";
static const char GLYPH_hatafpatahnarrowhebrew[]= "hatafpatahnarrowhebrew";
static const char GLYPH_hatafpatahquarterhebrew[]= "hatafpatahquarterhebrew";
static const char GLYPH_hatafpatahwidehebrew[]= "hatafpatahwidehebrew";
static const char GLYPH_hatafsegol[]= "hatafsegol";
static const char GLYPH_hatafsegol17[]= "hatafsegol17";
static const char GLYPH_hatafsegol24[]= "hatafsegol24";
static const char GLYPH_hatafsegol30[]= "hatafsegol30";
static const char GLYPH_hatafsegolhebrew[]= "hatafsegolhebrew";
static const char GLYPH_hatafsegolnarrowhebrew[]= "hatafsegolnarrowhebrew";
static const char GLYPH_hatafsegolquarterhebrew[]= "hatafsegolquarterhebrew";
static const char GLYPH_hatafsegolwidehebrew[]= "hatafsegolwidehebrew";
static const char GLYPH_hatafqamats[]= "hatafqamats";
static const char GLYPH_hatafqamats1b[]= "hatafqamats1b";
static const char GLYPH_hatafqamats28[]= "hatafqamats28";
static const char GLYPH_hatafqamats34[]= "hatafqamats34";
static const char GLYPH_hatafqamatshebrew[]= "hatafqamatshebrew";
static const char GLYPH_hatafqamatsnarrowhebrew[]= "hatafqamatsnarrowhebrew";
static const char GLYPH_hatafqamatsquarterhebrew[]= "hatafqamatsquarterhebrew";
static const char GLYPH_hatafqamatswidehebrew[]= "hatafqamatswidehebrew";
static const char GLYPH_sindothebrew[]= "sindothebrew";
static const char GLYPH_shindothebrew[]= "shindothebrew";
static const char GLYPH_holam[]= "holam";
static const char GLYPH_holam19[]= "holam19";
static const char GLYPH_holam26[]= "holam26";
static const char GLYPH_holam32[]= "holam32";
static const char GLYPH_holamhebrew[]= "holamhebrew";
static const char GLYPH_holamnarrowhebrew[]= "holamnarrowhebrew";
static const char GLYPH_holamquarterhebrew[]= "holamquarterhebrew";
static const char GLYPH_holamwidehebrew[]= "holamwidehebrew";
static const char GLYPH_dagesh[]= "dagesh";
static const char GLYPH_dageshhebrew[]= "dageshhebrew";
static const char GLYPH_siluqhebrew[]= "siluqhebrew";
static const char GLYPH_siluqlefthebrew[]= "siluqlefthebrew";
static const char GLYPH_rafe[]= "rafe";
static const char GLYPH_rafehebrew[]= "rafehebrew";
static const char GLYPH_paseqhebrew[]= "paseqhebrew";
static const char GLYPH_apostrophemod[]= "apostrophemod";
static const char GLYPH_careof[]= "careof";
static const char GLYPH_lsquare[]= "lsquare";
static const char GLYPH_numero[]= "numero";
static const char GLYPH_zerowidthnonjoiner[]= "zerowidthnonjoiner";
static const char GLYPH_asteriskaltonearabic[]= "asteriskaltonearabic";
static const char GLYPH_asteriskarabic[]= "asteriskarabic";
static const char GLYPH_commareversedmod[]= "commareversedmod";
static const char GLYPH_dblarrowleft[]= "dblarrowleft";
static const char GLYPH_arrowleftdbl[]= "arrowleftdbl";
static const char GLYPH_dblarrowright[]= "dblarrowright";
static const char GLYPH_arrowupdownbase[]= "arrowupdownbase";
static const char GLYPH_verticalbar[]= "verticalbar";
static const char GLYPH_cdot[]= "cdot";
static const char GLYPH_whitecircle[]= "whitecircle";
static const char GLYPH_timescircle[]= "timescircle";
static const char GLYPH_pluscircle[]= "pluscircle";
static const char GLYPH_clubsuitblack[]= "clubsuitblack";
static const char GLYPH_colonsign[]= "colonsign";
static const char GLYPH_approximatelyequal[]= "approximatelyequal";
static const char GLYPH_dmacron[]= "dmacron";
static const char GLYPH_dialytikatonos[]= "dialytikatonos";
static const char GLYPH_shadedark[]= "shadedark";
static const char GLYPH_dotbelowcmb[]= "dotbelowcmb";
static const char GLYPH_edot[]= "edot";
static const char GLYPH_thereexists[]= "thereexists";
static const char GLYPH_venus[]= "venus";
static const char GLYPH_blacksquare[]= "blacksquare";
static const char GLYPH_blackrectangle[]= "blackrectangle";
static const char GLYPH_gcedilla[]= "gcedilla";
static const char GLYPH_gdot[]= "gdot";
static const char GLYPH_nabla[]= "nabla";
static const char GLYPH_gravecmb[]= "gravecmb";
static const char GLYPH_heartsuitblack[]= "heartsuitblack";
static const char GLYPH_hookcmb[]= "hookcmb";
static const char GLYPH_integralbottom[]= "integralbottom";
static const char GLYPH_integraltop[]= "integraltop";
static const char GLYPH_bulletinverse[]= "bulletinverse";
static const char GLYPH_whitecircleinverse[]= "whitecircleinverse";
static const char GLYPH_blacksmilingface[]= "blacksmilingface";
static const char GLYPH_kcedilla[]= "kcedilla";
static const char GLYPH_lcedilla[]= "lcedilla";
static const char GLYPH_ldotaccent[]= "ldotaccent";
static const char GLYPH_afii08941[]= "afii08941";
static const char GLYPH_slong[]= "slong";
static const char GLYPH_shadelight[]= "shadelight";
static const char GLYPH_overscore[]= "overscore";
static const char GLYPH_mars[]= "mars";
static const char GLYPH_mu1[]= "mu1";
static const char GLYPH_eighthnotebeamed[]= "eighthnotebeamed";
static const char GLYPH_quoterightn[]= "quoterightn";
static const char GLYPH_ncedilla[]= "ncedilla";
static const char GLYPH_notelementof[]= "notelementof";
static const char GLYPH_odblacute[]= "odblacute";
static const char GLYPH_pisymbolgreek[]= "pisymbolgreek";
static const char GLYPH_whitebullet[]= "whitebullet";
static const char GLYPH_rightangle[]= "rightangle";
static const char GLYPH_ostrokeacute[]= "ostrokeacute";
static const char GLYPH_middot[]= "middot";
static const char GLYPH_phisymbolgreek[]= "phisymbolgreek";
static const char GLYPH_subset[]= "subset";
static const char GLYPH_superset[]= "superset";
static const char GLYPH_quoteleftreversed[]= "quoteleftreversed";
static const char GLYPH_rcedilla[]= "rcedilla";
static const char GLYPH_subsetorequal[]= "subsetorequal";
static const char GLYPH_supersetorequal[]= "supersetorequal";
static const char GLYPH_logicalnotreversed[]= "logicalnotreversed";
static const char GLYPH_shademedium[]= "shademedium";
static const char GLYPH_sigmafinal[]= "sigmafinal";
static const char GLYPH_tildeoperator[]= "tildeoperator";
static const char GLYPH_whitesmilingface[]= "whitesmilingface";
static const char GLYPH_spacehackarabic[]= "spacehackarabic";
static const char GLYPH_spadesuitblack[]= "spadesuitblack";
static const char GLYPH_compass[]= "compass";
static const char GLYPH_tcedilla[]= "tcedilla";
static const char GLYPH_thetasymbolgreek[]= "thetasymbolgreek";
static const char GLYPH_ilde[]= "ilde";
static const char GLYPH_tildecmb[]= "tildecmb";
static const char GLYPH_blackdownpointingtriangle[]= "blackdownpointingtriangle";
static const char GLYPH_blackleftpointingpointer[]= "blackleftpointingpointer";
static const char GLYPH_blackrightpointingpointer[]= "blackrightpointingpointer";
static const char GLYPH_blackuppointingtriangle[]= "blackuppointingtriangle";
static const char GLYPH_twodotleader[]= "twodotleader";
static const char GLYPH_udblacute[]= "udblacute";
static const char GLYPH_dbllowline[]= "dbllowline";
static const char GLYPH_forall[]= "forall";
static const char GLYPH_zdot[]= "zdot";

/* EXTRA */

static const char GLYPH_apostrophe[]= "apostrophe";
static const char GLYPH_fullstop[]= "fullstop";
static const char GLYPH_lowline[]= "lowline";
static const char GLYPH_Lstroke[]= "Lstroke";
static const char GLYPH_diaeresis[]= "diaeresis";
static const char GLYPH_Zabovedot[]= "Zabovedot";
static const char GLYPH_lstroke[]= "lstroke";
static const char GLYPH_doubleacute[]= "doubleacute";
static const char GLYPH_zabovedot[]= "zabovedot";
static const char GLYPH_Adiaeresis[]= "Adiaeresis";
static const char GLYPH_Ediaeresis[]= "Ediaeresis";
static const char GLYPH_Dstroke[]= "Dstroke";
static const char GLYPH_Odoubleacute[]= "Odoubleacute";
static const char GLYPH_Odiaeresis[]= "Odiaeresis";
static const char GLYPH_Udoubleacute[]= "Udoubleacute";
static const char GLYPH_Udiaeresis[]= "Udiaeresis";
static const char GLYPH_ssharp[]= "ssharp";
static const char GLYPH_adiaeresis[]= "adiaeresis";
static const char GLYPH_ediaeresis[]= "ediaeresis";
static const char GLYPH_dstroke[]= "dstroke";
static const char GLYPH_odoubleacute[]= "odoubleacute";
static const char GLYPH_odiaeresis[]= "odiaeresis";
static const char GLYPH_division[]= "division";
static const char GLYPH_udoubleacute[]= "udoubleacute";
static const char GLYPH_udiaeresis[]= "udiaeresis";
static const char GLYPH_abovedot[]= "abovedot";
static const char GLYPH_Scedilla1[]= "Scedilla1";
static const char GLYPH_scedilla1[]= "scedilla1";
static const char GLYPH_Tcedilla1[]= "Tcedilla1";
static const char GLYPH_tcedilla1[]= "tcedilla1";
static const char GLYPH_Dmacron[]= "Dmacron";
static const char GLYPH_Ohungar[]= "Ohungar";
static const char GLYPH_Uhungar[]= "Uhungar";
static const char GLYPH_ohungar[]= "ohungar";
static const char GLYPH_uhungar[]= "uhungar";
static const char GLYPH_sfthyphen[]= "sfthyphen";
static const char GLYPH_commaaccent[]= "commaaccent";
static const char GLYPH_nonbreakingspace[]= "nonbreakingspace";

static const char GLYPH__notdef[]= ".notdef";
static const char GLYPH__null[]= ".null";

/*  AFII's for greek */
static const char GLYPH_afii9936[]= "afii9936";
static const char GLYPH_afii9937[]= "afii9937";
static const char GLYPH_afii9938[]= "afii9938";
static const char GLYPH_afii9939[]= "afii9939";
static const char GLYPH_afii9940[]= "afii9940";
static const char GLYPH_afii9941[]= "afii9941";
static const char GLYPH_afii9942[]= "afii9942";
static const char GLYPH_afii9977[]= "afii9977";
static const char GLYPH_afii9793[]= "afii9793";
static const char GLYPH_afii9794[]= "afii9794";
static const char GLYPH_afii9796[]= "afii9796";
static const char GLYPH_afii9797[]= "afii9797";
static const char GLYPH_afii9798[]= "afii9798";
static const char GLYPH_afii9801[]= "afii9801";
static const char GLYPH_afii9802[]= "afii9802";
static const char GLYPH_afii9803[]= "afii9803";
static const char GLYPH_afii9804[]= "afii9804";
static const char GLYPH_afii9805[]= "afii9805";
static const char GLYPH_afii9806[]= "afii9806";
static const char GLYPH_afii9807[]= "afii9807";
static const char GLYPH_afii9808[]= "afii9808";
static const char GLYPH_afii9809[]= "afii9809";
static const char GLYPH_afii9810[]= "afii9810";
static const char GLYPH_afii9811[]= "afii9811";
static const char GLYPH_afii9813[]= "afii9813";
static const char GLYPH_afii9814[]= "afii9814";
static const char GLYPH_afii9816[]= "afii9816";
static const char GLYPH_afii9817[]= "afii9817";
static const char GLYPH_afii9818[]= "afii9818";
static const char GLYPH_afii9819[]= "afii9819";
static const char GLYPH_afii9820[]= "afii9820";
static const char GLYPH_afii9821[]= "afii9821";
static const char GLYPH_afii9943[]= "afii9943";
static const char GLYPH_afii9944[]= "afii9944";
static const char GLYPH_afii9968[]= "afii9968";
static const char GLYPH_afii9969[]= "afii9969";
static const char GLYPH_afii9970[]= "afii9970";
static const char GLYPH_afii9971[]= "afii9971";
static const char GLYPH_afii9978[]= "afii9978";
static const char GLYPH_afii9825[]= "afii9825";
static const char GLYPH_afii9826[]= "afii9826";
static const char GLYPH_afii9828[]= "afii9828";
static const char GLYPH_afii9829[]= "afii9829";
static const char GLYPH_afii9830[]= "afii9830";
static const char GLYPH_afii9833[]= "afii9833";
static const char GLYPH_afii9834[]= "afii9834";
static const char GLYPH_afii9835[]= "afii9835";
static const char GLYPH_afii9836[]= "afii9836";
static const char GLYPH_afii9837[]= "afii9837";
static const char GLYPH_afii9838[]= "afii9838";
static const char GLYPH_afii9839[]= "afii9839";
static const char GLYPH_afii9840[]= "afii9840";
static const char GLYPH_afii9841[]= "afii9841";
static const char GLYPH_afii9842[]= "afii9842";
static const char GLYPH_afii9843[]= "afii9843";
static const char GLYPH_afii9845[]= "afii9845";
static const char GLYPH_afii9847[]= "afii9847";
static const char GLYPH_afii9846[]= "afii9846";
static const char GLYPH_afii9848[]= "afii9848";
static const char GLYPH_afii9849[]= "afii9849";
static const char GLYPH_afii9850[]= "afii9850";
static const char GLYPH_afii9851[]= "afii9851";
static const char GLYPH_afii9852[]= "afii9852";
static const char GLYPH_afii9853[]= "afii9853";
static const char GLYPH_afii9975[]= "afii9975";
static const char GLYPH_afii9976[]= "afii9976";
static const char GLYPH_afii9972[]= "afii9972";
static const char GLYPH_afii9973[]= "afii9973";
static const char GLYPH_afii9974[]= "afii9974";

static const char GLYPH_dieresisacute[]= "dieresisacute";

static const char GLYPH_uni037e[]= "uni037e";

/************************************************************************/
/*									*/
/*  Glyph names that can be surrogates for the official names.		*/
/*  It is irrelevant whether it is an acceptable substitution or an	*/
/*  alternative name for the same glyph.				*/
/*									*/
/*  Given the low number of alternatives per official glyph name, it	*/
/*  is not worth the effort to optimize the data structure. The simple	*/
/*  binary search will make less than one additional string comparison	*/
/*  on the average and the loop over the alternatives for one official	*/
/*  name makes only a few extra comparisons. Those could even be	*/
/*  replaced with pointer comparisons to get the same speed.		*/
/*									*/
/************************************************************************/

# if 0
/*

:
: script to sort:
:

sed	-e 's/^[ 	]*{[ 	]*GLYPH_//'	\
	-e 's/,[ 	]*GLYPH_/ /'		\
	-e 's/[ 	]}.*$//'		\
	alts					|
	sort +0 +1 |
	while read official surrogate
	do
	    printf '	{ GLYPH_%s, GLYPH_%s },\n' $official $surrogate
	done

*/
# endif

typedef struct AlternateGlyphName
    {
    const char *	agnOfficialName;
    const char *	agnSurrogateName;
    } AlternateGlyphName;

const AlternateGlyphName PS_AlternateNames[]=
    {
	{ GLYPH_Adieresis, GLYPH_Adiaeresis },
	{ GLYPH_Alpha, GLYPH_afii9793 },
	{ GLYPH_Alphatonos, GLYPH_afii9936 },
	{ GLYPH_Beta, GLYPH_afii9794 },
	{ GLYPH_Cdotaccent, GLYPH_Cdot },
	{ GLYPH_Chi, GLYPH_afii9819 },
	{ GLYPH_Dcroat, GLYPH_Dmacron },
	{ GLYPH_Dcroat, GLYPH_Dslash },
	{ GLYPH_Dcroat, GLYPH_Dstroke },
	{ GLYPH_Dcroat, GLYPH_Eth },
	{ GLYPH_Delta, GLYPH_afii9797 },
	{ GLYPH_Delta, GLYPH_increment },
	{ GLYPH_Edieresis, GLYPH_Ediaeresis },
	{ GLYPH_Edotaccent, GLYPH_Edot },
	{ GLYPH_Epsilon, GLYPH_afii9798 },
	{ GLYPH_Epsilontonos, GLYPH_afii9937 },
	{ GLYPH_Eta, GLYPH_afii9802 },
	{ GLYPH_Etatonos, GLYPH_afii9938 },
	{ GLYPH_Eth, GLYPH_Dcroat },
	{ GLYPH_Euro, GLYPH_euro },
	{ GLYPH_Euro, GLYPH_e },
	{ GLYPH_Gamma, GLYPH_afii9796 },
	{ GLYPH_Gcommaaccent, GLYPH_Gcedilla },
	{ GLYPH_Gdotaccent, GLYPH_Gdot },
	{ GLYPH_H18533, GLYPH_blackcircle },
	{ GLYPH_H18543, GLYPH_blacksmallsquare },
	{ GLYPH_H18551, GLYPH_whitesmallsquare },
	{ GLYPH_H22073, GLYPH_whitesquare },
	{ GLYPH_Idotaccent, GLYPH_Idot },
	{ GLYPH_Iota, GLYPH_afii9804 },
	{ GLYPH_Iotadieresis, GLYPH_afii9943 },
	{ GLYPH_Iotatonos, GLYPH_afii9939 },
	{ GLYPH_Kappa, GLYPH_afii9805 },
	{ GLYPH_Kcommaaccent, GLYPH_Kcedilla },
	{ GLYPH_Lambda, GLYPH_afii9806 },
	{ GLYPH_Lcommaaccent, GLYPH_Lcedilla },
	{ GLYPH_Ldot, GLYPH_Ldotaccent },
	{ GLYPH_Lslash, GLYPH_Lstroke },
	{ GLYPH_Mu, GLYPH_afii9807 },
	{ GLYPH_Ncommaaccent, GLYPH_Ncedilla },
	{ GLYPH_Nu, GLYPH_afii9808 },
	{ GLYPH_Odieresis, GLYPH_Odiaeresis },
	{ GLYPH_Ohungarumlaut, GLYPH_Odblacute },
	{ GLYPH_Ohungarumlaut, GLYPH_Odoubleacute },
	{ GLYPH_Ohungarumlaut, GLYPH_Ohungar },
	{ GLYPH_Omega, GLYPH_Ohm },
	{ GLYPH_Omega, GLYPH_afii9821 },
	{ GLYPH_Omegatonos, GLYPH_afii9942 },
	{ GLYPH_Omicron, GLYPH_afii9810 },
	{ GLYPH_Omicrontonos, GLYPH_afii9940 },
	{ GLYPH_Oslashacute, GLYPH_Ostrokeacute },
	{ GLYPH_Phi, GLYPH_afii9818 },
	{ GLYPH_Pi, GLYPH_afii9811 },
	{ GLYPH_Psi, GLYPH_afii9820 },
	{ GLYPH_Rcommaaccent, GLYPH_Rcedilla },
	{ GLYPH_Rho, GLYPH_afii9813 },
	{ GLYPH_Scedilla, GLYPH_Scedilla1 },
	{ GLYPH_Scedilla, GLYPH_Scommaaccent },
	{ GLYPH_Sigma, GLYPH_afii9814 },
	{ GLYPH_Tau, GLYPH_afii9816 },
	{ GLYPH_Tcommaaccent, GLYPH_Tcedilla },
	{ GLYPH_Tcommaaccent, GLYPH_Tcedilla1 },
	{ GLYPH_Theta, GLYPH_afii9803 },
	{ GLYPH_Udieresis, GLYPH_Udiaeresis },
	{ GLYPH_Uhungarumlaut, GLYPH_Udblacute },
	{ GLYPH_Uhungarumlaut, GLYPH_Udoubleacute },
	{ GLYPH_Uhungarumlaut, GLYPH_Uhungar },
	{ GLYPH_Upsilon, GLYPH_afii9817 },
	{ GLYPH_Upsilon1, GLYPH_Upsilonhooksymbol },
	{ GLYPH_Upsilondieresis, GLYPH_afii9944 },
	{ GLYPH_Upsilontonos, GLYPH_afii9941 },
	{ GLYPH_Xi, GLYPH_afii9809 },
	{ GLYPH_Zdotaccent, GLYPH_Zabovedot },
	{ GLYPH_Zdotaccent, GLYPH_Zdot },
	{ GLYPH_Zeta, GLYPH_afii9801 },
	{ GLYPH_acutecomb, GLYPH_acutecmb },
	{ GLYPH_adieresis, GLYPH_adiaeresis },
	{ GLYPH_afii00208, GLYPH_endash },
	{ GLYPH_afii00208, GLYPH_horizontalbar },
	{ GLYPH_afii10017, GLYPH_Acyrillic },
	{ GLYPH_afii10018, GLYPH_Becyrillic },
	{ GLYPH_afii10019, GLYPH_Vecyrillic },
	{ GLYPH_afii10020, GLYPH_Gecyrillic },
	{ GLYPH_afii10021, GLYPH_Decyrillic },
	{ GLYPH_afii10022, GLYPH_Iecyrillic },
	{ GLYPH_afii10023, GLYPH_Iocyrillic },
	{ GLYPH_afii10024, GLYPH_Zhecyrillic },
	{ GLYPH_afii10025, GLYPH_Zecyrillic },
	{ GLYPH_afii10026, GLYPH_Iicyrillic },
	{ GLYPH_afii10027, GLYPH_Iishortcyrillic },
	{ GLYPH_afii10028, GLYPH_Kacyrillic },
	{ GLYPH_afii10029, GLYPH_Elcyrillic },
	{ GLYPH_afii10030, GLYPH_Emcyrillic },
	{ GLYPH_afii10031, GLYPH_Encyrillic },
	{ GLYPH_afii10032, GLYPH_Ocyrillic },
	{ GLYPH_afii10033, GLYPH_Pecyrillic },
	{ GLYPH_afii10034, GLYPH_Ercyrillic },
	{ GLYPH_afii10035, GLYPH_Escyrillic },
	{ GLYPH_afii10036, GLYPH_Tecyrillic },
	{ GLYPH_afii10037, GLYPH_Ucyrillic },
	{ GLYPH_afii10038, GLYPH_Efcyrillic },
	{ GLYPH_afii10039, GLYPH_Khacyrillic },
	{ GLYPH_afii10040, GLYPH_Tsecyrillic },
	{ GLYPH_afii10041, GLYPH_Checyrillic },
	{ GLYPH_afii10042, GLYPH_Shacyrillic },
	{ GLYPH_afii10043, GLYPH_Shchacyrillic },
	{ GLYPH_afii10044, GLYPH_Hardsigncyrillic },
	{ GLYPH_afii10045, GLYPH_Yericyrillic },
	{ GLYPH_afii10046, GLYPH_Softsigncyrillic },
	{ GLYPH_afii10047, GLYPH_Ereversedcyrillic },
	{ GLYPH_afii10048, GLYPH_IUcyrillic },
	{ GLYPH_afii10049, GLYPH_IAcyrillic },
	{ GLYPH_afii10050, GLYPH_Gheupturncyrillic },
	{ GLYPH_afii10051, GLYPH_Djecyrillic },
	{ GLYPH_afii10052, GLYPH_Gjecyrillic },
	{ GLYPH_afii10053, GLYPH_Ecyrillic },
	{ GLYPH_afii10054, GLYPH_Dzecyrillic },
	{ GLYPH_afii10055, GLYPH_Icyrillic },
	{ GLYPH_afii10056, GLYPH_Yicyrillic },
	{ GLYPH_afii10057, GLYPH_Jecyrillic },
	{ GLYPH_afii10058, GLYPH_Ljecyrillic },
	{ GLYPH_afii10059, GLYPH_Njecyrillic },
	{ GLYPH_afii10060, GLYPH_Tshecyrillic },
	{ GLYPH_afii10061, GLYPH_Kjecyrillic },
	{ GLYPH_afii10062, GLYPH_Ushortcyrillic },
	{ GLYPH_afii10065, GLYPH_acyrillic },
	{ GLYPH_afii10066, GLYPH_becyrillic },
	{ GLYPH_afii10067, GLYPH_vecyrillic },
	{ GLYPH_afii10068, GLYPH_gecyrillic },
	{ GLYPH_afii10069, GLYPH_decyrillic },
	{ GLYPH_afii10070, GLYPH_iecyrillic },
	{ GLYPH_afii10071, GLYPH_iocyrillic },
	{ GLYPH_afii10072, GLYPH_zhecyrillic },
	{ GLYPH_afii10073, GLYPH_zecyrillic },
	{ GLYPH_afii10074, GLYPH_iicyrillic },
	{ GLYPH_afii10075, GLYPH_iishortcyrillic },
	{ GLYPH_afii10076, GLYPH_kacyrillic },
	{ GLYPH_afii10077, GLYPH_elcyrillic },
	{ GLYPH_afii10078, GLYPH_emcyrillic },
	{ GLYPH_afii10079, GLYPH_encyrillic },
	{ GLYPH_afii10080, GLYPH_ocyrillic },
	{ GLYPH_afii10081, GLYPH_pecyrillic },
	{ GLYPH_afii10082, GLYPH_ercyrillic },
	{ GLYPH_afii10083, GLYPH_escyrillic },
	{ GLYPH_afii10084, GLYPH_tecyrillic },
	{ GLYPH_afii10085, GLYPH_ucyrillic },
	{ GLYPH_afii10086, GLYPH_efcyrillic },
	{ GLYPH_afii10087, GLYPH_khacyrillic },
	{ GLYPH_afii10088, GLYPH_tsecyrillic },
	{ GLYPH_afii10089, GLYPH_checyrillic },
	{ GLYPH_afii10090, GLYPH_shacyrillic },
	{ GLYPH_afii10091, GLYPH_shchacyrillic },
	{ GLYPH_afii10092, GLYPH_hardsigncyrillic },
	{ GLYPH_afii10093, GLYPH_yericyrillic },
	{ GLYPH_afii10094, GLYPH_softsigncyrillic },
	{ GLYPH_afii10095, GLYPH_ereversedcyrillic },
	{ GLYPH_afii10096, GLYPH_iucyrillic },
	{ GLYPH_afii10097, GLYPH_iacyrillic },
	{ GLYPH_afii10098, GLYPH_gheupturncyrillic },
	{ GLYPH_afii10099, GLYPH_djecyrillic },
	{ GLYPH_afii10100, GLYPH_gjecyrillic },
	{ GLYPH_afii10101, GLYPH_ecyrillic },
	{ GLYPH_afii10102, GLYPH_dzecyrillic },
	{ GLYPH_afii10103, GLYPH_icyrillic },
	{ GLYPH_afii10104, GLYPH_yicyrillic },
	{ GLYPH_afii10105, GLYPH_jecyrillic },
	{ GLYPH_afii10106, GLYPH_ljecyrillic },
	{ GLYPH_afii10107, GLYPH_njecyrillic },
	{ GLYPH_afii10108, GLYPH_tshecyrillic },
	{ GLYPH_afii10109, GLYPH_kjecyrillic },
	{ GLYPH_afii10110, GLYPH_ushortcyrillic },
	{ GLYPH_afii10145, GLYPH_Dzhecyrillic },
	{ GLYPH_afii10146, GLYPH_Yatcyrillic },
	{ GLYPH_afii10147, GLYPH_Fitacyrillic },
	{ GLYPH_afii10148, GLYPH_Izhitsacyrillic },
	{ GLYPH_afii10193, GLYPH_dzhecyrillic },
	{ GLYPH_afii10194, GLYPH_yatcyrillic },
	{ GLYPH_afii10195, GLYPH_fitacyrillic },
	{ GLYPH_afii10196, GLYPH_izhitsacyrillic },
	{ GLYPH_afii10846, GLYPH_schwacyrillic },
	{ GLYPH_afii57381, GLYPH_percentarabic },
	{ GLYPH_afii57388, GLYPH_commaarabic },
	{ GLYPH_afii57392, GLYPH_zeroarabic },
	{ GLYPH_afii57392, GLYPH_zerohackarabic },
	{ GLYPH_afii57393, GLYPH_onearabic },
	{ GLYPH_afii57393, GLYPH_onehackarabic },
	{ GLYPH_afii57394, GLYPH_twoarabic },
	{ GLYPH_afii57394, GLYPH_twohackarabic },
	{ GLYPH_afii57395, GLYPH_threearabic },
	{ GLYPH_afii57395, GLYPH_threehackarabic },
	{ GLYPH_afii57396, GLYPH_fourarabic },
	{ GLYPH_afii57396, GLYPH_fourhackarabic },
	{ GLYPH_afii57397, GLYPH_fivearabic },
	{ GLYPH_afii57397, GLYPH_fivehackarabic },
	{ GLYPH_afii57398, GLYPH_sixarabic },
	{ GLYPH_afii57398, GLYPH_sixhackarabic },
	{ GLYPH_afii57399, GLYPH_sevenarabic },
	{ GLYPH_afii57399, GLYPH_sevenhackarabic },
	{ GLYPH_afii57400, GLYPH_eightarabic },
	{ GLYPH_afii57400, GLYPH_eighthackarabic },
	{ GLYPH_afii57401, GLYPH_ninearabic },
	{ GLYPH_afii57401, GLYPH_ninehackarabic },
	{ GLYPH_afii57403, GLYPH_semicolonarabic },
	{ GLYPH_afii57407, GLYPH_questionarabic },
	{ GLYPH_afii57409, GLYPH_hamzaarabic },
	{ GLYPH_afii57409, GLYPH_hamzalowarabic },
	{ GLYPH_afii57410, GLYPH_alefmaddaabovearabic },
	{ GLYPH_afii57411, GLYPH_alefhamzaabovearabic },
	{ GLYPH_afii57412, GLYPH_wawhamzaabovearabic },
	{ GLYPH_afii57413, GLYPH_alefhamzabelowarabic },
	{ GLYPH_afii57414, GLYPH_yehhamzaabovearabic },
	{ GLYPH_afii57415, GLYPH_alefarabic },
	{ GLYPH_afii57416, GLYPH_beharabic },
	{ GLYPH_afii57417, GLYPH_tehmarbutaarabic },
	{ GLYPH_afii57418, GLYPH_teharabic },
	{ GLYPH_afii57419, GLYPH_theharabic },
	{ GLYPH_afii57420, GLYPH_jeemarabic },
	{ GLYPH_afii57421, GLYPH_haharabic },
	{ GLYPH_afii57422, GLYPH_khaharabic },
	{ GLYPH_afii57423, GLYPH_dalarabic },
	{ GLYPH_afii57424, GLYPH_thalarabic },
	{ GLYPH_afii57425, GLYPH_reharabic },
	{ GLYPH_afii57426, GLYPH_zainarabic },
	{ GLYPH_afii57427, GLYPH_seenarabic },
	{ GLYPH_afii57428, GLYPH_sheenarabic },
	{ GLYPH_afii57429, GLYPH_sadarabic },
	{ GLYPH_afii57430, GLYPH_dadarabic },
	{ GLYPH_afii57431, GLYPH_taharabic },
	{ GLYPH_afii57432, GLYPH_zaharabic },
	{ GLYPH_afii57433, GLYPH_ainarabic },
	{ GLYPH_afii57434, GLYPH_ghainarabic },
	{ GLYPH_afii57440, GLYPH_kashidaautoarabic },
	{ GLYPH_afii57440, GLYPH_kashidaautonosidebearingarabic },
	{ GLYPH_afii57440, GLYPH_tatweelarabic },
	{ GLYPH_afii57441, GLYPH_feharabic },
	{ GLYPH_afii57442, GLYPH_qafarabic },
	{ GLYPH_afii57443, GLYPH_kafarabic },
	{ GLYPH_afii57444, GLYPH_lamarabic },
	{ GLYPH_afii57445, GLYPH_meemarabic },
	{ GLYPH_afii57446, GLYPH_noonarabic },
	{ GLYPH_afii57448, GLYPH_wawarabic },
	{ GLYPH_afii57449, GLYPH_alefmaksuraarabic },
	{ GLYPH_afii57450, GLYPH_yeharabic },
	{ GLYPH_afii57451, GLYPH_fathatanarabic },
	{ GLYPH_afii57452, GLYPH_dammatanaltonearabic },
	{ GLYPH_afii57452, GLYPH_dammatanarabic },
	{ GLYPH_afii57453, GLYPH_kasratanarabic },
	{ GLYPH_afii57454, GLYPH_fathaarabic },
	{ GLYPH_afii57454, GLYPH_fathalowarabic },
	{ GLYPH_afii57455, GLYPH_dammaarabic },
	{ GLYPH_afii57455, GLYPH_dammalowarabic },
	{ GLYPH_afii57456, GLYPH_kasraarabic },
	{ GLYPH_afii57457, GLYPH_shaddaarabic },
	{ GLYPH_afii57458, GLYPH_sukunarabic },
	{ GLYPH_afii57470, GLYPH_heharabic },
	{ GLYPH_afii57505, GLYPH_veharabic },
	{ GLYPH_afii57506, GLYPH_peharabic },
	{ GLYPH_afii57507, GLYPH_tcheharabic },
	{ GLYPH_afii57508, GLYPH_jeharabic },
	{ GLYPH_afii57509, GLYPH_gafarabic },
	{ GLYPH_afii57511, GLYPH_tteharabic },
	{ GLYPH_afii57512, GLYPH_ddalarabic },
	{ GLYPH_afii57513, GLYPH_rreharabic },
	{ GLYPH_afii57514, GLYPH_noonghunnaarabic },
	{ GLYPH_afii57519, GLYPH_yehbarreearabic },
	{ GLYPH_afii57636, GLYPH_newsheqelsign },
	{ GLYPH_afii57636, GLYPH_sheqel },
	{ GLYPH_afii57636, GLYPH_sheqelhebrew },
	{ GLYPH_afii57645, GLYPH_maqafhebrew },
	{ GLYPH_afii57658, GLYPH_sofpasuqhebrew },
	{ GLYPH_afii57664, GLYPH_alef },
	{ GLYPH_afii57664, GLYPH_alefhebrew },
	{ GLYPH_afii57665, GLYPH_bet },
	{ GLYPH_afii57665, GLYPH_bethebrew },
	{ GLYPH_afii57666, GLYPH_gimel },
	{ GLYPH_afii57666, GLYPH_gimelhebrew },
	{ GLYPH_afii57667, GLYPH_dalet },
	{ GLYPH_afii57667, GLYPH_dalethebrew },
	{ GLYPH_afii57668, GLYPH_he },
	{ GLYPH_afii57668, GLYPH_hehebrew },
	{ GLYPH_afii57669, GLYPH_vav },
	{ GLYPH_afii57669, GLYPH_vavhebrew },
	{ GLYPH_afii57670, GLYPH_zayin },
	{ GLYPH_afii57670, GLYPH_zayinhebrew },
	{ GLYPH_afii57671, GLYPH_het },
	{ GLYPH_afii57671, GLYPH_hethebrew },
	{ GLYPH_afii57672, GLYPH_tet },
	{ GLYPH_afii57672, GLYPH_tethebrew },
	{ GLYPH_afii57673, GLYPH_yod },
	{ GLYPH_afii57673, GLYPH_yodhebrew },
	{ GLYPH_afii57674, GLYPH_finalkaf },
	{ GLYPH_afii57674, GLYPH_finalkafhebrew },
	{ GLYPH_afii57675, GLYPH_kaf },
	{ GLYPH_afii57675, GLYPH_kafhebrew },
	{ GLYPH_afii57676, GLYPH_lamed },
	{ GLYPH_afii57676, GLYPH_lamedhebrew },
	{ GLYPH_afii57677, GLYPH_finalmem },
	{ GLYPH_afii57677, GLYPH_finalmemhebrew },
	{ GLYPH_afii57678, GLYPH_mem },
	{ GLYPH_afii57678, GLYPH_memhebrew },
	{ GLYPH_afii57679, GLYPH_finalnun },
	{ GLYPH_afii57679, GLYPH_finalnunhebrew },
	{ GLYPH_afii57680, GLYPH_nun },
	{ GLYPH_afii57680, GLYPH_nunhebrew },
	{ GLYPH_afii57681, GLYPH_samekh },
	{ GLYPH_afii57681, GLYPH_samekhhebrew },
	{ GLYPH_afii57682, GLYPH_ayin },
	{ GLYPH_afii57682, GLYPH_ayinhebrew },
	{ GLYPH_afii57683, GLYPH_finalpe },
	{ GLYPH_afii57683, GLYPH_finalpehebrew },
	{ GLYPH_afii57684, GLYPH_pe },
	{ GLYPH_afii57684, GLYPH_pehebrew },
	{ GLYPH_afii57685, GLYPH_finaltsadi },
	{ GLYPH_afii57685, GLYPH_finaltsadihebrew },
	{ GLYPH_afii57686, GLYPH_tsadi },
	{ GLYPH_afii57686, GLYPH_tsadihebrew },
	{ GLYPH_afii57687, GLYPH_qof },
	{ GLYPH_afii57687, GLYPH_qofhebrew },
	{ GLYPH_afii57688, GLYPH_resh },
	{ GLYPH_afii57688, GLYPH_reshhebrew },
	{ GLYPH_afii57689, GLYPH_shin },
	{ GLYPH_afii57689, GLYPH_shinhebrew },
	{ GLYPH_afii57690, GLYPH_tav },
	{ GLYPH_afii57690, GLYPH_tavhebrew },
	{ GLYPH_afii57716, GLYPH_vavvavhebrew },
	{ GLYPH_afii57717, GLYPH_vavyodhebrew },
	{ GLYPH_afii57718, GLYPH_yodyodhebrew },
	{ GLYPH_afii57793, GLYPH_hiriq },
	{ GLYPH_afii57793, GLYPH_hiriq14 },
	{ GLYPH_afii57793, GLYPH_hiriq21 },
	{ GLYPH_afii57793, GLYPH_hiriq2d },
	{ GLYPH_afii57793, GLYPH_hiriqhebrew },
	{ GLYPH_afii57793, GLYPH_hiriqnarrowhebrew },
	{ GLYPH_afii57793, GLYPH_hiriqquarterhebrew },
	{ GLYPH_afii57793, GLYPH_hiriqwidehebrew },
	{ GLYPH_afii57794, GLYPH_tsere },
	{ GLYPH_afii57794, GLYPH_tsere12 },
	{ GLYPH_afii57794, GLYPH_tsere1e },
	{ GLYPH_afii57794, GLYPH_tsere2b },
	{ GLYPH_afii57794, GLYPH_tserehebrew },
	{ GLYPH_afii57794, GLYPH_tserenarrowhebrew },
	{ GLYPH_afii57794, GLYPH_tserequarterhebrew },
	{ GLYPH_afii57794, GLYPH_tserewidehebrew },
	{ GLYPH_afii57795, GLYPH_segol },
	{ GLYPH_afii57795, GLYPH_segol13 },
	{ GLYPH_afii57795, GLYPH_segol1f },
	{ GLYPH_afii57795, GLYPH_segol2c },
	{ GLYPH_afii57795, GLYPH_segolhebrew },
	{ GLYPH_afii57795, GLYPH_segolnarrowhebrew },
	{ GLYPH_afii57795, GLYPH_segolquarterhebrew },
	{ GLYPH_afii57795, GLYPH_segolwidehebrew },
	{ GLYPH_afii57796, GLYPH_qubuts },
	{ GLYPH_afii57796, GLYPH_qubuts18 },
	{ GLYPH_afii57796, GLYPH_qubuts25 },
	{ GLYPH_afii57796, GLYPH_qubuts31 },
	{ GLYPH_afii57796, GLYPH_qubutshebrew },
	{ GLYPH_afii57796, GLYPH_qubutsnarrowhebrew },
	{ GLYPH_afii57796, GLYPH_qubutsquarterhebrew },
	{ GLYPH_afii57796, GLYPH_qubutswidehebrew },
	{ GLYPH_afii57797, GLYPH_qamats },
	{ GLYPH_afii57797, GLYPH_qamats10 },
	{ GLYPH_afii57797, GLYPH_qamats1a },
	{ GLYPH_afii57797, GLYPH_qamats1c },
	{ GLYPH_afii57797, GLYPH_qamats27 },
	{ GLYPH_afii57797, GLYPH_qamats29 },
	{ GLYPH_afii57797, GLYPH_qamats33 },
	{ GLYPH_afii57797, GLYPH_qamatsde },
	{ GLYPH_afii57797, GLYPH_qamatshebrew },
	{ GLYPH_afii57797, GLYPH_qamatsnarrowhebrew },
	{ GLYPH_afii57797, GLYPH_qamatsqatanhebrew },
	{ GLYPH_afii57797, GLYPH_qamatsqatannarrowhebrew },
	{ GLYPH_afii57797, GLYPH_qamatsqatanquarterhebrew },
	{ GLYPH_afii57797, GLYPH_qamatsqatanwidehebrew },
	{ GLYPH_afii57797, GLYPH_qamatsquarterhebrew },
	{ GLYPH_afii57797, GLYPH_qamatswidehebrew },
	{ GLYPH_afii57798, GLYPH_patah },
	{ GLYPH_afii57798, GLYPH_patah11 },
	{ GLYPH_afii57798, GLYPH_patah1d },
	{ GLYPH_afii57798, GLYPH_patah2a },
	{ GLYPH_afii57798, GLYPH_patahhebrew },
	{ GLYPH_afii57798, GLYPH_patahnarrowhebrew },
	{ GLYPH_afii57798, GLYPH_patahquarterhebrew },
	{ GLYPH_afii57798, GLYPH_patahwidehebrew },
	{ GLYPH_afii57799, GLYPH_sheva },
	{ GLYPH_afii57799, GLYPH_sheva115 },
	{ GLYPH_afii57799, GLYPH_sheva15 },
	{ GLYPH_afii57799, GLYPH_sheva22 },
	{ GLYPH_afii57799, GLYPH_sheva2e },
	{ GLYPH_afii57799, GLYPH_shevahebrew },
	{ GLYPH_afii57799, GLYPH_shevanarrowhebrew },
	{ GLYPH_afii57799, GLYPH_shevaquarterhebrew },
	{ GLYPH_afii57799, GLYPH_shevawidehebrew },
	{ GLYPH_afii57800, GLYPH_hatafpatah },
	{ GLYPH_afii57800, GLYPH_hatafpatah16 },
	{ GLYPH_afii57800, GLYPH_hatafpatah23 },
	{ GLYPH_afii57800, GLYPH_hatafpatah2f },
	{ GLYPH_afii57800, GLYPH_hatafpatahhebrew },
	{ GLYPH_afii57800, GLYPH_hatafpatahnarrowhebrew },
	{ GLYPH_afii57800, GLYPH_hatafpatahquarterhebrew },
	{ GLYPH_afii57800, GLYPH_hatafpatahwidehebrew },
	{ GLYPH_afii57801, GLYPH_hatafsegol },
	{ GLYPH_afii57801, GLYPH_hatafsegol17 },
	{ GLYPH_afii57801, GLYPH_hatafsegol24 },
	{ GLYPH_afii57801, GLYPH_hatafsegol30 },
	{ GLYPH_afii57801, GLYPH_hatafsegolhebrew },
	{ GLYPH_afii57801, GLYPH_hatafsegolnarrowhebrew },
	{ GLYPH_afii57801, GLYPH_hatafsegolquarterhebrew },
	{ GLYPH_afii57801, GLYPH_hatafsegolwidehebrew },
	{ GLYPH_afii57802, GLYPH_hatafqamats },
	{ GLYPH_afii57802, GLYPH_hatafqamats1b },
	{ GLYPH_afii57802, GLYPH_hatafqamats28 },
	{ GLYPH_afii57802, GLYPH_hatafqamats34 },
	{ GLYPH_afii57802, GLYPH_hatafqamatshebrew },
	{ GLYPH_afii57802, GLYPH_hatafqamatsnarrowhebrew },
	{ GLYPH_afii57802, GLYPH_hatafqamatsquarterhebrew },
	{ GLYPH_afii57802, GLYPH_hatafqamatswidehebrew },
	{ GLYPH_afii57803, GLYPH_sindothebrew },
	{ GLYPH_afii57804, GLYPH_shindothebrew },
	{ GLYPH_afii57806, GLYPH_holam },
	{ GLYPH_afii57806, GLYPH_holam19 },
	{ GLYPH_afii57806, GLYPH_holam26 },
	{ GLYPH_afii57806, GLYPH_holam32 },
	{ GLYPH_afii57806, GLYPH_holamhebrew },
	{ GLYPH_afii57806, GLYPH_holamnarrowhebrew },
	{ GLYPH_afii57806, GLYPH_holamquarterhebrew },
	{ GLYPH_afii57806, GLYPH_holamwidehebrew },
	{ GLYPH_afii57807, GLYPH_dagesh },
	{ GLYPH_afii57807, GLYPH_dageshhebrew },
	{ GLYPH_afii57839, GLYPH_siluqhebrew },
	{ GLYPH_afii57839, GLYPH_siluqlefthebrew },
	{ GLYPH_afii57841, GLYPH_rafe },
	{ GLYPH_afii57841, GLYPH_rafehebrew },
	{ GLYPH_afii57842, GLYPH_paseqhebrew },
	{ GLYPH_afii57929, GLYPH_apostrophemod },
	{ GLYPH_afii57929, GLYPH_quoteright },
	{ GLYPH_afii61248, GLYPH_careof },
	{ GLYPH_afii61289, GLYPH_lsquare },
	{ GLYPH_afii61352, GLYPH_numero },
	{ GLYPH_afii61664, GLYPH_zerowidthnonjoiner },
	{ GLYPH_afii63167, GLYPH_asteriskaltonearabic },
	{ GLYPH_afii63167, GLYPH_asteriskarabic },
	{ GLYPH_afii64937, GLYPH_commareversedmod },
	{ GLYPH_afii64937, GLYPH_quoteleft },
	{ GLYPH_alpha, GLYPH_afii9825 },
	{ GLYPH_alphatonos, GLYPH_afii9968 },
	{ GLYPH_arrowdblboth, GLYPH_dblarrowleft },
	{ GLYPH_arrowdblleft, GLYPH_arrowleftdbl },
	{ GLYPH_arrowdblright, GLYPH_dblarrowright },
	{ GLYPH_arrowupdnbse, GLYPH_arrowupdownbase },
	{ GLYPH_asciitilde, GLYPH_tilde },
	{ GLYPH_bar, GLYPH_verticalbar },
	{ GLYPH_beta, GLYPH_afii9826 },
	{ GLYPH_brokenbar, GLYPH_bar },
	{ GLYPH_cdotaccent, GLYPH_cdot },
	{ GLYPH_cedilla, GLYPH_commaaccent },
	{ GLYPH_chi, GLYPH_afii9851 },
	{ GLYPH_circle, GLYPH_whitecircle },
	{ GLYPH_circlemultiply, GLYPH_timescircle },
	{ GLYPH_circleplus, GLYPH_pluscircle },
	{ GLYPH_club, GLYPH_clubsuitblack },
	{ GLYPH_colonmonetary, GLYPH_colonsign },
	{ GLYPH_congruent, GLYPH_approximatelyequal },
	{ GLYPH_dcroat, GLYPH_dmacron },
	{ GLYPH_dcroat, GLYPH_dstroke },
	{ GLYPH_degree, GLYPH_ring },
	{ GLYPH_delta, GLYPH_afii9829 },
	{ GLYPH_dieresis, GLYPH_diaeresis },
	{ GLYPH_dieresistonos, GLYPH_dialytikatonos },
	{ GLYPH_dieresistonos, GLYPH_dieresisacute },
	{ GLYPH_divide, GLYPH_division },
	{ GLYPH_dkshade, GLYPH_shadedark },
	{ GLYPH_dotaccent, GLYPH_abovedot },
	{ GLYPH_dotbelowcomb, GLYPH_dotbelowcmb },
	{ GLYPH_edieresis, GLYPH_ediaeresis },
	{ GLYPH_edotaccent, GLYPH_edot },
	{ GLYPH_epsilon, GLYPH_afii9830 },
	{ GLYPH_epsilontonos, GLYPH_afii9969 },
	{ GLYPH_eta, GLYPH_afii9834 },
	{ GLYPH_etatonos, GLYPH_afii9970 },
	{ GLYPH_existential, GLYPH_thereexists },
	{ GLYPH_female, GLYPH_venus },
	{ GLYPH_filledbox, GLYPH_blacksquare },
	{ GLYPH_filledrect, GLYPH_blackrectangle },
	{ GLYPH_gamma, GLYPH_afii9828 },
	{ GLYPH_gcommaaccent, GLYPH_gcedilla },
	{ GLYPH_gdotaccent, GLYPH_gdot },
	{ GLYPH_germandbls, GLYPH_ssharp },
	{ GLYPH_gradient, GLYPH_nabla },
	{ GLYPH_grave, GLYPH_quoteleft },
	{ GLYPH_gravecomb, GLYPH_gravecmb },
	{ GLYPH_heart, GLYPH_heartsuitblack },
	{ GLYPH_hookabovecomb, GLYPH_hookcmb },
	{ GLYPH_hungarumlaut, GLYPH_doubleacute },
	{ GLYPH_hyphen, GLYPH_sfthyphen },
	{ GLYPH_integralbt, GLYPH_integralbottom },
	{ GLYPH_integraltp, GLYPH_integraltop },
	{ GLYPH_invbullet, GLYPH_bulletinverse },
	{ GLYPH_invcircle, GLYPH_whitecircleinverse },
	{ GLYPH_invsmileface, GLYPH_blacksmilingface },
	{ GLYPH_iota, GLYPH_afii9836 },
	{ GLYPH_iotadieresis, GLYPH_afii9975 },
	{ GLYPH_iotadieresistonos, GLYPH_afii9977 },
	{ GLYPH_iotatonos, GLYPH_afii9971 },
	{ GLYPH_kappa, GLYPH_afii9837 },
	{ GLYPH_kcommaaccent, GLYPH_kcedilla },
	{ GLYPH_lambda, GLYPH_afii9838 },
	{ GLYPH_lcommaaccent, GLYPH_lcedilla },
	{ GLYPH_ldot, GLYPH_ldotaccent },
	{ GLYPH_lira, GLYPH_afii08941 },
	{ GLYPH_longs, GLYPH_slong },
	{ GLYPH_lslash, GLYPH_lstroke },
	{ GLYPH_ltshade, GLYPH_shadelight },
	{ GLYPH_macron, GLYPH_overscore },
	{ GLYPH_male, GLYPH_mars },
	{ GLYPH_minus, GLYPH_hyphen },
	{ GLYPH_mu, GLYPH_afii9839 },
	{ GLYPH_mu, GLYPH_micro },
	{ GLYPH_mu, GLYPH_mu1 },
	{ GLYPH_musicalnotedbl, GLYPH_eighthnotebeamed },
	{ GLYPH_napostrophe, GLYPH_quoterightn },
	{ GLYPH_ncommaaccent, GLYPH_ncedilla },
	{ GLYPH_nobreakspace, GLYPH_nonbreakingspace },
	{ GLYPH_nobreakspace, GLYPH_space },
	{ GLYPH_notelement, GLYPH_notelementof },
	{ GLYPH_nu, GLYPH_afii9840 },
	{ GLYPH_odieresis, GLYPH_odiaeresis },
	{ GLYPH_ohungarumlaut, GLYPH_odblacute },
	{ GLYPH_ohungarumlaut, GLYPH_odoubleacute },
	{ GLYPH_ohungarumlaut, GLYPH_ohungar },
	{ GLYPH_omega, GLYPH_afii9853 },
	{ GLYPH_omega1, GLYPH_pisymbolgreek },
	{ GLYPH_omegatonos, GLYPH_afii9974 },
	{ GLYPH_omicron, GLYPH_afii9842 },
	{ GLYPH_omicrontonos, GLYPH_afii9972 },
	{ GLYPH_openbullet, GLYPH_whitebullet },
	{ GLYPH_orthogonal, GLYPH_rightangle },
	{ GLYPH_oslashacute, GLYPH_ostrokeacute },
	{ GLYPH_period, GLYPH_fullstop },
	{ GLYPH_periodcentered, GLYPH_middot },
	{ GLYPH_phi, GLYPH_afii9850 },
	{ GLYPH_phi1, GLYPH_phisymbolgreek },
	{ GLYPH_pi, GLYPH_afii9843 },
	{ GLYPH_propersubset, GLYPH_subset },
	{ GLYPH_propersuperset, GLYPH_superset },
	{ GLYPH_psi, GLYPH_afii9852 },
	{ GLYPH_quoteleft, GLYPH_grave },
	{ GLYPH_quotereversed, GLYPH_quoteleftreversed },
	{ GLYPH_quoteright, GLYPH_apostrophe },
	{ GLYPH_quoteright, GLYPH_quotesingle },
	{ GLYPH_quotesingle, GLYPH_quoteright },
	{ GLYPH_rcommaaccent, GLYPH_rcedilla },
	{ GLYPH_reflexsubset, GLYPH_subsetorequal },
	{ GLYPH_reflexsuperset, GLYPH_supersetorequal },
	{ GLYPH_revlogicalnot, GLYPH_logicalnotreversed },
	{ GLYPH_rho, GLYPH_afii9845 },
	{ GLYPH_ring, GLYPH_degree },
	{ GLYPH_scedilla, GLYPH_scedilla1 },
	{ GLYPH_scedilla, GLYPH_scommaaccent },
	{ GLYPH_semicolon, GLYPH_uni037e },
	{ GLYPH_shade, GLYPH_shademedium },
	{ GLYPH_sigma, GLYPH_afii9846 },
	{ GLYPH_sigma1, GLYPH_afii9847 },
	{ GLYPH_sigma1, GLYPH_sigmafinal },
	{ GLYPH_similar, GLYPH_tildeoperator },
	{ GLYPH_smileface, GLYPH_whitesmilingface },
	{ GLYPH_space, GLYPH__notdef },
	{ GLYPH_space, GLYPH_nonbreakingspace },
	{ GLYPH_space, GLYPH_spacehackarabic },
	{ GLYPH_spade, GLYPH_spadesuitblack },
	{ GLYPH_sun, GLYPH_compass },
	{ GLYPH_tau, GLYPH_afii9848 },
	{ GLYPH_tcommaaccent, GLYPH_tcedilla },
	{ GLYPH_tcommaaccent, GLYPH_tcedilla1 },
	{ GLYPH_theta, GLYPH_afii9835 },
	{ GLYPH_theta1, GLYPH_thetasymbolgreek },
	{ GLYPH_tilde, GLYPH_ilde },
	{ GLYPH_tildecomb, GLYPH_tildecmb },
	{ GLYPH_tonos, GLYPH_acute },
	{ GLYPH_triagdn, GLYPH_blackdownpointingtriangle },
	{ GLYPH_triaglf, GLYPH_blackleftpointingpointer },
	{ GLYPH_triagrt, GLYPH_blackrightpointingpointer },
	{ GLYPH_triagup, GLYPH_blackuppointingtriangle },
	{ GLYPH_twodotenleader, GLYPH_twodotleader },
	{ GLYPH_udieresis, GLYPH_udiaeresis },
	{ GLYPH_uhungarumlaut, GLYPH_udblacute },
	{ GLYPH_uhungarumlaut, GLYPH_udoubleacute },
	{ GLYPH_uhungarumlaut, GLYPH_uhungar },
	{ GLYPH_underscore, GLYPH_lowline },
	{ GLYPH_underscoredbl, GLYPH_dbllowline },
	{ GLYPH_universal, GLYPH_forall },
	{ GLYPH_upsilon, GLYPH_afii9849 },
	{ GLYPH_upsilondieresis, GLYPH_afii9976 },
	{ GLYPH_upsilondieresistonos, GLYPH_afii9978 },
	{ GLYPH_upsilontonos, GLYPH_afii9973 },
	{ GLYPH_xi, GLYPH_afii9841 },
	{ GLYPH_ydieresis, GLYPH_y },
	{ GLYPH_zdotaccent, GLYPH_zabovedot },
	{ GLYPH_zdotaccent, GLYPH_zdot },
	{ GLYPH_zeta, GLYPH_afii9833 },
    };

/************************************************************************/
/*									*/
/*  See whether the font contains an alternative for a missing glyph.	*/
/*									*/
/************************************************************************/

int psFindAlternate(	const AfmFontInfo *	afi,
			const char *		glyphName )
    {
    const AlternateGlyphName *	agn= PS_AlternateNames;

    int				l;
    int				r;
    int				m;

    int				cmp;

#   if 0
    static int			sortChecked= 0;

    r= sizeof(PS_AlternateNames)/sizeof(AlternateGlyphName);

    if  ( ! sortChecked )
	{
	for ( m= 1; m < r; m++ )
	    {
	    cmp= strcmp( agn[m].agnOfficialName, agn[m- 1].agnOfficialName );
	    if  ( cmp > 0 )
		{ continue;	}

	    if  ( cmp < 0 )
		{
		SSDEB(agn[m].agnOfficialName,agn[m- 1].agnOfficialName);
		return -1;
		}

	    cmp= strcmp( agn[m].agnSurrogateName, agn[m- 1].agnSurrogateName );
	    if  ( cmp > 0 )
		{ continue;	}

	    SSDEB(agn[m].agnOfficialName,agn[m- 1].agnOfficialName);
	    SSDEB(agn[m].agnSurrogateName,agn[m- 1].agnSurrogateName);
	    return -1;
	    }

	sortChecked= 1;
	}
#   endif

    l= 0;
    r= sizeof(PS_AlternateNames)/sizeof(AlternateGlyphName);
    m= ( l+ r )/ 2;

    while( m != l )
	{
	cmp= strcmp( agn[m].agnOfficialName, glyphName );

	if  ( cmp >= 0 )
	    { r= m;	}
	else{ l= m;	}

	m= ( l+ r )/ 2;
	}

    cmp= strcmp( agn[m].agnOfficialName, glyphName );
    if  ( cmp < 0 )
	{ m++; }


    agn += m;
    r= sizeof(PS_AlternateNames)/sizeof(AlternateGlyphName);

    while( m < r && ! strcmp( agn->agnOfficialName, glyphName ) )
	{
	int			glyphIndex;

	glyphIndex= psFontInfoGetGlyphIndex( afi, agn->agnSurrogateName );
	if  ( glyphIndex >= 0 )
	    { return glyphIndex;	}

	agn++; m++;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Translate unicode values to glyph names.				*/
/*									*/
/************************************************************************/

typedef struct UnicodeToGlyphName
    {
    unsigned short	utgUnicode;
    const char *	utgGlyphName;
    } UnicodeToGlyphName;

static const UnicodeToGlyphName psUnicodeToGlyphNames[]=
{
    { 0x0020, GLYPH_space },
    { 0x0021, GLYPH_exclam },
    { 0x0022, GLYPH_quotedbl },
    { 0x0023, GLYPH_numbersign },
    { 0x0024, GLYPH_dollar },
    { 0x0025, GLYPH_percent },
    { 0x0026, GLYPH_ampersand },
    { 0x0027, GLYPH_quotesingle },
    { 0x0028, GLYPH_parenleft },
    { 0x0029, GLYPH_parenright },
    { 0x002a, GLYPH_asterisk },
    { 0x002b, GLYPH_plus },
    { 0x002c, GLYPH_comma },
    { 0x002d, GLYPH_hyphen },
    { 0x002e, GLYPH_period },
    { 0x002f, GLYPH_slash },
    { 0x0030, GLYPH_zero },
    { 0x0030, GLYPH_zero },
    { 0x0031, GLYPH_one },
    { 0x0032, GLYPH_two },
    { 0x0033, GLYPH_three },
    { 0x0034, GLYPH_four },
    { 0x0035, GLYPH_five },
    { 0x0036, GLYPH_six },
    { 0x0037, GLYPH_seven },
    { 0x0038, GLYPH_eight },
    { 0x0039, GLYPH_nine },
    { 0x003a, GLYPH_colon },
    { 0x003b, GLYPH_semicolon },
    { 0x003c, GLYPH_less },
    { 0x003d, GLYPH_equal },
    { 0x003e, GLYPH_greater },
    { 0x003f, GLYPH_question },
    { 0x0040, GLYPH_at },
    { 0x0041, GLYPH_A },
    { 0x0042, GLYPH_B },
    { 0x0043, GLYPH_C },
    { 0x0044, GLYPH_D },
    { 0x0045, GLYPH_E },
    { 0x0046, GLYPH_F },
    { 0x0047, GLYPH_G },
    { 0x0048, GLYPH_H },
    { 0x0049, GLYPH_I },
    { 0x004a, GLYPH_J },
    { 0x004b, GLYPH_K },
    { 0x004c, GLYPH_L },
    { 0x004d, GLYPH_M },
    { 0x004e, GLYPH_N },
    { 0x004f, GLYPH_O },
    { 0x0050, GLYPH_P },
    { 0x0051, GLYPH_Q },
    { 0x0052, GLYPH_R },
    { 0x0053, GLYPH_S },
    { 0x0054, GLYPH_T },
    { 0x0055, GLYPH_U },
    { 0x0056, GLYPH_V },
    { 0x0057, GLYPH_W },
    { 0x0058, GLYPH_X },
    { 0x0059, GLYPH_Y },
    { 0x005a, GLYPH_Z },
    { 0x005b, GLYPH_bracketleft },
    { 0x005c, GLYPH_backslash },
    { 0x005d, GLYPH_bracketright },
    { 0x005e, GLYPH_asciicircum },
    { 0x005f, GLYPH_underscore },
    { 0x0060, GLYPH_grave },
    { 0x0061, GLYPH_a },
    { 0x0062, GLYPH_b },
    { 0x0063, GLYPH_c },
    { 0x0064, GLYPH_d },
    { 0x0065, GLYPH_e },
    { 0x0066, GLYPH_f },
    { 0x0067, GLYPH_g },
    { 0x0068, GLYPH_h },
    { 0x0069, GLYPH_i },
    { 0x006a, GLYPH_j },
    { 0x006b, GLYPH_k },
    { 0x006c, GLYPH_l },
    { 0x006d, GLYPH_m },
    { 0x006e, GLYPH_n },
    { 0x006f, GLYPH_o },
    { 0x0070, GLYPH_p },
    { 0x0071, GLYPH_q },
    { 0x0072, GLYPH_r },
    { 0x0073, GLYPH_s },
    { 0x0074, GLYPH_t },
    { 0x0075, GLYPH_u },
    { 0x0076, GLYPH_v },
    { 0x0077, GLYPH_w },
    { 0x0077, GLYPH_w },
    { 0x0078, GLYPH_x },
    { 0x0078, GLYPH_x },
    { 0x0079, GLYPH_y },
    { 0x0079, GLYPH_y },
    { 0x007a, GLYPH_z },
    { 0x007a, GLYPH_z },
    { 0x007b, GLYPH_braceleft },
    { 0x007c, GLYPH_bar },
    { 0x007d, GLYPH_braceright },
    { 0x007e, GLYPH_asciitilde },
    { 0x00a0, GLYPH_nobreakspace },
    { 0x00a1, GLYPH_exclamdown },
    { 0x00a2, GLYPH_cent },
    { 0x00a3, GLYPH_sterling },
    { 0x00a4, GLYPH_currency },
    { 0x00a5, GLYPH_yen },
    { 0x00a5, GLYPH_yen },
    { 0x00a6, GLYPH_brokenbar },
    { 0x00a7, GLYPH_section },
    { 0x00a8, GLYPH_dieresis },
    { 0x00a9, GLYPH_copyright },
    { 0x00aa, GLYPH_ordfeminine },
    { 0x00ab, GLYPH_guillemotleft },
    { 0x00ac, GLYPH_logicalnot },
    { 0x00ae, GLYPH_registered },
    { 0x00af, GLYPH_macron },
    { 0x00b0, GLYPH_degree },
    { 0x00b1, GLYPH_plusminus },
    { 0x00b2, GLYPH_twosuperior },
    { 0x00b3, GLYPH_threesuperior },
    { 0x00b4, GLYPH_acute },
    { 0x00b5, GLYPH_mu },
    { 0x00b6, GLYPH_paragraph },
    { 0x00b7, GLYPH_periodcentered },
    { 0x00b8, GLYPH_cedilla },
    { 0x00b9, GLYPH_onesuperior },
    { 0x00ba, GLYPH_ordmasculine },
    { 0x00bb, GLYPH_guillemotright },
    { 0x00bc, GLYPH_onequarter },
    { 0x00bd, GLYPH_onehalf },
    { 0x00be, GLYPH_threequarters },
    { 0x00bf, GLYPH_questiondown },
    { 0x00c0, GLYPH_Agrave },
    { 0x00c1, GLYPH_Aacute },
    { 0x00c2, GLYPH_Acircumflex },
    { 0x00c3, GLYPH_Atilde },
    { 0x00c4, GLYPH_Adieresis },
    { 0x00c5, GLYPH_Aring },
    { 0x00c6, GLYPH_AE },
    { 0x00c7, GLYPH_Ccedilla },
    { 0x00c8, GLYPH_Egrave },
    { 0x00c9, GLYPH_Eacute },
    { 0x00ca, GLYPH_Ecircumflex },
    { 0x00cb, GLYPH_Edieresis },
    { 0x00cc, GLYPH_Igrave },
    { 0x00cd, GLYPH_Iacute },
    { 0x00ce, GLYPH_Icircumflex },
    { 0x00cf, GLYPH_Idieresis },
    { 0x00d0, GLYPH_Eth },
    { 0x00d1, GLYPH_Ntilde },
    { 0x00d2, GLYPH_Ograve },
    { 0x00d3, GLYPH_Oacute },
    { 0x00d4, GLYPH_Ocircumflex },
    { 0x00d5, GLYPH_Otilde },
    { 0x00d6, GLYPH_Odieresis },
    { 0x00d7, GLYPH_multiply },
    { 0x00d8, GLYPH_Oslash },
    { 0x00d9, GLYPH_Ugrave },
    { 0x00da, GLYPH_Uacute },
    { 0x00db, GLYPH_Ucircumflex },
    { 0x00dc, GLYPH_Udieresis },
    { 0x00dd, GLYPH_Yacute },
    { 0x00de, GLYPH_Thorn },
    { 0x00df, GLYPH_germandbls },
    { 0x00e0, GLYPH_agrave },
    { 0x00e1, GLYPH_aacute },
    { 0x00e2, GLYPH_acircumflex },
    { 0x00e3, GLYPH_atilde },
    { 0x00e4, GLYPH_adieresis },
    { 0x00e5, GLYPH_aring },
    { 0x00e6, GLYPH_ae },
    { 0x00e7, GLYPH_ccedilla },
    { 0x00e8, GLYPH_egrave },
    { 0x00e9, GLYPH_eacute },
    { 0x00ea, GLYPH_ecircumflex },
    { 0x00eb, GLYPH_edieresis },
    { 0x00ec, GLYPH_igrave },
    { 0x00ed, GLYPH_iacute },
    { 0x00ee, GLYPH_icircumflex },
    { 0x00ef, GLYPH_idieresis },
    { 0x00f0, GLYPH_eth },
    { 0x00f1, GLYPH_ntilde },
    { 0x00f2, GLYPH_ograve },
    { 0x00f3, GLYPH_oacute },
    { 0x00f4, GLYPH_ocircumflex },
    { 0x00f5, GLYPH_otilde },
    { 0x00f6, GLYPH_odieresis },
    { 0x00f7, GLYPH_divide },
    { 0x00f8, GLYPH_oslash },
    { 0x00f9, GLYPH_ugrave },
    { 0x00fa, GLYPH_uacute },
    { 0x00fb, GLYPH_ucircumflex },
    { 0x00fc, GLYPH_udieresis },
    { 0x00fd, GLYPH_yacute },
    { 0x00fd, GLYPH_yacute },
    { 0x00fe, GLYPH_thorn },
    { 0x00ff, GLYPH_ydieresis },
    { 0x00ff, GLYPH_ydieresis },
    { 0x0100, GLYPH_Amacron },
    { 0x0101, GLYPH_amacron },
    { 0x0102, GLYPH_Abreve },
    { 0x0103, GLYPH_abreve },
    { 0x0104, GLYPH_Aogonek },
    { 0x0105, GLYPH_aogonek },
    { 0x0106, GLYPH_Cacute },
    { 0x0107, GLYPH_cacute },
    { 0x0108, GLYPH_Ccircumflex },
    { 0x0109, GLYPH_ccircumflex },
    { 0x010a, GLYPH_Cdotaccent },
    { 0x010b, GLYPH_cdotaccent },
    { 0x010c, GLYPH_Ccaron },
    { 0x010d, GLYPH_ccaron },
    { 0x010e, GLYPH_Dcaron },
    { 0x010f, GLYPH_dcaron },
    { 0x0110, GLYPH_Dcroat },
    { 0x0111, GLYPH_dcroat },
    { 0x0112, GLYPH_Emacron },
    { 0x0113, GLYPH_emacron },
    { 0x0114, GLYPH_Ebreve },
    { 0x0115, GLYPH_ebreve },
    { 0x0116, GLYPH_Edotaccent },
    { 0x0117, GLYPH_edotaccent },
    { 0x0118, GLYPH_Eogonek },
    { 0x0119, GLYPH_eogonek },
    { 0x011a, GLYPH_Ecaron },
    { 0x011b, GLYPH_ecaron },
    { 0x011c, GLYPH_Gcircumflex },
    { 0x011d, GLYPH_gcircumflex },
    { 0x011e, GLYPH_Gbreve },
    { 0x011f, GLYPH_gbreve },
    { 0x0120, GLYPH_Gdotaccent },
    { 0x0121, GLYPH_gdotaccent },
    { 0x0122, GLYPH_Gcommaaccent },
    { 0x0123, GLYPH_gcommaaccent },
    { 0x0124, GLYPH_Hcircumflex },
    { 0x0125, GLYPH_hcircumflex },
    { 0x0126, GLYPH_Hbar },
    { 0x0127, GLYPH_hbar },
    { 0x0128, GLYPH_Itilde },
    { 0x0129, GLYPH_itilde },
    { 0x012a, GLYPH_Imacron },
    { 0x012b, GLYPH_imacron },
    { 0x012c, GLYPH_Ibreve },
    { 0x012d, GLYPH_ibreve },
    { 0x012e, GLYPH_Iogonek },
    { 0x012f, GLYPH_iogonek },
    { 0x0130, GLYPH_Idotaccent },
    { 0x0131, GLYPH_dotlessi },
    { 0x0132, GLYPH_IJ },
    { 0x0133, GLYPH_ij },
    { 0x0134, GLYPH_Jcircumflex },
    { 0x0135, GLYPH_jcircumflex },
    { 0x0136, GLYPH_Kcommaaccent },
    { 0x0137, GLYPH_kcommaaccent },
    { 0x0138, GLYPH_kgreenlandic },
    { 0x0139, GLYPH_Lacute },
    { 0x013a, GLYPH_lacute },
    { 0x013b, GLYPH_Lcommaaccent },
    { 0x013c, GLYPH_lcommaaccent },
    { 0x013d, GLYPH_Lcaron },
    { 0x013e, GLYPH_lcaron },
    { 0x013f, GLYPH_Ldot },
    { 0x0140, GLYPH_ldot },
    { 0x0141, GLYPH_Lslash },
    { 0x0142, GLYPH_lslash },
    { 0x0143, GLYPH_Nacute },
    { 0x0144, GLYPH_nacute },
    { 0x0145, GLYPH_Ncommaaccent },
    { 0x0146, GLYPH_ncommaaccent },
    { 0x0147, GLYPH_Ncaron },
    { 0x0148, GLYPH_ncaron },
    { 0x0149, GLYPH_napostrophe },
    { 0x014a, GLYPH_Eng },
    { 0x014b, GLYPH_eng },
    { 0x014c, GLYPH_Omacron },
    { 0x014d, GLYPH_omacron },
    { 0x014e, GLYPH_Obreve },
    { 0x014f, GLYPH_obreve },
    { 0x0150, GLYPH_Ohungarumlaut },
    { 0x0151, GLYPH_ohungarumlaut },
    { 0x0152, GLYPH_OE },
    { 0x0153, GLYPH_oe },
    { 0x0154, GLYPH_Racute },
    { 0x0155, GLYPH_racute },
    { 0x0156, GLYPH_Rcommaaccent },
    { 0x0157, GLYPH_rcommaaccent },
    { 0x0158, GLYPH_Rcaron },
    { 0x0159, GLYPH_rcaron },
    { 0x015a, GLYPH_Sacute },
    { 0x015b, GLYPH_sacute },
    { 0x015c, GLYPH_Scircumflex },
    { 0x015d, GLYPH_scircumflex },
    { 0x015e, GLYPH_Scedilla },
    { 0x015f, GLYPH_scedilla },
    { 0x0160, GLYPH_Scaron },
    { 0x0161, GLYPH_scaron },
    { 0x0162, GLYPH_Tcommaaccent },
    { 0x0163, GLYPH_tcommaaccent },
    { 0x0164, GLYPH_Tcaron },
    { 0x0165, GLYPH_tcaron },
    { 0x0166, GLYPH_Tbar },
    { 0x0167, GLYPH_tbar },
    { 0x0168, GLYPH_Utilde },
    { 0x0169, GLYPH_utilde },
    { 0x016a, GLYPH_Umacron },
    { 0x016b, GLYPH_umacron },
    { 0x016c, GLYPH_Ubreve },
    { 0x016d, GLYPH_ubreve },
    { 0x016e, GLYPH_Uring },
    { 0x016f, GLYPH_uring },
    { 0x0170, GLYPH_Uhungarumlaut },
    { 0x0171, GLYPH_uhungarumlaut },
    { 0x0172, GLYPH_Uogonek },
    { 0x0173, GLYPH_uogonek },
    { 0x0174, GLYPH_Wcircumflex },
    { 0x0175, GLYPH_wcircumflex },
    { 0x0175, GLYPH_wcircumflex },
    { 0x0176, GLYPH_Ycircumflex },
    { 0x0177, GLYPH_ycircumflex },
    { 0x0177, GLYPH_ycircumflex },
    { 0x0178, GLYPH_Ydieresis },
    { 0x0179, GLYPH_Zacute },
    { 0x017a, GLYPH_zacute },
    { 0x017a, GLYPH_zacute },
    { 0x017b, GLYPH_Zdotaccent },
    { 0x017c, GLYPH_zdotaccent },
    { 0x017c, GLYPH_zdotaccent },
    { 0x017d, GLYPH_Zcaron },
    { 0x017e, GLYPH_zcaron },
    { 0x017e, GLYPH_zcaron },
    { 0x017f, GLYPH_longs },
    { 0x0192, GLYPH_florin },
    { 0x01a0, GLYPH_Ohorn },
    { 0x01a1, GLYPH_ohorn },
    { 0x01af, GLYPH_Uhorn },
    { 0x01b0, GLYPH_uhorn },
    { 0x01e6, GLYPH_Gcaron },
    { 0x01e7, GLYPH_gcaron },
    { 0x01fa, GLYPH_Aringacute },
    { 0x01fb, GLYPH_aringacute },
    { 0x01fc, GLYPH_AEacute },
    { 0x01fd, GLYPH_aeacute },
    { 0x01fe, GLYPH_Oslashacute },
    { 0x01ff, GLYPH_oslashacute },
    { 0x0218, GLYPH_Scommaaccent },
    { 0x0219, GLYPH_scommaaccent },
    { 0x02bc, GLYPH_afii57929 },
    { 0x02bd, GLYPH_afii64937 },
    { 0x02c6, GLYPH_circumflex },
    { 0x02c7, GLYPH_caron },
    { 0x02d8, GLYPH_breve },
    { 0x02d9, GLYPH_dotaccent },
    { 0x02da, GLYPH_ring },
    { 0x02db, GLYPH_ogonek },
    { 0x02dc, GLYPH_tilde },
    { 0x02dd, GLYPH_hungarumlaut },
    { 0x0300, GLYPH_gravecomb },
    { 0x0301, GLYPH_acutecomb },
    { 0x0303, GLYPH_tildecomb },
    { 0x0309, GLYPH_hookabovecomb },
    { 0x0323, GLYPH_dotbelowcomb },
    { 0x0384, GLYPH_tonos },
    { 0x0385, GLYPH_dieresistonos },
    { 0x0386, GLYPH_Alphatonos },
    { 0x0387, GLYPH_anoteleia },
    { 0x0388, GLYPH_Epsilontonos },
    { 0x0389, GLYPH_Etatonos },
    { 0x038a, GLYPH_Iotatonos },
    { 0x038c, GLYPH_Omicrontonos },
    { 0x038e, GLYPH_Upsilontonos },
    { 0x038f, GLYPH_Omegatonos },
    { 0x0390, GLYPH_iotadieresistonos },
    { 0x0391, GLYPH_Alpha },
    { 0x0392, GLYPH_Beta },
    { 0x0393, GLYPH_Gamma },
    { 0x0395, GLYPH_Epsilon },
    { 0x0396, GLYPH_Zeta },
    { 0x0397, GLYPH_Eta },
    { 0x0398, GLYPH_Theta },
    { 0x0399, GLYPH_Iota },
    { 0x039a, GLYPH_Kappa },
    { 0x039b, GLYPH_Lambda },
    { 0x039c, GLYPH_Mu },
    { 0x039d, GLYPH_Nu },
    { 0x039e, GLYPH_Xi },
    { 0x039f, GLYPH_Omicron },
    { 0x03a0, GLYPH_Pi },
    { 0x03a1, GLYPH_Rho },
    { 0x03a3, GLYPH_Sigma },
    { 0x03a4, GLYPH_Tau },
    { 0x03a5, GLYPH_Upsilon },
    { 0x03a6, GLYPH_Phi },
    { 0x03a7, GLYPH_Chi },
    { 0x03a8, GLYPH_Psi },
    { 0x03aa, GLYPH_Iotadieresis },
    { 0x03ab, GLYPH_Upsilondieresis },
    { 0x03ac, GLYPH_alphatonos },
    { 0x03ad, GLYPH_epsilontonos },
    { 0x03ae, GLYPH_etatonos },
    { 0x03af, GLYPH_iotatonos },
    { 0x03b0, GLYPH_upsilondieresistonos },
    { 0x03b1, GLYPH_alpha },
    { 0x03b2, GLYPH_beta },
    { 0x03b3, GLYPH_gamma },
    { 0x03b4, GLYPH_delta },
    { 0x03b5, GLYPH_epsilon },
    { 0x03b6, GLYPH_zeta },
    { 0x03b6, GLYPH_zeta },
    { 0x03b7, GLYPH_eta },
    { 0x03b8, GLYPH_theta },
    { 0x03b9, GLYPH_iota },
    { 0x03ba, GLYPH_kappa },
    { 0x03bb, GLYPH_lambda },
    { 0x03bd, GLYPH_nu },
    { 0x03be, GLYPH_xi },
    { 0x03be, GLYPH_xi },
    { 0x03bf, GLYPH_omicron },
    { 0x03c0, GLYPH_pi },
    { 0x03c1, GLYPH_rho },
    { 0x03c2, GLYPH_sigma1 },
    { 0x03c3, GLYPH_sigma },
    { 0x03c4, GLYPH_tau },
    { 0x03c5, GLYPH_upsilon },
    { 0x03c6, GLYPH_phi },
    { 0x03c7, GLYPH_chi },
    { 0x03c8, GLYPH_psi },
    { 0x03c9, GLYPH_omega },
    { 0x03ca, GLYPH_iotadieresis },
    { 0x03cb, GLYPH_upsilondieresis },
    { 0x03cc, GLYPH_omicrontonos },
    { 0x03cd, GLYPH_upsilontonos },
    { 0x03ce, GLYPH_omegatonos },
    { 0x03d1, GLYPH_theta1 },
    { 0x03d2, GLYPH_Upsilon1 },
    { 0x03d5, GLYPH_phi1 },
    { 0x03d6, GLYPH_omega1 },
    { 0x0401, GLYPH_afii10023 },
    { 0x0402, GLYPH_afii10051 },
    { 0x0403, GLYPH_afii10052 },
    { 0x0404, GLYPH_afii10053 },
    { 0x0405, GLYPH_afii10054 },
    { 0x0406, GLYPH_afii10055 },
    { 0x0407, GLYPH_afii10056 },
    { 0x0408, GLYPH_afii10057 },
    { 0x0409, GLYPH_afii10058 },
    { 0x040a, GLYPH_afii10059 },
    { 0x040b, GLYPH_afii10060 },
    { 0x040c, GLYPH_afii10061 },
    { 0x040e, GLYPH_afii10062 },
    { 0x040f, GLYPH_afii10145 },
    { 0x0410, GLYPH_afii10017 },
    { 0x0411, GLYPH_afii10018 },
    { 0x0412, GLYPH_afii10019 },
    { 0x0413, GLYPH_afii10020 },
    { 0x0414, GLYPH_afii10021 },
    { 0x0415, GLYPH_afii10022 },
    { 0x0416, GLYPH_afii10024 },
    { 0x0417, GLYPH_afii10025 },
    { 0x0418, GLYPH_afii10026 },
    { 0x0419, GLYPH_afii10027 },
    { 0x041a, GLYPH_afii10028 },
    { 0x041b, GLYPH_afii10029 },
    { 0x041c, GLYPH_afii10030 },
    { 0x041d, GLYPH_afii10031 },
    { 0x041e, GLYPH_afii10032 },
    { 0x041f, GLYPH_afii10033 },
    { 0x0420, GLYPH_afii10034 },
    { 0x0421, GLYPH_afii10035 },
    { 0x0422, GLYPH_afii10036 },
    { 0x0423, GLYPH_afii10037 },
    { 0x0424, GLYPH_afii10038 },
    { 0x0425, GLYPH_afii10039 },
    { 0x0426, GLYPH_afii10040 },
    { 0x0427, GLYPH_afii10041 },
    { 0x0428, GLYPH_afii10042 },
    { 0x0429, GLYPH_afii10043 },
    { 0x042a, GLYPH_afii10044 },
    { 0x042b, GLYPH_afii10045 },
    { 0x042c, GLYPH_afii10046 },
    { 0x042d, GLYPH_afii10047 },
    { 0x042e, GLYPH_afii10048 },
    { 0x042f, GLYPH_afii10049 },
    { 0x0430, GLYPH_afii10065 },
    { 0x0431, GLYPH_afii10066 },
    { 0x0432, GLYPH_afii10067 },
    { 0x0433, GLYPH_afii10068 },
    { 0x0434, GLYPH_afii10069 },
    { 0x0435, GLYPH_afii10070 },
    { 0x0436, GLYPH_afii10072 },
    { 0x0437, GLYPH_afii10073 },
    { 0x0438, GLYPH_afii10074 },
    { 0x0439, GLYPH_afii10075 },
    { 0x043a, GLYPH_afii10076 },
    { 0x043b, GLYPH_afii10077 },
    { 0x043c, GLYPH_afii10078 },
    { 0x043d, GLYPH_afii10079 },
    { 0x043e, GLYPH_afii10080 },
    { 0x043f, GLYPH_afii10081 },
    { 0x0440, GLYPH_afii10082 },
    { 0x0441, GLYPH_afii10083 },
    { 0x0442, GLYPH_afii10084 },
    { 0x0443, GLYPH_afii10085 },
    { 0x0444, GLYPH_afii10086 },
    { 0x0445, GLYPH_afii10087 },
    { 0x0446, GLYPH_afii10088 },
    { 0x0447, GLYPH_afii10089 },
    { 0x0448, GLYPH_afii10090 },
    { 0x0449, GLYPH_afii10091 },
    { 0x044a, GLYPH_afii10092 },
    { 0x044b, GLYPH_afii10093 },
    { 0x044c, GLYPH_afii10094 },
    { 0x044d, GLYPH_afii10095 },
    { 0x044e, GLYPH_afii10096 },
    { 0x044f, GLYPH_afii10097 },
    { 0x0451, GLYPH_afii10071 },
    { 0x0452, GLYPH_afii10099 },
    { 0x0453, GLYPH_afii10100 },
    { 0x0454, GLYPH_afii10101 },
    { 0x0455, GLYPH_afii10102 },
    { 0x0456, GLYPH_afii10103 },
    { 0x0457, GLYPH_afii10104 },
    { 0x0458, GLYPH_afii10105 },
    { 0x0459, GLYPH_afii10106 },
    { 0x045a, GLYPH_afii10107 },
    { 0x045b, GLYPH_afii10108 },
    { 0x045c, GLYPH_afii10109 },
    { 0x045e, GLYPH_afii10110 },
    { 0x045f, GLYPH_afii10193 },
    { 0x0462, GLYPH_afii10146 },
    { 0x0463, GLYPH_afii10194 },
    { 0x0472, GLYPH_afii10147 },
    { 0x0473, GLYPH_afii10195 },
    { 0x0474, GLYPH_afii10148 },
    { 0x0475, GLYPH_afii10196 },
    { 0x0490, GLYPH_afii10050 },
    { 0x0491, GLYPH_afii10098 },
    { 0x04d9, GLYPH_afii10846 },
    { 0x05b0, GLYPH_afii57799 },
    { 0x05b1, GLYPH_afii57801 },
    { 0x05b2, GLYPH_afii57800 },
    { 0x05b3, GLYPH_afii57802 },
    { 0x05b4, GLYPH_afii57793 },
    { 0x05b5, GLYPH_afii57794 },
    { 0x05b6, GLYPH_afii57795 },
    { 0x05b7, GLYPH_afii57798 },
    { 0x05b8, GLYPH_afii57797 },
    { 0x05b9, GLYPH_afii57806 },
    { 0x05bb, GLYPH_afii57796 },
    { 0x05bc, GLYPH_afii57807 },
    { 0x05bd, GLYPH_afii57839 },
    { 0x05be, GLYPH_afii57645 },
    { 0x05bf, GLYPH_afii57841 },
    { 0x05c0, GLYPH_afii57842 },
    { 0x05c1, GLYPH_afii57804 },
    { 0x05c2, GLYPH_afii57803 },
    { 0x05c3, GLYPH_afii57658 },
    { 0x05d0, GLYPH_afii57664 },
    { 0x05d1, GLYPH_afii57665 },
    { 0x05d2, GLYPH_afii57666 },
    { 0x05d3, GLYPH_afii57667 },
    { 0x05d4, GLYPH_afii57668 },
    { 0x05d5, GLYPH_afii57669 },
    { 0x05d6, GLYPH_afii57670 },
    { 0x05d7, GLYPH_afii57671 },
    { 0x05d8, GLYPH_afii57672 },
    { 0x05d9, GLYPH_afii57673 },
    { 0x05da, GLYPH_afii57674 },
    { 0x05db, GLYPH_afii57675 },
    { 0x05dc, GLYPH_afii57676 },
    { 0x05dd, GLYPH_afii57677 },
    { 0x05de, GLYPH_afii57678 },
    { 0x05df, GLYPH_afii57679 },
    { 0x05e0, GLYPH_afii57680 },
    { 0x05e1, GLYPH_afii57681 },
    { 0x05e2, GLYPH_afii57682 },
    { 0x05e3, GLYPH_afii57683 },
    { 0x05e4, GLYPH_afii57684 },
    { 0x05e5, GLYPH_afii57685 },
    { 0x05e6, GLYPH_afii57686 },
    { 0x05e7, GLYPH_afii57687 },
    { 0x05e8, GLYPH_afii57688 },
    { 0x05e9, GLYPH_afii57689 },
    { 0x05ea, GLYPH_afii57690 },
    { 0x05f0, GLYPH_afii57716 },
    { 0x05f1, GLYPH_afii57717 },
    { 0x05f2, GLYPH_afii57718 },
    { 0x060c, GLYPH_afii57388 },
    { 0x061b, GLYPH_afii57403 },
    { 0x061f, GLYPH_afii57407 },
    { 0x0621, GLYPH_afii57409 },
    { 0x0622, GLYPH_afii57410 },
    { 0x0623, GLYPH_afii57411 },
    { 0x0624, GLYPH_afii57412 },
    { 0x0625, GLYPH_afii57413 },
    { 0x0626, GLYPH_afii57414 },
    { 0x0627, GLYPH_afii57415 },
    { 0x0628, GLYPH_afii57416 },
    { 0x0629, GLYPH_afii57417 },
    { 0x062a, GLYPH_afii57418 },
    { 0x062b, GLYPH_afii57419 },
    { 0x062c, GLYPH_afii57420 },
    { 0x062d, GLYPH_afii57421 },
    { 0x062e, GLYPH_afii57422 },
    { 0x062f, GLYPH_afii57423 },
    { 0x0630, GLYPH_afii57424 },
    { 0x0631, GLYPH_afii57425 },
    { 0x0632, GLYPH_afii57426 },
    { 0x0633, GLYPH_afii57427 },
    { 0x0634, GLYPH_afii57428 },
    { 0x0635, GLYPH_afii57429 },
    { 0x0636, GLYPH_afii57430 },
    { 0x0637, GLYPH_afii57431 },
    { 0x0638, GLYPH_afii57432 },
    { 0x0639, GLYPH_afii57433 },
    { 0x063a, GLYPH_afii57434 },
    { 0x0640, GLYPH_afii57440 },
    { 0x0641, GLYPH_afii57441 },
    { 0x0642, GLYPH_afii57442 },
    { 0x0643, GLYPH_afii57443 },
    { 0x0644, GLYPH_afii57444 },
    { 0x0645, GLYPH_afii57445 },
    { 0x0646, GLYPH_afii57446 },
    { 0x0647, GLYPH_afii57470 },
    { 0x0648, GLYPH_afii57448 },
    { 0x0649, GLYPH_afii57449 },
    { 0x064a, GLYPH_afii57450 },
    { 0x064b, GLYPH_afii57451 },
    { 0x064c, GLYPH_afii57452 },
    { 0x064d, GLYPH_afii57453 },
    { 0x064e, GLYPH_afii57454 },
    { 0x064f, GLYPH_afii57455 },
    { 0x0650, GLYPH_afii57456 },
    { 0x0651, GLYPH_afii57457 },
    { 0x0652, GLYPH_afii57458 },
    { 0x0660, GLYPH_afii57392 },
    { 0x0661, GLYPH_afii57393 },
    { 0x0662, GLYPH_afii57394 },
    { 0x0663, GLYPH_afii57395 },
    { 0x0664, GLYPH_afii57396 },
    { 0x0665, GLYPH_afii57397 },
    { 0x0666, GLYPH_afii57398 },
    { 0x0667, GLYPH_afii57399 },
    { 0x0668, GLYPH_afii57400 },
    { 0x0669, GLYPH_afii57401 },
    { 0x066a, GLYPH_afii57381 },
    { 0x066d, GLYPH_afii63167 },
    { 0x0679, GLYPH_afii57511 },
    { 0x067e, GLYPH_afii57506 },
    { 0x0686, GLYPH_afii57507 },
    { 0x0688, GLYPH_afii57512 },
    { 0x0691, GLYPH_afii57513 },
    { 0x0698, GLYPH_afii57508 },
    { 0x06a4, GLYPH_afii57505 },
    { 0x06af, GLYPH_afii57509 },
    { 0x06ba, GLYPH_afii57514 },
    { 0x06d2, GLYPH_afii57519 },
    { 0x06d5, GLYPH_afii57534 },
    { 0x1e80, GLYPH_Wgrave },
    { 0x1e81, GLYPH_wgrave },
    { 0x1e81, GLYPH_wgrave },
    { 0x1e82, GLYPH_Wacute },
    { 0x1e83, GLYPH_wacute },
    { 0x1e83, GLYPH_wacute },
    { 0x1e84, GLYPH_Wdieresis },
    { 0x1e85, GLYPH_wdieresis },
    { 0x1e85, GLYPH_wdieresis },
    { 0x1ef2, GLYPH_Ygrave },
    { 0x1ef3, GLYPH_ygrave },
    { 0x1ef3, GLYPH_ygrave },
    { 0x200c, GLYPH_afii61664 },
    { 0x200d, GLYPH_afii301 },
    { 0x200e, GLYPH_afii299 },
    { 0x200f, GLYPH_afii300 },
    { 0x2012, GLYPH_figuredash },
    { 0x2013, GLYPH_endash },
    { 0x2014, GLYPH_emdash },
    { 0x2015, GLYPH_afii00208 },
    { 0x2017, GLYPH_underscoredbl },
    { 0x2018, GLYPH_quoteleft },
    { 0x2019, GLYPH_quoteright },
    { 0x201a, GLYPH_quotesinglbase },
    { 0x201b, GLYPH_quotereversed },
    { 0x201c, GLYPH_quotedblleft },
    { 0x201d, GLYPH_quotedblright },
    { 0x201e, GLYPH_quotedblbase },
    { 0x2020, GLYPH_dagger },
    { 0x2021, GLYPH_daggerdbl },
    { 0x2022, GLYPH_bullet },
    { 0x2024, GLYPH_onedotenleader },
    { 0x2025, GLYPH_twodotenleader },
    { 0x2026, GLYPH_ellipsis },
    { 0x202c, GLYPH_afii61573 },
    { 0x202d, GLYPH_afii61574 },
    { 0x202e, GLYPH_afii61575 },
    { 0x2030, GLYPH_perthousand },
    { 0x2032, GLYPH_minute },
    { 0x2033, GLYPH_second },
    { 0x2039, GLYPH_guilsinglleft },
    { 0x203a, GLYPH_guilsinglright },
    { 0x203c, GLYPH_exclamdbl },
    { 0x2044, GLYPH_fraction },
    { 0x20a1, GLYPH_colonmonetary },
    { 0x20a3, GLYPH_franc },
    { 0x20a4, GLYPH_lira },
    { 0x20a7, GLYPH_peseta },
    { 0x20aa, GLYPH_afii57636 },
    { 0x20ab, GLYPH_dong },
    { 0x20ac, GLYPH_Euro },
    { 0x2105, GLYPH_afii61248 },
    { 0x2111, GLYPH_Ifraktur },
    { 0x2113, GLYPH_afii61289 },
    { 0x2116, GLYPH_afii61352 },
    { 0x2118, GLYPH_weierstrass },
    { 0x2118, GLYPH_weierstrass },
    { 0x211c, GLYPH_Rfraktur },
    { 0x211e, GLYPH_prescription },
    { 0x2122, GLYPH_trademark },
    { 0x2126, GLYPH_Omega },
    { 0x212e, GLYPH_estimated },
    { 0x2135, GLYPH_aleph },
    { 0x2153, GLYPH_onethird },
    { 0x2154, GLYPH_twothirds },
    { 0x215b, GLYPH_oneeighth },
    { 0x215c, GLYPH_threeeighths },
    { 0x215d, GLYPH_fiveeighths },
    { 0x215e, GLYPH_seveneighths },
    { 0x2190, GLYPH_arrowleft },
    { 0x2191, GLYPH_arrowup },
    { 0x2192, GLYPH_arrowright },
    { 0x2193, GLYPH_arrowdown },
    { 0x2194, GLYPH_arrowboth },
    { 0x2195, GLYPH_arrowupdn },
    { 0x21a8, GLYPH_arrowupdnbse },
    { 0x21b5, GLYPH_carriagereturn },
    { 0x21d0, GLYPH_arrowdblleft },
    { 0x21d1, GLYPH_arrowdblup },
    { 0x21d2, GLYPH_arrowdblright },
    { 0x21d3, GLYPH_arrowdbldown },
    { 0x21d4, GLYPH_arrowdblboth },
    { 0x2200, GLYPH_universal },
    { 0x2202, GLYPH_partialdiff },
    { 0x2203, GLYPH_existential },
    { 0x2205, GLYPH_emptyset },
    { 0x2206, GLYPH_Delta },
    { 0x2207, GLYPH_gradient },
    { 0x2208, GLYPH_element },
    { 0x2209, GLYPH_notelement },
    { 0x220b, GLYPH_suchthat },
    { 0x220f, GLYPH_product },
    { 0x2211, GLYPH_summation },
    { 0x2212, GLYPH_minus },
    { 0x2217, GLYPH_asteriskmath },
    { 0x221a, GLYPH_radical },
    { 0x221d, GLYPH_proportional },
    { 0x221e, GLYPH_infinity },
    { 0x221f, GLYPH_orthogonal },
    { 0x2220, GLYPH_angle },
    { 0x2227, GLYPH_logicaland },
    { 0x2228, GLYPH_logicalor },
    { 0x2229, GLYPH_intersection },
    { 0x222a, GLYPH_union },
    { 0x222b, GLYPH_integral },
    { 0x2234, GLYPH_therefore },
    { 0x223c, GLYPH_similar },
    { 0x2245, GLYPH_congruent },
    { 0x2248, GLYPH_approxequal },
    { 0x2260, GLYPH_notequal },
    { 0x2261, GLYPH_equivalence },
    { 0x2264, GLYPH_lessequal },
    { 0x2265, GLYPH_greaterequal },
    { 0x2282, GLYPH_propersubset },
    { 0x2283, GLYPH_propersuperset },
    { 0x2284, GLYPH_notsubset },
    { 0x2286, GLYPH_reflexsubset },
    { 0x2287, GLYPH_reflexsuperset },
    { 0x2295, GLYPH_circleplus },
    { 0x2297, GLYPH_circlemultiply },
    { 0x22a5, GLYPH_perpendicular },
    { 0x22c5, GLYPH_dotmath },
    { 0x2302, GLYPH_house },
    { 0x2310, GLYPH_revlogicalnot },
    { 0x2320, GLYPH_integraltp },
    { 0x2321, GLYPH_integralbt },
    { 0x2329, GLYPH_angleleft },
    { 0x232a, GLYPH_angleright },
    { 0x2500, GLYPH_SF100000 },
    { 0x2502, GLYPH_SF110000 },
    { 0x250c, GLYPH_SF010000 },
    { 0x2510, GLYPH_SF030000 },
    { 0x2514, GLYPH_SF020000 },
    { 0x2518, GLYPH_SF040000 },
    { 0x251c, GLYPH_SF080000 },
    { 0x2524, GLYPH_SF090000 },
    { 0x252c, GLYPH_SF060000 },
    { 0x2534, GLYPH_SF070000 },
    { 0x253c, GLYPH_SF050000 },
    { 0x2550, GLYPH_SF430000 },
    { 0x2551, GLYPH_SF240000 },
    { 0x2552, GLYPH_SF510000 },
    { 0x2553, GLYPH_SF520000 },
    { 0x2554, GLYPH_SF390000 },
    { 0x2555, GLYPH_SF220000 },
    { 0x2556, GLYPH_SF210000 },
    { 0x2557, GLYPH_SF250000 },
    { 0x2558, GLYPH_SF500000 },
    { 0x2559, GLYPH_SF490000 },
    { 0x255a, GLYPH_SF380000 },
    { 0x255b, GLYPH_SF280000 },
    { 0x255c, GLYPH_SF270000 },
    { 0x255d, GLYPH_SF260000 },
    { 0x255e, GLYPH_SF360000 },
    { 0x255f, GLYPH_SF370000 },
    { 0x2560, GLYPH_SF420000 },
    { 0x2561, GLYPH_SF190000 },
    { 0x2562, GLYPH_SF200000 },
    { 0x2563, GLYPH_SF230000 },
    { 0x2564, GLYPH_SF470000 },
    { 0x2565, GLYPH_SF480000 },
    { 0x2566, GLYPH_SF410000 },
    { 0x2567, GLYPH_SF450000 },
    { 0x2568, GLYPH_SF460000 },
    { 0x2569, GLYPH_SF400000 },
    { 0x256a, GLYPH_SF540000 },
    { 0x256b, GLYPH_SF530000 },
    { 0x256c, GLYPH_SF440000 },
    { 0x2580, GLYPH_upblock },
    { 0x2584, GLYPH_dnblock },
    { 0x2588, GLYPH_block },
    { 0x258c, GLYPH_lfblock },
    { 0x2590, GLYPH_rtblock },
    { 0x2591, GLYPH_ltshade },
    { 0x2592, GLYPH_shade },
    { 0x2593, GLYPH_dkshade },
    { 0x25a0, GLYPH_filledbox },
    { 0x25a1, GLYPH_H22073 },
    { 0x25aa, GLYPH_H18543 },
    { 0x25ab, GLYPH_H18551 },
    { 0x25ac, GLYPH_filledrect },
    { 0x25b2, GLYPH_triagup },
    { 0x25ba, GLYPH_triagrt },
    { 0x25bc, GLYPH_triagdn },
    { 0x25c4, GLYPH_triaglf },
    { 0x25ca, GLYPH_lozenge },
    { 0x25cb, GLYPH_circle },
    { 0x25cf, GLYPH_H18533 },
    { 0x25d8, GLYPH_invbullet },
    { 0x25d9, GLYPH_invcircle },
    { 0x25e6, GLYPH_openbullet },
    { 0x263a, GLYPH_smileface },
    { 0x263b, GLYPH_invsmileface },
    { 0x263c, GLYPH_sun },
    { 0x2640, GLYPH_female },
    { 0x2642, GLYPH_male },
    { 0x2660, GLYPH_spade },
    { 0x2663, GLYPH_club },
    { 0x2665, GLYPH_heart },
    { 0x2666, GLYPH_diamond },
    { 0x266a, GLYPH_musicalnote },
    { 0x266b, GLYPH_musicalnotedbl },
};

static const UnicodeToGlyphName psGlyphNameToUnicodes[]=
{
    { 0x0041, GLYPH_A, },
    { 0x00c6, GLYPH_AE, },
    { 0x01fc, GLYPH_AEacute, },
    { 0x01e2, GLYPH_AEmacron, },
    { 0xf7e6, GLYPH_AEsmall, },
    { 0x00c1, GLYPH_Aacute, },
    { 0xf7e1, GLYPH_Aacutesmall, },
    { 0x0102, GLYPH_Abreve, },
    { 0x1eae, GLYPH_Abreveacute, },
    { 0x04d0, GLYPH_Abrevecyrillic, },
    { 0x1eb6, GLYPH_Abrevedotbelow, },
    { 0x1eb0, GLYPH_Abrevegrave, },
    { 0x1eb2, GLYPH_Abrevehookabove, },
    { 0x1eb4, GLYPH_Abrevetilde, },
    { 0x01cd, GLYPH_Acaron, },
    { 0x24b6, GLYPH_Acircle, },
    { 0x00c2, GLYPH_Acircumflex, },
    { 0x1ea4, GLYPH_Acircumflexacute, },
    { 0x1eac, GLYPH_Acircumflexdotbelow, },
    { 0x1ea6, GLYPH_Acircumflexgrave, },
    { 0x1ea8, GLYPH_Acircumflexhookabove, },
    { 0xf7e2, GLYPH_Acircumflexsmall, },
    { 0x1eaa, GLYPH_Acircumflextilde, },
    { 0xf6c9, GLYPH_Acute, },
    { 0xf7b4, GLYPH_Acutesmall, },
    { 0x0410, GLYPH_Acyrillic, },
    { 0x0200, GLYPH_Adblgrave, },
    { 0x00c4, GLYPH_Adiaeresis, },
    { 0x00c4, GLYPH_Adieresis, },
    { 0x04d2, GLYPH_Adieresiscyrillic, },
    { 0x01de, GLYPH_Adieresismacron, },
    { 0xf7e4, GLYPH_Adieresissmall, },
    { 0x1ea0, GLYPH_Adotbelow, },
    { 0x01e0, GLYPH_Adotmacron, },
    { 0x00c0, GLYPH_Agrave, },
    { 0xf7e0, GLYPH_Agravesmall, },
    { 0x1ea2, GLYPH_Ahookabove, },
    { 0x04d4, GLYPH_Aiecyrillic, },
    { 0x0202, GLYPH_Ainvertedbreve, },
    { 0x0391, GLYPH_Alpha, },
    { 0x0386, GLYPH_Alphatonos, },
    { 0x0100, GLYPH_Amacron, },
    { 0xff21, GLYPH_Amonospace, },
    { 0x0104, GLYPH_Aogonek, },
    { 0x00c5, GLYPH_Aring, },
    { 0x01fa, GLYPH_Aringacute, },
    { 0x1e00, GLYPH_Aringbelow, },
    { 0xf7e5, GLYPH_Aringsmall, },
    { 0xf761, GLYPH_Asmall, },
    { 0x00c3, GLYPH_Atilde, },
    { 0xf7e3, GLYPH_Atildesmall, },
    { 0x0531, GLYPH_Aybarmenian, },
    { 0x0042, GLYPH_B, },
    { 0x24b7, GLYPH_Bcircle, },
    { 0x1e02, GLYPH_Bdotaccent, },
    { 0x1e04, GLYPH_Bdotbelow, },
    { 0x0411, GLYPH_Becyrillic, },
    { 0x0532, GLYPH_Benarmenian, },
    { 0x0392, GLYPH_Beta, },
    { 0x0181, GLYPH_Bhook, },
    { 0x1e06, GLYPH_Blinebelow, },
    { 0xff22, GLYPH_Bmonospace, },
    { 0xf6f4, GLYPH_Brevesmall, },
    { 0xf762, GLYPH_Bsmall, },
    { 0x0182, GLYPH_Btopbar, },
    { 0x0043, GLYPH_C, },
    { 0x053e, GLYPH_Caarmenian, },
    { 0x0106, GLYPH_Cacute, },
    { 0xf6ca, GLYPH_Caron, },
    { 0xf6f5, GLYPH_Caronsmall, },
    { 0x010c, GLYPH_Ccaron, },
    { 0x00c7, GLYPH_Ccedilla, },
    { 0x1e08, GLYPH_Ccedillaacute, },
    { 0xf7e7, GLYPH_Ccedillasmall, },
    { 0x24b8, GLYPH_Ccircle, },
    { 0x0108, GLYPH_Ccircumflex, },
    { 0x010a, GLYPH_Cdot, },
    { 0x010a, GLYPH_Cdotaccent, },
    { 0xf7b8, GLYPH_Cedillasmall, },
    { 0x0549, GLYPH_Chaarmenian, },
    { 0x04bc, GLYPH_Cheabkhasiancyrillic, },
    { 0x0427, GLYPH_Checyrillic, },
    { 0x04be, GLYPH_Chedescenderabkhasiancyrillic, },
    { 0x04b6, GLYPH_Chedescendercyrillic, },
    { 0x04f4, GLYPH_Chedieresiscyrillic, },
    { 0x0543, GLYPH_Cheharmenian, },
    { 0x04cb, GLYPH_Chekhakassiancyrillic, },
    { 0x04b8, GLYPH_Cheverticalstrokecyrillic, },
    { 0x03a7, GLYPH_Chi, },
    { 0x0187, GLYPH_Chook, },
    { 0xf6f6, GLYPH_Circumflexsmall, },
    { 0xff23, GLYPH_Cmonospace, },
    { 0x0551, GLYPH_Coarmenian, },
    { 0xf763, GLYPH_Csmall, },
    { 0x0044, GLYPH_D, },
    { 0x01f1, GLYPH_DZ, },
    { 0x01c4, GLYPH_DZcaron, },
    { 0x0534, GLYPH_Daarmenian, },
    { 0x0189, GLYPH_Dafrican, },
    { 0x010e, GLYPH_Dcaron, },
    { 0x1e10, GLYPH_Dcedilla, },
    { 0x24b9, GLYPH_Dcircle, },
    { 0x1e12, GLYPH_Dcircumflexbelow, },
    { 0x0110, GLYPH_Dcroat, },
    { 0x1e0a, GLYPH_Ddotaccent, },
    { 0x1e0c, GLYPH_Ddotbelow, },
    { 0x0414, GLYPH_Decyrillic, },
    { 0x03ee, GLYPH_Deicoptic, },
    { 0x2206, GLYPH_Delta, },
    { 0x0394, GLYPH_Deltagreek, },
    { 0x018a, GLYPH_Dhook, },
    { 0xf6cb, GLYPH_Dieresis, },
    { 0xf6cc, GLYPH_DieresisAcute, },
    { 0xf6cd, GLYPH_DieresisGrave, },
    { 0xf7a8, GLYPH_Dieresissmall, },
    { 0x03dc, GLYPH_Digammagreek, },
    { 0x0402, GLYPH_Djecyrillic, },
    { 0x1e0e, GLYPH_Dlinebelow, },
    { 0x0110, GLYPH_Dmacron, },
    { 0xff24, GLYPH_Dmonospace, },
    { 0xf6f7, GLYPH_Dotaccentsmall, },
    { 0x0110, GLYPH_Dslash, },
    { 0xf764, GLYPH_Dsmall, },
    { 0x0110, GLYPH_Dstroke, },
    { 0x018b, GLYPH_Dtopbar, },
    { 0x01f2, GLYPH_Dz, },
    { 0x01c5, GLYPH_Dzcaron, },
    { 0x04e0, GLYPH_Dzeabkhasiancyrillic, },
    { 0x0405, GLYPH_Dzecyrillic, },
    { 0x040f, GLYPH_Dzhecyrillic, },
    { 0x0045, GLYPH_E, },
    { 0x00c9, GLYPH_Eacute, },
    { 0xf7e9, GLYPH_Eacutesmall, },
    { 0x0114, GLYPH_Ebreve, },
    { 0x011a, GLYPH_Ecaron, },
    { 0x1e1c, GLYPH_Ecedillabreve, },
    { 0x0535, GLYPH_Echarmenian, },
    { 0x24ba, GLYPH_Ecircle, },
    { 0x00ca, GLYPH_Ecircumflex, },
    { 0x1ebe, GLYPH_Ecircumflexacute, },
    { 0x1e18, GLYPH_Ecircumflexbelow, },
    { 0x1ec6, GLYPH_Ecircumflexdotbelow, },
    { 0x1ec0, GLYPH_Ecircumflexgrave, },
    { 0x1ec2, GLYPH_Ecircumflexhookabove, },
    { 0xf7ea, GLYPH_Ecircumflexsmall, },
    { 0x1ec4, GLYPH_Ecircumflextilde, },
    { 0x0404, GLYPH_Ecyrillic, },
    { 0x0204, GLYPH_Edblgrave, },
    { 0x00cb, GLYPH_Ediaeresis, },
    { 0x00cb, GLYPH_Edieresis, },
    { 0xf7eb, GLYPH_Edieresissmall, },
    { 0x0116, GLYPH_Edot, },
    { 0x0116, GLYPH_Edotaccent, },
    { 0x1eb8, GLYPH_Edotbelow, },
    { 0x0424, GLYPH_Efcyrillic, },
    { 0x00c8, GLYPH_Egrave, },
    { 0xf7e8, GLYPH_Egravesmall, },
    { 0x0537, GLYPH_Eharmenian, },
    { 0x1eba, GLYPH_Ehookabove, },
    { 0x2167, GLYPH_Eightroman, },
    { 0x0206, GLYPH_Einvertedbreve, },
    { 0x0464, GLYPH_Eiotifiedcyrillic, },
    { 0x041b, GLYPH_Elcyrillic, },
    { 0x216a, GLYPH_Elevenroman, },
    { 0x0112, GLYPH_Emacron, },
    { 0x1e16, GLYPH_Emacronacute, },
    { 0x1e14, GLYPH_Emacrongrave, },
    { 0x041c, GLYPH_Emcyrillic, },
    { 0xff25, GLYPH_Emonospace, },
    { 0x041d, GLYPH_Encyrillic, },
    { 0x04a2, GLYPH_Endescendercyrillic, },
    { 0x014a, GLYPH_Eng, },
    { 0x04a4, GLYPH_Enghecyrillic, },
    { 0x04c7, GLYPH_Enhookcyrillic, },
    { 0x0118, GLYPH_Eogonek, },
    { 0x0190, GLYPH_Eopen, },
    { 0x0395, GLYPH_Epsilon, },
    { 0x0388, GLYPH_Epsilontonos, },
    { 0x0420, GLYPH_Ercyrillic, },
    { 0x018e, GLYPH_Ereversed, },
    { 0x042d, GLYPH_Ereversedcyrillic, },
    { 0x0421, GLYPH_Escyrillic, },
    { 0x04aa, GLYPH_Esdescendercyrillic, },
    { 0x01a9, GLYPH_Esh, },
    { 0xf765, GLYPH_Esmall, },
    { 0x0397, GLYPH_Eta, },
    { 0x0538, GLYPH_Etarmenian, },
    { 0x0389, GLYPH_Etatonos, },
    { 0x00d0, GLYPH_Eth, },
    { 0xf7f0, GLYPH_Ethsmall, },
    { 0x1ebc, GLYPH_Etilde, },
    { 0x1e1a, GLYPH_Etildebelow, },
    { 0x20ac, GLYPH_Euro, },
    { 0x01b7, GLYPH_Ezh, },
    { 0x01ee, GLYPH_Ezhcaron, },
    { 0x01b8, GLYPH_Ezhreversed, },
    { 0x0046, GLYPH_F, },
    { 0x24bb, GLYPH_Fcircle, },
    { 0x1e1e, GLYPH_Fdotaccent, },
    { 0x0556, GLYPH_Feharmenian, },
    { 0x03e4, GLYPH_Feicoptic, },
    { 0x0191, GLYPH_Fhook, },
    { 0x0472, GLYPH_Fitacyrillic, },
    { 0x2164, GLYPH_Fiveroman, },
    { 0xff26, GLYPH_Fmonospace, },
    { 0x2163, GLYPH_Fourroman, },
    { 0xf766, GLYPH_Fsmall, },
    { 0x0047, GLYPH_G, },
    { 0x3387, GLYPH_GBsquare, },
    { 0x01f4, GLYPH_Gacute, },
    { 0x0393, GLYPH_Gamma, },
    { 0x0194, GLYPH_Gammaafrican, },
    { 0x03ea, GLYPH_Gangiacoptic, },
    { 0x011e, GLYPH_Gbreve, },
    { 0x01e6, GLYPH_Gcaron, },
    { 0x0122, GLYPH_Gcedilla, },
    { 0x24bc, GLYPH_Gcircle, },
    { 0x011c, GLYPH_Gcircumflex, },
    { 0x0122, GLYPH_Gcommaaccent, },
    { 0x0120, GLYPH_Gdot, },
    { 0x0120, GLYPH_Gdotaccent, },
    { 0x0413, GLYPH_Gecyrillic, },
    { 0x0542, GLYPH_Ghadarmenian, },
    { 0x0494, GLYPH_Ghemiddlehookcyrillic, },
    { 0x0492, GLYPH_Ghestrokecyrillic, },
    { 0x0490, GLYPH_Gheupturncyrillic, },
    { 0x0193, GLYPH_Ghook, },
    { 0x0533, GLYPH_Gimarmenian, },
    { 0x0403, GLYPH_Gjecyrillic, },
    { 0x1e20, GLYPH_Gmacron, },
    { 0xff27, GLYPH_Gmonospace, },
    { 0xf6ce, GLYPH_Grave, },
    { 0xf760, GLYPH_Gravesmall, },
    { 0xf767, GLYPH_Gsmall, },
    { 0x029b, GLYPH_Gsmallhook, },
    { 0x01e4, GLYPH_Gstroke, },
    { 0x0048, GLYPH_H, },
    { 0x25cf, GLYPH_H18533, },
    { 0x25aa, GLYPH_H18543, },
    { 0x25ab, GLYPH_H18551, },
    { 0x25a1, GLYPH_H22073, },
    { 0x33cb, GLYPH_HPsquare, },
    { 0x04a8, GLYPH_Haabkhasiancyrillic, },
    { 0x04b2, GLYPH_Hadescendercyrillic, },
    { 0x042a, GLYPH_Hardsigncyrillic, },
    { 0x0126, GLYPH_Hbar, },
    { 0x1e2a, GLYPH_Hbrevebelow, },
    { 0x1e28, GLYPH_Hcedilla, },
    { 0x24bd, GLYPH_Hcircle, },
    { 0x0124, GLYPH_Hcircumflex, },
    { 0x1e26, GLYPH_Hdieresis, },
    { 0x1e22, GLYPH_Hdotaccent, },
    { 0x1e24, GLYPH_Hdotbelow, },
    { 0xff28, GLYPH_Hmonospace, },
    { 0x0540, GLYPH_Hoarmenian, },
    { 0x03e8, GLYPH_Horicoptic, },
    { 0xf768, GLYPH_Hsmall, },
    { 0xf6cf, GLYPH_Hungarumlaut, },
    { 0xf6f8, GLYPH_Hungarumlautsmall, },
    { 0x3390, GLYPH_Hzsquare, },
    { 0x0049, GLYPH_I, },
    { 0x042f, GLYPH_IAcyrillic, },
    { 0x0132, GLYPH_IJ, },
    { 0x042e, GLYPH_IUcyrillic, },
    { 0x00cd, GLYPH_Iacute, },
    { 0xf7ed, GLYPH_Iacutesmall, },
    { 0x012c, GLYPH_Ibreve, },
    { 0x01cf, GLYPH_Icaron, },
    { 0x24be, GLYPH_Icircle, },
    { 0x00ce, GLYPH_Icircumflex, },
    { 0xf7ee, GLYPH_Icircumflexsmall, },
    { 0x0406, GLYPH_Icyrillic, },
    { 0x0208, GLYPH_Idblgrave, },
    { 0x00cf, GLYPH_Idieresis, },
    { 0x1e2e, GLYPH_Idieresisacute, },
    { 0x04e4, GLYPH_Idieresiscyrillic, },
    { 0xf7ef, GLYPH_Idieresissmall, },
    { 0x0130, GLYPH_Idot, },
    { 0x0130, GLYPH_Idotaccent, },
    { 0x1eca, GLYPH_Idotbelow, },
    { 0x04d6, GLYPH_Iebrevecyrillic, },
    { 0x0415, GLYPH_Iecyrillic, },
    { 0x2111, GLYPH_Ifraktur, },
    { 0x00cc, GLYPH_Igrave, },
    { 0xf7ec, GLYPH_Igravesmall, },
    { 0x1ec8, GLYPH_Ihookabove, },
    { 0x0418, GLYPH_Iicyrillic, },
    { 0x020a, GLYPH_Iinvertedbreve, },
    { 0x0419, GLYPH_Iishortcyrillic, },
    { 0x012a, GLYPH_Imacron, },
    { 0x04e2, GLYPH_Imacroncyrillic, },
    { 0xff29, GLYPH_Imonospace, },
    { 0x053b, GLYPH_Iniarmenian, },
    { 0x0401, GLYPH_Iocyrillic, },
    { 0x012e, GLYPH_Iogonek, },
    { 0x0399, GLYPH_Iota, },
    { 0x0196, GLYPH_Iotaafrican, },
    { 0x03aa, GLYPH_Iotadieresis, },
    { 0x038a, GLYPH_Iotatonos, },
    { 0xf769, GLYPH_Ismall, },
    { 0x0197, GLYPH_Istroke, },
    { 0x0128, GLYPH_Itilde, },
    { 0x1e2c, GLYPH_Itildebelow, },
    { 0x0474, GLYPH_Izhitsacyrillic, },
    { 0x0476, GLYPH_Izhitsadblgravecyrillic, },
    { 0x004a, GLYPH_J, },
    { 0x0541, GLYPH_Jaarmenian, },
    { 0x24bf, GLYPH_Jcircle, },
    { 0x0134, GLYPH_Jcircumflex, },
    { 0x0408, GLYPH_Jecyrillic, },
    { 0x054b, GLYPH_Jheharmenian, },
    { 0xff2a, GLYPH_Jmonospace, },
    { 0xf76a, GLYPH_Jsmall, },
    { 0x004b, GLYPH_K, },
    { 0x3385, GLYPH_KBsquare, },
    { 0x33cd, GLYPH_KKsquare, },
    { 0x04a0, GLYPH_Kabashkircyrillic, },
    { 0x1e30, GLYPH_Kacute, },
    { 0x041a, GLYPH_Kacyrillic, },
    { 0x049a, GLYPH_Kadescendercyrillic, },
    { 0x04c3, GLYPH_Kahookcyrillic, },
    { 0x039a, GLYPH_Kappa, },
    { 0x049e, GLYPH_Kastrokecyrillic, },
    { 0x049c, GLYPH_Kaverticalstrokecyrillic, },
    { 0x01e8, GLYPH_Kcaron, },
    { 0x0136, GLYPH_Kcedilla, },
    { 0x24c0, GLYPH_Kcircle, },
    { 0x0136, GLYPH_Kcommaaccent, },
    { 0x1e32, GLYPH_Kdotbelow, },
    { 0x0554, GLYPH_Keharmenian, },
    { 0x053f, GLYPH_Kenarmenian, },
    { 0x0425, GLYPH_Khacyrillic, },
    { 0x03e6, GLYPH_Kheicoptic, },
    { 0x0198, GLYPH_Khook, },
    { 0x040c, GLYPH_Kjecyrillic, },
    { 0x1e34, GLYPH_Klinebelow, },
    { 0xff2b, GLYPH_Kmonospace, },
    { 0x0480, GLYPH_Koppacyrillic, },
    { 0x03de, GLYPH_Koppagreek, },
    { 0x046e, GLYPH_Ksicyrillic, },
    { 0xf76b, GLYPH_Ksmall, },
    { 0x004c, GLYPH_L, },
    { 0x01c7, GLYPH_LJ, },
    { 0xf6bf, GLYPH_LL, },
    { 0x0139, GLYPH_Lacute, },
    { 0x039b, GLYPH_Lambda, },
    { 0x013d, GLYPH_Lcaron, },
    { 0x013b, GLYPH_Lcedilla, },
    { 0x24c1, GLYPH_Lcircle, },
    { 0x1e3c, GLYPH_Lcircumflexbelow, },
    { 0x013b, GLYPH_Lcommaaccent, },
    { 0x013f, GLYPH_Ldot, },
    { 0x013f, GLYPH_Ldotaccent, },
    { 0x1e36, GLYPH_Ldotbelow, },
    { 0x1e38, GLYPH_Ldotbelowmacron, },
    { 0x053c, GLYPH_Liwnarmenian, },
    { 0x01c8, GLYPH_Lj, },
    { 0x0409, GLYPH_Ljecyrillic, },
    { 0x1e3a, GLYPH_Llinebelow, },
    { 0xff2c, GLYPH_Lmonospace, },
    { 0x0141, GLYPH_Lslash, },
    { 0xf6f9, GLYPH_Lslashsmall, },
    { 0xf76c, GLYPH_Lsmall, },
    { 0x0141, GLYPH_Lstroke, },
    { 0x004d, GLYPH_M, },
    { 0x3386, GLYPH_MBsquare, },
    { 0xf6d0, GLYPH_Macron, },
    { 0xf7af, GLYPH_Macronsmall, },
    { 0x1e3e, GLYPH_Macute, },
    { 0x24c2, GLYPH_Mcircle, },
    { 0x1e40, GLYPH_Mdotaccent, },
    { 0x1e42, GLYPH_Mdotbelow, },
    { 0x0544, GLYPH_Menarmenian, },
    { 0xff2d, GLYPH_Mmonospace, },
    { 0xf76d, GLYPH_Msmall, },
    { 0x019c, GLYPH_Mturned, },
    { 0x039c, GLYPH_Mu, },
    { 0x004e, GLYPH_N, },
    { 0x01ca, GLYPH_NJ, },
    { 0x0143, GLYPH_Nacute, },
    { 0x0147, GLYPH_Ncaron, },
    { 0x0145, GLYPH_Ncedilla, },
    { 0x24c3, GLYPH_Ncircle, },
    { 0x1e4a, GLYPH_Ncircumflexbelow, },
    { 0x0145, GLYPH_Ncommaaccent, },
    { 0x1e44, GLYPH_Ndotaccent, },
    { 0x1e46, GLYPH_Ndotbelow, },
    { 0x019d, GLYPH_Nhookleft, },
    { 0x2168, GLYPH_Nineroman, },
    { 0x01cb, GLYPH_Nj, },
    { 0x040a, GLYPH_Njecyrillic, },
    { 0x1e48, GLYPH_Nlinebelow, },
    { 0xff2e, GLYPH_Nmonospace, },
    { 0x0546, GLYPH_Nowarmenian, },
    { 0xf76e, GLYPH_Nsmall, },
    { 0x00d1, GLYPH_Ntilde, },
    { 0xf7f1, GLYPH_Ntildesmall, },
    { 0x039d, GLYPH_Nu, },
    { 0x004f, GLYPH_O, },
    { 0x0152, GLYPH_OE, },
    { 0xf6fa, GLYPH_OEsmall, },
    { 0x00d3, GLYPH_Oacute, },
    { 0xf7f3, GLYPH_Oacutesmall, },
    { 0x04e8, GLYPH_Obarredcyrillic, },
    { 0x04ea, GLYPH_Obarreddieresiscyrillic, },
    { 0x014e, GLYPH_Obreve, },
    { 0x01d1, GLYPH_Ocaron, },
    { 0x019f, GLYPH_Ocenteredtilde, },
    { 0x24c4, GLYPH_Ocircle, },
    { 0x00d4, GLYPH_Ocircumflex, },
    { 0x1ed0, GLYPH_Ocircumflexacute, },
    { 0x1ed8, GLYPH_Ocircumflexdotbelow, },
    { 0x1ed2, GLYPH_Ocircumflexgrave, },
    { 0x1ed4, GLYPH_Ocircumflexhookabove, },
    { 0xf7f4, GLYPH_Ocircumflexsmall, },
    { 0x1ed6, GLYPH_Ocircumflextilde, },
    { 0x041e, GLYPH_Ocyrillic, },
    { 0x0150, GLYPH_Odblacute, },
    { 0x020c, GLYPH_Odblgrave, },
    { 0x00d6, GLYPH_Odiaeresis, },
    { 0x00d6, GLYPH_Odieresis, },
    { 0x04e6, GLYPH_Odieresiscyrillic, },
    { 0xf7f6, GLYPH_Odieresissmall, },
    { 0x1ecc, GLYPH_Odotbelow, },
    { 0x0150, GLYPH_Odoubleacute, },
    { 0xf6fb, GLYPH_Ogoneksmall, },
    { 0x00d2, GLYPH_Ograve, },
    { 0xf7f2, GLYPH_Ogravesmall, },
    { 0x0555, GLYPH_Oharmenian, },
    { 0x2126, GLYPH_Ohm, },
    { 0x1ece, GLYPH_Ohookabove, },
    { 0x01a0, GLYPH_Ohorn, },
    { 0x1eda, GLYPH_Ohornacute, },
    { 0x1ee2, GLYPH_Ohorndotbelow, },
    { 0x1edc, GLYPH_Ohorngrave, },
    { 0x1ede, GLYPH_Ohornhookabove, },
    { 0x1ee0, GLYPH_Ohorntilde, },
    { 0x0150, GLYPH_Ohungar, },
    { 0x0150, GLYPH_Ohungarumlaut, },
    { 0x01a2, GLYPH_Oi, },
    { 0x020e, GLYPH_Oinvertedbreve, },
    { 0x014c, GLYPH_Omacron, },
    { 0x1e52, GLYPH_Omacronacute, },
    { 0x1e50, GLYPH_Omacrongrave, },
    { 0x2126, GLYPH_Omega, },
    { 0x0460, GLYPH_Omegacyrillic, },
    { 0x03a9, GLYPH_Omegagreek, },
    { 0x047a, GLYPH_Omegaroundcyrillic, },
    { 0x047c, GLYPH_Omegatitlocyrillic, },
    { 0x038f, GLYPH_Omegatonos, },
    { 0x039f, GLYPH_Omicron, },
    { 0x038c, GLYPH_Omicrontonos, },
    { 0xff2f, GLYPH_Omonospace, },
    { 0x2160, GLYPH_Oneroman, },
    { 0x01ea, GLYPH_Oogonek, },
    { 0x01ec, GLYPH_Oogonekmacron, },
    { 0x0186, GLYPH_Oopen, },
    { 0x00d8, GLYPH_Oslash, },
    { 0x01fe, GLYPH_Oslashacute, },
    { 0xf7f8, GLYPH_Oslashsmall, },
    { 0xf76f, GLYPH_Osmall, },
    { 0x01fe, GLYPH_Ostrokeacute, },
    { 0x047e, GLYPH_Otcyrillic, },
    { 0x00d5, GLYPH_Otilde, },
    { 0x1e4c, GLYPH_Otildeacute, },
    { 0x1e4e, GLYPH_Otildedieresis, },
    { 0xf7f5, GLYPH_Otildesmall, },
    { 0x0050, GLYPH_P, },
    { 0x1e54, GLYPH_Pacute, },
    { 0x24c5, GLYPH_Pcircle, },
    { 0x1e56, GLYPH_Pdotaccent, },
    { 0x041f, GLYPH_Pecyrillic, },
    { 0x054a, GLYPH_Peharmenian, },
    { 0x04a6, GLYPH_Pemiddlehookcyrillic, },
    { 0x03a6, GLYPH_Phi, },
    { 0x01a4, GLYPH_Phook, },
    { 0x03a0, GLYPH_Pi, },
    { 0x0553, GLYPH_Piwrarmenian, },
    { 0xff30, GLYPH_Pmonospace, },
    { 0x03a8, GLYPH_Psi, },
    { 0x0470, GLYPH_Psicyrillic, },
    { 0xf770, GLYPH_Psmall, },
    { 0x0051, GLYPH_Q, },
    { 0x24c6, GLYPH_Qcircle, },
    { 0xff31, GLYPH_Qmonospace, },
    { 0xf771, GLYPH_Qsmall, },
    { 0x0052, GLYPH_R, },
    { 0x054c, GLYPH_Raarmenian, },
    { 0x0154, GLYPH_Racute, },
    { 0x0158, GLYPH_Rcaron, },
    { 0x0156, GLYPH_Rcedilla, },
    { 0x24c7, GLYPH_Rcircle, },
    { 0x0156, GLYPH_Rcommaaccent, },
    { 0x0210, GLYPH_Rdblgrave, },
    { 0x1e58, GLYPH_Rdotaccent, },
    { 0x1e5a, GLYPH_Rdotbelow, },
    { 0x1e5c, GLYPH_Rdotbelowmacron, },
    { 0x0550, GLYPH_Reharmenian, },
    { 0x211c, GLYPH_Rfraktur, },
    { 0x03a1, GLYPH_Rho, },
    { 0xf6fc, GLYPH_Ringsmall, },
    { 0x0212, GLYPH_Rinvertedbreve, },
    { 0x1e5e, GLYPH_Rlinebelow, },
    { 0xff32, GLYPH_Rmonospace, },
    { 0xf772, GLYPH_Rsmall, },
    { 0x0281, GLYPH_Rsmallinverted, },
    { 0x02b6, GLYPH_Rsmallinvertedsuperior, },
    { 0x0053, GLYPH_S, },
    { 0x250c, GLYPH_SF010000, },
    { 0x2514, GLYPH_SF020000, },
    { 0x2510, GLYPH_SF030000, },
    { 0x2518, GLYPH_SF040000, },
    { 0x253c, GLYPH_SF050000, },
    { 0x252c, GLYPH_SF060000, },
    { 0x2534, GLYPH_SF070000, },
    { 0x251c, GLYPH_SF080000, },
    { 0x2524, GLYPH_SF090000, },
    { 0x2500, GLYPH_SF100000, },
    { 0x2502, GLYPH_SF110000, },
    { 0x2561, GLYPH_SF190000, },
    { 0x2562, GLYPH_SF200000, },
    { 0x2556, GLYPH_SF210000, },
    { 0x2555, GLYPH_SF220000, },
    { 0x2563, GLYPH_SF230000, },
    { 0x2551, GLYPH_SF240000, },
    { 0x2557, GLYPH_SF250000, },
    { 0x255d, GLYPH_SF260000, },
    { 0x255c, GLYPH_SF270000, },
    { 0x255b, GLYPH_SF280000, },
    { 0x255e, GLYPH_SF360000, },
    { 0x255f, GLYPH_SF370000, },
    { 0x255a, GLYPH_SF380000, },
    { 0x2554, GLYPH_SF390000, },
    { 0x2569, GLYPH_SF400000, },
    { 0x2566, GLYPH_SF410000, },
    { 0x2560, GLYPH_SF420000, },
    { 0x2550, GLYPH_SF430000, },
    { 0x256c, GLYPH_SF440000, },
    { 0x2567, GLYPH_SF450000, },
    { 0x2568, GLYPH_SF460000, },
    { 0x2564, GLYPH_SF470000, },
    { 0x2565, GLYPH_SF480000, },
    { 0x2559, GLYPH_SF490000, },
    { 0x2558, GLYPH_SF500000, },
    { 0x2552, GLYPH_SF510000, },
    { 0x2553, GLYPH_SF520000, },
    { 0x256b, GLYPH_SF530000, },
    { 0x256a, GLYPH_SF540000, },
    { 0x015a, GLYPH_Sacute, },
    { 0x1e64, GLYPH_Sacutedotaccent, },
    { 0x03e0, GLYPH_Sampigreek, },
    { 0x0160, GLYPH_Scaron, },
    { 0x1e66, GLYPH_Scarondotaccent, },
    { 0xf6fd, GLYPH_Scaronsmall, },
    { 0x015e, GLYPH_Scedilla, },
    { 0x015e, GLYPH_Scedilla1, },
    { 0x018f, GLYPH_Schwa, },
    { 0x04d8, GLYPH_Schwacyrillic, },
    { 0x04da, GLYPH_Schwadieresiscyrillic, },
    { 0x24c8, GLYPH_Scircle, },
    { 0x015c, GLYPH_Scircumflex, },
    { 0x0218, GLYPH_Scommaaccent, },
    { 0x1e60, GLYPH_Sdotaccent, },
    { 0x1e62, GLYPH_Sdotbelow, },
    { 0x1e68, GLYPH_Sdotbelowdotaccent, },
    { 0x054d, GLYPH_Seharmenian, },
    { 0x2166, GLYPH_Sevenroman, },
    { 0x0547, GLYPH_Shaarmenian, },
    { 0x0428, GLYPH_Shacyrillic, },
    { 0x0429, GLYPH_Shchacyrillic, },
    { 0x03e2, GLYPH_Sheicoptic, },
    { 0x04ba, GLYPH_Shhacyrillic, },
    { 0x03ec, GLYPH_Shimacoptic, },
    { 0x03a3, GLYPH_Sigma, },
    { 0x2165, GLYPH_Sixroman, },
    { 0xff33, GLYPH_Smonospace, },
    { 0x042c, GLYPH_Softsigncyrillic, },
    { 0xf773, GLYPH_Ssmall, },
    { 0x03da, GLYPH_Stigmagreek, },
    { 0x0054, GLYPH_T, },
    { 0x03a4, GLYPH_Tau, },
    { 0x0166, GLYPH_Tbar, },
    { 0x0164, GLYPH_Tcaron, },
    { 0x0162, GLYPH_Tcedilla, },
    { 0x0162, GLYPH_Tcedilla1, },
    { 0x24c9, GLYPH_Tcircle, },
    { 0x1e70, GLYPH_Tcircumflexbelow, },
    { 0x0162, GLYPH_Tcommaaccent, },
    { 0x1e6a, GLYPH_Tdotaccent, },
    { 0x1e6c, GLYPH_Tdotbelow, },
    { 0x0422, GLYPH_Tecyrillic, },
    { 0x04ac, GLYPH_Tedescendercyrillic, },
    { 0x2169, GLYPH_Tenroman, },
    { 0x04b4, GLYPH_Tetsecyrillic, },
    { 0x0398, GLYPH_Theta, },
    { 0x01ac, GLYPH_Thook, },
    { 0x00de, GLYPH_Thorn, },
    { 0xf7fe, GLYPH_Thornsmall, },
    { 0x2162, GLYPH_Threeroman, },
    { 0xf6fe, GLYPH_Tildesmall, },
    { 0x054f, GLYPH_Tiwnarmenian, },
    { 0x1e6e, GLYPH_Tlinebelow, },
    { 0xff34, GLYPH_Tmonospace, },
    { 0x0539, GLYPH_Toarmenian, },
    { 0x01bc, GLYPH_Tonefive, },
    { 0x0184, GLYPH_Tonesix, },
    { 0x01a7, GLYPH_Tonetwo, },
    { 0x01ae, GLYPH_Tretroflexhook, },
    { 0x0426, GLYPH_Tsecyrillic, },
    { 0x040b, GLYPH_Tshecyrillic, },
    { 0xf774, GLYPH_Tsmall, },
    { 0x216b, GLYPH_Twelveroman, },
    { 0x2161, GLYPH_Tworoman, },
    { 0x0055, GLYPH_U, },
    { 0x00da, GLYPH_Uacute, },
    { 0xf7fa, GLYPH_Uacutesmall, },
    { 0x016c, GLYPH_Ubreve, },
    { 0x01d3, GLYPH_Ucaron, },
    { 0x24ca, GLYPH_Ucircle, },
    { 0x00db, GLYPH_Ucircumflex, },
    { 0x1e76, GLYPH_Ucircumflexbelow, },
    { 0xf7fb, GLYPH_Ucircumflexsmall, },
    { 0x0423, GLYPH_Ucyrillic, },
    { 0x0170, GLYPH_Udblacute, },
    { 0x0214, GLYPH_Udblgrave, },
    { 0x00dc, GLYPH_Udiaeresis, },
    { 0x00dc, GLYPH_Udieresis, },
    { 0x01d7, GLYPH_Udieresisacute, },
    { 0x1e72, GLYPH_Udieresisbelow, },
    { 0x01d9, GLYPH_Udieresiscaron, },
    { 0x04f0, GLYPH_Udieresiscyrillic, },
    { 0x01db, GLYPH_Udieresisgrave, },
    { 0x01d5, GLYPH_Udieresismacron, },
    { 0xf7fc, GLYPH_Udieresissmall, },
    { 0x1ee4, GLYPH_Udotbelow, },
    { 0x0170, GLYPH_Udoubleacute, },
    { 0x00d9, GLYPH_Ugrave, },
    { 0xf7f9, GLYPH_Ugravesmall, },
    { 0x1ee6, GLYPH_Uhookabove, },
    { 0x01af, GLYPH_Uhorn, },
    { 0x1ee8, GLYPH_Uhornacute, },
    { 0x1ef0, GLYPH_Uhorndotbelow, },
    { 0x1eea, GLYPH_Uhorngrave, },
    { 0x1eec, GLYPH_Uhornhookabove, },
    { 0x1eee, GLYPH_Uhorntilde, },
    { 0x0170, GLYPH_Uhungar, },
    { 0x0170, GLYPH_Uhungarumlaut, },
    { 0x04f2, GLYPH_Uhungarumlautcyrillic, },
    { 0x0216, GLYPH_Uinvertedbreve, },
    { 0x0478, GLYPH_Ukcyrillic, },
    { 0x016a, GLYPH_Umacron, },
    { 0x04ee, GLYPH_Umacroncyrillic, },
    { 0x1e7a, GLYPH_Umacrondieresis, },
    { 0xff35, GLYPH_Umonospace, },
    { 0x0172, GLYPH_Uogonek, },
    { 0x03a5, GLYPH_Upsilon, },
    { 0x03d2, GLYPH_Upsilon1, },
    { 0x03d3, GLYPH_Upsilonacutehooksymbolgreek, },
    { 0x01b1, GLYPH_Upsilonafrican, },
    { 0x03ab, GLYPH_Upsilondieresis, },
    { 0x03d4, GLYPH_Upsilondieresishooksymbolgreek, },
    { 0x03d2, GLYPH_Upsilonhooksymbol, },
    { 0x038e, GLYPH_Upsilontonos, },
    { 0x016e, GLYPH_Uring, },
    { 0x040e, GLYPH_Ushortcyrillic, },
    { 0xf775, GLYPH_Usmall, },
    { 0x04ae, GLYPH_Ustraightcyrillic, },
    { 0x04b0, GLYPH_Ustraightstrokecyrillic, },
    { 0x0168, GLYPH_Utilde, },
    { 0x1e78, GLYPH_Utildeacute, },
    { 0x1e74, GLYPH_Utildebelow, },
    { 0x0056, GLYPH_V, },
    { 0x24cb, GLYPH_Vcircle, },
    { 0x1e7e, GLYPH_Vdotbelow, },
    { 0x0412, GLYPH_Vecyrillic, },
    { 0x054e, GLYPH_Vewarmenian, },
    { 0x01b2, GLYPH_Vhook, },
    { 0xff36, GLYPH_Vmonospace, },
    { 0x0548, GLYPH_Voarmenian, },
    { 0xf776, GLYPH_Vsmall, },
    { 0x1e7c, GLYPH_Vtilde, },
    { 0x0057, GLYPH_W, },
    { 0x1e82, GLYPH_Wacute, },
    { 0x24cc, GLYPH_Wcircle, },
    { 0x0174, GLYPH_Wcircumflex, },
    { 0x1e84, GLYPH_Wdieresis, },
    { 0x1e86, GLYPH_Wdotaccent, },
    { 0x1e88, GLYPH_Wdotbelow, },
    { 0x1e80, GLYPH_Wgrave, },
    { 0xff37, GLYPH_Wmonospace, },
    { 0xf777, GLYPH_Wsmall, },
    { 0x0058, GLYPH_X, },
    { 0x24cd, GLYPH_Xcircle, },
    { 0x1e8c, GLYPH_Xdieresis, },
    { 0x1e8a, GLYPH_Xdotaccent, },
    { 0x053d, GLYPH_Xeharmenian, },
    { 0x039e, GLYPH_Xi, },
    { 0xff38, GLYPH_Xmonospace, },
    { 0xf778, GLYPH_Xsmall, },
    { 0x0059, GLYPH_Y, },
    { 0x00dd, GLYPH_Yacute, },
    { 0xf7fd, GLYPH_Yacutesmall, },
    { 0x0462, GLYPH_Yatcyrillic, },
    { 0x24ce, GLYPH_Ycircle, },
    { 0x0176, GLYPH_Ycircumflex, },
    { 0x0178, GLYPH_Ydieresis, },
    { 0xf7ff, GLYPH_Ydieresissmall, },
    { 0x1e8e, GLYPH_Ydotaccent, },
    { 0x1ef4, GLYPH_Ydotbelow, },
    { 0x042b, GLYPH_Yericyrillic, },
    { 0x04f8, GLYPH_Yerudieresiscyrillic, },
    { 0x1ef2, GLYPH_Ygrave, },
    { 0x01b3, GLYPH_Yhook, },
    { 0x1ef6, GLYPH_Yhookabove, },
    { 0x0545, GLYPH_Yiarmenian, },
    { 0x0407, GLYPH_Yicyrillic, },
    { 0x0552, GLYPH_Yiwnarmenian, },
    { 0xff39, GLYPH_Ymonospace, },
    { 0xf779, GLYPH_Ysmall, },
    { 0x1ef8, GLYPH_Ytilde, },
    { 0x046a, GLYPH_Yusbigcyrillic, },
    { 0x046c, GLYPH_Yusbigiotifiedcyrillic, },
    { 0x0466, GLYPH_Yuslittlecyrillic, },
    { 0x0468, GLYPH_Yuslittleiotifiedcyrillic, },
    { 0x005a, GLYPH_Z, },
    { 0x0536, GLYPH_Zaarmenian, },
    { 0x017b, GLYPH_Zabovedot, },
    { 0x0179, GLYPH_Zacute, },
    { 0x017d, GLYPH_Zcaron, },
    { 0xf6ff, GLYPH_Zcaronsmall, },
    { 0x24cf, GLYPH_Zcircle, },
    { 0x1e90, GLYPH_Zcircumflex, },
    { 0x017b, GLYPH_Zdot, },
    { 0x017b, GLYPH_Zdotaccent, },
    { 0x1e92, GLYPH_Zdotbelow, },
    { 0x0417, GLYPH_Zecyrillic, },
    { 0x0498, GLYPH_Zedescendercyrillic, },
    { 0x04de, GLYPH_Zedieresiscyrillic, },
    { 0x0396, GLYPH_Zeta, },
    { 0x053a, GLYPH_Zhearmenian, },
    { 0x04c1, GLYPH_Zhebrevecyrillic, },
    { 0x0416, GLYPH_Zhecyrillic, },
    { 0x0496, GLYPH_Zhedescendercyrillic, },
    { 0x04dc, GLYPH_Zhedieresiscyrillic, },
    { 0x1e94, GLYPH_Zlinebelow, },
    { 0xff3a, GLYPH_Zmonospace, },
    { 0xf77a, GLYPH_Zsmall, },
    { 0x01b5, GLYPH_Zstroke, },
    { 0x0020, GLYPH__notdef, },
    { 0x0061, GLYPH_a, },
    { 0x2701, GLYPH_a1, },
    { 0x2721, GLYPH_a10, },
    { 0x275e, GLYPH_a100, },
    { 0x2761, GLYPH_a101, },
    { 0x2762, GLYPH_a102, },
    { 0x2763, GLYPH_a103, },
    { 0x2764, GLYPH_a104, },
    { 0x2710, GLYPH_a105, },
    { 0x2765, GLYPH_a105, },
    { 0x2766, GLYPH_a106, },
    { 0x2767, GLYPH_a107, },
    { 0x2660, GLYPH_a109, },
    { 0x261b, GLYPH_a11, },
    { 0x2665, GLYPH_a110, },
    { 0x2666, GLYPH_a111, },
    { 0x2663, GLYPH_a112, },
    { 0x2709, GLYPH_a117, },
    { 0x2708, GLYPH_a118, },
    { 0x2707, GLYPH_a119, },
    { 0x261e, GLYPH_a12, },
    { 0x2460, GLYPH_a120, },
    { 0x2461, GLYPH_a121, },
    { 0x2462, GLYPH_a122, },
    { 0x2463, GLYPH_a123, },
    { 0x2464, GLYPH_a124, },
    { 0x2465, GLYPH_a125, },
    { 0x2466, GLYPH_a126, },
    { 0x2467, GLYPH_a127, },
    { 0x2468, GLYPH_a128, },
    { 0x2469, GLYPH_a129, },
    { 0x270c, GLYPH_a13, },
    { 0x2776, GLYPH_a130, },
    { 0x2777, GLYPH_a131, },
    { 0x2778, GLYPH_a132, },
    { 0x2779, GLYPH_a133, },
    { 0x277a, GLYPH_a134, },
    { 0x277b, GLYPH_a135, },
    { 0x277c, GLYPH_a136, },
    { 0x277d, GLYPH_a137, },
    { 0x277e, GLYPH_a138, },
    { 0x277f, GLYPH_a139, },
    { 0x270d, GLYPH_a14, },
    { 0x2780, GLYPH_a140, },
    { 0x2781, GLYPH_a141, },
    { 0x2782, GLYPH_a142, },
    { 0x2783, GLYPH_a143, },
    { 0x2784, GLYPH_a144, },
    { 0x2785, GLYPH_a145, },
    { 0x2786, GLYPH_a146, },
    { 0x2787, GLYPH_a147, },
    { 0x2788, GLYPH_a148, },
    { 0x2789, GLYPH_a149, },
    { 0x270e, GLYPH_a15, },
    { 0x278a, GLYPH_a150, },
    { 0x278b, GLYPH_a151, },
    { 0x278c, GLYPH_a152, },
    { 0x278d, GLYPH_a153, },
    { 0x278e, GLYPH_a154, },
    { 0x278f, GLYPH_a155, },
    { 0x2790, GLYPH_a156, },
    { 0x2791, GLYPH_a157, },
    { 0x2792, GLYPH_a158, },
    { 0x2793, GLYPH_a159, },
    { 0x270f, GLYPH_a16, },
    { 0x2794, GLYPH_a160, },
    { 0x2192, GLYPH_a161, },
    { 0x27a3, GLYPH_a162, },
    { 0x2194, GLYPH_a163, },
    { 0x2195, GLYPH_a164, },
    { 0x2799, GLYPH_a165, },
    { 0x279b, GLYPH_a166, },
    { 0x279c, GLYPH_a167, },
    { 0x279d, GLYPH_a168, },
    { 0x279e, GLYPH_a169, },
    { 0x2711, GLYPH_a17, },
    { 0x279f, GLYPH_a170, },
    { 0x27a0, GLYPH_a171, },
    { 0x27a1, GLYPH_a172, },
    { 0x27a2, GLYPH_a173, },
    { 0x27a4, GLYPH_a174, },
    { 0x27a5, GLYPH_a175, },
    { 0x27a6, GLYPH_a176, },
    { 0x27a7, GLYPH_a177, },
    { 0x27a8, GLYPH_a178, },
    { 0x27a9, GLYPH_a179, },
    { 0x2712, GLYPH_a18, },
    { 0x27ab, GLYPH_a180, },
    { 0x27ad, GLYPH_a181, },
    { 0x27af, GLYPH_a182, },
    { 0x27b2, GLYPH_a183, },
    { 0x27b3, GLYPH_a184, },
    { 0x27b5, GLYPH_a185, },
    { 0x27b8, GLYPH_a186, },
    { 0x27ba, GLYPH_a187, },
    { 0x27bb, GLYPH_a188, },
    { 0x27bc, GLYPH_a189, },
    { 0x2713, GLYPH_a19, },
    { 0x27bd, GLYPH_a190, },
    { 0x27be, GLYPH_a191, },
    { 0x279a, GLYPH_a192, },
    { 0x27aa, GLYPH_a193, },
    { 0x27b6, GLYPH_a194, },
    { 0x27b9, GLYPH_a195, },
    { 0x2798, GLYPH_a196, },
    { 0x27b4, GLYPH_a197, },
    { 0x27b7, GLYPH_a198, },
    { 0x27ac, GLYPH_a199, },
    { 0x2702, GLYPH_a2, },
    { 0x2714, GLYPH_a20, },
    { 0x27ae, GLYPH_a200, },
    { 0x27b1, GLYPH_a201, },
    { 0x2703, GLYPH_a202, },
    { 0x2750, GLYPH_a203, },
    { 0x2752, GLYPH_a204, },
    { 0x2715, GLYPH_a21, },
    { 0x2716, GLYPH_a22, },
    { 0x2717, GLYPH_a23, },
    { 0x2718, GLYPH_a24, },
    { 0x2719, GLYPH_a25, },
    { 0x271a, GLYPH_a26, },
    { 0x271b, GLYPH_a27, },
    { 0x271c, GLYPH_a28, },
    { 0x2722, GLYPH_a29, },
    { 0x2704, GLYPH_a3, },
    { 0x2723, GLYPH_a30, },
    { 0x2724, GLYPH_a31, },
    { 0x2725, GLYPH_a32, },
    { 0x2726, GLYPH_a33, },
    { 0x2727, GLYPH_a34, },
    { 0x2605, GLYPH_a35, },
    { 0x2729, GLYPH_a36, },
    { 0x272a, GLYPH_a37, },
    { 0x272b, GLYPH_a38, },
    { 0x272c, GLYPH_a39, },
    { 0x260e, GLYPH_a4, },
    { 0x272d, GLYPH_a40, },
    { 0x272e, GLYPH_a41, },
    { 0x272f, GLYPH_a42, },
    { 0x2730, GLYPH_a43, },
    { 0x2731, GLYPH_a44, },
    { 0x2732, GLYPH_a45, },
    { 0x2733, GLYPH_a46, },
    { 0x2734, GLYPH_a47, },
    { 0x2735, GLYPH_a48, },
    { 0x2736, GLYPH_a49, },
    { 0x2706, GLYPH_a5, },
    { 0x2737, GLYPH_a50, },
    { 0x2738, GLYPH_a51, },
    { 0x2739, GLYPH_a52, },
    { 0x273a, GLYPH_a53, },
    { 0x273b, GLYPH_a54, },
    { 0x273c, GLYPH_a55, },
    { 0x273d, GLYPH_a56, },
    { 0x273e, GLYPH_a57, },
    { 0x273f, GLYPH_a58, },
    { 0x2740, GLYPH_a59, },
    { 0x271d, GLYPH_a6, },
    { 0x2741, GLYPH_a60, },
    { 0x2742, GLYPH_a61, },
    { 0x2743, GLYPH_a62, },
    { 0x2744, GLYPH_a63, },
    { 0x2745, GLYPH_a64, },
    { 0x2746, GLYPH_a65, },
    { 0x2747, GLYPH_a66, },
    { 0x2748, GLYPH_a67, },
    { 0x2749, GLYPH_a68, },
    { 0x274a, GLYPH_a69, },
    { 0x271e, GLYPH_a7, },
    { 0x274b, GLYPH_a70, },
    { 0x274d, GLYPH_a72, },
    { 0x25a0, GLYPH_a73, },
    { 0x274f, GLYPH_a74, },
    { 0x2751, GLYPH_a75, },
    { 0x25b2, GLYPH_a76, },
    { 0x25bc, GLYPH_a77, },
    { 0x25c6, GLYPH_a78, },
    { 0x2756, GLYPH_a79, },
    { 0x271f, GLYPH_a8, },
    { 0x2758, GLYPH_a82, },
    { 0x2759, GLYPH_a83, },
    { 0x275a, GLYPH_a84, },
    { 0x2720, GLYPH_a9, },
    { 0x275b, GLYPH_a97, },
    { 0x275c, GLYPH_a98, },
    { 0x275d, GLYPH_a99, },
    { 0x0986, GLYPH_aabengali, },
    { 0x00e1, GLYPH_aacute, },
    { 0x0906, GLYPH_aadeva, },
    { 0x0a86, GLYPH_aagujarati, },
    { 0x0a06, GLYPH_aagurmukhi, },
    { 0x0a3e, GLYPH_aamatragurmukhi, },
    { 0x3303, GLYPH_aarusquare, },
    { 0x09be, GLYPH_aavowelsignbengali, },
    { 0x093e, GLYPH_aavowelsigndeva, },
    { 0x0abe, GLYPH_aavowelsigngujarati, },
    { 0x055f, GLYPH_abbreviationmarkarmenian, },
    { 0x0970, GLYPH_abbreviationsigndeva, },
    { 0x0985, GLYPH_abengali, },
    { 0x311a, GLYPH_abopomofo, },
    { 0x02d9, GLYPH_abovedot, },
    { 0x0103, GLYPH_abreve, },
    { 0x1eaf, GLYPH_abreveacute, },
    { 0x04d1, GLYPH_abrevecyrillic, },
    { 0x1eb7, GLYPH_abrevedotbelow, },
    { 0x1eb1, GLYPH_abrevegrave, },
    { 0x1eb3, GLYPH_abrevehookabove, },
    { 0x1eb5, GLYPH_abrevetilde, },
    { 0x01ce, GLYPH_acaron, },
    { 0x24d0, GLYPH_acircle, },
    { 0x00e2, GLYPH_acircumflex, },
    { 0x1ea5, GLYPH_acircumflexacute, },
    { 0x1ead, GLYPH_acircumflexdotbelow, },
    { 0x1ea7, GLYPH_acircumflexgrave, },
    { 0x1ea9, GLYPH_acircumflexhookabove, },
    { 0x1eab, GLYPH_acircumflextilde, },
    { 0x00b4, GLYPH_acute, },
    { 0x0317, GLYPH_acutebelowcmb, },
    { 0x0301, GLYPH_acutecmb, },
    { 0x0301, GLYPH_acutecomb, },
    { 0x0954, GLYPH_acutedeva, },
    { 0x02cf, GLYPH_acutelowmod, },
    { 0x0341, GLYPH_acutetonecmb, },
    { 0x0430, GLYPH_acyrillic, },
    { 0x0201, GLYPH_adblgrave, },
    { 0x0a71, GLYPH_addakgurmukhi, },
    { 0x0905, GLYPH_adeva, },
    { 0x00e4, GLYPH_adiaeresis, },
    { 0x00e4, GLYPH_adieresis, },
    { 0x04d3, GLYPH_adieresiscyrillic, },
    { 0x01df, GLYPH_adieresismacron, },
    { 0x1ea1, GLYPH_adotbelow, },
    { 0x01e1, GLYPH_adotmacron, },
    { 0x00e6, GLYPH_ae, },
    { 0x01fd, GLYPH_aeacute, },
    { 0x3150, GLYPH_aekorean, },
    { 0x01e3, GLYPH_aemacron, },
    { 0x2015, GLYPH_afii00208, },
    { 0x20a4, GLYPH_afii08941, },
    { 0x0410, GLYPH_afii10017, },
    { 0x0411, GLYPH_afii10018, },
    { 0x0412, GLYPH_afii10019, },
    { 0x0413, GLYPH_afii10020, },
    { 0x0414, GLYPH_afii10021, },
    { 0x0415, GLYPH_afii10022, },
    { 0x0401, GLYPH_afii10023, },
    { 0x0416, GLYPH_afii10024, },
    { 0x0417, GLYPH_afii10025, },
    { 0x0418, GLYPH_afii10026, },
    { 0x0419, GLYPH_afii10027, },
    { 0x041a, GLYPH_afii10028, },
    { 0x041b, GLYPH_afii10029, },
    { 0x041c, GLYPH_afii10030, },
    { 0x041d, GLYPH_afii10031, },
    { 0x041e, GLYPH_afii10032, },
    { 0x041f, GLYPH_afii10033, },
    { 0x0420, GLYPH_afii10034, },
    { 0x0421, GLYPH_afii10035, },
    { 0x0422, GLYPH_afii10036, },
    { 0x0423, GLYPH_afii10037, },
    { 0x0424, GLYPH_afii10038, },
    { 0x0425, GLYPH_afii10039, },
    { 0x0426, GLYPH_afii10040, },
    { 0x0427, GLYPH_afii10041, },
    { 0x0428, GLYPH_afii10042, },
    { 0x0429, GLYPH_afii10043, },
    { 0x042a, GLYPH_afii10044, },
    { 0x042b, GLYPH_afii10045, },
    { 0x042c, GLYPH_afii10046, },
    { 0x042d, GLYPH_afii10047, },
    { 0x042e, GLYPH_afii10048, },
    { 0x042f, GLYPH_afii10049, },
    { 0x0490, GLYPH_afii10050, },
    { 0x0402, GLYPH_afii10051, },
    { 0x0403, GLYPH_afii10052, },
    { 0x0404, GLYPH_afii10053, },
    { 0x0405, GLYPH_afii10054, },
    { 0x0406, GLYPH_afii10055, },
    { 0x0407, GLYPH_afii10056, },
    { 0x0408, GLYPH_afii10057, },
    { 0x0409, GLYPH_afii10058, },
    { 0x040a, GLYPH_afii10059, },
    { 0x040b, GLYPH_afii10060, },
    { 0x040c, GLYPH_afii10061, },
    { 0x040e, GLYPH_afii10062, },
    { 0xf6c4, GLYPH_afii10063, },
    { 0xf6c5, GLYPH_afii10064, },
    { 0x0430, GLYPH_afii10065, },
    { 0x0431, GLYPH_afii10066, },
    { 0x0432, GLYPH_afii10067, },
    { 0x0433, GLYPH_afii10068, },
    { 0x0434, GLYPH_afii10069, },
    { 0x0435, GLYPH_afii10070, },
    { 0x0451, GLYPH_afii10071, },
    { 0x0436, GLYPH_afii10072, },
    { 0x0437, GLYPH_afii10073, },
    { 0x0438, GLYPH_afii10074, },
    { 0x0439, GLYPH_afii10075, },
    { 0x043a, GLYPH_afii10076, },
    { 0x043b, GLYPH_afii10077, },
    { 0x043c, GLYPH_afii10078, },
    { 0x043d, GLYPH_afii10079, },
    { 0x043e, GLYPH_afii10080, },
    { 0x043f, GLYPH_afii10081, },
    { 0x0440, GLYPH_afii10082, },
    { 0x0441, GLYPH_afii10083, },
    { 0x0442, GLYPH_afii10084, },
    { 0x0443, GLYPH_afii10085, },
    { 0x0444, GLYPH_afii10086, },
    { 0x0445, GLYPH_afii10087, },
    { 0x0446, GLYPH_afii10088, },
    { 0x0447, GLYPH_afii10089, },
    { 0x0448, GLYPH_afii10090, },
    { 0x0449, GLYPH_afii10091, },
    { 0x044a, GLYPH_afii10092, },
    { 0x044b, GLYPH_afii10093, },
    { 0x044c, GLYPH_afii10094, },
    { 0x044d, GLYPH_afii10095, },
    { 0x044e, GLYPH_afii10096, },
    { 0x044f, GLYPH_afii10097, },
    { 0x0491, GLYPH_afii10098, },
    { 0x0452, GLYPH_afii10099, },
    { 0x0453, GLYPH_afii10100, },
    { 0x0454, GLYPH_afii10101, },
    { 0x0455, GLYPH_afii10102, },
    { 0x0456, GLYPH_afii10103, },
    { 0x0457, GLYPH_afii10104, },
    { 0x0458, GLYPH_afii10105, },
    { 0x0459, GLYPH_afii10106, },
    { 0x045a, GLYPH_afii10107, },
    { 0x045b, GLYPH_afii10108, },
    { 0x045c, GLYPH_afii10109, },
    { 0x045e, GLYPH_afii10110, },
    { 0x040f, GLYPH_afii10145, },
    { 0x0462, GLYPH_afii10146, },
    { 0x0472, GLYPH_afii10147, },
    { 0x0474, GLYPH_afii10148, },
    { 0xf6c6, GLYPH_afii10192, },
    { 0x045f, GLYPH_afii10193, },
    { 0x0463, GLYPH_afii10194, },
    { 0x0473, GLYPH_afii10195, },
    { 0x0475, GLYPH_afii10196, },
    { 0xf6c7, GLYPH_afii10831, },
    { 0xf6c8, GLYPH_afii10832, },
    { 0x04d9, GLYPH_afii10846, },
    { 0x200e, GLYPH_afii299, },
    { 0x200f, GLYPH_afii300, },
    { 0x200d, GLYPH_afii301, },
    { 0x066a, GLYPH_afii57381, },
    { 0x060c, GLYPH_afii57388, },
    { 0x0660, GLYPH_afii57392, },
    { 0x0661, GLYPH_afii57393, },
    { 0x0662, GLYPH_afii57394, },
    { 0x0663, GLYPH_afii57395, },
    { 0x0664, GLYPH_afii57396, },
    { 0x0665, GLYPH_afii57397, },
    { 0x0666, GLYPH_afii57398, },
    { 0x0667, GLYPH_afii57399, },
    { 0x0668, GLYPH_afii57400, },
    { 0x0669, GLYPH_afii57401, },
    { 0x061b, GLYPH_afii57403, },
    { 0x061f, GLYPH_afii57407, },
    { 0x0621, GLYPH_afii57409, },
    { 0x0622, GLYPH_afii57410, },
    { 0x0623, GLYPH_afii57411, },
    { 0x0624, GLYPH_afii57412, },
    { 0x0625, GLYPH_afii57413, },
    { 0x0626, GLYPH_afii57414, },
    { 0x0627, GLYPH_afii57415, },
    { 0x0628, GLYPH_afii57416, },
    { 0x0629, GLYPH_afii57417, },
    { 0x062a, GLYPH_afii57418, },
    { 0x062b, GLYPH_afii57419, },
    { 0x062c, GLYPH_afii57420, },
    { 0x062d, GLYPH_afii57421, },
    { 0x062e, GLYPH_afii57422, },
    { 0x062f, GLYPH_afii57423, },
    { 0x0630, GLYPH_afii57424, },
    { 0x0631, GLYPH_afii57425, },
    { 0x0632, GLYPH_afii57426, },
    { 0x0633, GLYPH_afii57427, },
    { 0x0634, GLYPH_afii57428, },
    { 0x0635, GLYPH_afii57429, },
    { 0x0636, GLYPH_afii57430, },
    { 0x0637, GLYPH_afii57431, },
    { 0x0638, GLYPH_afii57432, },
    { 0x0639, GLYPH_afii57433, },
    { 0x063a, GLYPH_afii57434, },
    { 0x0640, GLYPH_afii57440, },
    { 0x0641, GLYPH_afii57441, },
    { 0x0642, GLYPH_afii57442, },
    { 0x0643, GLYPH_afii57443, },
    { 0x0644, GLYPH_afii57444, },
    { 0x0645, GLYPH_afii57445, },
    { 0x0646, GLYPH_afii57446, },
    { 0x0648, GLYPH_afii57448, },
    { 0x0649, GLYPH_afii57449, },
    { 0x064a, GLYPH_afii57450, },
    { 0x064b, GLYPH_afii57451, },
    { 0x064c, GLYPH_afii57452, },
    { 0x064d, GLYPH_afii57453, },
    { 0x064e, GLYPH_afii57454, },
    { 0x064f, GLYPH_afii57455, },
    { 0x0650, GLYPH_afii57456, },
    { 0x0651, GLYPH_afii57457, },
    { 0x0652, GLYPH_afii57458, },
    { 0x0647, GLYPH_afii57470, },
    { 0x06a4, GLYPH_afii57505, },
    { 0x067e, GLYPH_afii57506, },
    { 0x0686, GLYPH_afii57507, },
    { 0x0698, GLYPH_afii57508, },
    { 0x06af, GLYPH_afii57509, },
    { 0x0679, GLYPH_afii57511, },
    { 0x0688, GLYPH_afii57512, },
    { 0x0691, GLYPH_afii57513, },
    { 0x06ba, GLYPH_afii57514, },
    { 0x06d2, GLYPH_afii57519, },
    { 0x06d5, GLYPH_afii57534, },
    { 0x20aa, GLYPH_afii57636, },
    { 0x05be, GLYPH_afii57645, },
    { 0x05c3, GLYPH_afii57658, },
    { 0x05d0, GLYPH_afii57664, },
    { 0x05d1, GLYPH_afii57665, },
    { 0x05d2, GLYPH_afii57666, },
    { 0x05d3, GLYPH_afii57667, },
    { 0x05d4, GLYPH_afii57668, },
    { 0x05d5, GLYPH_afii57669, },
    { 0x05d6, GLYPH_afii57670, },
    { 0x05d7, GLYPH_afii57671, },
    { 0x05d8, GLYPH_afii57672, },
    { 0x05d9, GLYPH_afii57673, },
    { 0x05da, GLYPH_afii57674, },
    { 0x05db, GLYPH_afii57675, },
    { 0x05dc, GLYPH_afii57676, },
    { 0x05dd, GLYPH_afii57677, },
    { 0x05de, GLYPH_afii57678, },
    { 0x05df, GLYPH_afii57679, },
    { 0x05e0, GLYPH_afii57680, },
    { 0x05e1, GLYPH_afii57681, },
    { 0x05e2, GLYPH_afii57682, },
    { 0x05e3, GLYPH_afii57683, },
    { 0x05e4, GLYPH_afii57684, },
    { 0x05e5, GLYPH_afii57685, },
    { 0x05e6, GLYPH_afii57686, },
    { 0x05e7, GLYPH_afii57687, },
    { 0x05e8, GLYPH_afii57688, },
    { 0x05e9, GLYPH_afii57689, },
    { 0x05ea, GLYPH_afii57690, },
    { 0xfb2a, GLYPH_afii57694, },
    { 0xfb2b, GLYPH_afii57695, },
    { 0xfb4b, GLYPH_afii57700, },
    { 0xfb1f, GLYPH_afii57705, },
    { 0x05f0, GLYPH_afii57716, },
    { 0x05f1, GLYPH_afii57717, },
    { 0x05f2, GLYPH_afii57718, },
    { 0xfb35, GLYPH_afii57723, },
    { 0x05b4, GLYPH_afii57793, },
    { 0x05b5, GLYPH_afii57794, },
    { 0x05b6, GLYPH_afii57795, },
    { 0x05bb, GLYPH_afii57796, },
    { 0x05b8, GLYPH_afii57797, },
    { 0x05b7, GLYPH_afii57798, },
    { 0x05b0, GLYPH_afii57799, },
    { 0x05b2, GLYPH_afii57800, },
    { 0x05b1, GLYPH_afii57801, },
    { 0x05b3, GLYPH_afii57802, },
    { 0x05c2, GLYPH_afii57803, },
    { 0x05c1, GLYPH_afii57804, },
    { 0x05b9, GLYPH_afii57806, },
    { 0x05bc, GLYPH_afii57807, },
    { 0x05bd, GLYPH_afii57839, },
    { 0x05bf, GLYPH_afii57841, },
    { 0x05c0, GLYPH_afii57842, },
    { 0x02bc, GLYPH_afii57929, },
    { 0x2105, GLYPH_afii61248, },
    { 0x2113, GLYPH_afii61289, },
    { 0x2116, GLYPH_afii61352, },
    { 0x202c, GLYPH_afii61573, },
    { 0x202d, GLYPH_afii61574, },
    { 0x202e, GLYPH_afii61575, },
    { 0x200c, GLYPH_afii61664, },
    { 0x066d, GLYPH_afii63167, },
    { 0x02bd, GLYPH_afii64937, },
    { 0x0391, GLYPH_afii9793, },
    { 0x0392, GLYPH_afii9794, },
    { 0x0393, GLYPH_afii9796, },
    { 0x2206, GLYPH_afii9797, },
    { 0x0395, GLYPH_afii9798, },
    { 0x0396, GLYPH_afii9801, },
    { 0x0397, GLYPH_afii9802, },
    { 0x0398, GLYPH_afii9803, },
    { 0x0399, GLYPH_afii9804, },
    { 0x039a, GLYPH_afii9805, },
    { 0x039b, GLYPH_afii9806, },
    { 0x039c, GLYPH_afii9807, },
    { 0x039d, GLYPH_afii9808, },
    { 0x039e, GLYPH_afii9809, },
    { 0x039f, GLYPH_afii9810, },
    { 0x03a0, GLYPH_afii9811, },
    { 0x03a1, GLYPH_afii9813, },
    { 0x03a3, GLYPH_afii9814, },
    { 0x03a4, GLYPH_afii9816, },
    { 0x03a5, GLYPH_afii9817, },
    { 0x03a6, GLYPH_afii9818, },
    { 0x03a7, GLYPH_afii9819, },
    { 0x03a8, GLYPH_afii9820, },
    { 0x2126, GLYPH_afii9821, },
    { 0x03b1, GLYPH_afii9825, },
    { 0x03b2, GLYPH_afii9826, },
    { 0x03b3, GLYPH_afii9828, },
    { 0x03b4, GLYPH_afii9829, },
    { 0x03b5, GLYPH_afii9830, },
    { 0x03b6, GLYPH_afii9833, },
    { 0x03b7, GLYPH_afii9834, },
    { 0x03b8, GLYPH_afii9835, },
    { 0x03b9, GLYPH_afii9836, },
    { 0x03ba, GLYPH_afii9837, },
    { 0x03bb, GLYPH_afii9838, },
    { 0x00b5, GLYPH_afii9839, },
    { 0x03bd, GLYPH_afii9840, },
    { 0x03be, GLYPH_afii9841, },
    { 0x03bf, GLYPH_afii9842, },
    { 0x03c0, GLYPH_afii9843, },
    { 0x03c1, GLYPH_afii9845, },
    { 0x03c3, GLYPH_afii9846, },
    { 0x03c2, GLYPH_afii9847, },
    { 0x03c4, GLYPH_afii9848, },
    { 0x03c5, GLYPH_afii9849, },
    { 0x03c6, GLYPH_afii9850, },
    { 0x03c7, GLYPH_afii9851, },
    { 0x03c8, GLYPH_afii9852, },
    { 0x03c9, GLYPH_afii9853, },
    { 0x0386, GLYPH_afii9936, },
    { 0x0388, GLYPH_afii9937, },
    { 0x0389, GLYPH_afii9938, },
    { 0x038a, GLYPH_afii9939, },
    { 0x038c, GLYPH_afii9940, },
    { 0x038e, GLYPH_afii9941, },
    { 0x038f, GLYPH_afii9942, },
    { 0x03aa, GLYPH_afii9943, },
    { 0x03ab, GLYPH_afii9944, },
    { 0x03ac, GLYPH_afii9968, },
    { 0x03ad, GLYPH_afii9969, },
    { 0x03ae, GLYPH_afii9970, },
    { 0x03af, GLYPH_afii9971, },
    { 0x03cc, GLYPH_afii9972, },
    { 0x03cd, GLYPH_afii9973, },
    { 0x03ce, GLYPH_afii9974, },
    { 0x03ca, GLYPH_afii9975, },
    { 0x03cb, GLYPH_afii9976, },
    { 0x0390, GLYPH_afii9977, },
    { 0x03b0, GLYPH_afii9978, },
    { 0x00e0, GLYPH_agrave, },
    { 0x0a85, GLYPH_agujarati, },
    { 0x0a05, GLYPH_agurmukhi, },
    { 0x3042, GLYPH_ahiragana, },
    { 0x1ea3, GLYPH_ahookabove, },
    { 0x0990, GLYPH_aibengali, },
    { 0x311e, GLYPH_aibopomofo, },
    { 0x0910, GLYPH_aideva, },
    { 0x04d5, GLYPH_aiecyrillic, },
    { 0x0a90, GLYPH_aigujarati, },
    { 0x0a10, GLYPH_aigurmukhi, },
    { 0x0a48, GLYPH_aimatragurmukhi, },
    { 0x0639, GLYPH_ainarabic, },
    { 0xfeca, GLYPH_ainfinalarabic, },
    { 0xfecb, GLYPH_aininitialarabic, },
    { 0xfecc, GLYPH_ainmedialarabic, },
    { 0x0203, GLYPH_ainvertedbreve, },
    { 0x09c8, GLYPH_aivowelsignbengali, },
    { 0x0948, GLYPH_aivowelsigndeva, },
    { 0x0ac8, GLYPH_aivowelsigngujarati, },
    { 0x30a2, GLYPH_akatakana, },
    { 0xff71, GLYPH_akatakanahalfwidth, },
    { 0x314f, GLYPH_akorean, },
    { 0x05d0, GLYPH_alef, },
    { 0x0627, GLYPH_alefarabic, },
    { 0xfb30, GLYPH_alefdageshhebrew, },
    { 0xfe8e, GLYPH_aleffinalarabic, },
    { 0x0623, GLYPH_alefhamzaabovearabic, },
    { 0xfe84, GLYPH_alefhamzaabovefinalarabic, },
    { 0x0625, GLYPH_alefhamzabelowarabic, },
    { 0xfe88, GLYPH_alefhamzabelowfinalarabic, },
    { 0x05d0, GLYPH_alefhebrew, },
    { 0xfb4f, GLYPH_aleflamedhebrew, },
    { 0x0622, GLYPH_alefmaddaabovearabic, },
    { 0xfe82, GLYPH_alefmaddaabovefinalarabic, },
    { 0x0649, GLYPH_alefmaksuraarabic, },
    { 0xfef0, GLYPH_alefmaksurafinalarabic, },
    { 0xfef3, GLYPH_alefmaksurainitialarabic, },
    { 0xfef4, GLYPH_alefmaksuramedialarabic, },
    { 0xfb2e, GLYPH_alefpatahhebrew, },
    { 0xfb2f, GLYPH_alefqamatshebrew, },
    { 0x2135, GLYPH_aleph, },
    { 0x224c, GLYPH_allequal, },
    { 0x03b1, GLYPH_alpha, },
    { 0x03ac, GLYPH_alphatonos, },
    { 0x0101, GLYPH_amacron, },
    { 0xff41, GLYPH_amonospace, },
    { 0x0026, GLYPH_ampersand, },
    { 0xff06, GLYPH_ampersandmonospace, },
    { 0xf726, GLYPH_ampersandsmall, },
    { 0x33c2, GLYPH_amsquare, },
    { 0x3122, GLYPH_anbopomofo, },
    { 0x3124, GLYPH_angbopomofo, },
    { 0x0e5a, GLYPH_angkhankhuthai, },
    { 0x2220, GLYPH_angle, },
    { 0x3008, GLYPH_anglebracketleft, },
    { 0xfe3f, GLYPH_anglebracketleftvertical, },
    { 0x3009, GLYPH_anglebracketright, },
    { 0xfe40, GLYPH_anglebracketrightvertical, },
    { 0x2329, GLYPH_angleleft, },
    { 0x232a, GLYPH_angleright, },
    { 0x212b, GLYPH_angstrom, },
    { 0x0387, GLYPH_anoteleia, },
    { 0x0952, GLYPH_anudattadeva, },
    { 0x0982, GLYPH_anusvarabengali, },
    { 0x0902, GLYPH_anusvaradeva, },
    { 0x0a82, GLYPH_anusvaragujarati, },
    { 0x0105, GLYPH_aogonek, },
    { 0x3300, GLYPH_apaatosquare, },
    { 0x249c, GLYPH_aparen, },
    { 0x2019, GLYPH_apostrophe, },
    { 0x055a, GLYPH_apostrophearmenian, },
    { 0x02bc, GLYPH_apostrophemod, },
    { 0xf8ff, GLYPH_apple, },
    { 0x2250, GLYPH_approaches, },
    { 0x2248, GLYPH_approxequal, },
    { 0x2252, GLYPH_approxequalorimage, },
    { 0x2245, GLYPH_approximatelyequal, },
    { 0x318e, GLYPH_araeaekorean, },
    { 0x318d, GLYPH_araeakorean, },
    { 0x2312, GLYPH_arc, },
    { 0x1e9a, GLYPH_arighthalfring, },
    { 0x00e5, GLYPH_aring, },
    { 0x01fb, GLYPH_aringacute, },
    { 0x1e01, GLYPH_aringbelow, },
    { 0x2194, GLYPH_arrowboth, },
    { 0x21e3, GLYPH_arrowdashdown, },
    { 0x21e0, GLYPH_arrowdashleft, },
    { 0x21e2, GLYPH_arrowdashright, },
    { 0x21e1, GLYPH_arrowdashup, },
    { 0x21d4, GLYPH_arrowdblboth, },
    { 0x21d3, GLYPH_arrowdbldown, },
    { 0x21d0, GLYPH_arrowdblleft, },
    { 0x21d2, GLYPH_arrowdblright, },
    { 0x21d1, GLYPH_arrowdblup, },
    { 0x2193, GLYPH_arrowdown, },
    { 0x2199, GLYPH_arrowdownleft, },
    { 0x2198, GLYPH_arrowdownright, },
    { 0x21e9, GLYPH_arrowdownwhite, },
    { 0x02c5, GLYPH_arrowheaddownmod, },
    { 0x02c2, GLYPH_arrowheadleftmod, },
    { 0x02c3, GLYPH_arrowheadrightmod, },
    { 0x02c4, GLYPH_arrowheadupmod, },
    { 0xf8e7, GLYPH_arrowhorizex, },
    { 0x2190, GLYPH_arrowleft, },
    { 0x21d0, GLYPH_arrowleftdbl, },
    { 0x21cd, GLYPH_arrowleftdblstroke, },
    { 0x21c6, GLYPH_arrowleftoverright, },
    { 0x21e6, GLYPH_arrowleftwhite, },
    { 0x2192, GLYPH_arrowright, },
    { 0x21cf, GLYPH_arrowrightdblstroke, },
    { 0x279e, GLYPH_arrowrightheavy, },
    { 0x21c4, GLYPH_arrowrightoverleft, },
    { 0x21e8, GLYPH_arrowrightwhite, },
    { 0x21e4, GLYPH_arrowtableft, },
    { 0x21e5, GLYPH_arrowtabright, },
    { 0x2191, GLYPH_arrowup, },
    { 0x2195, GLYPH_arrowupdn, },
    { 0x21a8, GLYPH_arrowupdnbse, },
    { 0x21a8, GLYPH_arrowupdownbase, },
    { 0x2196, GLYPH_arrowupleft, },
    { 0x21c5, GLYPH_arrowupleftofdown, },
    { 0x2197, GLYPH_arrowupright, },
    { 0x21e7, GLYPH_arrowupwhite, },
    { 0xf8e6, GLYPH_arrowvertex, },
    { 0x005e, GLYPH_asciicircum, },
    { 0xff3e, GLYPH_asciicircummonospace, },
    { 0x007e, GLYPH_asciitilde, },
    { 0xff5e, GLYPH_asciitildemonospace, },
    { 0x0251, GLYPH_ascript, },
    { 0x0252, GLYPH_ascriptturned, },
    { 0x3041, GLYPH_asmallhiragana, },
    { 0x30a1, GLYPH_asmallkatakana, },
    { 0xff67, GLYPH_asmallkatakanahalfwidth, },
    { 0x002a, GLYPH_asterisk, },
    { 0x066d, GLYPH_asteriskaltonearabic, },
    { 0x066d, GLYPH_asteriskarabic, },
    { 0x2217, GLYPH_asteriskmath, },
    { 0xff0a, GLYPH_asteriskmonospace, },
    { 0xfe61, GLYPH_asterisksmall, },
    { 0x2042, GLYPH_asterism, },
    { 0xf6e9, GLYPH_asuperior, },
    { 0x2243, GLYPH_asymptoticallyequal, },
    { 0x0040, GLYPH_at, },
    { 0x00e3, GLYPH_atilde, },
    { 0xff20, GLYPH_atmonospace, },
    { 0xfe6b, GLYPH_atsmall, },
    { 0x0250, GLYPH_aturned, },
    { 0x0994, GLYPH_aubengali, },
    { 0x3120, GLYPH_aubopomofo, },
    { 0x0914, GLYPH_audeva, },
    { 0x0a94, GLYPH_augujarati, },
    { 0x0a14, GLYPH_augurmukhi, },
    { 0x09d7, GLYPH_aulengthmarkbengali, },
    { 0x0a4c, GLYPH_aumatragurmukhi, },
    { 0x09cc, GLYPH_auvowelsignbengali, },
    { 0x094c, GLYPH_auvowelsigndeva, },
    { 0x0acc, GLYPH_auvowelsigngujarati, },
    { 0x093d, GLYPH_avagrahadeva, },
    { 0x0561, GLYPH_aybarmenian, },
    { 0x05e2, GLYPH_ayin, },
    { 0xfb20, GLYPH_ayinaltonehebrew, },
    { 0x05e2, GLYPH_ayinhebrew, },
    { 0x0062, GLYPH_b, },
    { 0x09ac, GLYPH_babengali, },
    { 0x005c, GLYPH_backslash, },
    { 0xff3c, GLYPH_backslashmonospace, },
    { 0x092c, GLYPH_badeva, },
    { 0x0aac, GLYPH_bagujarati, },
    { 0x0a2c, GLYPH_bagurmukhi, },
    { 0x3070, GLYPH_bahiragana, },
    { 0x0e3f, GLYPH_bahtthai, },
    { 0x30d0, GLYPH_bakatakana, },
    { 0x007c, GLYPH_bar, },
    { 0xff5c, GLYPH_barmonospace, },
    { 0x3105, GLYPH_bbopomofo, },
    { 0x24d1, GLYPH_bcircle, },
    { 0x1e03, GLYPH_bdotaccent, },
    { 0x1e05, GLYPH_bdotbelow, },
    { 0x266c, GLYPH_beamedsixteenthnotes, },
    { 0x2235, GLYPH_because, },
    { 0x0431, GLYPH_becyrillic, },
    { 0x0628, GLYPH_beharabic, },
    { 0xfe90, GLYPH_behfinalarabic, },
    { 0xfe91, GLYPH_behinitialarabic, },
    { 0x3079, GLYPH_behiragana, },
    { 0xfe92, GLYPH_behmedialarabic, },
    { 0xfc9f, GLYPH_behmeeminitialarabic, },
    { 0xfc08, GLYPH_behmeemisolatedarabic, },
    { 0xfc6d, GLYPH_behnoonfinalarabic, },
    { 0x30d9, GLYPH_bekatakana, },
    { 0x0562, GLYPH_benarmenian, },
    { 0x05d1, GLYPH_bet, },
    { 0x03b2, GLYPH_beta, },
    { 0x03d0, GLYPH_betasymbolgreek, },
    { 0xfb31, GLYPH_betdagesh, },
    { 0xfb31, GLYPH_betdageshhebrew, },
    { 0x05d1, GLYPH_bethebrew, },
    { 0xfb4c, GLYPH_betrafehebrew, },
    { 0x09ad, GLYPH_bhabengali, },
    { 0x092d, GLYPH_bhadeva, },
    { 0x0aad, GLYPH_bhagujarati, },
    { 0x0a2d, GLYPH_bhagurmukhi, },
    { 0x0253, GLYPH_bhook, },
    { 0x3073, GLYPH_bihiragana, },
    { 0x30d3, GLYPH_bikatakana, },
    { 0x0298, GLYPH_bilabialclick, },
    { 0x0a02, GLYPH_bindigurmukhi, },
    { 0x3331, GLYPH_birusquare, },
    { 0x25cf, GLYPH_blackcircle, },
    { 0x25c6, GLYPH_blackdiamond, },
    { 0x25bc, GLYPH_blackdownpointingtriangle, },
    { 0x25c4, GLYPH_blackleftpointingpointer, },
    { 0x25c0, GLYPH_blackleftpointingtriangle, },
    { 0x3010, GLYPH_blacklenticularbracketleft, },
    { 0xfe3b, GLYPH_blacklenticularbracketleftvertical, },
    { 0x3011, GLYPH_blacklenticularbracketright, },
    { 0xfe3c, GLYPH_blacklenticularbracketrightvertical, },
    { 0x25e3, GLYPH_blacklowerlefttriangle, },
    { 0x25e2, GLYPH_blacklowerrighttriangle, },
    { 0x25ac, GLYPH_blackrectangle, },
    { 0x25ba, GLYPH_blackrightpointingpointer, },
    { 0x25b6, GLYPH_blackrightpointingtriangle, },
    { 0x25aa, GLYPH_blacksmallsquare, },
    { 0x263b, GLYPH_blacksmilingface, },
    { 0x25a0, GLYPH_blacksquare, },
    { 0x2605, GLYPH_blackstar, },
    { 0x25e4, GLYPH_blackupperlefttriangle, },
    { 0x25e5, GLYPH_blackupperrighttriangle, },
    { 0x25b4, GLYPH_blackuppointingsmalltriangle, },
    { 0x25b2, GLYPH_blackuppointingtriangle, },
    { 0x2423, GLYPH_blank, },
    { 0x1e07, GLYPH_blinebelow, },
    { 0x2588, GLYPH_block, },
    { 0xff42, GLYPH_bmonospace, },
    { 0x0e1a, GLYPH_bobaimaithai, },
    { 0x307c, GLYPH_bohiragana, },
    { 0x30dc, GLYPH_bokatakana, },
    { 0x249d, GLYPH_bparen, },
    { 0x33c3, GLYPH_bqsquare, },
    { 0xf8f4, GLYPH_braceex, },
    { 0x007b, GLYPH_braceleft, },
    { 0xf8f3, GLYPH_braceleftbt, },
    { 0xf8f2, GLYPH_braceleftmid, },
    { 0xff5b, GLYPH_braceleftmonospace, },
    { 0xfe5b, GLYPH_braceleftsmall, },
    { 0xf8f1, GLYPH_bracelefttp, },
    { 0xfe37, GLYPH_braceleftvertical, },
    { 0x007d, GLYPH_braceright, },
    { 0xf8fe, GLYPH_bracerightbt, },
    { 0xf8fd, GLYPH_bracerightmid, },
    { 0xff5d, GLYPH_bracerightmonospace, },
    { 0xfe5c, GLYPH_bracerightsmall, },
    { 0xf8fc, GLYPH_bracerighttp, },
    { 0xfe38, GLYPH_bracerightvertical, },
    { 0x005b, GLYPH_bracketleft, },
    { 0xf8f0, GLYPH_bracketleftbt, },
    { 0xf8ef, GLYPH_bracketleftex, },
    { 0xff3b, GLYPH_bracketleftmonospace, },
    { 0xf8ee, GLYPH_bracketlefttp, },
    { 0x005d, GLYPH_bracketright, },
    { 0xf8fb, GLYPH_bracketrightbt, },
    { 0xf8fa, GLYPH_bracketrightex, },
    { 0xff3d, GLYPH_bracketrightmonospace, },
    { 0xf8f9, GLYPH_bracketrighttp, },
    { 0x02d8, GLYPH_breve, },
    { 0x032e, GLYPH_brevebelowcmb, },
    { 0x0306, GLYPH_brevecmb, },
    { 0x032f, GLYPH_breveinvertedbelowcmb, },
    { 0x0311, GLYPH_breveinvertedcmb, },
    { 0x0361, GLYPH_breveinverteddoublecmb, },
    { 0x032a, GLYPH_bridgebelowcmb, },
    { 0x033a, GLYPH_bridgeinvertedbelowcmb, },
    { 0x00a6, GLYPH_brokenbar, },
    { 0x0180, GLYPH_bstroke, },
    { 0xf6ea, GLYPH_bsuperior, },
    { 0x0183, GLYPH_btopbar, },
    { 0x3076, GLYPH_buhiragana, },
    { 0x30d6, GLYPH_bukatakana, },
    { 0x2022, GLYPH_bullet, },
    { 0x25d8, GLYPH_bulletinverse, },
    { 0x2219, GLYPH_bulletoperator, },
    { 0x25ce, GLYPH_bullseye, },
    { 0x0063, GLYPH_c, },
    { 0x056e, GLYPH_caarmenian, },
    { 0x099a, GLYPH_cabengali, },
    { 0x0107, GLYPH_cacute, },
    { 0x091a, GLYPH_cadeva, },
    { 0x0a9a, GLYPH_cagujarati, },
    { 0x0a1a, GLYPH_cagurmukhi, },
    { 0x3388, GLYPH_calsquare, },
    { 0x0981, GLYPH_candrabindubengali, },
    { 0x0310, GLYPH_candrabinducmb, },
    { 0x0901, GLYPH_candrabindudeva, },
    { 0x0a81, GLYPH_candrabindugujarati, },
    { 0x21ea, GLYPH_capslock, },
    { 0x2105, GLYPH_careof, },
    { 0x02c7, GLYPH_caron, },
    { 0x032c, GLYPH_caronbelowcmb, },
    { 0x030c, GLYPH_caroncmb, },
    { 0x21b5, GLYPH_carriagereturn, },
    { 0x3118, GLYPH_cbopomofo, },
    { 0x010d, GLYPH_ccaron, },
    { 0x00e7, GLYPH_ccedilla, },
    { 0x1e09, GLYPH_ccedillaacute, },
    { 0x24d2, GLYPH_ccircle, },
    { 0x0109, GLYPH_ccircumflex, },
    { 0x0255, GLYPH_ccurl, },
    { 0x010b, GLYPH_cdot, },
    { 0x010b, GLYPH_cdotaccent, },
    { 0x33c5, GLYPH_cdsquare, },
    { 0x00b8, GLYPH_cedilla, },
    { 0x0327, GLYPH_cedillacmb, },
    { 0x00a2, GLYPH_cent, },
    { 0x2103, GLYPH_centigrade, },
    { 0xf6df, GLYPH_centinferior, },
    { 0xffe0, GLYPH_centmonospace, },
    { 0xf7a2, GLYPH_centoldstyle, },
    { 0xf6e0, GLYPH_centsuperior, },
    { 0x0579, GLYPH_chaarmenian, },
    { 0x099b, GLYPH_chabengali, },
    { 0x091b, GLYPH_chadeva, },
    { 0x0a9b, GLYPH_chagujarati, },
    { 0x0a1b, GLYPH_chagurmukhi, },
    { 0x3114, GLYPH_chbopomofo, },
    { 0x04bd, GLYPH_cheabkhasiancyrillic, },
    { 0x2713, GLYPH_checkmark, },
    { 0x0447, GLYPH_checyrillic, },
    { 0x04bf, GLYPH_chedescenderabkhasiancyrillic, },
    { 0x04b7, GLYPH_chedescendercyrillic, },
    { 0x04f5, GLYPH_chedieresiscyrillic, },
    { 0x0573, GLYPH_cheharmenian, },
    { 0x04cc, GLYPH_chekhakassiancyrillic, },
    { 0x04b9, GLYPH_cheverticalstrokecyrillic, },
    { 0x03c7, GLYPH_chi, },
    { 0x3277, GLYPH_chieuchacirclekorean, },
    { 0x3217, GLYPH_chieuchaparenkorean, },
    { 0x3269, GLYPH_chieuchcirclekorean, },
    { 0x314a, GLYPH_chieuchkorean, },
    { 0x3209, GLYPH_chieuchparenkorean, },
    { 0x0e0a, GLYPH_chochangthai, },
    { 0x0e08, GLYPH_chochanthai, },
    { 0x0e09, GLYPH_chochingthai, },
    { 0x0e0c, GLYPH_chochoethai, },
    { 0x0188, GLYPH_chook, },
    { 0x3276, GLYPH_cieucacirclekorean, },
    { 0x3216, GLYPH_cieucaparenkorean, },
    { 0x3268, GLYPH_cieuccirclekorean, },
    { 0x3148, GLYPH_cieuckorean, },
    { 0x3208, GLYPH_cieucparenkorean, },
    { 0x321c, GLYPH_cieucuparenkorean, },
    { 0x25cb, GLYPH_circle, },
    { 0x2297, GLYPH_circlemultiply, },
    { 0x2299, GLYPH_circleot, },
    { 0x2295, GLYPH_circleplus, },
    { 0x3036, GLYPH_circlepostalmark, },
    { 0x25d0, GLYPH_circlewithlefthalfblack, },
    { 0x25d1, GLYPH_circlewithrighthalfblack, },
    { 0x02c6, GLYPH_circumflex, },
    { 0x032d, GLYPH_circumflexbelowcmb, },
    { 0x0302, GLYPH_circumflexcmb, },
    { 0x2327, GLYPH_clear, },
    { 0x01c2, GLYPH_clickalveolar, },
    { 0x01c0, GLYPH_clickdental, },
    { 0x01c1, GLYPH_clicklateral, },
    { 0x01c3, GLYPH_clickretroflex, },
    { 0x2663, GLYPH_club, },
    { 0x2663, GLYPH_clubsuitblack, },
    { 0x2667, GLYPH_clubsuitwhite, },
    { 0x33a4, GLYPH_cmcubedsquare, },
    { 0xff43, GLYPH_cmonospace, },
    { 0x33a0, GLYPH_cmsquaredsquare, },
    { 0x0581, GLYPH_coarmenian, },
    { 0x003a, GLYPH_colon, },
    { 0x20a1, GLYPH_colonmonetary, },
    { 0xff1a, GLYPH_colonmonospace, },
    { 0x20a1, GLYPH_colonsign, },
    { 0xfe55, GLYPH_colonsmall, },
    { 0x02d1, GLYPH_colontriangularhalfmod, },
    { 0x02d0, GLYPH_colontriangularmod, },
    { 0x002c, GLYPH_comma, },
    { 0x0313, GLYPH_commaabovecmb, },
    { 0x0315, GLYPH_commaaboverightcmb, },
    { 0xf6c3, GLYPH_commaaccent, },
    { 0x060c, GLYPH_commaarabic, },
    { 0x055d, GLYPH_commaarmenian, },
    { 0xf6e1, GLYPH_commainferior, },
    { 0xff0c, GLYPH_commamonospace, },
    { 0x0314, GLYPH_commareversedabovecmb, },
    { 0x02bd, GLYPH_commareversedmod, },
    { 0xfe50, GLYPH_commasmall, },
    { 0xf6e2, GLYPH_commasuperior, },
    { 0x0312, GLYPH_commaturnedabovecmb, },
    { 0x02bb, GLYPH_commaturnedmod, },
    { 0x263c, GLYPH_compass, },
    { 0x2245, GLYPH_congruent, },
    { 0x222e, GLYPH_contourintegral, },
    { 0x2303, GLYPH_control, },
    { 0x0006, GLYPH_controlACK, },
    { 0x0007, GLYPH_controlBEL, },
    { 0x0008, GLYPH_controlBS, },
    { 0x0018, GLYPH_controlCAN, },
    { 0x000d, GLYPH_controlCR, },
    { 0x0011, GLYPH_controlDC1, },
    { 0x0012, GLYPH_controlDC2, },
    { 0x0013, GLYPH_controlDC3, },
    { 0x0014, GLYPH_controlDC4, },
    { 0x007f, GLYPH_controlDEL, },
    { 0x0010, GLYPH_controlDLE, },
    { 0x0019, GLYPH_controlEM, },
    { 0x0005, GLYPH_controlENQ, },
    { 0x0004, GLYPH_controlEOT, },
    { 0x001b, GLYPH_controlESC, },
    { 0x0017, GLYPH_controlETB, },
    { 0x0003, GLYPH_controlETX, },
    { 0x000c, GLYPH_controlFF, },
    { 0x001c, GLYPH_controlFS, },
    { 0x001d, GLYPH_controlGS, },
    { 0x0009, GLYPH_controlHT, },
    { 0x000a, GLYPH_controlLF, },
    { 0x0015, GLYPH_controlNAK, },
    { 0x001e, GLYPH_controlRS, },
    { 0x000f, GLYPH_controlSI, },
    { 0x000e, GLYPH_controlSO, },
    { 0x0002, GLYPH_controlSOT, },
    { 0x0001, GLYPH_controlSTX, },
    { 0x001a, GLYPH_controlSUB, },
    { 0x0016, GLYPH_controlSYN, },
    { 0x001f, GLYPH_controlUS, },
    { 0x000b, GLYPH_controlVT, },
    { 0x00a9, GLYPH_copyright, },
    { 0xf8e9, GLYPH_copyrightsans, },
    { 0xf6d9, GLYPH_copyrightserif, },
    { 0x300c, GLYPH_cornerbracketleft, },
    { 0xff62, GLYPH_cornerbracketlefthalfwidth, },
    { 0xfe41, GLYPH_cornerbracketleftvertical, },
    { 0x300d, GLYPH_cornerbracketright, },
    { 0xff63, GLYPH_cornerbracketrighthalfwidth, },
    { 0xfe42, GLYPH_cornerbracketrightvertical, },
    { 0x337f, GLYPH_corporationsquare, },
    { 0x33c7, GLYPH_cosquare, },
    { 0x33c6, GLYPH_coverkgsquare, },
    { 0x249e, GLYPH_cparen, },
    { 0x20a2, GLYPH_cruzeiro, },
    { 0x0297, GLYPH_cstretched, },
    { 0x22cf, GLYPH_curlyand, },
    { 0x22ce, GLYPH_curlyor, },
    { 0x00a4, GLYPH_currency, },
    { 0xf6d1, GLYPH_cyrBreve, },
    { 0xf6d2, GLYPH_cyrFlex, },
    { 0xf6d4, GLYPH_cyrbreve, },
    { 0xf6d5, GLYPH_cyrflex, },
    { 0x0064, GLYPH_d, },
    { 0x0564, GLYPH_daarmenian, },
    { 0x09a6, GLYPH_dabengali, },
    { 0x0636, GLYPH_dadarabic, },
    { 0x0926, GLYPH_dadeva, },
    { 0xfebe, GLYPH_dadfinalarabic, },
    { 0xfebf, GLYPH_dadinitialarabic, },
    { 0xfec0, GLYPH_dadmedialarabic, },
    { 0x05bc, GLYPH_dagesh, },
    { 0x05bc, GLYPH_dageshhebrew, },
    { 0x2020, GLYPH_dagger, },
    { 0x2021, GLYPH_daggerdbl, },
    { 0x0aa6, GLYPH_dagujarati, },
    { 0x0a26, GLYPH_dagurmukhi, },
    { 0x3060, GLYPH_dahiragana, },
    { 0x30c0, GLYPH_dakatakana, },
    { 0x062f, GLYPH_dalarabic, },
    { 0x05d3, GLYPH_dalet, },
    { 0xfb33, GLYPH_daletdagesh, },
    { 0xfb33, GLYPH_daletdageshhebrew, },
    { 0x05d3, GLYPH_dalethebrew, },
    { 0xfeaa, GLYPH_dalfinalarabic, },
    { 0x064f, GLYPH_dammaarabic, },
    { 0x064f, GLYPH_dammalowarabic, },
    { 0x064c, GLYPH_dammatanaltonearabic, },
    { 0x064c, GLYPH_dammatanarabic, },
    { 0x0964, GLYPH_danda, },
    { 0x05a7, GLYPH_dargahebrew, },
    { 0x05a7, GLYPH_dargalefthebrew, },
    { 0x0485, GLYPH_dasiapneumatacyrilliccmb, },
    { 0xf6d3, GLYPH_dblGrave, },
    { 0x300a, GLYPH_dblanglebracketleft, },
    { 0xfe3d, GLYPH_dblanglebracketleftvertical, },
    { 0x300b, GLYPH_dblanglebracketright, },
    { 0xfe3e, GLYPH_dblanglebracketrightvertical, },
    { 0x032b, GLYPH_dblarchinvertedbelowcmb, },
    { 0x21d4, GLYPH_dblarrowleft, },
    { 0x21d2, GLYPH_dblarrowright, },
    { 0x0965, GLYPH_dbldanda, },
    { 0xf6d6, GLYPH_dblgrave, },
    { 0x030f, GLYPH_dblgravecmb, },
    { 0x222c, GLYPH_dblintegral, },
    { 0x2017, GLYPH_dbllowline, },
    { 0x0333, GLYPH_dbllowlinecmb, },
    { 0x033f, GLYPH_dbloverlinecmb, },
    { 0x02ba, GLYPH_dblprimemod, },
    { 0x2016, GLYPH_dblverticalbar, },
    { 0x030e, GLYPH_dblverticallineabovecmb, },
    { 0x3109, GLYPH_dbopomofo, },
    { 0x33c8, GLYPH_dbsquare, },
    { 0x010f, GLYPH_dcaron, },
    { 0x1e11, GLYPH_dcedilla, },
    { 0x24d3, GLYPH_dcircle, },
    { 0x1e13, GLYPH_dcircumflexbelow, },
    { 0x0111, GLYPH_dcroat, },
    { 0x09a1, GLYPH_ddabengali, },
    { 0x0921, GLYPH_ddadeva, },
    { 0x0aa1, GLYPH_ddagujarati, },
    { 0x0a21, GLYPH_ddagurmukhi, },
    { 0x0688, GLYPH_ddalarabic, },
    { 0xfb89, GLYPH_ddalfinalarabic, },
    { 0x095c, GLYPH_dddhadeva, },
    { 0x09a2, GLYPH_ddhabengali, },
    { 0x0922, GLYPH_ddhadeva, },
    { 0x0aa2, GLYPH_ddhagujarati, },
    { 0x0a22, GLYPH_ddhagurmukhi, },
    { 0x1e0b, GLYPH_ddotaccent, },
    { 0x1e0d, GLYPH_ddotbelow, },
    { 0x066b, GLYPH_decimalseparatorarabic, },
    { 0x066b, GLYPH_decimalseparatorpersian, },
    { 0x0434, GLYPH_decyrillic, },
    { 0x00b0, GLYPH_degree, },
    { 0x05ad, GLYPH_dehihebrew, },
    { 0x3067, GLYPH_dehiragana, },
    { 0x03ef, GLYPH_deicoptic, },
    { 0x30c7, GLYPH_dekatakana, },
    { 0x232b, GLYPH_deleteleft, },
    { 0x2326, GLYPH_deleteright, },
    { 0x03b4, GLYPH_delta, },
    { 0x018d, GLYPH_deltaturned, },
    { 0x09f8, GLYPH_denominatorminusonenumeratorbengali, },
    { 0x02a4, GLYPH_dezh, },
    { 0x09a7, GLYPH_dhabengali, },
    { 0x0927, GLYPH_dhadeva, },
    { 0x0aa7, GLYPH_dhagujarati, },
    { 0x0a27, GLYPH_dhagurmukhi, },
    { 0x0257, GLYPH_dhook, },
    { 0x00a8, GLYPH_diaeresis, },
    { 0x0385, GLYPH_dialytikatonos, },
    { 0x0344, GLYPH_dialytikatonoscmb, },
    { 0x2666, GLYPH_diamond, },
    { 0x2662, GLYPH_diamondsuitwhite, },
    { 0x00a8, GLYPH_dieresis, },
    { 0xf6d7, GLYPH_dieresisacute, },
    { 0x0324, GLYPH_dieresisbelowcmb, },
    { 0x0308, GLYPH_dieresiscmb, },
    { 0xf6d8, GLYPH_dieresisgrave, },
    { 0x0385, GLYPH_dieresistonos, },
    { 0x3062, GLYPH_dihiragana, },
    { 0x30c2, GLYPH_dikatakana, },
    { 0x3003, GLYPH_dittomark, },
    { 0x00f7, GLYPH_divide, },
    { 0x2223, GLYPH_divides, },
    { 0x00f7, GLYPH_division, },
    { 0x2215, GLYPH_divisionslash, },
    { 0x0452, GLYPH_djecyrillic, },
    { 0x2593, GLYPH_dkshade, },
    { 0x1e0f, GLYPH_dlinebelow, },
    { 0x3397, GLYPH_dlsquare, },
    { 0x0111, GLYPH_dmacron, },
    { 0xff44, GLYPH_dmonospace, },
    { 0x2584, GLYPH_dnblock, },
    { 0x0e0e, GLYPH_dochadathai, },
    { 0x0e14, GLYPH_dodekthai, },
    { 0x3069, GLYPH_dohiragana, },
    { 0x30c9, GLYPH_dokatakana, },
    { 0x0024, GLYPH_dollar, },
    { 0xf6e3, GLYPH_dollarinferior, },
    { 0xff04, GLYPH_dollarmonospace, },
    { 0xf724, GLYPH_dollaroldstyle, },
    { 0xfe69, GLYPH_dollarsmall, },
    { 0xf6e4, GLYPH_dollarsuperior, },
    { 0x20ab, GLYPH_dong, },
    { 0x3326, GLYPH_dorusquare, },
    { 0x02d9, GLYPH_dotaccent, },
    { 0x0307, GLYPH_dotaccentcmb, },
    { 0x0323, GLYPH_dotbelowcmb, },
    { 0x0323, GLYPH_dotbelowcomb, },
    { 0x30fb, GLYPH_dotkatakana, },
    { 0x0131, GLYPH_dotlessi, },
    { 0xf6be, GLYPH_dotlessj, },
    { 0x0284, GLYPH_dotlessjstrokehook, },
    { 0x22c5, GLYPH_dotmath, },
    { 0x25cc, GLYPH_dottedcircle, },
    { 0x02dd, GLYPH_doubleacute, },
    { 0xfb1f, GLYPH_doubleyodpatah, },
    { 0xfb1f, GLYPH_doubleyodpatahhebrew, },
    { 0x031e, GLYPH_downtackbelowcmb, },
    { 0x02d5, GLYPH_downtackmod, },
    { 0x249f, GLYPH_dparen, },
    { 0x0111, GLYPH_dstroke, },
    { 0xf6eb, GLYPH_dsuperior, },
    { 0x0256, GLYPH_dtail, },
    { 0x018c, GLYPH_dtopbar, },
    { 0x3065, GLYPH_duhiragana, },
    { 0x30c5, GLYPH_dukatakana, },
    { 0x01f3, GLYPH_dz, },
    { 0x02a3, GLYPH_dzaltone, },
    { 0x01c6, GLYPH_dzcaron, },
    { 0x02a5, GLYPH_dzcurl, },
    { 0x04e1, GLYPH_dzeabkhasiancyrillic, },
    { 0x0455, GLYPH_dzecyrillic, },
    { 0x045f, GLYPH_dzhecyrillic, },
    { 0x0065, GLYPH_e, },
    { 0x00e9, GLYPH_eacute, },
    { 0x2641, GLYPH_earth, },
    { 0x098f, GLYPH_ebengali, },
    { 0x311c, GLYPH_ebopomofo, },
    { 0x0115, GLYPH_ebreve, },
    { 0x090d, GLYPH_ecandradeva, },
    { 0x0a8d, GLYPH_ecandragujarati, },
    { 0x0945, GLYPH_ecandravowelsigndeva, },
    { 0x0ac5, GLYPH_ecandravowelsigngujarati, },
    { 0x011b, GLYPH_ecaron, },
    { 0x1e1d, GLYPH_ecedillabreve, },
    { 0x0565, GLYPH_echarmenian, },
    { 0x0587, GLYPH_echyiwnarmenian, },
    { 0x24d4, GLYPH_ecircle, },
    { 0x00ea, GLYPH_ecircumflex, },
    { 0x1ebf, GLYPH_ecircumflexacute, },
    { 0x1e19, GLYPH_ecircumflexbelow, },
    { 0x1ec7, GLYPH_ecircumflexdotbelow, },
    { 0x1ec1, GLYPH_ecircumflexgrave, },
    { 0x1ec3, GLYPH_ecircumflexhookabove, },
    { 0x1ec5, GLYPH_ecircumflextilde, },
    { 0x0454, GLYPH_ecyrillic, },
    { 0x0205, GLYPH_edblgrave, },
    { 0x090f, GLYPH_edeva, },
    { 0x00eb, GLYPH_ediaeresis, },
    { 0x00eb, GLYPH_edieresis, },
    { 0x0117, GLYPH_edot, },
    { 0x0117, GLYPH_edotaccent, },
    { 0x1eb9, GLYPH_edotbelow, },
    { 0x0a0f, GLYPH_eegurmukhi, },
    { 0x0a47, GLYPH_eematragurmukhi, },
    { 0x0444, GLYPH_efcyrillic, },
    { 0x00e8, GLYPH_egrave, },
    { 0x0a8f, GLYPH_egujarati, },
    { 0x0567, GLYPH_eharmenian, },
    { 0x311d, GLYPH_ehbopomofo, },
    { 0x3048, GLYPH_ehiragana, },
    { 0x1ebb, GLYPH_ehookabove, },
    { 0x311f, GLYPH_eibopomofo, },
    { 0x0038, GLYPH_eight, },
    { 0x0668, GLYPH_eightarabic, },
    { 0x09ee, GLYPH_eightbengali, },
    { 0x2467, GLYPH_eightcircle, },
    { 0x2791, GLYPH_eightcircleinversesansserif, },
    { 0x096e, GLYPH_eightdeva, },
    { 0x2471, GLYPH_eighteencircle, },
    { 0x2485, GLYPH_eighteenparen, },
    { 0x2499, GLYPH_eighteenperiod, },
    { 0x0aee, GLYPH_eightgujarati, },
    { 0x0a6e, GLYPH_eightgurmukhi, },
    { 0x0668, GLYPH_eighthackarabic, },
    { 0x3028, GLYPH_eighthangzhou, },
    { 0x266b, GLYPH_eighthnotebeamed, },
    { 0x3227, GLYPH_eightideographicparen, },
    { 0x2088, GLYPH_eightinferior, },
    { 0xff18, GLYPH_eightmonospace, },
    { 0xf738, GLYPH_eightoldstyle, },
    { 0x247b, GLYPH_eightparen, },
    { 0x248f, GLYPH_eightperiod, },
    { 0x06f8, GLYPH_eightpersian, },
    { 0x2177, GLYPH_eightroman, },
    { 0x2078, GLYPH_eightsuperior, },
    { 0x0e58, GLYPH_eightthai, },
    { 0x0207, GLYPH_einvertedbreve, },
    { 0x0465, GLYPH_eiotifiedcyrillic, },
    { 0x30a8, GLYPH_ekatakana, },
    { 0xff74, GLYPH_ekatakanahalfwidth, },
    { 0x0a74, GLYPH_ekonkargurmukhi, },
    { 0x3154, GLYPH_ekorean, },
    { 0x043b, GLYPH_elcyrillic, },
    { 0x2208, GLYPH_element, },
    { 0x246a, GLYPH_elevencircle, },
    { 0x247e, GLYPH_elevenparen, },
    { 0x2492, GLYPH_elevenperiod, },
    { 0x217a, GLYPH_elevenroman, },
    { 0x2026, GLYPH_ellipsis, },
    { 0x22ee, GLYPH_ellipsisvertical, },
    { 0x0113, GLYPH_emacron, },
    { 0x1e17, GLYPH_emacronacute, },
    { 0x1e15, GLYPH_emacrongrave, },
    { 0x043c, GLYPH_emcyrillic, },
    { 0x2014, GLYPH_emdash, },
    { 0xfe31, GLYPH_emdashvertical, },
    { 0xff45, GLYPH_emonospace, },
    { 0x055b, GLYPH_emphasismarkarmenian, },
    { 0x2205, GLYPH_emptyset, },
    { 0x3123, GLYPH_enbopomofo, },
    { 0x043d, GLYPH_encyrillic, },
    { 0x2013, GLYPH_endash, },
    { 0xfe32, GLYPH_endashvertical, },
    { 0x04a3, GLYPH_endescendercyrillic, },
    { 0x014b, GLYPH_eng, },
    { 0x3125, GLYPH_engbopomofo, },
    { 0x04a5, GLYPH_enghecyrillic, },
    { 0x04c8, GLYPH_enhookcyrillic, },
    { 0x2002, GLYPH_enspace, },
    { 0x0119, GLYPH_eogonek, },
    { 0x3153, GLYPH_eokorean, },
    { 0x025b, GLYPH_eopen, },
    { 0x029a, GLYPH_eopenclosed, },
    { 0x025c, GLYPH_eopenreversed, },
    { 0x025e, GLYPH_eopenreversedclosed, },
    { 0x025d, GLYPH_eopenreversedhook, },
    { 0x24a0, GLYPH_eparen, },
    { 0x03b5, GLYPH_epsilon, },
    { 0x03ad, GLYPH_epsilontonos, },
    { 0x003d, GLYPH_equal, },
    { 0xff1d, GLYPH_equalmonospace, },
    { 0xfe66, GLYPH_equalsmall, },
    { 0x207c, GLYPH_equalsuperior, },
    { 0x2261, GLYPH_equivalence, },
    { 0x3126, GLYPH_erbopomofo, },
    { 0x0440, GLYPH_ercyrillic, },
    { 0x0258, GLYPH_ereversed, },
    { 0x044d, GLYPH_ereversedcyrillic, },
    { 0x0441, GLYPH_escyrillic, },
    { 0x04ab, GLYPH_esdescendercyrillic, },
    { 0x0283, GLYPH_esh, },
    { 0x0286, GLYPH_eshcurl, },
    { 0x090e, GLYPH_eshortdeva, },
    { 0x0946, GLYPH_eshortvowelsigndeva, },
    { 0x01aa, GLYPH_eshreversedloop, },
    { 0x0285, GLYPH_eshsquatreversed, },
    { 0x3047, GLYPH_esmallhiragana, },
    { 0x30a7, GLYPH_esmallkatakana, },
    { 0xff6a, GLYPH_esmallkatakanahalfwidth, },
    { 0x212e, GLYPH_estimated, },
    { 0xf6ec, GLYPH_esuperior, },
    { 0x03b7, GLYPH_eta, },
    { 0x0568, GLYPH_etarmenian, },
    { 0x03ae, GLYPH_etatonos, },
    { 0x00f0, GLYPH_eth, },
    { 0x1ebd, GLYPH_etilde, },
    { 0x1e1b, GLYPH_etildebelow, },
    { 0x0591, GLYPH_etnahtafoukhhebrew, },
    { 0x0591, GLYPH_etnahtafoukhlefthebrew, },
    { 0x0591, GLYPH_etnahtahebrew, },
    { 0x0591, GLYPH_etnahtalefthebrew, },
    { 0x01dd, GLYPH_eturned, },
    { 0x3161, GLYPH_eukorean, },
    { 0x20ac, GLYPH_euro, },
    { 0x09c7, GLYPH_evowelsignbengali, },
    { 0x0947, GLYPH_evowelsigndeva, },
    { 0x0ac7, GLYPH_evowelsigngujarati, },
    { 0x0021, GLYPH_exclam, },
    { 0x055c, GLYPH_exclamarmenian, },
    { 0x203c, GLYPH_exclamdbl, },
    { 0x00a1, GLYPH_exclamdown, },
    { 0xf7a1, GLYPH_exclamdownsmall, },
    { 0xff01, GLYPH_exclammonospace, },
    { 0xf721, GLYPH_exclamsmall, },
    { 0x2203, GLYPH_existential, },
    { 0x0292, GLYPH_ezh, },
    { 0x01ef, GLYPH_ezhcaron, },
    { 0x0293, GLYPH_ezhcurl, },
    { 0x01b9, GLYPH_ezhreversed, },
    { 0x01ba, GLYPH_ezhtail, },
    { 0x0066, GLYPH_f, },
    { 0x095e, GLYPH_fadeva, },
    { 0x0a5e, GLYPH_fagurmukhi, },
    { 0x2109, GLYPH_fahrenheit, },
    { 0x064e, GLYPH_fathaarabic, },
    { 0x064e, GLYPH_fathalowarabic, },
    { 0x064b, GLYPH_fathatanarabic, },
    { 0x3108, GLYPH_fbopomofo, },
    { 0x24d5, GLYPH_fcircle, },
    { 0x1e1f, GLYPH_fdotaccent, },
    { 0x0641, GLYPH_feharabic, },
    { 0x0586, GLYPH_feharmenian, },
    { 0xfed2, GLYPH_fehfinalarabic, },
    { 0xfed3, GLYPH_fehinitialarabic, },
    { 0xfed4, GLYPH_fehmedialarabic, },
    { 0x03e5, GLYPH_feicoptic, },
    { 0x2640, GLYPH_female, },
    { 0xfb00, GLYPH_ff, },
    { 0xfb03, GLYPH_ffi, },
    { 0xfb04, GLYPH_ffl, },
    { 0xfb01, GLYPH_fi, },
    { 0x246e, GLYPH_fifteencircle, },
    { 0x2482, GLYPH_fifteenparen, },
    { 0x2496, GLYPH_fifteenperiod, },
    { 0x2012, GLYPH_figuredash, },
    { 0x25a0, GLYPH_filledbox, },
    { 0x25ac, GLYPH_filledrect, },
    { 0x05da, GLYPH_finalkaf, },
    { 0xfb3a, GLYPH_finalkafdagesh, },
    { 0xfb3a, GLYPH_finalkafdageshhebrew, },
    { 0x05da, GLYPH_finalkafhebrew, },
    { 0x05dd, GLYPH_finalmem, },
    { 0x05dd, GLYPH_finalmemhebrew, },
    { 0x05df, GLYPH_finalnun, },
    { 0x05df, GLYPH_finalnunhebrew, },
    { 0x05e3, GLYPH_finalpe, },
    { 0x05e3, GLYPH_finalpehebrew, },
    { 0x05e5, GLYPH_finaltsadi, },
    { 0x05e5, GLYPH_finaltsadihebrew, },
    { 0x02c9, GLYPH_firsttonechinese, },
    { 0x25c9, GLYPH_fisheye, },
    { 0x0473, GLYPH_fitacyrillic, },
    { 0x0035, GLYPH_five, },
    { 0x0665, GLYPH_fivearabic, },
    { 0x09eb, GLYPH_fivebengali, },
    { 0x2464, GLYPH_fivecircle, },
    { 0x278e, GLYPH_fivecircleinversesansserif, },
    { 0x096b, GLYPH_fivedeva, },
    { 0x215d, GLYPH_fiveeighths, },
    { 0x0aeb, GLYPH_fivegujarati, },
    { 0x0a6b, GLYPH_fivegurmukhi, },
    { 0x0665, GLYPH_fivehackarabic, },
    { 0x3025, GLYPH_fivehangzhou, },
    { 0x3224, GLYPH_fiveideographicparen, },
    { 0x2085, GLYPH_fiveinferior, },
    { 0xff15, GLYPH_fivemonospace, },
    { 0xf735, GLYPH_fiveoldstyle, },
    { 0x2478, GLYPH_fiveparen, },
    { 0x248c, GLYPH_fiveperiod, },
    { 0x06f5, GLYPH_fivepersian, },
    { 0x2174, GLYPH_fiveroman, },
    { 0x2075, GLYPH_fivesuperior, },
    { 0x0e55, GLYPH_fivethai, },
    { 0xfb02, GLYPH_fl, },
    { 0x0192, GLYPH_florin, },
    { 0xff46, GLYPH_fmonospace, },
    { 0x3399, GLYPH_fmsquare, },
    { 0x0e1f, GLYPH_fofanthai, },
    { 0x0e1d, GLYPH_fofathai, },
    { 0x0e4f, GLYPH_fongmanthai, },
    { 0x2200, GLYPH_forall, },
    { 0x0034, GLYPH_four, },
    { 0x0664, GLYPH_fourarabic, },
    { 0x09ea, GLYPH_fourbengali, },
    { 0x2463, GLYPH_fourcircle, },
    { 0x278d, GLYPH_fourcircleinversesansserif, },
    { 0x096a, GLYPH_fourdeva, },
    { 0x0aea, GLYPH_fourgujarati, },
    { 0x0a6a, GLYPH_fourgurmukhi, },
    { 0x0664, GLYPH_fourhackarabic, },
    { 0x3024, GLYPH_fourhangzhou, },
    { 0x3223, GLYPH_fourideographicparen, },
    { 0x2084, GLYPH_fourinferior, },
    { 0xff14, GLYPH_fourmonospace, },
    { 0x09f7, GLYPH_fournumeratorbengali, },
    { 0xf734, GLYPH_fouroldstyle, },
    { 0x2477, GLYPH_fourparen, },
    { 0x248b, GLYPH_fourperiod, },
    { 0x06f4, GLYPH_fourpersian, },
    { 0x2173, GLYPH_fourroman, },
    { 0x2074, GLYPH_foursuperior, },
    { 0x246d, GLYPH_fourteencircle, },
    { 0x2481, GLYPH_fourteenparen, },
    { 0x2495, GLYPH_fourteenperiod, },
    { 0x0e54, GLYPH_fourthai, },
    { 0x02cb, GLYPH_fourthtonechinese, },
    { 0x24a1, GLYPH_fparen, },
    { 0x2044, GLYPH_fraction, },
    { 0x20a3, GLYPH_franc, },
    { 0x002e, GLYPH_fullstop, },
    { 0x0067, GLYPH_g, },
    { 0x0997, GLYPH_gabengali, },
    { 0x01f5, GLYPH_gacute, },
    { 0x0917, GLYPH_gadeva, },
    { 0x06af, GLYPH_gafarabic, },
    { 0xfb93, GLYPH_gaffinalarabic, },
    { 0xfb94, GLYPH_gafinitialarabic, },
    { 0xfb95, GLYPH_gafmedialarabic, },
    { 0x0a97, GLYPH_gagujarati, },
    { 0x0a17, GLYPH_gagurmukhi, },
    { 0x304c, GLYPH_gahiragana, },
    { 0x30ac, GLYPH_gakatakana, },
    { 0x03b3, GLYPH_gamma, },
    { 0x0263, GLYPH_gammalatinsmall, },
    { 0x02e0, GLYPH_gammasuperior, },
    { 0x03eb, GLYPH_gangiacoptic, },
    { 0x310d, GLYPH_gbopomofo, },
    { 0x011f, GLYPH_gbreve, },
    { 0x01e7, GLYPH_gcaron, },
    { 0x0123, GLYPH_gcedilla, },
    { 0x24d6, GLYPH_gcircle, },
    { 0x011d, GLYPH_gcircumflex, },
    { 0x0123, GLYPH_gcommaaccent, },
    { 0x0121, GLYPH_gdot, },
    { 0x0121, GLYPH_gdotaccent, },
    { 0x0433, GLYPH_gecyrillic, },
    { 0x3052, GLYPH_gehiragana, },
    { 0x30b2, GLYPH_gekatakana, },
    { 0x2251, GLYPH_geometricallyequal, },
    { 0x059c, GLYPH_gereshaccenthebrew, },
    { 0x05f3, GLYPH_gereshhebrew, },
    { 0x059d, GLYPH_gereshmuqdamhebrew, },
    { 0x00df, GLYPH_germandbls, },
    { 0x059e, GLYPH_gershayimaccenthebrew, },
    { 0x05f4, GLYPH_gershayimhebrew, },
    { 0x3013, GLYPH_getamark, },
    { 0x0998, GLYPH_ghabengali, },
    { 0x0572, GLYPH_ghadarmenian, },
    { 0x0918, GLYPH_ghadeva, },
    { 0x0a98, GLYPH_ghagujarati, },
    { 0x0a18, GLYPH_ghagurmukhi, },
    { 0x063a, GLYPH_ghainarabic, },
    { 0xfece, GLYPH_ghainfinalarabic, },
    { 0xfecf, GLYPH_ghaininitialarabic, },
    { 0xfed0, GLYPH_ghainmedialarabic, },
    { 0x0495, GLYPH_ghemiddlehookcyrillic, },
    { 0x0493, GLYPH_ghestrokecyrillic, },
    { 0x0491, GLYPH_gheupturncyrillic, },
    { 0x095a, GLYPH_ghhadeva, },
    { 0x0a5a, GLYPH_ghhagurmukhi, },
    { 0x0260, GLYPH_ghook, },
    { 0x3393, GLYPH_ghzsquare, },
    { 0x304e, GLYPH_gihiragana, },
    { 0x30ae, GLYPH_gikatakana, },
    { 0x0563, GLYPH_gimarmenian, },
    { 0x05d2, GLYPH_gimel, },
    { 0xfb32, GLYPH_gimeldagesh, },
    { 0xfb32, GLYPH_gimeldageshhebrew, },
    { 0x05d2, GLYPH_gimelhebrew, },
    { 0x0453, GLYPH_gjecyrillic, },
    { 0x01be, GLYPH_glottalinvertedstroke, },
    { 0x0294, GLYPH_glottalstop, },
    { 0x0296, GLYPH_glottalstopinverted, },
    { 0x02c0, GLYPH_glottalstopmod, },
    { 0x0295, GLYPH_glottalstopreversed, },
    { 0x02c1, GLYPH_glottalstopreversedmod, },
    { 0x02e4, GLYPH_glottalstopreversedsuperior, },
    { 0x02a1, GLYPH_glottalstopstroke, },
    { 0x02a2, GLYPH_glottalstopstrokereversed, },
    { 0x1e21, GLYPH_gmacron, },
    { 0xff47, GLYPH_gmonospace, },
    { 0x3054, GLYPH_gohiragana, },
    { 0x30b4, GLYPH_gokatakana, },
    { 0x24a2, GLYPH_gparen, },
    { 0x33ac, GLYPH_gpasquare, },
    { 0x2207, GLYPH_gradient, },
    { 0x0060, GLYPH_grave, },
    { 0x0316, GLYPH_gravebelowcmb, },
    { 0x0300, GLYPH_gravecmb, },
    { 0x0300, GLYPH_gravecomb, },
    { 0x0953, GLYPH_gravedeva, },
    { 0x02ce, GLYPH_gravelowmod, },
    { 0xff40, GLYPH_gravemonospace, },
    { 0x0340, GLYPH_gravetonecmb, },
    { 0x003e, GLYPH_greater, },
    { 0x2265, GLYPH_greaterequal, },
    { 0x22db, GLYPH_greaterequalorless, },
    { 0xff1e, GLYPH_greatermonospace, },
    { 0x2273, GLYPH_greaterorequivalent, },
    { 0x2277, GLYPH_greaterorless, },
    { 0x2267, GLYPH_greateroverequal, },
    { 0xfe65, GLYPH_greatersmall, },
    { 0x0261, GLYPH_gscript, },
    { 0x01e5, GLYPH_gstroke, },
    { 0x3050, GLYPH_guhiragana, },
    { 0x00ab, GLYPH_guillemotleft, },
    { 0x00bb, GLYPH_guillemotright, },
    { 0x2039, GLYPH_guilsinglleft, },
    { 0x203a, GLYPH_guilsinglright, },
    { 0x30b0, GLYPH_gukatakana, },
    { 0x3318, GLYPH_guramusquare, },
    { 0x33c9, GLYPH_gysquare, },
    { 0x0068, GLYPH_h, },
    { 0x04a9, GLYPH_haabkhasiancyrillic, },
    { 0x06c1, GLYPH_haaltonearabic, },
    { 0x09b9, GLYPH_habengali, },
    { 0x04b3, GLYPH_hadescendercyrillic, },
    { 0x0939, GLYPH_hadeva, },
    { 0x0ab9, GLYPH_hagujarati, },
    { 0x0a39, GLYPH_hagurmukhi, },
    { 0x062d, GLYPH_haharabic, },
    { 0xfea2, GLYPH_hahfinalarabic, },
    { 0xfea3, GLYPH_hahinitialarabic, },
    { 0x306f, GLYPH_hahiragana, },
    { 0xfea4, GLYPH_hahmedialarabic, },
    { 0x332a, GLYPH_haitusquare, },
    { 0x30cf, GLYPH_hakatakana, },
    { 0xff8a, GLYPH_hakatakanahalfwidth, },
    { 0x0a4d, GLYPH_halantgurmukhi, },
    { 0x0621, GLYPH_hamzaarabic, },
    { 0x0621, GLYPH_hamzalowarabic, },
    { 0x3164, GLYPH_hangulfiller, },
    { 0x044a, GLYPH_hardsigncyrillic, },
    { 0x21bc, GLYPH_harpoonleftbarbup, },
    { 0x21c0, GLYPH_harpoonrightbarbup, },
    { 0x33ca, GLYPH_hasquare, },
    { 0x05b2, GLYPH_hatafpatah, },
    { 0x05b2, GLYPH_hatafpatah16, },
    { 0x05b2, GLYPH_hatafpatah23, },
    { 0x05b2, GLYPH_hatafpatah2f, },
    { 0x05b2, GLYPH_hatafpatahhebrew, },
    { 0x05b2, GLYPH_hatafpatahnarrowhebrew, },
    { 0x05b2, GLYPH_hatafpatahquarterhebrew, },
    { 0x05b2, GLYPH_hatafpatahwidehebrew, },
    { 0x05b3, GLYPH_hatafqamats, },
    { 0x05b3, GLYPH_hatafqamats1b, },
    { 0x05b3, GLYPH_hatafqamats28, },
    { 0x05b3, GLYPH_hatafqamats34, },
    { 0x05b3, GLYPH_hatafqamatshebrew, },
    { 0x05b3, GLYPH_hatafqamatsnarrowhebrew, },
    { 0x05b3, GLYPH_hatafqamatsquarterhebrew, },
    { 0x05b3, GLYPH_hatafqamatswidehebrew, },
    { 0x05b1, GLYPH_hatafsegol, },
    { 0x05b1, GLYPH_hatafsegol17, },
    { 0x05b1, GLYPH_hatafsegol24, },
    { 0x05b1, GLYPH_hatafsegol30, },
    { 0x05b1, GLYPH_hatafsegolhebrew, },
    { 0x05b1, GLYPH_hatafsegolnarrowhebrew, },
    { 0x05b1, GLYPH_hatafsegolquarterhebrew, },
    { 0x05b1, GLYPH_hatafsegolwidehebrew, },
    { 0x0127, GLYPH_hbar, },
    { 0x310f, GLYPH_hbopomofo, },
    { 0x1e2b, GLYPH_hbrevebelow, },
    { 0x1e29, GLYPH_hcedilla, },
    { 0x24d7, GLYPH_hcircle, },
    { 0x0125, GLYPH_hcircumflex, },
    { 0x1e27, GLYPH_hdieresis, },
    { 0x1e23, GLYPH_hdotaccent, },
    { 0x1e25, GLYPH_hdotbelow, },
    { 0x05d4, GLYPH_he, },
    { 0x2665, GLYPH_heart, },
    { 0x2665, GLYPH_heartsuitblack, },
    { 0x2661, GLYPH_heartsuitwhite, },
    { 0xfb34, GLYPH_hedagesh, },
    { 0xfb34, GLYPH_hedageshhebrew, },
    { 0x06c1, GLYPH_hehaltonearabic, },
    { 0x0647, GLYPH_heharabic, },
    { 0x05d4, GLYPH_hehebrew, },
    { 0xfba7, GLYPH_hehfinalaltonearabic, },
    { 0xfeea, GLYPH_hehfinalalttwoarabic, },
    { 0xfeea, GLYPH_hehfinalarabic, },
    { 0xfba5, GLYPH_hehhamzaabovefinalarabic, },
    { 0xfba4, GLYPH_hehhamzaaboveisolatedarabic, },
    { 0xfba8, GLYPH_hehinitialaltonearabic, },
    { 0xfeeb, GLYPH_hehinitialarabic, },
    { 0x3078, GLYPH_hehiragana, },
    { 0xfba9, GLYPH_hehmedialaltonearabic, },
    { 0xfeec, GLYPH_hehmedialarabic, },
    { 0x337b, GLYPH_heiseierasquare, },
    { 0x30d8, GLYPH_hekatakana, },
    { 0xff8d, GLYPH_hekatakanahalfwidth, },
    { 0x3336, GLYPH_hekutaarusquare, },
    { 0x0267, GLYPH_henghook, },
    { 0x3339, GLYPH_herutusquare, },
    { 0x05d7, GLYPH_het, },
    { 0x05d7, GLYPH_hethebrew, },
    { 0x0266, GLYPH_hhook, },
    { 0x02b1, GLYPH_hhooksuperior, },
    { 0x327b, GLYPH_hieuhacirclekorean, },
    { 0x321b, GLYPH_hieuhaparenkorean, },
    { 0x326d, GLYPH_hieuhcirclekorean, },
    { 0x314e, GLYPH_hieuhkorean, },
    { 0x320d, GLYPH_hieuhparenkorean, },
    { 0x3072, GLYPH_hihiragana, },
    { 0x30d2, GLYPH_hikatakana, },
    { 0xff8b, GLYPH_hikatakanahalfwidth, },
    { 0x05b4, GLYPH_hiriq, },
    { 0x05b4, GLYPH_hiriq14, },
    { 0x05b4, GLYPH_hiriq21, },
    { 0x05b4, GLYPH_hiriq2d, },
    { 0x05b4, GLYPH_hiriqhebrew, },
    { 0x05b4, GLYPH_hiriqnarrowhebrew, },
    { 0x05b4, GLYPH_hiriqquarterhebrew, },
    { 0x05b4, GLYPH_hiriqwidehebrew, },
    { 0x1e96, GLYPH_hlinebelow, },
    { 0xff48, GLYPH_hmonospace, },
    { 0x0570, GLYPH_hoarmenian, },
    { 0x0e2b, GLYPH_hohipthai, },
    { 0x307b, GLYPH_hohiragana, },
    { 0x30db, GLYPH_hokatakana, },
    { 0xff8e, GLYPH_hokatakanahalfwidth, },
    { 0x05b9, GLYPH_holam, },
    { 0x05b9, GLYPH_holam19, },
    { 0x05b9, GLYPH_holam26, },
    { 0x05b9, GLYPH_holam32, },
    { 0x05b9, GLYPH_holamhebrew, },
    { 0x05b9, GLYPH_holamnarrowhebrew, },
    { 0x05b9, GLYPH_holamquarterhebrew, },
    { 0x05b9, GLYPH_holamwidehebrew, },
    { 0x0e2e, GLYPH_honokhukthai, },
    { 0x0309, GLYPH_hookabovecomb, },
    { 0x0309, GLYPH_hookcmb, },
    { 0x0321, GLYPH_hookpalatalizedbelowcmb, },
    { 0x0322, GLYPH_hookretroflexbelowcmb, },
    { 0x3342, GLYPH_hoonsquare, },
    { 0x03e9, GLYPH_horicoptic, },
    { 0x2015, GLYPH_horizontalbar, },
    { 0x031b, GLYPH_horncmb, },
    { 0x2668, GLYPH_hotsprings, },
    { 0x2302, GLYPH_house, },
    { 0x24a3, GLYPH_hparen, },
    { 0x02b0, GLYPH_hsuperior, },
    { 0x0265, GLYPH_hturned, },
    { 0x3075, GLYPH_huhiragana, },
    { 0x3333, GLYPH_huiitosquare, },
    { 0x30d5, GLYPH_hukatakana, },
    { 0xff8c, GLYPH_hukatakanahalfwidth, },
    { 0x02dd, GLYPH_hungarumlaut, },
    { 0x030b, GLYPH_hungarumlautcmb, },
    { 0x0195, GLYPH_hv, },
    { 0x002d, GLYPH_hyphen, },
    { 0xf6e5, GLYPH_hypheninferior, },
    { 0xff0d, GLYPH_hyphenmonospace, },
    { 0xfe63, GLYPH_hyphensmall, },
    { 0xf6e6, GLYPH_hyphensuperior, },
    { 0x2010, GLYPH_hyphentwo, },
    { 0x0069, GLYPH_i, },
    { 0x00ed, GLYPH_iacute, },
    { 0x044f, GLYPH_iacyrillic, },
    { 0x0987, GLYPH_ibengali, },
    { 0x3127, GLYPH_ibopomofo, },
    { 0x012d, GLYPH_ibreve, },
    { 0x01d0, GLYPH_icaron, },
    { 0x24d8, GLYPH_icircle, },
    { 0x00ee, GLYPH_icircumflex, },
    { 0x0456, GLYPH_icyrillic, },
    { 0x0209, GLYPH_idblgrave, },
    { 0x328f, GLYPH_ideographearthcircle, },
    { 0x328b, GLYPH_ideographfirecircle, },
    { 0x323f, GLYPH_ideographicallianceparen, },
    { 0x323a, GLYPH_ideographiccallparen, },
    { 0x32a5, GLYPH_ideographiccentrecircle, },
    { 0x3006, GLYPH_ideographicclose, },
    { 0x3001, GLYPH_ideographiccomma, },
    { 0xff64, GLYPH_ideographiccommaleft, },
    { 0x3237, GLYPH_ideographiccongratulationparen, },
    { 0x32a3, GLYPH_ideographiccorrectcircle, },
    { 0x322f, GLYPH_ideographicearthparen, },
    { 0x323d, GLYPH_ideographicenterpriseparen, },
    { 0x329d, GLYPH_ideographicexcellentcircle, },
    { 0x3240, GLYPH_ideographicfestivalparen, },
    { 0x3296, GLYPH_ideographicfinancialcircle, },
    { 0x3236, GLYPH_ideographicfinancialparen, },
    { 0x322b, GLYPH_ideographicfireparen, },
    { 0x3232, GLYPH_ideographichaveparen, },
    { 0x32a4, GLYPH_ideographichighcircle, },
    { 0x3005, GLYPH_ideographiciterationmark, },
    { 0x3298, GLYPH_ideographiclaborcircle, },
    { 0x3238, GLYPH_ideographiclaborparen, },
    { 0x32a7, GLYPH_ideographicleftcircle, },
    { 0x32a6, GLYPH_ideographiclowcircle, },
    { 0x32a9, GLYPH_ideographicmedicinecircle, },
    { 0x322e, GLYPH_ideographicmetalparen, },
    { 0x322a, GLYPH_ideographicmoonparen, },
    { 0x3234, GLYPH_ideographicnameparen, },
    { 0x3002, GLYPH_ideographicperiod, },
    { 0x329e, GLYPH_ideographicprintcircle, },
    { 0x3243, GLYPH_ideographicreachparen, },
    { 0x3239, GLYPH_ideographicrepresentparen, },
    { 0x323e, GLYPH_ideographicresourceparen, },
    { 0x32a8, GLYPH_ideographicrightcircle, },
    { 0x3299, GLYPH_ideographicsecretcircle, },
    { 0x3242, GLYPH_ideographicselfparen, },
    { 0x3233, GLYPH_ideographicsocietyparen, },
    { 0x3000, GLYPH_ideographicspace, },
    { 0x3235, GLYPH_ideographicspecialparen, },
    { 0x3231, GLYPH_ideographicstockparen, },
    { 0x323b, GLYPH_ideographicstudyparen, },
    { 0x3230, GLYPH_ideographicsunparen, },
    { 0x323c, GLYPH_ideographicsuperviseparen, },
    { 0x322c, GLYPH_ideographicwaterparen, },
    { 0x322d, GLYPH_ideographicwoodparen, },
    { 0x3007, GLYPH_ideographiczero, },
    { 0x328e, GLYPH_ideographmetalcircle, },
    { 0x328a, GLYPH_ideographmooncircle, },
    { 0x3294, GLYPH_ideographnamecircle, },
    { 0x3290, GLYPH_ideographsuncircle, },
    { 0x328c, GLYPH_ideographwatercircle, },
    { 0x328d, GLYPH_ideographwoodcircle, },
    { 0x0907, GLYPH_ideva, },
    { 0x00ef, GLYPH_idieresis, },
    { 0x1e2f, GLYPH_idieresisacute, },
    { 0x04e5, GLYPH_idieresiscyrillic, },
    { 0x1ecb, GLYPH_idotbelow, },
    { 0x04d7, GLYPH_iebrevecyrillic, },
    { 0x0435, GLYPH_iecyrillic, },
    { 0x3275, GLYPH_ieungacirclekorean, },
    { 0x3215, GLYPH_ieungaparenkorean, },
    { 0x3267, GLYPH_ieungcirclekorean, },
    { 0x3147, GLYPH_ieungkorean, },
    { 0x3207, GLYPH_ieungparenkorean, },
    { 0x00ec, GLYPH_igrave, },
    { 0x0a87, GLYPH_igujarati, },
    { 0x0a07, GLYPH_igurmukhi, },
    { 0x3044, GLYPH_ihiragana, },
    { 0x1ec9, GLYPH_ihookabove, },
    { 0x0988, GLYPH_iibengali, },
    { 0x0438, GLYPH_iicyrillic, },
    { 0x0908, GLYPH_iideva, },
    { 0x0a88, GLYPH_iigujarati, },
    { 0x0a08, GLYPH_iigurmukhi, },
    { 0x0a40, GLYPH_iimatragurmukhi, },
    { 0x020b, GLYPH_iinvertedbreve, },
    { 0x0439, GLYPH_iishortcyrillic, },
    { 0x09c0, GLYPH_iivowelsignbengali, },
    { 0x0940, GLYPH_iivowelsigndeva, },
    { 0x0ac0, GLYPH_iivowelsigngujarati, },
    { 0x0133, GLYPH_ij, },
    { 0x30a4, GLYPH_ikatakana, },
    { 0xff72, GLYPH_ikatakanahalfwidth, },
    { 0x3163, GLYPH_ikorean, },
    { 0x02dc, GLYPH_ilde, },
    { 0x05ac, GLYPH_iluyhebrew, },
    { 0x012b, GLYPH_imacron, },
    { 0x04e3, GLYPH_imacroncyrillic, },
    { 0x2253, GLYPH_imageorapproximatelyequal, },
    { 0x0a3f, GLYPH_imatragurmukhi, },
    { 0xff49, GLYPH_imonospace, },
    { 0x2206, GLYPH_increment, },
    { 0x221e, GLYPH_infinity, },
    { 0x056b, GLYPH_iniarmenian, },
    { 0x222b, GLYPH_integral, },
    { 0x2321, GLYPH_integralbottom, },
    { 0x2321, GLYPH_integralbt, },
    { 0xf8f5, GLYPH_integralex, },
    { 0x2320, GLYPH_integraltop, },
    { 0x2320, GLYPH_integraltp, },
    { 0x2229, GLYPH_intersection, },
    { 0x3305, GLYPH_intisquare, },
    { 0x25d8, GLYPH_invbullet, },
    { 0x25d9, GLYPH_invcircle, },
    { 0x263b, GLYPH_invsmileface, },
    { 0x0451, GLYPH_iocyrillic, },
    { 0x012f, GLYPH_iogonek, },
    { 0x03b9, GLYPH_iota, },
    { 0x03ca, GLYPH_iotadieresis, },
    { 0x0390, GLYPH_iotadieresistonos, },
    { 0x0269, GLYPH_iotalatin, },
    { 0x03af, GLYPH_iotatonos, },
    { 0x24a4, GLYPH_iparen, },
    { 0x0a72, GLYPH_irigurmukhi, },
    { 0x3043, GLYPH_ismallhiragana, },
    { 0x30a3, GLYPH_ismallkatakana, },
    { 0xff68, GLYPH_ismallkatakanahalfwidth, },
    { 0x09fa, GLYPH_issharbengali, },
    { 0x0268, GLYPH_istroke, },
    { 0xf6ed, GLYPH_isuperior, },
    { 0x309d, GLYPH_iterationhiragana, },
    { 0x30fd, GLYPH_iterationkatakana, },
    { 0x0129, GLYPH_itilde, },
    { 0x1e2d, GLYPH_itildebelow, },
    { 0x3129, GLYPH_iubopomofo, },
    { 0x044e, GLYPH_iucyrillic, },
    { 0x09bf, GLYPH_ivowelsignbengali, },
    { 0x093f, GLYPH_ivowelsigndeva, },
    { 0x0abf, GLYPH_ivowelsigngujarati, },
    { 0x0475, GLYPH_izhitsacyrillic, },
    { 0x0477, GLYPH_izhitsadblgravecyrillic, },
    { 0x006a, GLYPH_j, },
    { 0x0571, GLYPH_jaarmenian, },
    { 0x099c, GLYPH_jabengali, },
    { 0x091c, GLYPH_jadeva, },
    { 0x0a9c, GLYPH_jagujarati, },
    { 0x0a1c, GLYPH_jagurmukhi, },
    { 0x3110, GLYPH_jbopomofo, },
    { 0x01f0, GLYPH_jcaron, },
    { 0x24d9, GLYPH_jcircle, },
    { 0x0135, GLYPH_jcircumflex, },
    { 0x029d, GLYPH_jcrossedtail, },
    { 0x025f, GLYPH_jdotlessstroke, },
    { 0x0458, GLYPH_jecyrillic, },
    { 0x062c, GLYPH_jeemarabic, },
    { 0xfe9e, GLYPH_jeemfinalarabic, },
    { 0xfe9f, GLYPH_jeeminitialarabic, },
    { 0xfea0, GLYPH_jeemmedialarabic, },
    { 0x0698, GLYPH_jeharabic, },
    { 0xfb8b, GLYPH_jehfinalarabic, },
    { 0x099d, GLYPH_jhabengali, },
    { 0x091d, GLYPH_jhadeva, },
    { 0x0a9d, GLYPH_jhagujarati, },
    { 0x0a1d, GLYPH_jhagurmukhi, },
    { 0x057b, GLYPH_jheharmenian, },
    { 0x3004, GLYPH_jis, },
    { 0xff4a, GLYPH_jmonospace, },
    { 0x24a5, GLYPH_jparen, },
    { 0x02b2, GLYPH_jsuperior, },
    { 0x006b, GLYPH_k, },
    { 0x04a1, GLYPH_kabashkircyrillic, },
    { 0x0995, GLYPH_kabengali, },
    { 0x1e31, GLYPH_kacute, },
    { 0x043a, GLYPH_kacyrillic, },
    { 0x049b, GLYPH_kadescendercyrillic, },
    { 0x0915, GLYPH_kadeva, },
    { 0x05db, GLYPH_kaf, },
    { 0x0643, GLYPH_kafarabic, },
    { 0xfb3b, GLYPH_kafdagesh, },
    { 0xfb3b, GLYPH_kafdageshhebrew, },
    { 0xfeda, GLYPH_kaffinalarabic, },
    { 0x05db, GLYPH_kafhebrew, },
    { 0xfedb, GLYPH_kafinitialarabic, },
    { 0xfedc, GLYPH_kafmedialarabic, },
    { 0xfb4d, GLYPH_kafrafehebrew, },
    { 0x0a95, GLYPH_kagujarati, },
    { 0x0a15, GLYPH_kagurmukhi, },
    { 0x304b, GLYPH_kahiragana, },
    { 0x04c4, GLYPH_kahookcyrillic, },
    { 0x30ab, GLYPH_kakatakana, },
    { 0xff76, GLYPH_kakatakanahalfwidth, },
    { 0x03ba, GLYPH_kappa, },
    { 0x03f0, GLYPH_kappasymbolgreek, },
    { 0x3171, GLYPH_kapyeounmieumkorean, },
    { 0x3184, GLYPH_kapyeounphieuphkorean, },
    { 0x3178, GLYPH_kapyeounpieupkorean, },
    { 0x3179, GLYPH_kapyeounssangpieupkorean, },
    { 0x330d, GLYPH_karoriisquare, },
    { 0x0640, GLYPH_kashidaautoarabic, },
    { 0x0640, GLYPH_kashidaautonosidebearingarabic, },
    { 0x30f5, GLYPH_kasmallkatakana, },
    { 0x3384, GLYPH_kasquare, },
    { 0x0650, GLYPH_kasraarabic, },
    { 0x064d, GLYPH_kasratanarabic, },
    { 0x049f, GLYPH_kastrokecyrillic, },
    { 0xff70, GLYPH_katahiraprolongmarkhalfwidth, },
    { 0x049d, GLYPH_kaverticalstrokecyrillic, },
    { 0x310e, GLYPH_kbopomofo, },
    { 0x3389, GLYPH_kcalsquare, },
    { 0x01e9, GLYPH_kcaron, },
    { 0x0137, GLYPH_kcedilla, },
    { 0x24da, GLYPH_kcircle, },
    { 0x0137, GLYPH_kcommaaccent, },
    { 0x1e33, GLYPH_kdotbelow, },
    { 0x0584, GLYPH_keharmenian, },
    { 0x3051, GLYPH_kehiragana, },
    { 0x30b1, GLYPH_kekatakana, },
    { 0xff79, GLYPH_kekatakanahalfwidth, },
    { 0x056f, GLYPH_kenarmenian, },
    { 0x30f6, GLYPH_kesmallkatakana, },
    { 0x0138, GLYPH_kgreenlandic, },
    { 0x0996, GLYPH_khabengali, },
    { 0x0445, GLYPH_khacyrillic, },
    { 0x0916, GLYPH_khadeva, },
    { 0x0a96, GLYPH_khagujarati, },
    { 0x0a16, GLYPH_khagurmukhi, },
    { 0x062e, GLYPH_khaharabic, },
    { 0xfea6, GLYPH_khahfinalarabic, },
    { 0xfea7, GLYPH_khahinitialarabic, },
    { 0xfea8, GLYPH_khahmedialarabic, },
    { 0x03e7, GLYPH_kheicoptic, },
    { 0x0959, GLYPH_khhadeva, },
    { 0x0a59, GLYPH_khhagurmukhi, },
    { 0x3278, GLYPH_khieukhacirclekorean, },
    { 0x3218, GLYPH_khieukhaparenkorean, },
    { 0x326a, GLYPH_khieukhcirclekorean, },
    { 0x314b, GLYPH_khieukhkorean, },
    { 0x320a, GLYPH_khieukhparenkorean, },
    { 0x0e02, GLYPH_khokhaithai, },
    { 0x0e05, GLYPH_khokhonthai, },
    { 0x0e03, GLYPH_khokhuatthai, },
    { 0x0e04, GLYPH_khokhwaithai, },
    { 0x0e5b, GLYPH_khomutthai, },
    { 0x0199, GLYPH_khook, },
    { 0x0e06, GLYPH_khorakhangthai, },
    { 0x3391, GLYPH_khzsquare, },
    { 0x304d, GLYPH_kihiragana, },
    { 0x30ad, GLYPH_kikatakana, },
    { 0xff77, GLYPH_kikatakanahalfwidth, },
    { 0x3315, GLYPH_kiroguramusquare, },
    { 0x3316, GLYPH_kiromeetorusquare, },
    { 0x3314, GLYPH_kirosquare, },
    { 0x326e, GLYPH_kiyeokacirclekorean, },
    { 0x320e, GLYPH_kiyeokaparenkorean, },
    { 0x3260, GLYPH_kiyeokcirclekorean, },
    { 0x3131, GLYPH_kiyeokkorean, },
    { 0x3200, GLYPH_kiyeokparenkorean, },
    { 0x3133, GLYPH_kiyeoksioskorean, },
    { 0x045c, GLYPH_kjecyrillic, },
    { 0x1e35, GLYPH_klinebelow, },
    { 0x3398, GLYPH_klsquare, },
    { 0x33a6, GLYPH_kmcubedsquare, },
    { 0xff4b, GLYPH_kmonospace, },
    { 0x33a2, GLYPH_kmsquaredsquare, },
    { 0x3053, GLYPH_kohiragana, },
    { 0x33c0, GLYPH_kohmsquare, },
    { 0x0e01, GLYPH_kokaithai, },
    { 0x30b3, GLYPH_kokatakana, },
    { 0xff7a, GLYPH_kokatakanahalfwidth, },
    { 0x331e, GLYPH_kooposquare, },
    { 0x0481, GLYPH_koppacyrillic, },
    { 0x327f, GLYPH_koreanstandardsymbol, },
    { 0x0343, GLYPH_koroniscmb, },
    { 0x24a6, GLYPH_kparen, },
    { 0x33aa, GLYPH_kpasquare, },
    { 0x046f, GLYPH_ksicyrillic, },
    { 0x33cf, GLYPH_ktsquare, },
    { 0x029e, GLYPH_kturned, },
    { 0x304f, GLYPH_kuhiragana, },
    { 0x30af, GLYPH_kukatakana, },
    { 0xff78, GLYPH_kukatakanahalfwidth, },
    { 0x33b8, GLYPH_kvsquare, },
    { 0x33be, GLYPH_kwsquare, },
    { 0x006c, GLYPH_l, },
    { 0x09b2, GLYPH_labengali, },
    { 0x013a, GLYPH_lacute, },
    { 0x0932, GLYPH_ladeva, },
    { 0x0ab2, GLYPH_lagujarati, },
    { 0x0a32, GLYPH_lagurmukhi, },
    { 0x0e45, GLYPH_lakkhangyaothai, },
    { 0xfefc, GLYPH_lamaleffinalarabic, },
    { 0xfef8, GLYPH_lamalefhamzaabovefinalarabic, },
    { 0xfef7, GLYPH_lamalefhamzaaboveisolatedarabic, },
    { 0xfefa, GLYPH_lamalefhamzabelowfinalarabic, },
    { 0xfef9, GLYPH_lamalefhamzabelowisolatedarabic, },
    { 0xfefb, GLYPH_lamalefisolatedarabic, },
    { 0xfef6, GLYPH_lamalefmaddaabovefinalarabic, },
    { 0xfef5, GLYPH_lamalefmaddaaboveisolatedarabic, },
    { 0x0644, GLYPH_lamarabic, },
    { 0x03bb, GLYPH_lambda, },
    { 0x019b, GLYPH_lambdastroke, },
    { 0x05dc, GLYPH_lamed, },
    { 0xfb3c, GLYPH_lameddagesh, },
    { 0xfb3c, GLYPH_lameddageshhebrew, },
    { 0x05dc, GLYPH_lamedhebrew, },
    { 0xfede, GLYPH_lamfinalarabic, },
    { 0xfcca, GLYPH_lamhahinitialarabic, },
    { 0xfedf, GLYPH_laminitialarabic, },
    { 0xfcc9, GLYPH_lamjeeminitialarabic, },
    { 0xfccb, GLYPH_lamkhahinitialarabic, },
    { 0xfdf2, GLYPH_lamlamhehisolatedarabic, },
    { 0xfee0, GLYPH_lammedialarabic, },
    { 0xfd88, GLYPH_lammeemhahinitialarabic, },
    { 0xfccc, GLYPH_lammeeminitialarabic, },
    { 0x25ef, GLYPH_largecircle, },
    { 0x019a, GLYPH_lbar, },
    { 0x026c, GLYPH_lbelt, },
    { 0x310c, GLYPH_lbopomofo, },
    { 0x013e, GLYPH_lcaron, },
    { 0x013c, GLYPH_lcedilla, },
    { 0x24db, GLYPH_lcircle, },
    { 0x1e3d, GLYPH_lcircumflexbelow, },
    { 0x013c, GLYPH_lcommaaccent, },
    { 0x0140, GLYPH_ldot, },
    { 0x0140, GLYPH_ldotaccent, },
    { 0x1e37, GLYPH_ldotbelow, },
    { 0x1e39, GLYPH_ldotbelowmacron, },
    { 0x031a, GLYPH_leftangleabovecmb, },
    { 0x0318, GLYPH_lefttackbelowcmb, },
    { 0x003c, GLYPH_less, },
    { 0x2264, GLYPH_lessequal, },
    { 0x22da, GLYPH_lessequalorgreater, },
    { 0xff1c, GLYPH_lessmonospace, },
    { 0x2272, GLYPH_lessorequivalent, },
    { 0x2276, GLYPH_lessorgreater, },
    { 0x2266, GLYPH_lessoverequal, },
    { 0xfe64, GLYPH_lesssmall, },
    { 0x026e, GLYPH_lezh, },
    { 0x258c, GLYPH_lfblock, },
    { 0x026d, GLYPH_lhookretroflex, },
    { 0x20a4, GLYPH_lira, },
    { 0x056c, GLYPH_liwnarmenian, },
    { 0x01c9, GLYPH_lj, },
    { 0x0459, GLYPH_ljecyrillic, },
    { 0xf6c0, GLYPH_ll, },
    { 0x0933, GLYPH_lladeva, },
    { 0x0ab3, GLYPH_llagujarati, },
    { 0x1e3b, GLYPH_llinebelow, },
    { 0x0934, GLYPH_llladeva, },
    { 0x09e1, GLYPH_llvocalicbengali, },
    { 0x0961, GLYPH_llvocalicdeva, },
    { 0x09e3, GLYPH_llvocalicvowelsignbengali, },
    { 0x0963, GLYPH_llvocalicvowelsigndeva, },
    { 0x026b, GLYPH_lmiddletilde, },
    { 0xff4c, GLYPH_lmonospace, },
    { 0x33d0, GLYPH_lmsquare, },
    { 0x0e2c, GLYPH_lochulathai, },
    { 0x2227, GLYPH_logicaland, },
    { 0x00ac, GLYPH_logicalnot, },
    { 0x2310, GLYPH_logicalnotreversed, },
    { 0x2228, GLYPH_logicalor, },
    { 0x0e25, GLYPH_lolingthai, },
    { 0x017f, GLYPH_longs, },
    { 0x005f, GLYPH_lowline, },
    { 0xfe4e, GLYPH_lowlinecenterline, },
    { 0x0332, GLYPH_lowlinecmb, },
    { 0xfe4d, GLYPH_lowlinedashed, },
    { 0x25ca, GLYPH_lozenge, },
    { 0x24a7, GLYPH_lparen, },
    { 0x0142, GLYPH_lslash, },
    { 0x2113, GLYPH_lsquare, },
    { 0x0142, GLYPH_lstroke, },
    { 0xf6ee, GLYPH_lsuperior, },
    { 0x2591, GLYPH_ltshade, },
    { 0x0e26, GLYPH_luthai, },
    { 0x098c, GLYPH_lvocalicbengali, },
    { 0x090c, GLYPH_lvocalicdeva, },
    { 0x09e2, GLYPH_lvocalicvowelsignbengali, },
    { 0x0962, GLYPH_lvocalicvowelsigndeva, },
    { 0x33d3, GLYPH_lxsquare, },
    { 0x006d, GLYPH_m, },
    { 0x09ae, GLYPH_mabengali, },
    { 0x00af, GLYPH_macron, },
    { 0x0331, GLYPH_macronbelowcmb, },
    { 0x0304, GLYPH_macroncmb, },
    { 0x02cd, GLYPH_macronlowmod, },
    { 0xffe3, GLYPH_macronmonospace, },
    { 0x1e3f, GLYPH_macute, },
    { 0x092e, GLYPH_madeva, },
    { 0x0aae, GLYPH_magujarati, },
    { 0x0a2e, GLYPH_magurmukhi, },
    { 0x05a4, GLYPH_mahapakhhebrew, },
    { 0x05a4, GLYPH_mahapakhlefthebrew, },
    { 0x307e, GLYPH_mahiragana, },
    { 0xf895, GLYPH_maichattawalowleftthai, },
    { 0xf894, GLYPH_maichattawalowrightthai, },
    { 0x0e4b, GLYPH_maichattawathai, },
    { 0xf893, GLYPH_maichattawaupperleftthai, },
    { 0xf88c, GLYPH_maieklowleftthai, },
    { 0xf88b, GLYPH_maieklowrightthai, },
    { 0x0e48, GLYPH_maiekthai, },
    { 0xf88a, GLYPH_maiekupperleftthai, },
    { 0xf884, GLYPH_maihanakatleftthai, },
    { 0x0e31, GLYPH_maihanakatthai, },
    { 0xf889, GLYPH_maitaikhuleftthai, },
    { 0x0e47, GLYPH_maitaikhuthai, },
    { 0xf88f, GLYPH_maitholowleftthai, },
    { 0xf88e, GLYPH_maitholowrightthai, },
    { 0x0e49, GLYPH_maithothai, },
    { 0xf88d, GLYPH_maithoupperleftthai, },
    { 0xf892, GLYPH_maitrilowleftthai, },
    { 0xf891, GLYPH_maitrilowrightthai, },
    { 0x0e4a, GLYPH_maitrithai, },
    { 0xf890, GLYPH_maitriupperleftthai, },
    { 0x0e46, GLYPH_maiyamokthai, },
    { 0x30de, GLYPH_makatakana, },
    { 0xff8f, GLYPH_makatakanahalfwidth, },
    { 0x2642, GLYPH_male, },
    { 0x3347, GLYPH_mansyonsquare, },
    { 0x05be, GLYPH_maqafhebrew, },
    { 0x2642, GLYPH_mars, },
    { 0x05af, GLYPH_masoracirclehebrew, },
    { 0x3383, GLYPH_masquare, },
    { 0x3107, GLYPH_mbopomofo, },
    { 0x33d4, GLYPH_mbsquare, },
    { 0x24dc, GLYPH_mcircle, },
    { 0x33a5, GLYPH_mcubedsquare, },
    { 0x1e41, GLYPH_mdotaccent, },
    { 0x1e43, GLYPH_mdotbelow, },
    { 0x0645, GLYPH_meemarabic, },
    { 0xfee2, GLYPH_meemfinalarabic, },
    { 0xfee3, GLYPH_meeminitialarabic, },
    { 0xfee4, GLYPH_meemmedialarabic, },
    { 0xfcd1, GLYPH_meemmeeminitialarabic, },
    { 0xfc48, GLYPH_meemmeemisolatedarabic, },
    { 0x334d, GLYPH_meetorusquare, },
    { 0x3081, GLYPH_mehiragana, },
    { 0x337e, GLYPH_meizierasquare, },
    { 0x30e1, GLYPH_mekatakana, },
    { 0xff92, GLYPH_mekatakanahalfwidth, },
    { 0x05de, GLYPH_mem, },
    { 0xfb3e, GLYPH_memdagesh, },
    { 0xfb3e, GLYPH_memdageshhebrew, },
    { 0x05de, GLYPH_memhebrew, },
    { 0x0574, GLYPH_menarmenian, },
    { 0x05a5, GLYPH_merkhahebrew, },
    { 0x05a6, GLYPH_merkhakefulahebrew, },
    { 0x05a6, GLYPH_merkhakefulalefthebrew, },
    { 0x05a5, GLYPH_merkhalefthebrew, },
    { 0x0271, GLYPH_mhook, },
    { 0x3392, GLYPH_mhzsquare, },
    { 0x00b5, GLYPH_micro, },
    { 0xff65, GLYPH_middledotkatakanahalfwidth, },
    { 0x00b7, GLYPH_middot, },
    { 0x3272, GLYPH_mieumacirclekorean, },
    { 0x3212, GLYPH_mieumaparenkorean, },
    { 0x3264, GLYPH_mieumcirclekorean, },
    { 0x3141, GLYPH_mieumkorean, },
    { 0x3170, GLYPH_mieumpansioskorean, },
    { 0x3204, GLYPH_mieumparenkorean, },
    { 0x316e, GLYPH_mieumpieupkorean, },
    { 0x316f, GLYPH_mieumsioskorean, },
    { 0x307f, GLYPH_mihiragana, },
    { 0x30df, GLYPH_mikatakana, },
    { 0xff90, GLYPH_mikatakanahalfwidth, },
    { 0x2212, GLYPH_minus, },
    { 0x0320, GLYPH_minusbelowcmb, },
    { 0x2296, GLYPH_minuscircle, },
    { 0x02d7, GLYPH_minusmod, },
    { 0x2213, GLYPH_minusplus, },
    { 0x2032, GLYPH_minute, },
    { 0x334a, GLYPH_miribaarusquare, },
    { 0x3349, GLYPH_mirisquare, },
    { 0x0270, GLYPH_mlonglegturned, },
    { 0x3396, GLYPH_mlsquare, },
    { 0x33a3, GLYPH_mmcubedsquare, },
    { 0xff4d, GLYPH_mmonospace, },
    { 0x339f, GLYPH_mmsquaredsquare, },
    { 0x3082, GLYPH_mohiragana, },
    { 0x33c1, GLYPH_mohmsquare, },
    { 0x30e2, GLYPH_mokatakana, },
    { 0xff93, GLYPH_mokatakanahalfwidth, },
    { 0x33d6, GLYPH_molsquare, },
    { 0x0e21, GLYPH_momathai, },
    { 0x33a7, GLYPH_moverssquare, },
    { 0x33a8, GLYPH_moverssquaredsquare, },
    { 0x24a8, GLYPH_mparen, },
    { 0x33ab, GLYPH_mpasquare, },
    { 0x33b3, GLYPH_mssquare, },
    { 0xf6ef, GLYPH_msuperior, },
    { 0x026f, GLYPH_mturned, },
    { 0x00b5, GLYPH_mu, },
    { 0x00b5, GLYPH_mu1, },
    { 0x3382, GLYPH_muasquare, },
    { 0x226b, GLYPH_muchgreater, },
    { 0x226a, GLYPH_muchless, },
    { 0x338c, GLYPH_mufsquare, },
    { 0x03bc, GLYPH_mugreek, },
    { 0x338d, GLYPH_mugsquare, },
    { 0x3080, GLYPH_muhiragana, },
    { 0x30e0, GLYPH_mukatakana, },
    { 0xff91, GLYPH_mukatakanahalfwidth, },
    { 0x3395, GLYPH_mulsquare, },
    { 0x00d7, GLYPH_multiply, },
    { 0x339b, GLYPH_mumsquare, },
    { 0x05a3, GLYPH_munahhebrew, },
    { 0x05a3, GLYPH_munahlefthebrew, },
    { 0x266a, GLYPH_musicalnote, },
    { 0x266b, GLYPH_musicalnotedbl, },
    { 0x266d, GLYPH_musicflatsign, },
    { 0x266f, GLYPH_musicsharpsign, },
    { 0x33b2, GLYPH_mussquare, },
    { 0x33b6, GLYPH_muvsquare, },
    { 0x33bc, GLYPH_muwsquare, },
    { 0x33b9, GLYPH_mvmegasquare, },
    { 0x33b7, GLYPH_mvsquare, },
    { 0x33bf, GLYPH_mwmegasquare, },
    { 0x33bd, GLYPH_mwsquare, },
    { 0x006e, GLYPH_n, },
    { 0x09a8, GLYPH_nabengali, },
    { 0x2207, GLYPH_nabla, },
    { 0x0144, GLYPH_nacute, },
    { 0x0928, GLYPH_nadeva, },
    { 0x0aa8, GLYPH_nagujarati, },
    { 0x0a28, GLYPH_nagurmukhi, },
    { 0x306a, GLYPH_nahiragana, },
    { 0x30ca, GLYPH_nakatakana, },
    { 0xff85, GLYPH_nakatakanahalfwidth, },
    { 0x0149, GLYPH_napostrophe, },
    { 0x3381, GLYPH_nasquare, },
    { 0x310b, GLYPH_nbopomofo, },
    { 0x00a0, GLYPH_nbspace, },
    { 0x0148, GLYPH_ncaron, },
    { 0x0146, GLYPH_ncedilla, },
    { 0x24dd, GLYPH_ncircle, },
    { 0x1e4b, GLYPH_ncircumflexbelow, },
    { 0x0146, GLYPH_ncommaaccent, },
    { 0x1e45, GLYPH_ndotaccent, },
    { 0x1e47, GLYPH_ndotbelow, },
    { 0x306d, GLYPH_nehiragana, },
    { 0x30cd, GLYPH_nekatakana, },
    { 0xff88, GLYPH_nekatakanahalfwidth, },
    { 0x20aa, GLYPH_newsheqelsign, },
    { 0x338b, GLYPH_nfsquare, },
    { 0x0999, GLYPH_ngabengali, },
    { 0x0919, GLYPH_ngadeva, },
    { 0x0a99, GLYPH_ngagujarati, },
    { 0x0a19, GLYPH_ngagurmukhi, },
    { 0x0e07, GLYPH_ngonguthai, },
    { 0x3093, GLYPH_nhiragana, },
    { 0x0272, GLYPH_nhookleft, },
    { 0x0273, GLYPH_nhookretroflex, },
    { 0x326f, GLYPH_nieunacirclekorean, },
    { 0x320f, GLYPH_nieunaparenkorean, },
    { 0x3135, GLYPH_nieuncieuckorean, },
    { 0x3261, GLYPH_nieuncirclekorean, },
    { 0x3136, GLYPH_nieunhieuhkorean, },
    { 0x3134, GLYPH_nieunkorean, },
    { 0x3168, GLYPH_nieunpansioskorean, },
    { 0x3201, GLYPH_nieunparenkorean, },
    { 0x3167, GLYPH_nieunsioskorean, },
    { 0x3166, GLYPH_nieuntikeutkorean, },
    { 0x306b, GLYPH_nihiragana, },
    { 0x30cb, GLYPH_nikatakana, },
    { 0xff86, GLYPH_nikatakanahalfwidth, },
    { 0xf899, GLYPH_nikhahitleftthai, },
    { 0x0e4d, GLYPH_nikhahitthai, },
    { 0x0039, GLYPH_nine, },
    { 0x0669, GLYPH_ninearabic, },
    { 0x09ef, GLYPH_ninebengali, },
    { 0x2468, GLYPH_ninecircle, },
    { 0x2792, GLYPH_ninecircleinversesansserif, },
    { 0x096f, GLYPH_ninedeva, },
    { 0x0aef, GLYPH_ninegujarati, },
    { 0x0a6f, GLYPH_ninegurmukhi, },
    { 0x0669, GLYPH_ninehackarabic, },
    { 0x3029, GLYPH_ninehangzhou, },
    { 0x3228, GLYPH_nineideographicparen, },
    { 0x2089, GLYPH_nineinferior, },
    { 0xff19, GLYPH_ninemonospace, },
    { 0xf739, GLYPH_nineoldstyle, },
    { 0x247c, GLYPH_nineparen, },
    { 0x2490, GLYPH_nineperiod, },
    { 0x06f9, GLYPH_ninepersian, },
    { 0x2178, GLYPH_nineroman, },
    { 0x2079, GLYPH_ninesuperior, },
    { 0x2472, GLYPH_nineteencircle, },
    { 0x2486, GLYPH_nineteenparen, },
    { 0x249a, GLYPH_nineteenperiod, },
    { 0x0e59, GLYPH_ninethai, },
    { 0x01cc, GLYPH_nj, },
    { 0x045a, GLYPH_njecyrillic, },
    { 0x30f3, GLYPH_nkatakana, },
    { 0xff9d, GLYPH_nkatakanahalfwidth, },
    { 0x019e, GLYPH_nlegrightlong, },
    { 0x1e49, GLYPH_nlinebelow, },
    { 0xff4e, GLYPH_nmonospace, },
    { 0x339a, GLYPH_nmsquare, },
    { 0x09a3, GLYPH_nnabengali, },
    { 0x0923, GLYPH_nnadeva, },
    { 0x0aa3, GLYPH_nnagujarati, },
    { 0x0a23, GLYPH_nnagurmukhi, },
    { 0x0929, GLYPH_nnnadeva, },
    { 0x0020, GLYPH_nobreakspace, },
    { 0x306e, GLYPH_nohiragana, },
    { 0x30ce, GLYPH_nokatakana, },
    { 0xff89, GLYPH_nokatakanahalfwidth, },
    { 0x00a0, GLYPH_nonbreakingspace, },
    { 0x0e13, GLYPH_nonenthai, },
    { 0x0e19, GLYPH_nonuthai, },
    { 0x0646, GLYPH_noonarabic, },
    { 0xfee6, GLYPH_noonfinalarabic, },
    { 0x06ba, GLYPH_noonghunnaarabic, },
    { 0xfb9f, GLYPH_noonghunnafinalarabic, },
    { 0xfee7, GLYPH_nooninitialarabic, },
    { 0xfcd2, GLYPH_noonjeeminitialarabic, },
    { 0xfc4b, GLYPH_noonjeemisolatedarabic, },
    { 0xfee8, GLYPH_noonmedialarabic, },
    { 0xfcd5, GLYPH_noonmeeminitialarabic, },
    { 0xfc4e, GLYPH_noonmeemisolatedarabic, },
    { 0xfc8d, GLYPH_noonnoonfinalarabic, },
    { 0x220c, GLYPH_notcontains, },
    { 0x2209, GLYPH_notelement, },
    { 0x2209, GLYPH_notelementof, },
    { 0x2260, GLYPH_notequal, },
    { 0x226f, GLYPH_notgreater, },
    { 0x2271, GLYPH_notgreaternorequal, },
    { 0x2279, GLYPH_notgreaternorless, },
    { 0x2262, GLYPH_notidentical, },
    { 0x226e, GLYPH_notless, },
    { 0x2270, GLYPH_notlessnorequal, },
    { 0x2226, GLYPH_notparallel, },
    { 0x2280, GLYPH_notprecedes, },
    { 0x2284, GLYPH_notsubset, },
    { 0x2281, GLYPH_notsucceeds, },
    { 0x2285, GLYPH_notsuperset, },
    { 0x0576, GLYPH_nowarmenian, },
    { 0x24a9, GLYPH_nparen, },
    { 0x33b1, GLYPH_nssquare, },
    { 0x207f, GLYPH_nsuperior, },
    { 0x00f1, GLYPH_ntilde, },
    { 0x03bd, GLYPH_nu, },
    { 0x306c, GLYPH_nuhiragana, },
    { 0x30cc, GLYPH_nukatakana, },
    { 0xff87, GLYPH_nukatakanahalfwidth, },
    { 0x09bc, GLYPH_nuktabengali, },
    { 0x093c, GLYPH_nuktadeva, },
    { 0x0abc, GLYPH_nuktagujarati, },
    { 0x0a3c, GLYPH_nuktagurmukhi, },
    { 0x0023, GLYPH_numbersign, },
    { 0xff03, GLYPH_numbersignmonospace, },
    { 0xfe5f, GLYPH_numbersignsmall, },
    { 0x0374, GLYPH_numeralsigngreek, },
    { 0x0375, GLYPH_numeralsignlowergreek, },
    { 0x2116, GLYPH_numero, },
    { 0x05e0, GLYPH_nun, },
    { 0xfb40, GLYPH_nundagesh, },
    { 0xfb40, GLYPH_nundageshhebrew, },
    { 0x05e0, GLYPH_nunhebrew, },
    { 0x33b5, GLYPH_nvsquare, },
    { 0x33bb, GLYPH_nwsquare, },
    { 0x099e, GLYPH_nyabengali, },
    { 0x091e, GLYPH_nyadeva, },
    { 0x0a9e, GLYPH_nyagujarati, },
    { 0x0a1e, GLYPH_nyagurmukhi, },
    { 0x006f, GLYPH_o, },
    { 0x00f3, GLYPH_oacute, },
    { 0x0e2d, GLYPH_oangthai, },
    { 0x0275, GLYPH_obarred, },
    { 0x04e9, GLYPH_obarredcyrillic, },
    { 0x04eb, GLYPH_obarreddieresiscyrillic, },
    { 0x0993, GLYPH_obengali, },
    { 0x311b, GLYPH_obopomofo, },
    { 0x014f, GLYPH_obreve, },
    { 0x0911, GLYPH_ocandradeva, },
    { 0x0a91, GLYPH_ocandragujarati, },
    { 0x0949, GLYPH_ocandravowelsigndeva, },
    { 0x0ac9, GLYPH_ocandravowelsigngujarati, },
    { 0x01d2, GLYPH_ocaron, },
    { 0x24de, GLYPH_ocircle, },
    { 0x00f4, GLYPH_ocircumflex, },
    { 0x1ed1, GLYPH_ocircumflexacute, },
    { 0x1ed9, GLYPH_ocircumflexdotbelow, },
    { 0x1ed3, GLYPH_ocircumflexgrave, },
    { 0x1ed5, GLYPH_ocircumflexhookabove, },
    { 0x1ed7, GLYPH_ocircumflextilde, },
    { 0x043e, GLYPH_ocyrillic, },
    { 0x0151, GLYPH_odblacute, },
    { 0x020d, GLYPH_odblgrave, },
    { 0x0913, GLYPH_odeva, },
    { 0x00f6, GLYPH_odiaeresis, },
    { 0x00f6, GLYPH_odieresis, },
    { 0x04e7, GLYPH_odieresiscyrillic, },
    { 0x1ecd, GLYPH_odotbelow, },
    { 0x0151, GLYPH_odoubleacute, },
    { 0x0153, GLYPH_oe, },
    { 0x315a, GLYPH_oekorean, },
    { 0x02db, GLYPH_ogonek, },
    { 0x0328, GLYPH_ogonekcmb, },
    { 0x00f2, GLYPH_ograve, },
    { 0x0a93, GLYPH_ogujarati, },
    { 0x0585, GLYPH_oharmenian, },
    { 0x304a, GLYPH_ohiragana, },
    { 0x1ecf, GLYPH_ohookabove, },
    { 0x01a1, GLYPH_ohorn, },
    { 0x1edb, GLYPH_ohornacute, },
    { 0x1ee3, GLYPH_ohorndotbelow, },
    { 0x1edd, GLYPH_ohorngrave, },
    { 0x1edf, GLYPH_ohornhookabove, },
    { 0x1ee1, GLYPH_ohorntilde, },
    { 0x0151, GLYPH_ohungar, },
    { 0x0151, GLYPH_ohungarumlaut, },
    { 0x01a3, GLYPH_oi, },
    { 0x020f, GLYPH_oinvertedbreve, },
    { 0x30aa, GLYPH_okatakana, },
    { 0xff75, GLYPH_okatakanahalfwidth, },
    { 0x3157, GLYPH_okorean, },
    { 0x05ab, GLYPH_olehebrew, },
    { 0x014d, GLYPH_omacron, },
    { 0x1e53, GLYPH_omacronacute, },
    { 0x1e51, GLYPH_omacrongrave, },
    { 0x0950, GLYPH_omdeva, },
    { 0x03c9, GLYPH_omega, },
    { 0x03d6, GLYPH_omega1, },
    { 0x0461, GLYPH_omegacyrillic, },
    { 0x0277, GLYPH_omegalatinclosed, },
    { 0x047b, GLYPH_omegaroundcyrillic, },
    { 0x047d, GLYPH_omegatitlocyrillic, },
    { 0x03ce, GLYPH_omegatonos, },
    { 0x0ad0, GLYPH_omgujarati, },
    { 0x03bf, GLYPH_omicron, },
    { 0x03cc, GLYPH_omicrontonos, },
    { 0xff4f, GLYPH_omonospace, },
    { 0x0031, GLYPH_one, },
    { 0x0661, GLYPH_onearabic, },
    { 0x09e7, GLYPH_onebengali, },
    { 0x2460, GLYPH_onecircle, },
    { 0x278a, GLYPH_onecircleinversesansserif, },
    { 0x0967, GLYPH_onedeva, },
    { 0x2024, GLYPH_onedotenleader, },
    { 0x215b, GLYPH_oneeighth, },
    { 0xf6dc, GLYPH_onefitted, },
    { 0x0ae7, GLYPH_onegujarati, },
    { 0x0a67, GLYPH_onegurmukhi, },
    { 0x0661, GLYPH_onehackarabic, },
    { 0x00bd, GLYPH_onehalf, },
    { 0x3021, GLYPH_onehangzhou, },
    { 0x3220, GLYPH_oneideographicparen, },
    { 0x2081, GLYPH_oneinferior, },
    { 0xff11, GLYPH_onemonospace, },
    { 0x09f4, GLYPH_onenumeratorbengali, },
    { 0xf731, GLYPH_oneoldstyle, },
    { 0x2474, GLYPH_oneparen, },
    { 0x2488, GLYPH_oneperiod, },
    { 0x06f1, GLYPH_onepersian, },
    { 0x00bc, GLYPH_onequarter, },
    { 0x2170, GLYPH_oneroman, },
    { 0x00b9, GLYPH_onesuperior, },
    { 0x0e51, GLYPH_onethai, },
    { 0x2153, GLYPH_onethird, },
    { 0x01eb, GLYPH_oogonek, },
    { 0x01ed, GLYPH_oogonekmacron, },
    { 0x0a13, GLYPH_oogurmukhi, },
    { 0x0a4b, GLYPH_oomatragurmukhi, },
    { 0x0254, GLYPH_oopen, },
    { 0x24aa, GLYPH_oparen, },
    { 0x25e6, GLYPH_openbullet, },
    { 0x2325, GLYPH_option, },
    { 0x00aa, GLYPH_ordfeminine, },
    { 0x00ba, GLYPH_ordmasculine, },
    { 0x221f, GLYPH_orthogonal, },
    { 0x0912, GLYPH_oshortdeva, },
    { 0x094a, GLYPH_oshortvowelsigndeva, },
    { 0x00f8, GLYPH_oslash, },
    { 0x01ff, GLYPH_oslashacute, },
    { 0x3049, GLYPH_osmallhiragana, },
    { 0x30a9, GLYPH_osmallkatakana, },
    { 0xff6b, GLYPH_osmallkatakanahalfwidth, },
    { 0x01ff, GLYPH_ostrokeacute, },
    { 0xf6f0, GLYPH_osuperior, },
    { 0x047f, GLYPH_otcyrillic, },
    { 0x00f5, GLYPH_otilde, },
    { 0x1e4d, GLYPH_otildeacute, },
    { 0x1e4f, GLYPH_otildedieresis, },
    { 0x3121, GLYPH_oubopomofo, },
    { 0x203e, GLYPH_overline, },
    { 0xfe4a, GLYPH_overlinecenterline, },
    { 0x0305, GLYPH_overlinecmb, },
    { 0xfe49, GLYPH_overlinedashed, },
    { 0xfe4c, GLYPH_overlinedblwavy, },
    { 0xfe4b, GLYPH_overlinewavy, },
    { 0x00af, GLYPH_overscore, },
    { 0x09cb, GLYPH_ovowelsignbengali, },
    { 0x094b, GLYPH_ovowelsigndeva, },
    { 0x0acb, GLYPH_ovowelsigngujarati, },
    { 0x0070, GLYPH_p, },
    { 0x3380, GLYPH_paampssquare, },
    { 0x332b, GLYPH_paasentosquare, },
    { 0x09aa, GLYPH_pabengali, },
    { 0x1e55, GLYPH_pacute, },
    { 0x092a, GLYPH_padeva, },
    { 0x21df, GLYPH_pagedown, },
    { 0x21de, GLYPH_pageup, },
    { 0x0aaa, GLYPH_pagujarati, },
    { 0x0a2a, GLYPH_pagurmukhi, },
    { 0x3071, GLYPH_pahiragana, },
    { 0x0e2f, GLYPH_paiyannoithai, },
    { 0x30d1, GLYPH_pakatakana, },
    { 0x0484, GLYPH_palatalizationcyrilliccmb, },
    { 0x04c0, GLYPH_palochkacyrillic, },
    { 0x317f, GLYPH_pansioskorean, },
    { 0x00b6, GLYPH_paragraph, },
    { 0x2225, GLYPH_parallel, },
    { 0x0028, GLYPH_parenleft, },
    { 0xfd3e, GLYPH_parenleftaltonearabic, },
    { 0xf8ed, GLYPH_parenleftbt, },
    { 0xf8ec, GLYPH_parenleftex, },
    { 0x208d, GLYPH_parenleftinferior, },
    { 0xff08, GLYPH_parenleftmonospace, },
    { 0xfe59, GLYPH_parenleftsmall, },
    { 0x207d, GLYPH_parenleftsuperior, },
    { 0xf8eb, GLYPH_parenlefttp, },
    { 0xfe35, GLYPH_parenleftvertical, },
    { 0x0029, GLYPH_parenright, },
    { 0xfd3f, GLYPH_parenrightaltonearabic, },
    { 0xf8f8, GLYPH_parenrightbt, },
    { 0xf8f7, GLYPH_parenrightex, },
    { 0x208e, GLYPH_parenrightinferior, },
    { 0xff09, GLYPH_parenrightmonospace, },
    { 0xfe5a, GLYPH_parenrightsmall, },
    { 0x207e, GLYPH_parenrightsuperior, },
    { 0xf8f6, GLYPH_parenrighttp, },
    { 0xfe36, GLYPH_parenrightvertical, },
    { 0x2202, GLYPH_partialdiff, },
    { 0x05c0, GLYPH_paseqhebrew, },
    { 0x0599, GLYPH_pashtahebrew, },
    { 0x33a9, GLYPH_pasquare, },
    { 0x05b7, GLYPH_patah, },
    { 0x05b7, GLYPH_patah11, },
    { 0x05b7, GLYPH_patah1d, },
    { 0x05b7, GLYPH_patah2a, },
    { 0x05b7, GLYPH_patahhebrew, },
    { 0x05b7, GLYPH_patahnarrowhebrew, },
    { 0x05b7, GLYPH_patahquarterhebrew, },
    { 0x05b7, GLYPH_patahwidehebrew, },
    { 0x05a1, GLYPH_pazerhebrew, },
    { 0x3106, GLYPH_pbopomofo, },
    { 0x24df, GLYPH_pcircle, },
    { 0x1e57, GLYPH_pdotaccent, },
    { 0x05e4, GLYPH_pe, },
    { 0x043f, GLYPH_pecyrillic, },
    { 0xfb44, GLYPH_pedagesh, },
    { 0xfb44, GLYPH_pedageshhebrew, },
    { 0x333b, GLYPH_peezisquare, },
    { 0xfb43, GLYPH_pefinaldageshhebrew, },
    { 0x067e, GLYPH_peharabic, },
    { 0x057a, GLYPH_peharmenian, },
    { 0x05e4, GLYPH_pehebrew, },
    { 0xfb57, GLYPH_pehfinalarabic, },
    { 0xfb58, GLYPH_pehinitialarabic, },
    { 0x307a, GLYPH_pehiragana, },
    { 0xfb59, GLYPH_pehmedialarabic, },
    { 0x30da, GLYPH_pekatakana, },
    { 0x04a7, GLYPH_pemiddlehookcyrillic, },
    { 0xfb4e, GLYPH_perafehebrew, },
    { 0x0025, GLYPH_percent, },
    { 0x066a, GLYPH_percentarabic, },
    { 0xff05, GLYPH_percentmonospace, },
    { 0xfe6a, GLYPH_percentsmall, },
    { 0x002e, GLYPH_period, },
    { 0x0589, GLYPH_periodarmenian, },
    { 0x00b7, GLYPH_periodcentered, },
    { 0xff61, GLYPH_periodhalfwidth, },
    { 0xf6e7, GLYPH_periodinferior, },
    { 0xff0e, GLYPH_periodmonospace, },
    { 0xfe52, GLYPH_periodsmall, },
    { 0xf6e8, GLYPH_periodsuperior, },
    { 0x0342, GLYPH_perispomenigreekcmb, },
    { 0x22a5, GLYPH_perpendicular, },
    { 0x2030, GLYPH_perthousand, },
    { 0x20a7, GLYPH_peseta, },
    { 0x338a, GLYPH_pfsquare, },
    { 0x09ab, GLYPH_phabengali, },
    { 0x092b, GLYPH_phadeva, },
    { 0x0aab, GLYPH_phagujarati, },
    { 0x0a2b, GLYPH_phagurmukhi, },
    { 0x03c6, GLYPH_phi, },
    { 0x03d5, GLYPH_phi1, },
    { 0x327a, GLYPH_phieuphacirclekorean, },
    { 0x321a, GLYPH_phieuphaparenkorean, },
    { 0x326c, GLYPH_phieuphcirclekorean, },
    { 0x314d, GLYPH_phieuphkorean, },
    { 0x320c, GLYPH_phieuphparenkorean, },
    { 0x0278, GLYPH_philatin, },
    { 0x0e3a, GLYPH_phinthuthai, },
    { 0x03d5, GLYPH_phisymbolgreek, },
    { 0x01a5, GLYPH_phook, },
    { 0x0e1e, GLYPH_phophanthai, },
    { 0x0e1c, GLYPH_phophungthai, },
    { 0x0e20, GLYPH_phosamphaothai, },
    { 0x03c0, GLYPH_pi, },
    { 0x3273, GLYPH_pieupacirclekorean, },
    { 0x3213, GLYPH_pieupaparenkorean, },
    { 0x3176, GLYPH_pieupcieuckorean, },
    { 0x3265, GLYPH_pieupcirclekorean, },
    { 0x3172, GLYPH_pieupkiyeokkorean, },
    { 0x3142, GLYPH_pieupkorean, },
    { 0x3205, GLYPH_pieupparenkorean, },
    { 0x3174, GLYPH_pieupsioskiyeokkorean, },
    { 0x3144, GLYPH_pieupsioskorean, },
    { 0x3175, GLYPH_pieupsiostikeutkorean, },
    { 0x3177, GLYPH_pieupthieuthkorean, },
    { 0x3173, GLYPH_pieuptikeutkorean, },
    { 0x3074, GLYPH_pihiragana, },
    { 0x30d4, GLYPH_pikatakana, },
    { 0x03d6, GLYPH_pisymbolgreek, },
    { 0x0583, GLYPH_piwrarmenian, },
    { 0x002b, GLYPH_plus, },
    { 0x031f, GLYPH_plusbelowcmb, },
    { 0x2295, GLYPH_pluscircle, },
    { 0x00b1, GLYPH_plusminus, },
    { 0x02d6, GLYPH_plusmod, },
    { 0xff0b, GLYPH_plusmonospace, },
    { 0xfe62, GLYPH_plussmall, },
    { 0x207a, GLYPH_plussuperior, },
    { 0xff50, GLYPH_pmonospace, },
    { 0x33d8, GLYPH_pmsquare, },
    { 0x307d, GLYPH_pohiragana, },
    { 0x261f, GLYPH_pointingindexdownwhite, },
    { 0x261c, GLYPH_pointingindexleftwhite, },
    { 0x261e, GLYPH_pointingindexrightwhite, },
    { 0x261d, GLYPH_pointingindexupwhite, },
    { 0x30dd, GLYPH_pokatakana, },
    { 0x0e1b, GLYPH_poplathai, },
    { 0x3012, GLYPH_postalmark, },
    { 0x3020, GLYPH_postalmarkface, },
    { 0x24ab, GLYPH_pparen, },
    { 0x227a, GLYPH_precedes, },
    { 0x211e, GLYPH_prescription, },
    { 0x02b9, GLYPH_primemod, },
    { 0x2035, GLYPH_primereversed, },
    { 0x220f, GLYPH_product, },
    { 0x2305, GLYPH_projective, },
    { 0x30fc, GLYPH_prolongedkana, },
    { 0x2318, GLYPH_propellor, },
    { 0x2282, GLYPH_propersubset, },
    { 0x2283, GLYPH_propersuperset, },
    { 0x2237, GLYPH_proportion, },
    { 0x221d, GLYPH_proportional, },
    { 0x03c8, GLYPH_psi, },
    { 0x0471, GLYPH_psicyrillic, },
    { 0x0486, GLYPH_psilipneumatacyrilliccmb, },
    { 0x33b0, GLYPH_pssquare, },
    { 0x3077, GLYPH_puhiragana, },
    { 0x30d7, GLYPH_pukatakana, },
    { 0x33b4, GLYPH_pvsquare, },
    { 0x33ba, GLYPH_pwsquare, },
    { 0x0071, GLYPH_q, },
    { 0x0958, GLYPH_qadeva, },
    { 0x05a8, GLYPH_qadmahebrew, },
    { 0x0642, GLYPH_qafarabic, },
    { 0xfed6, GLYPH_qaffinalarabic, },
    { 0xfed7, GLYPH_qafinitialarabic, },
    { 0xfed8, GLYPH_qafmedialarabic, },
    { 0x05b8, GLYPH_qamats, },
    { 0x05b8, GLYPH_qamats10, },
    { 0x05b8, GLYPH_qamats1a, },
    { 0x05b8, GLYPH_qamats1c, },
    { 0x05b8, GLYPH_qamats27, },
    { 0x05b8, GLYPH_qamats29, },
    { 0x05b8, GLYPH_qamats33, },
    { 0x05b8, GLYPH_qamatsde, },
    { 0x05b8, GLYPH_qamatshebrew, },
    { 0x05b8, GLYPH_qamatsnarrowhebrew, },
    { 0x05b8, GLYPH_qamatsqatanhebrew, },
    { 0x05b8, GLYPH_qamatsqatannarrowhebrew, },
    { 0x05b8, GLYPH_qamatsqatanquarterhebrew, },
    { 0x05b8, GLYPH_qamatsqatanwidehebrew, },
    { 0x05b8, GLYPH_qamatsquarterhebrew, },
    { 0x05b8, GLYPH_qamatswidehebrew, },
    { 0x059f, GLYPH_qarneyparahebrew, },
    { 0x3111, GLYPH_qbopomofo, },
    { 0x24e0, GLYPH_qcircle, },
    { 0x02a0, GLYPH_qhook, },
    { 0xff51, GLYPH_qmonospace, },
    { 0x05e7, GLYPH_qof, },
    { 0xfb47, GLYPH_qofdagesh, },
    { 0xfb47, GLYPH_qofdageshhebrew, },
    { 0x05e7, GLYPH_qofhebrew, },
    { 0x24ac, GLYPH_qparen, },
    { 0x2669, GLYPH_quarternote, },
    { 0x05bb, GLYPH_qubuts, },
    { 0x05bb, GLYPH_qubuts18, },
    { 0x05bb, GLYPH_qubuts25, },
    { 0x05bb, GLYPH_qubuts31, },
    { 0x05bb, GLYPH_qubutshebrew, },
    { 0x05bb, GLYPH_qubutsnarrowhebrew, },
    { 0x05bb, GLYPH_qubutsquarterhebrew, },
    { 0x05bb, GLYPH_qubutswidehebrew, },
    { 0x003f, GLYPH_question, },
    { 0x061f, GLYPH_questionarabic, },
    { 0x055e, GLYPH_questionarmenian, },
    { 0x00bf, GLYPH_questiondown, },
    { 0xf7bf, GLYPH_questiondownsmall, },
    { 0x037e, GLYPH_questiongreek, },
    { 0xff1f, GLYPH_questionmonospace, },
    { 0xf73f, GLYPH_questionsmall, },
    { 0x0022, GLYPH_quotedbl, },
    { 0x201e, GLYPH_quotedblbase, },
    { 0x201c, GLYPH_quotedblleft, },
    { 0xff02, GLYPH_quotedblmonospace, },
    { 0x301e, GLYPH_quotedblprime, },
    { 0x301d, GLYPH_quotedblprimereversed, },
    { 0x201d, GLYPH_quotedblright, },
    { 0x2018, GLYPH_quoteleft, },
    { 0x201b, GLYPH_quoteleftreversed, },
    { 0x201b, GLYPH_quotereversed, },
    { 0x2019, GLYPH_quoteright, },
    { 0x0149, GLYPH_quoterightn, },
    { 0x201a, GLYPH_quotesinglbase, },
    { 0x0027, GLYPH_quotesingle, },
    { 0xff07, GLYPH_quotesinglemonospace, },
    { 0x0072, GLYPH_r, },
    { 0x057c, GLYPH_raarmenian, },
    { 0x09b0, GLYPH_rabengali, },
    { 0x0155, GLYPH_racute, },
    { 0x0930, GLYPH_radeva, },
    { 0x221a, GLYPH_radical, },
    { 0xf8e5, GLYPH_radicalex, },
    { 0x33ae, GLYPH_radoverssquare, },
    { 0x33af, GLYPH_radoverssquaredsquare, },
    { 0x33ad, GLYPH_radsquare, },
    { 0x05bf, GLYPH_rafe, },
    { 0x05bf, GLYPH_rafehebrew, },
    { 0x0ab0, GLYPH_ragujarati, },
    { 0x0a30, GLYPH_ragurmukhi, },
    { 0x3089, GLYPH_rahiragana, },
    { 0x30e9, GLYPH_rakatakana, },
    { 0xff97, GLYPH_rakatakanahalfwidth, },
    { 0x09f1, GLYPH_ralowerdiagonalbengali, },
    { 0x09f0, GLYPH_ramiddlediagonalbengali, },
    { 0x0264, GLYPH_ramshorn, },
    { 0x2236, GLYPH_ratio, },
    { 0x3116, GLYPH_rbopomofo, },
    { 0x0159, GLYPH_rcaron, },
    { 0x0157, GLYPH_rcedilla, },
    { 0x24e1, GLYPH_rcircle, },
    { 0x0157, GLYPH_rcommaaccent, },
    { 0x0211, GLYPH_rdblgrave, },
    { 0x1e59, GLYPH_rdotaccent, },
    { 0x1e5b, GLYPH_rdotbelow, },
    { 0x1e5d, GLYPH_rdotbelowmacron, },
    { 0x203b, GLYPH_referencemark, },
    { 0x2286, GLYPH_reflexsubset, },
    { 0x2287, GLYPH_reflexsuperset, },
    { 0x00ae, GLYPH_registered, },
    { 0xf8e8, GLYPH_registersans, },
    { 0xf6da, GLYPH_registerserif, },
    { 0x0631, GLYPH_reharabic, },
    { 0x0580, GLYPH_reharmenian, },
    { 0xfeae, GLYPH_rehfinalarabic, },
    { 0x308c, GLYPH_rehiragana, },
    { 0x30ec, GLYPH_rekatakana, },
    { 0xff9a, GLYPH_rekatakanahalfwidth, },
    { 0x05e8, GLYPH_resh, },
    { 0xfb48, GLYPH_reshdageshhebrew, },
    { 0x05e8, GLYPH_reshhebrew, },
    { 0x223d, GLYPH_reversedtilde, },
    { 0x0597, GLYPH_reviahebrew, },
    { 0x0597, GLYPH_reviamugrashhebrew, },
    { 0x2310, GLYPH_revlogicalnot, },
    { 0x027e, GLYPH_rfishhook, },
    { 0x027f, GLYPH_rfishhookreversed, },
    { 0x09dd, GLYPH_rhabengali, },
    { 0x095d, GLYPH_rhadeva, },
    { 0x03c1, GLYPH_rho, },
    { 0x027d, GLYPH_rhook, },
    { 0x027b, GLYPH_rhookturned, },
    { 0x02b5, GLYPH_rhookturnedsuperior, },
    { 0x03f1, GLYPH_rhosymbolgreek, },
    { 0x02de, GLYPH_rhotichookmod, },
    { 0x3271, GLYPH_rieulacirclekorean, },
    { 0x3211, GLYPH_rieulaparenkorean, },
    { 0x3263, GLYPH_rieulcirclekorean, },
    { 0x3140, GLYPH_rieulhieuhkorean, },
    { 0x313a, GLYPH_rieulkiyeokkorean, },
    { 0x3169, GLYPH_rieulkiyeoksioskorean, },
    { 0x3139, GLYPH_rieulkorean, },
    { 0x313b, GLYPH_rieulmieumkorean, },
    { 0x316c, GLYPH_rieulpansioskorean, },
    { 0x3203, GLYPH_rieulparenkorean, },
    { 0x313f, GLYPH_rieulphieuphkorean, },
    { 0x313c, GLYPH_rieulpieupkorean, },
    { 0x316b, GLYPH_rieulpieupsioskorean, },
    { 0x313d, GLYPH_rieulsioskorean, },
    { 0x313e, GLYPH_rieulthieuthkorean, },
    { 0x316a, GLYPH_rieultikeutkorean, },
    { 0x316d, GLYPH_rieulyeorinhieuhkorean, },
    { 0x221f, GLYPH_rightangle, },
    { 0x0319, GLYPH_righttackbelowcmb, },
    { 0x22bf, GLYPH_righttriangle, },
    { 0x308a, GLYPH_rihiragana, },
    { 0x30ea, GLYPH_rikatakana, },
    { 0xff98, GLYPH_rikatakanahalfwidth, },
    { 0x02da, GLYPH_ring, },
    { 0x0325, GLYPH_ringbelowcmb, },
    { 0x030a, GLYPH_ringcmb, },
    { 0x02bf, GLYPH_ringhalfleft, },
    { 0x0559, GLYPH_ringhalfleftarmenian, },
    { 0x031c, GLYPH_ringhalfleftbelowcmb, },
    { 0x02d3, GLYPH_ringhalfleftcentered, },
    { 0x02be, GLYPH_ringhalfright, },
    { 0x0339, GLYPH_ringhalfrightbelowcmb, },
    { 0x02d2, GLYPH_ringhalfrightcentered, },
    { 0x0213, GLYPH_rinvertedbreve, },
    { 0x3351, GLYPH_rittorusquare, },
    { 0x1e5f, GLYPH_rlinebelow, },
    { 0x027c, GLYPH_rlongleg, },
    { 0x027a, GLYPH_rlonglegturned, },
    { 0xff52, GLYPH_rmonospace, },
    { 0x308d, GLYPH_rohiragana, },
    { 0x30ed, GLYPH_rokatakana, },
    { 0xff9b, GLYPH_rokatakanahalfwidth, },
    { 0x0e23, GLYPH_roruathai, },
    { 0x24ad, GLYPH_rparen, },
    { 0x09dc, GLYPH_rrabengali, },
    { 0x0931, GLYPH_rradeva, },
    { 0x0a5c, GLYPH_rragurmukhi, },
    { 0x0691, GLYPH_rreharabic, },
    { 0xfb8d, GLYPH_rrehfinalarabic, },
    { 0x09e0, GLYPH_rrvocalicbengali, },
    { 0x0960, GLYPH_rrvocalicdeva, },
    { 0x0ae0, GLYPH_rrvocalicgujarati, },
    { 0x09c4, GLYPH_rrvocalicvowelsignbengali, },
    { 0x0944, GLYPH_rrvocalicvowelsigndeva, },
    { 0x0ac4, GLYPH_rrvocalicvowelsigngujarati, },
    { 0xf6f1, GLYPH_rsuperior, },
    { 0x2590, GLYPH_rtblock, },
    { 0x0279, GLYPH_rturned, },
    { 0x02b4, GLYPH_rturnedsuperior, },
    { 0x308b, GLYPH_ruhiragana, },
    { 0x30eb, GLYPH_rukatakana, },
    { 0xff99, GLYPH_rukatakanahalfwidth, },
    { 0x09f2, GLYPH_rupeemarkbengali, },
    { 0x09f3, GLYPH_rupeesignbengali, },
    { 0xf6dd, GLYPH_rupiah, },
    { 0x0e24, GLYPH_ruthai, },
    { 0x098b, GLYPH_rvocalicbengali, },
    { 0x090b, GLYPH_rvocalicdeva, },
    { 0x0a8b, GLYPH_rvocalicgujarati, },
    { 0x09c3, GLYPH_rvocalicvowelsignbengali, },
    { 0x0943, GLYPH_rvocalicvowelsigndeva, },
    { 0x0ac3, GLYPH_rvocalicvowelsigngujarati, },
    { 0x0073, GLYPH_s, },
    { 0x09b8, GLYPH_sabengali, },
    { 0x015b, GLYPH_sacute, },
    { 0x1e65, GLYPH_sacutedotaccent, },
    { 0x0635, GLYPH_sadarabic, },
    { 0x0938, GLYPH_sadeva, },
    { 0xfeba, GLYPH_sadfinalarabic, },
    { 0xfebb, GLYPH_sadinitialarabic, },
    { 0xfebc, GLYPH_sadmedialarabic, },
    { 0x0ab8, GLYPH_sagujarati, },
    { 0x0a38, GLYPH_sagurmukhi, },
    { 0x3055, GLYPH_sahiragana, },
    { 0x30b5, GLYPH_sakatakana, },
    { 0xff7b, GLYPH_sakatakanahalfwidth, },
    { 0xfdfa, GLYPH_sallallahoualayhewasallamarabic, },
    { 0x05e1, GLYPH_samekh, },
    { 0xfb41, GLYPH_samekhdagesh, },
    { 0xfb41, GLYPH_samekhdageshhebrew, },
    { 0x05e1, GLYPH_samekhhebrew, },
    { 0x0e32, GLYPH_saraaathai, },
    { 0x0e41, GLYPH_saraaethai, },
    { 0x0e44, GLYPH_saraaimaimalaithai, },
    { 0x0e43, GLYPH_saraaimaimuanthai, },
    { 0x0e33, GLYPH_saraamthai, },
    { 0x0e30, GLYPH_saraathai, },
    { 0x0e40, GLYPH_saraethai, },
    { 0xf886, GLYPH_saraiileftthai, },
    { 0x0e35, GLYPH_saraiithai, },
    { 0xf885, GLYPH_saraileftthai, },
    { 0x0e34, GLYPH_saraithai, },
    { 0x0e42, GLYPH_saraothai, },
    { 0xf888, GLYPH_saraueeleftthai, },
    { 0x0e37, GLYPH_saraueethai, },
    { 0xf887, GLYPH_saraueleftthai, },
    { 0x0e36, GLYPH_sarauethai, },
    { 0x0e38, GLYPH_sarauthai, },
    { 0x0e39, GLYPH_sarauuthai, },
    { 0x3119, GLYPH_sbopomofo, },
    { 0x0161, GLYPH_scaron, },
    { 0x1e67, GLYPH_scarondotaccent, },
    { 0x015f, GLYPH_scedilla, },
    { 0x015f, GLYPH_scedilla1, },
    { 0x0259, GLYPH_schwa, },
    { 0x04d9, GLYPH_schwacyrillic, },
    { 0x04db, GLYPH_schwadieresiscyrillic, },
    { 0x025a, GLYPH_schwahook, },
    { 0x24e2, GLYPH_scircle, },
    { 0x015d, GLYPH_scircumflex, },
    { 0x0219, GLYPH_scommaaccent, },
    { 0x1e61, GLYPH_sdotaccent, },
    { 0x1e63, GLYPH_sdotbelow, },
    { 0x1e69, GLYPH_sdotbelowdotaccent, },
    { 0x033c, GLYPH_seagullbelowcmb, },
    { 0x2033, GLYPH_second, },
    { 0x02ca, GLYPH_secondtonechinese, },
    { 0x00a7, GLYPH_section, },
    { 0x0633, GLYPH_seenarabic, },
    { 0xfeb2, GLYPH_seenfinalarabic, },
    { 0xfeb3, GLYPH_seeninitialarabic, },
    { 0xfeb4, GLYPH_seenmedialarabic, },
    { 0x05b6, GLYPH_segol, },
    { 0x05b6, GLYPH_segol13, },
    { 0x05b6, GLYPH_segol1f, },
    { 0x05b6, GLYPH_segol2c, },
    { 0x05b6, GLYPH_segolhebrew, },
    { 0x05b6, GLYPH_segolnarrowhebrew, },
    { 0x05b6, GLYPH_segolquarterhebrew, },
    { 0x0592, GLYPH_segoltahebrew, },
    { 0x05b6, GLYPH_segolwidehebrew, },
    { 0x057d, GLYPH_seharmenian, },
    { 0x305b, GLYPH_sehiragana, },
    { 0x30bb, GLYPH_sekatakana, },
    { 0xff7e, GLYPH_sekatakanahalfwidth, },
    { 0x003b, GLYPH_semicolon, },
    { 0x061b, GLYPH_semicolonarabic, },
    { 0xff1b, GLYPH_semicolonmonospace, },
    { 0xfe54, GLYPH_semicolonsmall, },
    { 0x309c, GLYPH_semivoicedmarkkana, },
    { 0xff9f, GLYPH_semivoicedmarkkanahalfwidth, },
    { 0x3322, GLYPH_sentisquare, },
    { 0x3323, GLYPH_sentosquare, },
    { 0x0037, GLYPH_seven, },
    { 0x0667, GLYPH_sevenarabic, },
    { 0x09ed, GLYPH_sevenbengali, },
    { 0x2466, GLYPH_sevencircle, },
    { 0x2790, GLYPH_sevencircleinversesansserif, },
    { 0x096d, GLYPH_sevendeva, },
    { 0x215e, GLYPH_seveneighths, },
    { 0x0aed, GLYPH_sevengujarati, },
    { 0x0a6d, GLYPH_sevengurmukhi, },
    { 0x0667, GLYPH_sevenhackarabic, },
    { 0x3027, GLYPH_sevenhangzhou, },
    { 0x3226, GLYPH_sevenideographicparen, },
    { 0x2087, GLYPH_seveninferior, },
    { 0xff17, GLYPH_sevenmonospace, },
    { 0xf737, GLYPH_sevenoldstyle, },
    { 0x247a, GLYPH_sevenparen, },
    { 0x248e, GLYPH_sevenperiod, },
    { 0x06f7, GLYPH_sevenpersian, },
    { 0x2176, GLYPH_sevenroman, },
    { 0x2077, GLYPH_sevensuperior, },
    { 0x2470, GLYPH_seventeencircle, },
    { 0x2484, GLYPH_seventeenparen, },
    { 0x2498, GLYPH_seventeenperiod, },
    { 0x0e57, GLYPH_seventhai, },
    { 0x00ad, GLYPH_sfthyphen, },
    { 0x0577, GLYPH_shaarmenian, },
    { 0x09b6, GLYPH_shabengali, },
    { 0x0448, GLYPH_shacyrillic, },
    { 0x0651, GLYPH_shaddaarabic, },
    { 0xfc61, GLYPH_shaddadammaarabic, },
    { 0xfc5e, GLYPH_shaddadammatanarabic, },
    { 0xfc60, GLYPH_shaddafathaarabic, },
    { 0xfc62, GLYPH_shaddakasraarabic, },
    { 0xfc5f, GLYPH_shaddakasratanarabic, },
    { 0x2592, GLYPH_shade, },
    { 0x2593, GLYPH_shadedark, },
    { 0x2591, GLYPH_shadelight, },
    { 0x2592, GLYPH_shademedium, },
    { 0x0936, GLYPH_shadeva, },
    { 0x0ab6, GLYPH_shagujarati, },
    { 0x0a36, GLYPH_shagurmukhi, },
    { 0x0593, GLYPH_shalshelethebrew, },
    { 0x3115, GLYPH_shbopomofo, },
    { 0x0449, GLYPH_shchacyrillic, },
    { 0x0634, GLYPH_sheenarabic, },
    { 0xfeb6, GLYPH_sheenfinalarabic, },
    { 0xfeb7, GLYPH_sheeninitialarabic, },
    { 0xfeb8, GLYPH_sheenmedialarabic, },
    { 0x03e3, GLYPH_sheicoptic, },
    { 0x20aa, GLYPH_sheqel, },
    { 0x20aa, GLYPH_sheqelhebrew, },
    { 0x05b0, GLYPH_sheva, },
    { 0x05b0, GLYPH_sheva115, },
    { 0x05b0, GLYPH_sheva15, },
    { 0x05b0, GLYPH_sheva22, },
    { 0x05b0, GLYPH_sheva2e, },
    { 0x05b0, GLYPH_shevahebrew, },
    { 0x05b0, GLYPH_shevanarrowhebrew, },
    { 0x05b0, GLYPH_shevaquarterhebrew, },
    { 0x05b0, GLYPH_shevawidehebrew, },
    { 0x04bb, GLYPH_shhacyrillic, },
    { 0x03ed, GLYPH_shimacoptic, },
    { 0x05e9, GLYPH_shin, },
    { 0xfb49, GLYPH_shindagesh, },
    { 0xfb49, GLYPH_shindageshhebrew, },
    { 0xfb2c, GLYPH_shindageshshindot, },
    { 0xfb2c, GLYPH_shindageshshindothebrew, },
    { 0xfb2d, GLYPH_shindageshsindot, },
    { 0xfb2d, GLYPH_shindageshsindothebrew, },
    { 0x05c1, GLYPH_shindothebrew, },
    { 0x05e9, GLYPH_shinhebrew, },
    { 0xfb2a, GLYPH_shinshindot, },
    { 0xfb2a, GLYPH_shinshindothebrew, },
    { 0xfb2b, GLYPH_shinsindot, },
    { 0xfb2b, GLYPH_shinsindothebrew, },
    { 0x0282, GLYPH_shook, },
    { 0x03c3, GLYPH_sigma, },
    { 0x03c2, GLYPH_sigma1, },
    { 0x03c2, GLYPH_sigmafinal, },
    { 0x03f2, GLYPH_sigmalunatesymbolgreek, },
    { 0x3057, GLYPH_sihiragana, },
    { 0x30b7, GLYPH_sikatakana, },
    { 0xff7c, GLYPH_sikatakanahalfwidth, },
    { 0x05bd, GLYPH_siluqhebrew, },
    { 0x05bd, GLYPH_siluqlefthebrew, },
    { 0x223c, GLYPH_similar, },
    { 0x05c2, GLYPH_sindothebrew, },
    { 0x3274, GLYPH_siosacirclekorean, },
    { 0x3214, GLYPH_siosaparenkorean, },
    { 0x317e, GLYPH_sioscieuckorean, },
    { 0x3266, GLYPH_sioscirclekorean, },
    { 0x317a, GLYPH_sioskiyeokkorean, },
    { 0x3145, GLYPH_sioskorean, },
    { 0x317b, GLYPH_siosnieunkorean, },
    { 0x3206, GLYPH_siosparenkorean, },
    { 0x317d, GLYPH_siospieupkorean, },
    { 0x317c, GLYPH_siostikeutkorean, },
    { 0x0036, GLYPH_six, },
    { 0x0666, GLYPH_sixarabic, },
    { 0x09ec, GLYPH_sixbengali, },
    { 0x2465, GLYPH_sixcircle, },
    { 0x278f, GLYPH_sixcircleinversesansserif, },
    { 0x096c, GLYPH_sixdeva, },
    { 0x0aec, GLYPH_sixgujarati, },
    { 0x0a6c, GLYPH_sixgurmukhi, },
    { 0x0666, GLYPH_sixhackarabic, },
    { 0x3026, GLYPH_sixhangzhou, },
    { 0x3225, GLYPH_sixideographicparen, },
    { 0x2086, GLYPH_sixinferior, },
    { 0xff16, GLYPH_sixmonospace, },
    { 0xf736, GLYPH_sixoldstyle, },
    { 0x2479, GLYPH_sixparen, },
    { 0x248d, GLYPH_sixperiod, },
    { 0x06f6, GLYPH_sixpersian, },
    { 0x2175, GLYPH_sixroman, },
    { 0x2076, GLYPH_sixsuperior, },
    { 0x246f, GLYPH_sixteencircle, },
    { 0x09f9, GLYPH_sixteencurrencydenominatorbengali, },
    { 0x2483, GLYPH_sixteenparen, },
    { 0x2497, GLYPH_sixteenperiod, },
    { 0x0e56, GLYPH_sixthai, },
    { 0x002f, GLYPH_slash, },
    { 0xff0f, GLYPH_slashmonospace, },
    { 0x017f, GLYPH_slong, },
    { 0x1e9b, GLYPH_slongdotaccent, },
    { 0x263a, GLYPH_smileface, },
    { 0xff53, GLYPH_smonospace, },
    { 0x05c3, GLYPH_sofpasuqhebrew, },
    { 0x00ad, GLYPH_softhyphen, },
    { 0x044c, GLYPH_softsigncyrillic, },
    { 0x305d, GLYPH_sohiragana, },
    { 0x30bd, GLYPH_sokatakana, },
    { 0xff7f, GLYPH_sokatakanahalfwidth, },
    { 0x0338, GLYPH_soliduslongoverlaycmb, },
    { 0x0337, GLYPH_solidusshortoverlaycmb, },
    { 0x0e29, GLYPH_sorusithai, },
    { 0x0e28, GLYPH_sosalathai, },
    { 0x0e0b, GLYPH_sosothai, },
    { 0x0e2a, GLYPH_sosuathai, },
    { 0x0020, GLYPH_space, },
    { 0x0020, GLYPH_spacehackarabic, },
    { 0x2660, GLYPH_spade, },
    { 0x2660, GLYPH_spadesuitblack, },
    { 0x2664, GLYPH_spadesuitwhite, },
    { 0x24ae, GLYPH_sparen, },
    { 0x033b, GLYPH_squarebelowcmb, },
    { 0x33c4, GLYPH_squarecc, },
    { 0x339d, GLYPH_squarecm, },
    { 0x25a9, GLYPH_squarediagonalcrosshatchfill, },
    { 0x25a4, GLYPH_squarehorizontalfill, },
    { 0x338f, GLYPH_squarekg, },
    { 0x339e, GLYPH_squarekm, },
    { 0x33ce, GLYPH_squarekmcapital, },
    { 0x33d1, GLYPH_squareln, },
    { 0x33d2, GLYPH_squarelog, },
    { 0x338e, GLYPH_squaremg, },
    { 0x33d5, GLYPH_squaremil, },
    { 0x339c, GLYPH_squaremm, },
    { 0x33a1, GLYPH_squaremsquared, },
    { 0x25a6, GLYPH_squareorthogonalcrosshatchfill, },
    { 0x25a7, GLYPH_squareupperlefttolowerrightfill, },
    { 0x25a8, GLYPH_squareupperrighttolowerleftfill, },
    { 0x25a5, GLYPH_squareverticalfill, },
    { 0x25a3, GLYPH_squarewhitewithsmallblack, },
    { 0x33db, GLYPH_srsquare, },
    { 0x09b7, GLYPH_ssabengali, },
    { 0x0937, GLYPH_ssadeva, },
    { 0x0ab7, GLYPH_ssagujarati, },
    { 0x3149, GLYPH_ssangcieuckorean, },
    { 0x3185, GLYPH_ssanghieuhkorean, },
    { 0x3180, GLYPH_ssangieungkorean, },
    { 0x3132, GLYPH_ssangkiyeokkorean, },
    { 0x3165, GLYPH_ssangnieunkorean, },
    { 0x3143, GLYPH_ssangpieupkorean, },
    { 0x3146, GLYPH_ssangsioskorean, },
    { 0x3138, GLYPH_ssangtikeutkorean, },
    { 0x00df, GLYPH_ssharp, },
    { 0xf6f2, GLYPH_ssuperior, },
    { 0x00a3, GLYPH_sterling, },
    { 0xffe1, GLYPH_sterlingmonospace, },
    { 0x0336, GLYPH_strokelongoverlaycmb, },
    { 0x0335, GLYPH_strokeshortoverlaycmb, },
    { 0x2282, GLYPH_subset, },
    { 0x228a, GLYPH_subsetnotequal, },
    { 0x2286, GLYPH_subsetorequal, },
    { 0x227b, GLYPH_succeeds, },
    { 0x220b, GLYPH_suchthat, },
    { 0x3059, GLYPH_suhiragana, },
    { 0x30b9, GLYPH_sukatakana, },
    { 0xff7d, GLYPH_sukatakanahalfwidth, },
    { 0x0652, GLYPH_sukunarabic, },
    { 0x2211, GLYPH_summation, },
    { 0x263c, GLYPH_sun, },
    { 0x2283, GLYPH_superset, },
    { 0x228b, GLYPH_supersetnotequal, },
    { 0x2287, GLYPH_supersetorequal, },
    { 0x33dc, GLYPH_svsquare, },
    { 0x337c, GLYPH_syouwaerasquare, },
    { 0x0074, GLYPH_t, },
    { 0x09a4, GLYPH_tabengali, },
    { 0x22a4, GLYPH_tackdown, },
    { 0x22a3, GLYPH_tackleft, },
    { 0x0924, GLYPH_tadeva, },
    { 0x0aa4, GLYPH_tagujarati, },
    { 0x0a24, GLYPH_tagurmukhi, },
    { 0x0637, GLYPH_taharabic, },
    { 0xfec2, GLYPH_tahfinalarabic, },
    { 0xfec3, GLYPH_tahinitialarabic, },
    { 0x305f, GLYPH_tahiragana, },
    { 0xfec4, GLYPH_tahmedialarabic, },
    { 0x337d, GLYPH_taisyouerasquare, },
    { 0x30bf, GLYPH_takatakana, },
    { 0xff80, GLYPH_takatakanahalfwidth, },
    { 0x0640, GLYPH_tatweelarabic, },
    { 0x03c4, GLYPH_tau, },
    { 0x05ea, GLYPH_tav, },
    { 0xfb4a, GLYPH_tavdages, },
    { 0xfb4a, GLYPH_tavdagesh, },
    { 0xfb4a, GLYPH_tavdageshhebrew, },
    { 0x05ea, GLYPH_tavhebrew, },
    { 0x0167, GLYPH_tbar, },
    { 0x310a, GLYPH_tbopomofo, },
    { 0x0165, GLYPH_tcaron, },
    { 0x02a8, GLYPH_tccurl, },
    { 0x0163, GLYPH_tcedilla, },
    { 0x0163, GLYPH_tcedilla1, },
    { 0x0686, GLYPH_tcheharabic, },
    { 0xfb7b, GLYPH_tchehfinalarabic, },
    { 0xfb7c, GLYPH_tchehinitialarabic, },
    { 0xfb7d, GLYPH_tchehmedialarabic, },
    { 0x24e3, GLYPH_tcircle, },
    { 0x1e71, GLYPH_tcircumflexbelow, },
    { 0x0163, GLYPH_tcommaaccent, },
    { 0x1e97, GLYPH_tdieresis, },
    { 0x1e6b, GLYPH_tdotaccent, },
    { 0x1e6d, GLYPH_tdotbelow, },
    { 0x0442, GLYPH_tecyrillic, },
    { 0x04ad, GLYPH_tedescendercyrillic, },
    { 0x062a, GLYPH_teharabic, },
    { 0xfe96, GLYPH_tehfinalarabic, },
    { 0xfca2, GLYPH_tehhahinitialarabic, },
    { 0xfc0c, GLYPH_tehhahisolatedarabic, },
    { 0xfe97, GLYPH_tehinitialarabic, },
    { 0x3066, GLYPH_tehiragana, },
    { 0xfca1, GLYPH_tehjeeminitialarabic, },
    { 0xfc0b, GLYPH_tehjeemisolatedarabic, },
    { 0x0629, GLYPH_tehmarbutaarabic, },
    { 0xfe94, GLYPH_tehmarbutafinalarabic, },
    { 0xfe98, GLYPH_tehmedialarabic, },
    { 0xfca4, GLYPH_tehmeeminitialarabic, },
    { 0xfc0e, GLYPH_tehmeemisolatedarabic, },
    { 0xfc73, GLYPH_tehnoonfinalarabic, },
    { 0x30c6, GLYPH_tekatakana, },
    { 0xff83, GLYPH_tekatakanahalfwidth, },
    { 0x2121, GLYPH_telephone, },
    { 0x260e, GLYPH_telephoneblack, },
    { 0x05a0, GLYPH_telishagedolahebrew, },
    { 0x05a9, GLYPH_telishaqetanahebrew, },
    { 0x2469, GLYPH_tencircle, },
    { 0x3229, GLYPH_tenideographicparen, },
    { 0x247d, GLYPH_tenparen, },
    { 0x2491, GLYPH_tenperiod, },
    { 0x2179, GLYPH_tenroman, },
    { 0x02a7, GLYPH_tesh, },
    { 0x05d8, GLYPH_tet, },
    { 0xfb38, GLYPH_tetdagesh, },
    { 0xfb38, GLYPH_tetdageshhebrew, },
    { 0x05d8, GLYPH_tethebrew, },
    { 0x04b5, GLYPH_tetsecyrillic, },
    { 0x059b, GLYPH_tevirhebrew, },
    { 0x059b, GLYPH_tevirlefthebrew, },
    { 0x09a5, GLYPH_thabengali, },
    { 0x0925, GLYPH_thadeva, },
    { 0x0aa5, GLYPH_thagujarati, },
    { 0x0a25, GLYPH_thagurmukhi, },
    { 0x0630, GLYPH_thalarabic, },
    { 0xfeac, GLYPH_thalfinalarabic, },
    { 0xf898, GLYPH_thanthakhatlowleftthai, },
    { 0xf897, GLYPH_thanthakhatlowrightthai, },
    { 0x0e4c, GLYPH_thanthakhatthai, },
    { 0xf896, GLYPH_thanthakhatupperleftthai, },
    { 0x062b, GLYPH_theharabic, },
    { 0xfe9a, GLYPH_thehfinalarabic, },
    { 0xfe9b, GLYPH_thehinitialarabic, },
    { 0xfe9c, GLYPH_thehmedialarabic, },
    { 0x2203, GLYPH_thereexists, },
    { 0x2234, GLYPH_therefore, },
    { 0x03b8, GLYPH_theta, },
    { 0x03d1, GLYPH_theta1, },
    { 0x03d1, GLYPH_thetasymbolgreek, },
    { 0x3279, GLYPH_thieuthacirclekorean, },
    { 0x3219, GLYPH_thieuthaparenkorean, },
    { 0x326b, GLYPH_thieuthcirclekorean, },
    { 0x314c, GLYPH_thieuthkorean, },
    { 0x320b, GLYPH_thieuthparenkorean, },
    { 0x246c, GLYPH_thirteencircle, },
    { 0x2480, GLYPH_thirteenparen, },
    { 0x2494, GLYPH_thirteenperiod, },
    { 0x0e11, GLYPH_thonangmonthothai, },
    { 0x01ad, GLYPH_thook, },
    { 0x0e12, GLYPH_thophuthaothai, },
    { 0x00fe, GLYPH_thorn, },
    { 0x0e17, GLYPH_thothahanthai, },
    { 0x0e10, GLYPH_thothanthai, },
    { 0x0e18, GLYPH_thothongthai, },
    { 0x0e16, GLYPH_thothungthai, },
    { 0x0482, GLYPH_thousandcyrillic, },
    { 0x066c, GLYPH_thousandsseparatorarabic, },
    { 0x066c, GLYPH_thousandsseparatorpersian, },
    { 0x0033, GLYPH_three, },
    { 0x0663, GLYPH_threearabic, },
    { 0x09e9, GLYPH_threebengali, },
    { 0x2462, GLYPH_threecircle, },
    { 0x278c, GLYPH_threecircleinversesansserif, },
    { 0x0969, GLYPH_threedeva, },
    { 0x215c, GLYPH_threeeighths, },
    { 0x0ae9, GLYPH_threegujarati, },
    { 0x0a69, GLYPH_threegurmukhi, },
    { 0x0663, GLYPH_threehackarabic, },
    { 0x3023, GLYPH_threehangzhou, },
    { 0x3222, GLYPH_threeideographicparen, },
    { 0x2083, GLYPH_threeinferior, },
    { 0xff13, GLYPH_threemonospace, },
    { 0x09f6, GLYPH_threenumeratorbengali, },
    { 0xf733, GLYPH_threeoldstyle, },
    { 0x2476, GLYPH_threeparen, },
    { 0x248a, GLYPH_threeperiod, },
    { 0x06f3, GLYPH_threepersian, },
    { 0x00be, GLYPH_threequarters, },
    { 0xf6de, GLYPH_threequartersemdash, },
    { 0x2172, GLYPH_threeroman, },
    { 0x00b3, GLYPH_threesuperior, },
    { 0x0e53, GLYPH_threethai, },
    { 0x3394, GLYPH_thzsquare, },
    { 0x3061, GLYPH_tihiragana, },
    { 0x30c1, GLYPH_tikatakana, },
    { 0xff81, GLYPH_tikatakanahalfwidth, },
    { 0x3270, GLYPH_tikeutacirclekorean, },
    { 0x3210, GLYPH_tikeutaparenkorean, },
    { 0x3262, GLYPH_tikeutcirclekorean, },
    { 0x3137, GLYPH_tikeutkorean, },
    { 0x3202, GLYPH_tikeutparenkorean, },
    { 0x02dc, GLYPH_tilde, },
    { 0x0330, GLYPH_tildebelowcmb, },
    { 0x0303, GLYPH_tildecmb, },
    { 0x0303, GLYPH_tildecomb, },
    { 0x0360, GLYPH_tildedoublecmb, },
    { 0x223c, GLYPH_tildeoperator, },
    { 0x0334, GLYPH_tildeoverlaycmb, },
    { 0x033e, GLYPH_tildeverticalcmb, },
    { 0x2297, GLYPH_timescircle, },
    { 0x0596, GLYPH_tipehahebrew, },
    { 0x0596, GLYPH_tipehalefthebrew, },
    { 0x0a70, GLYPH_tippigurmukhi, },
    { 0x0483, GLYPH_titlocyrilliccmb, },
    { 0x057f, GLYPH_tiwnarmenian, },
    { 0x1e6f, GLYPH_tlinebelow, },
    { 0xff54, GLYPH_tmonospace, },
    { 0x0569, GLYPH_toarmenian, },
    { 0x3068, GLYPH_tohiragana, },
    { 0x30c8, GLYPH_tokatakana, },
    { 0xff84, GLYPH_tokatakanahalfwidth, },
    { 0x02e5, GLYPH_tonebarextrahighmod, },
    { 0x02e9, GLYPH_tonebarextralowmod, },
    { 0x02e6, GLYPH_tonebarhighmod, },
    { 0x02e8, GLYPH_tonebarlowmod, },
    { 0x02e7, GLYPH_tonebarmidmod, },
    { 0x01bd, GLYPH_tonefive, },
    { 0x0185, GLYPH_tonesix, },
    { 0x01a8, GLYPH_tonetwo, },
    { 0x0384, GLYPH_tonos, },
    { 0x3327, GLYPH_tonsquare, },
    { 0x0e0f, GLYPH_topatakthai, },
    { 0x3014, GLYPH_tortoiseshellbracketleft, },
    { 0xfe5d, GLYPH_tortoiseshellbracketleftsmall, },
    { 0xfe39, GLYPH_tortoiseshellbracketleftvertical, },
    { 0x3015, GLYPH_tortoiseshellbracketright, },
    { 0xfe5e, GLYPH_tortoiseshellbracketrightsmall, },
    { 0xfe3a, GLYPH_tortoiseshellbracketrightvertical, },
    { 0x0e15, GLYPH_totaothai, },
    { 0x01ab, GLYPH_tpalatalhook, },
    { 0x24af, GLYPH_tparen, },
    { 0x2122, GLYPH_trademark, },
    { 0xf8ea, GLYPH_trademarksans, },
    { 0xf6db, GLYPH_trademarkserif, },
    { 0x0288, GLYPH_tretroflexhook, },
    { 0x25bc, GLYPH_triagdn, },
    { 0x25c4, GLYPH_triaglf, },
    { 0x25ba, GLYPH_triagrt, },
    { 0x25b2, GLYPH_triagup, },
    { 0x02a6, GLYPH_ts, },
    { 0x05e6, GLYPH_tsadi, },
    { 0xfb46, GLYPH_tsadidagesh, },
    { 0xfb46, GLYPH_tsadidageshhebrew, },
    { 0x05e6, GLYPH_tsadihebrew, },
    { 0x0446, GLYPH_tsecyrillic, },
    { 0x05b5, GLYPH_tsere, },
    { 0x05b5, GLYPH_tsere12, },
    { 0x05b5, GLYPH_tsere1e, },
    { 0x05b5, GLYPH_tsere2b, },
    { 0x05b5, GLYPH_tserehebrew, },
    { 0x05b5, GLYPH_tserenarrowhebrew, },
    { 0x05b5, GLYPH_tserequarterhebrew, },
    { 0x05b5, GLYPH_tserewidehebrew, },
    { 0x045b, GLYPH_tshecyrillic, },
    { 0xf6f3, GLYPH_tsuperior, },
    { 0x099f, GLYPH_ttabengali, },
    { 0x091f, GLYPH_ttadeva, },
    { 0x0a9f, GLYPH_ttagujarati, },
    { 0x0a1f, GLYPH_ttagurmukhi, },
    { 0x0679, GLYPH_tteharabic, },
    { 0xfb67, GLYPH_ttehfinalarabic, },
    { 0xfb68, GLYPH_ttehinitialarabic, },
    { 0xfb69, GLYPH_ttehmedialarabic, },
    { 0x09a0, GLYPH_tthabengali, },
    { 0x0920, GLYPH_tthadeva, },
    { 0x0aa0, GLYPH_tthagujarati, },
    { 0x0a20, GLYPH_tthagurmukhi, },
    { 0x0287, GLYPH_tturned, },
    { 0x3064, GLYPH_tuhiragana, },
    { 0x30c4, GLYPH_tukatakana, },
    { 0xff82, GLYPH_tukatakanahalfwidth, },
    { 0x3063, GLYPH_tusmallhiragana, },
    { 0x30c3, GLYPH_tusmallkatakana, },
    { 0xff6f, GLYPH_tusmallkatakanahalfwidth, },
    { 0x246b, GLYPH_twelvecircle, },
    { 0x247f, GLYPH_twelveparen, },
    { 0x2493, GLYPH_twelveperiod, },
    { 0x217b, GLYPH_twelveroman, },
    { 0x2473, GLYPH_twentycircle, },
    { 0x5344, GLYPH_twentyhangzhou, },
    { 0x2487, GLYPH_twentyparen, },
    { 0x249b, GLYPH_twentyperiod, },
    { 0x0032, GLYPH_two, },
    { 0x0662, GLYPH_twoarabic, },
    { 0x09e8, GLYPH_twobengali, },
    { 0x2461, GLYPH_twocircle, },
    { 0x278b, GLYPH_twocircleinversesansserif, },
    { 0x0968, GLYPH_twodeva, },
    { 0x2025, GLYPH_twodotenleader, },
    { 0x2025, GLYPH_twodotleader, },
    { 0xfe30, GLYPH_twodotleadervertical, },
    { 0x0ae8, GLYPH_twogujarati, },
    { 0x0a68, GLYPH_twogurmukhi, },
    { 0x0662, GLYPH_twohackarabic, },
    { 0x3022, GLYPH_twohangzhou, },
    { 0x3221, GLYPH_twoideographicparen, },
    { 0x2082, GLYPH_twoinferior, },
    { 0xff12, GLYPH_twomonospace, },
    { 0x09f5, GLYPH_twonumeratorbengali, },
    { 0xf732, GLYPH_twooldstyle, },
    { 0x2475, GLYPH_twoparen, },
    { 0x2489, GLYPH_twoperiod, },
    { 0x06f2, GLYPH_twopersian, },
    { 0x2171, GLYPH_tworoman, },
    { 0x01bb, GLYPH_twostroke, },
    { 0x00b2, GLYPH_twosuperior, },
    { 0x0e52, GLYPH_twothai, },
    { 0x2154, GLYPH_twothirds, },
    { 0x0075, GLYPH_u, },
    { 0x00fa, GLYPH_uacute, },
    { 0x0289, GLYPH_ubar, },
    { 0x0989, GLYPH_ubengali, },
    { 0x3128, GLYPH_ubopomofo, },
    { 0x016d, GLYPH_ubreve, },
    { 0x01d4, GLYPH_ucaron, },
    { 0x24e4, GLYPH_ucircle, },
    { 0x00fb, GLYPH_ucircumflex, },
    { 0x1e77, GLYPH_ucircumflexbelow, },
    { 0x0443, GLYPH_ucyrillic, },
    { 0x0951, GLYPH_udattadeva, },
    { 0x0171, GLYPH_udblacute, },
    { 0x0215, GLYPH_udblgrave, },
    { 0x0909, GLYPH_udeva, },
    { 0x00fc, GLYPH_udiaeresis, },
    { 0x00fc, GLYPH_udieresis, },
    { 0x01d8, GLYPH_udieresisacute, },
    { 0x1e73, GLYPH_udieresisbelow, },
    { 0x01da, GLYPH_udieresiscaron, },
    { 0x04f1, GLYPH_udieresiscyrillic, },
    { 0x01dc, GLYPH_udieresisgrave, },
    { 0x01d6, GLYPH_udieresismacron, },
    { 0x1ee5, GLYPH_udotbelow, },
    { 0x0171, GLYPH_udoubleacute, },
    { 0x00f9, GLYPH_ugrave, },
    { 0x0a89, GLYPH_ugujarati, },
    { 0x0a09, GLYPH_ugurmukhi, },
    { 0x3046, GLYPH_uhiragana, },
    { 0x1ee7, GLYPH_uhookabove, },
    { 0x01b0, GLYPH_uhorn, },
    { 0x1ee9, GLYPH_uhornacute, },
    { 0x1ef1, GLYPH_uhorndotbelow, },
    { 0x1eeb, GLYPH_uhorngrave, },
    { 0x1eed, GLYPH_uhornhookabove, },
    { 0x1eef, GLYPH_uhorntilde, },
    { 0x0171, GLYPH_uhungar, },
    { 0x0171, GLYPH_uhungarumlaut, },
    { 0x04f3, GLYPH_uhungarumlautcyrillic, },
    { 0x0217, GLYPH_uinvertedbreve, },
    { 0x30a6, GLYPH_ukatakana, },
    { 0xff73, GLYPH_ukatakanahalfwidth, },
    { 0x0479, GLYPH_ukcyrillic, },
    { 0x315c, GLYPH_ukorean, },
    { 0x016b, GLYPH_umacron, },
    { 0x04ef, GLYPH_umacroncyrillic, },
    { 0x1e7b, GLYPH_umacrondieresis, },
    { 0x0a41, GLYPH_umatragurmukhi, },
    { 0xff55, GLYPH_umonospace, },
    { 0x005f, GLYPH_underscore, },
    { 0x2017, GLYPH_underscoredbl, },
    { 0xff3f, GLYPH_underscoremonospace, },
    { 0xfe33, GLYPH_underscorevertical, },
    { 0xfe4f, GLYPH_underscorewavy, },
    { 0x222a, GLYPH_union, },
    { 0x2200, GLYPH_universal, },
    { 0x0173, GLYPH_uogonek, },
    { 0x24b0, GLYPH_uparen, },
    { 0x2580, GLYPH_upblock, },
    { 0x05c4, GLYPH_upperdothebrew, },
    { 0x03c5, GLYPH_upsilon, },
    { 0x03cb, GLYPH_upsilondieresis, },
    { 0x03b0, GLYPH_upsilondieresistonos, },
    { 0x028a, GLYPH_upsilonlatin, },
    { 0x03cd, GLYPH_upsilontonos, },
    { 0x031d, GLYPH_uptackbelowcmb, },
    { 0x02d4, GLYPH_uptackmod, },
    { 0x0a73, GLYPH_uragurmukhi, },
    { 0x016f, GLYPH_uring, },
    { 0x045e, GLYPH_ushortcyrillic, },
    { 0x3045, GLYPH_usmallhiragana, },
    { 0x30a5, GLYPH_usmallkatakana, },
    { 0xff69, GLYPH_usmallkatakanahalfwidth, },
    { 0x04af, GLYPH_ustraightcyrillic, },
    { 0x04b1, GLYPH_ustraightstrokecyrillic, },
    { 0x0169, GLYPH_utilde, },
    { 0x1e79, GLYPH_utildeacute, },
    { 0x1e75, GLYPH_utildebelow, },
    { 0x098a, GLYPH_uubengali, },
    { 0x090a, GLYPH_uudeva, },
    { 0x0a8a, GLYPH_uugujarati, },
    { 0x0a0a, GLYPH_uugurmukhi, },
    { 0x0a42, GLYPH_uumatragurmukhi, },
    { 0x09c2, GLYPH_uuvowelsignbengali, },
    { 0x0942, GLYPH_uuvowelsigndeva, },
    { 0x0ac2, GLYPH_uuvowelsigngujarati, },
    { 0x09c1, GLYPH_uvowelsignbengali, },
    { 0x0941, GLYPH_uvowelsigndeva, },
    { 0x0ac1, GLYPH_uvowelsigngujarati, },
    { 0x0076, GLYPH_v, },
    { 0x0935, GLYPH_vadeva, },
    { 0x0ab5, GLYPH_vagujarati, },
    { 0x0a35, GLYPH_vagurmukhi, },
    { 0x30f7, GLYPH_vakatakana, },
    { 0x05d5, GLYPH_vav, },
    { 0xfb35, GLYPH_vavdagesh, },
    { 0xfb35, GLYPH_vavdagesh65, },
    { 0xfb35, GLYPH_vavdageshhebrew, },
    { 0x05d5, GLYPH_vavhebrew, },
    { 0xfb4b, GLYPH_vavholam, },
    { 0xfb4b, GLYPH_vavholamhebrew, },
    { 0x05f0, GLYPH_vavvavhebrew, },
    { 0x05f1, GLYPH_vavyodhebrew, },
    { 0x24e5, GLYPH_vcircle, },
    { 0x1e7f, GLYPH_vdotbelow, },
    { 0x0432, GLYPH_vecyrillic, },
    { 0x06a4, GLYPH_veharabic, },
    { 0xfb6b, GLYPH_vehfinalarabic, },
    { 0xfb6c, GLYPH_vehinitialarabic, },
    { 0xfb6d, GLYPH_vehmedialarabic, },
    { 0x30f9, GLYPH_vekatakana, },
    { 0x2640, GLYPH_venus, },
    { 0x007c, GLYPH_verticalbar, },
    { 0x030d, GLYPH_verticallineabovecmb, },
    { 0x0329, GLYPH_verticallinebelowcmb, },
    { 0x02cc, GLYPH_verticallinelowmod, },
    { 0x02c8, GLYPH_verticallinemod, },
    { 0x057e, GLYPH_vewarmenian, },
    { 0x028b, GLYPH_vhook, },
    { 0x30f8, GLYPH_vikatakana, },
    { 0x09cd, GLYPH_viramabengali, },
    { 0x094d, GLYPH_viramadeva, },
    { 0x0acd, GLYPH_viramagujarati, },
    { 0x0983, GLYPH_visargabengali, },
    { 0x0903, GLYPH_visargadeva, },
    { 0x0a83, GLYPH_visargagujarati, },
    { 0xff56, GLYPH_vmonospace, },
    { 0x0578, GLYPH_voarmenian, },
    { 0x309e, GLYPH_voicediterationhiragana, },
    { 0x30fe, GLYPH_voicediterationkatakana, },
    { 0x309b, GLYPH_voicedmarkkana, },
    { 0xff9e, GLYPH_voicedmarkkanahalfwidth, },
    { 0x30fa, GLYPH_vokatakana, },
    { 0x24b1, GLYPH_vparen, },
    { 0x1e7d, GLYPH_vtilde, },
    { 0x028c, GLYPH_vturned, },
    { 0x3094, GLYPH_vuhiragana, },
    { 0x30f4, GLYPH_vukatakana, },
    { 0x0077, GLYPH_w, },
    { 0x1e83, GLYPH_wacute, },
    { 0x3159, GLYPH_waekorean, },
    { 0x308f, GLYPH_wahiragana, },
    { 0x30ef, GLYPH_wakatakana, },
    { 0xff9c, GLYPH_wakatakanahalfwidth, },
    { 0x3158, GLYPH_wakorean, },
    { 0x308e, GLYPH_wasmallhiragana, },
    { 0x30ee, GLYPH_wasmallkatakana, },
    { 0x3357, GLYPH_wattosquare, },
    { 0x301c, GLYPH_wavedash, },
    { 0xfe34, GLYPH_wavyunderscorevertical, },
    { 0x0648, GLYPH_wawarabic, },
    { 0xfeee, GLYPH_wawfinalarabic, },
    { 0x0624, GLYPH_wawhamzaabovearabic, },
    { 0xfe86, GLYPH_wawhamzaabovefinalarabic, },
    { 0x33dd, GLYPH_wbsquare, },
    { 0x24e6, GLYPH_wcircle, },
    { 0x0175, GLYPH_wcircumflex, },
    { 0x1e85, GLYPH_wdieresis, },
    { 0x1e87, GLYPH_wdotaccent, },
    { 0x1e89, GLYPH_wdotbelow, },
    { 0x3091, GLYPH_wehiragana, },
    { 0x2118, GLYPH_weierstrass, },
    { 0x30f1, GLYPH_wekatakana, },
    { 0x315e, GLYPH_wekorean, },
    { 0x315d, GLYPH_weokorean, },
    { 0x1e81, GLYPH_wgrave, },
    { 0x25e6, GLYPH_whitebullet, },
    { 0x25cb, GLYPH_whitecircle, },
    { 0x25d9, GLYPH_whitecircleinverse, },
    { 0x300e, GLYPH_whitecornerbracketleft, },
    { 0xfe43, GLYPH_whitecornerbracketleftvertical, },
    { 0x300f, GLYPH_whitecornerbracketright, },
    { 0xfe44, GLYPH_whitecornerbracketrightvertical, },
    { 0x25c7, GLYPH_whitediamond, },
    { 0x25c8, GLYPH_whitediamondcontainingblacksmalldiamond, },
    { 0x25bf, GLYPH_whitedownpointingsmalltriangle, },
    { 0x25bd, GLYPH_whitedownpointingtriangle, },
    { 0x25c3, GLYPH_whiteleftpointingsmalltriangle, },
    { 0x25c1, GLYPH_whiteleftpointingtriangle, },
    { 0x3016, GLYPH_whitelenticularbracketleft, },
    { 0x3017, GLYPH_whitelenticularbracketright, },
    { 0x25b9, GLYPH_whiterightpointingsmalltriangle, },
    { 0x25b7, GLYPH_whiterightpointingtriangle, },
    { 0x25ab, GLYPH_whitesmallsquare, },
    { 0x263a, GLYPH_whitesmilingface, },
    { 0x25a1, GLYPH_whitesquare, },
    { 0x2606, GLYPH_whitestar, },
    { 0x260f, GLYPH_whitetelephone, },
    { 0x3018, GLYPH_whitetortoiseshellbracketleft, },
    { 0x3019, GLYPH_whitetortoiseshellbracketright, },
    { 0x25b5, GLYPH_whiteuppointingsmalltriangle, },
    { 0x25b3, GLYPH_whiteuppointingtriangle, },
    { 0x3090, GLYPH_wihiragana, },
    { 0x30f0, GLYPH_wikatakana, },
    { 0x315f, GLYPH_wikorean, },
    { 0xff57, GLYPH_wmonospace, },
    { 0x3092, GLYPH_wohiragana, },
    { 0x30f2, GLYPH_wokatakana, },
    { 0xff66, GLYPH_wokatakanahalfwidth, },
    { 0x20a9, GLYPH_won, },
    { 0xffe6, GLYPH_wonmonospace, },
    { 0x0e27, GLYPH_wowaenthai, },
    { 0x24b2, GLYPH_wparen, },
    { 0x1e98, GLYPH_wring, },
    { 0x02b7, GLYPH_wsuperior, },
    { 0x028d, GLYPH_wturned, },
    { 0x01bf, GLYPH_wynn, },
    { 0x0078, GLYPH_x, },
    { 0x033d, GLYPH_xabovecmb, },
    { 0x3112, GLYPH_xbopomofo, },
    { 0x24e7, GLYPH_xcircle, },
    { 0x1e8d, GLYPH_xdieresis, },
    { 0x1e8b, GLYPH_xdotaccent, },
    { 0x056d, GLYPH_xeharmenian, },
    { 0x03be, GLYPH_xi, },
    { 0xff58, GLYPH_xmonospace, },
    { 0x24b3, GLYPH_xparen, },
    { 0x02e3, GLYPH_xsuperior, },
    { 0x0079, GLYPH_y, },
    { 0x334e, GLYPH_yaadosquare, },
    { 0x09af, GLYPH_yabengali, },
    { 0x00fd, GLYPH_yacute, },
    { 0x092f, GLYPH_yadeva, },
    { 0x3152, GLYPH_yaekorean, },
    { 0x0aaf, GLYPH_yagujarati, },
    { 0x0a2f, GLYPH_yagurmukhi, },
    { 0x3084, GLYPH_yahiragana, },
    { 0x30e4, GLYPH_yakatakana, },
    { 0xff94, GLYPH_yakatakanahalfwidth, },
    { 0x3151, GLYPH_yakorean, },
    { 0x0e4e, GLYPH_yamakkanthai, },
    { 0x3083, GLYPH_yasmallhiragana, },
    { 0x30e3, GLYPH_yasmallkatakana, },
    { 0xff6c, GLYPH_yasmallkatakanahalfwidth, },
    { 0x0463, GLYPH_yatcyrillic, },
    { 0x24e8, GLYPH_ycircle, },
    { 0x0177, GLYPH_ycircumflex, },
    { 0x00ff, GLYPH_ydieresis, },
    { 0x1e8f, GLYPH_ydotaccent, },
    { 0x1ef5, GLYPH_ydotbelow, },
    { 0x064a, GLYPH_yeharabic, },
    { 0x06d2, GLYPH_yehbarreearabic, },
    { 0xfbaf, GLYPH_yehbarreefinalarabic, },
    { 0xfef2, GLYPH_yehfinalarabic, },
    { 0x0626, GLYPH_yehhamzaabovearabic, },
    { 0xfe8a, GLYPH_yehhamzaabovefinalarabic, },
    { 0xfe8b, GLYPH_yehhamzaaboveinitialarabic, },
    { 0xfe8c, GLYPH_yehhamzaabovemedialarabic, },
    { 0xfef3, GLYPH_yehinitialarabic, },
    { 0xfef4, GLYPH_yehmedialarabic, },
    { 0xfcdd, GLYPH_yehmeeminitialarabic, },
    { 0xfc58, GLYPH_yehmeemisolatedarabic, },
    { 0xfc94, GLYPH_yehnoonfinalarabic, },
    { 0x06d1, GLYPH_yehthreedotsbelowarabic, },
    { 0x3156, GLYPH_yekorean, },
    { 0x00a5, GLYPH_yen, },
    { 0xffe5, GLYPH_yenmonospace, },
    { 0x3155, GLYPH_yeokorean, },
    { 0x3186, GLYPH_yeorinhieuhkorean, },
    { 0x05aa, GLYPH_yerahbenyomohebrew, },
    { 0x05aa, GLYPH_yerahbenyomolefthebrew, },
    { 0x044b, GLYPH_yericyrillic, },
    { 0x04f9, GLYPH_yerudieresiscyrillic, },
    { 0x3181, GLYPH_yesieungkorean, },
    { 0x3183, GLYPH_yesieungpansioskorean, },
    { 0x3182, GLYPH_yesieungsioskorean, },
    { 0x059a, GLYPH_yetivhebrew, },
    { 0x1ef3, GLYPH_ygrave, },
    { 0x01b4, GLYPH_yhook, },
    { 0x1ef7, GLYPH_yhookabove, },
    { 0x0575, GLYPH_yiarmenian, },
    { 0x0457, GLYPH_yicyrillic, },
    { 0x3162, GLYPH_yikorean, },
    { 0x262f, GLYPH_yinyang, },
    { 0x0582, GLYPH_yiwnarmenian, },
    { 0xff59, GLYPH_ymonospace, },
    { 0x05d9, GLYPH_yod, },
    { 0xfb39, GLYPH_yoddagesh, },
    { 0xfb39, GLYPH_yoddageshhebrew, },
    { 0x05d9, GLYPH_yodhebrew, },
    { 0x05f2, GLYPH_yodyodhebrew, },
    { 0xfb1f, GLYPH_yodyodpatahhebrew, },
    { 0x3088, GLYPH_yohiragana, },
    { 0x3189, GLYPH_yoikorean, },
    { 0x30e8, GLYPH_yokatakana, },
    { 0xff96, GLYPH_yokatakanahalfwidth, },
    { 0x315b, GLYPH_yokorean, },
    { 0x3087, GLYPH_yosmallhiragana, },
    { 0x30e7, GLYPH_yosmallkatakana, },
    { 0xff6e, GLYPH_yosmallkatakanahalfwidth, },
    { 0x03f3, GLYPH_yotgreek, },
    { 0x3188, GLYPH_yoyaekorean, },
    { 0x3187, GLYPH_yoyakorean, },
    { 0x0e22, GLYPH_yoyakthai, },
    { 0x0e0d, GLYPH_yoyingthai, },
    { 0x24b4, GLYPH_yparen, },
    { 0x037a, GLYPH_ypogegrammeni, },
    { 0x0345, GLYPH_ypogegrammenigreekcmb, },
    { 0x01a6, GLYPH_yr, },
    { 0x1e99, GLYPH_yring, },
    { 0x02b8, GLYPH_ysuperior, },
    { 0x1ef9, GLYPH_ytilde, },
    { 0x028e, GLYPH_yturned, },
    { 0x3086, GLYPH_yuhiragana, },
    { 0x318c, GLYPH_yuikorean, },
    { 0x30e6, GLYPH_yukatakana, },
    { 0xff95, GLYPH_yukatakanahalfwidth, },
    { 0x3160, GLYPH_yukorean, },
    { 0x046b, GLYPH_yusbigcyrillic, },
    { 0x046d, GLYPH_yusbigiotifiedcyrillic, },
    { 0x0467, GLYPH_yuslittlecyrillic, },
    { 0x0469, GLYPH_yuslittleiotifiedcyrillic, },
    { 0x3085, GLYPH_yusmallhiragana, },
    { 0x30e5, GLYPH_yusmallkatakana, },
    { 0xff6d, GLYPH_yusmallkatakanahalfwidth, },
    { 0x318b, GLYPH_yuyekorean, },
    { 0x318a, GLYPH_yuyeokorean, },
    { 0x09df, GLYPH_yyabengali, },
    { 0x095f, GLYPH_yyadeva, },
    { 0x007a, GLYPH_z, },
    { 0x0566, GLYPH_zaarmenian, },
    { 0x017c, GLYPH_zabovedot, },
    { 0x017a, GLYPH_zacute, },
    { 0x095b, GLYPH_zadeva, },
    { 0x0a5b, GLYPH_zagurmukhi, },
    { 0x0638, GLYPH_zaharabic, },
    { 0xfec6, GLYPH_zahfinalarabic, },
    { 0xfec7, GLYPH_zahinitialarabic, },
    { 0x3056, GLYPH_zahiragana, },
    { 0xfec8, GLYPH_zahmedialarabic, },
    { 0x0632, GLYPH_zainarabic, },
    { 0xfeb0, GLYPH_zainfinalarabic, },
    { 0x30b6, GLYPH_zakatakana, },
    { 0x0595, GLYPH_zaqefgadolhebrew, },
    { 0x0594, GLYPH_zaqefqatanhebrew, },
    { 0x0598, GLYPH_zarqahebrew, },
    { 0x05d6, GLYPH_zayin, },
    { 0xfb36, GLYPH_zayindagesh, },
    { 0xfb36, GLYPH_zayindageshhebrew, },
    { 0x05d6, GLYPH_zayinhebrew, },
    { 0x3117, GLYPH_zbopomofo, },
    { 0x017e, GLYPH_zcaron, },
    { 0x24e9, GLYPH_zcircle, },
    { 0x1e91, GLYPH_zcircumflex, },
    { 0x0291, GLYPH_zcurl, },
    { 0x017c, GLYPH_zdot, },
    { 0x017c, GLYPH_zdotaccent, },
    { 0x1e93, GLYPH_zdotbelow, },
    { 0x0437, GLYPH_zecyrillic, },
    { 0x0499, GLYPH_zedescendercyrillic, },
    { 0x04df, GLYPH_zedieresiscyrillic, },
    { 0x305c, GLYPH_zehiragana, },
    { 0x30bc, GLYPH_zekatakana, },
    { 0x0030, GLYPH_zero, },
    { 0x0660, GLYPH_zeroarabic, },
    { 0x09e6, GLYPH_zerobengali, },
    { 0x0966, GLYPH_zerodeva, },
    { 0x0ae6, GLYPH_zerogujarati, },
    { 0x0a66, GLYPH_zerogurmukhi, },
    { 0x0660, GLYPH_zerohackarabic, },
    { 0x2080, GLYPH_zeroinferior, },
    { 0xff10, GLYPH_zeromonospace, },
    { 0xf730, GLYPH_zerooldstyle, },
    { 0x06f0, GLYPH_zeropersian, },
    { 0x2070, GLYPH_zerosuperior, },
    { 0x0e50, GLYPH_zerothai, },
    { 0xfeff, GLYPH_zerowidthjoiner, },
    { 0x200c, GLYPH_zerowidthnonjoiner, },
    { 0x200b, GLYPH_zerowidthspace, },
    { 0x03b6, GLYPH_zeta, },
    { 0x3113, GLYPH_zhbopomofo, },
    { 0x056a, GLYPH_zhearmenian, },
    { 0x04c2, GLYPH_zhebrevecyrillic, },
    { 0x0436, GLYPH_zhecyrillic, },
    { 0x0497, GLYPH_zhedescendercyrillic, },
    { 0x04dd, GLYPH_zhedieresiscyrillic, },
    { 0x3058, GLYPH_zihiragana, },
    { 0x30b8, GLYPH_zikatakana, },
    { 0x05ae, GLYPH_zinorhebrew, },
    { 0x1e95, GLYPH_zlinebelow, },
    { 0xff5a, GLYPH_zmonospace, },
    { 0x305e, GLYPH_zohiragana, },
    { 0x30be, GLYPH_zokatakana, },
    { 0x24b5, GLYPH_zparen, },
    { 0x0290, GLYPH_zretroflexhook, },
    { 0x01b6, GLYPH_zstroke, },
    { 0x305a, GLYPH_zuhiragana, },
    { 0x30ba, GLYPH_zukatakana, },
};

/************************************************************************/
/*									*/
/*  Translate a unicode value to a glyph name.				*/
/*									*/
/*  As the callers use the names inside fonts: Use the glyph list for	*/
/*  new fonts.								*/
/*									*/
/************************************************************************/

const char * psUnicodeToGlyphName(	int	unicode )
    {
    static char	scratch[20];

    int		l= 0;
    int		r= sizeof(psUnicodeToGlyphNames)/sizeof(UnicodeToGlyphName);
    int		m= ( l+ r )/ 2;

    while( m != l )
	{
	if  ( psUnicodeToGlyphNames[m].utgUnicode > unicode )
	    { r= m;	}
	else{ l= m;	}

	m= ( l+ r )/ 2;
	}

    if  ( psUnicodeToGlyphNames[m].utgUnicode == unicode )
	{ return  psUnicodeToGlyphNames[m].utgGlyphName;	}

    sprintf( scratch, "uni%04X", unicode );

    return scratch;
    }

/************************************************************************/
/*									*/
/*  Translate a glyph name to a unicode value.				*/
/*									*/
/*  As the callers use the names to identify glyphs by name, use the	*/
/*  adobe glyph list and some heuristics to perform the mapping.	*/
/*									*/
/************************************************************************/

int psGlyphNameToUnicode(	const char * glyphname )
    {
    int		l= 0;
    int		r= sizeof(psGlyphNameToUnicodes)/sizeof(UnicodeToGlyphName);
    int		m= ( l+ r )/ 2;

    unsigned int	unicode;
    char		c;
    int			cmp;

    if  ( sscanf( glyphname, "uni%x%c", &unicode, &c ) == 1 )
	{
	if  ( unicode >= 256* 256 )
	    { /* SXDEB(glyphname,unicode); */ return -1;	}

	return unicode;
	}

    while( m != l )
	{
	cmp= strcmp( psGlyphNameToUnicodes[m].utgGlyphName, glyphname );

	if  ( cmp > 0 )
	    { r= m;	}
	else{ l= m;	}

	m= ( l+ r )/ 2;
	}

    cmp= strcmp( psGlyphNameToUnicodes[m].utgGlyphName, glyphname );
    if  ( cmp == 0 )
	{ return  psGlyphNameToUnicodes[m].utgUnicode;	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Standard apple encoding as used in a true type file.		*/
/*									*/
/************************************************************************/

static const char GLYPH_nonmarkingreturn[]= "nonmarkingreturn";

const char * const psAppleTtfGlyphNames[258]=
{
    GLYPH__notdef,
    GLYPH__null,
    GLYPH_nonmarkingreturn,
    GLYPH_space,
    GLYPH_exclam,
    GLYPH_quotedbl,
    GLYPH_numbersign,
    GLYPH_dollar,
    GLYPH_percent,
    GLYPH_ampersand,
    GLYPH_quotesingle,
    GLYPH_parenleft,
    GLYPH_parenright,
    GLYPH_asterisk,
    GLYPH_plus,
    GLYPH_comma,
    GLYPH_hyphen,
    GLYPH_period,
    GLYPH_slash,
    GLYPH_zero,
    GLYPH_one,
    GLYPH_two,
    GLYPH_three,
    GLYPH_four,
    GLYPH_five,
    GLYPH_six,
    GLYPH_seven,
    GLYPH_eight,
    GLYPH_nine,
    GLYPH_colon,
    GLYPH_semicolon,
    GLYPH_less,
    GLYPH_equal,
    GLYPH_greater,
    GLYPH_question,
    GLYPH_at,
    GLYPH_A,
    GLYPH_B,
    GLYPH_C,
    GLYPH_D,
    GLYPH_E,
    GLYPH_F,
    GLYPH_G,
    GLYPH_H,
    GLYPH_I,
    GLYPH_J,
    GLYPH_K,
    GLYPH_L,
    GLYPH_M,
    GLYPH_N,
    GLYPH_O,
    GLYPH_P,
    GLYPH_Q,
    GLYPH_R,
    GLYPH_S,
    GLYPH_T,
    GLYPH_U,
    GLYPH_V,
    GLYPH_W,
    GLYPH_X,
    GLYPH_Y,
    GLYPH_Z,
    GLYPH_bracketleft,
    GLYPH_backslash,
    GLYPH_bracketright,
    GLYPH_asciicircum,
    GLYPH_underscore,
    GLYPH_grave,
    GLYPH_a,
    GLYPH_b,
    GLYPH_c,
    GLYPH_d,
    GLYPH_e,
    GLYPH_f,
    GLYPH_g,
    GLYPH_h,
    GLYPH_i,
    GLYPH_j,
    GLYPH_k,
    GLYPH_l,
    GLYPH_m,
    GLYPH_n,
    GLYPH_o,
    GLYPH_p,
    GLYPH_q,
    GLYPH_r,
    GLYPH_s,
    GLYPH_t,
    GLYPH_u,
    GLYPH_v,
    GLYPH_w,
    GLYPH_x,
    GLYPH_y,
    GLYPH_z,
    GLYPH_braceleft,
    GLYPH_bar,
    GLYPH_braceright,
    GLYPH_asciitilde,
    GLYPH_Adieresis,
    GLYPH_Aring,
    GLYPH_Ccedilla,
    GLYPH_Eacute,
    GLYPH_Ntilde,
    GLYPH_Odieresis,
    GLYPH_Udieresis,
    GLYPH_aacute,
    GLYPH_agrave,
    GLYPH_acircumflex,
    GLYPH_adieresis,
    GLYPH_atilde,
    GLYPH_aring,
    GLYPH_ccedilla,
    GLYPH_eacute,
    GLYPH_egrave,
    GLYPH_ecircumflex,
    GLYPH_edieresis,
    GLYPH_iacute,
    GLYPH_igrave,
    GLYPH_icircumflex,
    GLYPH_idieresis,
    GLYPH_ntilde,
    GLYPH_oacute,
    GLYPH_ograve,
    GLYPH_ocircumflex,
    GLYPH_odieresis,
    GLYPH_otilde,
    GLYPH_uacute,
    GLYPH_ugrave,
    GLYPH_ucircumflex,
    GLYPH_udieresis,
    GLYPH_dagger,
    GLYPH_degree,
    GLYPH_cent,
    GLYPH_sterling,
    GLYPH_section,
    GLYPH_bullet,
    GLYPH_paragraph,
    GLYPH_germandbls,
    GLYPH_registered,
    GLYPH_copyright,
    GLYPH_trademark,
    GLYPH_acute,
    GLYPH_dieresis,
    GLYPH_notequal,
    GLYPH_AE,
    GLYPH_Oslash,
    GLYPH_infinity,
    GLYPH_plusminus,
    GLYPH_lessequal,
    GLYPH_greaterequal,
    GLYPH_yen,
    GLYPH_mu,
    GLYPH_partialdiff,
    GLYPH_summation,
    GLYPH_product,
    GLYPH_pi,
    GLYPH_integral,
    GLYPH_ordfeminine,
    GLYPH_ordmasculine,
    GLYPH_Omega,
    GLYPH_ae,
    GLYPH_oslash,
    GLYPH_questiondown,
    GLYPH_exclamdown,
    GLYPH_logicalnot,
    GLYPH_radical,
    GLYPH_florin,
    GLYPH_approxequal,
    GLYPH_Delta,
    GLYPH_guillemotleft,
    GLYPH_guillemotright,
    GLYPH_ellipsis,
    GLYPH_nonbreakingspace,
    GLYPH_Agrave,
    GLYPH_Atilde,
    GLYPH_Otilde,
    GLYPH_OE,
    GLYPH_oe,
    GLYPH_endash,
    GLYPH_emdash,
    GLYPH_quotedblleft,
    GLYPH_quotedblright,
    GLYPH_quoteleft,
    GLYPH_quoteright,
    GLYPH_divide,
    GLYPH_lozenge,
    GLYPH_ydieresis,
    GLYPH_Ydieresis,
    GLYPH_fraction,
    GLYPH_currency,
    GLYPH_guilsinglleft,
    GLYPH_guilsinglright,
    GLYPH_fi,
    GLYPH_fl,
    GLYPH_daggerdbl,
    GLYPH_periodcentered,
    GLYPH_quotesinglbase,
    GLYPH_quotedblbase,
    GLYPH_perthousand,
    GLYPH_Acircumflex,
    GLYPH_Ecircumflex,
    GLYPH_Aacute,
    GLYPH_Edieresis,
    GLYPH_Egrave,
    GLYPH_Iacute,
    GLYPH_Icircumflex,
    GLYPH_Idieresis,
    GLYPH_Igrave,
    GLYPH_Oacute,
    GLYPH_Ocircumflex,
    GLYPH_apple,
    GLYPH_Ograve,
    GLYPH_Uacute,
    GLYPH_Ucircumflex,
    GLYPH_Ugrave,
    GLYPH_dotlessi,
    GLYPH_circumflex,
    GLYPH_tilde,
    GLYPH_macron,
    GLYPH_breve,
    GLYPH_dotaccent,
    GLYPH_ring,
    GLYPH_cedilla,
    GLYPH_hungarumlaut,
    GLYPH_ogonek,
    GLYPH_caron,
    GLYPH_Lslash,
    GLYPH_lslash,
    GLYPH_Scaron,
    GLYPH_scaron,
    GLYPH_Zcaron,
    GLYPH_zcaron,
    GLYPH_brokenbar,
    GLYPH_Eth,
    GLYPH_eth,
    GLYPH_Yacute,
    GLYPH_yacute,
    GLYPH_Thorn,
    GLYPH_thorn,
    GLYPH_minus,
    GLYPH_multiply,
    GLYPH_onesuperior,
    GLYPH_twosuperior,
    GLYPH_threesuperior,
    GLYPH_onehalf,
    GLYPH_onequarter,
    GLYPH_threequarters,
    GLYPH_franc,
    GLYPH_Gbreve,
    GLYPH_gbreve,
    GLYPH_Idotaccent,
    GLYPH_Scedilla,
    GLYPH_scedilla,
    GLYPH_Cacute,
    GLYPH_cacute,
    GLYPH_Ccaron,
    GLYPH_ccaron,
    GLYPH_dcroat,
};

