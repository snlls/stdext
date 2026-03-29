#include <ccpsx/logc.h>

int log_test()
{
	logc_open("tests", LOG_TARGET_STDOUT);
	logc_color_enable(false);
	logc(LOG_ERR, "log: no colors");
	logc_color_enable(true);
	logc_level_set(LOG_DEBUG);
	logc(LOG_EMERG, "log: emergency");
	logc(LOG_ALERT, "log: alert");
	logc(LOG_CRIT, "log: critical");
	logc(LOG_ERR, "log: error");
	logc(LOG_WARNING, "log: warning");
	logc(LOG_INFO, "log: info");
	logc(LOG_DEBUG, "log: debug");
	logc_close();
	int errors = 0;
	return errors;
}
