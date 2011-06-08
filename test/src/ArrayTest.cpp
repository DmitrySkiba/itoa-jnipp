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

#define TEST_NAME "ArrayTest"

///////////////////////////////////////////////////////////////////// classes

#define JB_CURRENT_CLASS Array

JB_DEFINE_ACCESSOR(
    "com/itoa/jnipp/test/Array"
    ,
    NoFields
    ,
    Methods
    (
        Factorial,
        "+factorial","(I)[J"
    )
    (
        Int2Bits,
        "+int2bits","(I)[Z"
    )
    (
        Bits2Int,
        "+bits2int","([Z)I"
    )
)

static java::PLongArray Factorial(int32_t value) {
    return java::PLongArray::Wrap(JB_CALL_STATIC(ObjectMethod,Factorial,value));
}

static java::PBoolArray Int2Bits(int32_t value) {
    return java::PBoolArray::Wrap(JB_CALL_STATIC(ObjectMethod,Int2Bits,value));   
}

static int32_t Bits2Int(java::PBoolArray bits) {
    return JB_CALL_STATIC(IntMethod,Bits2Int,bits);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// test

void RunArrayTest() {

    {
        const int factorialLength=19;
        java::PLongArray factorial=Factorial(factorialLength);
        
        TEST_CHECK_FAIL(factorial->GetLength()!=factorialLength,
            "Invalid factorial length %d (expected %d).",
            factorial->GetLength(),factorialLength);
        
        jlong f=1;
        for (int i=0;i!=factorial->GetLength();++i) {
            f*=(i+1);
            jlong value=factorial->GetAt(i);
            TEST_CHECK_FAIL(value!=f,
                "Invalid factorial[%d] value %lld (expected %lld).",i,value,f);
        }
    }
    
    {
        java::PBoolArray bits=new java::BoolArray(32);
        bits->SetAt(31,true);
        bits->SetAt(30,true);
        bits->SetAt(17,true);
        bits->SetAt(7,true);
        const int expectedValue=0xC0020080;
        
        int value=Bits2Int(bits);
        TEST_CHECK_FAIL(value!=expectedValue,
            "Invalid bits value: %08X (expected %08X).",value,expectedValue);
    }

    TEST_PASSED();
}
