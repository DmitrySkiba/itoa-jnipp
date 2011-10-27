#include "EmailValidator.h"

/**************************************/

/* Email validator function which uses Pattern and Matcher
 *  classes implemented below.
 */


/* We compile email pattern only once.
 * Note that we use pointer to Pattern instead of advised PPattern.
 *
 * The problem with using PPattern is that destructor of g_emailPattern
 *  will be called on library unload *after* the JNI unload event
 *  JNI_OnUnload. So jni::GetEnv() which every jni:: function use will
 *  fail and call jni::FatalError() to report a failure. jni::FatalError()
 *  will try to call jni::GetEnv()->FatalError() but since JNIEnv is not
 *  available jni::FatalError() will abort the whole process with abort().
 *
 * So we use (and leak) Pattern pointer.
 */
static Pattern* g_emailPattern=0;


bool ValidateEmail(const char* email) {
    if (!email || !*email) {
        return false;
    }
    if (!g_emailPattern) {
        // NOT THREAD SAFE!
        // Add locking to your taste.
        PPattern pattern=Pattern::Compile(
             "^[_A-Za-z0-9-]+(\\.[_A-Za-z0-9-]+)*@"
            "[A-Za-z0-9]+(\\.[A-Za-z0-9]+)*(\\.[A-Za-z]{2,})$"
        );
        g_emailPattern=pattern.Detach();
    }

    // Compare with Java's "pattern.matcher(email).matches()".
    return g_emailPattern->CreateMatcher(email)->Matches();
}


/**************************************/

/* Let JB_ macros know the name of the class we are implementing.
 *
 * Each wrapper class implementation must start with a definition
 *  of the current class.
 *
 * Remember that JB_ macros are only usable when JB_CURRENT_CLASS
 *  is defined. Otherwise you will get compiler errors.
 */
#define JB_CURRENT_CLASS Pattern


/* Define information about Java class we are going to call.
 *
 * Note that there are no commas between method definitions.
 * Also note that static methods are denoted with '+'.
 *
 * Fields are declared the same way - see JB_DEFINE_WRAPPER_CLASS()
 *  documentation.
 */
JB_DEFINE_WRAPPER_CLASS(
    "java/util/regex/Pattern"
    ,
    NoFields
    ,
    Methods
    (
        CompileTag,
        "+compile",
        "(Ljava/lang/String;)Ljava/util/regex/Pattern;"
    )
    (
        CreateMatcherTag,
        "matcher",
        "(Ljava/lang/CharSequence;)Ljava/util/regex/Matcher;"
    )
)

/* Wrapping constructor.
 * This constructor is used in implementation of PPattern::Wrap().
 */
Pattern::Pattern(const jni::LObject& pattern):
    java::Object(pattern)
{
}

PPattern Pattern::Compile(const char* regex) {
    return Compile(java::PString::New(regex));
}

/*
 * Here is what happens in this method:
 * - JB_CALL_STATIC() calls JB_INIT_CLASS() to make sure class descriptor
 *    (defined by JB_DEFINE_CLASS) is initialized.
 * - JB_INIT_CLASS() gets Java class "java/util/regex/Pattern" and retrieves
 *    method/field ids for each method or field defined by JB_DEFINE_WRAPPER_CLASS().
 *    Initialization happens only once (via pthread_once).
 * - jni::CallStaticObjectMethod() is called with method id for the 'CompileTag'
 *    method. You can get method id for the tag with JB_METHOD_ID() macro.
 * - Result of method call is wrapped into the Pattern class. Note that for
 *    performance reasons PPattern::Wrap() doesn't check that you are wrapping
 *    object of correct class (java.util.regex.Pattern or derived).
 */
PPattern Pattern::Compile(java::PString regex) {
    return PPattern::Wrap(JB_CALL_STATIC(ObjectMethod,CompileTag,regex));
}

PMatcher Pattern::CreateMatcher(const char* input) const {
    return CreateMatcher(java::PString::New(input));
}

/* When calling non-static methods (or accessing fields) use
 * _THIS version of JB_ macros.
 */
PMatcher Pattern::CreateMatcher(java::PCharSequence input) const {
    return PMatcher::Wrap(JB_CALL_THIS(ObjectMethod,CreateMatcherTag,input));
}

/* Finished with implementation of Pattern class, undefine
 *  JB_CURRENT_CLASS. JB_ macros will not work past this line.
 */
#undef JB_CURRENT_CLASS


/**************************************/

/* Matcher class implementation. */

#define JB_CURRENT_CLASS Matcher

JB_DEFINE_WRAPPER_CLASS(
    "java/util/regex/Matcher"
    ,
    NoFields
    ,
    Methods
    (
        Matches,
        "matches",
        "()Z"
    )
)

Matcher::Matcher(const jni::LObject& matcher):
    java::Object(matcher)
{
}

/* Note that we call 'BoolMethod', which expands to jni::CallBoolMethod().
 * This is a version of traditional jni::CallBooleanMethod() which converts
 *  jboolean to bool.
 */
bool Matcher::Matches() const {
    return JB_CALL_THIS(BoolMethod,Matches);
}

#undef JB_CURRENT_CLASS


/**************************************/

#ifdef ONLY_FOR_DOXYGEN
// Wire this example to:
java::Object;
#endif
