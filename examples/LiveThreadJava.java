
/* Java counterpart of LiveThread live class.
 * See examples/LiveThread.h
 */
public class LiveThreadJava extends Thread {

    /* Constructors are private to disallow
     *  instantiation by Java code.
     */
    private LiveThreadJava() {
        super();
    }
    private LiveThreadJava(String name) {
        super(name);
    }


    /* run() is implemented in native code.
     */
    public native void run();


    /* These two things are needed for live class operation:
     * - finalize() destroys native object
     * - nativeInstance provides storage for native object
     *
     * Note: implementation for finalize() provided by
     *  JB_DEFINE_LIVE_CLASS macro.
     *
     * Caveat: since nativeInstance is 'int' 64-bit hosts
     *  are out of luck.
     */
    protected native void finalize();
    private int nativeInstance;
}