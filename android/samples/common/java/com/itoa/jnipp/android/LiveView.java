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

package com.itoa.jnipp.android;

import android.view.View;
import android.content.Context;
import android.util.AttributeSet;
import android.graphics.Canvas;

public class LiveView extends View {

    /* Construction */

    public LiveView(Context context,AttributeSet attrs) {
        super(context,attrs);
        setNativeInstance(attrs);
        construct(context,attrs);
    }
    public LiveView(Context context,AttributeSet attrs,int defStyle) {
        super(context,attrs,defStyle);
        setNativeInstance(attrs);
        construct(context,attrs,defStyle);
    }

    private void setNativeInstance(AttributeSet attrs) {
        String tag=attrs.getAttributeValue(null,"nativeTag");
        if (tag==null) {
            throw new RuntimeException("LiveView is missing nativeTag attribute.");
        }
        setNativeInstance(tag);
        if (nativeInstance==0) {
            throw new RuntimeException(String.format(
                "Native instance for LiveView '%s' was not set.",tag));
        }
    }

    private native void setNativeInstance(String tag);
    private native void construct(Context context,AttributeSet attrs);
    private native void construct(Context context,AttributeSet attrs,int defStyle);

    /* Overridables */

    protected native void onDraw(Canvas canvas);

    // This method is called from native's default implementation of onDraw.
    private void superOnDraw(Canvas canvas) {
        super.onDraw(canvas);
    }


    /* Required by JNIpp */
    protected native void finalize();
    private int nativeInstance;
}
