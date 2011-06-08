#include <unistd.h>
#include "LiveThread.h"

/**************************************/

/* Demostrates two ways of starting a thread.
 */
void StartMyThread() {

    // There is no leak.
    // MyThread instance stored nativeInstance variable of
    //  LiveThread object. When LiveThread is finalized by
    //  GC it also destroys native MyThread instance.
    (new MyThread())->Start();

    // More readable alternative to the above:
    PLiveThread liveThread=new MyThread();
    liveThread->Start();
}

/* The following function demonstrates different calls
 *  that are all end up in MyThread::Run().
 */
void CallRunVirtually() {
    
    PLiveThread liveThread=new MyThread();

    // This is simple: when you call Run() on LiveThread
    //  call ends up in MyThread::Run() because Run() is
    //  declared 'virtual' in LiveThread.
    liveThread->Run();

    // Now if you call Run() on a Thread object, which
    //  doesn't declare Run() as 'virtual'...
    PThread thread=liveThread;
    thread->Run();
    // ...the call ends in MyThread::Run() too! That is because
    // it goes through Java where all methods a virtual:
    // Thread::Run()->[java]Thread.run()->[java]LiveThread.run()
    //  ->LiveThread::Run()->MyThread::Run().
}

/**************************************/

MyThread::MyThread():
    LiveThread(java::PString::New("MyThread"))
{
}

void MyThread::Run() {
    java::PString name=GetName();
    for (int i=0;i!=5;++i) {
        sleep(1);
        printf("%s: working...\n",name->GetUTF());
    }
    printf("%s: done\n",name->GetUTF());
}


/**************************************/

/* Usual wrapper class implementation.
*/

#define JB_CURRENT_CLASS Thread

JB_DEFINE_WRAPPER_CLASS(
    "java/lang/Thread"
    ,
    NoFields
    ,
    Methods
    (
        Ctor,
        "<init>","()V"
    )
    (
        CtorString,
        "<init>","(Ljava/lang/String;)V"
    )
    (
        GetName,
        "getName","()Ljava/lang/String;"
    )
    (
        Start,
        "start","()V"
    )
    (
        Run,
        "run","()V"
    )
    (
        GetCurrentThread,
        "+currentThread","()Ljava/lang/Thread;"
    )
)

Thread::Thread():
    java::Object(JB_NEW(Ctor))
{
}

Thread::Thread(java::PString name):
    java::Object(JB_NEW(CtorString,name))
{
}

Thread::Thread(const jni::LObject& thread):
    java::Object(thread)
{
}

Thread::Thread(const jni::LObject& object,jfieldID instanceFieldID):
    java::Object(object,instanceFieldID)
{
}

java::PString Thread::GetName() const {
    return java::PString::Wrap(JB_CALL_THIS(ObjectMethod,GetName));
}

void Thread::Start() {
    JB_CALL_THIS(VoidMethod,Start);
}

void Thread::Run() {
    JB_CALL_THIS(VoidMethod,Run);
}
    
PThread Thread::GetCurrentThread() {
    return PThread::Wrap(JB_CALL_STATIC(ObjectMethod,GetCurrentThread));
}

#undef JB_CURRENT_CLASS


/**************************************/

#define JB_CURRENT_CLASS LiveThread

JB_DEFINE_LIVE_CLASS(
    "LiveThreadJava"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
        "<init>","(Ljava/lang/String;)V"
    )
    ,
    Callbacks
    (
         Method(Run),
        "run","()V"
    )
)

LiveThread::LiveThread(java::PString name):
    Thread(JB_NEW(Constructor,name),GetInstanceFieldID())
{
}

#undef JB_CURRENT_CLASS


/**************************************/

#ifdef ONLY_FOR_DOXYGEN
// Wire this example to:
java::Object;
#endif
