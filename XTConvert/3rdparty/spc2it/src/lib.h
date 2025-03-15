/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#ifndef LIB_H
#define LIB_H

int spc2it_load(const char* src, int ITrows, int limit, int verbose);
int spc2it_save(const char* dest, int verbose);
void spc2it_cleanup();

#endif
