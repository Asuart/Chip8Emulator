-- premake5.lua
workspace "Chip8 Emulator"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "EmulatorApp"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "EmulatorCore/Build-Core.lua"

include "EmulatorApp/Build-App.lua"