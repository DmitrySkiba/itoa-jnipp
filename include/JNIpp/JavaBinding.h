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
 * Provides macros for implementing live and wrapper classes.
 */

#ifndef _JNIPP_JAVABINDING_INCLUDED_
#define _JNIPP_JAVABINDING_INCLUDED_

#include <pthread.h>
#include <limits.h>
#include <dropins/begin_namespace.h>
#include "JavaNI.h"
#include "JavaObjectPointer.h"

// TODO use custom struct instead of JNINativeMethod, because some
//        implementations declare 'name' and 'signature' as 'char*' so
//        ugly cast is required (which can trigger error depending on GCC
//        settings).
// TODO Move ConvertBool to jni::
// TODO Make JB_GET_CLASS() return java::PClass.
// TODO Move callbacks initialization to the method (GetCallbacks()).
// TODO Obfuscation might strip nativeInstance field.
// TODO Special-case nativeInstance and finalizer callback.
// TODO Move most of Finalize out of the macro.
// TODO Get rid of hack behind xJB_THIS.

/** Contains implementation details of JB_ macros; 
 *  see JavaBinding.h for the list.
 */
BEGIN_NAMESPACE(jb)

///////////////////////////////////////////////////////////////////// java bindings

#ifdef ONLY_FOR_DOXYGEN

/** Sets current class; needs to be defined before using any 
 *  other JB_ macros.
 *
 * When implementing wrapper or live class \c JB_CURRENT_CLASS
 *  should match class name. When used with JB_DEFINE_ACCESSOR()
 *  it can be  arbitrary (but unique) name.
 *
 * JB_CURRENT_CLASS must be undefined at the end of implementation.
 *
 * \code
 * #define JB_CURRENT_CLASS MyWrapperClass
 *
 * JB_DEFINE_WRAPPER_CLASS(...)
 *
 * Implementation of MyWrapperClass methods using
 *  JB_ macros like JB_SET() or JB_NEW().
 *
 * #undef JB_CURRENT_CLASS 
 * \endcode
 */
#define JB_CURRENT_CLASS

#endif // ONLY_FOR_DOXYGEN


/** Declares \c Type to be Java wrapper class.
 *
 * Macro declares the following:
 *  - <tt> const static bool IsLiveType=false; </tt>
 *  - <tt> static java::PClass GetTypeClass(); </tt>
 *
 * Macro changes access to \c private.
 *
 * \code
 * class MyWrapper: public java::Object {
 *   JB_WRAPPER_CLASS(MyWrapper);
 * public:
 *   ...
 * };
 * \endcode
 * See also: JB_DEFINE_WRAPPER_CLASS().
 */
#define JB_WRAPPER_CLASS(Type) \
    public: \
    const static bool IsLiveType=false; \
    static java::PClass GetTypeClass(); \
    private: \
    friend class java::ObjectPointerWrapper<Type>;


/** Declares \c Type to be Java live class.
 *
 * Macro declares the following:
 *  - <tt> const static bool IsLiveType=true; </tt>
 *  - <tt> static java::PClass GetTypeClass(); </tt>
 *  - <tt> static jfieldID GetInstanceFieldID(); </tt>
 *  - Couple of internal implementation details.
 *
 * Macro changes access to \c private.
 *
 * \code
 * class MyLiveClass: public java::Object {
 *   JB_LIVE_CLASS(MyLiveClass);
 * public:
 *   ...
 * };
 * \endcode
 * See also: JB_DEFINE_LIVE_CLASS().
 */
#define JB_LIVE_CLASS(Type) \
    public: \
    const static bool IsLiveType=true; \
    static java::PClass GetTypeClass(); \
    static jfieldID GetInstanceFieldID(); \
    private: \
    friend class java::ObjectPointerWrapper<Type>; \
    friend class xJB_CALLBACKS_ACCESSOR_FOR(Type); \
    static void Finalizer(JNIEnv*,jobject thiz)


/** Defines Java bindings for the current accessor class.
 * This macro is equivalent to JB_DEFINE_WRAPPER_CLASS() except that
 *  it doesn't implement any class methods.
 *
 * Accessor classes are useful when you want to provide
 *  interface to the Java class as a set of global static 
 *  functions (example: \c GC() global function that calls 
 *  \c System.gc()).
 */
#define JB_DEFINE_ACCESSOR(JavaName,FieldsSpec,MethodsSpec) \
    xJB_DEFINE_FIELDS(FieldsSpec) \
    xJB_DEFINE_METHODS(MethodsSpec) \
    xJB_DEFINE_CALLBACKS(NoCallbacks) \
    xJB_DEFINE_CLASS_DESCRIPTOR(false,JavaName)


