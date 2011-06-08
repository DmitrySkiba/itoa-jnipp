/*
 * Copyright (C) 2011 Dmitry Skiba
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _COMMON_INCLUDED_
#define _COMMON_INCLUDED_

#include <JNIpp.h>

/////////////////////////////////////////////////////////////////////

#define xTEST_MESSAGE_PREFIX TEST_NAME "> "

#define TEST_PRINTF(...) \
    PlatformPrintf(xTEST_MESSAGE_PREFIX __VA_ARGS__)

#define TEST_FAILED(message,...) \
    jni::FatalError(xTEST_MESSAGE_PREFIX "FAILED: " message "\n", ##__VA_ARGS__)

#define TEST_PASSED() \
    TEST_PRINTF("PASSED")

#define TEST_CHECK_FAIL(Condition,...) \
    if (Condition) TEST_FAILED(__VA_ARGS__)

/** Outputs formatted message in a platform-specific way.
 * Always terminates output with a new line.
 */
void PlatformPrintf(const char* message,...);

/////////////////////////////////////////////////////////////////////

#endif // _COMMON_INCLUDED_
