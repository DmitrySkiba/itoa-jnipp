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
#include <stdio.h>

#ifdef ANDROID
#    include <android/log.h>
#endif

BEGIN_NAMESPACE(jni)

///////////////////////////////////////////////////////////////////// helpers

///////////////////////////////////////////////// ThrowRuntimeException

#define JB_CURRENT_CLASS RuntimeException

JB_DEFINE_ACCESSOR(
    "java/lang/RuntimeException"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
        "<init>","(Ljava/lang/String;)V"
    )
)

static void ThrowRuntimeException(const char* message) {
    Throw(JB_NEW(Constructor,java::PString::New(message)));
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// LObject

LObject::LObject() {
    Construct(0,false);
}

LObject::LObject(const LObject& other) {
    Construct(other.GetJObject(),true);
}

LObject::LObject(jobject object,bool addReference) {
    Construct(object,addReference);
}

void LObject::Construct(jobject object,bool addReference) {
    m_object=object;
    if (m_object && addReference) {
        m_object=GetEnv()->NewLocalRef(m_object);
    }
}

LObject::~LObject() {
    if (m_object) {
        GetEnv()->DeleteLocalRef(m_object);
    }
}

void LObject::Swap(LObject& other) {
    std::swap(m_object,other.m_object);
}

jobject LObject::GetJObject() const {
    return m_object;
}

LObject& LObject::operator=(const LObject& other) {
    LObject(other).Swap(*this);
    return *this;
}

bool LObject::IsEmpty() const {
    return m_object==0;
}

bool LObject::operator!() const {
    return IsEmpty();
}

LObject::operator ComparasionHelper*() const {
    return (ComparasionHelper*)m_object;
}

LObject LObject::Wrap(jobject object) {
    return LObject(object,true);
}

LObject LObject::WrapLocal(jobject object) {
    return LObject(object,false);
}

///////////////////////////////////////////////////////////////////// NullObject

LObject NullObject;

///////////////////////////////////////////////////////////////////// basic functions

static JavaVM* g_javaVM=0;

void Initialize(JavaVM* vm) {
    if (!g_javaVM) {
        g_javaVM=vm;
    }
}

void Initialize(JNIEnv* env) {
    JavaVM* vm=0;
    int error=env->GetJavaVM(&vm);
    if (error) {
        FatalError("GetJavaVM failed with %d error.",error);
    }
    Initialize(vm);
}

static JNIEnv* GetEnv(bool fail) {
    if (!g_javaVM) {
        if (fail) {
            FatalError("jni:: is not initialized. "
                "Call jni::Initialize() before using jni:: functions.");
        } else {
            return 0;
        }
    }
    JNIEnv* env=0;
#ifdef JNIPP_ATTACHCURRENTTHREAD_WANTS_VOIDPP
    void* venv=0;
    jint result=g_javaVM->AttachCurrentThread(&venv,0);
    env=(JNIEnv*)venv;
#else
    // If compiler complains it can't convert &env to void**
    //  define JNIPP_ATTACHCURRENTTHREAD_WANTS_VOIDPP in project settings.
    jint result=g_javaVM->AttachCurrentThread(&env,0);
#endif // JNIPP_ATTACHCURRENTTHREAD_WANTS_VOIDPP
    if (result) {
        if (fail) {
            FatalError("AttachCurrentThread failed with %d error.",result);
        } else {
            return 0;
        }
    }
    return env;
}

JNIEnv* GetEnv() {
    return GetEnv(true);
}

void FatalError(const char* message,...) {
    const size_t MaxLength=1024;
    char formattedMessage[MaxLength+1];
    {
        va_list arguments;
        va_start(arguments,message);
        formattedMessage[MaxLength]=0;
        vsnprintf(formattedMessage,MaxLength,message,arguments);
        va_end(arguments);
    }

    // If JNIEnv is available, use FatalError() method.
    JNIEnv* env=GetEnv(false);
    if (env) {
        env->FatalError(formattedMessage);
        // Just in case FatalError() returns, proceed to the
        //  'log and die' method.
    }

    // If JNIEnv is not available, log and die.
#ifdef ANDROID
    __android_log_write(ANDROID_LOG_ERROR,"JNIpp",formattedMessage);
#else
    puts(formattedMessage);
#endif
    abort();
}

///////////////////////////////////////////////////////////////////// exceptions

void Throw(const AbstractObject& throwable) {
    jthrowable jThrowable=(jthrowable)throwable.GetJObject();
    int error=GetEnv()->Throw(jThrowable);
    if (error) {
        FatalError("Throw() failed with %d error.",error);
    }
}

void TranslateJavaException() {
    jobject exception=GetEnv()->ExceptionOccurred();
    if (!exception) {
        return;
    }
    LObject throwable=LObject::WrapLocal(exception);
    GetEnv()->ExceptionClear();
    throw java::PThrowable::Wrap(throwable);
}

void TranslateCppException() {
    try {
        throw;
    }
    catch (const AbstractObject& throwable) {
        Throw(throwable);
    }
    catch (const std::exception& exception) {
        ThrowRuntimeException(exception.what());
    }
    catch (...) {
        ThrowRuntimeException("Unknown C++ exception.");
    }
}

///////////////////////////////////////////////////////////////////// classes & objects

LObject FindClass(const char* className) {
    jclass clazz=GetEnv()->FindClass(className);
    TranslateJavaException();
    return LObject::WrapLocal(clazz);
}

LObject GetObjectClass(const AbstractObject& object) {
    jobject javaObject=object.GetJObject();
    return LObject::WrapLocal(GetEnv()->GetObjectClass(javaObject));
}

LObject GetSuperclass(const AbstractObject& clazz) {
    jclass jClazz=(jclass)clazz.GetJObject();
    return LObject::WrapLocal(GetEnv()->GetSuperclass(jClazz));
}

bool IsAssignableFrom(const AbstractObject& clazz,const AbstractObject& clazzFrom) {
    jclass jClazz=(jclass)clazz.GetJObject();
    jclass jClazzFrom=(jclass)clazzFrom.GetJObject();
    return GetEnv()->IsAssignableFrom(jClazz,jClazzFrom)==JNI_TRUE;
}

bool IsSameObject(const AbstractObject& object1,const AbstractObject& object2) {
    jobject jObject1=object1.GetJObject();
    jobject jObject2=object2.GetJObject();
    return GetEnv()->IsSameObject(jObject1,jObject2)==JNI_TRUE;
}

jmethodID GetMethodID(const AbstractObject& clazz,const char* name,const char* signature) {
    jclass jClazz=(jclass)clazz.GetJObject();
    jmethodID result=GetEnv()->GetMethodID(jClazz,name,signature);
    TranslateJavaException();
    return result;
}

jmethodID GetStaticMethodID(const AbstractObject& clazz,const char* name,const char* signature) {
    jclass jClazz=(jclass)clazz.GetJObject();
    jmethodID result=GetEnv()->GetStaticMethodID(jClazz,name,signature);
    TranslateJavaException();
    return result;
}

jfieldID GetFieldID(const AbstractObject& clazz,const char* name,const char* signature) {
    jclass jClazz=(jclass)clazz.GetJObject();
    jfieldID result=GetEnv()->GetFieldID(jClazz,name,signature);
    TranslateJavaException();
    return result;
}

jfieldID GetStaticFieldID(const AbstractObject& clazz,const char* name,const char* signature) {
    jclass jClazz=(jclass)clazz.GetJObject();
    jfieldID result=GetEnv()->GetStaticFieldID(jClazz,name,signature);
    TranslateJavaException();
    return result;
}

///////////////////////////////////////////////////////////////////// methods

/* The following two macros cast 'target' to jclass as a simple way to
 *  deal with static methods (which take jclass instead of jobject).
 */
#define xJNIPP_GENERATE_CALL_METHOD(ReturnType,Name,JNIFunction,ExtraArgslist,ExtraArgs) \
    ReturnType Name(const AbstractObject& target,ExtraArgslist jmethodID methodID,...) { \
        ReturnType result; \
        va_list arguments; \
        va_start(arguments,methodID); \
        result=_WrapJValue( \
            GetEnv()->JNIFunction##V((jclass)_GetJValue(target),ExtraArgs methodID,arguments) \
        ); \
        va_end(arguments); \
        TranslateJavaException(); \
        return result; \
    }
#define xJNIPP_GENERATE_CALL_VOID_METHOD(Name,JNIFunction,ExtraArgslist,ExtraArgs) \
    void Name(const AbstractObject& target,ExtraArgslist jmethodID methodID,...) { \
        va_list arguments; \
        va_start(arguments,methodID); \
        GetEnv()->JNIFunction##V((jclass)_GetJValue(target),ExtraArgs methodID,arguments); \
        va_end(arguments); \
        TranslateJavaException(); \
}

///////////////////////////////////////////////// CallXXX/CallStaticXXX

#define JNIPP_IMPLEMENT_CALL_METHOD(ReturnType,Name,JNIFunction) \
    xJNIPP_GENERATE_CALL_METHOD(ReturnType,Name,JNIFunction,,)

#define JNIPP_IMPLEMENT_CALL_VOID_METHOD(Name,JNIFunction) \
    xJNIPP_GENERATE_CALL_VOID_METHOD(Name,JNIFunction,,)

JNIPP_IMPLEMENT_CALL_METHOD(LObject,_NewObject,NewObject);

JNIPP_IMPLEMENT_CALL_METHOD(LObject,_CallObjectMethod,CallObjectMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jboolean,_CallBooleanMethod,CallBooleanMethod);
JNIPP_IMPLEMENT_CALL_METHOD(bool,_CallBoolMethod,CallBooleanMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jbyte,_CallByteMethod,CallByteMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jchar,_CallCharMethod,CallCharMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jshort,_CallShortMethod,CallShortMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jint,_CallIntMethod,CallIntMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jlong,_CallLongMethod,CallLongMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jfloat,_CallFloatMethod,CallFloatMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jdouble,_CallDoubleMethod,CallDoubleMethod);
JNIPP_IMPLEMENT_CALL_VOID_METHOD(_CallVoidMethod,CallVoidMethod);

JNIPP_IMPLEMENT_CALL_METHOD(LObject,_CallStaticObjectMethod,CallStaticObjectMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jboolean,_CallStaticBooleanMethod,CallStaticBooleanMethod);
JNIPP_IMPLEMENT_CALL_METHOD(bool,_CallStaticBoolMethod,CallStaticBooleanMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jbyte,_CallStaticByteMethod,CallStaticByteMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jchar,_CallStaticCharMethod,CallStaticCharMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jshort,_CallStaticShortMethod,CallStaticShortMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jint,_CallStaticIntMethod,CallStaticIntMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jlong,_CallStaticLongMethod,CallStaticLongMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jfloat,_CallStaticFloatMethod,CallStaticFloatMethod);
JNIPP_IMPLEMENT_CALL_METHOD(jdouble,_CallStaticDoubleMethod,CallStaticDoubleMethod);
JNIPP_IMPLEMENT_CALL_VOID_METHOD(_CallStaticVoidMethod,CallStaticVoidMethod);

///////////////////////////////////////////////// CallNonVirtualXXX

#define xJNIPP_CLASS_ARGLIST const AbstractObject& clazz,
#define xJNIPP_CLASS_ARG (jclass)clazz.GetJObject(),

#define JNIPP_IMPLEMENT_NVCALL_METHOD(ReturnType,Name,JNIFunction) \
    xJNIPP_GENERATE_CALL_METHOD(ReturnType,Name,JNIFunction, \
        xJNIPP_CLASS_ARGLIST,xJNIPP_CLASS_ARG)

#define JNIPP_IMPLEMENT_NVCALL_VOID_METHOD(Name,JNIFunction) \
    xJNIPP_GENERATE_CALL_VOID_METHOD(Name,JNIFunction, \
        xJNIPP_CLASS_ARGLIST,xJNIPP_CLASS_ARG)

#define JNIPP_IMPLEMENT_NVCALL_OBJECT_METHOD(Name,JNIFunction) \
    xJNIPP_GENERATE_CALL_OBJECT_METHOD(Name,JNIFunction, \
        xJNIPP_CLASS_ARGLIST,xJNIPP_CLASS_ARG)

JNIPP_IMPLEMENT_NVCALL_METHOD(LObject,_CallNonvirtualObjectMethod,CallNonvirtualObjectMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(jboolean,_CallNonvirtualBooleanMethod,CallNonvirtualBooleanMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(bool,_CallNonvirtualBoolMethod,CallNonvirtualBooleanMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(jbyte,_CallNonvirtualByteMethod,CallNonvirtualByteMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(jchar,_CallNonvirtualCharMethod,CallNonvirtualCharMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(jshort,_CallNonvirtualShortMethod,CallNonvirtualShortMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(jint,_CallNonvirtualIntMethod,CallNonvirtualIntMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(jlong,_CallNonvirtualLongMethod,CallNonvirtualLongMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(jfloat,_CallNonvirtualFloatMethod,CallNonvirtualFloatMethod);
JNIPP_IMPLEMENT_NVCALL_METHOD(jdouble,_CallNonvirtualDoubleMethod,CallNonvirtualDoubleMethod);
JNIPP_IMPLEMENT_NVCALL_VOID_METHOD(_CallNonvirtualVoidMethod,CallNonvirtualVoidMethod);


///////////////////////////////////////////////////////////////////// arrays

///////////////////////////////////////////////// object array

jsize GetArrayLength(const AbstractObject& array) {
    jarray jArray=(jarray)array.GetJObject();
    return GetEnv()->GetArrayLength(jArray);
}

LObject NewObjectArray(jsize length,const AbstractObject& elementClass) {
    return NewObjectArray(length,elementClass,LObject());
}

LObject NewObjectArray(jsize length,const AbstractObject& elementClass,const AbstractObject& initialElement) {
    jclass jElementClass=(jclass)elementClass.GetJObject();
    jobject jInitialElement=initialElement.GetJObject();
    jobject array=GetEnv()->NewObjectArray(length,jElementClass,jInitialElement);
    TranslateJavaException();
    return LObject::WrapLocal(array);
}

LObject GetObjectArrayElement(const AbstractObject& array,jsize index) {
    jobjectArray jArray=(jobjectArray)array.GetJObject();
    jobject object=GetEnv()->GetObjectArrayElement(jArray,index);
    TranslateJavaException();
    return LObject::WrapLocal(object);
}

void SetObjectArrayElement(const AbstractObject& array,jsize index,const AbstractObject& value) {
    jobjectArray jArray=(jobjectArray)array.GetJObject();
    jobject jValue=value.GetJObject();
    GetEnv()->SetObjectArrayElement(jArray,index,jValue);
    TranslateJavaException();
}

///////////////////////////////////////////////// bool[]

LObject NewBoolArray(jsize length) {
    jobject array=GetEnv()->NewBooleanArray(length);
    TranslateJavaException();
    return LObject::WrapLocal(array);
}

bool* GetBoolArrayElements(const AbstractObject& array,bool* isCopy) {
    jbooleanArray jArray=(jbooleanArray)array.GetJObject();
    jboolean jIsCopy=JNI_FALSE;
    bool* result=0;
    if (JBooleanIsBool) {
        jboolean* elements=GetEnv()->GetBooleanArrayElements(jArray,&jIsCopy);
        TranslateJavaException();
        result=(bool*)elements;
    } else {
        FatalError("jni::GetBoolArrayElements is not implemented yet.");
    }
    if (isCopy) {
        *isCopy=(jIsCopy==JNI_TRUE);
    }
    return result;
}

void ReleaseBoolArrayElements(const AbstractObject& array,bool* elements,jint mode) {
    jbooleanArray jArray=(jbooleanArray)array.GetJObject();
    if (JBooleanIsBool) {
        GetEnv()->ReleaseBooleanArrayElements(jArray,(jboolean*)elements,mode);
    } else {
        FatalError("jni::ReleaseBoolArrayElements is not implemented yet.");
    }
}

void GetBoolArrayRegion(const AbstractObject& array,jsize start,jsize length,bool* buffer) {
    jbooleanArray jArray=(jbooleanArray)array.GetJObject();
    if (JBooleanIsBool) {
        GetEnv()->GetBooleanArrayRegion(jArray,start,length,(jboolean*)buffer);
    } else {
        FatalError("jni::GetBoolArrayRegion is not implemented yet.");
    }
}

void SetBoolArrayRegion(const AbstractObject& array,jsize start,jsize length,const bool* buffer) {
    jbooleanArray jArray=(jbooleanArray)array.GetJObject();
    if (JBooleanIsBool) {
        GetEnv()->SetBooleanArrayRegion(jArray,start,length,(const jboolean*)buffer);
    } else {
        FatalError("jni::SetBoolArrayRegion is not implemented yet.");
    }
}

///////////////////////////////////////////////// primitive arrays

#define JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(Type,TypeName) \
    LObject New##TypeName##Array(jsize length) { \
        jobject array=GetEnv()->New##TypeName##Array(length); \
        TranslateJavaException(); \
        return LObject::WrapLocal(array); \
    } \
    Type* Get##TypeName##ArrayElements(const AbstractObject& array,bool* isCopy) { \
        Type##Array jArray=(Type##Array)array.GetJObject(); \
        jboolean jIsCopy=JNI_FALSE; \
        Type* elements=GetEnv()->Get##TypeName##ArrayElements(jArray,&jIsCopy); \
        TranslateJavaException(); \
        if (isCopy) { \
            *isCopy=(jIsCopy==JNI_TRUE); \
        } \
        return elements; \
    } \
    void Release##TypeName##ArrayElements(const AbstractObject& array,Type* elements,ArrayReleaseMode mode) { \
        Type##Array jArray=(Type##Array)array.GetJObject(); \
        GetEnv()->Release##TypeName##ArrayElements(jArray,elements,mode); \
    } \
    void Get##TypeName##ArrayRegion(const AbstractObject& array,jsize start,jsize length,Type* buffer) { \
        Type##Array jArray=(Type##Array)array.GetJObject(); \
        GetEnv()->Get##TypeName##ArrayRegion(jArray,start,length,buffer); \
        TranslateJavaException(); \
    } \
    void Set##TypeName##ArrayRegion(const AbstractObject& array,jsize start,jsize length,const Type* buffer) { \
        Type##Array jArray=(Type##Array)array.GetJObject(); \
        GetEnv()->Set##TypeName##ArrayRegion(jArray,start,length,buffer); \
        TranslateJavaException(); \
    }

JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(jboolean,Boolean)
JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(jbyte,Byte)
JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(jchar,Char)
JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(jshort,Short)
JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(jint,Int)
JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(jlong,Long)
JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(jfloat,Float)
JNIPP_IMPLEMENT_PRIMITIVE_ARRAY(jdouble,Double)

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(jni)
