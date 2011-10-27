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
 * Contains Java Native Interface functions; see #jni for more.
 */

#ifndef _JNIPP_JAVANI_INCLUDED_
#define _JNIPP_JAVANI_INCLUDED_

#include <jni.h>
#include <algorithm>
#include <dropins/begin_namespace.h>

/** Contains Java Native Interface functions.
 *
 * Functions in this namespace are modified versions of standard
 *  JNI functions found in \c jni.h and described in JNI specification
 *  at http://java.sun.com/docs/books/jni/.
 *
 *
 * Overview of modifications:
 *
 *  - Functions don't take JNIEnv* pointer. They get it from the current
 *    thread using jni::GetEnv().
 *
 *  - jni::AbstractObject is used in function arguments instead of
 *    \c jobject and its descendants (\c jclass, \c jintArray, etc.).
 *    This allows to pass objects derived from jni::AbstractObject
 *    (e.g. java::String) to the functions (e.g. jni::SetObjectField).
 *
 *  - Functions return jni::LObject instead of \c jobject.
 *
 *  - Java exceptions that were raised during function calls are translated
 *    to C++ exceptions. See jni::TranslateJavaException() for details.
 *
 *  - Functions that return \c jboolean have companion versions that
 *    return \c bool (e.g jni::GetBooleanField() and jni::GetBoolField()).
 *    Vararg functions like \c jni::NewObject() also support both \c jboolean
 *    and \c bool.
 */
BEGIN_NAMESPACE(jni)

///////////////////////////////////////////////////////////////////// classes

///////////////////////////////////////////////// AbstractObject

/** Abstract Java object (\c jobject) container.
 */
class AbstractObject {
public:
    AbstractObject() {}
    virtual ~AbstractObject() {}

    /** Returns contained Java object; returns NULL to indicate
     *  NULL object.
     */
    virtual jobject GetJObject() const=0;
private:
    AbstractObject(const AbstractObject&);
    AbstractObject& operator=(const AbstractObject&);
};

///////////////////////////////////////////////// LObject

/** Wrapper for local Java objects.
 *
 * Local objects are returned from functions like jni::NewObject()
 *  or jni::CallObjectMethod(). They have local references added
 *  to them and are valid only within native method that created
 *  them. See 5.1.1 Local References in JNI specification.
 *
 * LObjects are just safe containers for local objects on their way
 *  from #jni functions to wrapper classes like java::Object or
 *  java::String.
 *
 * LObject supports the following usages:
 * - <tt> if (lobject) </tt>
 * - <tt> if (!lobject) </tt>
 * - <tt> if (lobject1==lobject2) </tt>
 * - <tt> if (lobject1!=lobject2) </tt>
 */
class LObject: public AbstractObject {
    class ComparasionHelper;
public:
    /** Constructs empty object.
     */
    LObject();

    /** Copies Java object from \c other, adds local reference.
     */
    LObject(const LObject& other);

    /** Deletes local reference from contained Java object.
     */
    virtual ~LObject();

    /** Swaps two instances.
     */
    void Swap(LObject& other);

    /** Returns contained Java object.
     */
    virtual jobject GetJObject() const;

    /** Copies Java object from \c other, adds local reference.
     */
    LObject& operator=(const LObject& other);

    /** Returns \c true if object is empty (contains NULL Java object).
     */
    bool IsEmpty() const;

    /** Checks if object is empty (contains NULL Java object).
     */
    bool operator!() const;

    /** Comparasion helper.
     */
    operator ComparasionHelper*() const;

    /** Creates and returns LObject by adding a local reference
     *  to the \c object.
     */
    static LObject Wrap(jobject object);

    /** Creates and returns LObject, but DOES NOT add local
     *  reference.
     * Use this method when you already have local reference
     *  added to the object (e.g. object is a result of calling
     *  JNIEnv::NewObject).
     */
    static LObject WrapLocal(jobject object);

private:
    LObject(jobject object,bool addReference);
    void Construct(jobject object,bool addReference);
private:
    jobject m_object;
};

///////////////////////////////////////////////// NullObject

/** Pass this object instead of NULL.
 * If NULL is defined as "0" on your platform, passing NULL to
 *  vararg functions will not cause a compiler error. However,
 *  NULL will be treated as int and will likely cause a crash on
 *  systems where sizeof(int)!=sizeof(void*).
 */
