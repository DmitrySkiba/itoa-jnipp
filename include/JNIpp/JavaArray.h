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
 * Contains wrappers for Java arrays.
 */

#ifndef _JNIPP_JAVARRAY_INCLUDED_
#define _JNIPP_JAVARRAY_INCLUDED_

#include "JavaLang.h"

BEGIN_NAMESPACE(java)

///////////////////////////////////////////////////////////////////// PrimitiveArray

/** Wrapper for primitive arrays (like \c int[]).
 * You don't need to use this class, use typedefs like 
 *  java::PIntArray instead.
 */
template <class JType>
class PrimitiveArray: public Object {
    JB_WRAPPER_CLASS(PrimitiveArray);
public:

    /** Creates primitive array of the specified length and
     *  wraps it.
     */
    PrimitiveArray(jsize length);

    /** Wraps \c array.
     */
    PrimitiveArray(const jni::LObject& array):
        Object(array),
        m_length(-1)
    {
    }

    /** Returns length of the array.
     */
    jsize GetLength() const {
        if (m_length==-1) {
            m_length=jni::GetArrayLength(*this);
        }
        return m_length;
    }
   
    /** Returns value at the specified index.
     */
    JType GetAt(jsize index) const {
        JType value=0;
        GetRegion(index,1,&value);
        return value;
    }

    /** Sets value at the specified index.
     */
    void SetAt(jsize index,JType value) {
        SetRegion(index,1,&value);
    }

    /** Copies specified region to the buffer.
     */
    void GetRegion(jsize start,jsize length,JType* buffer) const;

    /** Updates specifies region in the array.
     */
    void SetRegion(jsize start,jsize length,const JType* elements);

private:
    mutable jsize m_length;
    static java::Class* m_class;
};

template <class JType>
java::Class* PrimitiveArray<JType>::m_class=0;

///////////////////////////////////////////////// typedefs

/** Array of booleans (\c boolean[]) which uses
 *  C++ \c bool values.
 */
typedef PrimitiveArray<bool> BoolArray;
/** Pointer to BoolArray.
 */
typedef ObjectPointer<BoolArray> PBoolArray;


/** Array of booleans (\c boolean[]); see also BoolArray.
 */
typedef PrimitiveArray<bool> BooleanArray;
/** Pointer to BooleanArray.
 */
typedef ObjectPointer<BooleanArray> PBooleanArray;


/** Array of bytes (byte[]).
 */
typedef PrimitiveArray<jbyte> ByteArray;
/** Pointer to ByteArray.
 */
typedef ObjectPointer<ByteArray> PByteArray;


/** Array of chars (char[]).
 */
typedef PrimitiveArray<jchar> CharArray;
/** Pointer to CharArray.
 */
typedef ObjectPointer<CharArray> PCharArray;


/** Array of short integers (short[]).
 */
typedef PrimitiveArray<jshort> ShortArray;
/** Pointer to ShortArray.
 */
typedef ObjectPointer<ShortArray> PShortArray;


/** Array of integers (int[]).
 */
typedef PrimitiveArray<jint> IntArray;
/** Pointer to IntArray.
 */
typedef ObjectPointer<IntArray> PIntArray;


/** Array of long integers (long[]).
 */
typedef PrimitiveArray<jlong> LongArray;
/** Pointer to LongArray.
 */
typedef ObjectPointer<LongArray> PLongArray;


/** Array of floats (float[]).
 */
typedef PrimitiveArray<jfloat> FloatArray;
/** Pointer to FloatArray.
 */
typedef ObjectPointer<FloatArray> PFloatArray;


/** Array of doubles (double[]).
 */
typedef PrimitiveArray<jdouble> DoubleArray;
/** Pointer to DoubleArray.
 */
typedef ObjectPointer<DoubleArray> PDoubleArray;

///////////////////////////////////////////////////////////////////// ObjectArray

/** Wrapper template for object arrays.
 * Default value of \c ObjectType is java::Object, so 
 *  java::ObjectArray<> is a wrapper for an array of objects
 *  (\c Object[]). However, you should use java::PObjectArray
 *  most of the time. For example, to create object array of
 *  length 7 use \c java::PObjectArray::New(7).
 *
 * To declare N-dimensional array nest java::ObjectArray N times:
 * - <tt>
 *   java::ObjectArray<java::ObjectArray<java::ObjectArray<MyClass> > >
 *   </tt> 
 */
template <class ObjectType=Object>
class ObjectArray: public Object {
    JB_WRAPPER_CLASS(ObjectArray);
    typedef ObjectPointer<ObjectType> PObjectType;
public:

    /** Creates object array of the specified length and wraps it.
     */
    ObjectArray(jsize length):
        Object(jni::NewObjectArray(length,ObjectType::GetTypeClass())),
        m_length(length)
    {
    }

    /** Wraps \c array.
     */
    ObjectArray(const jni::LObject& array):
        Object(array),
        m_length(-1)
    {
    }

    /** Returns array length.
     */
    jsize GetLength() const {
        if (m_length==-1) {
            m_length=jni::GetArrayLength(*this);
        }
        return m_length;
    }

    /** Returns object at the specified index.
     */
    PObjectType GetAt(jsize index) const {
        return PObjectType::Wrap(jni::GetObjectArrayElement(*this,index));
    }
    
    /** Sets object at the specified index.
     */
    void SetAt(jsize index,const ObjectType& value) {
        jni::SetObjectArrayElement(*this,index,value);
    }

    /** Sets object at the specified index.
     */
    void SetAt(jsize index,PObjectType pvalue) {
        jni::SetObjectArrayElement(*this,index,pvalue);
    }
private:
    mutable jsize m_length;
    static java::Class* m_class;
};

template <class ObjectType>
java::Class* ObjectArray<ObjectType>::m_class=0;

template <class ObjectType>
inline java::PClass ObjectArray<ObjectType>::GetTypeClass() {
    extern java::PClass InitObjectArrayClass(java::Class*&,java::PClass);
    return InitObjectArrayClass(m_class,ObjectType::GetTypeClass());
}

///////////////////////////////////////////////// typedefs

/** Pointer to ObjectArray<Object>.
 */
typedef ObjectPointer<ObjectArray<Object> > PObjectArray;


/** Array of strings (\c String[]).
 */
typedef ObjectArray<String> StringArray;
/** Pointer to StringArray.
 */
typedef ObjectPointer<StringArray> PStringArray;

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(java)

#endif // _JNIPP_JAVARRAY_INCLUDED_
