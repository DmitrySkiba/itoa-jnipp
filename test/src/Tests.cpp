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

#include "Tests.h"
#include "Common.h"

#define TEST_NAME "AllTests"

/////////////////////////////////////////////////////////////////////

void RunMethodTest();
void RunFieldsTest();
void RunLiveClassTest();
void RunCastsTest();
void RunArrayTest();

extern "C" void Java_com_itoa_jnipp_test_Tests_run(JNIEnv* env,jclass) {
    jni::Initialize(env);

    try {
        RunArrayTest();
        RunMethodTest();
        RunFieldsTest();
        RunLiveClassTest();
        RunCastsTest();

        TEST_PRINTF("Done");
    }
    catch (const jni::AbstractObject& exception) {
        TEST_FAILED("Exception: %s\n",java::Cast<java::Object>(exception)->ToString()->GetUTF());
    }
    catch (const std::exception& exception) {
        TEST_FAILED("std::exception: %s",exception.what());
    }
    catch (...) {
        TEST_FAILED("Unknown (...) exception.");
    }
}
