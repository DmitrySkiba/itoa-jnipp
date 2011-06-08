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

#ifndef _ANDROIDCLASSES_INCLUDED_
#define _ANDROIDCLASSES_INCLUDED_

#include "JNIpp.h"
#include <dropins/pthreadpp.h>

///////////////////////////////////////////////////////////////////// Log

/** This class is a counterpart for \c android.util.Log,
 *  only with sanely named methods.
 */
class Log {
public:
    static void Info(const char* tag,const char* message,...);
    static void Warning(const char* tag,const char* message,...);
    static void Error(const char* tag,const char* message,...);
    static void Debug(const char* tag,const char* message,...);
};

///////////////////////////////////////////////////////////////////// R

/** Accessor for R class.
 * Values are queried in runtime by their names.
 *
 * Note that class must be initialized with package name, see
 *  \c Initialize() method.
 */
class R {
public:
    static void Initialize(const char* packageName);
    static void Destroy();
    static int32_t GetID(const char* name);
    static int32_t GetLayout(const char* name);
private:
    static void DestroyNoLock();
    static void InitInnerClass(java::Class*&,const char*);
    static void DestroyInnerClass(java::Class*&);
    static int32_t GetInnerClassInt(java::Class*&,const char*,const char*);
private:
    static char* m_packageName;
    static pthreadpp::mutex m_lock;
    static java::Class* m_idClass;
    static java::Class* m_layoutClass;
};

///////////////////////////////////////////////////////////////////// IListener

/** Base class for all listeners.
 * Concrete listener should inherit virtually to allow multiple
 *  listeners to be inherited.
 */
class IListener {
public:
    IListener() {}
    virtual ~IListener() {}

    /** Destroys listener.
     * Called when Java adapter holding listener is destroyed.
     */
    virtual void DestroyListener()=0;
private:
    IListener(const IListener&);
    IListener& operator=(const IListener&);
};

///////////////////////////////////////////////////////////////////// Window

class Window;
typedef java::ObjectPointer<Window> PWindow;

class View;
typedef java::ObjectPointer<View> PView;

/** Wrapper for android.view.Window.
 * Add methods when necessary.
 */
class Window: public java::Object {
    JB_WRAPPER_CLASS(Window);
public:
    Window(const jni::LObject& object);
    void SetContentView(int32_t layoutID);
    PView FindViewById(int32_t id);
};

///////////////////////////////////////////////////////////////////// View

class View: public java::Object {
    JB_WRAPPER_CLASS(View);
public:
    class IOnClickListener: public virtual IListener {
    public:
        virtual void OnClick(PView view)=0;
    };
public:
    View(const jni::LObject& object);

    void Invalidate();

    void SetOnClickListener(IOnClickListener* listener);

    /* A little bit of sugar: allow member functions to handle
     *  OnClick events.
     */
    template <class C>
    void SetOnClickListener(C& object,void(C::*listener)(PView)) {
        class ListenerAdapter: public IOnClickListener {
        public:
            ListenerAdapter(C& object,void(C::*listener)(PView)):
                m_object(object),
                m_listener(listener)
            {
            }
            virtual void OnClick(PView view) {
                (m_object.*m_listener)(view);
            }
            virtual void DestroyListener() {
                delete this;
            }
        private:
            C& m_object;
            void (C::*m_listener)(PView);
        };
        SetOnClickListener(new ListenerAdapter(object,listener));
    }
protected:
    View(const jni::LObject& object,jfieldID instanceFieldID);
};

///////////////////////////////////////////////////////////////////// SetNativeInstance

/** Creates and sets native instance identified by \c tag.
 * You should provide implementation of this function.
 */
void SetNativeInstance(const char* tag,const jni::LObject& object);

///////////////////////////////////////////////////////////////////// Context

class Context;
typedef java::ObjectPointer<Context> PContext;

/** Wrapper for \c android.content.Context.
 * Note that this class has live constructor, and hence allows
 *  derived classes to be live classes. See Activity class for
 *  an example.
 */
class Context: public java::Object {
    JB_WRAPPER_CLASS(Context);
public:
    Context(const jni::LObject& object);
protected:
    Context(const jni::LObject& object,jfieldID instanceFieldID);
};

///////////////////////////////////////////////////////////////////// Activity

class Activity;
typedef java::ObjectPointer<Activity> PActivity;

/** The activity class you inherit from to make your own
 *  activities.
 * This class calls \c SetNativeInstance() with the full class name
 *  to create concrete native implementation.
 */
class Activity: public Context {
    JB_LIVE_CLASS(Activity);
public:
    static void RegisterCallbacks();
    PWindow GetWindow() const;
    void SetContentView(int32_t layoutID);
    PView FindViewById(int32_t id);
protected:
    Activity(const jni::LObject& object);
    virtual void OnCreate(const jni::LObject& bundle);
    //TODO: add OnPause, OnResume, etc.
private:
    static void Construct(const jni::LObject& thiz);
};

///////////////////////////////////////////////////////////////////// AttributeSet

class AttributeSet;
typedef java::ObjectPointer<AttributeSet> PAttributeSet;

/** Wrapper for \c android.util.AttributeSet.
 * Note that this class doesn't have live constructor which means
 *  that all its descendants will always be wrapper classes.
 */
class AttributeSet: public java::Object {
    JB_WRAPPER_CLASS(AttributeSet);
public:
    AttributeSet(const jni::LObject&);
};

///////////////////////////////////////////////////////////////////// Color

typedef int32_t color_t;

/** Native counterpart for \c android.graphics.Color.
 * Utility classes like this should be implemented entirely
 *  in native code.
 */
class Color {
public:
    static const color_t BLACK       = 0xFF000000;
    static const color_t DKGRAY      = 0xFF444444;
    static const color_t GRAY        = 0xFF888888;
    static const color_t LTGRAY      = 0xFFCCCCCC;
    static const color_t WHITE       = 0xFFFFFFFF;
    static const color_t RED         = 0xFFFF0000;
    static const color_t GREEN       = 0xFF00FF00;
    static const color_t BLUE        = 0xFF0000FF;
    static const color_t YELLOW      = 0xFFFFFF00;
    static const color_t CYAN        = 0xFF00FFFF;
    static const color_t MAGENTA     = 0xFFFF00FF;
    static const color_t TRANSPARENT = 0;
};

///////////////////////////////////////////////////////////////////// Canvas

class Canvas;
typedef java::ObjectPointer<Canvas> PCanvas;

class Canvas: public java::Object {
    JB_WRAPPER_CLASS(Canvas);
public:
    Canvas(const jni::LObject& object);
    void DrawColor(color_t color);
protected:
    Canvas(const jni::LObject& object,jfieldID instanceFieldID);
};

///////////////////////////////////////////////////////////////////// LiveView

class LiveView: public View {
    JB_LIVE_CLASS(LiveView);
public:
    static void RegisterCallbacks();
protected:
    LiveView(const jni::LObject&);
    virtual void Construct(PContext,PAttributeSet);
    virtual void Construct(PContext,PAttributeSet,int32_t);
    virtual void OnDraw(PCanvas canvas);
private:
    static void SetNativeInstance(const jni::LObject&,java::PString);
    void Construct1(PContext,PAttributeSet);
    void Construct2(PContext,PAttributeSet,int32_t);
};

/////////////////////////////////////////////////////////////////////

#endif // _ANDROIDCLASSES_INCLUDED_
