
/* This is an example of implementing Java class completely in
 *  native code.
 * See implementation in NativeSound.h, NativeSound.cpp.
 */
class NativeSound {

    /*** Test ***/
    
    public static void test(String path) {
        try {
            NativeSound sound=new NativeSound(path);
            sound.play();

            // Lose last reference to the sound object.
            sound=null;
        }
        catch (NativeSoundException e) {
            e.printStackTrace();
            return;
        }

        // Here the sound may still be playing, because GC didn't
        //  yet collect the 'sound' object.
        // Let's force GC.
        for (int i=0;i!=10;++i) {
            System.gc();
        }
        // When GC collects NativeSound objects it calls finalize()
        //  function which in turns calls destructor on the native
        //  instances.
        // Thats why here the sound is not playing anymore.
    }


    /*** Public interface ***/
    
    public static class Checkpoint {
        public Checkpoint() {
            this(0,null);
        }
        public Checkpoint(long time,Object data) {
            this.time=time;
            this.data=data;
        }
        
        public long time;
        public Object data;
    }

    /* Constructors can't be native, so we use proxy
     *  method construct() to create native instance.
     */
    public NativeSound(String path) throws NativeSoundException {
        construct(path);
    }

    public native String getPath();

    public native void play() throws NativeSoundException;
    public native void stop() throws NativeSoundException;
    
    /* Method demonstrates how to handle arguments in callbacks.
     */
    public native void setCheckpoints(Checkpoint[] checkpoints) throws NativeSoundException;


    /*** Implementation details ***/

    private native void construct(String path) throws NativeSoundException;
    
    // Usual stuff for live classes.
    protected native void finalize();
    private int nativeInstance;
}
