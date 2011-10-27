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

public class Array {

    public static long[] factorial(int length) {
        long[] array=new long[length];
        long f=1;
        for (int i=0;i!=length;++i) {
            f=f*(1+i);
            array[i]=f;
        }
        return array;
    }

    public static boolean[] int2bits(int value) {
        boolean[] bits=new boolean[32];
        for (int i=0;i!=32;++i) {
            bits[i]=((value & (1<<i))!=0);
        }
        return bits;
    }
    public static int bits2int(boolean[] bits) {
        int value=0;
        for (int i=0;i!=32;++i) {
            if (bits[i]) {
                value|=(1<<i);
            }
        }
        return value;
    }

}

