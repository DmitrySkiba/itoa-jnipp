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
 * Main include file for the JNIpp library.
 */

#include "JNIpp/JavaNI.h"
#include "JNIpp/JavaLang.h"
#include "JNIpp/JavaArray.h"

/**
 * \example EmailValidator.h
 * \example EmailValidator.cpp
 * \example LiveThread.h
 * \example LiveThread.cpp
 * \example LiveThreadJava.java
 * \example NativeSound.h
 * \example NativeSound.cpp
 * \example NativeSound.java
 * \example NativeSoundException.java
 */

/**
 \mainpage Home
 
 \section Introduction
 
 Welcome to JNIpp library which uses the power of C++ to give you
  better Java native interface.

 JNIpp key features:
  - C++ wrappers for basic Java classes like Object, String or Class.
     There are also wrappers for arrays. See #jni namespace.
  - Live class concept to implement classes that can be called from
     Java. See java::Object.
  - Macros to simplify creation of your own wrapper and live classes.
     See JavaBinding.h.
  - Exception translation: Java exceptions are converted to C++ 
     exceptions and vice versa. See jni::TranslateJavaException() and
     jni::TranslateCppException().
  - C++ style casts Java objects. See java::Cast() and java::IsInstanceOf().

 Start exploring JNIpp by looking at the <a href="examples.html">examples</a>.
 
 See \c android/samples/HelloJNIpp for Android-specific example that features
  native Activity, native custom draw View and natively handled Button.

 \section Installation

 JNIpp has the following dependencies:
 - itoa-dropins, a collection of small helpers
    (https://github.com/DmitrySkiba/itoa-dropins)
 - STL
 - C++ exception support from the compiler

 Currently JNIpp supports Android platform only. It can be build on other 
  platforms too (in fact it was developed on MacOS), however, no library 
  projects are provided so you have to manually arrange and add source 
  files to your app project.
  
 On Android platform both JNIpp and dropins projects are installed as NDK
  modules (supported in NDK r5+). Steps to install:
 - Checkout both libraries to your NDK_MODULE_PATH directory. If you don't
    want to bother with NDK_MODULE_PATH you can checkout directly to the 
    \c NDK_ROOT/sources directory. Checkout commands:
   - <tt> git clone git@github.com:DmitrySkiba/itoa-dropins.git </tt>
   - <tt> git clone git@github.com:DmitrySkiba/itoa-jnipp.git </tt>
 - Add the following to your Android.mk:
    \code
    LOCAL_STATIC_LIBRARIES := itoa-jnipp
    $(call import-module,itoa-jnipp/android) 
    \endcode
    See \c android/samples/HelloJNIpp/jni/Android.mk for example.
 - Add the following to your Application.mk:
    \code
     APP_CPPFLAGS := -fexceptions
     APP_STL := gnustl_static
    \endcode
    See \c android/samples/HelloJNIpp/jni/Application.mk for example.
 - <tt> #include <JNIpp.h> </tt> in your header and source files.
 
 \section Issues
 
 Currently library has the following issues:
 - No support for MonitorEnter/MonitorExit. That will definitely be fixed in the
    next release. For now you have to use raw functions from JNIEnv:
    <tt> jni::GetEnv()->MonitorEnter(obj.GetJObject()). </tt>
    Wrap calls in RAII object!
 - No support for GetElements/SetElements in java::PrimitiveArray. 
    Use Get/ReleaseXXXArrayElements from #jni namespace (e.g. jni::GetIntArrayElements).
 - Other issues, see numerous TODOs in the source files.
 */