/*
 * SPDX-FileCopyrightText: <year> <author>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/* clib includes */

#include "embUnit/embUnit.h"


#include "tests_event_processing.h"

/* your macros */
#define PHASE_CLOSING true
#define PHASE_OPENING true
/* your global variables */

// static void set_up(void)
// {
//     /* omit if not needed */
// }

// static void tear_down(void)
// {
//     /* omit if not needed */
// }


void tests_event_processing(void)
{
    TESTS_RUN(tests_event_processing_seq_verification_tests());
}
