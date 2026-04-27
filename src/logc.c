#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <syslog.h>
#include "stdext/logc.h"



void logc_open(logc_state *state, char *name, logc_target target, int log_level)
{
	memset(state, 0, sizeof(*state));
	state->target = target;
	state->suppress = false;
	strncpy(state->module, name, sizeof(state->module) - 1);
	state->opened = true;
	state->level = log_level;
	state->color = true;
	if(target & LOGC_TARGET_SYSLOG) {
		openlog(state->module, 0, LOG_USER);
	}
}



static char *log_pri_txt[] = {
	[LOG_EMERG] = "EMGR",
	[LOG_ALERT] = "ALRT",
	[LOG_CRIT] = "CRIT",
	[LOG_ERR] = "ERR ",
	[LOG_WARNING] = "WARN",
	[LOG_NOTICE] = "NTIC",
	[LOG_INFO] = "INFO",
	[LOG_DEBUG] = "DBUG",
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

void logc_level_set(logc_state *state, int level)
{
	state->level = level;
}

int logc_level_get(logc_state *state)
{
	return state->level;
}


void logc_suppress(logc_state *state, int enable_suppress)
{
	state->suppress = enable_suppress;
}

void logc_color_enable(logc_state *state, bool enable)
{
	state->color = enable;
}

void logc_target_set(logc_state *state, logc_target target_in)
{
	if(target_in == state->target)
		return;
	
	if(state->opened) {
		bool hassys = state->target & LOGC_TARGET_SYSLOG;
		bool addingsys = target_in & LOGC_TARGET_SYSLOG;
		if(hassys && !addingsys) {
			closelog();
		}
		if(addingsys && !hassys) {
			openlog(state->module, 0, LOG_USER);
		}
	}
	state->target = target_in;
}


void logc_close(logc_state *state)
{
	if(state->opened && state->target == LOGC_TARGET_SYSLOG) {
		closelog();
	}
	state->opened = false;
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

static void logc_va_list(logc_state *state, int pri, const char *fmt, va_list args)
{
	if(state->target == LOGC_TARGET_SYSLOG) {
		syslog(pri, fmt, args);
	}
	else {
		if(state->color) {
			printf("%s%s " LOGC_COL_RESET, priority_to_color(pri), priority_to_str(pri));
		}
		else {
			printf("%s ", priority_to_str(pri));
		}
		vprintf(fmt, args);
		puts("");
	}
	
}

void logc(logc_state *state, int level, const char *fmt, ...)
{
	va_list args;
	if(state->suppress || level > state->level) {
		return;
	}	
	va_start(args, fmt);
	logc_va_list(state, level, fmt, args);
	va_end(args);
}
 
