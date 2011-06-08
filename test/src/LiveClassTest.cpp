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
#include <unistd.h>

#define TEST_NAME "LiveClassTest"

const int JavaGCAttempts=5;
const int JavaGCSleepSeconds=1;

///////////////////////////////////////////////////////////////////// JavaGC

#define JB_CURRENT_CLASS System

JB_DEFINE_ACCESSOR(
    "java/lang/System"
    ,
    NoFields
    ,
    Methods
    (
        GC,
        "+gc","()V"
    )
)

void JavaGC() {
    JB_CALL_STATIC(VoidMethod,GC);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// LiveClass

class LiveClass;
typedef java::ObjectPointer<LiveClass> PLiveClass;

class LiveClassCaller;
typedef java::ObjectPointer<LiveClassCaller> PLiveClassCaller;

class LiveClassCounted;
typedef java::ObjectPointer<LiveClassCounted> PLiveClassCounted;

class LiveClass: public java::Object {
    JB_LIVE_CLASS(LiveClass);
public:
    LiveClass();
    virtual ~LiveClass();
    PLiveClassCaller GetCaller();
    static int32_t GetInstanceCount();
    static int32_t GetJavaInstanceCount();
    static void DestroyJavaInstance();
    static void CheckInstanceCount(int32_t expectedCount);
    static void CheckCallbackEvent(const char* callbackName);
public:    
    static const bool TestBool;
    static const jbyte TestByte;
    static const jchar TestChar;
    static const jshort TestShort;
    static const jint TestInt;
    static const jlong TestLong;
    static const jfloat TestFloat;
    static const jdouble TestDouble;
    static const char * const TestString;
private:
    void SimpleCallback();
    jlong PrimitiveCallback(bool z,jdouble d,jchar c,jbyte b,jshort s,jint i,jlong j,jfloat f);
    PLiveClassCounted ObjectCallback();
    void ExceptionCallback(java::PString message);
    static void NonStaticCallback(const jni::LObject& thiz,PLiveClass other);
    static void StaticSimpleCallback(java::PClass);
private:
    static int32_t m_instanceCount;
    static volatile bool m_destructorEvent;
    static bool m_callbackEvent;
};

class LiveClassCaller: public java::Object {
    JB_WRAPPER_CLASS(LiveClassCaller);
public:
    LiveClassCaller(const jni::LObject& object);
    void CallSimpleCallback();
    jlong CallPrimitiveCallback(bool z,jdouble d,jchar c,jbyte b,jshort s,jint i,jlong j,jfloat f);
    java::PString CallObjectCallback();
    java::PString CallExceptionCallback(java::PString message);
    void CallNonStaticCallback();
    static void CallStaticSimpleCallback();
};

class LiveClassCounted: public java::Object {
    JB_WRAPPER_CLASS(LiveClassCounted);
public:
    LiveClassCounted(const char* message);
    static int32_t GetInstanceCount();
    static void CheckNoInstances();
};

/////////////////////////////////////////////////

#define JB_CURRENT_CLASS LiveClass

JB_DEFINE_LIVE_CLASS(
    "com/itoa/jnipp/test/LiveClass"
    ,
    Fields
    (
        InstanceCount,
        "+instanceCount","I"
    )
    ,
    Methods
    (
        Constructor,
        "<init>","()V"
    )
    (
        GetCaller,
        "getCaller","()Lcom/itoa/jnipp/test/LiveClass$Caller;"
    )
    (
        ThrowException,
        "throwException","(Ljava/lang/String;)V"
    )
    ,
    Callbacks
    (
        Method(SimpleCallback),
        "simpleCallback","()V"
    )
    (
        Method(PrimitiveCallback),
        "primitiveCallback","(ZDCBSIJF)J"
    )
    (
        Method(ObjectCallback),
        "objectCallback","()Ljava/lang/Object;"
    )
    (
        Method(ExceptionCallback),
        "exceptionCallback","(Ljava/lang/String;)V"
    )
    (
        Method(NonStaticCallback),
        "nonStaticCallback","(Lcom/itoa/jnipp/test/LiveClass;)V"
    )
    (
        Method(StaticSimpleCallback),
        "staticSimpleCallback","()V"
    )
)

int32_t LiveClass::m_instanceCount=0;
volatile bool LiveClass::m_destructorEvent=false;
bool LiveClass::m_callbackEvent=false;

const bool LiveClass::TestBool=true;
const jbyte LiveClass::TestByte=0xFA;
const jchar LiveClass::TestChar=0x1022;
const jshort LiveClass::TestShort=0x551E;
const jint LiveClass::TestInt=0xFEFE1111;
const jlong LiveClass::TestLong=0xF0FFFFFAAFFFFF1FLL;
const jfloat LiveClass::TestFloat=232323.222e22;
const jdouble LiveClass::TestDouble=982.343e-159;
const char * const LiveClass::TestString="Raz, raz, raz.";

LiveClass::LiveClass():
    java::Object(JB_NEW(Constructor),GetInstanceFieldID())
{
    m_instanceCount++;
    
    m_callbackEvent=false;
    m_destructorEvent=false;
}

LiveClass::~LiveClass() {
    m_instanceCount--;
    m_destructorEvent=true;
}

PLiveClassCaller LiveClass::GetCaller() {
    return PLiveClassCaller::Wrap(JB_CALL_THIS(ObjectMethod,GetCaller));
}

int32_t LiveClass::GetInstanceCount() {
    return m_instanceCount;
}

int32_t LiveClass::GetJavaInstanceCount() {
    return JB_GET_STATIC(IntField,InstanceCount);
}

void LiveClass::DestroyJavaInstance() {
    m_destructorEvent=false;
    JavaGC();
    for (int i=0;i!=JavaGCAttempts && !m_destructorEvent;++i) {
        sleep(JavaGCSleepSeconds);
    }
    if (!m_destructorEvent) {
        TEST_FAILED("Didn't get destructor event in time.");
    }
    m_destructorEvent=false;
}

void LiveClass::CheckInstanceCount(int32_t expectedCount) {
    int32_t nativeCount=LiveClass::GetInstanceCount();
    if (nativeCount!=expectedCount) {
        TEST_FAILED("Native instance count (%d) doesn't match expected count (%d).",nativeCount,expectedCount);
    }
    int32_t javaCount=LiveClass::GetJavaInstanceCount();
    if (javaCount!=expectedCount) {
        TEST_FAILED("Java instance count (%d) doesn't match expected count (%d).",javaCount,expectedCount);
    }
    if (nativeCount!=javaCount) {
        TEST_FAILED("Java instance count (%d) doesn't match native count (%d).",javaCount,nativeCount);
    }
}

void LiveClass::CheckCallbackEvent(const char* callbackName) {
    if (!m_callbackEvent) {
        TEST_FAILED("Callback '%s' was not called!",callbackName);
    }
    m_callbackEvent=false;
}

/* Callbacks */

void LiveClass::SimpleCallback() {
    m_callbackEvent=true;
}

jlong LiveClass::PrimitiveCallback(bool z,jdouble d,jchar c,jbyte b,jshort s,jint i,jlong j,jfloat f) {
    m_callbackEvent=true;
    if (z!=TestBool || d!=TestDouble || c!=TestChar || b!=TestByte ||
        s!=TestShort || i!=TestInt || j!=TestLong || f!=TestFloat)
    {
        TEST_FAILED("Some of primitive values are invalid.");
    }
    return TestLong*2;
}

PLiveClassCounted LiveClass::ObjectCallback() {
    m_callbackEvent=true;
    return new LiveClassCounted(TestString);
}

void LiveClass::ExceptionCallback(java::PString message) {
    m_callbackEvent=true;
    JB_CALL_THIS(VoidMethod,ThrowException,message);
}

void LiveClass::NonStaticCallback(const jni::LObject& thiz,PLiveClass other) {
    TEST_CHECK_FAIL(
        !jni::IsSameObject(thiz,other),
        "Objects passed to NonStaticCallback don't match!");
    m_callbackEvent=true;
}

void LiveClass::StaticSimpleCallback(java::PClass clazz) {
    java::PString className=clazz->GetName();
    TEST_CHECK_FAIL(
        strcmp(className->GetUTF(),"com.itoa.jnipp.test.LiveClass"),
        "Invalid class in StaticSimpleCallback: %s",className->GetUTF());
    m_callbackEvent=true;
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////// LiveClassCaller

#define JB_CURRENT_CLASS LiveClassCaller

JB_DEFINE_WRAPPER_CLASS(
    "com/itoa/jnipp/test/LiveClass$Caller"
    ,
    NoFields
    ,
    Methods
    (
        CallSimpleCallback,
        "callSimpleCallback","()V"
    )
    (
        CallPrimitiveCallback,
        "callPrimitiveCallback","(ZDCBSIJF)J"
    )
    (
        CallObjectCallback,
        "callObjectCallback","()Ljava/lang/String;"
    )
    (
        CallExceptionCallback,
        "callExceptionCallback","(Ljava/lang/String;)Ljava/lang/String;"
    )
    (
        CallNonStaticCallback,
        "callNonStaticCallback","()V"
    )
    (
        CallStaticSimpleCallback,
        "+callStaticSimpleCallback","()V"
    )
)

LiveClassCaller::LiveClassCaller(const jni::LObject& object):
    java::Object(object)
{
}

void LiveClassCaller::CallSimpleCallback() {
    JB_CALL_THIS(VoidMethod,CallSimpleCallback);
}

jlong LiveClassCaller::CallPrimitiveCallback(bool z,jdouble d,jchar c,jbyte b,jshort s,jint i,jlong j,jfloat f) {
    return JB_CALL_THIS(LongMethod,CallPrimitiveCallback,z,d,c,b,s,i,j,f);
}

java::PString LiveClassCaller::CallObjectCallback() {
    return java::PString::Wrap(JB_CALL_THIS(ObjectMethod,CallObjectCallback));
}

java::PString LiveClassCaller::CallExceptionCallback(java::PString message) {
    return java::PString::Wrap(JB_CALL_THIS(ObjectMethod,CallExceptionCallback,message));
}

void LiveClassCaller::CallNonStaticCallback() {
    JB_CALL_THIS(VoidMethod,CallNonStaticCallback);
}

void LiveClassCaller::CallStaticSimpleCallback() {
    JB_CALL_STATIC(VoidMethod,CallStaticSimpleCallback);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////// LiveClassCounted

#define JB_CURRENT_CLASS LiveClassCounted

JB_DEFINE_WRAPPER_CLASS(
    "com/itoa/jnipp/test/LiveClass$Counted"
    ,
    Fields
    (
        InstanceCount,
        "+instanceCount","I"
    )
    ,
    Methods
    (
        Constructor,
        "<init>","(Ljava/lang/String;)V"
    )
)

LiveClassCounted::LiveClassCounted(const char* message):
    java::Object(JB_NEW(Constructor,java::PString::New(message)))
{
}

int32_t LiveClassCounted::GetInstanceCount() {
    return JB_GET_STATIC(IntField,InstanceCount);
}

void LiveClassCounted::CheckNoInstances() {
    int32_t count=0;
    for (int i=0;i!=JavaGCAttempts;++i) {
        JavaGC();
        sleep(JavaGCSleepSeconds);
        count=GetInstanceCount();
        if (!count) {
            break;
        }
    }
    if (count) {
        TEST_FAILED("Number of instances of Counted class is %d, not 0.",count);
    }
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// test

void RunLiveClassTest() {
    
    LiveClass::CheckInstanceCount(0);
    
    {
        PLiveClass test=new LiveClass();
        LiveClass::CheckInstanceCount(1);
    }
    LiveClass::CheckInstanceCount(1);
    LiveClass::DestroyJavaInstance();
    LiveClass::CheckInstanceCount(0);

    {
        PLiveClassCaller caller;
        {
            PLiveClass test=new LiveClass();
            caller=test->GetCaller();
        }
        
        caller->CallSimpleCallback();
        LiveClass::CheckCallbackEvent("simpleCallback");
        
        {
            jlong result=caller->CallPrimitiveCallback(
                LiveClass::TestBool,LiveClass::TestDouble,LiveClass::TestChar,
                LiveClass::TestByte,LiveClass::TestShort,LiveClass::TestInt,
                LiveClass::TestLong,LiveClass::TestFloat);
            LiveClass::CheckCallbackEvent("primitiveCallback");
            if (result!=LiveClass::TestLong*2) {
                TEST_FAILED("primitiveCallback returned wrong value %lld",result);
            }
        }
        
        {
            LiveClassCounted::CheckNoInstances();
            java::PString result=caller->CallObjectCallback();
            LiveClass::CheckCallbackEvent("objectCallback");
            if (strcmp(result->GetUTF(),LiveClass::TestString)) {
                TEST_FAILED("objectCallback returned wrong value %s.",result->GetUTF());
            }
            LiveClassCounted::CheckNoInstances();
        }
        
        {
            java::PString messageIn=java::PString::New("Zoooooooom!");
            java::PString messageOut=caller->CallExceptionCallback(messageIn);
            LiveClass::CheckCallbackEvent("exceptionCallback");
            if (strcmp(messageIn->GetUTF(),messageOut->GetUTF())) {
                TEST_FAILED("Wrong exception message '%s', must be '%s'.",
                    messageOut->GetUTF(),messageIn->GetUTF());
            }
        }
        
        {
            caller->CallNonStaticCallback();
            LiveClass::CheckCallbackEvent("CallNonStaticCallback");
        }
    }
    LiveClass::DestroyJavaInstance();
    LiveClass::CheckInstanceCount(0);
    
    LiveClassCaller::CallStaticSimpleCallback();
    LiveClass::CheckCallbackEvent("staticSimpleCallback");

    
    TEST_PASSED();    
}
