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

// TODO Convert all FatalError()s to std::exceptions.

#define DEBUG_LOG(...) //fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\n")

BEGIN_NAMESPACE(java)

///////////////////////////////////////////////////////////////////// helpers

///////////////////////////////////////////////// WeakReference

// Creates weak reference for the object.
static jobject CreateWeakReference(jobject object);

// Gets object from the weak reference, adds global
//  reference to it.
static jobject DerefWeakReference(jobject weakReference);

// Destroys weak reference.
static void DestroyWeakReference(jobject weakReference);


#ifdef JNIPP_EMULATE_WEAK_GLOBAL_REFERENCES

/* Implementation of the WeakReference functions using 
 *  java.lang.ref.WeakReference class.
 */

#define JB_CURRENT_CLASS WeakReference

JB_DEFINE_ACCESSOR(
    "java/lang/ref/WeakReference"
    ,
    NoFields    
    ,
    Methods
    (
        Constructor,
        "<init>","(Ljava/lang/Object;)V"
    )
    (
        Get,
        "get","()Ljava/lang/Object;"
    )
)

static jobject LocalToGlobalRef(jobject object) {
    if (!object) {
        return 0;
    }
    jobject globalObject=jni::GetEnv()->NewGlobalRef(object);
    jni::GetEnv()->DeleteLocalRef(object);
    return globalObject;
}

static jobject CreateWeakReference(jobject object) {
    jobject weakReference=jni::GetEnv()->NewObject(
        (jclass)JB_GET_CLASS()->GetJObject(),
        JB_GET_METHOD_ID(Constructor),
        object);
    jni::TranslateJavaException();
    return LocalToGlobalRef(weakReference);
}

static jobject DerefWeakReference(jobject weakReference) {
    jobject object=jni::GetEnv()->CallObjectMethod(
        weakReference,
        JB_GET_METHOD_ID(Get));
    return LocalToGlobalRef(object);
}

static void DestroyWeakReference(jobject weakReference) {
    jni::GetEnv()->DeleteGlobalRef(weakReference);
}

#undef JB_CURRENT_CLASS

#else // !JNIPP_EMULATE_WEAK_GLOBAL_REFERENCES

/* Implementation of the WeakReference functions using 
 *  JNI's weak global references.
 */

static jobject CreateWeakReference(jobject object) {
    return jni::GetEnv()->NewWeakGlobalRef(object);
}

static jobject DerefWeakReference(jobject weakReference) {
    return jni::GetEnv()->NewGlobalRef(weakReference);
}

static void DestroyWeakReference(jobject weakReference) {
    jni::GetEnv()->DeleteWeakGlobalRef((jweak)weakReference);
}

#endif // JNIPP_EMULATE_WEAK_GLOBAL_REFERENCES

///////////////////////////////////////////////////////////////////// Object

#define JB_CURRENT_CLASS Object

JB_DEFINE_WRAPPER_CLASS(
    "java/lang/Object"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
         "<init>","()V"
    )
    (
        Equals,
        "equals","(Ljava/lang/Object;)Z"
    )
    (
        GetHashCode,
        "hashCode","()I"
    )
    (
        Notify,
        "notify","()V"
    )
    (
        NotifyAll,
        "notifyAll","()V"
    )
    (
        Wait,
        "wait","()V"
    )
    (
        WaitTimeout,
        "wait","(J)V"
    )
    (
        WaitNanoTimeout,
        "wait","(JI)V"
    )
    (
        ToString,
        "toString","()Ljava/lang/String;"
    )
)

void Object::Construct(const jni::LObject& object,jfieldID instanceFieldID) {
    m_object=0;
    m_weakReference=0;
    m_referenceCount=0;
    m_instanceFieldID=instanceFieldID;
    if (!object) {
        jni::FatalError("Can't create java::Object with null object.");
    }
    if (!m_instanceFieldID) {
        m_object=jni::GetEnv()->NewGlobalRef(object.GetJObject());
        DEBUG_LOG("Object(%p): wrapped %p",this,m_object);
    } else {
        m_weakReference=CreateWeakReference(object.GetJObject());
        DEBUG_LOG("Object(%p): live instance: %p, weak reference: %p",
                  this,
                  object.GetJObject(),
                  m_weakReference);
        jni::SetIntField(object,m_instanceFieldID,(jint)this);
        m_referenceCount++;
    }
}

