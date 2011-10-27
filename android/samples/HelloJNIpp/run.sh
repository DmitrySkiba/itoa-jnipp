#! /bin/bash
#
# Copyright (C) 2011 Dmitry Skiba
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

THIS_PATH=`dirname $0`
THIS_PATH=`cd $THIS_PATH && pwd`

ndk-build -C $THIS_PATH/jni &&
ant -f $THIS_PATH/build.xml install &&
adb shell am start -a android.intent.action.MAIN -n com.itoa.jnipp.hello/.MainActivity