extern LObject NullObject;

///////////////////////////////////////////////////////////////////// common

/** Returns \c JNIEnv* for the current thread.
 * Performs JavaVM::AttachCurrentThread().
 */
JNIEnv* GetEnv();

/** Initializes #jni; must be called before any other #jni function.
 * Not thread safe, must be called on startup (in JNI_OnLoad()).
 *
 * Can be called multiple times, only first invocation will have
 *  an effect.
 */
void Initialize(JavaVM* vm);

/** Version of Initialize(JavaVM*) that takes \c JNIEnv*.
 * Handy when you need to initialize #jni from ordinary JNI function
 *  (not from JNI_OnLoad()).
 *
 * Not thread safe.
 */
void Initialize(JNIEnv* env);

/** Terminates current program with a message.
 * If \c JNIEnv is available to the current thread, this function
 *  calls \c JNIEnv::FatalError(), otherwise it logs message
 *  in a platform-specific way and calls \c abort().
 */
void FatalError(const char* message,...);

///////////////////////////////////////////////////////////////////// classes & objects

/** Finds class by name.
 * Name is a fully-qualified class name, i.e. \c java/lang/Class and not
 *  \c java.lang.Class. See JNI reference for details.
 * \n
 * Function may return empty (NULL) object or throw Java exception.
 */
LObject FindClass(const char* name);

/** Returns class of the \c object.
 */
LObject GetObjectClass(const AbstractObject& object);

/** Returns superclass of the \c clazz.
 */
LObject GetSuperclass(const AbstractObject& clazz);

/** Returns \c true if object of type \c clazz can be safely cast to \c clazzFrom.
 */
bool IsAssignableFrom(const AbstractObject& clazz,const AbstractObject& clazzFrom);

/** Returns \c true if \c object is an instance of \c clazz.
 */
bool IsInstanceOf(const AbstractObject& object,const AbstractObject& clazz);

/** Tests whether two references refer to the same Java object.
 */
bool IsSameObject(const AbstractObject& object1,const AbstractObject& object2);

///////////////////////////////////////////////////////////////////// exceptions

/** Raises Java exception.
 * This function instructs Java VM that Java exception
 *  \c throwable should be thrown once control returns
 *  to the VM.
 */
void Throw(const AbstractObject& throwable);

/** Converts currently pending Java exception to C++ exception.
 *
 * If no Java exception is pending this function does nothing.
 *
 * If Java exception is pending this function wraps it in private
 *  object derived from jni::AbstractObject, clears pending exception
 *  status and throws exception object by value. Use java::Cast() to
 *  cast exception object to concrete type. You can safely cast to
 *  java::Throwable class.
 *
 * You should catch Java exception by const reference to
 *  jni::AbstractObject:
 * \code
 *  try {
 *    java::PIntArray array=new java::IntArray(1);
 *    array->SetAt(10,0); // throws IndexOutOfBoundsException
 *  }
 *  catch (const jni::AbstractObject& e) {
 *    java::PThrowable thr=java::Cast<java::Throwable>(e);
 *    java::PString message=thr->GetMessage();
 *    printf("Exception's type: %s, message: %s\n",
 *      thr->GetClass()->GetName()->GetUTF(),
 *      message?message->GetUTF():NULL);
 *  }
 * \endcode
 *
 * See also jni::TranslateCppException().
 */
void TranslateJavaException();

/** Converts current C++ exception to the Java exception and
 * raises it using jni::Throw().
 *
 * This function should be used as follows:
 * \code
 * try {
 *   std::vector<int> vec(1);
 *   vec.at(10)=0; // throws std::out_of_range
 * }
 * catch (...) {
 *   jni::TranslateCppException();
 *   // java.lang.RuntimeException is raised.
 * }
 * \endcode
 *
 * Translation algorithm:
 *  - If <tt> const jni::AbstractObject& </tt> was thrown,
 *    it is raised.
 *  - If <tt> const std::basic_exception& </tt> was thrown,
 *    function raises \c java.lang.RuntimeException with
 *    exception's message.
 *  - All other exception types are translated to
 *    \c java.lang.RuntimeException with the message of
 *    'Unknown C++ exception'.
 *
 * This function is used to translate exceptions occurred during
 *  invocation of callbacks specified in JB_DEFINE_LIVE_CLASS().
 */
