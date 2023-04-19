<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Issues][issues-shield]][issues-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<p align="center">
  <img src="./images/CheckBox.gif" alt="animated" />
</p>

  <h3 align="center">One Stop Shop To "C" Spatial Safety</h3>

  <p align="center">
    This repo contains a version of the LLVM/Clang toolchain that is being modified to support Check-C-Box which from the fork of Checked C
    <br />
    <a href="https://sites.google.com/view/checkcbox/home?authuser=0"><strong>Concept»</strong></a>
    <br />
    <br />
    <a href="https://github.com/purs3lab/CheckC-Box">View Examples</a>
    ·
    <a href="https://github.com/arunkumarbhattar/CheckCBox_Compiler/issues">Report Bug</a>
    ·
    <a href="https://github.com/arunkumarbhattar/CheckCBox_Compiler/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#what-is-rlbox">What is RLBOX?</a></li>
      </ul>
      <ul>
        <li><a href="#what-is-checked-c">What is Checked-C?</a></li>
      </ul>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project
We intend to fuse the goodness of Checked-C and RL Sandboxing to make C more secure than ever. 
While Microsoft's Checked-C already takes care of Spatial safety by encapsulating C code to Checked-C regions. 
What about "Unchecked" regions?, How can we guarantee spatial safety for unchecked regions?

Thats where we intend to use RLBOX Sandboxing. 
With a fusion of the above two, we intend to enforce spatial safety to a Legacy C's codebase. 

Specifically, our plan is to **encapsulate unchecked regions of code into a RLBOX sandbox and allow policed communication between checked and unchecked regions with minimal overhead**.

## Background
### What is RLBOX?
RLBox ensures that a sandboxed library is memory isolated from the rest of the application. The library cannot directly access memory outside its designated region; and that all boundary crossings are explicit. This ensures that the library cannot, for example, corrupt program's address space. It also ensures that Firefox cannot inadvertently expose sensitive data to the library

[More Details](docs/background/rlbox.md)

### What is Checked-C?
Checked C extends C with checked pointer types which are restricted by the compiler to spatially safe uses.
Such pointers have one of three possible types, `ptr<T>` , `array_ptr<T>` ,
or `nt_array_ptr<T>` representing a pointer to a single element, array
of elements, or null-terminated array of elements of type `T`, respectively. The latter two have an
associated bounds annotation; e.g., a declaration `array_ptr<int> p : count(n)` says that `p` is a pointer
to an int array whose size is `n` . Checked C's Clang/LLVM-based compiler represents checked
pointers as system-level memory words, i.e., without “fattening” metadata ensuring backward
compatibility. 
**The bounds annotations are used to add dynamic checks on corresponding pointer
accesses to prevent spatial safety violations. Furthermore, these run-time checks are optimized
by LLVM, yielding good performance.**

[More Details](docs/background/checkedc.md)

## Our Idea
When a program is ported to completly converted to Checked C, **spatial safety is guaranteed**.
However, converting to Checked C requires some effort, and the developer may not be able to convert the entire codebase to Checked C.
As Checked C is backward compatible, a programmer is able to designate regions of code—whole files, single functions, or even single
blocks of code—**as checked regions**;
Such a region must contain only checked pointer types and adhere to a few other restrictions. The region is sure to be spatially safe.
We call the other not converted regions as **unchecked regions**.
However, there are no protections in unchecked regions, and thus bugs in unchecked regions can affect the execution of code in checked regions. 

To handle this, we use RLBOX. Specifically, **all unchecked functions (or unchecked regions) will be encapsulated into a RLBOX sandbox and will convert the calls to unchecked function as sandbox calls**.


<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

* Professor/Lead: [Dr. Aravind Machiry](https://machiry.github.io/)

* Lab: [PurS3](https://purs3lab.github.io/)

Researcher(s):
 * Arun Kumar Bhattar

## Testing out CheckCBox

Programmers are welcome to use CheckCBox as it is being implemented.  You will
have to build your own copy of the compiler. 
Below are the directions on how to do it.

Step 1: Switch to work Directory
```
cd <WORK_DIR>
```

Step 2: Clone the repository
```
git clone https://github.com/arunkumarbhattar/CheckCBox_Compiler.git
```

Step 3: Import the CheckCBox tests and headers from a different repository.
Change to the src/llvm/projects/checkedc-wrapper directory and clone the Checked C repo.

```
cd CheckCBox_Compiler/llvm/projects/checkedc-wrapper/
git clone https://github.com/secure-sw-dev/checkedc.git
```

Then switch to the below directory and clone the below 
```
cd CheckCBox_Compiler/llvm/projects/checkcbox-wrapper/
git clone https://github.com/arunkumarbhattar/checkcbox
```


Step 4: Now create a build directory generate the Cmake files 

```
cd <WORK_DIR>/build
cmake -G Ninja -DLLVM_ENABLE_PROJECTS=clang -DLLVM_ENABLE_RUNTIMES=compiler-rt -DCMAKE_LINKER=/usr/bin/gold DCMAKE_BUILD_TYPE=Debug -DLLVM_LIT_ARGS=-v -DLLVM_PARALLEL_LINK_JOBS=1 ../llvm/
```

Step 6: Now you are all set to build the target 
Execute any of the following command (based on your requirement) in the build directory 
```
ninja // this command will build the compiler and all other supporting tools
```
```
ninja clang //this command will only build the compiler
```
```
ninja check-checkcbox // This command will run all the sanity test cases for checkcbox project 
```
```
ninja check-checkedc // This command will run all the sanity test cases for checked project 
```
```
ninja check-clang // This command will run all the sanity test cases for clang frontend
```
Step 5 (Optional) Sanitizers --> 
```
First compile target llvm-config
cd <compiler-rt> // Switch to compiler-rt directory
cmake .
make install
export ASAN_SYMBOLIZER_PATH=<pathToLLVMSymbolizer> // For meaningful ASAN backtrace
```
## Testing Playground for CheckCBox

After the checkcbox tests have been cloned in the src/llvm/projects/checkcbox-wrapper directory by performing git clone https://github.com/arunkumarbhattar/checkcbox.git in the above mentioned directory

You are presented with a comprehensive test case suit that test various aspets of checkcbox.

Please switch to https://github.com/arunkumarbhattar/checkcbox/tree/main/tests directory in your respective project folder.

From here you can switch to directory of any particular sub-feature you are interested in and add/modify code to test the operation of your interest.
After the file has been changed, you can directory compile the target "check-checkcbox" in your build directory to test the changes.


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/arunkumarbhattar/CheckCBox_Compiler
[contributors-url]: https://github.com/arunkumarbhattar/CheckCBox_Compiler/pulse
[forks-shield]: https://img.shields.io/github/forks/arunkumarbhattar/CheckCBox_Compiler?style=social
[forks-url]: https://github.com/arunkumarbhattar/CheckCBox_Compiler/network/members
[issues-shield]: https://img.shields.io/bitbucket/issues/arunkumarbhattar/CheckCBox_Compiler
[issues-url]: https://github.com/arunkumarbhattar/CheckCBox_Compiler/issues
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/arun-b-093693148
[product-screenshot]: images/screenshot.png

# The CheckCBox clang repo

This repo contains a version of the LLVM/Clang toolchain that is being modified
to support the CheckCBox fork of Checked C. Checked C extends
C with checking to detect or prevent common programming errors such as out-of-bounds memory accesses.
The SSDP fork of the Checked
C specification is available at the
[SSDP Checked C repo](https://github.com/secure-sw-dev/checkedc).


