/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "hal.h"
#include "ch_test.h"
#include "test_root.h"

/**
 * @page test_sequence_002 Semaphores
 *
 * File: @ref test_sequence_002.c
 *
 * <h2>Description</h2>
 * This sequence tests the ChibiOS/NIL functionalities related to
 * counter semaphores.
 *
 * <h2>Test Cases</h2>
 * - @subpage test_002_001
 * - @subpage test_002_002
 * - @subpage test_002_003
 * .
 */

/****************************************************************************
 * Shared code.
 ****************************************************************************/

#include "ch.h"

static semaphore_t sem1;

/****************************************************************************
 * Test cases.
 ****************************************************************************/

/**
 * @page test_002_001 Semaphore primitives, no state change
 *
 * <h2>Description</h2>
 * Wait, Signal and Reset primitives are tested. The testing thread
 * does not trigger a state change.
 *
 * <h2>Test Steps</h2>
 * - The function chSemWait() is invoked, after return the counter and
 *   the returned message are tested.
 * - The function chSemSignal() is invoked, after return the counter is
 *   tested.
 * - The function chSemReset() is invoked, after return the counter is
 *   tested.
 * .
 */

static void test_002_001_setup(void) {
  chSemObjectInit(&sem1, 1);
}

static void test_002_001_teardown(void) {
  chSemReset(&sem1, 0);
}

static void test_002_001_execute(void) {

  /* The function chSemWait() is invoked, after return the counter and
     the returned message are tested.*/
  test_set_step(1);
  {
    msg_t msg;

    msg = chSemWait(&sem1);
    test_assert_lock(chSemGetCounterI(&sem1) == 0, "wrong counter value");
    test_assert(MSG_OK == msg, "wrong returned message");
  }

  /* The function chSemSignal() is invoked, after return the counter is
     tested.*/
  test_set_step(2);
  {
    chSemSignal(&sem1);
    test_assert_lock(chSemGetCounterI(&sem1) == 1, "wrong counter value");
  }

  /* The function chSemReset() is invoked, after return the counter is
     tested.*/
  test_set_step(3);
  {
    chSemReset(&sem1, 2);
    test_assert_lock(chSemGetCounterI(&sem1) == 2, "wrong counter value");
  }
}

static const testcase_t test_002_001 = {
  "Semaphore primitives, no state change",
  test_002_001_setup,
  test_002_001_teardown,
  test_002_001_execute
};

/**
 * @page test_002_002 Semaphore primitives, with state change
 *
 * <h2>Description</h2>
 * Wait, Signal and Reset primitives are tested. The testing thread
 * triggers a state change.
 *
 * <h2>Test Steps</h2>
 * - The function chSemWait() is invoked, after return the counter and
 *   the returned message are tested. The semaphore is signaled by
 *   another thread.
 * - The function chSemWait() is invoked, after return the counter and
 *   the returned message are tested. The semaphore is reset by another
 *   thread.
 * .
 */

static void test_002_002_setup(void) {
  chSemObjectInit(&gsem1, 0);
}

static void test_002_002_teardown(void) {
  chSemReset(&gsem1, 0);
}

static void test_002_002_execute(void) {

  /* The function chSemWait() is invoked, after return the counter and
     the returned message are tested. The semaphore is signaled by
     another thread.*/
  test_set_step(1);
  {
    msg_t msg;

    msg = chSemWait(&gsem1);
    test_assert_lock(chSemGetCounterI(&gsem1) == 0, "wrong counter value");
    test_assert(MSG_OK == msg, "wrong returned message");
  }

  /* The function chSemWait() is invoked, after return the counter and
     the returned message are tested. The semaphore is reset by another
     thread.*/
  test_set_step(2);
  {
    msg_t msg;

    msg = chSemWait(&gsem2);
    test_assert_lock(chSemGetCounterI(&gsem2) == 0,"wrong counter value");
    test_assert(MSG_RESET == msg, "wrong returned message");
  }
}

static const testcase_t test_002_002 = {
  "Semaphore primitives, with state change",
  test_002_002_setup,
  test_002_002_teardown,
  test_002_002_execute
};

/**
 * @page test_002_003 Semaphores timeout
 *
 * <h2>Description</h2>
 * Timeout on semaphores is tested.
 *
 * <h2>Test Steps</h2>
 * - The function chSemWaitTimeout() is invoked a first time, after
 *   return the system time, the counter and the returned message are
 *   tested.
 * - The function chSemWaitTimeout() is invoked again, after return the
 *   system time, the counter and the returned message are tested.
 * .
 */

static void test_002_003_setup(void) {
  chSemObjectInit(&sem1, 0);
}

static void test_002_003_teardown(void) {
  chSemReset(&sem1, 0);
}

static void test_002_003_execute(void) {
  systime_t time;
  msg_t msg;

  /* The function chSemWaitTimeout() is invoked a first time, after
     return the system time, the counter and the returned message are
     tested.*/
  test_set_step(1);
  {
    time = chVTGetSystemTimeX();
    msg = chSemWaitTimeout(&sem1, MS2ST(1000));
    test_assert_time_window(time + MS2ST(1000),
                            time + MS2ST(1000) + 1,
                            "out of time window");
    test_assert_lock(chSemGetCounterI(&sem1) == 0, "wrong counter value");
    test_assert(MSG_TIMEOUT == msg, "wrong timeout message");
  }

  /* The function chSemWaitTimeout() is invoked again, after return the
     system time, the counter and the returned message are tested.*/
  test_set_step(2);
  {
    time = chVTGetSystemTimeX();
    msg = chSemWaitTimeout(&sem1, MS2ST(1000));
    test_assert_time_window(time + MS2ST(1000),
                            time + MS2ST(1000) + 1,
                            "out of time window");
    test_assert_lock(chSemGetCounterI(&sem1) == 0, "wrong counter value");
    test_assert(MSG_TIMEOUT == msg, "wrong timeout message");
  }
}

static const testcase_t test_002_003 = {
  "Semaphores timeout",
  test_002_003_setup,
  test_002_003_teardown,
  test_002_003_execute
};

/****************************************************************************
 * Exported data.
 ****************************************************************************/

/**
 * @brief   Semaphores.
 */
const testcase_t * const test_sequence_002[] = {
  &test_002_001,
  &test_002_002,
  &test_002_003,
  NULL
};