/** Defines Java bindings for the current wrapper class.
 * This macro implements member functions declared by JB_WRAPPER_CLASS()
 *  and also defines fields and methods that will be used to
 *  implement current wrapper class.
 *
 * \c JavaName is a fully-qualified name of Java class,
 *  see 12.3.2 "Class Descriptors" in JNI specification.
 *
 * \c FieldsSpec is either
 *  - Single identifier \c NoFields, when no fields are 
 *     specified, or
 *  - Identifier \c Fields followed by a number of field
 *     specs of form (\c Tag, \c Name, \c Descriptor).
 *
 * \c MethodSpec is either
 *  - Single identifier \c NoMethods, when no methods are 
 *     specified, or
 *  - Identifier \c Methods followed by a number of method
 *     specs of form (\c Tag, \c Name, \c Descriptor).
 *
 * Where
 * - \c Tag is an arbitrary tag identifying field or method.
 *    It is used when accessing fields (JB_GET()/JB_SET())
 *    or calling methods (JB_CALL() and the like).
 * - \c Name is field or method name as defined in Java class.
 *    It doesn'tinclude any type or signature information.
 *    To indicate a static field or method start name with a 
 *    plus sign (e.g. \c "+myStaticField", \c "+myStaticMethod").
 * - \c Descriptor is field/method descriptor.
 *    See 12.3.3 "Field Descriptors" or 12.3.4 "Method Descriptors"
 *    in JNI specification.
 *
 * \code
 * *** Java ***
 * public double factor;
 * private static Object[] cache;
 *
 * public double scale(int value) {...}
 * public static boolean isCached(Object value) {...}
 *
 * *** C++ ***
 * JB_DEFINE_WRAPPER_CLASS(
 *   "com/my/SomeClass"
 *   ,
 *   Fields
 *   (FactorTag,"factor","D")
 *   (CacheTag,"+cache","[Ljava/lang/Object;")
 *   ,
 *   Methods
 *   (ScaleTag,"scale","(I)D")
 *   (IsCachedTag,"+isCached","(Ljava/lang/Object;)Z")
 * )
 *
 * JB_SET_THIS(DoubleField,FactorTag,factor);
 * java::PObjectArray cache=java::PObjectArray::Wrap(
 *   JB_GET_STATIC(ObjectMethod,CacheTag));
 *
 * double scaled=JB_CALL_THIS(DoubleMethod,ScaleTag,value);
 * bool isCached=JB_CALL_STATIC(BoolMethod,IsCachedTag,object);
 * \endcode
 */
#define JB_DEFINE_WRAPPER_CLASS(JavaName,Fields,Methods) \
    JB_DEFINE_ACCESSOR(JavaName,Fields,Methods) \
    xJB_IMPLEMENT_STATICCLASS()


#ifndef JB_NATIVE_INSTANCE_NAME

/** Name of the \p int variable in Java class that holds
 *  instance pointer; default is \c nativeInstance.
 */
#define JB_NATIVE_INSTANCE_NAME "nativeInstance"

#endif


/** Defines Java bindings for the current live class.
 *
 * This macro implements member functions declared by 
 *  JB_LIVE_CLASS() and also defines fields, methods and 
 *  callbacks that will be used to implement current live class.
 * 
 * See JB_DEFINE_WRAPPER_CLASS() for description of \c JavaName, 
 *  \c FieldsSpec and \c MethodsSpec.
 *
 * Callback is a member function of a live class which will
 *  be called when corresponding Java method is called.
 *  Java methods which are bound to callbacks must be marked 
 *  with 'native' keywords.
 *
 * \c CallbacksSpec is either
 *  - Single identifier \c NoCallbacks, when no callbacks are
 *     specified, or
 *  - Identifier \c Callbacks followed by a number of callback
 *     specs of form (\c Binding, \c Name, Descriptor), where
 *    - \c Binding specifies function that will be bound to the
 *       Java method (see below).
 *    - \c Name is a \c name of Java method. No indication is 
 *       required for the static methods.
 *    - \c Descriptor is a Java method descriptor.
 *       See 12.3.4 "Methods Descriptors" in JNI specification.
 *
 * \c Binding has the form \c Method(MethodName) where 
 *  \c MethodName identifies method in the current live class.
 *
 * Facts about callback methods:
 * - Callback methods are subject to the restrictions specified
 *    by jni::VarArgs.
 * - Currently callback methods can't be \c const (will be fixed in
 *    next release).
 * - C++ exceptions thrown from callback methods are translated to
 *    the Java exceptions by jni::TranslateCppException().
 * - Callback methods can be static. In this case method must
 *    include extra argument of object type. When static callback
 *    method is bound to non-static Java method that extra argument
 *    is a Java object which called the callback, otherwise it is 
 *    Java class callback is bound to.
 *
 * \code
 * *** Java ***
 * public native String[] parse(String input);
 * private static native Object loadObject(int cookie);
 *
 * *** C++ ***
 * java::PStringArray Parse(java::PString);
 * // Note extra java::PClass argument.
 * static java::PObject LoadObject(java::PClass,int cookie);
 *
 * JB_DEFINE_LIVE_CLASS(
 *   ...
 *   ,
 *   Callbacks
 *   (Method(Parse),"parse","(Ljava/lang/String;)[Ljava/lang/String;")
 *   (Method(LoadObject),"loadObject","(I)Ljava/lang/Object;")
 * )
 * \endcode
 */
#define JB_DEFINE_LIVE_CLASS(JavaName,FieldsSpec,MethodsSpec,CallbacksSpec) \
    xJB_DEFINE_FIELDS(xJB_ADD_INSTANCE_FIELD(FieldsSpec)) \
    xJB_DEFINE_METHODS(MethodsSpec) \
    xJB_DEFINE_CALLBACKS(xJB_ADD_FINALIZE_CALLBACK(CallbacksSpec)) \
    xJB_DEFINE_CLASS_DESCRIPTOR(true,JavaName) \
    xJB_IMPLEMENT_FINALIZE() \
    xJB_IMPLEMENT_INSTANCE_FIELD_ID() \
    xJB_IMPLEMENT_STATICCLASS()


/** Initializes data structures defined by \c JB_DEFINE_ macros.
 *
 * Initialization consists of getting Java class by specified fully
 *  qualified name, getting ids for all fields and methods and binding
 *  callbacks. Any error during initialization treated as fatal error
 *  and jni::FatalError() is called.
 *
 * Generally you don't need to call this method, it is implicitly 
 *  called when needed. The only exception is when you need to bind
 *  callbacks to the Java class.
 */
