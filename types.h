
#ifndef __TYPES_H__
#define __TYPES_H__

#define FALSE 0
#define TRUE  1

typedef unsigned char BOOL;

#ifdef _WIN32
typedef int/*__int64*/ __INT64;
#define ATOI(x)  _atoi64(x)
#else
typedef long long __INT64;
#define ATOI(x)    atoll(x)
#endif

#ifdef _WIN32
#define LONG_PR	 "I64"
#else
#define LONG_PR  "ll"
#endif

typedef struct tagItem
{
	struct tagItem   *pNext;       // Next Item in the list
	BOOL             isValid;      // is parameter valid ?
	BOOL             isInterva;    // is Interval or single Bit field
	BOOL             isName;       // is Name of Bit field or Interval exist
	BOOL             isAssignment; // is Assignment value exist ?

	int              From, To;     // Interval, even if single bit is requested

	char             *pName;       // Name of Bit field or Interval
	int              NameLength;   // Length of Name

	char             sValues[100]; // String of ItemValues and AssValues
	int              ValuesLength; // Length of sValues;

	char             sMask[100];   // Mask string
	int              MaskLength;   // Length of sMask string

	__INT64          ItemValue;    // Interval Value or One Bit Value
	__INT64          AssValue;     // Value to assign in place of bit mask.
	__INT64          Mask;         // Mask
}TItem, *PItem;

typedef struct tagListCntrBlock
{
	PItem pHead;
	PItem pCurr;
	__INT64 MainValue;			// Input value
	__INT64 ResultValue;		// Output value
	BOOL  isGlobalAssignment;   // is Assignment value exist ?
	int   MaxNameLength ;
	int   MaxValuesLength;
	int   MaxMaskLength;
	
} TLISTCNTRBLOCK, *PLISTCNTRBLOCK;

#endif //__TYPES_H__

