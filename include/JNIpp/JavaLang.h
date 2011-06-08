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

/** \file
 * Contains java::Object and basic wrapper classes
 *  like java::String or java::Class.
 */

#ifndef _JNIPP_JAVALANG_INCLUDED_
#define _JNIPP_JAVALANG_INCLUDED_

#include <dropins/pthreadpp.h>
#include "JavaObject.h"

//TODO cache length/utfLength in java::String.
//TODO add all JNIEnv functions that java::String to #jni.

/** Contains java::Object and basic wrapper classes.
 */
BEGIN_NAMESPACE(java)

class String;
typedef ObjectPointer<String> PString;

///////////////////////////////////////////////////////////////////// Class

class Class;
/** Pointer to Class.
 */
typedef ObjectPointer<Class> PClass;


/** Wrapper for \c java.lang.Class.
 */
class Class: public Object {
    JB_WRAPPER_CLASS(Class);
public:
    /** Wraps \c clazz.
     */
    explicit Class(const jni::LObject& clazz);
    
    /** Returns class' name.
     */
    PString GetName() const;

    /** Return class for a \c name.
     */
    static PClass ForName(const char* name);

    /** Returns class for a \c name.
     */
    static PClass ForName(PString name);
};

///////////////////////////////////////////////////////////////////// CharSequence

class CharSequence;
/** Pointer to CharSequence.
 */
typedef ObjectPointer<CharSequence> PCharSequence;


/** Wrapper for \c java.lang.CharSequence.
 *
 * Note that while CharSequence represents interface its methods
 *  are not virtual. That is because they are virtual at the
 *  Java side, so calling GetLength() on a CharSequence will 
 *  indirectly call implementation from the most derived class.
 */
class CharSequence: public Object {
    JB_WRAPPER_CLASS(CharSequence);
public:
    /** Wraps \c charSequence.
     */
    CharSequence(const jni::LObject& charSequence);
   
    /** Returns character at \c index.
     */
    jchar CharAt(jint index) const;
    
    /** Returns length.
     */
    jint GetLength() const;
    
    /** Returns sub-sequence.
     */
    PCharSequence GetSubSequence(jint start,jint end) const;

protected:
    /** Constructs live object.
     */
    CharSequence(const jni::LObject&,jfieldID);
};

///////////////////////////////////////////////////////////////////// String

class String;
/** Pointer to String.
 */
typedef java::ObjectPointer<String> PString;


/** Wrapper for \c java.lang.String.
 */
class String: public CharSequence {
    JB_WRAPPER_CLASS(String);
public:
    /** Creates empty \c java.lang.String and wraps it.
     */
    String();

    /** Creates \c java.lang.String from \c string and wraps it.
     */
    explicit String(const char* string);

    /** Creates \c java.lang.String from \c string and wraps it.
     */
    explicit String(const jchar* string);

    /** Creates \c java.lang.String from \c string of the specified
     *  length and wraps it.
     */
    String(const jchar* string,jint length);

    /** Wraps \c string.
     */
    explicit String(const jni::LObject& string);
    
    /** Returns number of unicode characters in the string.
     * The value is NOT cached, #jni function is called each time.
     */
    jint GetLength() const;
    
    /** Returns unicode characters of the string.
     * The value is cached, so you can safely call this method
     *  multiple times.
     */
    const jchar* Get() const;

    /** Returns number of bytes in UTF representation of the
     *  string.
     * To get number of characters in the string use GetLength().
     */
    jint GetUTFLength() const;
    
    /** Returns UTF version of the string.
     * The value is cached, so you can safely call this method
     *  multiple times.
     */
    const char* GetUTF() const;

protected:
    virtual ~String();
private:
    void Construct();
    void RetrieveString();
    void RetrieveUTFString();
    // These methods will go to #jni some day.
    static jni::LObject NewString(const jchar*);
    static jni::LObject NewString(const jchar*,jsize);
    static jni::LObject NewStringUTF(const char*);
private:
    mutable pthreadpp::mutex m_lock;
    jchar* m_string;
    char* m_utfString;
    static jchar EmptyString[1];
    static char EmptyUTFString[1];
};

///////////////////////////////////////////////////////////////////// Throwable

class Throwable;
/** Pointer to Throwable.
 */
typedef ObjectPointer<Throwable> PThrowable;


/** Wrapper for \c java.lang.Throwable.
 * Don't throw instances of this class, use concrete exception
 *  type or java::RuntimeException.
 *
 * See java::RuntimeException on how to throw exceptions.
 */
class Throwable: public Object {
    JB_WRAPPER_CLASS(Throwable);
public:
    /** Creates \c java.lang.Throwable and wraps it.
     */
    Throwable();

    /** Creates \c java.lang.Throwable(message) 
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    explicit Throwable(PString message);

    /** Creates \c java.lang.Throwable(message,cause)
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    Throwable(PString message,PThrowable cause);

    /** Creates \c java.lang.Throwable(cause)
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    explicit Throwable(PThrowable cause);

    /** Wraps \c throwable.
     */
    explicit Throwable(const jni::LObject& throwable);

    /** Returns the detail message string of this throwable.
     */
    PString GetMessage() const;

    /** Creates a localized description of this throwable.
     */
    PString GetLocalizedMessage() const;