#define JB_INIT_CLASS() \
    xJB_INIT_CLASS()


/** Returns java::PClass for the current class.
 */
#define JB_GET_CLASS() \
    xJB_GET_CLASS()


/** Returns \c jmethodID for the tag.
 */
#define JB_GET_METHOD_ID(MethodTag) \
    xJB_GET_METHOD_ID(xJB_METHOD_INDICES::MethodTag)


/** Returns \c fieldID for the tag.
 */
#define JB_GET_FIELD_ID(FieldTag) \
    xJB_GET_FIELD_ID(xJB_FIELD_INDICES::FieldTag)


/** Allocates new instance of Java class and calls constructor
 *  method identified by \c ConstructorTag.
 * Constructors are ordinary methods having name of \c "<init>" and
 *  void (\c V) return type.
 */
#define JB_NEW(ConstructorTag,...) \
    jni::NewObject( \
        *JB_GET_CLASS(), \
        JB_GET_METHOD_ID(ConstructorTag),##__VA_ARGS__)


/** Calls method identified by \c MethodTag on \c object.
 *
 * \c WhichMethod specifies return type of the Java method identified
 *  by \c MethodTag. For example, if \c MethodTag refers to method
 *  returning \c int \c WhichMethod must be \c IntMethod.
 *
 * Possible values of \c WhichMethod:
 * - \c VoidMethod, for methods that return nothing.
 * - \c BooleanMethod, for Java methods returning \c boolean.
 * - \c BoolMethod, for Java methods returning \c boolean;
 *      return value is converted to \c bool.
 * - \c ByteMethod, for Java methods returning \c byte.
 * - \c ShortMethod, for Java methods returning \c short.
 * - \c CharMethod, for Java methods returning \c char.
 * - \c IntMethod, for Java methods returning \c int.
 * - \c LongMethod, for Java methods returning \c long.
 * - \c FloatMethod, for Java methods returning \c float.
 * - \c DoubleMethod, for Java methods returning \c double.
 * - \c ObjectMethod, for Java methods returning objects or
 *      arrays.
 *
 * When calling \c ObjectMethod use java::ObjectPointer::Wrap() to
 *  convert return value to an instance of concrete class:
 * \code
 * typedef java::ObjectPointer<MyClass> PMyClass;
 * PMyClass my=PMyClass::Wrap(JB_CALL(ObjectMethod,object,MethodTag));
 * my->DoSomething();
 * \endcode
 *
 * This macro expands to \c jni::Call##WhichMethod().
 */
#define JB_CALL(WhichMethod,object,MethodTag,...) \
    jni::Call##WhichMethod( \
        object, \
        JB_GET_METHOD_ID(MethodTag),##__VA_ARGS__)

/** Calls method identified by \c MethodTag on the current object;
 *  use in member functions.
 *
 * See also: JB_CALL().
 */
#define JB_CALL_THIS(WhichMethod,MethodTag,...) \
    JB_CALL(WhichMethod,xJB_THIS,MethodTag,##__VA_ARGS__)


/** Nonvirtually calls method identified by \c MethodTag
 *  on \c object.
 * See JB_CALL() for \c WhichMethod values.
 *
 * This macro expands to \c jni::CallNonvirtual##WhichMethod().
 */
#define JB_NVCALL(WhichMethod,object,MethodTag,...) \
    jni::CallNonvirtual##WhichMethod( \
        object, *JB_GET_CLASS(), \
        JB_GET_METHOD_ID(MethodTag),##__VA_ARGS__)

/** Nonvirtually calls method identified by \c MethodTag
 *  on the current object; use in member functions.
 *
 * See also JB_NVCALL().
 */
#define JB_NVCALL_THIS(WhichMethod,MethodTag,...) \
    JB_NVCALL(WhichMethod,xJB_THIS,MethodTag,##__VA_ARGS__)


/** Calls static method identified by \c MethodTag.
 * See JB_CALL() for \c WhichMethod values.
 *
 * This macro expands to \c jni::CallStatic##WhichMethod().
 */
#define JB_CALL_STATIC(WhichMethod,MethodTag,...) \
    jni::CallStatic##WhichMethod( \
        *JB_GET_CLASS(), \
        JB_GET_METHOD_ID(MethodTag),##__VA_ARGS__)


/** Retrieves value of the field identified by \c FieldTag from
 *  \c object.
 *
 * \c WhichField specifies type of the Java field identified by
 *  \c FieldTag. For example, if \c FieldTag refers to field
 *  of type \c long \c WhichField must be \c LongField.
 *
 * Possible values of \c WhichField:
 * - \c BooleanField, for accessing \c boolean fields.
 * - \c BoolField, for accessing \c boolean fields;
 *      value is converted to/from \c bool.
 * - \c ByteField, for accessing \c byte fields.
 * - \c ShortField, for accessing \c short fields.
 * - \c CharField, for accessing \c char fields.
 * - \c IntField, for accessing \c int fields.
 * - \c LongField, for accessing \c long fields.
 * - \c FloatField, for accessing \c float fields.
 * - \c DoubleField, for accessing \c double fields.
 * - \c ObjectField, for accessing object or array fields.
 *
 * When retrieving \c ObjectField use java::ObjectPointer::Wrap() to
 *  convert return value to an instance of concrete class:
 * \code
 * typedef java::ObjectPointer<MyClass> PMyClass;
 * PMyClass my=PMyClass::Wrap(JB_GET(ObjectMethod,object,FieldTag));
 * my->DoSomething();
 * \endcode
 *
 * This macro expands to \c jni::Get##WhichField(object).
 */
#define JB_GET(WhichField,object,FieldTag) \
    jni::Get##WhichField( \
        object, \
        JB_GET_FIELD_ID(FieldTag))

/** Retrieves value of the field identified by \c FieldTag from
 *  the current object; use in member functions.
 *
 * See also: JB_GET().
 */
#define JB_GET_THIS(WhichField,FieldTag) \
    JB_GET(WhichField,xJB_THIS,FieldTag)


/** Sets value to the field identified by \c FieldTag on \c object.
 * See JB_GET() for \c WhichField values.
 *
 * This macro expands to \c jni::Set##WhichField(object,value).
 */
#define JB_SET(WhichField,object,FieldTag,value) \
    jni::Set##WhichField( \
        object, \
        JB_GET_FIELD_ID(FieldTag),value)

/** Sets value to the field identified by \c FieldTag on the current
 *  object; use in member functions.
 * See JB_GET() for \c WhichField values.
 *
 * See also: JB_SET().
 */
#define JB_SET_THIS(WhichField,FieldTag,value) \
    JB_SET(WhichField,xJB_THIS,FieldTag,value)


/** Retrieves value from the static field identified by \c FieldTag.
 * See JB_GET() for \c WhichField values.
 *
 * This macro expands to \c jni::GetStatic##WhichField(object).
 */
#define JB_GET_STATIC(WhichField,FieldTag) \
    jni::GetStatic##WhichField( \
        *JB_GET_CLASS(), \
        JB_GET_FIELD_ID(FieldTag))


