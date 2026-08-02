# DragonQuest3

A recreation of Dragon Quest 3 written in C

## Building in VSCode

You'll need to install the following extensions:

- Microsoft CMake tools
- Microsoft C/C++ Extensions

Once these are installed, you can select a CMake compiler kit by opening the command palette (`ctrl+shift+p` in Windows) and typing `CMake: Select a Kit`).

The next step is creating a `launch.json` file in the `.vscode` directory in the project's root folder. This will allow you to easily launch a debugger, here's an example of a `launch.json` file:

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
      "stopAtEntry": false
    }
  ]
}
```

## Building in Visual Studio 2022

This is actually really easy, just select "Open a local folder", and everything should be done. Select any target executable at the top, and it should just work.
