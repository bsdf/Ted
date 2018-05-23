/************************************************************************/
/*									*/
/*  Strings and arrays of strings that are used as RTF tags.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_TAGS_H
#   define	DOC_RTF_TAGS_H

extern const char * const	DOCrtf_DocFootNotesJustificationTags[];
extern const int		DOCrtf_DocFootNotesJustificationTagCount;

extern const char * const	DOCrtf_DocEndNotesJustificationTags[];
extern const int		DOCrtf_DocEndNotesJustificationTagCount;

extern const char * const	DOCrtf_DocFootNotesPlacementTags[];
extern const int		DOCrtf_DocFootNotesPlacementTagCount;

extern const char * const	DOCrtf_DocEndNotesPlacementTags[];
extern const int		DOCrtf_DocEndNotesPlacementTagCount;

extern const char * const	DOCrtf_SectFootNotesJustificationTags[];
extern const int		DOCrtf_SectFootNotesJustificationTagCount;

extern const char * const	DOCrtf_SectEndNotesJustificationTags[];
extern const int		DOCrtf_SectEndNotesJustificationTagCount;

extern const char * const	DOCrtf_SectEndNotesPlacementTags[];
extern const int		DOCrtf_SectEndNotesPlacementTagCount;

extern const char * const	DOCrtf_DocFootNotesRestartTags[];
extern const int		DOCrtf_DocFootNotesRestartTagCount;

extern const char * const	DOCrtf_DocEndNotesRestartTags[];
extern const int		DOCrtf_DocEndNotesRestartTagCount;

extern const char * const	DOCrtf_SectFootNotesRestartTags[];
extern const int		DOCrtf_SectFootNotesRestartTagCount;

extern const char * const	DOCrtf_SectEndNotesRestartTags[];
extern const int		DOCrtf_SectEndNotesRestartTagCount;

extern const char * const	DOCrtf_DocFootNotesNumberStyleTags[];
extern const int		DOCrtf_DocFootNotesNumberStyleTagCount;

extern const char * const	DOCrtf_DocEndNotesNumberStyleTags[];
extern const int		DOCrtf_DocEndNotesNumberStyleTagCount;

extern const char * const	DOCrtf_SectFootNotesNumberStyleTags[];
extern const int		DOCrtf_SectFootNotesNumberStyleTagCount;

extern const char * const	DOCrtf_SectEndNotesNumberStyleTags[];
extern const int		DOCrtf_SectEndNotesNumberStyleTagCount;

extern const char * const	DOCrtf_BorderStyleTags[];
extern const int		DOCrtf_BorderStyleTagCount;

extern const char * const	DOCrtf_ParaShadingTags[];
extern const int		DOCrtf_ParaShadingTagCount;

extern const char * const	DOCrtf_TextShadingTags[];
extern const int		DOCrtf_TextShadingTagCount;

extern const char * const	DOCrtf_RowShadingTags[];
extern const int		DOCrtf_RowShadingTagCount;

extern const char * const	DOCrtf_CellShadingTags[];
extern const int		DOCrtf_CellShadingTagCount;

extern const char * const	DOCrtf_SectBreakTags[];
extern const int		DOCrtf_SectBreakTagCount;

extern const char * const	DOCrtf_ParaBreakTags[];
extern const int		DOCrtf_ParaBreakTagCount;

extern const char * const	DOCrtf_PageNumberStyleTags[];
extern const int		DOCrtf_PageNumberStyleTagCount;

extern const char * const	DOCrtf_PageNumberHyphenTags[];
extern const int		DOCrtf_PageNumberHyphenTagCount;

extern const char * const	DOCrtf_CellValignTags[];
extern const int		DOCrtf_CellValignTagCount;

extern const char * const	DOCrtf_CellTextFlowTags[];
extern const int		DOCrtf_CellTextFlowTagCount;

extern const char * const	DOCrtf_CellHorMergeTags[];
extern const int		DOCrtf_CellHorMergeTagCount;

extern const char * const	DOCrtf_CellVerMergeTags[];
extern const int		DOCrtf_CellVerMergeTagCount;

extern const char * const	DOCrtf_ParaAlignTags[];
extern const int		DOCrtf_ParaAlignTagCount;

extern const char * const	DOCrtf_TabAlignTags[];
extern const int		DOCrtf_TabAlignTagCount;

extern const char * const	DOCrtf_TabLeaderTags[];
extern const int		DOCrtf_TabLeaderTagCount;

extern const char * const	DOCrtf_SupersubTags[];
extern const int		DOCrtf_SupersubTagCount;

extern const char RTFtag_pict[];
extern const char RTFtag_result[];

extern const char RTFtag_par[];
extern const char RTFtag_pard[];
extern const char RTFtag_plain[];

extern const char RTFtag_footnote[];
extern const char RTFtag_chftn[];
extern const char RTFtag_fet[];

extern const char RTFtag_revtim[];
extern const char RTFtag_hlinkbase[];
extern const char RTFtag__generator[];

extern const char RTFtag_nosupersub[];
extern const char RTFtag_super[];
extern const char RTFtag_sub[];

extern const char RTFtag_tql[];
extern const char RTFtag_tqr[];
extern const char RTFtag_tqc[];
extern const char RTFtag_tqdec[];

/* Tags for copy/paste and the Trace */

