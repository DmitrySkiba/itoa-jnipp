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

public class Callee {

    /* Static methods */

    public static boolean getStaticBoolean() { return staticBooleanField; }
    public static void setStaticBoolean(boolean value) { staticBooleanField=value; }
    public static byte getStaticByte() { return staticByteField; }
    public static void setStaticByte(byte value) { staticByteField=value; }
    public static char getStaticChar() { return staticCharField; }
    public static void setStaticChar(char value) { staticCharField=value; }
    public static short getStaticShort() { return staticShortField; }
    public static void setStaticShort(short value) { staticShortField=value; }
    public static int getStaticInt() { return staticIntField; }
    public static void setStaticInt(int value) { staticIntField=value; }
    public static long getStaticLong() { return staticLongField; }
    public static void setStaticLong(long value) { staticLongField=value; }
    public static float getStaticFloat() { return staticFloatField; }
    public static void setStaticFloat(float value) { staticFloatField=value; }
    public static double getStaticDouble() { return staticDoubleField; }
    public static void setStaticDouble(double value) { staticDoubleField=value; }
    public static Object getStaticObject() { return staticObjectField; }
    public static void setStaticObject(Object value) { staticObjectField=value; }

    public static boolean selectBoolean(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return z; }
    public static byte selectByte(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return b; }
    public static char selectChar(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return c; }
    public static short selectShort(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return s; }
    public static int selectInt(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return i; }
    public static long selectLong(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return j; }
    public static float selectFloat(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return f; }
    public static double selectDouble(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return d; }
    public static Object selectObject(int i,boolean z,short s,char c,long j,float f,double d,Object o,byte b) { return o; }

    /* Instance methods */

    public boolean getBoolean() { return booleanField; }
    public void setBoolean(boolean value) { booleanField=value; }
    public byte getByte() { return byteField; }
    public void setByte(byte value) { byteField=value; }
    public char getChar() { return charField; }
    public void setChar(char value) { charField=value; }
    public short getShort() { return shortField; }
    public void setShort(short value) { shortField=value; }
    public int getInt() { return intField; }
    public void setInt(int value) { intField=value; }
    public long getLong() { return longField; }
    public void setLong(long value) { longField=value; }
    public float getFloat() { return floatField; }
    public void setFloat(float value) { floatField=value; }
    public double getDouble() { return doubleField; }
    public void setDouble(double value) { doubleField=value; }
    public Object getObject() { return objectField; }
    public void setObject(Object value) { objectField=value; }

    /* Data, not accessed from native code */

    private static Object staticObjectField;
    private static boolean staticBooleanField;
    private static byte staticByteField;
    private static char staticCharField;
    private static short staticShortField;
    private static int staticIntField;
    private static long staticLongField;
    private static float staticFloatField;
    private static double staticDoubleField;

    private Object objectField;
    private boolean booleanField;
    private byte byteField;
    private char charField;
    private short shortField;
    private int intField;
    private long longField;
    private float floatField;
    private double doubleField;
}
