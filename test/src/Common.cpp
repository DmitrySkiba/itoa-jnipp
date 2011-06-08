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

#include "Common.h"
#include "stdio.h"

#ifdef ANDROID
#   include <android/log.h>
#endif

/////////////////////////////////////////////////////////////////////

void PlatformPrintf(const char* message,...) {
    va_list arguments;
    va_start(arguments,message);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_INFO,"JNIpp",message,arguments);
#else
    vprintf(message,arguments);
    puts("");
#endif // ANDROID
    va_end(arguments);
}

/////////////////////////////////////////////////////////////////////
