<<<<<<< HEAD
# CheckCBox
CheckCBox is an extension to checked-c that guarantees spatial safety for unchecked regions of the checked-c converted code.

Checked C itself adds static and dynamic checking to C to detect or prevent common programming
errors such as buffer overruns and out-of-bounds memory accesses. 
The goal of the project is to improve systems programming by making fundamental improvements to C.
This repo contains
sample code, the [extension specification](https://github.com/Microsoft/checkedc/releases),
and test code.

- For a quick overview of Checked C, more information, and pointers to example code,
  see our [Wiki](https://github.com/Microsoft/checkedc/wiki).
- The PDF of the specification is available [here](https://github.com/Microsoft/checkedc/releases).
- Compilers are available [here](https://github.com/Microsoft/checkedc-clang/releases).
- The Checked C clang repo is
  [here](https://github.com/Microsoft/checkedc-clang).
- The instructions to build and test the Checked C compiler are documented on
  the [Checked C clang wiki](https://github.com/Microsoft/checkedc-clang/wiki).

=======
# The Checked C clang repo

This repo contains a version of the LLVM/Clang toolchain that is being modified
to support the Secure Software Development Project (SSDP) fork of Checked C. Checked C extends
C with checking to detect or prevent common programming errors such as out-of-bounds memory accesses.
The SSDP fork of the Checked
C specification is available at the
[SSDP Checked C repo](https://github.com/secure-sw-dev/checkedc).

## Announcements

### Source code update

On Feb 19, 2021 we updated the checkedc-clang sources to upstream release_110,
specifically [this](https://github.com/llvm/llvm-project/commit/2e10b7a39b930ef8d9c4362509d8835b221fbc0a) commit.

On Feb 18, 2020 we updated the checkedc-clang sources to upstream release_90,
specifically [this](https://github.com/llvm/llvm-project/commit/c89a3d78f43d81b9cff7b9248772ddf14d21b749) commit.

## Trying out Checked C

Programmers are welcome to use Checked C as it is being implemented.  You will
have to build your own copy of the compiler. For
directions on how to do this, see the [Checked C clang
wiki](https://github.com/secure-sw-dev/checkedc-llvm-project/wiki). The compiler user
manual is
[here](https://github.com/secure-sw-dev/checkedc-llvm-project/wiki/Checked-C-clang-user-manual).
For more information on Checked C and pointers to example code, see our
[Wiki](https://github.com/secure-sw-dev/checkedc/wiki).

You can use `clangd` built from this repository to get similar IDE support for
editing Checked C code as upstream `clangd` provides for C code. For example,
you can jump to definition/references and get a real-time display of errors and
warnings, etc. Here is [more information about Checked C's
`clangd`](clang/docs/checkedc/clangd.md).

## 3C: Semi-automated conversion of C code to Checked C

This repository includes a tool called 3C that partially automates the
conversion of C code to Checked C. Quick documentation links:

* [General information](clang/docs/checkedc/3C/README.md), including development
  status and how to contribute

* [Build instructions](clang/docs/checkedc/3C/INSTALL.md)

* [Usage instructions for the `3c` command-line tool](clang/tools/3c/README.md)

## More information

For more information on the SSDP Checked C clang compiler, see the [SSDP LLVM Project
wiki](https://github.com/secure-sw-dev/checkedc-llvm-project/wiki).

## Build Status

Automated builds are not currently available.

## Contributing

We welcome contributions to the Checked C project. To get involved in the
project, see [Contributing to Checked
C](https://github.com/secure-sw-dev/checkedc/blob/main/CONTRIBUTING.md).

For code contributions, we follow the standard [Github
workflow](https://guides.github.com/introduction/flow/). See [Contributing to
Checked C](https://github.com/secure-sw-dev/checkedc/blob/main/CONTRIBUTING.md)
for more detail.

## Code of conduct

This project has adopted a
[code of conduct](https://github.com/secure-sw-dev/checkedc/blob/main/CODE_OF_CONDUCT.md).
>>>>>>> ebf531a99e507a8ea5ee6c5a032948a0e7f4c159
