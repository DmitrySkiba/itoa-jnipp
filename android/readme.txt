This directory contains android specific projects.

* Android.mk : defines NDK module 'itoa-jnipp'

* samples/common : contains common files for all samples

* samples/HelloJNIpp : demostrates how JNIpp can be used to create
   native Activity and native custom drawn View.

* test : project for running tests

Both samples/HelloJNIpp/ and test/ projects must be updated before
 use: 'cd projest && android update project -p .' After update run
 run.sh script to build and run.

  