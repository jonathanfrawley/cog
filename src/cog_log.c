#include "cog_log.h"

#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#ifdef __ANDROID__
#include <android/log.h>
#endif

void cog_errorf(const char* log_msg, ...) {
    va_list ap;
    va_start(ap, log_msg);
    char buf[COG_MAX_BUF];
    vsprintf(buf, log_msg, ap);
    printf("CRITICAL: %s \n", buf);
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_ERROR, "cog", buf);
#endif
    assert(0);

}

void cog_debugf(const char* log_msg, ...) {
    va_list ap;
    va_start(ap, log_msg);
    char buf[COG_MAX_BUF];
    vsprintf(buf, log_msg, ap);
    printf("DEBUG: %s \n", buf);
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_DEBUG, "cog", buf);
#endif
}

void cog_infof(const char* log_msg, ...) {
    va_list ap;
    va_start(ap, log_msg);
    char buf[COG_MAX_BUF];
    vsprintf(buf, log_msg, ap);
    printf("INFO: %s \n", buf);
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_INFO, "cog", buf);
#endif
}