/** Sets value to the static field identified by \c FieldTag.
 * See JB_GET() for \c WhichField values.
 *
 * This macro expands to \c jni::SetStatic##WhichField(object,value).
 */
#define JB_SET_STATIC(WhichField,FieldTag,value) \
    jni::SetStatic##WhichField( \
        *JB_GET_CLASS(), \
        JB_GET_FIELD_ID(FieldTag),value)

///////////////////////////////////////////////////////////////////// implementation

///////////////////////////////////////////////// java::Class

/* Forward declaration of java::Class.
 * Doxygen should't see it.
 */

END_NAMESPACE(jb)

#ifndef ONLY_FOR_DOXYGEN

BEGIN_NAMESPACE(java)

class Class;
typedef ObjectPointer<Class> PClass;

END_NAMESPACE(java)

#endif // ONLY_FOR_DOXYGEN

BEGIN_NAMESPACE(jb)

///////////////////////////////////////////////// methods

#define xJB_G_METHODS \
    xJB_JOIN3(g_jb,JB_CURRENT_CLASS,Methods)

#define xJB_METHOD_INDICES \
    xJB_JOIN3(JB,JB_CURRENT_CLASS,MethodIndices)

#define xJB_DEFINE_METHODS(Methods) \
    namespace { \
    struct xJB_METHOD_INDICES { \
        enum ID { \
            xJB_END(xJB_JOIN(xJB_PUT_TAG_,Methods)) \
        }; \
    }; \
    static ::jb::MethodDescriptor xJB_G_METHODS[]={ \
        xJB_END(xJB_JOIN(xJB_PUT_NAME_SIGNATURE_,Methods)) \
        {0} \
    }; \
    }

#define xJB_PUT_TAG_Methods(...) \
    xJB_PUT_TAG(__VA_ARGS__)xJB_PUT_TAG_0_
#define xJB_PUT_NAME_SIGNATURE_Methods(...) \
    xJB_PUT_NAME_SIGNATURE(__VA_ARGS__)xJB_PUT_NAME_SIGNATURE_1_
#define xJB_PUT_TAG_NoMethodsEND
#define xJB_PUT_NAME_SIGNATURE_NoMethodsEND

#define xJB_GET_METHOD_ID \
    xJB_JOIN3(JBGet,JB_CURRENT_CLASS,MethodID)
#define xJB_IMPLEMENT_GET_METHOD_ID() \
    static jmethodID xJB_GET_METHOD_ID(int index) { \
        xJB_INIT_CLASS(); \
        return xJB_G_METHODS[index].id; \
    }

///////////////////////////////////////////////// fields
    
#define xJB_G_FIELDS \
    xJB_JOIN3(g_jb,JB_CURRENT_CLASS,Fields)

#define xJB_FIELD_INDICES \
    xJB_JOIN3(JB,JB_CURRENT_CLASS,FieldIndices)

#define xJB_DEFINE_FIELDS(Fields) \
    namespace { \
    struct xJB_FIELD_INDICES { \
        enum ID { \
            xJB_END(xJB_JOIN(xJB_PUT_TAG_,Fields)) \
        }; \
    }; \
    static ::jb::FieldDescriptor xJB_G_FIELDS[]={ \
        xJB_END(xJB_JOIN(xJB_PUT_NAME_SIGNATURE_,Fields)) \
        {0} \
    }; \
    }

#define xJB_PUT_TAG_Fields(...) \
    xJB_PUT_TAG(__VA_ARGS__)xJB_PUT_TAG_0_
#define xJB_PUT_NAME_SIGNATURE_Fields(...) \
    xJB_PUT_NAME_SIGNATURE(__VA_ARGS__)xJB_PUT_NAME_SIGNATURE_1_
#define xJB_PUT_TAG_NoFieldsEND
#define xJB_PUT_NAME_SIGNATURE_NoFieldsEND

#define xJB_INSTANCE_FIELD \
    InstanceField
#define xJB_INSTANCE_FIELD_DECLARATION \
    (xJB_INSTANCE_FIELD,JB_NATIVE_INSTANCE_NAME,"I")
