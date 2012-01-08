#ifndef LIVETHREAD_INCLUDED
#define LIVETHREAD_INCLUDED

#include <JNIpp.h>

/* In this example we define wrapper class for java.lang.Thread and
 *  then implement live class derived from it.
 *
 * Then we implement subclass of a live class which does some work
 *  in a separate thread.
 */


/**************************************/

/* Wrapper for java.lang.Thread.
 *
 * See examples\EmailValidator.h for detailed information about
 *  defining wrapper classes.
 */

class Thread;
typedef java::ObjectPointer<Thread> PThread;


class Thread: public java::Object {
    JB_WRAPPER_CLASS(Thread);
public:
    /* Each of these constructors creates instance of
     *  java.lang.Thread and then wraps it.
     */
    Thread();
    Thread(java::PString name);

    /* Wrapping constructor.
     */
    Thread(const jni::LObject& thread);

    /* Instance methods.
     */
    java::PString GetName() const;
    void Start();
    void Run();

    /* Static methods.
     */
    static PThread GetCurrentThread();
protected:
    Thread(const jni::LObject&,jfieldID);
};


/**************************************/

/* Live thread class.
 *
 * As opposed to wrapper classes, which allow only native->Java calls
 *  live classes allow Java->native calls too. However, this requires
 *  support from Java class, see examples/LiveThreadJava.java.
 *
 * It is 'live' because instances of this class are bound
 *  to Java objects of class LiveThread (binding is defined
 *  in .cpp file). Methods from live class can be bound to
 *  the Java class methods. For example LiveThread::Run() is
 *  bound to LiveThreadJava.run().
 */

class LiveThread;
typedef java::ObjectPointer<LiveThread> PLiveThread;

/* Note that JB_LIVE_CLASS() is used instead of JB_WRAPPER_CLASS().
 */
class LiveThread: public Thread {
    JB_LIVE_CLASS(LiveThread);
public:

    /* Both constructors create instances of LiveThreadJava classes.
     * No wrapping constructor is defined, since it is not a wrapper
     *  class.
     */
    LiveThread();
    LiveThread(java::PString name);

    /* Implementation of LiveThreadJava.run().
     */
    virtual void Run()=0;
};


/**************************************/

/* Custom thread class.
 * MyThread::Run() does some work.
 */

class MyThread: public LiveThread {
public:
    MyThread();
    virtual void Run();
};


#endif // LIVETHREAD_INCLUDED
