# WUSS
Wallet using SGX SDK - Project for PV204

## Prerequisites and Setup
You need SGX SDK installed, you can find it [here](https://github.com/intel/linux-sgx),
for simulation mode, only SDK is required, no drivers needed.

Another requirement is `Boost`, we use it for `program_options` and `process`.

We also use `Catch2`, but it's downloaded automatically.

Before running cmake, either set `SGX_SDK` enviroment variable, or pass  
`-DSGX_DIR=path/to/sgxsdk-install-dir` when running cmake.

We recommend using `gcc`

## Usage
Build produces `wuss` executable, run with `--help` to see available options.
The program stores wallet file inside build directory, it's called `wallet.seal` by
default.

Program is tested by running `./tests/tests`
