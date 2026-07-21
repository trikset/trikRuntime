## How to write and compile a C++ program for TRIK

### Step 1 — Install the SDK

Download and run the SDK installer:

```bash
wget https://dl.trikset.com/distro/latest-full/trik-sdk-x86_64-arm926ejse-toolchain-trik-nodistro.0.sh

chmod +x trik-sdk-x86_64-arm926ejse-toolchain-trik-nodistro.0.sh

./trik-sdk-x86_64-arm926ejse-toolchain-trik-nodistro.0.sh
```

Before building, activate the cross-compilation environment:

```bash
. /opt/trik-sdk/environment-setup-arm926ejse-oe-linux-gnueabi
```

This sets up the cross-compiler, sysroot, and qmake for the TRIK controller.

---

### Step 2 — Write your program

Your program is a C++ class that inherits `trikScriptRunner::TrikCppProgram`.

Include the convenience header (already present in `trikCppProgram/`):

```cpp
#include "trikCppProgramInterface.h"
```

#### Required structure

```cpp
#include "trikCppProgramInterface.h"

class MyProgram : public trikScriptRunner::TrikCppProgram
{
public:
    using TrikCppProgram::TrikCppProgram; // inherit the constructor — required

    void startProgram(const std::atomic<bool> &isTerminate) override
    {
        // Your robot logic here.
        // Check isTerminate in every loop iteration:
        while (!isTerminate) {
            mBrick->led()->red();
            mScriptControl->wait(500);
            mBrick->led()->green();
            mScriptControl->wait(500);
        }
    }
};

// Register the class as the library entry point — required, place once per file.
TRIK_REGISTER_CPP_PROGRAM(MyProgram)
```

#### Available fields (inherited from `TrikCppProgram`)

| Field | Type | Purpose |
|---|---|---|
| `mBrick` | `trikControl::BrickInterface*` | Motors, sensors, LEDs, display, camera |
| `mMailbox` | `trikNetwork::MailboxInterface*` | Inter-robot messaging |
| `mScriptControl` | `TrikScriptControlInterface*` | `wait()`, `quit()`, `random()`, etc. |

`getPhoto()` is also available as a member method returning a rescaled image from the camera.

#### `startProgram` contract

- Called once in a dedicated thread when the user launches the program.
- `isTerminate` is set to `true` by the runtime when the user requests a stop (e.g. pressing the Stop button). Check it in every loop.
- The program finishes when `startProgram` returns.

#### `TRIK_REGISTER_CPP_PROGRAM(ClassName)`

This macro generates the factory function that the runtime resolves from the `.so` at run time:

```cpp
extern "C" TrikCppProgram* createTrikCppProgram(BrickInterface*, MailboxInterface*, TrikScriptControlInterface*);
```

Without it the runtime cannot load your program. Place it exactly once, after the class definition.

---

### Step 3 — Build

Generate the Makefile with qmake (run from the `trikCppProgram/` directory, or provide the full path):

```bash
qmake TARGET=myprogram /path/to/trikRuntime/trikCppProgram/trikCppProgram.pro

make -j$(nproc)
```

`TARGET` sets the library name. If omitted, the default name `trikCppProgram` is used.

The build output is placed in `trikCppProgram/bin/`.

---

### Step 4 — Upload and run

Transfer the `.so` file to the controller and launch it from the TRIK file manager.
