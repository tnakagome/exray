# Introduction

This is a pluggable shared library for C++ exception analysis at run-time. There is no need to rebuild target applications. You can attach the library to existing applications at start-up very easily, and see the origin and type of a C++ exception thrown as well as where it is caught. LD_PRELOAD environment variable will do all the magic for you.

It is also very easy to add call stack dumps to arbitrary C/C++ functions by creating wrapper functions around them. Supporting classes and functions already exist in this library.

# Usage Scenario

A customer has a fairly large C++ program deployed in the production system. It continues to dump cores every once in a while. Your developers have analyzed cores, but what they tell you is that the cores are due to unhandled exceptions and they cannot do much about it. It is practically impossible to create debug binaries for analysis. After all, the customer will not allow you to casually swap binaries in the production environment. One of the developers have gave you a debugger script and suggested to trace the execution by attaching a debugger to the running process, but it only slowed down the execution significantly and the problem disappeared magically. Customer's pressure is escalating as you are unable to stop the core. You want to get a clue as to the cause of cores so your developers can create a patch.

libexray.so can help you in such a case. By restarting your program with libexray.so attached by LD_PRELOAD, you can see C++ exception information that the program throws. Run the program until it cores, then give the output along with the core file to your developers. It may help them identify the source of the problem and hopefully fix the core.

# Supported Platforms

Generally libexray should work on all modern Linux distributions.

Tested and supported distributions are:

- Red Hat Enterprise Linux 5.5 and higher, including clones
- Ubuntu 16 and higher

The library is compiled in C++11 mode by default, but C++98 mode is also supported. See below for the procedure.

# Compatibility

There are known compatibility issues with other libraries.

- libunwind.so - libexray.so may not be compatible with libunwind.so. The target process can freeze if both libraries are used at the same time. There is no workaround.

- jemalloc - libexray.so will only work with jemalloc if you do NOT interpose pthread\_mutex functions. src/interpose/PThread.cpp enables interposing of some of the pthread functions, but they are excluded from the build by default. In order to enable pthread interposition, examine src/interpose/PThread.cpp carefully, possibly remove unnecessary wrapper functions, and add it into the src/interpose/modules.mk file.

# Build
In order to build the library, sync the source code to your Linux box and run `make all`. It will generate libexray.so.

The binary is 64-bit by default. In case you need a 32-bit version of libexray, you can configure the bitness of the build to 32. Edit the `BUILD_MODE` at the beginning of the Makefile at the top level.

Support for `std::rethrow_exception()` (C++11) is enabled by default. In order to disable C++11 support and to compile libexray in C++98 mode, comment out the `CPP_MODE` in the Makefile at the top level. (The existence of `rethrow_exception()` in libexray should not interfere with C++98 binary execution though.)

# Usage
You can attach the library to an application when you run it, like this

    $ LD_PRELOAD=./libexray.so <command line>

If your target program forks, or is a wrapper script, export the LD_PRELOAD variable.

    $ export LD_PRELOAD=./libexray.so <command line>

