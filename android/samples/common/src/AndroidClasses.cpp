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

#include "AndroidClasses.h"
#include <string>

#include <android/log.h>

///////////////////////////////////////////////////////////////////// Log

void Log::Info(const char* tag,const char* message,...) {
    va_list arguments;
    va_start(arguments,message);
    __android_log_vprint(ANDROID_LOG_INFO,tag,message,arguments);
    va_end(arguments);
}

void Log::Warning(const char* tag,const char* message,...) {
    va_list arguments;
    va_start(arguments,message);
    __android_log_vprint(ANDROID_LOG_WARN,tag,message,arguments);
    va_end(arguments);
}

void Log::Error(const char* tag,const char* message,...) {
    va_list arguments;
    va_start(arguments,message);
    __android_log_vprint(ANDROID_LOG_ERROR,tag,message,arguments);
    va_end(arguments);
}

void Log::Debug(const char* tag,const char* message,...) {
    va_list arguments;
    va_start(arguments,message);
    __android_log_vprint(ANDROID_LOG_DEBUG,tag,message,arguments);
    va_end(arguments);
}
///////////////////////////////////////////////////////////////////// R

char* R::m_packageName=0;
pthreadpp::mutex R::m_lock(pthreadpp::mutex::initializer());
java::Class* R::m_idClass=0;
java::Class* R::m_layoutClass=0;

void R::Initialize(const char* packageName) {
    pthreadpp::mutex_guard guard(m_lock);
    DestroyNoLock();
    m_packageName=strdup(packageName);
}

void R::Destroy() {
    pthreadpp::mutex_guard guard(m_lock);
    DestroyNoLock();
}

void R::DestroyNoLock() {
    if (m_packageName) {
        free(m_packageName);
        m_packageName=0;
    }
    DestroyInnerClass(m_idClass);
    DestroyInnerClass(m_layoutClass);
}

void R::InitInnerClass(java::Class*& clazz,const char* className) {
    pthreadpp::mutex_guard guard(m_lock);
    if (!clazz) {
        std::string fullName;
        fullName+=m_packageName;
        fullName+=".R$";
        fullName+=className;
        clazz=java::Class::ForName(fullName.c_str()).Detach();
    }
}

void R::DestroyInnerClass(java::Class*& clazz) {
    if (clazz) {
        clazz->Release();
        clazz=0;
    }
}

int32_t R::GetInnerClassInt(java::Class*& clazz,const char* className,const char* valueName) {
    InitInnerClass(clazz,className);
    jfieldID fieldID=jni::GetStaticFieldID(*clazz,valueName,"I");
    return jni::GetStaticIntField(*clazz,fieldID);
}

int32_t R::GetID(const char* valueName) {
    return GetInnerClassInt(m_idClass,"id",valueName);
}

int32_t R::GetLayout(const char* valueName) {
    return GetInnerClassInt(m_layoutClass,"layout",valueName);
}

///////////////////////////////////////////////////////////////////// Window

#define JB_CURRENT_CLASS Window

JB_DEFINE_WRAPPER_CLASS(
    "android/view/Window"
    ,
    NoFields
    ,
    Methods
    (
        SetContentViewById,
        "setContentView",
        "(I)V"
    )
    (
        FindViewById,
        "findViewById",
        "(I)Landroid/view/View;"
    )
)

Window::Window(const jni::LObject& object):
    java::Object(object)
{
}

void Window::SetContentView(int32_t layoutID) {
    JB_CALL_THIS(VoidMethod,SetContentViewById,layoutID);
}

