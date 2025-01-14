#pragma once

#include <openssl/trace.h>


void start_tracing();

/**
 * Print string if this level is smaller or equal to global level
 * @param string String to be printed
 * @param this_level This level
 * @param global_level Global level
 */
void cb_printf(const char *string);