Object::Object() {
    Construct(JB_NEW(Constructor),0);
}

Object::Object(const jni::LObject& object) {
    Construct(object,0);
}

Object::Object(const jni::LObject& object,jfieldID instanceFieldID) {
    if (!instanceFieldID) {
        jni::FatalError("Invalid instance field (0) passed to the live constructor.");
    }
    Construct(object,instanceFieldID);
}

Object::~Object() {
    DEBUG_LOG("Object(%p): destructor",this);
    if (m_object) {
        jni::GetEnv()->DeleteGlobalRef(m_object);
    }
    if (m_weakReference) {
        // We must zero instance field to avoid destroying
        //  object twice in the situation where constructor 
        //  of a derived class calls Object's live constructor
        //  but then finishes with exception. In that case 
        //  finalizer for the derived live object will try to 
        //  Release() reference causing second object destruction.
        jobject object=DerefWeakReference(m_weakReference);
        if (object) {
            jni::GetEnv()->SetIntField(object,m_instanceFieldID,0);
            jni::GetEnv()->DeleteGlobalRef(object);
        }
        DestroyWeakReference(m_weakReference);
    }
}

jobject Object::GetJObject() const {
    if (!m_object) {
        if (m_weakReference) {
            jni::FatalError("Tried to get weakly referenced object.");
        }
    }
    return m_object;
}

bool Object::IsLive() const {
    return m_instanceFieldID!=0;
}

void Object::Retain() const {
    pthreadpp::mutex_guard guard(m_lock);
    m_referenceCount++;
    if (IsLive() && m_referenceCount==2) {
        // Someone referenced us from the native code - switch to
        //  global reference.
        m_object=DerefWeakReference(m_weakReference);
        DEBUG_LOG("Object(%p): switched to strong reference %p",this,m_object);
    }
}

void Object::Release() const {
    {
        pthreadpp::mutex_guard guard(m_lock);
        m_referenceCount--;
        if (IsLive() && m_referenceCount==1) {
            // Last reference from native code has gone, the only reference
            //  left is from the java class - switch to weak reference.
            DEBUG_LOG("Object(%p): switched %p to weak reference...",this,m_object);
            jni::GetEnv()->DeleteGlobalRef(m_object);
            m_object=0;
            return;
        }
        if (m_referenceCount>0) {
            // There are references, do nothing.
            return;
        }
    }
    // No references left - delete self.
    delete this;
}

Object* Object::GetLiveInstance(jobject object,jfieldID instanceFieldID) {
    int instanceData=jni::GetEnv()->GetIntField(object,instanceFieldID);
    return (Object*)instanceData;
}


/***** java.lang.Object methods *****/

bool Object::Equals(PObject other) const {
    return JB_CALL_THIS(BoolMethod,Equals,other);
}

PClass Object::GetClass() const {
    return GetClass(*this);
}

jint Object::GetHashCode() const {
    return JB_CALL_THIS(IntMethod,GetHashCode);
}

void Object::Notify() {
    JB_CALL_THIS(VoidMethod,Notify);
}

void Object::NotifyAll() {
    JB_CALL_THIS(VoidMethod,NotifyAll);
}

void Object::Wait() {
    JB_CALL_THIS(VoidMethod,Wait);
}

void Object::Wait(jlong timeout) {
    JB_CALL_THIS(VoidMethod,WaitTimeout,timeout);
}

void Object::Wait(jlong timeout,jint nanos) {
    JB_CALL_THIS(VoidMethod,WaitNanoTimeout,timeout,nanos);
}

PString Object::ToString() const {
    return PString::Wrap(JB_CALL_THIS(ObjectMethod,ToString));
}

PClass Object::GetClass(const AbstractObject& object) {
    // This method can be implemented by calling 'getClass' java method
    //  or JNIEnv::GetObjectClass. For now I prefer the latter, it seems
    //  faster.
    return PClass::Wrap(jni::GetObjectClass(object));
}

#undef JB_CURRENT_CLASS

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(java)
