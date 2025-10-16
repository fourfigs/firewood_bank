# Firewood Bank

Minimal skeleton for a Qt 6 + CMake desktop app with SQLite.

## Prerequisites
- Qt 6 (Widgets, Sql) installed and in PATH
- CMake 3.21+
- A C++17 compiler (MSVC on Windows, Clang/GCC on macOS/Linux)

## Build (Windows, PowerShell)
```powershell
mkdir build
cd build
cmake -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.6.0/msvc2019_64" ..
cmake --build . --config Release
```

If not using Ninja, omit the generator flag. Set `CMAKE_PREFIX_PATH` to your Qt installation.

## Run
```
./bin/firewood_bank
```

## Project Structure
- `src/app`: application entry point
- `src/ui`: UI components and main window
- `src/core`: core utilities (app name, shared helpers)
- `src/db`: database helpers (SQLite connection)

## Next Steps
See `docs/PROJECT_OUTLINE.md` and `docs/TODO.md` for planned modules and work.
