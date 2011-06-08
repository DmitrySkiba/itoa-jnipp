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

#include "GenerateMacro.h"

///////////////////////////////////////////////////////////////////// details

///////////////////////////////////////////////// GetJValue

/* _GetJValue functions convert various types to 
 *  the corresponding JNI types.
 *
 * _GetJValue functions play important role - they decide
 *  which argument types are allowed in CallXXXMethod.
 */

inline jobject _GetJValue(const AbstractObject& object) {
    return object.GetJObject();
}

inline jboolean _GetJValue(bool value) {
    return value?JNI_TRUE:JNI_FALSE;
}

/* Shortcut for primitive types - we rely on convertion
 *  and don't supply 'unsigned' overloads.
 */
inline char _GetJValue(char value) {
    return value;
}
inline short _GetJValue(short value) {
    return value;
}
inline int _GetJValue(int value) {
    return value;
}
inline long _GetJValue(long value) {
    return value;
}


inline jlong _GetJValue(jlong value) {
    return value;
}
inline jfloat _GetJValue(jfloat value) {
    return value;
}
inline jdouble _GetJValue(jdouble value) {
    return value;
}

template <class T>
inline T* _GetJValue(T* p) {
    // If you landed here it means that you passed an object of
    //  an invalid type to vararg function (e.g. CallXXXMethod).
    //  See jni::VarArgs for the list of valid types.
    char invalid_argument_type[-1-sizeof(T)];
    return p;
}

///////////////////////////////////////////////// WrapJValue

/* _WrapJValue converts raw JNI values to the 
 *  appropriate JNIpp equivalents.
 */

inline LObject _WrapJValue(jobject value) {
    return LObject::WrapLocal(value);
}

inline bool _WrapJValue(jboolean value) {
    return value==JNI_TRUE;
}

/* All other return types are not converted.
 */
template <class T>
inline T _WrapJValue(T value) {
    return value;
}

///////////////////////////////////////////////////////////////////// methods

/* Functions that used by the macros below.
 * Each function takes raw JNI types, but returns JNIpp 
 *  values and handle exceptions.
 */

LObject _NewObject(const AbstractObject& clazz,jmethodID methodID,...);

LObject _CallObjectMethod(const AbstractObject& object,jmethodID methodID,...);
jboolean _CallBooleanMethod(const AbstractObject& object,jmethodID methodID,...);
bool _CallBoolMethod(const AbstractObject& object,jmethodID methodID,...);
jbyte _CallByteMethod(const AbstractObject& object,jmethodID methodID,...);
jchar _CallCharMethod(const AbstractObject& object,jmethodID methodID,...);
jshort _CallShortMethod(const AbstractObject& object,jmethodID methodID,...);
jint _CallIntMethod(const AbstractObject& object,jmethodID methodID,...);
jlong _CallLongMethod(const AbstractObject& object,jmethodID methodID,...);
jfloat _CallFloatMethod(const AbstractObject& object,jmethodID methodID,...);
jdouble _CallDoubleMethod(const AbstractObject& object,jmethodID methodID,...);
void _CallVoidMethod(const AbstractObject& object,jmethodID methodID,...);

LObject _CallNonvirtualObjectMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
jboolean _CallNonvirtualBooleanMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
bool _CallNonvirtualBoolMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
jbyte _CallNonvirtualByteMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
jchar _CallNonvirtualCharMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
jshort _CallNonvirtualShortMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
jint _CallNonvirtualIntMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
jlong _CallNonvirtualLongMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
jfloat _CallNonvirtualFloatMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
jdouble _CallNonvirtualDoubleMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);
void _CallNonvirtualVoidMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,...);

LObject _CallStaticObjectMethod(const AbstractObject& clazz,jmethodID methodID,...);
jboolean _CallStaticBooleanMethod(const AbstractObject& clazz,jmethodID methodID,...);
bool _CallStaticBoolMethod(const AbstractObject& clazz,jmethodID methodID,...);
jbyte _CallStaticByteMethod(const AbstractObject& clazz,jmethodID methodID,...);
jchar _CallStaticCharMethod(const AbstractObject& clazz,jmethodID methodID,...);
jshort _CallStaticShortMethod(const AbstractObject& clazz,jmethodID methodID,...);
jint _CallStaticIntMethod(const AbstractObject& clazz,jmethodID methodID,...);
jlong _CallStaticLongMethod(const AbstractObject& clazz,jmethodID methodID,...);
jfloat _CallStaticFloatMethod(const AbstractObject& clazz,jmethodID methodID,...);
jdouble _CallStaticDoubleMethod(const AbstractObject& clazz,jmethodID methodID,...);
void _CallStaticVoidMethod(const AbstractObject& clazz,jmethodID methodID,...);

