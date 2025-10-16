# Firewood Bank

Minimal skeleton for a Qt 6 + CMake desktop app with SQLite.

## Prerequisites
- Qt 6 (Widgets, Sql) installed and in PATH
- CMake 3.21+
- A C++17 compiler (MSVC on Windows, Clang/GCC on macOS/Linux)

## Build

### All Platforms
```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/<compiler> ..
cmake --build . --config Release
```

### Platform-Specific Examples

**Windows (PowerShell):**
```powershell
cmake -DCMAKE_PREFIX_PATH="C:/Qt/6.6.0/msvc2019_64" ..
```

**macOS:**
```bash
cmake -DCMAKE_PREFIX_PATH="$HOME/Qt/6.6.0/macos" ..
```

**Linux:**
```bash
cmake -DCMAKE_PREFIX_PATH="$HOME/Qt/6.6.0/gcc_64" ..
```

If Qt is installed system-wide or in PATH, you may omit `CMAKE_PREFIX_PATH`.

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
