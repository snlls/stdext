#pragma once
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>

#define LOGC_MODULE_NAME_MAX 32


struct logc_state;
typedef struct logc_state logc_state;

typedef enum {
	LOGC_TARGET_STDOUT = (1 << 0),
	LOGC_TARGET_SYSLOG = (1 << 1),
	LOGC_TARGET_FILE = (1 << 2)
} logc_target;


#define	LOGC_LVL_EMERG		0	/* system is unusable */
#define	LOGC_LVL_ALERT		1	/* action must be taken immediately */
#define	LOGC_LVL_CRIT		2	/* critical conditions */
#define	LOGC_LVL_ERR		3	/* error conditions */
#define	LOGC_LVL_WARNING	4	/* warning conditions */
#define	LOGC_LVL_NOTICE		5	/* normal but significant condition */
#define	LOGC_LVL_INFO		6	/* informational */
#define	LOGC_LVL_DEBUG		7	/* debug-level messages */
#define	LOGC_LVL_COUNT		8

struct logc_state {
	logc_target target;
	atomic_bool suppress;
	char module[LOGC_MODULE_NAME_MAX];
	atomic_bool opened;
	atomic_int level;
	atomic_bool color;
};



/** @param level LOG_EMERG <= level <= LOG_DEBUG */
void logc_level_set(logc_state *state, int level);

/** @brief set log suppress, by default logging is enabled */
void logc_suppress(logc_state *state, int enable_suppress);

/** @brief set logging target, by default log to stdout */
void logc_target_set(logc_state *state, logc_target target_mask);


/** @brief enable logging colors, by default they are on */
void logc_color_enable(logc_state *state, bool enable);

int logc_level_get(logc_state *state);

/** @brief utility function for logging
 * @param pri priority for syslog.h, LOG_*
 */
void logc(logc_state *state, int pri, const char *fmt, ...);
void logc_close(logc_state *state);
void logc_open(logc_state *state, char *name, logc_target target, int log_level);

// terminal ansi colors

//Regular text
#define LOGC_COL_BLK "\e[0;30m"
#define LOGC_COL_RED "\e[0;31m"
#define LOGC_COL_GRN "\e[0;32m"
#define LOGC_COL_YEL "\e[0;33m"
#define LOGC_COL_BLU "\e[0;34m"
#define LOGC_COL_MAG "\e[0;35m"
#define LOGC_COL_CYN "\e[0;36m"
#define LOGC_COL_WHT "\e[0;37m"

//Regular bold text
#define LOGC_COL_BBLK "\e[1;30m"
#define LOGC_COL_BRED "\e[1;31m"
#define LOGC_COL_BGRN "\e[1;32m"
#define LOGC_COL_BYEL "\e[1;33m"
#define LOGC_COL_BBLU "\e[1;34m"
#define LOGC_COL_BMAG "\e[1;35m"
#define LOGC_COL_BCYN "\e[1;36m"
#define LOGC_COL_BWHT "\e[1;37m"

//Regular underline text
#define LOGC_COL_UBLK "\e[4;30m"
#define LOGC_COL_URED "\e[4;31m"
#define LOGC_COL_UGRN "\e[4;32m"
#define LOGC_COL_UYEL "\e[4;33m"
#define LOGC_COL_UBLU "\e[4;34m"
#define LOGC_COL_UMAG "\e[4;35m"
#define LOGC_COL_UCYN "\e[4;36m"
#define LOGC_COL_UWHT "\e[4;37m"

//Regular background
#define LOGC_COL_BLKB "\e[40m"
#define LOGC_COL_REDB "\e[41m"
#define LOGC_COL_GRNB "\e[42m"
#define LOGC_COL_YELB "\e[43m"
#define LOGC_COL_BLUB "\e[44m"
#define LOGC_COL_MAGB "\e[45m"
#define LOGC_COL_CYNB "\e[46m"
#define LOGC_COL_WHTB "\e[47m"

//High intensty background 
#define LOGC_COL_BLKHB "\e[0;100m"
#define LOGC_COL_REDHB "\e[0;101m"
#define LOGC_COL_GRNHB "\e[0;102m"
#define LOGC_COL_YELHB "\e[0;103m"
#define LOGC_COL_BLUHB "\e[0;104m"
#define LOGC_COL_MAGHB "\e[0;105m"
#define LOGC_COL_CYNHB "\e[0;106m"
#define LOGC_COL_WHTHB "\e[0;107m"

//High intensty text
#define LOGC_COL_HBLK "\e[0;90m"
#define LOGC_COL_HRED "\e[0;91m"
#define LOGC_COL_HGRN "\e[0;92m"
#define LOGC_COL_HYEL "\e[0;93m"
#define LOGC_COL_HBLU "\e[0;94m"
#define LOGC_COL_HMAG "\e[0;95m"
#define LOGC_COL_HCYN "\e[0;96m"
#define LOGC_COL_HWHT "\e[0;97m"

//Bold high intensity text
#define LOGC_COL_BHBLK "\e[1;90m"
#define LOGC_COL_BHRED "\e[1;91m"
#define LOGC_COL_BHGRN "\e[1;92m"
#define LOGC_COL_BHYEL "\e[1;93m"
#define LOGC_COL_BHBLU "\e[1;94m"
#define LOGC_COL_BHMAG "\e[1;95m"
#define LOGC_COL_BHCYN "\e[1;96m"
#define LOGC_COL_BHWHT "\e[1;97m"

#define LOGC_COL_RESET "\e[0m"