#define xJB_ADD_INSTANCE_FIELD(Fields) \
    xJB_EVAL(xJB_ADD_INSTANCE_FIELD_##Fields)
#define xJB_ADD_INSTANCE_FIELD_NoFields \
    Fields xJB_INSTANCE_FIELD_DECLARATION
#define xJB_ADD_INSTANCE_FIELD_Fields \
    Fields xJB_INSTANCE_FIELD_DECLARATION

#define xJB_GET_FIELD_ID \
    xJB_JOIN3(JBGet,JB_CURRENT_CLASS,FieldID)
#define xJB_IMPLEMENT_GET_FIELD_ID() \
    static jfieldID xJB_GET_FIELD_ID(int index) { \
        xJB_INIT_CLASS(); \
        return xJB_G_FIELDS[index].id; \
    }

///////////////////////////////////////////////// callbacks

#define xJB_IMPLEMENT_FINALIZE() \
    void JB_CURRENT_CLASS::Finalizer(JNIEnv*,jobject thiz) { \
        Object* instance=GetLiveInstance(thiz,GetInstanceFieldID()); \
        if (instance) { \
            instance->Release(); \
        } \
        if (xJB_G_CLASS.superFinalizer) { \
            jni::LObject superclass=jni::GetSuperclass(GetTypeClass()); \
            jni::GetEnv()->CallNonvirtualVoidMethod( \
                thiz, \
                (jclass)superclass.GetJObject(), \
                xJB_G_CLASS.superFinalizer); \
        } \
    }

#define xJB_CALLBACKS_ACCESSOR_FOR(ClassName) \
    xJB_JOIN3(JB,ClassName,CallbackAccessor)
#define xJB_CALLBACKS_ACCESSOR \
    xJB_CALLBACKS_ACCESSOR_FOR(JB_CURRENT_CLASS)

#define xJB_DEFINE_CALLBACKS_ACCESSOR \
    class xJB_CALLBACKS_ACCESSOR { \
    public: \
        static const JNINativeMethod Callbacks[]; \
    }
#define xJB_G_CALLBACKS \
    xJB_CALLBACKS_ACCESSOR::Callbacks
    
#define xJB_DEFINE_CALLBACKS(Callbacks) \
    xJB_DEFINE_NATIVEMETHOD_TAGS(Callbacks) \
    xJB_DEFINE_CALLBACKS_ACCESSOR; \
    const JNINativeMethod xJB_G_CALLBACKS[]={ \
        xJB_END(xJB_JOIN(xJB_PUT_NATIVEMETHOD_,Callbacks)) \
        {0} \
    };

/* Adds finalize callback to the callbacks list.
 * Handles NoCallbacks spec.
 */
#define xJB_FINALIZE_CALLBACK_DECLARATION \
    (Native(&JB_CURRENT_CLASS::Finalizer),"finalize","()V")
#define xJB_ADD_FINALIZE_CALLBACK(Callbacks) \
    xJB_EVAL(xJB_ADD_FINALIZE_CALLBACK_##Callbacks)
#define xJB_ADD_FINALIZE_CALLBACK_Callbacks \
    Callbacks xJB_FINALIZE_CALLBACK_DECLARATION
#define xJB_ADD_FINALIZE_CALLBACK_NoCallbacks \
    Callbacks xJB_FINALIZE_CALLBACK_DECLARATION


#define xJB_PUT_NATIVEMETHOD_Callbacks(...) \
    xJB_PUT_NATIVEMETHOD(__VA_ARGS__)xJB_PUT_NATIVEMETHOD_0_
#define xJB_PUT_NATIVEMETHOD_NoCallbacksEND

#define xJB_PUT_NATIVEMETHOD(Implementation,Name,Signature) \
    {(char*)Name,(char*)Signature,(void*)xJB_ACCESS_CALLBACK(Implementation)},
#define xJB_PUT_NATIVEMETHOD_0_(...) \
    xJB_PUT_NATIVEMETHOD(__VA_ARGS__)xJB_PUT_NATIVEMETHOD_1_
#define xJB_PUT_NATIVEMETHOD_1_(...) \
    xJB_PUT_NATIVEMETHOD(__VA_ARGS__)xJB_PUT_NATIVEMETHOD_0_
#define xJB_PUT_NATIVEMETHOD_0_END
#define xJB_PUT_NATIVEMETHOD_1_END

#define xJB_ACCESS_CALLBACK(Implementation) \
    xJB_EVAL(xJB_ACCESS_CALLBACK##_##Implementation)
#define xJB_ACCESS_CALLBACK_Native(Function) \
    Function
#define xJB_ACCESS_CALLBACK_Method(Function) \
    ::jb::ConvertCC< \
        xJB_UNIQUE_NUMBER+xJB_NATIVEMETHOD_TAGS::Function \
    >(&JB_CURRENT_CLASS::Function)

///////////////////////////////////////////////// class

#define xJB_G_CLASS \
    xJB_JOIN3(g_jb,JB_CURRENT_CLASS,Class)

#define xJB_DEFINE_CLASS_DESCRIPTOR(IsLive,JavaName) \
    namespace { \
    static ::jb::ClassDescriptor xJB_G_CLASS={ \
        IsLive, \
        JavaName, \
        xJB_G_METHODS, \
        xJB_G_FIELDS, \
        xJB_G_CALLBACKS \
    }; \
    xJB_IMPLEMENT_INIT_CLASS(); \
    xJB_IMPLEMENT_GET_CLASS(); \
    xJB_IMPLEMENT_GET_METHOD_ID(); \
    xJB_IMPLEMENT_GET_FIELD_ID(); \
    }

#define xJB_INIT_CLASS() \
    pthread_once(&xJB_G_CLASS_ONCE_INIT,xJB_DO_INIT_CLASS)
#define xJB_G_CLASS_ONCE_INIT \
    xJB_JOIN3(g_jb,JB_CURRENT_CLASS,ClassOnceInit)
#define xJB_DO_INIT_CLASS \
    xJB_JOIN3(JBDoInit,JB_CURRENT_CLASS,Class)
#define xJB_IMPLEMENT_INIT_CLASS() \
    static pthread_once_t xJB_G_CLASS_ONCE_INIT=PTHREAD_ONCE_INIT; \
    static void xJB_DO_INIT_CLASS() { \
        ::jb::InitClassDescriptor(xJB_G_CLASS); \
    }

#define xJB_GET_CLASS \
    xJB_JOIN3(JBGet,JB_CURRENT_CLASS,Class)
#define xJB_IMPLEMENT_GET_CLASS() \
    static java::Class* xJB_GET_CLASS() { \
        xJB_INIT_CLASS(); \
        return xJB_G_CLASS.clazz; \
    }
    
#define xJB_IMPLEMENT_INSTANCE_FIELD_ID() \
    jfieldID JB_CURRENT_CLASS::GetInstanceFieldID() { \
        return JB_GET_FIELD_ID(InstanceField); \
    }

#define xJB_IMPLEMENT_STATICCLASS() \
    java::PClass JB_CURRENT_CLASS::GetTypeClass() { \
        return JB_GET_CLASS(); \
    }

///////////////////////////////////////////////// common

#define xJB_PUT_TAG(Tag,Name,Signature) \
    Tag,
#define xJB_PUT_TAG_0_(...) \
    xJB_PUT_TAG(__VA_ARGS__)xJB_PUT_TAG_1_
#define xJB_PUT_TAG_1_(...) \
    xJB_PUT_TAG(__VA_ARGS__)xJB_PUT_TAG_0_
#define xJB_PUT_TAG_0_END
#define xJB_PUT_TAG_1_END

#define xJB_PUT_NAME_SIGNATURE(Tag,Name,Signature) \
    {Name,Signature},
#define xJB_PUT_NAME_SIGNATURE_0_(...) \
    xJB_PUT_NAME_SIGNATURE(__VA_ARGS__)xJB_PUT_NAME_SIGNATURE_1_
#define xJB_PUT_NAME_SIGNATURE_1_(...) \
    xJB_PUT_NAME_SIGNATURE(__VA_ARGS__)xJB_PUT_NAME_SIGNATURE_0_
#define xJB_PUT_NAME_SIGNATURE_0_END
#define xJB_PUT_NAME_SIGNATURE_1_END

#define xJB_THIS \
    java::ObjectPointer<const JB_CURRENT_CLASS>(this)

///////////////////////////////////////////////// helpers

#define xJB_EVAL(...) __VA_ARGS__

#define xJB_JOIN(a,b) xJB_DO_JOIN(a,b)
#define xJB_DO_JOIN(a,b) a##b

#define xJB_JOIN3(a,b,c) xJB_DO_JOIN3(a,b,c)
#define xJB_DO_JOIN3(a,b,c) a##b##c

#define xJB_END(...) xJB_DO_END(__VA_ARGS__)
#define xJB_DO_END(...) __VA_ARGS__##END

///////////////////////////////////////////////// structs

struct MethodDescriptor;
struct FieldDescriptor;

struct ClassDescriptor {
    // Statically initialized.
    bool isLive;
    const char* className;
    MethodDescriptor* methods;
    FieldDescriptor* fields;
    const JNINativeMethod* callbacks;
    
    // Initialized in runtime.
    jmethodID superFinalizer;
    java::Class* clazz;
};

struct MethodDescriptor {
    const char* name;
    const char* signature;
    jmethodID id;
};

struct FieldDescriptor {
    const char* name;
    const char* signature;
    jfieldID id;
};

void InitClassDescriptor(ClassDescriptor& descriptor);

///////////////////////////////////////////////////////////////////// ConvertCC

/* Everything below are implementation details of ConvertCC
 *  function which converts class member function to C callback ready
 *  for JNIEnv::RegisterNatives() method. Class member function can use
 *  JNIpp types like java::PIntArray instead of raw JNI objects like 
 *  jintArray. ConvertCC is used in implementation of Method() callback 
 *  spec.
 *
 * See documentation for JB_DEFINE_LIVE_CLASS() macro.
 */

///////////////////////////////////////////////// macros

/* Calls ConvertCC() on the member function FunctionName
 *  in the current class.
 */
#define xJB_CONVERTCC(FunctionName) \
    jb::ConvertCC< \
        xJB_CONVERTCC_UNIQUE_NUMBER(FunctionName) \
    >(&JB_CURRENT_CLASS::FunctionName)

/* For proper use of ConvertCC we need unique number for each
 *  instantiation. __COUNTER__ does the job. However, for
 *  compilers that don't support __COUNTER__ we have to use
 *  __LINE__ and add a unique number since __LINE__ will be
 *  the same for each callback defined in a JB_DEFINE_LIVE_CLASS().
 */
#ifdef _MSC_VER

/* __COUNTER__ is available. Nice. */

#define xJB_UNIQUE_NUMBER __COUNTER__

#define xJB_DEFINE_NATIVEMETHOD_TAGS(Callbacks)
#define xJB_CONVERTCC_UNIQUE_NUMBER(FunctionName) __COUNTER__

#else

/* __COUNTER__ is available, use __LINE__ and jump
 *  through hoops.
 */

#define xJB_UNIQUE_NUMBER __LINE__

#define xJB_PUT_NATIVEMETHOD_TAG_Callbacks(...) \
    xJB_PUT_NATIVEMETHOD_TAG(__VA_ARGS__)xJB_PUT_NATIVEMETHOD_TAG_0_
#define xJB_PUT_NATIVEMETHOD_TAG_NoCallbacksEND

#define xJB_PUT_NATIVEMETHOD_TAG_Native(Function)
#define xJB_PUT_NATIVEMETHOD_TAG_Method(Function) \
    Function,
#define xJB_PUT_NATIVEMETHOD_TAG(Implementation,Name,Signature) \
    xJB_EVAL(xJB_PUT_NATIVEMETHOD_TAG##_##Implementation)
#define xJB_PUT_NATIVEMETHOD_TAG_0_(...) \
    xJB_PUT_NATIVEMETHOD_TAG(__VA_ARGS__)xJB_PUT_NATIVEMETHOD_TAG_1_
#define xJB_PUT_NATIVEMETHOD_TAG_1_(...) \
    xJB_PUT_NATIVEMETHOD_TAG(__VA_ARGS__)xJB_PUT_NATIVEMETHOD_TAG_0_
#define xJB_PUT_NATIVEMETHOD_TAG_0_END
#define xJB_PUT_NATIVEMETHOD_TAG_1_END

#define xJB_NATIVEMETHOD_TAGS \
    xJB_JOIN3(JB,JB_CURRENT_CLASS,NativeMethodTags)

#define xJB_DEFINE_NATIVEMETHOD_TAGS(Callbacks) \
    namespace { \
    struct xJB_NATIVEMETHOD_TAGS { \
        enum ID { \
            xJB_END(xJB_JOIN(xJB_PUT_NATIVEMETHOD_TAG_,Callbacks)) \
        }; \
    }; \
    }


#define xJB_CONVERTCC_UNIQUE_NUMBER(FunctionName) \
    __LINE__+xJB_NATIVEMETHOD_TAGS::Function

#endif

///////////////////////////////////////////////// CCTypeConverter

/* CCTypeConverter converts raw JNI types like jobject
 *  to/from JNIpp equivalents like java::PObject or
 *  jni::LObject.
 */

template <class T>
class CCTypeConverter {
    // All supported types should implement the 
    //  following members:
    //
    // typedef X NativeType;
    // static NativeType ToNative(T value);
    // static T FromNative(NativeType value);
};

inline jboolean ConvertBool(bool value) {
    return value?JNI_TRUE:JNI_FALSE;
}
inline bool ConvertBool(jboolean value) {
    return value==JNI_TRUE;
}

template <>
class CCTypeConverter<bool> {
public:
    typedef jboolean NativeType;
    static jboolean ToNative(bool value) {
        return ConvertBool(value);
    }
    static bool FromNative(jboolean value) {
        return ConvertBool(value);
    }
};

template <>
class CCTypeConverter<jni::AbstractObject const&> {
public:
    typedef jobject NativeType;
    static jobject ToNative(jni::AbstractObject const& value) {
        return jni::GetEnv()->NewLocalRef(value.GetJObject());
    }
    static jni::LObject FromNative(jobject value) {
        return jni::LObject::Wrap(value);
    }
};

template <>
class CCTypeConverter<jni::LObject const&>:
    public CCTypeConverter<jni::AbstractObject const&>
{
};

template <class T>
class CCTypeConverter<java::ObjectPointer<T> > {
public:
    typedef jobject NativeType;
    static jobject ToNative(java::ObjectPointer<T> value) {
        return CCTypeConverter<jni::AbstractObject const&>::ToNative(value);
    }
    static java::ObjectPointer<T> FromNative(jobject value) {
        return java::ObjectPointer<T>::Wrap(value);
    }
};

#define xJB_SPECIALIZE_CCTYPECONVERTER(JType) \
    template <> \
    class CCTypeConverter<JType> { \
    public: \
        typedef JType NativeType; \
        static JType ToNative(JType value) { \
            return value; \
        } \
        static JType FromNative(JType value) { \
            return value; \
        } \
    };

xJB_SPECIALIZE_CCTYPECONVERTER(jboolean)
xJB_SPECIALIZE_CCTYPECONVERTER(jbyte)
xJB_SPECIALIZE_CCTYPECONVERTER(jchar)
xJB_SPECIALIZE_CCTYPECONVERTER(jshort)
xJB_SPECIALIZE_CCTYPECONVERTER(jint)
xJB_SPECIALIZE_CCTYPECONVERTER(jlong)
xJB_SPECIALIZE_CCTYPECONVERTER(jfloat)
xJB_SPECIALIZE_CCTYPECONVERTER(jdouble)

#undef xJB_SPECIALIZE_CCTYPECONVERTER

///////////////////////////////////////////////// ConvertCC

#include "GenerateMacro.h"

#define xJB_GENERATOR_COMMA_CLASS_A(N) \
    ,class A##N
#define xJB_GENERATOR_A(N) \
    A##N
#define xJB_GENERATOR_COMMA_A(N) \
    ,xJB_GENERATOR_A(N)
#define xJB_GENERATOR_COMMA_NATIVETYPE_A(N) \
    ,typename CCTypeConverter<A##N>::NativeType a##N
#define xJB_GENERATOR_FROMNATIVE_A(N) \
    CCTypeConverter<A##N>::FromNative(a##N)
#define xJB_GENERATOR_COMMA_FROMNATIVE_A(N) \
    ,xJB_GENERATOR_FROMNATIVE_A(N)

/* When worn this macro gives +10 to PreprocessorVoodoo and
 *  +7 to TemplateMagic.
 */
#define xJB_GENERATE_CONVERTCC(N,D) \
    template <int U,class C,class R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    class CCHolder##N { \
    public: \
        static R(C::*Value)( \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_A,comma) \
        ); \
    }; \
    template <int U,class C,class R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    R(C::*CCHolder##N<U,C,R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
    >::Value)( \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_A,comma) \
    )=0; \
    template <int U,class C,class R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    class CCConverter##N { \
    public: \
        static typename CCTypeConverter<R>::NativeType NativeCallback( \
            JNIEnv*,jobject thiz \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_NATIVETYPE_A,empty) \
        ) { \
            java::ObjectPointer<C> instance= \
                java::ObjectPointer<C>::Wrap(thiz); \
            try { \
                return CCTypeConverter<R>::ToNative( \
                    ((*instance).*CCHolder##N<U,C,R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
                    >::Value)( \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_FROMNATIVE_A,comma) \
                    )); \
            } catch (...) { \
                jni::TranslateCppException(); \
                return 0; \
            } \
        } \
    }; \
    template <int U,class C \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    class CCConverter##N<U,C,void \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
    > { \
    public: \
        static void NativeCallback( \
            JNIEnv*,jobject thiz \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_NATIVETYPE_A,empty) \
        ) { \
            java::ObjectPointer<C> instance= \
                java::ObjectPointer<C>::Wrap(thiz); \
            try { \
                ((*instance).*CCHolder##N<U,C,void \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
                    >::Value)( \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_FROMNATIVE_A,comma) \
                    ); \
            } catch (...) { \
                jni::TranslateCppException(); \
            } \
        } \
    }; \
    template <int U,class C,class R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    inline void* ConvertCC(R(C::*callback)( \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_A,comma) \
    )) { \
        if (CCHolder##N<U,C,R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
        >::Value) { \
            jni::FatalError("ConvertCC's magic failed!"); \
        } \
        CCHolder##N<U,C,R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
        >::Value=callback; \
        return (void*)(&CCConverter##N<U,C,R \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
        >::NativeCallback); \
    }