///////////////////////////////////////////////// generators

#define xJNIPP_EVAL(...) __VA_ARGS__

#define xJNIPP_CALLMETHOD_RETURN_TYPE(RT,R,N) RT
#define xJNIPP_CALLMETHOD_RETURN(RT,R,N) R
#define xJNIPP_CALLMETHOD_NAME(RT,R,N) N
#define xJNIPP_CALLMETHOD_IMPL_NAME(RT,R,N) _##N

#define xJNIPP_CALLMETHOD_RETURN_VOID(X) X;
#define xJNIPP_CALLMETHOD_RETURN_WRAP(X) return _WrapJValue(X);

#define xJNIPP_GENERATE_COMMA_ARG_A(N) ,A##N a##N
#define xJNIPP_GENERATE_CLASS_A(N) class A##N
#define xJNIPP_GENERATE_COMMA_GETJVALUE_A(N) ,_GetJValue(a##N)

/* CallXXXMethod,CallStaticXXXMethod generator */

#define xJNIPP_GENERATE_CALLMETHOD(D) \
    inline xJNIPP_EVAL(xJNIPP_CALLMETHOD_RETURN_TYPE D) \
        xJNIPP_EVAL(xJNIPP_CALLMETHOD_NAME D) ( \
            const AbstractObject & target, \
            jmethodID methodID \
        ) { \
        xJNIPP_EVAL(xJNIPP_CALLMETHOD_RETURN D)( \
            xJNIPP_EVAL(xJNIPP_CALLMETHOD_IMPL_NAME D)(target,methodID \
            )) \
    }

#define xJNIPP_GENERATE_CALLMETHOD_TEMPLATES(N,D) \
    template < \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATE_CLASS_A,comma) \
    > \
    inline xJNIPP_EVAL(xJNIPP_CALLMETHOD_RETURN_TYPE D) \
        xJNIPP_EVAL(xJNIPP_CALLMETHOD_NAME D) ( \
            const AbstractObject & target, \
            jmethodID methodID \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATE_COMMA_ARG_A,empty) \
        ) { \
        xJNIPP_EVAL(xJNIPP_CALLMETHOD_RETURN D)( \
            xJNIPP_EVAL(xJNIPP_CALLMETHOD_IMPL_NAME D)(target,methodID \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATE_COMMA_GETJVALUE_A,empty) \
            )) \
    }

#define xJNIPP_IMPLEMENT_CALLMETHOD(ReturnType,Return,FunctionName) \
    xJNIPP_GENERATE_CALLMETHOD( \
        (ReturnType,xJNIPP_CALLMETHOD_RETURN_##Return,FunctionName) \
    ) \
    xJNIPP_GENERATE(1,xJNIPP_GENERATE_MAX_N, \
        xJNIPP_GENERATE_CALLMETHOD_TEMPLATES, \
        (ReturnType,xJNIPP_CALLMETHOD_RETURN_##Return,FunctionName) \
    )

/* CallNonvirtualXXXMethod generator */

#define xJNIPP_GENERATE_CALLNVMETHOD(D) \
    inline xJNIPP_EVAL(xJNIPP_CALLMETHOD_RETURN_TYPE D) \
        xJNIPP_EVAL(xJNIPP_CALLMETHOD_NAME D) ( \
            const AbstractObject & target, \
            const AbstractObject & clazz, \
            jmethodID methodID \
        ) { \
        xJNIPP_EVAL(xJNIPP_CALLMETHOD_RETURN D)( \
            xJNIPP_EVAL(xJNIPP_CALLMETHOD_IMPL_NAME D)(target,clazz,methodID \
            )) \
    }

#define xJNIPP_GENERATE_CALLNVMETHOD_TEMPLATES(N,D) \
    template < \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATE_CLASS_A,comma) \
    > \
    inline xJNIPP_EVAL(xJNIPP_CALLMETHOD_RETURN_TYPE D) \
        xJNIPP_EVAL(xJNIPP_CALLMETHOD_NAME D) ( \
            const AbstractObject & target, \
            const AbstractObject & clazz, \
            jmethodID methodID \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATE_COMMA_ARG_A,empty) \
        ) { \
        xJNIPP_EVAL(xJNIPP_CALLMETHOD_RETURN D)( \
            xJNIPP_EVAL(xJNIPP_CALLMETHOD_IMPL_NAME D)(target,clazz,methodID \
xJNIPP_GENERATE_ARGS(N,xJNIPP_GENERATE_COMMA_GETJVALUE_A,empty) \
            )) \
    }

