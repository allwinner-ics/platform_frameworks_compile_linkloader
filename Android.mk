LOCAL_PATH := $(call my-dir)

LLVM_ROOT_PATH := external/llvm


#=============================================================================
# rsloader
#-----------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := rsloader

LOCAL_MODULE_TAGS := tests

LOCAL_SHARED_LIBRARIES := \
  libstlport

LOCAL_STATIC_LIBRARIES := \
  libLLVMSupport

LOCAL_SRC_FILES := \
  lib/ELFHeader.cpp \
  lib/ELFSymbol.cpp \
  lib/ELFSectionHeader.cpp \
  lib/ELFTypes.cpp \
  lib/StubLayout.cpp \
  utils/raw_ostream.cpp \
  utils/helper.cpp \
  main.cpp

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH) \
  $(LOCAL_PATH)/include \
  bionic \
  external/elfutils/libelf \
  external/stlport/stlport \
  $(LOCAL_C_INCLUDES)

include $(LLVM_ROOT_PATH)/llvm-device-build.mk
include $(BUILD_EXECUTABLE)


#=============================================================================
# stub-layout-unit-test
#-----------------------------------------------------------------------------

ifdef BUILD_STUB_LAYOUT_TEST
include $(CLEAR_VARS)

LOCAL_MODULE := stub-layout-unit-test

LOCAL_MODULE_TAGS := tests

LOCAL_SHARED_LIBRARIES := \
  libstlport

LOCAL_SRC_FILES := \
  lib/StubLayout.cpp \
  utils/raw_ostream.cpp \
  utils/helper.cpp \
  tests/stub-test.cpp

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH) \
  $(LOCAL_PATH)/include \
  bionic \
  external/elfutils/libelf \
  external/stlport/stlport \
  $(LOCAL_C_INCLUDES)

include $(LLVM_ROOT_PATH)/llvm-device-build.mk
include $(BUILD_EXECUTABLE)
endif