/* Hooray, another preprocessor artifact! Combine with the previous
 *  one and become invincible to fire spells.
 */
#define xJB_GENERATE_CONVERTCC_STATIC(N,D) \
    template <int U,class R,class O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    class StaticCCHolder##N { \
    public: \
        static R(*Value)(O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
        ); \
    }; \
    template <int U,class R,class O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    R(*StaticCCHolder##N<U,R,O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
    >::Value)(O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
    )=0; \
    template <int U,class R,class O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    class StaticCCConverter##N { \
    public: \
        static typename CCTypeConverter<R>::NativeType NativeCallback( \
            JNIEnv*,jobject object \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_NATIVETYPE_A,empty) \
        ) { \
            try { \
                return CCTypeConverter<R>::ToNative( \
                    (StaticCCHolder##N<U,R,O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
                    >::Value)(CCTypeConverter<O>::FromNative(object) \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_FROMNATIVE_A,empty) \
                    )); \
            } catch (...) { \
                jni::TranslateCppException(); \
                return 0; \
            } \
        } \
    }; \
    template <int U,class O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    class StaticCCConverter##N<U,void,O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
    > { \
    public: \
        static void NativeCallback( \
            JNIEnv*,jobject object \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_NATIVETYPE_A,empty) \
        ) { \
            try { \
                (*StaticCCHolder##N<U,void,O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
                    >::Value)(CCTypeConverter<O>::FromNative(object) \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_FROMNATIVE_A,empty) \
                    ); \
            } catch (...) { \
                jni::TranslateCppException(); \
            } \
        } \
    }; \
    template <int U,class R,class O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_CLASS_A,empty) \
    > \
    inline void* ConvertCC(R(*callback)(O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
    )) { \
        if (StaticCCHolder##N<U,R,O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
        >::Value) { \
            jni::FatalError("ConvertCC's magic failed!"); \
        } \
        StaticCCHolder##N<U,R,O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
        >::Value=callback; \
        return (void*)(&StaticCCConverter##N<U,R,O \
xJNIPP_GENERATE_ARGS(N,xJB_GENERATOR_COMMA_A,empty) \
        >::NativeCallback); \
    }

