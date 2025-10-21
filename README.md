# ShadowVMT

A C++ class to perform shadow VMT hooking, allowing you to hook and unhook virtual methods safely by creating a copy of the target class's virtual method table (VMT).

## Features

- Create a shadow VMT for a given class instance
- Hook specific virtual methods by index
- Unhook and restore original VMT
- Unook specific virtual methods by index
- Call original methods by index