# Example
Real-world application examples of libexray are in the [Wiki](https://github.com/tnakagome/exray/wiki).

Below is a sample exception thrown and caught within libreoffice.

This application is chosen as an example because it is one of a few, popular applications that can demonstrate the power of libexray.

Each line begins with PID[ThreadID].

    $ export LD_PRELOAD=./libexray.so libreoffice

    ...
	7560[7560] Origin of Dump: __cxa_throw
	7560[7560] Exception Time: 15:58:16.672872
	7560[7560] Exception Type: com::sun::star::ucb::InteractiveAugmentedIOException
	7560[7560] Stack Frames
	7560[7560] #1: ./libexray.so(__cxa_throw+0x6b) [0x7f832cd9be4f]
	7560[7560] #2: /usr/lib64/libreoffice/ure/lib/libgcc3_uno.so(+0x5c98) [0x7f830fc61c98]
	7560[7560] #3: /usr/lib64/libreoffice/ure/lib/libgcc3_uno.so(+0x3b13) [0x7f830fc5fb13]
	7560[7560] #4: /usr/lib64/libreoffice/ure/lib/libgcc3_uno.so(+0x3fcb) [0x7f830fc5ffcb]
	7560[7560] #5: /usr/lib64/libreoffice/ure/lib/libgcc3_uno.so(privateSnippetExecutor+0x76) [0x7f830fc67c1e]
	7560[7560] #6: /usr/lib64/libreoffice/program/../ure-link/lib/libuno_cppuhelpergcc3.so.3(cppu::throwException(com::sun::star::uno::Any const&)+0x23a) [0x7f832ae4a41a]
	7560[7560] #7: /usr/lib64/libreoffice/program/libucbhelper.so(ucbhelper::cancelCommandExecution(com::sun::star::ucb::IOErrorCode, com::sun::star::uno::Sequence<com::sun::star::uno::Any> const&, com::sun::star::uno::Reference<com::sun::star::ucb::XCommandEnvironment> const&, rtl::OUString const&, com::sun::star::uno::Reference<com::sun::star::ucb::XCommandProcessor> const&)+0x198) [0x7f83288d4218]
	7560[7560] #8: /usr/lib64/libreoffice/program/../program/libucpfile1.so(+0x1e85e) [0x7f8306bee85e]
	7560[7560] #9: /usr/lib64/libreoffice/program/../program/libucpfile1.so(+0x4177a) [0x7f8306c1177a]
	7560[7560] #10: /usr/lib64/libreoffice/program/../program/libucpfile1.so(+0x15fc4) [0x7f8306be5fc4]
	7560[7560] #11: /usr/lib64/libreoffice/program/libucbhelper.so(+0x2a525) [0x7f83288c5525]
	7560[7560] #12: /usr/lib64/libreoffice/program/libucbhelper.so(ucbhelper::Content::executeCommand(rtl::OUString const&, com::sun::star::uno::Any const&)+0x9c) [0x7f83288c82cc]
	7560[7560] #13: /usr/lib64/libreoffice/program/libutllo.so(+0x10652f) [0x7f832865452f]
	7560[7560] #14: /usr/lib64/libreoffice/program/libutllo.so(utl::UcbLockBytes::CreateLockBytes(com::sun::star::uno::Reference<com::sun::star::ucb::XContent> const&, com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> const&, unsigned short, com::sun::star::uno::Reference<com::sun::star::task::XInteractionHandler> const&, utl::UcbLockBytesHandler*)+0x534) [0x7f8328656d74]
	7560[7560] #15: /usr/lib64/libreoffice/program/libutllo.so(+0x10c9e7) [0x7f832865a9e7]
	7560[7560] #16: /usr/lib64/libreoffice/program/libutllo.so(utl::UcbStreamHelper::CreateStream(rtl::OUString const&, unsigned short, utl::UcbLockBytesHandler*)+0x31) [0x7f832865b9a1]
	7560[7560] #17: /usr/lib64/libreoffice/program/libvcllo.so(GraphicFilter::ImportGraphic(Graphic&, INetURLObject const&, unsigned short, unsigned short*, unsigned int)+0x71) [0x7f832817c411]
	7560[7560] #18: /usr/lib64/libreoffice/program/libvcllo.so(GraphicFilter::LoadGraphic(rtl::OUString const&, rtl::OUString const&, Graphic&, GraphicFilter*, unsigned short*)+0x1c0) [0x7f832817c680]
	7560[7560] #19: /usr/lib64/libreoffice/program/libvcllo.so(+0x3c43c7) [0x7f832815e3c7]
	7560[7560] #20: /usr/lib64/libreoffice/program/libvcllo.so(+0x3c47a8) [0x7f832815e7a8]
	7560[7560] #21: /usr/lib64/libreoffice/program/libvcllo.so(StyleSettings::GetPersonaHeader() const+0x29) [0x7f832815ef19]
	7560[7560] #22: /usr/lib64/libreoffice/program/libvcllo.so(StyleSettings::GetPersonaMenuBarTextColor() const+0x27) [0x7f832815ef67]
	7560[7560] #23: /usr/lib64/libreoffice/program/libvclplug_gtklo.so(+0x4d9a4) [0x7f83166999a4]
	7560[7560] #24: /usr/lib64/libreoffice/program/libvcllo.so(+0x1cd411) [0x7f8327f67411]
	7560[7560] #25: /usr/lib64/libreoffice/program/libvcllo.so(+0x1da281) [0x7f8327f74281]
	7560[7560] #26: /usr/lib64/libreoffice/program/libvcllo.so(+0x134d63) [0x7f8327eced63]
	7560[7560] #27: /usr/lib64/libreoffice/program/libvcllo.so(+0x134fb0) [0x7f8327ecefb0]
	7560[7560] #28: /usr/lib64/libreoffice/program/libvcllo.so(+0x1e4ce8) [0x7f8327f7ece8]
	7560[7560] #29: /usr/lib64/libreoffice/program/libvcllo.so(WorkWindow::WorkWindow(Window*, long)+0x4e) [0x7f8327f7f06e]
	7560[7560] #30: /usr/lib64/libreoffice/program/libtklo.so(+0x1808cc) [0x7f8328f268cc]
	7560[7560] #31: /usr/lib64/libreoffice/program/libtklo.so(+0x181d4b) [0x7f8328f27d4b]
	7560[7560] #32: /usr/lib64/libreoffice/program/../program/libfwklo.so(+0x192238) [0x7f8306fd1238]
	7560[7560] #33: /usr/lib64/libreoffice/program/../program/libfwklo.so(+0x193f8a) [0x7f8306fd2f8a]
	7560[7560] #34: /usr/lib64/libreoffice/program/../program/libfwklo.so(+0xac579) [0x7f8306eeb579]
	7560[7560] #35: /usr/lib64/libreoffice/program/../program/libfwklo.so(+0x15cd5c) [0x7f8306f9bd5c]
	7560[7560] #36: /usr/lib64/libreoffice/program/libsofficeapp.so(+0x1a361) [0x7f832c8de361]
	7560[7560] #37: /usr/lib64/libreoffice/program/libsofficeapp.so(+0x238e3) [0x7f832c8e78e3]
	7560[7560] #38: /usr/lib64/libreoffice/program/libvcllo.so(+0x3cdd31) [0x7f8328167d31]
	7560[7560] #39: /usr/lib64/libreoffice/program/libvcllo.so(SVMain()+0x22) [0x7f8328167d62]
	7560[7560] #40: /usr/lib64/libreoffice/program/libsofficeapp.so(soffice_main+0x135) [0x7f832c90c675]
	7560[7560] #41: /usr/lib64/libreoffice/program/soffice.bin() [0x40071b]
	7560[7560] #42: /lib64/libc.so.6(__libc_start_main+0xf5) [0x7f832c523b15]
	7560[7560] #43: /usr/lib64/libreoffice/program/soffice.bin() [0x400751]
	7560[7560] ------------------------------------------------------------
	7560[7560] Origin of Dump: __cxa_begin_catch
	7560[7560] Exception Time: 15:58:16.674866
	7560[7560] Stack Frames
	7560[7560] #1: ./libexray.so(__cxa_begin_catch+0x55) [0x7f832cd9bec6]
	7560[7560] #2: /usr/lib64/libreoffice/program/libutllo.so(+0x108491) [0x7f8328656491]
	7560[7560] #3: /usr/lib64/libreoffice/program/libutllo.so(utl::UcbLockBytes::CreateLockBytes(com::sun::star::uno::Reference<com::sun::star::ucb::XContent> const&, com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> const&, unsigned short, com::sun::star::uno::Reference<com::sun::star::task::XInteractionHandler> const&, utl::UcbLockBytesHandler*)+0x534) [0x7f8328656d74]
	7560[7560] #4: /usr/lib64/libreoffice/program/libutllo.so(+0x10c9e7) [0x7f832865a9e7]
	7560[7560] #5: /usr/lib64/libreoffice/program/libutllo.so(utl::UcbStreamHelper::CreateStream(rtl::OUString const&, unsigned short, utl::UcbLockBytesHandler*)+0x31) [0x7f832865b9a1]
	7560[7560] #6: /usr/lib64/libreoffice/program/libvcllo.so(GraphicFilter::ImportGraphic(Graphic&, INetURLObject const&, unsigned short, unsigned short*, unsigned int)+0x71) [0x7f832817c411]
	7560[7560] #7: /usr/lib64/libreoffice/program/libvcllo.so(GraphicFilter::LoadGraphic(rtl::OUString const&, rtl::OUString const&, Graphic&, GraphicFilter*, unsigned short*)+0x1c0) [0x7f832817c680]
	7560[7560] #8: /usr/lib64/libreoffice/program/libvcllo.so(+0x3c43c7) [0x7f832815e3c7]
	7560[7560] #9: /usr/lib64/libreoffice/program/libvcllo.so(+0x3c47a8) [0x7f832815e7a8]
	7560[7560] #10: /usr/lib64/libreoffice/program/libvcllo.so(StyleSettings::GetPersonaHeader() const+0x29) [0x7f832815ef19]
	7560[7560] #11: /usr/lib64/libreoffice/program/libvcllo.so(StyleSettings::GetPersonaMenuBarTextColor() const+0x27) [0x7f832815ef67]
	7560[7560] #12: /usr/lib64/libreoffice/program/libvclplug_gtklo.so(+0x4d9a4) [0x7f83166999a4]
	7560[7560] #13: /usr/lib64/libreoffice/program/libvcllo.so(+0x1cd411) [0x7f8327f67411]
	7560[7560] #14: /usr/lib64/libreoffice/program/libvcllo.so(+0x1da281) [0x7f8327f74281]
	7560[7560] #15: /usr/lib64/libreoffice/program/libvcllo.so(+0x134d63) [0x7f8327eced63]
	7560[7560] #16: /usr/lib64/libreoffice/program/libvcllo.so(+0x134fb0) [0x7f8327ecefb0]
	7560[7560] #17: /usr/lib64/libreoffice/program/libvcllo.so(+0x1e4ce8) [0x7f8327f7ece8]
	7560[7560] #18: /usr/lib64/libreoffice/program/libvcllo.so(WorkWindow::WorkWindow(Window*, long)+0x4e) [0x7f8327f7f06e]
	7560[7560] #19: /usr/lib64/libreoffice/program/libtklo.so(+0x1808cc) [0x7f8328f268cc]
	7560[7560] #20: /usr/lib64/libreoffice/program/libtklo.so(+0x181d4b) [0x7f8328f27d4b]
	7560[7560] #21: /usr/lib64/libreoffice/program/../program/libfwklo.so(+0x192238) [0x7f8306fd1238]
	7560[7560] #22: /usr/lib64/libreoffice/program/../program/libfwklo.so(+0x193f8a) [0x7f8306fd2f8a]
	7560[7560] #23: /usr/lib64/libreoffice/program/../program/libfwklo.so(+0xac579) [0x7f8306eeb579]
	7560[7560] #24: /usr/lib64/libreoffice/program/../program/libfwklo.so(+0x15cd5c) [0x7f8306f9bd5c]
	7560[7560] #25: /usr/lib64/libreoffice/program/libsofficeapp.so(+0x1a361) [0x7f832c8de361]
	7560[7560] #26: /usr/lib64/libreoffice/program/libsofficeapp.so(+0x238e3) [0x7f832c8e78e3]
	7560[7560] #27: /usr/lib64/libreoffice/program/libvcllo.so(+0x3cdd31) [0x7f8328167d31]
	7560[7560] #28: /usr/lib64/libreoffice/program/libvcllo.so(SVMain()+0x22) [0x7f8328167d62]
	7560[7560] #29: /usr/lib64/libreoffice/program/libsofficeapp.so(soffice_main+0x135) [0x7f832c90c675]
	7560[7560] #30: /usr/lib64/libreoffice/program/soffice.bin() [0x40071b]
	7560[7560] #31: /lib64/libc.so.6(__libc_start_main+0xf5) [0x7f832c523b15]
	7560[7560] #32: /usr/lib64/libreoffice/program/soffice.bin() [0x400751]
    ...

# Options

You can control the behaviour of the library by configuring options as an environment variable like

    $ export LIBEXRAY=exitonly,demangle

before invoking your target application with LD_PRELOAD.

Use comma as delimiter when specifying multiple options.

### Option Values

- `demangle` : Demangle function names within stack frames whenever possible. This is disabled by default.
- `exitonly` : Only write the latest exception call stacks when `exit()` is called in the target application, and all exceptions after `exit()` is called.
- `logfile=filename` : The library writes information to stderr by default. This option redirects the output to specified file.
- `maxframes=n` : Limit the number of frames written to this value in each dump. Hardcoded limit is 100 frames.
- `outputfilter=exception-name|...` : Pipe-separated (`|`) list of name of exceptions that will be excluded from output. Useful when you see numerous number of insignificant exceptions. You can write partial name of exceptions. For example, "InteractiveAugmentedIOException" will match the exception name in the above example.
- `pthread` : Enable stack frame dumps from pthread functions. This only takes effect when you include src/interpose/PThead.cpp into the build (modules.mk). See the .cpp file for the target pthread functions.
- `rindex` : Reverse the index for each frame. Frame index starts from 1 by default. This option will reverse the index. It may be easier to get the number of frames unwound between the throw and catch stack dumps for an exception.

# Symbol Demangling
C++ function names in the stack traces are mangled unless you use the demangle option. For example, you may see this function name in a back trace from libreoffice:

	_ZN9ucbhelper7Content16getPropertyValueERKN3rtl8OUStringE

If you want to derive the original function name from this mangled name, use c++filt command like this:

	$ c++filt _ZN9ucbhelper7Content16getPropertyValueERKN3rtl8OUStringE
	ucbhelper::Content::getPropertyValue(rtl::OUString const&)

# Tips
- If your target application throws tons of exceptions and stacks are deep, this library can slow down its execution significantly due to the amount of output it generates. (Frame dumps are serialized by a global mutex.) In such cases, filter out unnecessary exceptions by the `outputfilter` option and limit the number of frames by the `maxframes` option.

- In case you want to enhance this library, it is not possible to interpose functions within the ELF executable itself, i.e., the program with `main()` function. Only functions defined in shared libraries can be interposed and analyzed.

- If log file is empty and you see stack frames in your console, the target program may have closed all file descriptors including the one that this library had opened for logging. In this case, try using subshell to redirect all outputs to a file with parenthesis.

	`$ ( LD_PRELOAD=./libexray.so libreoffice ) > exray-output.txt 2>&1`

# Mechanism
Fundamentally this is a collection of wrappers around OS library functions including `exit()`, `throw`, `catch` and `std::rethrow_exception()`. Underneath the wrappers are building blocks for capturing and writing stack frames at an arbitrary execution point.

LD_PRELOAD and `dlsym()` are OS facilities that allow you to add functionality to or even completely replace functions in shared libraries. With the help of these facilities, this library grabs calls to C++ throw and catch statements in libstdc++.so.

g++ translates `throw` and `catch` statements into these function calls, which reside in libstdc++.so.

    __cxa_throw()
    __cxa_begin_catch()

libexray.so adds back trace functionality to these functions and `std::rethrow_exception()` as well as `exit()` by interposing calls to them and writing back traces from there.

For more details on C++ exception handling, this is the reference: https://itanium-cxx-abi.github.io/cxx-abi/abi-eh.html