template <int U,class R>
inline void* ConvertCC(R(*callback)()) {
    char readme[U-INT_MIN];
    /* If you get here this means that you tried to use static callback
     *  with zero arguments. First argument of a static callback is 
     *  always present and is of object type (const jni::AbstractObject&).
     */
}

/* If you get error "no type named 'NativeType' in 'class jb::CCTypeConverter<TYPE>"
 *  this means that your member callback is using unsupported TYPE in arguments or 
 *  return value. See documentation for JB_DEFINE_LIVE_CLASS() for the list
 *  of supported types.
 *
 * The following macros generate ConvertCC functions which support
 *  callbacks with up to xJNIPP_GENERATE_MAX_N arguments.
 */
xJNIPP_GENERATE(0,xJNIPP_GENERATE_MAX_N,xJB_GENERATE_CONVERTCC,)
xJNIPP_GENERATE(0,xJNIPP_GENERATE_MAX_N,xJB_GENERATE_CONVERTCC_STATIC,)

/* Cleanup */

#undef xJB_GENERATOR_COMMA_CLASS_A
#undef xJB_GENERATOR_A
#undef xJB_GENERATOR_COMMA_A
#undef xJB_GENERATOR_COMMA_NATIVETYPE_A
#undef xJB_GENERATOR_FROMNATIVE_A
#undef xJB_GENERATOR_COMMA_FROMNATIVE_A

#undef xJB_GENERATE_CONVERTCC
#undef xJB_GENERATE_CONVERTCC_STATIC

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(jb)

#endif // _JNIPP_JAVABINDING_INCLUDED_