void TranslateCppException();

///////////////////////////////////////////////////////////////////// arrays

/** Release mode for ReleaseXXXArrayElements functions.
 */
enum ArrayReleaseMode {

    /** Copy back the content and free the elements buffer.
     */
    CommitFreeElements=0,

    /** Copy back the content but do not free the elements buffer.
     */
    CommitElements=JNI_COMMIT,

    /** Free the elements buffer without copying back the possible changes.
     */
    FreeElements=JNI_ABORT
};

/** Indicates whether an array of \c jboolean values can be cast to
 *  an array of C++ \c bool values.
 *
 * Evaluates to
 * \code
 * sizeof(jboolean)==sizeof(bool) &&
 *   JNI_TRUE==true && JNI_FALSE==false
 * \endcode
 *
 * Bool array functions (e.g jni::GetBoolArrayElements()) use this
 *  value to decide whether to cast or convert.
 */
const bool JBooleanIsBool=
    sizeof(jboolean)==sizeof(bool) &&
    JNI_TRUE==1 && JNI_FALSE==0;

/** Returns array's length.
 */
jsize GetArrayLength(const AbstractObject& array);

/////////////////////////////////////// object[]

/** Creates new object array and fills it with nulls.
 */
LObject NewObjectArray(jsize length,const AbstractObject& elementClass);

/** Creates new object array of objects and fills it with \c initialElement.
 */
LObject NewObjectArray(jsize length,const AbstractObject& elementClass,const AbstractObject& initialElement);

/** Retrieves object array element.
 */
LObject GetObjectArrayElement(const AbstractObject& array,jsize index);

/** Sets object array element.
 */
void SetObjectArrayElement(const AbstractObject& array,jsize index,const AbstractObject& value);

/////////////////////////////////////// bool[]

/** Creates new \c boolean array; equivalent to jni::NewBooleanArray().
 */
LObject NewBoolArray(jsize length);

/** Retrieves and locks \c boolean array elements.
 */
bool* GetBoolArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c boolean array elements.
 */
void ReleaseBoolArrayElements(const AbstractObject& array,bool* elements,jint mode);

/** Retrieves region from \c boolean array.
 */
void GetBoolArrayRegion(const AbstractObject& array,jsize start,jsize length,bool* buffer);

/** Updates region in \c boolean array.
 */
void SetBoolArrayRegion(const AbstractObject& array,jsize start,jsize length,const bool* buffer);

/////////////////////////////////////// boolean[]

/** Creates new \c boolean array.
 */
LObject NewBooleanArray(jsize length);

/** Retrieves and locks \c boolean array elements;
 *  see also jni::GetBoolArrayElements().
 */
jboolean* GetBooleanArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c boolean array elements;
 *  see also jni::ReleaseBoolArrayElements().
 */
void ReleaseBooleanArrayElements(const AbstractObject& array,jboolean* elements,ArrayReleaseMode mode);

/** Retrieves region from \c boolean array;
 *  see also jni::GetBoolArrayRegion().
 */
void GetBooleanArrayRegion(const AbstractObject& array,jsize start,jsize length,jboolean* buffer);

/** Updates region in \c boolean array;
 *  see also jni::SetBoolArrayRegion().
 */
void SetBooleanArrayRegion(const AbstractObject& array,jsize start,jsize length,const jboolean* buffer);

/////////////////////////////////////// byte[]

/** Creates new \c byte array.
 */
LObject NewByteArray(jsize length);

/** Retrieves and locks \c byte array elements.
 */
jbyte* GetByteArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c byte array elements.
 */
void ReleaseByteArrayElements(const AbstractObject& array,jbyte* elements,ArrayReleaseMode mode);

/** Retrieves region from \c byte array.
 */
void GetByteArrayRegion(const AbstractObject& array,jsize start,jsize length,jbyte* buffer);

/** Updates region in \c byte array.
 */
void SetByteArrayRegion(const AbstractObject& array,jsize start,jsize length,const jbyte* buffer);

/////////////////////////////////////// char[]

/** Creates new \c char array.
 */
LObject NewCharArray(jsize length);

/** Retrieves and locks \c char array elements.
 */
jchar* GetCharArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c char array elements.
 */
