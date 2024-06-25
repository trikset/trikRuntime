# Copyright 2023 Nick Ponomarev
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TEMPLATE = lib

include(../global.pri)

INCLUDEPATH += $$PWD/mlx90640-library/headers

PUBLIC_HEADERS += $$PWD/mlx90640-library/headers/MLX90640_API.h

HEADERS += \
        $$PWD/mlx90640-library/headers/MLX90640_API.h \
	$$PWD/mlx90640-library/headers/MLX90640_I2C_Driver.h

SOURCES += \
        $$PWD/mlx90640-library/functions/MLX90640_API.cpp \
	$$PWD/mlx90640-library/functions/MLX90640_LINUX_I2C_Driver.cpp

# This is a foreign library with some flaws in their code
# We need to silence some errors, otherwise the build fails
QMAKE_CXXFLAGS += \
        -Wno-error=redundant-decls \
	-Wno-error=type-limits \
	-Wno-error=unused-variable \
	-Wno-error=unused-parameter
