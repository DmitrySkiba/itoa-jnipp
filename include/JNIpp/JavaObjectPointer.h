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
 * Smart pointer for java wrapper objects.
 */

#ifndef _JNIPP_JAVAOBJECTPOINTER_INCLUDED_
#define _JNIPP_JAVAOBJECTPOINTER_INCLUDED_

#include <algorithm>
#include <dropins/begin_namespace.h>

BEGIN_NAMESPACE(java)

///////////////////////////////////////////////////////////////////// helpers

///////////////////////////////////////////////// New() method

#define xJNIPP_GENERATOR_CLASS_A(N) class A##N
#define xJNIPP_GENERATOR_ARGUMENT_A(N) A##N a##N
#define xJNIPP_GENERATOR_A(N) a##N

#define xJNIPP_GENERATE_NEW(N,Type) \
    template < \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATOR_CLASS_A,comma) \
    > \
    static ObjectPointer<Type> New( \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATOR_ARGUMENT_A,comma) \
    ) { \
        return ObjectPointer<Type>(new Type( \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATOR_A,comma) \
        )); \
    }

#define xJNIPP_IMPLEMENT_NEW(Type) \
    static ObjectPointer<Type> New() { \
        return ObjectPointer<Type>(new Type()); \
    } \
    xJNIPP_GENERATE(1,xJNIPP_GENERATE_MAX_N,xJNIPP_GENERATE_NEW,Type)

///////////////////////////////////////////////// ObjectPointerWrapper

template <class ObjectType>
class ObjectPointer;

/* Depending on \c ObjectType::IsLiveType value we either wrap
 *  Java object itself or wrap a live object instance stored 
 *  inside Java object.
 */
template <class ObjectType,bool Live=ObjectType::IsLiveType>
class ObjectPointerWrapper;

/* ObjectType is live class.
 */
template <class ObjectType>
class ObjectPointerWrapper<ObjectType,true> {
public:
    static ObjectPointer<ObjectType> Wrap(const jni::LObject& object) {
        return Wrap(object.GetJObject());
    }
    static ObjectPointer<ObjectType> Wrap(jobject object) {
        return ObjectPointer<ObjectType>(static_cast<ObjectType*>(
            ObjectType::GetLiveInstance(object,ObjectType::GetInstanceFieldID())
        ));
    }
};

/* ObjectType is wrapper class.
 */
template <class ObjectType>
class ObjectPointerWrapper<ObjectType,false> {
public:
    static ObjectPointer<ObjectType> Wrap(const jni::LObject& object) {
        return ObjectPointer<ObjectType>(new ObjectType(object));
    }
    static ObjectPointer<ObjectType> Wrap(jobject object) {
        return Wrap(jni::LObject::Wrap(object));
    }
};

///////////////////////////////////////////////////////////////////// ObjectPointer

/** Smart pointer for classes derived from Object.
 *
 * ObjectPointer adds a reference (Object::Retain()) to the object
 *  in constructor and releases (Object::Release()) in destructor.
 *  You must use this class because all JNIpp library is build
 *  around it (for example java::Cast() returns it).
 *
 * ObjectPointer is derived from jni::AbstractObject, so you can
 *  (and in fact should) pass it to #jni functions by value.
 *
 * The next most useful thing is ObjectPointer::Wrap() method which
 *  wraps jni::LObject into \c ObjectType. That method works for
 *  both wrapper and live classes:
 * - If \c ObjectType is a wrapper class (Object::IsLiveType=false)
 *   new instance of \c ObjectType is created using wrapping 
 *   constructor (Object::Object(const jni::LObject&).
 * - If \c ObjectType is a live class (Object::IsLiveType=true)
 *   instance pointer is extracted from Java object using
 *   Object::GetLiveInstance() and wrapped into ObjectPointer. 
 *   Object::GetInstanceFieldID() is used to get id of an instance
 *   field.
 *
 * ObjectPointer also features ObjectPointer::New() method which
 *  creates \c ObjectType object and wraps it into ObjectPointer
 *  in one call. Compare:
 * - <tt> Func(java::PString::New("Hello!")) </tt>
 * - <tt> Func(java::PString(new java::String("Hello!"))) </tt>
 *
 * ObjectPointer supports the following usages:
 * - <tt> if (objectPtr) </tt>
 * - <tt> if (!objectPtr) </tt>
 * - <tt> if (objectPtr1==objectPtr2) </tt>
 * - <tt> if (objectPtr1!=objectPtr2) </tt>
 */
template <typename ObjectType>
class ObjectPointer: public jni::AbstractObject {
    class UnknownObject;
public:
    
