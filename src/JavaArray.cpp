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
#include "array_deleter.h"

BEGIN_NAMESPACE(java)

///////////////////////////////////////////////////////////////////// PrimitiveArray

/* Mutex used by JNIPP_SPECIALIZE_PRIMITIVEARRAY.
 */
static pthreadpp::mutex g_primitiveArrayClassLock(
    pthreadpp::mutex::initializer());

/* Generates implementation for PrimitiveArray specialization.
 */
#define JNIPP_SPECIALIZE_PRIMITIVEARRAY(Type,TypeTag,TypeName) \
    template<> \
    PrimitiveArray<Type>::PrimitiveArray(jsize length): \
        Object(jni::New##TypeTag##Array(length)), \
        m_length(length) \
    { \
    } \
    template<> \
    java::PClass PrimitiveArray<Type>::GetTypeClass() { \
        pthreadpp::mutex_guard guard(g_primitiveArrayClassLock); \
        if (!m_class) { \
            java::PString className=new java::String("[" TypeName); \
            m_class=java::Class::ForName(className).Detach(); \
        } \
        return m_class; \
    } \
    template<> \
    void PrimitiveArray<Type>::GetRegion(jsize start,jsize length,Type* buffer) const { \
        jni::Get##TypeTag##ArrayRegion(*this,start,length,buffer); \
    } \
    template<> \
    void PrimitiveArray<Type>::SetRegion(jsize start,jsize length,const Type* elements) { \
        jni::Set##TypeTag##ArrayRegion(*this,start,length,elements); \
    }

JNIPP_SPECIALIZE_PRIMITIVEARRAY(bool,Bool,"Z")
JNIPP_SPECIALIZE_PRIMITIVEARRAY(jboolean,Boolean,"Z")
JNIPP_SPECIALIZE_PRIMITIVEARRAY(jbyte,Byte,"B")
JNIPP_SPECIALIZE_PRIMITIVEARRAY(jchar,Char,"C")
JNIPP_SPECIALIZE_PRIMITIVEARRAY(jshort,Short,"S")
JNIPP_SPECIALIZE_PRIMITIVEARRAY(jint,Int,"I")
JNIPP_SPECIALIZE_PRIMITIVEARRAY(jlong,Long,"J")
JNIPP_SPECIALIZE_PRIMITIVEARRAY(jfloat,Float,"F")
JNIPP_SPECIALIZE_PRIMITIVEARRAY(jdouble,Double,"D")

#undef JNIPP_SPECIALIZE_PRIMITIVEARRAY

///////////////////////////////////////////////////////////////////// Array

/* Mutex used by InitObjectArrayClass.
 */
static pthreadpp::mutex g_initObjectArrayClassLock(
    pthreadpp::mutex::initializer());

/* Function sets up object array class for a given element class.
 */
java::PClass InitObjectArrayClass(java::Class*& arrayClass,java::PClass elementClass) {
    pthreadpp::mutex_guard guard(g_initObjectArrayClassLock);
    if (arrayClass) {
        return arrayClass;
    }

    // Get name of element class.
    java::PString elementName=elementClass->GetName();
    size_t elementNameLength=elementName->GetLength();
    const jchar* elementNameChars=elementName->Get();

    // Make name of array class by prepending a '['.
    java::PString arrayName;
    {
        size_t length=elementNameLength+1;
        jchar* chars=new jchar[length];
        chars[0]='[';
        memcpy(chars+1,elementNameChars,elementNameLength*sizeof(jchar));

        array_deleter<jchar> charsDeleter(chars);
        arrayName=new java::String(chars,length);
    }

    // Get array class by name.
    arrayClass=Class::ForName(arrayName).Detach();
    return arrayClass;
}

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(java)
