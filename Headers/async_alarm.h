/*
 * BRLTTY - A background process providing access to the console screen (when in
 *          text mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 1995-2017 by The BRLTTY Developers.
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version. Please see the file LICENSE-GPL for details.
 *
 * Web Page: http://brltty.com/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

#ifndef BRLTTY_INCLUDED_ASYNC_ALARM
#define BRLTTY_INCLUDED_ASYNC_ALARM

#include "async.h"
#include "timing.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  const TimeValue *now;
  void *data;
} AsyncAlarmCallbackParameters;

#define ASYNC_ALARM_CALLBACK(name) void name (const AsyncAlarmCallbackParameters *parameters)
typedef ASYNC_ALARM_CALLBACK(AsyncAlarmCallback);

extern int asyncNewAbsoluteAlarm (
  AsyncHandle *handle,
  const TimeValue *time,
  AsyncAlarmCallback *callback,
  void *data
);

extern int asyncNewRelativeAlarm (
  AsyncHandle *handle,
  int milliseconds,
  AsyncAlarmCallback *callback,
  void *data
);

extern int asyncResetAlarmTo (AsyncHandle handle, const TimeValue *time);
extern int asyncResetAlarmIn (AsyncHandle handle, int milliseconds);
extern int asyncResetAlarmEvery (AsyncHandle handle, int milliseconds);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BRLTTY_INCLUDED_ASYNC_ALARM */
