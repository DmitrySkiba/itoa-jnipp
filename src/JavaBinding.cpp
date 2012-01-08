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

#include "JNIpp.h"

BEGIN_NAMESPACE(jb)

///////////////////////////////////////////////////////////////////// implementation

#define COMMON_HINTS \
    " [Hints: fully qualify all types, put + in front of static entity names," \
    " don't forget ; at the end of class names]"

static bool CheckClearException(bool describe=true) {
    jobject exception=jni::GetEnv()->ExceptionOccurred();
    if (exception) {
        if (describe) {
            jni::GetEnv()->ExceptionDescribe();
        }
        jni::GetEnv()->DeleteLocalRef(exception);
        jni::GetEnv()->ExceptionClear();
        return true;
    } else {
        return false;
    }
}

/* jni::GetField/MethodId can throw an exception. However, the concrete
 *  exception type depends on current exception handler which may not
 *  even throw at all. So we have to use raw methods and check for
 *  java exceptions. All exceptions are qualified as 'not found' errors.
 */
void InitClassDescriptor(ClassDescriptor& descriptor) {
    if (CheckClearException()) {
        jni::FatalError(
            "InitClassDescriptor(%s) called with java exception pending!",
            descriptor.className);
    }
    jni::LObject clazz=jni::FindClass(descriptor.className);
    if (!clazz) {
        jni::FatalError("Can't find class %s.",descriptor.className);
    }
    jclass jClazz=(jclass)clazz.GetJObject();
    if (descriptor.methods) {
        MethodDescriptor* method=descriptor.methods;
        for (;method->name;++method) {
            const char* name=method->name;
            if (*name=='+') {
                name++;
                method->id=jni::GetEnv()->GetStaticMethodID(jClazz,name,method->signature);
            } else {
                method->id=jni::GetEnv()->GetMethodID(jClazz,name,method->signature);
            }
            if (!method->id || CheckClearException()) {
                jni::FatalError(
                    "Can't find method %s%s in class %s." COMMON_HINTS,
                    method->name,method->signature,descriptor.className);
            }
        }
    }
    if (descriptor.fields) {
        FieldDescriptor* field=descriptor.fields;
        for (;field->name;++field) {
            const char* name=field->name;
            if (*name=='+') {
                name++;
                field->id=jni::GetEnv()->GetStaticFieldID(jClazz,name,field->signature);
            } else {
                field->id=jni::GetEnv()->GetFieldID(jClazz,name,field->signature);
            }
            if (!field->id || CheckClearException()) {
                jni::FatalError(
                    "Can't find field %s%s in class %s." COMMON_HINTS,
                    field->name,field->signature,descriptor.className);
            }
        }
    }
    if (descriptor.callbacks) {
        const JNINativeMethod* last=descriptor.callbacks;
        while (last->name) {
            ++last;
        }
        jint error=jni::GetEnv()->RegisterNatives(
            jClazz,
            descriptor.callbacks,last-descriptor.callbacks);
        if (CheckClearException()) {
            jni::FatalError("Java exception occurred while registering callbacks.");
        }
        if (error) {
            jni::FatalError("Error registering callbacks (%d).",error);
        }
    }
    if (descriptor.isLive) {
        jni::LObject superclass=jni::GetSuperclass(clazz);
        if (superclass) {
            jmethodID finalizer=jni::GetEnv()->GetMethodID(
                (jclass)superclass.GetJObject(),
                "finalize","()V");
            if (!CheckClearException(false)) {
                descriptor.superFinalizer=finalizer;
            }
        }
    }

    descriptor.clazz=new java::Class(clazz);
    descriptor.clazz->Retain();
}

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(jb)
