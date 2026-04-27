#include "stdext/logc.h"
#include <stdio.h>


int log_test() {
	// Test 1: Basic initialization and logging
	{
		logc_state state = {0};
		logc_open(&state, "test_module", LOGC_TARGET_STDOUT, LOGC_LVL_DEBUG);

		logc(&state, LOGC_LVL_INFO, "Test message: %s", "Hello World!");
		logc_close(&state);
	}

	// Test 2: Level setting and suppression
	{
		logc_state state = {0};
		logc_open(&state, "test_module", LOGC_TARGET_STDOUT, LOGC_LVL_WARNING);

		logc(&state, LOGC_LVL_INFO, "This should not appear (level too low)");
		logc(&state, LOGC_LVL_WARNING, "This should appear");

		logc_level_set(&state, LOGC_LVL_DEBUG);
		logc(&state, LOGC_LVL_INFO, "Now this should appear (level changed)");

		logc_suppress(&state, true);
		logc(&state, LOGC_LVL_CRIT, "This should not appear (suppressed)");
		logc_suppress(&state, false);
		logc(&state, LOGC_LVL_CRIT, "This should appear again");

		logc_close(&state);
	}

	// Test 3: Target switching
	{
		logc_state state = {0};
		logc_open(&state, "test_module", LOGC_TARGET_STDOUT, LOGC_LVL_INFO);

		logc_target_set(&state, LOGC_TARGET_FILE);
		logc(&state, LOGC_LVL_INFO, "This should go to file (if implemented)");

		logc_target_set(&state, LOGC_TARGET_STDOUT);
		logc(&state, LOGC_LVL_INFO, "Back to stdout");

		logc_close(&state);
	}

	// Test 4: Color toggle
	{
		logc_state state = {0};
		logc_open(&state, "test_module", LOGC_TARGET_STDOUT, LOGC_LVL_INFO);

		logc_color_enable(&state, true);
		logc(&state, LOGC_LVL_INFO, "Colored output (if supported)");

		logc_color_enable(&state, false);
		logc(&state, LOGC_LVL_INFO, "Plain output");

		logc_close(&state);
	}

	// Test 5: Level get
	{
		logc_state state = {0};
		logc_open(&state, "test_module", LOGC_TARGET_STDOUT, LOGC_LVL_ERR);

		int level = logc_level_get(&state);
		printf("Current log level: %d\n", level);

		logc_level_set(&state, LOGC_LVL_DEBUG);
		level = logc_level_get(&state);
		printf("Updated log level: %d\n", level);

		logc_close(&state);
	}

	printf("All tests completed.\n");
	return 0;
}
