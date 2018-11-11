# DLL loading shellcode
This project is based off the Reflective DLL loader used in meterpreter (https://github.com/stephenfewer/ReflectiveDLLInjection). However, instead of searching backwards from the instruction pointer to find and load itself, it searches forward in memory to load an appended DLL. The advantage is that the DLL does not have to have the reflective loader compiled in.

## Usage
The solution will output 2 files:
  * loader.x86.bin
  * loader.x64.bin

These are 32 and 64 bit shellcode for loading a DLL. To use:
```
copy /b loader.x64.bin+MyModule.dll MyModule.bin
```
This will concatenate the loader shellcode and an arbitrary DLL. Point execution at the beginning and it will load the DLL and call its entry point. Make sure the shellcode and DLL architecture match.

To test the shellcode:
```
TestExe.exe MyModule.bin
```

Building TestDll will produce the file TestDll/TestDll.ARCH.CONFIG.bin which is a concatenation of the loader and a test Dll that simply calls MessageBoxA in DllMain.