void ReleaseCharArrayElements(const AbstractObject& array,jchar* elements,ArrayReleaseMode mode);

/** Retrieves region from \c char array.
 */
void GetCharArrayRegion(const AbstractObject& array,jsize start,jsize length,jchar* buffer);

/** Updates region in \c char array.
 */
void SetCharArrayRegion(const AbstractObject& array,jsize start,jsize length,const jchar* buffer);

/////////////////////////////////////// short[]

/** Creates new \c short array.
 */
LObject NewShortArray(jsize length);

/** Retrieves and locks \c short array elements.
 */
jshort* GetShortArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c short array elements.
 */
void ReleaseShortArrayElements(const AbstractObject& array,jshort* elements,ArrayReleaseMode mode);

/** Retrieves region from \c short array.
 */
void GetShortArrayRegion(const AbstractObject& array,jsize start,jsize length,jshort* buffer);

/** Updates region in \c short array.
 */
void SetShortArrayRegion(const AbstractObject& array,jsize start,jsize length,const jshort* buffer);

/////////////////////////////////////// int[]

/** Creates new \c int array.
 */
LObject NewIntArray(jsize length);

/** Retrieves and locks \c int array elements.
 */
jint* GetIntArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c int array elements.
 */
void ReleaseIntArrayElements(const AbstractObject& array,jint* elements,ArrayReleaseMode mode);

/** Retrieves region from \c int array.
 */
void GetIntArrayRegion(const AbstractObject& array,jsize start,jsize length,jint* buffer);

/** Updates region in \c int array.
 */
void SetIntArrayRegion(const AbstractObject& array,jsize start,jsize length,const jint* buffer);

/////////////////////////////////////// long[]

/** Creates new \c long array.
 */
LObject NewLongArray(jsize length);

/** Retrieves and locks \c long array elements.
 */
jlong* GetLongArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c long array elements.
 */
void ReleaseLongArrayElements(const AbstractObject& array,jlong* elements,ArrayReleaseMode mode);

/** Retrieves region from \c long array.
 */
void GetLongArrayRegion(const AbstractObject& array,jsize start,jsize length,jlong* buffer);

/** Updates region in \c long array.
 */
void SetLongArrayRegion(const AbstractObject& array,jsize start,jsize length,const jlong* buffer);

/////////////////////////////////////// float[]

/** Creates new \c float array.
 */
LObject NewFloatArray(jsize length);

/** Retrieves and locks \c float array elements.
 */
jfloat* GetFloatArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c float array elements.
 */
void ReleaseFloatArrayElements(const AbstractObject& array,jfloat* elements,ArrayReleaseMode mode);

/** Retrieves region from \c float array.
 */
void GetFloatArrayRegion(const AbstractObject& array,jsize start,jsize length,jfloat* buffer);

/** Updates region in \c float array.
 */
void SetFloatArrayRegion(const AbstractObject& array,jsize start,jsize length,const jfloat* buffer);

/////////////////////////////////////// double[]

/** Creates new \c double array.
 */
LObject NewDoubleArray(jsize length);

/** Retrieves and locks \c double array elements.
 */
jdouble* GetDoubleArrayElements(const AbstractObject& array,bool* isCopy=0);

/** Releases (and optionally commits) \c double array elements.
 */
void ReleaseDoubleArrayElements(const AbstractObject& array,jdouble* elements,ArrayReleaseMode mode);

/** Retrieves region from \c double array.
 */
void GetDoubleArrayRegion(const AbstractObject& array,jsize start,jsize length,jdouble* buffer);

/** Updates region in \c double array.
 */
void SetDoubleArrayRegion(const AbstractObject& array,jsize start,jsize length,const jdouble* buffer);

///////////////////////////////////////////////////////////////////// methods

/** Finds method in the class.
 */
jmethodID GetMethodID(const AbstractObject& clazz,const char* name,const char* signature);

/** Finds static method in the class.
 */
jmethodID GetStaticMethodID(const AbstractObject& clazz,const char* name,const char* signature);


#ifdef ONLY_FOR_DOXYGEN

