#include "./settings.h"

// global variable definition cannot be in header file bc it would cause multiple definitions 
// of g_Settings when header is included in multiple .cpp files
Settings_t g_Settings;