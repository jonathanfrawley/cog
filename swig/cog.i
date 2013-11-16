%module cog 
%{
#define SWIG_FILE_WITH_INIT
#include "cog_main.h"
%}
%rename("%(strip:[cog_])s") "";

void cog_init();
void cog_mainloop();
void cog_quit();
