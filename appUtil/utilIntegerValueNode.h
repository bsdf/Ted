/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of structures with integer values	*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_INTEGER_VALUE_NODE_H
#   define	UTIL_INTEGER_VALUE_NODE_H

typedef struct IntegerValueNode
    {
    int					ivnValue;
    unsigned int			ivnIsLeaf:1;
    unsigned int			ivnChildCount:31;

    union
	{
	struct IntegerValueNode *	u_ivnChildren;
	int				u_ivnReference;
	} ivn_u;
#   define ivnChildren	ivn_u.u_ivnChildren
#   define ivnReference	ivn_u.u_ivnReference

    } IntegerValueNode;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitIntegerValueNode(	IntegerValueNode *	tal );

extern void utilCleanIntegerValueNode(	IntegerValueNode *	tal );

extern IntegerValueNode * utilChildIntegerValueNode(
					IntegerValueNode *	ivn,
					int			make,
					int			propval );

#   endif	/*  UTIL_INTEGER_VALUE_NODE_H	*/
