#include "NativeSound.h"

/***************************************/

/* One good place to call NativeSound::RegisterCallbacks()
 *  is JNI_OnLoad function.
 */
extern "C" jint JNI_OnLoad(JavaVM* vm,void* reserved) {
    
    // Initialize jni:: first.
    jni::Initialize(vm);

    // Register callbacks.
    NativeSound::RegisterCallbacks();

    return JNI_VERSION_1_4;
}


/***************************************/

/* Implementation of NativeSoundException class.
 */

#define JB_CURRENT_CLASS NativeSoundException

JB_DEFINE_WRAPPER_CLASS(
    "NativeSoundException"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
        "<init>","(Ljava/lang/String;)V"
    )
)

/* ATTENTION!
 * Note that we are NOT calling Exception(PString) constructor
 *  from base class. That is because it would create Java object of
 *  wrong type (java.lang.Exception).
 * Instead we are creating Java object by ourselves and calling
 *  wrapping constructor Exception(const jni::LObject&).
 */
NativeSoundException::NativeSoundException(const char* message):
    java::Exception(JB_NEW(Constructor,java::PString::New(message)))
{
}
NativeSoundException::NativeSoundException(java::PString message):
    java::Exception(JB_NEW(Constructor,message))
{
}

#undef JB_CURRENT_CLASS


/***************************************/

/* Implementation of NativeSoundCheckpoint class.
 */

#define JB_CURRENT_CLASS NativeSoundCheckpoint

JB_DEFINE_WRAPPER_CLASS(
    "NativeSound$Checkpoint"
    ,
    Fields
    (
        Time,
         "time","J"
    )
    (
        Data,
        "data","Ljava/lang/Object;"
    )
    ,
    NoMethods
)

NativeSoundCheckpoint::NativeSoundCheckpoint(const jni::LObject& checkpoint):
    java::Object(checkpoint)
{
}
    
jlong NativeSoundCheckpoint::GetTime() const {
    return JB_GET_THIS(LongField,Time);
}

java::PObject NativeSoundCheckpoint::GetData() const {
    return java::PObject::Wrap(JB_GET_THIS(ObjectField,Data));
}

#undef JB_CURRENT_CLASS


/***************************************/

/* Implementation of NativeSound class.
 */

#define JB_CURRENT_CLASS NativeSound

JB_DEFINE_LIVE_CLASS(
    "NativeSound"
    ,
    NoFields
    ,
    NoMethods
    ,
    Callbacks
    (
        Method(Construct),
        "construct","(Ljava/lang/String;)V"
    )
    (
        Method(GetPath),
        "getPath","()Ljava/lang/String;"
    )
    (
        Method(SetCheckpoints),
        "setCheckpoints","([LNativeSound$Checkpoint;)V"
    )
    (
        Method(Play),
        "play","()V"
    )
    (
        Method(Stop),
        "stop","()V"
    )
)

void NativeSound::RegisterCallbacks() {
    JB_INIT_CLASS();
}

void NativeSound::Construct(const jni::LObject& thiz,java::PString path) {
    // No leak here because instance pointer is stored in the Java
    //  object 'thiz' by the live constructor 
    //  NativeSound(const jni::LObject&,jfieldID).
    new NativeSound(thiz,path);
}

NativeSound::NativeSound(const jni::LObject& thiz,java::PString path):
    java::Object(thiz,GetInstanceFieldID()),
    m_path(path)
{
    const char* utfPath=path->GetUTF();

    FILE* file=fopen(utfPath,"r");
    if (!file) {
        // The only correct syntax to throw a Java exception:
        //  throw object pointer by value.
        throw PNativeSoundException::New("Can't open file.");

        /*** throw new NativeSoundException("Can't open file."); ***/
        // This will leak pointer. Besides jni::TranslateCppException()
        //  doesn't recognize pointers and will translate this exception
        //  to generic java.lang.RuntimeException('Unknown C++ exception').

        /*** throw NativeSoundException("Can't open file."); ***/
        // This will work, but is still wrong - descendants of java::Object
        //  are reference counted, and should not be instantiated on a
        //  stack. 
    }

    // Work with file...

    fclose(file);
}

NativeSound::~NativeSound() {
    // Free resources.
}

java::PString NativeSound::GetPath() {
    return m_path;
}

void NativeSound::SetCheckpoints(PCheckpointArray checkpoints) {
    if (checkpoints) {
        for (int i=0;i!=checkpoints->GetLength();++i) {
            PCheckpoint checkpoint=checkpoints->GetAt(i);
            jlong time=checkpoint->GetTime();
            java::PObject data=checkpoint->GetData();
            // Register checkpoint.
        }
    }
}

void NativeSound::Play() {
    // Play sound.
}
void NativeSound::Stop() {
    // Stop sound.
}

#undef JB_CURRENT_CLASS


/**************************************/

#ifdef ONLY_FOR_DOXYGEN
// Wire this example to:
java::Object;
java::Exception;
#endif
