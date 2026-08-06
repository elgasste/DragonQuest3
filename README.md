# DragonQuest3

A recreation of Dragon Quest 3 written in C

## Building in VSCode

You'll need to install the following extensions:

- Microsoft CMake tools
- Microsoft C/C++ Extensions

Once these are installed, you can select a CMake compiler kit by opening the command palette (`ctrl+shift+p` in Windows) and typing `CMake: Select a Kit`).

The next step is creating a `launch.json` file in the `.vscode` directory in the project's root folder. This will allow you to easily launch a debugger, here's an example of a `launch.json` file, with an accompanying `tasks.json` file (this will ensure the project is built every time you try to debug):

```
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug DragonQuest3",
      "type": "cppvsdbg",
      "request": "launch",
      "program": "${workspaceFolder}/build/Debug/DragonQuest3.exe",
      "cwd": "${workspaceFolder}",
      "console": "integratedTerminal",
      "stopAtEntry": false,
      "preLaunchTask": "build DragonQuest3"
    }
  ]
}
```

```
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "build DragonQuest3",
      "type": "shell",
      "command": "cmake",
      "args": [
        "--build",
        "${workspaceFolder}/build",
        "--config",
        "Debug"
      ],
      "options": {
        "cwd": "${workspaceFolder}"
      },
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "problemMatcher": "$msCompile"
    }
  ]
}
```

## Building in Visual Studio 2022

This is actually really easy, just select "Open a local folder", and everything should be done. Select any target executable at the top, and it should just work.

## Key Mappings

### Game Keys

In Windows, the following keys are bound to the equivalent of the NES controller buttons:

- NES A: keyboard X
- NES B: keyboard Z
- NES Start: keyboard Enter
- NES Select: keyboard Shift
- NES D-Pad: keyboard Arrow Keys

### Debug Keys

In Windows, these keys are bound to the following operations:

- `ctrl + m`: prints the current memory usage to the log file.
- `F8`: toggle the diagnostics view.
- `ctrl + s + up-arrow`: increase the graphics scale in increments of 0.5x, up to 4.0x.
- `ctrl + s + down-arrow`: decrease the graphics scale in increments of 0.5x, down to 0.5x.
- `ctrl + f + up-arrow`: increase the game's frame rate in increments of 15 FPS, up to 120 FPS.
- `ctrl + f + down-arrow`: decrease the game's frame rate in increments of 15 FPS, down to 15 FPS.

The default values for these settings are as follows:

- Graphics scale: 2.0
- Frame Rate: 60 FPS
