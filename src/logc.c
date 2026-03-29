#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <stdbool.h>
#include <syslog.h>
#include <stdatomic.h>
#include "stdext/logc.h"

// TODO: add mutex for thread safety?
#define MODULE_NAME_MAX 32
static logc_target target = LOGC_TARGET_STDOUT;
static atomic_bool log_suppress = false;
static char module[MODULE_NAME_MAX] = "unknown";
static atomic_bool opened = false;
static atomic_int log_level = LOG_INFO;
static atomic_bool log_color = true;

static char *log_pri_txt[] = {
	[LOG_EMERG] = "EMERGENCY",
	[LOG_ALERT] = "ALERT    ",
	[LOG_CRIT] = "CRITICAL",
	[LOG_ERR] = "ERROR",
	[LOG_WARNING] = "WARNING",
	[LOG_NOTICE] = "NOTICE",
	[LOG_INFO] = "INFO",
	[LOG_DEBUG] = "DEBUG",
};

static char *log_pri_color[] = {
	[LOG_EMERG] = LOGC_COL_REDHB,
	[LOG_ALERT] = LOGC_COL_YELHB,
	[LOG_CRIT] = LOGC_COL_HRED,
	[LOG_ERR] = LOGC_COL_RED,
	[LOG_WARNING] = LOGC_COL_YEL,
	[LOG_NOTICE] = LOGC_COL_GRN,
	[LOG_INFO] = LOGC_COL_BLU,
	[LOG_DEBUG] = LOGC_COL_GRN,
};

void logc_level_set(int level)
{
	log_level = level;
}

int logc_level_get()
{
	return log_level;
}


void logc_suppress(int enable_suppress)
{
	log_suppress = enable_suppress;
}

void logc_color_enable(bool enable)
{
	log_color = enable;
}

void logc_target_set(logc_target target_in)
{
	if(!opened && target_in == LOGC_TARGET_SYSLOG) {
		openlog("", 0, LOG_USER);
		opened = true;
	}
	target = target_in;
}

void logc_open(const char *name)
{
	strncpy(module, name, sizeof(module) - 1);
	if(!opened && target == LOGC_TARGET_SYSLOG) {
		opened = true;
		openlog(module, 0, LOG_USER);
	}
}

void logc_close()
{
	if(opened) {
		closelog();
		opened = false;
	}
}


static char *priority_to_color(int pri)
{
	if(pri < 0 || pri > LOG_DEBUG)
		pri = 0;
	return log_pri_color[pri];
}

static char *priority_to_str(int pri)
{
	if(pri < 0 || pri > LOG_DEBUG)
		pri = 0;
	return log_pri_txt[pri];
}
static void logc_va_list(int pri, const char *fmt, va_list args)
{
	if(target == LOGC_TARGET_SYSLOG) {
		syslog(pri, fmt, args);
	}
	else {
		if(log_color) {
			printf("%s%s " LOGC_COL_RESET, priority_to_color(pri), priority_to_str(pri));
		}
		else {
			printf("%s ", priority_to_str(pri));
		}
		vprintf(fmt, args);
		puts("");
	}
	
}

void logclvl(int pri, int lvl, const char *fmt, ...)
{
	va_list args;
	if(log_suppress || pri > lvl) {
		return;
	}	
	va_start(args, fmt);
	logc_va_list(pri, fmt, args);
	va_end(args);
}

void logc(int pri, const char *fmt, ...)
{
	va_list args;
	if(log_suppress || pri > log_level) {
		return;
	}	
	va_start(args, fmt);
	logc_va_list(pri, fmt, args);
	va_end(args);
}
