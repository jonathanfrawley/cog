#ifndef  COG_LOG_H
#define  COG_LOG_H

#include "cog_types.h"

#define COG_MAX_BUF 255

//#logging
void cog_errorf(const char* logMsg, ...);
void cog_debugf(const char* logMsg, ...);
void cog_infof(const char* logMsg, ...);

#endif // COG_LOG_H
