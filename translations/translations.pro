# Copyright 2015 CybrTech Labs Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TEMPLATE = subdirs

include(../global.pri)

win32 {
	DESTDIR ~= s,/,\\,g
	system(cmd.exe /C "FOR /R %G in (*.ts) do lrelease -removeidentical %G")
	system("cmd.exe /C \"xcopy *.qm $$DESTDIR\\translations\\ /s /e /y\"")
	system("cmd.exe /C \"xcopy *.ini $$DESTDIR\\translations\\ /s /e /y\"")
	system(cmd.exe /C "DEL /s *.qm")
}
else {
	system(find $$system_quote($$PWD/) -name '*.qm' -delete)
	system(find $$system_quote($$PWD) -name '*.ts' -print0 | xargs -0 $$[QT_HOST_BINS]/lrelease -removeidentical)
	system(mkdir -p $$DESTDIR/translations/; find ./ -name *.qm -exec rsync --remove-source-files -R {} $$DESTDIR/translations \\;)
	system(mkdir -p $$DESTDIR/translations/; find ./ -name *.ini -exec rsync -R {} $$DESTDIR/translations \\;)
}

OTHER_FILES += \
	$$PWD/ru/locale.ini \
	$$PWD/fr/locale.ini \

OTHER_FILES += \
	$$PWD/ru/*.ts \
	$$PWD/fr/*.ts \

installAdditionalSharedFiles($$DESTDIR/translations)