/** Information about vararg support in #jni functions.
 *
 * All vararg #jni functions are in fact template overloads with
 *  different number of arguments. That is because C++ doesn't
 *  allow passing objects to standard vararg functions (denoted
 *  with ...).
 *
 * There are following limitations to the vararg arguments:
 *  - Maximum number of arguments is 20.
 *  - Allowed primitive types:
 *    - <tt> bool </tt>
 *    - <tt> signed / unsigned char </tt> (\c jbyte, \c jboolean)
 *    - <tt> signed / unsigned short </tt> (\c jchar, \c jshort)
 *    - <tt> signed / unsigned int </tt> (\c jint)
 *    - <tt> jlong </tt>
 *    - <tt> jfloat </tt>
 *    - <tt> jdouble </tt>
 *  - Allowed object types:
 *    - All objects derived from jni::AbstractObject and passed
 *      by value or by reference.
 */
class VarArgs {};


/** Create new Java object (equivalent to \c new in Java).
 */
LObject NewObject(const AbstractObject& clazz,jmethodID constructorID,VarArgs);

/////////////////////////////////////// object methods

/** Calls \c void method.
 */
void CallVoidMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning object.
 */
LObject CallObjectMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c boolean; see also CallBoolMethod().
 */
jboolean CallBooleanMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c boolean and converts result to C++ \c bool.
 */
bool CallBoolMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c byte.
 */
jbyte CallByteMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c char.
 */
jchar CallCharMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c short.
 */
jshort CallShortMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c int.
 */
jint CallIntMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c long.
 */
jlong CallLongMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c float.
 */
jfloat CallFloatMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/** Calls method returning \c double.
 */
jdouble CallDoubleMethod(const AbstractObject& object,jmethodID methodID,VarArgs);

/////////////////////////////////////// nonvirtual methods

/** Calls void method defined in \c clazz.
 */
void CallNonvirtualVoidMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning object defined in \c clazz.
 */
LObject CallNonvirtualObjectMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c boolean; see also CallNonvirtualBoolMethod().
 */
jboolean CallNonvirtualBooleanMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c boolean defined in \c clazz and converts result to C++ \c bool.
 */
bool CallNonvirtualBoolMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c byte defined in \c clazz.
 */
jbyte CallNonvirtualByteMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c char defined in \c clazz.
 */
jchar CallNonvirtualCharMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c short defined in \c clazz.
 */
jshort CallNonvirtualShortMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c int defined in \c clazz.
 */
jint CallNonvirtualIntMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c long defined in \c clazz.
 */
jlong CallNonvirtualLongMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c float defined in \c clazz.
 */
jfloat CallNonvirtualFloatMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls method returning \c double defined in \c clazz.
 */
jdouble CallNonvirtualDoubleMethod(const AbstractObject& object,const AbstractObject& clazz,jmethodID methodID,VarArgs);


/////////////////////////////////////// static methods

/** Calls void static method.
 */
void CallStaticVoidMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning object.
 */
jobject CallStaticObjectMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c boolean; see also CallStaticBoolMethod().
 */
jboolean CallStaticBooleanMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c boolean and converts result to C++ bool.
 */
bool CallStaticBoolMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c byte.
 */
jbyte CallStaticByteMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c char.
 */
jchar CallStaticCharMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c short.
 */
jshort CallStaticShortMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c int.
 */
jint CallStaticIntMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c long.
 */
jlong CallStaticLongMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c float.
 */
jfloat CallStaticFloatMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

/** Calls static method returning \c double.
 */
jdouble CallStaticDoubleMethod(const AbstractObject& clazz,jmethodID methodID,VarArgs);

#endif // ONLY_FOR_DOXYGEN

///////////////////////////////////////////////////////////////////// fields

/* All Get/Set methods are implemented as inline
 *  functions in JavaNIDetails.h.
 */


/** Returns id of the field.
 */
jfieldID GetFieldID(const AbstractObject& clazz,const char* name,const char* signature);

/** Returns id of the static field.
 */
jfieldID GetStaticFieldID(const AbstractObject& clazz,const char* name,const char* signature);

/////////////////////////////////////// object fields

/** Retrieves value of an object field.
 */
LObject GetObjectField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to an object field.
 */
void SetObjectField(const AbstractObject& object,jfieldID fieldID,const AbstractObject& value);


/** Retrieves value of a \c boolean field; see also GetBoolField().
 */
jboolean GetBooleanField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to a \c boolean field; see also SetBoolField().
 */
