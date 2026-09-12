# *******************************************************************************
# Copyright (c) 2026 ZF Friedrichshafen AG
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# 
# Contributors:
#   Thomas Pfleiderer - initial API and implementation
# *******************************************************************************
#
# AutoAPI Component Designer

wxWidgets desktop application for reading, inspecting and writing Eclipse
autoapiframework function specifications (`*.acs`, YAML content).

## What it does

* **Read .acs file** – opens a function specification, parses it and builds
  the in-memory structures (`FunctionSpecification`, order preserving `YamlNode`
  tree).
* Displays the content on four tabs:
  * **Attributes** – `name`, `version`, `metaModelRef.*`, collection sizes and
    the full `description`.
  * **Signal collection** – entries of `dataInterfaces`, columns taken from the
    `Data` interface type of the meta model.
  * **Parameter collection** – entries of `parameters`, columns from the
    `Parameter` interface type.
  * **Scheduling collection** – entries of `scheduling`, columns from the
    `Scheduling` interface type.
  Keys that are present in the file but not in the meta model are appended as
  extra columns, so nothing is hidden.
* **Write .acs file as...** – writes the parsed content back under a new
  name in the same block style layout (key order, folded `>-` descriptions and
  quoting are preserved).

`../autoapiframework_meta_model.yaml` is located automatically on startup
(relative to the executable and the working directory); it can also be selected
manually via *Load meta model...*.

## Build

Requires CMake >= 3.16 (>= 3.21 for the presets), a C++17 compiler and
wxWidgets.

### VS Code (CMake Tools extension)

`cmake.sourceDirectory` in `.vscode/settings.json` already points to this
folder, so *CMake: Select Configure Preset* -> *Configure* -> *Build* -> *Run*
works from the repository window. Available presets: `windows-gcc` and
`linux-debug`.

### Windows

Install CMake, MSYS2 UCRT64 GCC and `mingw32-make`. Ensure
`C:\msys64\ucrt64\bin` is on `PATH`, then configure and build with GCC:

```powershell
cmake --preset windows-gcc
cmake --build --preset windows-gcc-debug
.\build\windows-gcc\bin\AutoAPIComponentDesigner.exe
```

If wxWidgets is not already installed, CMake downloads and builds wxWidgets
3.2.6 automatically. This is slow only on the first configure and is cached
afterwards. Disable it with `-DACD_FETCH_WXWIDGETS=OFF`.

### Linux

```bash
sudo apt-get install cmake libwxgtk3.2-dev
cd component_designer
cmake --preset linux-debug
cmake --build --preset linux-debug
./build/linux-debug/bin/AutoAPIComponentDesigner
```

The meta model and the sample specification are copied next to the executable
after each build.

## Sample

`../examples/speed_hazard_detection.acs` is a ready to use function
specification for trying out read, display and write.

## Sources

| File | Purpose |
| --- | --- |
| `src/main.cpp` | wxApp entry point |
| `src/main_frame.*` | Window, buttons and the four collection views |
| `src/yaml_node.*` | Order preserving YAML document tree |
| `src/yaml_parser.*` | Block style YAML subset parser |
| `src/yaml_writer.*` | Emitter reproducing the `.acs` layout |
| `src/meta_model.*` | `autoapiframework_meta_model.yaml` (enums, interface types) |
| `src/function_specification.*` | Function specification access and load/save |
