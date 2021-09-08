/*
 * _TIME.C
 *
 * DESCRIPTION
 *
 *   This module contains several timer functions. These timers are software
 *   timers controlled by the timer overflow handler. Each time a timer interrupt
 *   occurs the timers are decremented if they are unequal to zero.
 * 
 *
 *    timer_init       : reset all timers and enable timer overflow interrupt
 *    get_timer        : request for timer and mark timer as being used
 *    free_timer       : reset timer and make available
 *    start_timer      : start particular timer with value in milliseconds
 *    start_function_timer : Starts a timer wich will start a function when
 *                       the timer does elapse.
 *    timer_elapsed    : check whether a particular timer has elapsed
 *    reset_timer      : clear a timer ; but is still avaialble
 *    timer_value      : get current value of a particular timer
 *
 *
 *
 * REVISION DATE
 *
 *   09/04/08   HG: version for the ECO Switch, uses TPM2
 *
 *
 */

#include "stdbool.h"
#include "time.h"
#include "string.h"

#define	RESET_WDT  // __RESET_WATCHDOG() // CHANGE THIS <<======================================

// Private variables

static uint32_t timer_pool[MAX_TIMERS];
static uint8_t timer_alloc_table[MAX_TIMERS / 8];
static uint8_t timer_active_table[MAX_TIMERS / 8];
static void (*timer_func[MAX_TIMERS])(void);

// Private function prototypes

/*!
 * \brief This function opens the timer subsystem.
 *
 * \param -.
 *
 * \return TRUE if successful, FALSE on error.
 */
bool timer_open(void) {
	// Clear the timer allocation table
	memset(timer_alloc_table, 0, MAX_TIMERS / 8);

	// Clear the timer active table
	memset(timer_active_table, 0, MAX_TIMERS / 8);

	// Clear the timer function pointers
	memset(timer_func, 0, MAX_TIMERS);

	return 1;
}

/*!
 * \brief This function handles the timers.
 *
 * \param -.
 *
 * \return -.
 */
void timer_callback(void) {
	uint8_t timerid;

	// Handle the timers
	for (timerid = 0; timerid < MAX_TIMERS; timerid++) {
		if (timer_active_table[timerid / 8] & (1 << (timerid % 8))) {
			if (timer_pool[timerid])
				timer_pool[timerid]--;
			if ((timer_pool[timerid] == 1) && timer_func[timerid])
				timer_func[timerid]();
		}
	}
}

/*!
 * \brief This function allocates a timer.
 *
 * \param -.
 *
 * \return A timer id if successful, NO_TIMER on error.
 */
uint8_t timer_get(void) {
	uint8_t i, j, bit;
	uint8_t timerid = NO_TIMER;

	// Skip used bytes in the allocation table
	for (i = 0; i < (MAX_TIMERS / 8) && (timer_alloc_table[i] == 0xFF); i++)
		;

	if (i < (MAX_TIMERS / 8)) {
		// Skip used bits in the allocation table
		for (j = 0, bit = 1; j < 8 && (timer_alloc_table[i] & bit); j++, bit <<=
				1)
			;

		// Calculate the timer id
		timerid = (i * 8) + j;

		// Clear the timer value
		timer_pool[timerid] = 0;

		// Clear the timer callback function pointer
		timer_func[timerid] = 0;

		// Mark the timer as used in the allocation table
		timer_alloc_table[i] |= bit;
	}

	return (timerid);
}

/*!
 * \brief This function frees a timer.
 *
 * \param timerid The id of the timer to free.
 *
 * \return TRUE if successful, FALSE on error.
 */
bool timer_free(uint8_t *timerid) {
	if (*timerid < MAX_TIMERS) {
		// Mark timer as not active in the allocation table
		timer_active_table[*timerid / 8] &= ~(1 << (*timerid % 8));

		// Mark timer as free in the allocation table
		timer_alloc_table[*timerid / 8] &= ~(1 << (*timerid % 8));

		// Clear the timer value
		timer_pool[*timerid] = 0;

		// Clear the timer callback function pointer
		timer_func[*timerid] = 0;

		*timerid = NO_TIMER;
		return (true);
	}
	*timerid = NO_TIMER;
	return (false);
}

/*!
 * \brief This function starts a timer.
 *
 * \param timerid The id of the timer to start.
 * \param ms The time to wait in milliseconds.
 * \param p_fxn A function to be called when the timer is elapsed.
 *
 * \return TRUE if successful, FALSE on error.
 */
bool timer_start(uint8_t timerid, uint32_t value, void *p_fxn) {
	if (timerid < MAX_TIMERS) {
		// Set the timer value
		timer_pool[timerid] = value;

		// Set the timer callback function pointer
		timer_func[timerid] = (void (*)(void)) p_fxn;

		// Mark timer as active in the allocation table
		timer_active_table[timerid / 8] |= (1 << (timerid % 8));

		return (true);
	}

	return (false);
}

/*!
 * \brief This function stops a timer.
 *
 * \param timerid The id of the timer to stop.
 *
 * \return TRUE if successful, FALSE on error.
 */
bool timer_stop(uint8_t timerid) {
	if (timerid < MAX_TIMERS) {
		// Mark timer as not active in the allocation table
		timer_active_table[timerid / 8] &= ~(1 << (timerid % 8));

		return (true);
	}

	return (false);
}

/*!
 * \brief This function resume a timer.
 *
 * \param timerid The id of the timer to resume.
 *
 * \return TRUE if successful, FALSE on error.
 */
bool timer_resume(uint8_t timerid) {
	if (timerid < MAX_TIMERS) {
		// Mark timer as active in the allocation table
		timer_active_table[timerid / 8] |= (1 << (timerid % 8));

		return (true);
	}

	return (false);
}

/*!
 * \brief This function checks if a timer is elapsed.
 *
 * \param timerid The id of the timer to check.
 *
 * \return TRUE if the timer is elapsed, FALSE on error or if the timer is not elapsed.
 */
bool timer_elapsed(uint8_t timerid) {
	if (timerid < MAX_TIMERS) {
		return (timer_pool[timerid] == 0);
	}
	return (false);
}

