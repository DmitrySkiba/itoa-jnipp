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

//TODO Object field tests

#define TEST_NAME "FieldsTest"

///////////////////////////////////////////////////////////////////// helpers

#define JB_CURRENT_CLASS FieldsTest

JB_DEFINE_ACCESSOR(
    "com/itoa/jnipp/test/Fields"
    ,
    Fields
    (Object,"objectField","Ljava/lang/Object;")
    (Boolean,"booleanField","Z")
    (Byte,"byteField","B")
    (Char,"charField","C")
    (Short,"shortField","S")
    (Int,"intField","I")
    (Long,"longField","J")
    (Float,"floatField","F")
    (Double,"doubleField","D")
    (StaticObject,"+staticObjectField","Ljava/lang/Object;")
    (StaticBoolean,"+staticBooleanField","Z")
    (StaticByte,"+staticByteField","B")
    (StaticChar,"+staticCharField","C")
    (StaticShort,"+staticShortField","S")
    (StaticInt,"+staticIntField","I")
    (StaticLong,"+staticLongField","J")
    (StaticFloat,"+staticFloatField","F")
    (StaticDouble,"+staticDoubleField","D")
    ,
    Methods
    (Constructor,"<init>","()V")
)

static jni::LObject CreateTestObject() {
    return JB_NEW(Constructor);
}

#define GENERATE_FIELD_TEST(FieldTag,FieldType,CType,PrinfFormatter) \
    static void Test##FieldType(const jni::LObject& object,CType value) { \
        JB_SET(FieldType,object,FieldTag,value); \
        CType testValue=JB_GET(FieldType,object,FieldTag); \
        if (testValue!=value) { \
            TEST_FAILED( \
                #FieldTag " value " PrinfFormatter \
                    " doesn't match original value " PrinfFormatter ".\n", \
                testValue,value); \
        } \
    }

#define GENERATE_STATIC_FIELD_TEST(FieldTag,FieldType,CType,PrinfFormatter) \
    static void TestStatic##FieldType(CType value) { \
        JB_SET_STATIC(FieldType,Static##FieldTag,value); \
        CType testValue=JB_GET_STATIC(FieldType,Static##FieldTag); \
        if (testValue!=value) { \
            TEST_FAILED( \
                #FieldTag " value " PrinfFormatter \
                    " doesn't match original value " PrinfFormatter ".\n", \
                testValue,value); \
        } \
    }

GENERATE_FIELD_TEST(Boolean,BooleanField,jboolean,"%d");
GENERATE_FIELD_TEST(Boolean,BoolField,bool,"%d");
GENERATE_FIELD_TEST(Byte,ByteField,jbyte,"%02X");
GENERATE_FIELD_TEST(Char,CharField,jchar,"%c");
GENERATE_FIELD_TEST(Short,ShortField,jshort,"%d");
GENERATE_FIELD_TEST(Int,IntField,jint,"%d");
GENERATE_FIELD_TEST(Long,LongField,jlong,"%lld");
GENERATE_FIELD_TEST(Float,FloatField,jfloat,"%f");
GENERATE_FIELD_TEST(Double,DoubleField,jdouble,"%g");

GENERATE_STATIC_FIELD_TEST(Boolean,BooleanField,jboolean,"%d");
GENERATE_STATIC_FIELD_TEST(Boolean,BoolField,bool,"%d");
GENERATE_STATIC_FIELD_TEST(Byte,ByteField,jbyte,"%02X");
GENERATE_STATIC_FIELD_TEST(Char,CharField,jchar,"%c");
GENERATE_STATIC_FIELD_TEST(Short,ShortField,jshort,"%d");
GENERATE_STATIC_FIELD_TEST(Int,IntField,jint,"%d");
GENERATE_STATIC_FIELD_TEST(Long,LongField,jlong,"%lld");
GENERATE_STATIC_FIELD_TEST(Float,FloatField,jfloat,"%f");
GENERATE_STATIC_FIELD_TEST(Double,DoubleField,jdouble,"%g");

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// test

void RunFieldsTest() {

    TestStaticBooleanField(JNI_FALSE);
    TestStaticBooleanField(JNI_TRUE);
    TestStaticBoolField(true);
    TestStaticBoolField(false);
    TestStaticByteField(100);
    TestStaticCharField(0x0901);
    TestStaticShortField(32000);
    TestStaticIntField(0x77777776);
    TestStaticLongField(0xFFFFAAAABBBBCCCCll);
    TestStaticFloatField(0.0001f);
    TestStaticDoubleField(0.77e+100);

    jni::LObject testObject=CreateTestObject();

    TestBooleanField(testObject,JNI_FALSE);
    TestBooleanField(testObject,JNI_TRUE);
    TestBoolField(testObject,true);
    TestBoolField(testObject,false);
    TestByteField(testObject,0xAA);
    TestCharField(testObject,'*');
    TestShortField(testObject,13900);
    TestIntField(testObject,0xFEFE0001);
    TestLongField(testObject,0xAAAABBBBCCCCll);
    TestFloatField(testObject,0.3434f);
    TestDoubleField(testObject,0.77e-12);

    TEST_PASSED();
}
