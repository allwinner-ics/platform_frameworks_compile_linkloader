LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LLVM_ROOT_PATH := external/llvm

LOCAL_MODULE := rsloader-run

LOCAL_MODULE_TAGS := tests

LOCAL_SHARED_LIBRARIES := \
  libstlport

LOCAL_STATIC_LIBRARIES := \
  libLLVMSupport

LOCAL_SRC_FILES := \
  ELFHeader.cpp \
  ELFSectionSymTabEntry.cpp \
  ELFSectionHeader.cpp \
  ELFTypes.cpp \
  StubLayout.cpp \
  utils/raw_ostream.cpp \
  utils/term.cpp \
  utils/helper.cpp \
  main.cpp

LOCAL_C_INCLUDES := \
  bionic \
  external/elfutils/libelf \
  external/stlport/stlport \
  $(LOCAL_C_INCLUDES)

include $(LLVM_ROOT_PATH)/llvm-device-build.mk
include $(BUILD_EXECUTABLE)
