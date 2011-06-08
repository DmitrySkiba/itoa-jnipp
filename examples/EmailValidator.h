#ifndef EMAILVALIDATOR_INCLUDED
#define EMAILVALIDATOR_INCLUDED

#include <JNIpp.h>

/* In this example we create wrappers for Pattern and Matcher
 *  classes from java.util.regex namespace.
 *
 * With wrappers in hand we write a native version of email
 *  validation function presented at http://goo.gl/cWJ1L
 */

/**************************************/

/* Email validation function.
 */
bool ValidateEmail(const char* email);


/**************************************/

/* Forward declare both classes for convenience.
 * ObjectPointer typedefs are the must, because working with
 *  raw Object pointers is error-prone and exception-unsafe.
 */

class Pattern;
typedef java::ObjectPointer<Pattern> PPattern;

class Matcher;
typedef java::ObjectPointer<Matcher> PMatcher;


/* We start with Pattern class.
 * Example uses 'compile' and 'matcher' methods, so for now
 *  our class will contain only these.
 */
class Pattern: public java::Object {
    /* Manifest that this class is a wrapper class. */
    JB_WRAPPER_CLASS(Pattern);
public:

    /* For convenience we add an overload which takes
     *  good old 'const char*'
     */
    static PPattern Compile(const char* regex);
    static PPattern Compile(java::PString regex);
    
    /* Java naming rules are strange, so we rename 'matcher'
     *  method.
     */
    PMatcher CreateMatcher(const char* input) const;
    PMatcher CreateMatcher(java::PCharSequence input) const;

private:
    
    /* Wrapping constructor.
     * Private, as you should use Compile() method to get
     *  an instance.
     */
    Pattern(const jni::LObject& pattern);
};


/* For now Matcher contains single method 'matches'.
 */
class Matcher: public java::Object {
    JB_WRAPPER_CLASS(Matcher);
public:
    /* Wrapping constructor.
     * Public to allow creating instances by Pattern class.
     */
    Matcher(const jni::LObject& matcher);

    /* Checks for match. */
    bool Matches() const;
};


#endif // EMAILVALIDATOR_INCLUDED
