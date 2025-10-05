* Windows

install python from https://www.python.org/

PYTHONROOT must be a valid Python installation directory

cp Tellusim_$(ARCH).dll tellusim.pyd
cp Tellusim_$(ARCH)d.dll tellusimd.pyd

mklink tellusim.pyd ..\plugins\binding\python\TellusimPy_x64.dll
mklink tellusimd.pyd ..\plugins\binding\python\TellusimPy_x64d.dll

* Linux

sudo apt-get install python3-dev

ln -s libTellusimPy_$(ARCH).so tellusim.so
ln -s libTellusimPy_$(ARCH)d.so tellusimd.so

* macOS

brew install python

ln -s libTellusimPy_$(ARCH).dylib tellusim.so
ln -s libTellusimPy_$(ARCH)d.dylib tellusimd.so

* All OS

PYTHONPATH must contain modules directory
