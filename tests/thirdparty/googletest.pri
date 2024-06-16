DEFINES += GTEST_HAS_STREAM_REDIRECTION=1 GTEST_LANG_CXX11=1 GTEST_HAS_STD_WSTRING=1 \
           GTEST_HAS_RTTI=1 \
           GTEST_HAS_PTHREAD=0 \
           GTEST_HAS_EXCEPTIONS=1 \
           GTEST_LINKED_AS_SHARED_LIBRARY=1 \
           GTEST_CREATE_SHARED_LIBRARY=1

CONFIG(clang) {
        QMAKE_CXXFLAGS += -Wno-unused-local-typedef -Wno-error=gnu-zero-variadic-macro-arguments
} else {
        QMAKE_CXXFLAGS += -Wno-unused-local-typedefs -Wno-error=variadic-macros
}

QMAKE_CXXFLAGS += -Wno-error=pedantic

QMAKE_CXXFLAGS += \
        -isystem $$PWD/googletest/googlemock/include \
        -isystem $$PWD/googletest/googletest/include \

LIBS += -ltrikRuntime-googletest$$CONFIGURATION_SUFFIX
