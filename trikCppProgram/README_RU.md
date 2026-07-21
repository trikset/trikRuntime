## Как написать и скомпилировать программу на C++ для TRIK

### Шаг 1 — Установка SDK

Скачайте и запустите установщик SDK:

```bash
wget https://dl.trikset.com/distro/latest-full/trik-sdk-x86_64-arm926ejse-toolchain-trik-nodistro.0.sh

chmod +x trik-sdk-x86_64-arm926ejse-toolchain-trik-nodistro.0.sh

./trik-sdk-x86_64-arm926ejse-toolchain-trik-nodistro.0.sh
```

Перед сборкой активируйте окружение кросс-компиляции:

```bash
. /opt/trik-sdk/environment-setup-arm926ejse-oe-linux-gnueabi
```

Эта команда настраивает кросс-компилятор, sysroot и qmake под целевую платформу контроллера TRIK.

---

### Шаг 2 — Написание программы

Ваша программа — это C++ класс, наследующий `trikScriptRunner::TrikCppProgram`.

Включите заголовочный файл (уже присутствует в директории `trikCppProgram/`):

```cpp
#include "trikCppProgramInterface.h"
```

#### Обязательная структура

```cpp
#include "trikCppProgramInterface.h"

class MyProgram : public trikScriptRunner::TrikCppProgram
{
public:
    using TrikCppProgram::TrikCppProgram; // наследуем конструктор — обязательно

    void startProgram(const std::atomic<bool> &isTerminate) override
    {
        // Логика управления роботом.
        // Проверяйте isTerminate в каждой итерации цикла:
        while (!isTerminate) {
            mBrick->led()->red();
            mScriptControl->wait(500);
            mBrick->led()->green();
            mScriptControl->wait(500);
        }
    }
};

// Регистрирует класс как точку входа библиотеки — обязательно, один раз на файл.
TRIK_REGISTER_CPP_PROGRAM(MyProgram)
```

#### Доступные поля (унаследованы от `TrikCppProgram`)

| Поле | Тип | Назначение |
|---|---|---|
| `mBrick` | `trikControl::BrickInterface*` | Моторы, датчики, светодиоды, дисплей, камера |
| `mMailbox` | `trikNetwork::MailboxInterface*` | Межроботный обмен сообщениями |
| `mScriptControl` | `TrikScriptControlInterface*` | `wait()`, `quit()`, `random()` и др. |

Также доступен метод `getPhoto()`, возвращающий масштабированное изображение с камеры.

#### Контракт `startProgram`

- Вызывается однократно в отдельном потоке в момент запуска программы пользователем.
- `isTerminate` устанавливается в `true` средой выполнения, когда пользователь запрашивает остановку (например, нажатием кнопки «Стоп»). Проверяйте его в каждом цикле.
- Программа завершается, когда `startProgram` возвращает управление.

#### `TRIK_REGISTER_CPP_PROGRAM(ClassName)`

Макрос генерирует фабричную функцию, которую среда выполнения ищет в `.so` при загрузке:

```cpp
extern "C" TrikCppProgram* createTrikCppProgram(BrickInterface*, MailboxInterface*, TrikScriptControlInterface*);
```

Без него среда выполнения не сможет загрузить вашу программу. Разместите макрос ровно один раз, после определения класса.

---

### Шаг 3 — Сборка

Сгенерируйте Makefile с помощью qmake (из директории `trikCppProgram/` или с полным путём):

```bash
qmake TARGET=myprogram /путь/к/trikRuntime/trikCppProgram/trikCppProgram.pro

make -j$(nproc)
```

`TARGET` задаёт имя библиотеки. Если не указан, используется имя по умолчанию `trikCppProgram`.

Результат сборки помещается в `trikCppProgram/bin/`.

---

### Шаг 4 — Загрузка и запуск

Перенесите файл `.so` на контроллер и запустите его из файлового менеджера TRIK.