void SetBooleanField(const AbstractObject& object,jfieldID fieldID,jboolean value);


/** Retrieves value of a \c boolean field and converts it to C++ \c bool.
 */
bool GetBoolField(const AbstractObject& object,jfieldID fieldID);

/** Sets value of a C++ \c bool to a \c boolean field.
 */
void SetBoolField(const AbstractObject& object,jfieldID fieldID,bool value);


/** Retrieves value of a \c byte field.
 */
jbyte GetByteField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to a \c byte field.
 */
void SetByteField(const AbstractObject& object,jfieldID fieldID,jbyte value);


/** Retrieves value of a \c char field.
 */
jchar GetCharField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to a \c char field.
 */
void SetCharField(const AbstractObject& object,jfieldID fieldID,jchar value);


/** Retrieves value of a \c short field.
 */
jshort GetShortField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to a \c short field.
 */
void SetShortField(const AbstractObject& object,jfieldID fieldID,jshort value);


/** Retrieves value of a \c int field.
 */
jint GetIntField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to a \c int field.
 */
void SetIntField(const AbstractObject& object,jfieldID fieldID,jint value);


/** Retrieves value of a \c long field.
 */
jlong GetLongField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to a \c long field.
 */
void SetLongField(const AbstractObject& object,jfieldID fieldID,jlong value);


/** Retrieves value of a \c float field.
 */
jfloat GetFloatField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to a \c float field.
 */
void SetFloatField(const AbstractObject& object,jfieldID fieldID,jfloat value);


/** Retrieves value of a \c double field.
 */
jdouble GetDoubleField(const AbstractObject& object,jfieldID fieldID);

/** Sets value to a \c double field.
 */
void SetDoubleField(const AbstractObject& object,jfieldID fieldID,jdouble value);

/////////////////////////////////////// static fields

/** Retrieves value of a static object field.
 */
LObject GetStaticObjectField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static object field.
 */
void SetStaticObjectField(const AbstractObject& clazz,jfieldID fieldID,const AbstractObject& value);


/** Retrieves value of a \c boolean field; see also GetStaticBoolField.
 */
jboolean GetStaticBooleanField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static \c boolean field; see also SetStaticBoolField.
 */
void SetStaticBooleanField(const AbstractObject& clazz,jfieldID fieldID,jboolean value);


/** Retrieves value of a \c boolean field and converts it to C++ \c bool.
 */
bool GetStaticBoolField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value of a C++ \c bool to a static \c boolean field.
 */
void SetStaticBoolField(const AbstractObject& clazz,jfieldID fieldID,bool value);


/** Retrieves value of a static \c byte field.
 */
jbyte GetStaticByteField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static \c byte field.
 */
void SetStaticByteField(const AbstractObject& clazz,jfieldID fieldID,jbyte value);


/** Retrieves value of a static \c char field.
 */
jchar GetStaticCharField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static \c char field.
 */
void SetStaticCharField(const AbstractObject& clazz,jfieldID fieldID,jchar value);


/** Retrieves value of a static \c short field.
 */
jshort GetStaticShortField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static \c short field.
 */
void SetStaticShortField(const AbstractObject& clazz,jfieldID fieldID,jshort value);


/** Retrieves value of a static \c int field.
 */
jint GetStaticIntField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static \c int field.
 */
void SetStaticIntField(const AbstractObject& clazz,jfieldID fieldID,jint value);


/** Retrieves value of a static \c long field.
 */
jlong GetStaticLongField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static \c long field.
 */
void SetStaticLongField(const AbstractObject& clazz,jfieldID fieldID,jlong value);


/** Retrieves value of a static \c float field.
 */
jfloat GetStaticFloatField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static \c float field.
 */
void SetStaticFloatField(const AbstractObject& clazz,jfieldID fieldID,jfloat value);


/** Retrieves value of a static \c double field.
 */
jdouble GetStaticDoubleField(const AbstractObject& clazz,jfieldID fieldID);

/** Sets value to a static \c double field.
 */
void SetStaticDoubleField(const AbstractObject& clazz,jfieldID fieldID,jdouble value);

/////////////////////////////////////////////////////////////////////

#include "JavaNIDetails.h"

END_NAMESPACE(jni)

#endif // _JNIPP_JAVANI_INCLUDED_
