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

#include "array_deleter.h"
#include "JNIpp.h"

BEGIN_NAMESPACE(java)

///////////////////////////////////////////////////////////////////// Class

#define JB_CURRENT_CLASS Class

JB_DEFINE_WRAPPER_CLASS(
    "java/lang/Class"
    ,
    NoFields
    ,
    Methods
    (
        GetName,
        "getName",
        "()Ljava/lang/String;"
    )
    (
        ForName,
        "+forName",
        "(Ljava/lang/String;)Ljava/lang/Class;"
    )
)

Class::Class(const jni::LObject& clazz):
    java::Object(clazz)
{
}

PString Class::GetName() const {
    return PString::Wrap(JB_CALL_THIS(ObjectMethod,GetName));
}

PClass Class::ForName(const char* name) {
    return ForName(PString::New(name));
}

PClass Class::ForName(PString name) {
    return PClass::Wrap(JB_CALL_STATIC(ObjectMethod,ForName,name));
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// CharSequence

#define JB_CURRENT_CLASS CharSequence

JB_DEFINE_WRAPPER_CLASS(
    "java/lang/CharSequence"
    ,
    NoFields
    ,
    Methods
    (
        CharAt,
         "charAt",
         "()C"
    )
    (
        GetLength,
         "length",
         "()I"
    )
    (
        GetSubSequence,
         "subSequence",
         "(II)Ljava/lang/CharSequence;"
    )
)

CharSequence::CharSequence(const jni::LObject& charSequence):
    java::Object(charSequence)
{
}

CharSequence::CharSequence(const jni::LObject& object,jfieldID instanceFieldID):
    java::Object(object,instanceFieldID)
{
}

jchar CharSequence::CharAt(jint index) const {
    return JB_CALL_THIS(CharMethod,CharAt,index);
}

jint CharSequence::GetLength() const {
     return JB_CALL_THIS(IntMethod,GetLength);
}

PCharSequence CharSequence::GetSubSequence(jint start,jint end) const {
    return PCharSequence::Wrap(JB_CALL_THIS(ObjectMethod,GetSubSequence,start,end));
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// String

#define JB_CURRENT_CLASS String

JB_DEFINE_WRAPPER_CLASS(
    "java/lang/String"
    ,
    NoFields
    ,
    NoMethods
)

jchar String::EmptyString[1]={0};
char String::EmptyUTFString[1]={0};

void String::Construct() {
    m_string=0;
    m_utfString=0;
}

String::String():
    CharSequence(NewString(EmptyString))
{
    Construct();
}

String::String(const jchar* string):
    CharSequence(NewString(string))
{
    Construct();
}

String::String(const jchar* string,jint length):
    CharSequence(NewString(string,length))
{
    Construct();
}

String::String(const char* string):
    CharSequence(NewStringUTF(string))
{
    Construct();
}

String::String(const jni::LObject& javaString):
    CharSequence(javaString)
{
    Construct();
}

String::~String() {
    if (m_string!=EmptyString) {
        delete[] m_string;
    }
    if (m_utfString!=EmptyUTFString) {
        delete[] m_utfString;
    }
}

jint String::GetLength() const {
    return jni::GetEnv()->GetStringLength((jstring)GetJObject());
}

const jchar* String::Get() const {
    pthreadpp::mutex_guard guard(m_lock);
    const_cast<String*>(this)->RetrieveString();
    return m_string;
}

const char* String::GetUTF() const {
    pthreadpp::mutex_guard guard(m_lock);
    const_cast<String*>(this)->RetrieveUTFString();
    return m_utfString;
}

void String::RetrieveString() {
    if (m_string) {
        return;
    }
    jstring javaString=(jstring)GetJObject();
    size_t length=jni::GetEnv()->GetStringLength(javaString);
    if (!length) {
        m_string=EmptyString;
    } else {
        jchar* string=new jchar[length+1];
        array_deleter<jchar> stringDeleter(string);
        const jchar* chars=jni::GetEnv()->GetStringChars(javaString,0);
        jni::TranslateJavaException();
        memcpy(string,chars,sizeof(jchar)*length);
        string[length]=0;
        jni::GetEnv()->ReleaseStringChars(javaString,chars);
        m_string=string;
        stringDeleter.detach();
    }
}

void String::RetrieveUTFString() {
    if (m_utfString) {
        return;
    }
    jstring javaString=(jstring)GetJObject();
    jint length=jni::GetEnv()->GetStringUTFLength(javaString);
    if (!length) {
        m_utfString=EmptyUTFString;
    } else {
        char* utfChars=new char[length+1];
        array_deleter<char> utfCharsDeleter(utfChars);
        const char* chars=jni::GetEnv()->GetStringUTFChars(javaString,0);
        jni::TranslateJavaException();
        memcpy(utfChars,chars,length);
        utfChars[length]=0;
        jni::GetEnv()->ReleaseStringUTFChars(javaString,chars);
        m_utfString=utfChars;
        utfCharsDeleter.detach();
    }
}

jni::LObject String::NewString(const jchar* string) {
    const jchar* end=string;
    while (*end) end++;
    return NewString(string,jint(end-string));
}

jni::LObject String::NewString(const jchar* string,jint length) {
    jobject result=jni::GetEnv()->NewString(string,length);
    jni::TranslateJavaException();
    return jni::LObject::WrapLocal(result);
}

jni::LObject String::NewStringUTF(const char* string) {
    jobject result=jni::GetEnv()->NewStringUTF(string);
    jni::TranslateJavaException();
    return jni::LObject::WrapLocal(result);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// Throwable

#define JB_CURRENT_CLASS Throwable

JB_DEFINE_WRAPPER_CLASS(
    "java/lang/Throwable"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
        "<init>",
        "()V"
    )
    (
        ConstructorS,
        "<init>",
        "(Ljava/lang/String;)V"
    )
    (
        ConstructorST,
        "<init>",
        "(Ljava/lang/String;Ljava/lang/Throwable;)V"
    )
    (
        ConstructorT,
        "<init>",
        "(Ljava/lang/Throwable;)V"
    )
    (
        GetMessage,
        "getMessage",
        "()Ljava/lang/String;"
    )
    (
        GetLocalizedMessage,
        "getLocalizedMessage",
        "()Ljava/lang/String;"
    )
    (
        GetCause,
        "getCause",
        "()Ljava/lang/Throwable;"
    )
    (
        InitCause,
        "initCause",
        "(Ljava/lang/Throwable;)Ljava/lang/Throwable;"
    )
    (
        PrintStackTrace,
        "printStackTrace",
        "()V"
    )
)

Throwable::Throwable():
    Object(JB_NEW(Constructor))
{
}

Throwable::Throwable(PString message):
    Object(JB_NEW(ConstructorS,message))
{
}

Throwable::Throwable(PString message,PThrowable cause):
    Object(JB_NEW(ConstructorST,message,cause))
{
}

Throwable::Throwable(PThrowable cause):
    Object(JB_NEW(ConstructorT,cause))
{
}

Throwable::Throwable(const jni::LObject& object):
    Object(object)
{
}

Throwable::Throwable(const jni::LObject& object,jfieldID instanceFieldID):
    Object(object,instanceFieldID)
{
}

PString Throwable::GetMessage() const {
    return PString::Wrap(JB_CALL_THIS(ObjectMethod,GetMessage));
}

PString Throwable::GetLocalizedMessage() const {
    return PString::Wrap(JB_CALL_THIS(ObjectMethod,GetLocalizedMessage));
}

PThrowable Throwable::GetCause() const {
    return PThrowable::Wrap(JB_CALL_THIS(ObjectMethod,GetCause));
}

PThrowable Throwable::InitCause(PThrowable cause) {
    return PThrowable::Wrap(JB_CALL_THIS(ObjectMethod,InitCause,cause));
}

void Throwable::PrintStackTrace() {
    JB_CALL_THIS(VoidMethod,PrintStackTrace);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// Exception

#define JB_CURRENT_CLASS Exception

JB_DEFINE_WRAPPER_CLASS(
    "java/lang/Exception"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
        "<init>",
        "()V"
    )
    (
        ConstructorS,
        "<init>",
        "(Ljava/lang/String;)V"
    )
    (
        ConstructorST,
        "<init>",
        "(Ljava/lang/String;Ljava/lang/Throwable;)V"
    )
    (
        ConstructorT,
        "<init>",
        "(Ljava/lang/Throwable;)V"
    )
)

Exception::Exception():
    Throwable(JB_NEW(Constructor))
{
}

Exception::Exception(PString message):
    Throwable(JB_NEW(ConstructorS,message))
{
}

Exception::Exception(PString message,PThrowable cause):
    Throwable(JB_NEW(ConstructorST,message,cause))
{
}

Exception::Exception(PThrowable cause):
    Throwable(JB_NEW(ConstructorT,cause))
{
}

Exception::Exception(const jni::LObject& object):
    Throwable(object)
{
}

Exception::Exception(const jni::LObject& object,jfieldID instanceFieldID):
    Throwable(object,instanceFieldID)
{
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// RuntimeException

#define JB_CURRENT_CLASS RuntimeException

JB_DEFINE_WRAPPER_CLASS(
    "java/lang/RuntimeException"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
        "<init>",
        "()V"
    )
    (
        ConstructorS,
        "<init>",
        "(Ljava/lang/String;)V"
    )
    (
        ConstructorST,
        "<init>",
        "(Ljava/lang/String;Ljava/lang/Throwable;)V"
    )
    (
        ConstructorT,
        "<init>",
        "(Ljava/lang/Throwable;)V"
    )
)

RuntimeException::RuntimeException():
    Exception(JB_NEW(Constructor))
{
}

RuntimeException::RuntimeException(PString message):
    Exception(JB_NEW(ConstructorS,message))
{
}

RuntimeException::RuntimeException(PString message,PThrowable cause):
    Exception(JB_NEW(ConstructorST,message,cause))
{
}

RuntimeException::RuntimeException(PThrowable cause):
    Exception(JB_NEW(ConstructorT,cause))
{
}

RuntimeException::RuntimeException(const jni::LObject& object):
    Exception(object)
{
}

RuntimeException::RuntimeException(const jni::LObject& object,jfieldID instanceFieldID):
    Exception(object,instanceFieldID)
{
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// misc

#define JB_CURRENT_CLASS ClassCastException

JB_DEFINE_ACCESSOR(
    "java/lang/ClassCastException"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
         "<init>","(Ljava/lang/String;)V"
    )
)

void ThrowClassCastException(java::PClass targetClass) {
    throw java::PThrowable::Wrap(JB_NEW(Constructor,targetClass->GetName()));
}

#undef JB_CURRENT_CLASS

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(java)
