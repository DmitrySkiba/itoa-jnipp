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
 * Defines Object class.
 */

#ifndef _JNIPP_JAVAOBJECT_INCLUDED_
#define _JNIPP_JAVAOBJECT_INCLUDED_

#include <typeinfo>
#include <dropins/pthreadpp.h>
#include "JavaNI.h"
#include "JavaBinding.h"
#include "JavaObjectPointer.h"

BEGIN_NAMESPACE(java)

class Class;
typedef ObjectPointer<Class> PClass;

class String;
typedef ObjectPointer<String> PString;

///////////////////////////////////////////////////////////////////// Object

class Object;
/** Pointer to Object.
 */
typedef ObjectPointer<Object> PObject;


/** The Object class, root of all.
 *
 * Wrapper for \c java.lang.Object.
 *
 * Object class uses intrusive reference counting: Retain() adds
 *  a reference, Release() releases it. However, you should use
 *  ObjectPointer for managing objects lifetime in an exception
 *  safe manner.
 *
 * When subclassing Object you need to decide whether to
 *  implement wrapper or live class:
 * - Wrapper classes let you to manipulate existing Java 
 *    classes.
 * - Live classes let you to implement Java class methods in
 *    native code. Live classes can also call methods and 
 *    get/set fields.
 *
 *
 * More about creating a wrapper class:
 * - Wrapper class can only be derived from other wrapper 
 *    class (like Object or Exception).
 * - JB_WRAPPER_CLASS() is used to declare wrapper class and
 *    JB_DEFINE_WRAPPER_CLASS() is used to describe methods and
 *    fields that wrapper class is going to access.
 * - Wrapping constructor Object(const jni::LObject&) is used to
 *    construct instances of a wrapper class.
 * - In most cases wrapper class should also implement protected
 *    live constructor Object(const jni::LObject&,jfieldID) to 
 *    allow live classes to be derived.
 * - Wrapper class object is destroyed when last reference is
 *    released by Release() method.
 * - Wrapper class objects are stateless, they are created ad-hoc
 *    and discarded after the use.
 * - EmailValidator example shows how to create wrapper classes.
 *
 *
 * More about creating a live class:
 * - Java class should define the following two members to be
 *    manageable by a live class:
 *   - <tt> protected native void finalize(); </tt>
 *   - <tt> private int nativeInstance; </tt>
 * - Live class can only be derived from wrapper class that have
 *    live constructor available (e.g. Object or Exception but not
 *    String). Live class can't be derived from other live class.
 * - JB_LIVE_CLASS() is used to declare live class and
 *    JB_DEFINE_LIVE_CLASS() is used to describe methods, fields
 *    and callbacks. Callbacks are class methods that implement
 *    Java class methods declared \c native. Association between
 *    callbacks and corresponding Java methods is established in 
 *    JB_DEFINE_LIVE_CLASS().
 * - Live constructor Object(const jni::LObject&,jfieldID)
 *    is used to construct instances of a live class.
 * - Live class object is destroyed only when the Java object (which
 *    was supplied to the live constructor) is collected by GC.
 * - LiveThread example shows how to override method of an existing
 *    Java class (Thread) and NativeSound example shows how to
 *    implement Java class completely in native code.
 */
class Object: public jni::AbstractObject {
public:

    /** Flag indicating whether class is live.
     * Added by JB_WRAPPER_CLASS() / JB_LIVE_CLASS().
     */
    const static bool IsLiveType=false;

    /** Returns Java class for this class.
     * Added by JB_WRAPPER_CLASS() / JB_LIVE_CLASS().
     */
    static java::PClass GetTypeClass();
    
#ifdef ONLY_FOR_DOXYGEN
    
    /** Returns instance field id.
     * Instance field must be declared as:
      * - <tt> private int nativeInstance; </tt>
     * Added by JB_LIVE_CLASS() macro.
     */
    static jfieldID GetInstanceFieldID();
    
#endif // ONLY_FOR_DOXYGEN

public:

    /** Creates \c java.lang.Object and wraps it.
     * See Object(const jni::LObject&) for details.
     */
    Object();

    /** Wraps \c object.
     * Method performs the following:
     * - Sets reference count to 0.
     * - Stores Java object and adds global reference to it.
     *
     * See also Object(const jni::LObject&,jfieldID).
     */
    Object(const jni::LObject& object);
    
    /** Tests whether this object is live.
     */
    bool IsLive() const;
   
    /** Increments reference counter.
     * If this object is live incrementing counter from 1 to 2
     *  switches contained Java object from weak to global
     *  reference. 
     */
    void Retain() const;
    
    /** Decrements reference counter.
     * For live objects decrementing counter from 2 to 1
     *  switches contained Java object from global to weak
     *  reference allowing Java's GC to collect it.
     *
     * When reference count hits zero (or goes below) object 
     *  is deleted.
     */
    void Release() const;

    /** Returns contained Java object.
     */
    virtual jobject GetJObject() const;

    /** Retrieves object instance pointer from the Java object.
     */
    static Object* GetLiveInstance(jobject object,jfieldID instanceFieldID);


    /***** java.lang.Object methods *****/

    /** Indicates whether some other object is "equal to" this one.
     */
    bool Equals(PObject other) const;

    /** Returns object's class.
     */
    PClass GetClass() const;

    /** Returns a hash code value for the object.
     */
    jint GetHashCode() const;

    /** Wakes up a single thread that is waiting on this object's monitor.
     */
    void Notify();

    /** Wakes up all threads that are waiting on this object's monitor.
     */
    void NotifyAll();

    /** Causes current thread to wait until another thread invokes the 
     *  Notify() method or the NotifyAll() method for this object.
     */
    void Wait();

    /** Causes current thread to wait until either another thread invokes 
     *  the Notify() method or the NotifyAll() method for this object, 
     *  or a specified amount of time has elapsed.
     */
    void Wait(jlong timeout);

    /** Causes current thread to wait until either another thread invokes 
     *  the Notify() method or the NotifyAll() method for this object, 
     *  or a specified amount of time has elapsed.
     */
    void Wait(jlong timeout,jint nanos);

    /** Returns a string representation of the object.
     */
    PString ToString() const;

    /** Returns class of the specified object.
     */
    static PClass GetClass(const AbstractObject& object);

protected:

    /** Constructs live object.
     * Method performs the following:
     * - Sets instance pointer (\c this) to the \c int field
     *    identified by \c instanceFieldID.
     * - Sets reference count to 1.
     * - Stores Java object and adds weak reference to it.
     *
     * See also Object(const jni::LObject&).
     */
    Object(const jni::LObject& object,jfieldID instanceFieldID);

    /** Destroys reference to the contained Java object.
     */
    virtual ~Object();

private:
    void Construct(const jni::LObject&,jfieldID);
private:
    mutable jobject m_object;
    jobject m_weakReference;
    jfieldID m_instanceFieldID;
    mutable size_t m_referenceCount;
    mutable pthreadpp::mutex m_lock;
};

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(java)

#endif // _JNIPP_JAVAOBJECT_INCLUDED_
