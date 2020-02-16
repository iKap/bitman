#ifndef __PRINT_H__
#define __PRINT_H__

#include "global.h"
#include "types.h"

void PrintUsage();
void PrintoutValue(__INT64 Value);
int  PrintoutHeader(PLISTCNTRBLOCK pLCB);
void PrintoutFooter(int Length);
void PrintoutItem(PItem pItem, PLISTCNTRBLOCK pLCB);

#endif //__PRINT_H__