    /** Constructs pointer with \c object; by default constructs
     *  empty pointer.
     * Method calls \c Object::Retain() on \c object.
     */
    ObjectPointer(ObjectType* object=0) {
        Construct(object);
    }
    
    /** Constructs pointer with \c object derived from to \c ObjectType.
     * Method calls \c Object::Retain() on \c object.
     */
    template <class OtherObjectType>
    ObjectPointer(OtherObjectType* object) {
        Construct(object);
    }
    
    /** Copy-constructor.
     */
    ObjectPointer(const ObjectPointer<ObjectType>& other) {
        Construct(other.m_object);
    }
    
    /** Copy-constructor that takes pointer to derived object.
     */
    template <typename OtherObjectType>
    ObjectPointer(const ObjectPointer<OtherObjectType>& other) {
        Construct(other.Get());
    }

    /** Destructor; calls \c Reset().
     */
    virtual ~ObjectPointer() {
        Reset();
    }

    /** Swaps content with another pointer.
     */    
    void Swap(ObjectPointer<ObjectType>& other) {
        std::swap(m_object,other.m_object);    
    }

    /** Resets object pointer: releases and nulls contained object.
     */
    void Reset() {
        if (m_object) {
            m_object->Release();
            m_object=0;
        }
    }
    
    /** Detaches contained object.
     * Nulls and returns contained object. NOTE: don't forget to call 
     *  \c Object::Release() on returned object.
     */
    ObjectType* Detach() {
        ObjectType* object=m_object;
        m_object=0;
        return object;
    }

    /** Returns contained object.
     */
    ObjectType* Get() const {
        return m_object;
    }
    
    /** Returns contained object.
     */
    ObjectType* operator->() const {
        return m_object;
    }
    
    /** Returns reference to the contained object.
     * NOTE: make sure that object pointer contains non-NULL object
     *  before calling this method.
     */
    ObjectType& operator*() const {
        return *m_object;
    }
    
    /** Helper operator, allows for testing and comparing object pointers.
     */
    operator const UnknownObject*() const {
        return reinterpret_cast<const UnknownObject*>(m_object);
    }
    
    /** Tests whether object pointer is empty.
     */
    bool operator!() const {
        return m_object==0;
    }
    
    /** Assignment operator.
     */
    ObjectPointer<ObjectType>& operator=(const ObjectPointer<ObjectType>& other) {
        ObjectPointer<ObjectType>(other).Swap(*this);
        return *this;
    }
    
    /** Assignment operator for types derived from \c ObjectType.
     */
    template <typename OtherObjectType>
    ObjectPointer<ObjectType>& operator=(const ObjectPointer<OtherObjectType>& other) {
        ObjectPointer<ObjectType>(other).Swap(*this);
        return *this;
    }
    
    /** Returns Java object that is contained in this pointer's
     *  object, or NULL if the pointer is empty.
     */
    virtual jobject GetJObject() const {
        return m_object?m_object->GetJObject():0;
    }

    #ifdef ONLY_FOR_DOXYGEN
    
    /** Instantiates \c ObjectType and wraps it into ObjectPointer.
     */
    static ObjectPointer<ObjectType> New(...);
    
    #endif // ONLY_FOR_DOXYGEN

    /* Generate New() implementations.
     */
    xJNIPP_IMPLEMENT_NEW(ObjectType);

    /** Creates object pointer from local object.
     * No type checks are performed, you should make sure you are wrapping
     *  correct object.
     */
    static ObjectPointer<ObjectType> Wrap(const jni::LObject& object) {
        if (!object) {
            return ObjectPointer<ObjectType>();
        }
        return ObjectPointerWrapper<ObjectType>::Wrap(object);
    }

    /** Creates object pointer from java object.
     * No type checks are performed, you should make sure you are wrapping
     *  correct object.
     */
    static ObjectPointer<ObjectType> Wrap(jobject object) {
        if (!object) {
            return ObjectPointer<ObjectType>();
        }
        return ObjectPointerWrapper<ObjectType>::Wrap(object);
    }
private:
    void Construct(ObjectType* object) {
        m_object=object;
        if (m_object) {
            m_object->Retain();
        }
    }
private:
    ObjectType* m_object;
};

/* Cleanup */

#undef xJNIPP_GENERATOR_CLASS_A
#undef xJNIPP_GENERATOR_ARGUMENT_A
#undef xJNIPP_GENERATOR_A
#undef xJNIPP_GENERATE_NEW
#undef xJNIPP_IMPLEMENT_NEW

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(java)

#endif // _JNIPP_JAVAOBJECTPOINTER_INCLUDED_