extern const char RTFtag__SelOpen[];

/* Tags for the Trace: Are terse to keep the trace compact and in capitals */
/* to avoid conflicts with the real RTF tags. */

extern const char RTFtag_OTX[];
extern const char RTFtag_NTX[];
extern const char RTFtag_NPR[];

extern const char RTFtag_SLI[];
extern const char RTFtag_SBK[];
extern const char RTFtag_DFL[];
extern const char RTFtag_UFL[];

extern const char RTFtag_ULI[];
extern const char RTFtag_NLI[];
extern const char RTFtag_NNO[];
extern const char RTFtag_UNO[];
extern const char RTFtag_UTA[];
extern const char RTFtag_UPP[];
extern const char RTFtag_UTP[];
extern const char RTFtag_USP[];
extern const char RTFtag_USDP[];
extern const char RTFtag_UDP[];
extern const char RTFtag_REP[];
extern const char RTFtag_REPF[];
extern const char RTFtag_REPB[];
extern const char RTFtag_DELS[];
extern const char RTFtag_DELF[];
extern const char RTFtag_DELB[];
extern const char RTFtag_REX[];
extern const char RTFtag_MPA[];

extern const char RTFtag_DPA[];
extern const char RTFtag_DSE[];
extern const char RTFtag_DRW[];
extern const char RTFtag_DTB[];
extern const char RTFtag_DCO[];
extern const char RTFtag_DHD[];
extern const char RTFtag_DFT[];

extern const char RTFtag_IPA[];
extern const char RTFtag_APA[];
extern const char RTFtag_ITB[];
extern const char RTFtag_ISE[];
extern const char RTFtag_ASE[];
extern const char RTFtag_SSE[];
extern const char RTFtag_IRW[];
extern const char RTFtag_ARW[];
extern const char RTFtag_ICO[];
extern const char RTFtag_ACO[];
extern const char RTFtag_IHD[];
extern const char RTFtag_IFT[];

extern const char RTFtag_UOB[];
extern const char RTFtag_SRWD[];
extern const char RTFtag_SRWU[];

extern const char RTFtag_SPR[];
extern const char RTFtag_SPL[];
extern const char RTFtag_Open[];
extern const char RTFtag_Save[];
extern const char RTFtag_New[];

extern const char RTFtag_OTT[];
extern const char RTFtag_OSN[];
extern const char RTFtag_OOSN[];
extern const char RTFtag_OON[];
extern const char RTFtag_OPG[];
extern const char RTFtag_OCO[];

extern const char RTFtag_NTT[];
extern const char RTFtag_NSN[];
extern const char RTFtag_NOSN[];
extern const char RTFtag_NON[];
extern const char RTFtag_NPG[];
extern const char RTFtag_NCO[];

extern const char RTFtag_OHC[];
extern const char RTFtag_OHP[];
extern const char RTFtag_OHS[];
extern const char RTFtag_PRL[];

extern const char RTFtag_OTC[];
extern const char RTFtag_OTP[];
extern const char RTFtag_OTS[];

extern const char RTFtag_NHP[];
extern const char RTFtag_NHS[];

extern const char RTFtag_NTP[];
extern const char RTFtag_NTS[];

extern const char RTFtag_SEH[];
extern const char RTFtag_SEN[];
extern const char RTFtag_SAL[];
extern const char RTFtag_SET[];
extern const char RTFtag_SEB[];
extern const char RTFtag_SEA[];

extern const char RTFtag_FLK[];

#    endif	/*  DOC_RTF_TAGS_H	*/
