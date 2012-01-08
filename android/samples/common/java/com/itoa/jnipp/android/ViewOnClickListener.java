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

public class ViewOnClickListener implements View.OnClickListener {

    /** This class is can be created only from native code.
     */
    private ViewOnClickListener() {
    }

    public native void onClick(View view);

    // Required by JNIpp.
    protected native void finalize();
    private int nativeInstance;
}