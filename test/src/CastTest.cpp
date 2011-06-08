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

#define TEST_NAME "CastsTest"

///////////////////////////////////////////////////////////////////// classes

class Interface: public java::Object {
    JB_WRAPPER_CLASS(Interface);
public:
    Interface(const jni::LObject& object);
};
typedef java::ObjectPointer<Interface> PInterface;

class Base: public java::Object {
    JB_WRAPPER_CLASS(Base);
public:
    Base();
    Base(const jni::LObject& object);
};
typedef java::ObjectPointer<Base> PBase;

class Derived: public Base {
    JB_WRAPPER_CLASS(Derived);
public:
    Derived();
    Derived(const jni::LObject& object);
};
typedef java::ObjectPointer<Derived> PDerived;

class Implementation: public java::Object {
    JB_WRAPPER_CLASS(Implementation);
public:
    Implementation();
    Implementation(const jni::LObject& object);
};
typedef java::ObjectPointer<Implementation> PImplementation;

///////////////////////////////////////////////////////////////////// implementation

///////////////////////////////////////////////// Interface

#define JB_CURRENT_CLASS Interface

JB_DEFINE_WRAPPER_CLASS(
    "com/itoa/jnipp/test/Casts$Interface"
    ,
    NoFields
    ,
    NoMethods
)

Interface::Interface(const jni::LObject& object):
    java::Object(object)
{
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////// Base

#define JB_CURRENT_CLASS Base

JB_DEFINE_WRAPPER_CLASS(
    "com/itoa/jnipp/test/Casts$Base"
    ,
    NoFields
    ,
    Methods
    (Constructor,"<init>","()V")
)

Base::Base():
    java::Object(JB_NEW(Constructor))
{
}

Base::Base(const jni::LObject& object):
    java::Object(object)
{
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////// Derived

#define JB_CURRENT_CLASS Derived

JB_DEFINE_WRAPPER_CLASS(
    "com/itoa/jnipp/test/Casts$Derived"
    ,
    NoFields
    ,
    Methods
    (Constructor,"<init>","()V")
)

Derived::Derived():
    Base(JB_NEW(Constructor))
{
}

Derived::Derived(const jni::LObject& object):
    Base(object)
{
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////// Implementation

#define JB_CURRENT_CLASS Implementation

JB_DEFINE_WRAPPER_CLASS(
    "com/itoa/jnipp/test/Casts$Implementation"
    ,
    NoFields
    ,
    Methods
    (Constructor,"<init>","()V")
)

Implementation::Implementation():
    java::Object(JB_NEW(Constructor))
{
}

Implementation::Implementation(const jni::LObject& object):
    java::Object(object)
{
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////// check

template <class OtherObjectType>
static void CheckedCast(const jni::AbstractObject& object,bool shouldSucceed) {
    typedef typename java::ObjectTypeExtractor<OtherObjectType>::Type RealOOType;
    try {
        java::ObjectPointer<RealOOType> result=java::Cast<OtherObjectType>(object);
        if (!shouldSucceed) {
            TEST_FAILED("Cast to '%s' should have failed.",
                RealOOType::GetTypeClass()->GetName()->GetUTF());
        }
        if (object.GetJObject() && !result) {
            TEST_FAILED("Casting to '%s' yeilded null object.",
                RealOOType::GetTypeClass()->GetName()->GetUTF());
        }
    }
    catch (const jni::AbstractObject& exception) {
        if (shouldSucceed) {
            TEST_FAILED("Cast to %s failed.",
                java::Object::GetClass(object)->GetName()->GetUTF());
        } else {
            java::PString exceptionClassName=java::Object::GetClass(exception)->GetName();
            if (strcmp(exceptionClassName->GetUTF(),"java.lang.ClassCastException")) {
                TEST_FAILED("Cast threw invalid exception '%s'.",
                    exceptionClassName->GetUTF());
            }
            java::PString exceptionMessage=java::Cast<java::Throwable>(exception)->GetMessage();
            java::PClass classTo=RealOOType::GetTypeClass();
            java::PString classToName=classTo->GetName();
            if (strcmp(classTo->GetName()->GetUTF(),exceptionMessage->GetUTF())) {
                TEST_FAILED("Cast threw wrong message '%s'.",
                    exceptionMessage->GetUTF());
            }
        }
    }
}

///////////////////////////////////////////////////////////////////// test

void RunCastsTest() {

    {
        jni::LObject null;

        CheckedCast<java::Object>(null,true);
        CheckedCast<Interface>(null,true);
        CheckedCast<Base>(null,true);
    }

    {
        java::PString string=java::PString::New("Hello!");

        CheckedCast<java::Object>(string,true);

        CheckedCast<Interface>(string,false);
        CheckedCast<Base>(string,false);
    }
    
    {
        PDerived derived=new Derived();
        
        CheckedCast<java::Object>(derived,true);
        CheckedCast<Base>(derived,true);
        CheckedCast<Interface>(derived,true);
        
        CheckedCast<Implementation>(derived,false);
        CheckedCast<java::PString>(derived,false);
    }

    {
        PImplementation implementation=new Implementation();
        
        CheckedCast<java::Object>(implementation,true);
        CheckedCast<Interface>(implementation,true);
        
        CheckedCast<Base>(implementation,false);
        CheckedCast<java::PString>(implementation,false);
    }
    
    TEST_PASSED();
}

