#ifndef NATIVESOUND_INCLUDED
#define NATIVESOUND_INCLUDED

#include <JNIpp.h>

/* This example demostrates how JNIpp can be used to provide
 *  native implementation for a Java class (NativeSound.java).
 *
 * Key thing is that native object is constructed from Java code.
 * Contrast with LiveThread, where things are in reverse: Java object
 *  is constructed from native code.
 */


/***************************************/

/* Wrapper for NativeSoundException class.
 */

class NativeSoundException;
typedef java::ObjectPointer<NativeSoundException> PNativeSoundException;

/* We don't declare wrapping constructor because we don't intend to
 *  wrap existing NativeSoundException objects. We will be creating
 *  our own.
 */
class NativeSoundException: public java::Exception {
    JB_WRAPPER_CLASS(NativeSoundException);
public:
    NativeSoundException(const char* message);
    NativeSoundException(java::PString message);
};


/***************************************/

/* Wrapper for NativeSound.Checkpoint class.
 * Currently JNIpp doesn't allow wrapper or live classes to be inner
 *  classes, so we declare Checkpoint at global scope and add typedefs
 *  to the NativeSound class.
 */
class NativeSoundCheckpoint: public java::Object {
    JB_WRAPPER_CLASS(NativeSoundCheckpoint);
public:
    NativeSoundCheckpoint(const jni::LObject& checkpoint);

    jlong GetTime() const;
    java::PObject GetData() const;
};


/***************************************/

/* Native part of NativeSound.java is declared as ordinary live
 *  class. Because it can't be created from native code its
 *  constructor is private.
 *
 * NOTE: before NativeSound can be created from Java callbacks
 *  need to be registered using NativeSound::RegisterCallbacks().
 *  See JNI_OnLoad in NativeSound.cpp.
 */
class NativeSound: public java::Object {
    JB_LIVE_CLASS(NativeSound);
public:
    /* Typedef NativeSoundCheckpoint to feel like inner class.
     */
    typedef NativeSoundCheckpoint Checkpoint;
    typedef java::ObjectPointer<Checkpoint> PCheckpoint;

    /* Array typedefs.
     * PCheckpointArray is most useful.
     */
    typedef java::ObjectArray<Checkpoint> CheckpointArray;
    typedef java::ObjectPointer<CheckpointArray> PCheckpointArray;

public:
    /* Binds to the Java class.
     */
    static void RegisterCallbacks();

    /* Currently callback methods can't be 'const'. That will be fixed
     *  in next release.
     */
    java::PString GetPath();

    void Play();
    void Stop();
    void SetCheckpoints(PCheckpointArray checkpoints);
private:
    NativeSound(const jni::LObject&,java::PString path);
    virtual ~NativeSound();

    /* This callback function will be called when Java object calls
     *  construct(path). Note that se are binding static callback
     *  to a non-static Java method. Hence implicit first argument
     *  will be the Java object itself. We will pass that object to
     *  the live constructor of the parent class.
     */
    static void Construct(const jni::LObject&,java::PString path);
private:
    java::PString m_path;
};

#endif // NATIVESOUND_INCLUDED