    /** Returns the cause of this throwable.
     */
    PThrowable GetCause() const;

    /** Initializes the cause of this throwable to the specified value.
     */
    PThrowable InitCause(PThrowable cause);

    /** Prints this throwable and its backtrace to the standard error stream.
     */
    void PrintStackTrace();

protected:
    /** Constructs live object.
     */
    Throwable(const jni::LObject&,jfieldID);
};

///////////////////////////////////////////////////////////////////// Exception

class Exception;
/** Pointer to Exception.
 */
typedef ObjectPointer<Exception> PException;


/** Wrapper for \c java.lang.Exception.
 *
 * See java::RuntimeException on how to throw exceptions.
 */
class Exception: public Throwable {
    JB_WRAPPER_CLASS(Exception);
public:
    /** Creates \c java.lang.Exception and wraps it.
     */
    Exception();

    /** Creates \c java.lang.Exception(message) 
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    explicit Exception(PString message);

    /** Creates \c java.lang.Exception(message,cause) 
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    Exception(PString message,PThrowable cause);

    /** Creates \c java.lang.Exception(cause) 
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    explicit Exception(PThrowable cause);

    /** Wraps \c exception.
     */
    explicit Exception(const jni::LObject& exception);

protected:
    /** Constructs live object.
     */
    Exception(const jni::LObject&,jfieldID);
};

///////////////////////////////////////////////////////////////////// RuntimeException

class RuntimeException;
/** Pointer to RuntimeException.
 */
typedef ObjectPointer<RuntimeException> PRuntimeException;


/** Wrapper for \c java.lang.RuntimeException.
 *
 * The only correct way to throw a Java exception is to
 *  throw its ObjectPointer by value:
 * \code
 *
 * java::PString message=java::PString::New("Oops!");
 * throw java::PRuntimeException::New(message);
 *
 * \endcode
 */
class RuntimeException: public Exception {
    JB_WRAPPER_CLASS(RuntimeException);
public:
    /** Creates \c java.lang.RuntimeException and wraps it.
     */
    RuntimeException();

    /** Creates \c java.lang.RuntimeException(message) 
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    explicit RuntimeException(PString message);

    /** Creates \c java.lang.RuntimeException(message,cause) 
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    RuntimeException(PString message,PThrowable cause);

    /** Creates \c java.lang.RuntimeException(cause) 
     *  and wraps it; don't call this constructor from
     *  derived classes.
     */
    explicit RuntimeException(PThrowable cause);

    /** Wraps \c exception.
     */
    explicit RuntimeException(const jni::LObject& exception);

protected:
    /** Constructs live object.
     */
    RuntimeException(const jni::LObject&,jfieldID);
};

///////////////////////////////////////////////////////////////////// casts

/* Extracts ObjectType from ObjectPointer.
 */
template <class ObjectType>
class ObjectTypeExtractor {
public:
    typedef ObjectType Type;
};
template <class ObjectType>
class ObjectTypeExtractor<ObjectPointer<ObjectType> > {
public:
    typedef ObjectType Type;
};


/** Checks whether \c object can be cast to \c OtherObjectType.
 */
template <class OtherObjectType>
inline bool IsInstanceOf(const jni::AbstractObject& object) {
    typedef typename ObjectTypeExtractor<OtherObjectType>::Type RealOOType;
    if (!object.GetJObject()) {
        return false;
    }
    return IsAssignableFrom(GetObjectClass(object),RealOOType::GetTypeClass());
}

#ifdef ONLY_FOR_DOXYGEN

/** Checks and casts \c object to \c OtherObjectType.
 * If check (see IsInstanceOf()) fails function throws \c java.lang.ClassCastException.
 */
template <class OtherObjectType>
inline ObjectPointer<OtherObjectType> Cast(const jni::AbstractObject& object);

#endif // ONLY_FOR_DOXYGEN

/* The hack above is to hide weird return type.
 */
template <class OtherObjectType>
inline ObjectPointer<typename ObjectTypeExtractor<OtherObjectType>::Type> Cast(const jni::AbstractObject& object) {
    typedef typename ObjectTypeExtractor<OtherObjectType>::Type RealOOType;
    if (!object.GetJObject()) {
        return ObjectPointer<RealOOType>();
    }
    if (!IsInstanceOf<RealOOType>(object)) {
       extern void ThrowClassCastException(java::PClass);
       ThrowClassCastException(RealOOType::GetTypeClass());
    }
    return ObjectPointer<RealOOType>::Wrap(object.GetJObject());
}


/* Since every object is-a \c java.lang.Object we can 
 *  optimize java::Object cases.
 */
template <>
inline ObjectPointer<Object> Cast<Object>(const jni::AbstractObject& object) {
    return ObjectPointer<Object>::Wrap(object.GetJObject());
}
template <>
inline ObjectPointer<Object> Cast<ObjectPointer<Object> >(const jni::AbstractObject& object) {
    return ObjectPointer<Object>::Wrap(object.GetJObject());
}
template <>
inline bool IsInstanceOf<Object>(const jni::AbstractObject& object) {
    return object.GetJObject()!=0;
}
template <>
inline bool IsInstanceOf<ObjectPointer<Object> >(const jni::AbstractObject& object) {
    return object.GetJObject()!=0;
}

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(java)

#endif // _JNIPP_JAVALANG_INCLUDED_
