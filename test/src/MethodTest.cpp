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

#include "Common.h"

//TODO NonVirtual methods
//TODO Methods with many parameters
//TODO Object returning methods

#define TEST_NAME "MethodTest"

///////////////////////////////////////////////////////////////////// helpers

#define JB_CURRENT_CLASS Callee

JB_DEFINE_ACCESSOR(
    "com/itoa/jnipp/test/Callee"
    ,
    NoFields
    ,
    Methods
    (Constructor,"<init>","()V")
    (GetBoolean,"getBoolean","()Z")(SetBoolean,"setBoolean","(Z)V")
    (GetByte,"getByte","()B")(SetByte,"setByte","(B)V")
    (GetChar,"getChar","()C")(SetChar,"setChar","(C)V")
    (GetShort,"getShort","()S")(SetShort,"setShort","(S)V")
    (GetInt,"getInt","()I")(SetInt,"setInt","(I)V")
    (GetLong,"getLong","()J")(SetLong,"setLong","(J)V")
    (GetFloat,"getFloat","()F")(SetFloat,"setFloat","(F)V")
    (GetDouble,"getDouble","()D")(SetDouble,"setDouble","(D)V")
    (GetStaticBoolean,"+getStaticBoolean","()Z")(SetStaticBoolean,"+setStaticBoolean","(Z)V")
    (GetStaticByte,"+getStaticByte","()B")(SetStaticByte,"+setStaticByte","(B)V")
    (GetStaticChar,"+getStaticChar","()C")(SetStaticChar,"+setStaticChar","(C)V")
    (GetStaticShort,"+getStaticShort","()S")(SetStaticShort,"+setStaticShort","(S)V")
    (GetStaticInt,"+getStaticInt","()I")(SetStaticInt,"+setStaticInt","(I)V")
    (GetStaticLong,"+getStaticLong","()J")(SetStaticLong,"+setStaticLong","(J)V")
    (GetStaticFloat,"+getStaticFloat","()F")(SetStaticFloat,"+setStaticFloat","(F)V")
    (GetStaticDouble,"+getStaticDouble","()D")(SetStaticDouble,"+setStaticDouble","(D)V")
)

static jni::LObject CreateTestObject() {
    return JB_NEW(Constructor);
}

#define GENERATE_GETSET(MethodType,ReturnType,CType,PrinfFormatter) \
    static void GetSet##ReturnType(const jni::LObject& object,CType value) { \
        JB_CALL(VoidMethod,object,Set##MethodType,value); \
        CType testValue=JB_CALL(ReturnType##Method,object,Get##MethodType); \
        if (testValue!=value) { \
            TEST_FAILED( \
                #ReturnType " value " PrinfFormatter \
                    " doesn't match original value " PrinfFormatter ".\n", \
                testValue,value); \
        } \
    }

#define GENERATE_STATIC_GETSET(MethodType,ReturnType,CType,PrinfFormatter) \
    static void GetSetStatic##ReturnType(CType value) { \
        JB_CALL_STATIC(VoidMethod,SetStatic##MethodType,value); \
        CType testValue=JB_CALL_STATIC(ReturnType##Method,GetStatic##MethodType); \
        if (testValue!=value) { \
            TEST_FAILED( \
                #ReturnType " value " PrinfFormatter \
                    " doesn't match original value " PrinfFormatter ".\n", \
                testValue,value); \
        } \
    }

GENERATE_GETSET(Boolean,Boolean,jboolean,"%d");
GENERATE_GETSET(Boolean,Bool,bool,"%d");
GENERATE_GETSET(Byte,Byte,jbyte,"%02X");
GENERATE_GETSET(Char,Char,jchar,"%c");
GENERATE_GETSET(Short,Short,jshort,"%d");
GENERATE_GETSET(Int,Int,jint,"%d");
GENERATE_GETSET(Long,Long,jlong,"%llx");
GENERATE_GETSET(Float,Float,jfloat,"%f");
GENERATE_GETSET(Double,Double,jdouble,"%g");

GENERATE_STATIC_GETSET(Boolean,Boolean,jboolean,"%d");
GENERATE_STATIC_GETSET(Boolean,Bool,bool,"%d");
GENERATE_STATIC_GETSET(Byte,Byte,jbyte,"%02X");
GENERATE_STATIC_GETSET(Char,Char,jchar,"%c");
GENERATE_STATIC_GETSET(Short,Short,jshort,"%d");
GENERATE_STATIC_GETSET(Int,Int,jint,"%d");
GENERATE_STATIC_GETSET(Long,Long,jlong,"%llx");
GENERATE_STATIC_GETSET(Float,Float,jfloat,"%f");
GENERATE_STATIC_GETSET(Double,Double,jdouble,"%g");

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// test

void RunMethodTest() {

    GetSetStaticBoolean(JNI_TRUE);
    GetSetStaticBoolean(JNI_FALSE);
    GetSetStaticBool(true);
    GetSetStaticBool(false);
    GetSetStaticByte(100);
    GetSetStaticChar(0x0901);
    GetSetStaticShort(32000);
    GetSetStaticInt(0x77777776);
    GetSetStaticLong(0xFFFFAAAABBBBCCCCll);
    GetSetStaticFloat(0.0001f);
    GetSetStaticDouble(0.77e+100);

    jni::LObject testObject=CreateTestObject();
    
    GetSetBoolean(testObject,JNI_TRUE);
    GetSetBoolean(testObject,JNI_FALSE);
    GetSetBool(testObject,true);
    GetSetBool(testObject,false);
    GetSetByte(testObject,0xAA);
    GetSetChar(testObject,'*');
    GetSetShort(testObject,13900);
    GetSetInt(testObject,0xFEFE0001);
    GetSetLong(testObject,0xAAAABBBBCCCCll);
    GetSetFloat(testObject,0.3434f);
    GetSetDouble(testObject,0.77e-12);

    TEST_PASSED();
}