#define xJNIPP_IMPLEMENT_CALLNVMETHOD(ReturnType,Return,FunctionName) \
    xJNIPP_GENERATE_CALLNVMETHOD( \
        (ReturnType,xJNIPP_CALLMETHOD_RETURN_##Return,FunctionName) \
    ) \
    xJNIPP_GENERATE(1,xJNIPP_GENERATE_MAX_N, \
        xJNIPP_GENERATE_CALLNVMETHOD_TEMPLATES, \
        (ReturnType,xJNIPP_CALLMETHOD_RETURN_##Return,FunctionName) \
    )

/* Implementation of jni::CallXXXMethod functions */

xJNIPP_IMPLEMENT_CALLMETHOD(LObject,WRAP,NewObject);

xJNIPP_IMPLEMENT_CALLMETHOD(LObject,WRAP,CallObjectMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jboolean,WRAP,CallBooleanMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(bool,WRAP,CallBoolMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jbyte,WRAP,CallByteMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jchar,WRAP,CallCharMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jshort,WRAP,CallShortMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jint,WRAP,CallIntMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jlong,WRAP,CallLongMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jfloat,WRAP,CallFloatMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jdouble,WRAP,CallDoubleMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(void,VOID,CallVoidMethod);

xJNIPP_IMPLEMENT_CALLMETHOD(LObject,WRAP,CallStaticObjectMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jboolean,WRAP,CallStaticBooleanMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(bool,WRAP,CallStaticBoolMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jbyte,WRAP,CallStaticByteMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jchar,WRAP,CallStaticCharMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jshort,WRAP,CallStaticShortMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jint,WRAP,CallStaticIntMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jlong,WRAP,CallStaticLongMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jfloat,WRAP,CallStaticFloatMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(jdouble,WRAP,CallStaticDoubleMethod);
xJNIPP_IMPLEMENT_CALLMETHOD(void,VOID,CallStaticVoidMethod);

xJNIPP_IMPLEMENT_CALLNVMETHOD(LObject,WRAP,CallNonvirtualObjectMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(jboolean,WRAP,CallNonvirtualBooleanMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(bool,WRAP,CallNonvirtualBoolMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(jbyte,WRAP,CallNonvirtualByteMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(jchar,WRAP,CallNonvirtualCharMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(jshort,WRAP,CallNonvirtualShortMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(jint,WRAP,CallNonvirtualIntMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(jlong,WRAP,CallNonvirtualLongMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(jfloat,WRAP,CallNonvirtualFloatMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(jdouble,WRAP,CallNonvirtualDoubleMethod);
xJNIPP_IMPLEMENT_CALLNVMETHOD(void,VOID,CallNonvirtualVoidMethod);

/* Cleanup */

#undef xJNIPP_EVAL
#undef xJNIPP_CALLMETHOD_RETURN_TYPE
#undef xJNIPP_CALLMETHOD_RETURN
#undef xJNIPP_CALLMETHOD_NAME
#undef xJNIPP_CALLMETHOD_IMPL_NAME
#undef xJNIPP_CALLMETHOD_RETURN_VOID
#undef xJNIPP_CALLMETHOD_RETURN_WRAP
#undef xJNIPP_GENERATE_COMMA_ARG_A
#undef xJNIPP_GENERATE_CLASS_A
#undef xJNIPP_GENERATE_COMMA_GETJVALUE_A

#undef xJNIPP_GENERATE_CALLMETHOD
#undef xJNIPP_GENERATE_CALLMETHOD_TEMPLATES
#undef xJNIPP_IMPLEMENT_CALLMETHOD

#undef xJNIPP_GENERATE_CALLNVMETHOD
#undef xJNIPP_GENERATE_CALLNVMETHOD_TEMPLATES
#undef xJNIPP_IMPLEMENT_CALLNVMETHOD


///////////////////////////////////////////////////////////////////// fields

/* Cast to jclass is a hack to be able to invoke static versions, which take
 *  jclass. Non-static versions take jobject and cast jclass back.
 */
#define xJNIPP_IMPLEMENT_GET_FIELD(ReturnType,Name) \
    inline ReturnType Name(const AbstractObject& target,jfieldID fieldID) { \
        return _WrapJValue(GetEnv()->Name((jclass)_GetJValue(target),fieldID)); \
    }
#define xJNIPP_IMPLEMENT_SET_FIELD(Name,ValueType) \
    inline void Name(const AbstractObject& target,jfieldID fieldID,ValueType value) { \
        GetEnv()->Name((jclass)_GetJValue(target),fieldID,_GetJValue(value)); \
    }

/* Implementation of jni::Get/SetXXXField functions. */

xJNIPP_IMPLEMENT_GET_FIELD(LObject,GetObjectField);
xJNIPP_IMPLEMENT_GET_FIELD(jboolean,GetBooleanField);
xJNIPP_IMPLEMENT_GET_FIELD(jbyte,GetByteField);
xJNIPP_IMPLEMENT_GET_FIELD(jchar,GetCharField);
xJNIPP_IMPLEMENT_GET_FIELD(jshort,GetShortField);
xJNIPP_IMPLEMENT_GET_FIELD(jint,GetIntField);
xJNIPP_IMPLEMENT_GET_FIELD(jlong,GetLongField);
xJNIPP_IMPLEMENT_GET_FIELD(jfloat,GetFloatField);
xJNIPP_IMPLEMENT_GET_FIELD(jdouble,GetDoubleField);

xJNIPP_IMPLEMENT_GET_FIELD(LObject,GetStaticObjectField);
xJNIPP_IMPLEMENT_GET_FIELD(jboolean,GetStaticBooleanField);
xJNIPP_IMPLEMENT_GET_FIELD(jbyte,GetStaticByteField);
xJNIPP_IMPLEMENT_GET_FIELD(jchar,GetStaticCharField);
xJNIPP_IMPLEMENT_GET_FIELD(jshort,GetStaticShortField);
xJNIPP_IMPLEMENT_GET_FIELD(jint,GetStaticIntField);
xJNIPP_IMPLEMENT_GET_FIELD(jlong,GetStaticLongField);
xJNIPP_IMPLEMENT_GET_FIELD(jfloat,GetStaticFloatField);
xJNIPP_IMPLEMENT_GET_FIELD(jdouble,GetStaticDoubleField);

xJNIPP_IMPLEMENT_SET_FIELD(SetObjectField,const AbstractObject&);
xJNIPP_IMPLEMENT_SET_FIELD(SetBooleanField,jboolean);
xJNIPP_IMPLEMENT_SET_FIELD(SetByteField,jbyte);
xJNIPP_IMPLEMENT_SET_FIELD(SetCharField,jchar);
xJNIPP_IMPLEMENT_SET_FIELD(SetShortField,jshort);
xJNIPP_IMPLEMENT_SET_FIELD(SetIntField,jint);
xJNIPP_IMPLEMENT_SET_FIELD(SetLongField,jlong);
xJNIPP_IMPLEMENT_SET_FIELD(SetFloatField,jfloat);
xJNIPP_IMPLEMENT_SET_FIELD(SetDoubleField,jdouble);

xJNIPP_IMPLEMENT_SET_FIELD(SetStaticObjectField,const AbstractObject&);
xJNIPP_IMPLEMENT_SET_FIELD(SetStaticBooleanField,jboolean);
xJNIPP_IMPLEMENT_SET_FIELD(SetStaticByteField,jbyte);
xJNIPP_IMPLEMENT_SET_FIELD(SetStaticCharField,jchar);
xJNIPP_IMPLEMENT_SET_FIELD(SetStaticShortField,jshort);
xJNIPP_IMPLEMENT_SET_FIELD(SetStaticIntField,jint);
xJNIPP_IMPLEMENT_SET_FIELD(SetStaticLongField,jlong);
xJNIPP_IMPLEMENT_SET_FIELD(SetStaticFloatField,jfloat);
xJNIPP_IMPLEMENT_SET_FIELD(SetStaticDoubleField,jdouble);

/* Cleanup */

#undef xJNIPP_IMPLEMENT_GET_FIELD
#undef xJNIPP_IMPLEMENT_SET_FIELD


///////////////////////////////////////////////// bool versions

inline bool GetBoolField(const AbstractObject& object,jfieldID fieldID) {
    return _WrapJValue(GetBooleanField(object,fieldID));   
}
inline void SetBoolField(const AbstractObject& object,jfieldID fieldID,bool value) {
    SetBooleanField(object,fieldID,_GetJValue(value));
}

inline bool GetStaticBoolField(const AbstractObject& clazz,jfieldID fieldID) {
    return _WrapJValue(GetStaticBooleanField(clazz,fieldID));   
}
inline void SetStaticBoolField(const AbstractObject& clazz,jfieldID fieldID,bool value) {
    SetStaticBooleanField(clazz,fieldID,_GetJValue(value));   
}

/////////////////////////////////////////////////////////////////////
