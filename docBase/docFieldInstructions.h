/************************************************************************/
/*									*/
/*  FieldInstructions: What to do with a field.				*/
/*									*/
/************************************************************************/

#   ifndef DOC_FIELD_INSTRUCTIONS_H
#   define DOC_FIELD_INSTRUCTIONS_H

#   include	<utilMemoryBuffer.h>

typedef struct InstructionsComponent
    {
    MemoryBuffer	icBuffer;
    unsigned char	icIsFlag;
    unsigned char	icIsQuoted;
    } InstructionsComponent;

typedef struct FieldInstructions
    {
    InstructionsComponent *	fiComponents;
    int				fiComponentCount;
    } FieldInstructions;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitFieldInstructions(	FieldInstructions *		fi );
extern void docCleanFieldInstructions(	FieldInstructions *		fi );

extern int docCopyFieldInstructions(	FieldInstructions *		to,
					const FieldInstructions *	from );

extern int docFieldInstructionsGetComponent(
				const MemoryBuffer **		pComponent,
				const FieldInstructions *	fi,
				int				comp );

extern int docAllocateComponents(	FieldInstructions *		fi,
					int				n );

extern int docSetFieldInstructions(	FieldInstructions *	fi,
					const char *		bytes,
					int			size );

extern int docFieldComponentInt(	int *				pVal,
					const InstructionsComponent *	ic ) ;

extern int docComponentIsArgFlag(	const FieldInstructions *	fi,
					int				comp,
					int				flag );

extern int docFieldHasMergeformat(	const FieldInstructions *	ic,
					int				comp );


extern int docComponentIsFlag(		const FieldInstructions *	fi,
					int				comp,
					int				flag );

extern int docFieldInstructionsAddFlag(
				FieldInstructions *	fi,
				int			flag );

extern int docStartFieldInstructions(	FieldInstructions *	fi,
					const char *		kindName,
					int			kindSize );

extern int docFieldInstructionsAddArgFlag(
					FieldInstructions *	fi,
					int			flag,
					const MemoryBuffer *	valueName );

extern int docFieldInstructionsAddArgFlagIfSet(
					FieldInstructions *	fi,
					int			flag,
					const MemoryBuffer *	valueName );

extern int docFieldInstructionsAddComponent(
					FieldInstructions *	fi,
					const MemoryBuffer *	mb );

extern int docFieldInstructionsAddFlagComponent(
					FieldInstructions *	fi,
					int			flag,
					const MemoryBuffer *	mb );

extern int docFieldComponentNumberFormat(
				unsigned char *			pNumberFormat,
				const InstructionsComponent *	ic ) ;

extern int docFieldComponentCaseShift(
				unsigned char *			pCaseShift,
				const InstructionsComponent *	ic );

extern int docComponentEqualsWord(	const InstructionsComponent *	ic,
					const char *			word,
					int				len );

extern int docComponentEqualsWordNoCase( const InstructionsComponent *	ic,
					const char *			word,
					int				len );

extern void docListFieldInstructions(	int				indent,
					const FieldInstructions *	fi );

#   endif /*  DOC_FIELD_INSTRUCTIONS_H  */
