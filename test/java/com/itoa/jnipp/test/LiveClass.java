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

package com.itoa.jnipp.test;

abstract class LiveClassBase {

    protected abstract void onFinalize();

    protected void finalize() {
        onFinalize();
    }
}

public class LiveClass extends LiveClassBase {

    public static class Counted {
        public Counted(String message) {
            this.message=message;
            this.instanceCount++;
        }
        public String toString() {
            return this.message;
        }
        protected void finalize() {
            instanceCount--;
        }
        private String message;
        private static int instanceCount;
    }

    public static class Caller {
        private LiveClass test;

        public void callSimpleCallback() {
            test.simpleCallback();
        }
        public long callPrimitiveCallback(boolean z,double d,char c,byte b,short s,int i,long j,float f) {
            return test.primitiveCallback(z,d,c,b,s,i,j,f);
        }
        public String callObjectCallback() {
            Object object=test.objectCallback();
            return object.toString();
        }
        public String callExceptionCallback(String message) {
            try {
                test.exceptionCallback(message);
                return null;
            }
            catch (Throwable t) {
                return t.getMessage();
            }
        }
        public void callNonStaticCallback() {
            test.nonStaticCallback(test);
        }

        public static void callStaticSimpleCallback() {
            LiveClass.staticSimpleCallback();
        }
    }

    public LiveClass() {
        instanceCount++;
    }

    public Caller getCaller() {
        Caller caller=new Caller();
        caller.test=this;
        return caller;
    }

    public native void simpleCallback();
    public native long primitiveCallback(boolean z,double d,char c,byte b,short s,int i,long j,float f);
    public native Object objectCallback();
    public native void exceptionCallback(String message);
    public native void nonStaticCallback(LiveClass other);

    public static native void staticSimpleCallback();

    private void throwException(String message) {
        throw new RuntimeException(message);
    }

    protected void onFinalize() {
        instanceCount--;
    }
    public static int instanceCount=0;

    /* Bindings to native code. */
    protected native void finalize();
    private int nativeInstance;
}