PView Window::FindViewById(int32_t id) {
    return PView::Wrap(JB_CALL_THIS(ObjectMethod,FindViewById,id));
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// ViewOCLAdapter

class ViewOCLAdapter;
typedef java::ObjectPointer<ViewOCLAdapter> PViewOCLAdapter;

/* This class adapts C++ View::IOnClickListener to
 *  android.view.View.OnClickListener using
 *  ViewOnClickListener helper.
 */
class ViewOCLAdapter: public java::Object {
    JB_LIVE_CLASS(ViewOCLAdapter);
public:
    ViewOCLAdapter(View::IOnClickListener* listener);
    virtual ~ViewOCLAdapter();
private:
    void OnClick(PView view);
private:
    View::IOnClickListener* m_listener;
};

#define JB_CURRENT_CLASS ViewOCLAdapter

JB_DEFINE_LIVE_CLASS(
    "com/itoa/jnipp/android/ViewOnClickListener"
    ,
    NoFields
    ,
    Methods
    (
        Constructor,
        "<init>","()V"
    )
    ,
    Callbacks
    (
        Method(OnClick),
        "onClick","(Landroid/view/View;)V"
    )
)

ViewOCLAdapter::ViewOCLAdapter(View::IOnClickListener* listener):
    java::Object(JB_NEW(Constructor),GetInstanceFieldID()),
    m_listener(listener)
{
}

ViewOCLAdapter::~ViewOCLAdapter() {
    m_listener->DestroyListener();
}

void ViewOCLAdapter::OnClick(PView view) {
    m_listener->OnClick(view);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// View

#define JB_CURRENT_CLASS View

JB_DEFINE_WRAPPER_CLASS(
    "android/view/View"
    ,
    NoFields
    ,
    Methods
    (
        SetOnClickListener,
        "setOnClickListener",
        "(Landroid/view/View$OnClickListener;)V"
    )
    (
        Invalidate,
        "invalidate",
        "()V"
    )
)

View::View(const jni::LObject& object):
    java::Object(object)
{
}

View::View(const jni::LObject& object,jfieldID instanceFieldID):
    java::Object(object,instanceFieldID)
{
}

void View::SetOnClickListener(IOnClickListener* listener) {
    if (!listener) {
        JB_CALL_THIS(VoidMethod,SetOnClickListener,jni::NullObject);
    } else {
        PViewOCLAdapter listenerAdapter;
        try {
            listenerAdapter=new ViewOCLAdapter(listener);
        }
        catch (...) {
            listener->DestroyListener();
            throw;
        }
        JB_CALL_THIS(VoidMethod,SetOnClickListener,listenerAdapter);
    }
}

void View::Invalidate() {
    JB_CALL_THIS(VoidMethod,Invalidate);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// Context

#define JB_CURRENT_CLASS Context

JB_DEFINE_WRAPPER_CLASS(
    "android/content/Context"
    ,
    NoFields
    ,
    NoMethods
)

Context::Context(const jni::LObject& object):
    java::Object(object)
{
}

Context::Context(const jni::LObject& object,jfieldID instanceFieldID):
    java::Object(object,instanceFieldID)
{
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// Activity

//TODO add OnPause, OnResume, etc.

#define JB_CURRENT_CLASS Activity

JB_DEFINE_LIVE_CLASS(
    "com/itoa/jnipp/android/Activity"
    ,
    NoFields
    ,
    Methods
    (
        GetWindow,
        "getWindow",
        "()Landroid/view/Window;"
    )
    ,
    Callbacks
    (
        Method(Construct),
        "construct",
        "()V"
    )
    (
        Method(OnCreate),
        "onCreate",
        "(Landroid/os/Bundle;)V"
    )
)

Activity::Activity(const jni::LObject& object):
    Context(object,GetInstanceFieldID())
{
}

void Activity::RegisterCallbacks() {
    JB_INIT_CLASS();
}

void Activity::Construct(const jni::LObject& thiz) {
    SetNativeInstance(
        java::Object::GetClass(thiz)->GetName()->GetUTF(),
        thiz);
}

PWindow Activity::GetWindow() const {
    return PWindow::Wrap(JB_CALL_THIS(ObjectMethod,GetWindow));
}

void Activity::SetContentView(int32_t layoutID) {
    GetWindow()->SetContentView(layoutID);
}

PView Activity::FindViewById(int32_t id) {
    return GetWindow()->FindViewById(id);
}

#undef JB_CURRENT_CLASS

/////////////////////////////////////// super methods

#define JB_CURRENT_CLASS ActivitySuper

JB_DEFINE_ACCESSOR(
    "android/app/Activity"
    ,
    NoFields
    ,
    Methods
    (
        OnCreate,
        "onCreate",
        "(Landroid/os/Bundle;)V"
    )
)

void Activity::OnCreate(const jni::LObject& bundle) {
    JB_NVCALL(VoidMethod,*this,OnCreate,bundle);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// AttributeSet

#define JB_CURRENT_CLASS AttributeSet

JB_DEFINE_WRAPPER_CLASS(
    "android/util/AttributeSet"
    ,
    NoFields
    ,
    NoMethods
)

AttributeSet::AttributeSet(const jni::LObject& object):
    java::Object(object)
{
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// Canvas

#define JB_CURRENT_CLASS Canvas

JB_DEFINE_WRAPPER_CLASS(
    "android/graphics/Canvas"
    ,
    NoFields
    ,
    Methods
    (
        DrawColor,
        "drawColor",
         "(I)V"
    )
)

Canvas::Canvas(const jni::LObject& object):
    java::Object(object)
{
}
Canvas::Canvas(const jni::LObject& object,jfieldID instanceFieldID):
    java::Object(object,instanceFieldID)
{
}

void Canvas::DrawColor(color_t color) {
    JB_CALL_THIS(VoidMethod,DrawColor,color);
}

#undef JB_CURRENT_CLASS

///////////////////////////////////////////////////////////////////// LiveView

#define JB_CURRENT_CLASS LiveView

JB_DEFINE_LIVE_CLASS(
    "com/itoa/jnipp/android/LiveView"
    ,
    NoFields
    ,
    Methods
    (
        SuperOnDraw,
         "superOnDraw",
        "(Landroid/graphics/Canvas;)V"
    )
    ,
    Callbacks
    (
        Method(SetNativeInstance),
        "setNativeInstance",
        "(Ljava/lang/String;)V"
    )
    (
        Method(Construct1),
        "construct",
        "(Landroid/content/Context;Landroid/util/AttributeSet;)V"
    )
    (
        Method(Construct2),
        "construct",
        "(Landroid/content/Context;Landroid/util/AttributeSet;I)V"
    )
    (
         Method(OnDraw),
        "onDraw",
        "(Landroid/graphics/Canvas;)V"
    )
)

LiveView::LiveView(const jni::LObject& object):
    View(object,GetInstanceFieldID())
{
}

void LiveView::RegisterCallbacks() {
    JB_INIT_CLASS();
}

void LiveView::SetNativeInstance(const jni::LObject& object,java::PString tag) {
    ::SetNativeInstance(tag->GetUTF(),object);
}

void LiveView::Construct1(PContext context,PAttributeSet attrs) {
    Construct(context,attrs);
}
void LiveView::Construct(PContext context,PAttributeSet attrs) {
    // Default implementation does nothing.
}

void LiveView::Construct2(PContext context,PAttributeSet attrs,int defStyle) {
    Construct(context,attrs,defStyle);
}
void LiveView::Construct(PContext context,PAttributeSet attrs,int defStyle) {
    // Default implementation does nothing.
}

void LiveView::OnDraw(PCanvas canvas) {
    JB_CALL_THIS(VoidMethod,SuperOnDraw,canvas);
}

#undef JB_CURRENT_CLASS

/////////////////////////////////////////////////////////////////////
