# wuss
Project for pv204

## Setup
Set `SGX_DIR` to sgxsdk directory (where you installed it)
Before running set enviroment variables  
```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SGX_SDK/sdk_libs
PATH=$PATH:$SGX_SDK/bin:$SGX_SDK/bin/x64
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$SGX_SDK/pkgconfig
SGX_SDK=/home/oncha/sgxsdk
```

in your IDE, or run `source /home/oncha/sgxsdk/environment` in the build dir.
