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

LOCAL_PATH := $(call my-dir)

APP_PACKAGE := $(shell $(HOST_AWK) -f $(BUILD_AWK)/extract-package-name.awk $(APP_MANIFEST))
ifndef APP_PACKAGE
    $(error Failed to read package name from '$(APP_MANIFEST)'.)
endif

# Subpath to folder where files common to all samples are located.
COMMON_SUBPATH := ../../common/src

include $(CLEAR_VARS)

LOCAL_MODULE := $(APP_PACKAGE)

LOCAL_CFLAGS += \
    -DAPP_PACKAGE='"$(APP_PACKAGE)"' \
    -I$(LOCAL_PATH)/$(COMMON_SUBPATH)

LOCAL_SRC_FILES := \
    Main.cpp \
    $(COMMON_SUBPATH)/AndroidClasses.cpp \

LOCAL_LDLIBS := -llog -landroid
LOCAL_STATIC_LIBRARIES := itoa-dropins itoa-jnipp

include $(BUILD_SHARED_LIBRARY)

# Files in 'common' use itoa-dropins.
$(call import-module,itoa-dropins/android)
$(call import-module,itoa-jnipp/android)
