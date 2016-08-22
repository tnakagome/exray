# Introduction

This is a pluggable shared library for C++ exception analysis at run-time. There is no need to rebuild target applications. You can attach the library to existing applications at start-up very easily, and see the origin and type of a C++ exception thrown as well as where it is caught. LD_PRELOAD environment variable will do all the magic for you.

The initial purpose of the library is to identify the source of exceptions that entered catch(...) {} block.

It is also very easy to add call stack dumps to arbitrary C/C++ functions by creating wrapper functions around them. Supporting classes and functions already exist in this library. 

# Supported Platforms

libexray.so is originally developed for Red Hat Enterprise Linux 5.5. It also works with RHEL 6 and 7, including their clones.

It should also work on modern Linux distributions in general.

# Compatibility

There are known compatibility issues with other libraries.

- libunwind.so - libexray.so may not be compatible with libunwind.so. The target process can freeze if both libraries are used at the same time. There is no workaround.

- jemalloc - libexray.so will only work with jemalloc if you do NOT interpose pthread\_mutex functions. src/interpose/PThread.cpp enables interposing of some of the pthread functions, but they are excluded from the build by default. In order to enable pthread interposition, examine src/interpose/PThread.cpp carefully, possibly remove unnecessary wrapper functions, and add it into the src/interpose/modules.mk file.

# Build
In order to build the library, sync the source code to your Linux box and run make all. It will generate libexray.so.

# Usage
You can attach the library to an application when you run it, like this

    $ LD_PRELOAD=./libexray.so <command line>

# Example
Below is an example of an exception thrown and caught within libreoffice.

This application is chosen as an example because it is one of a few, popular applications that can demonstrate the power of libexray.

Each line begins with PID[ThreadID].

    $ export LD_PRELOAD=./libexray.so libreoffice

    ...
    19139[19139] ------------------------------------------------------------
    19139[19139] Origin of Dump: __cxa_throw
    19139[19139] Exception Time: 22:25:09.985108
    19139[19139] Exception Type: com::sun::star::ucb::InteractiveAugmentedIOException
    19139[19139] Stack Frames
    19139[19139] #1: ./libexray.so(__cxa_throw+0x6b) [0x7feb445927c3]
    19139[19139] #2: /usr/lib64/libreoffice/ure/lib/libgcc3_uno.so(+0x5c98) [0x7feb2745ac98]
    19139[19139] #3: /usr/lib64/libreoffice/ure/lib/libgcc3_uno.so(+0x3b13) [0x7feb27458b13]
    19139[19139] #4: /usr/lib64/libreoffice/ure/lib/libgcc3_uno.so(+0x3fcb) [0x7feb27458fcb]
    19139[19139] #5: /usr/lib64/libreoffice/ure/lib/libgcc3_uno.so(privateSnippetExecutor+0x76) [0x7feb27460c1e]
    19139[19139] #6: /usr/lib64/libreoffice/program/../ure-link/lib/libuno_cppuhelpergcc3.so.3(_ZN4cppu14throwExceptionERKN3com3sun4star3uno3AnyE+0x23a) [0x7feb4264241a]
    19139[19139] #7: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper22cancelCommandExecutionEN3com3sun4star3ucb11IOErrorCodeERKNS2_3uno8SequenceINS5_3AnyEEERKNS5_9ReferenceINS3_19XCommandEnvironmentEEERKN3rtl8OUStringERKNSB_INS3_17XCommandProcessorEEE+0x198) [0x7feb400cc218]
    19139[19139] #8: /usr/lib64/libreoffice/program/../program/libucpfile1.so(+0x1e85e)     [0x7feb1e3c985e]
    19139[19139] #9: /usr/lib64/libreoffice/program/../program/libucpfile1.so(+0x4177a) [0x7feb1e3ec77a]
    19139[19139] #10: /usr/lib64/libreoffice/program/../program/libucpfile1.so(+0x15fc4) [0x7feb1e3c0fc4]
    19139[19139] #11: /usr/lib64/libreoffice/program/libucbhelper.so(+0x2a525) [0x7feb400bd525]
    19139[19139] #12: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content26getPropertyValuesInterfaceERKN3com3sun4star3uno8SequenceIN3rtl8OUStringEEE+0x17f) [0x7feb400be2ef]
    19139[19139] #13: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content17getPropertyValuesERKN3com3sun4star3uno8SequenceIN3rtl8OUStringEEE+0x2c) [0x7feb400beb6c]
    19139[19139] #14: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content16getPropertyValueERKN3rtl8OUStringE+0xa6) [0x7feb400bedf6]
    19139[19139] #15: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content8isFolderEv+0x54) [0x7feb400bf3b4]
    19139[19139] #16: /usr/lib64/libreoffice/program/libdeploymentmisclo.so(_ZN7dp_misc18create_ucb_contentEPN9ucbhelper7ContentERKN3rtl8OUStringERKN3com3sun4star3uno9ReferenceINS9_3ucb19XCommandEnvironmentEEEb+0x7e) [0x7feb423f187e]
    19139[19139] #17: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x6ea52) [0x7feaf9ea2a52]
    19139[19139] #18: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x72e03) [0x7feaf9ea6e03]
    19139[19139] #19: /usr/lib64/libreoffice/program/libcomphelper.so(_ZN10comphelper12service_decl11ServiceDecl7Factory37createInstanceWithArgumentsAndContextERKN3com3sun4star3uno8SequenceINS6_3AnyEEERKNS6_9ReferenceINS6_17XComponentContextEEE+0x3d) [0x7feb42c1147d]
    19139[19139] #20: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x88b6e) [0x7feaf9ebcb6e]
    19139[19139] #21: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x8b1bc) [0x7feaf9ebf1bc]
    19139[19139] #22: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x40898) [0x7feaf9e74898]
    19139[19139] #23: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x4abb1) [0x7feaf9e7ebb1]
    19139[19139] #24: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x4f7cf) [0x7feaf9e837cf]
    19139[19139] #25: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x2fd63) [0x7feaf9e63d63]
    19139[19139] #26: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x30252) [0x7feaf9e64252]
    19139[19139] #27: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x30439) [0x7feaf9e64439]
    19139[19139] #28: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x24940) [0x7feafaf11940]
    19139[19139] #29: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x27169) [0x7feafaf14169]
    19139[19139] #30: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x49dfc) [0x7feafaf36dfc]
    19139[19139] #31: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x4a50d) [0x7feafaf3750d]
    19139[19139] #32: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x4a555) [0x7feafaf37555]
    19139[19139] #33: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x13a26) [0x7feafaf00a26]
    19139[19139] #34: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x14551) [0x7feafaf01551]
    19139[19139] #35: /usr/lib64/libreoffice/program/libucbhelper.so(+0x2a525) [0x7feb400bd525]
    19139[19139] #36: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content26getPropertyValuesInterfaceERKN3com3sun4star3uno8SequenceIN3rtl8OUStringEEE+0x17f) [0x7feb400be2ef]
    19139[19139] #37: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content17getPropertyValuesERKN3com3sun4star3uno8SequenceIN3rtl8OUStringEEE+0x2c) [0x7feb400beb6c]
    19139[19139] #38: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content16getPropertyValueERKN3rtl8OUStringE+0xa6) [0x7feb400bedf6]
    19139[19139] #39: /usr/lib64/libreoffice/program/libsfxlo.so(+0x1a293b) [0x7feb4162f93b]
    19139[19139] #40: /usr/lib64/libreoffice/program/libsfxlo.so(+0x1a7026) [0x7feb41634026]
    19139[19139] #41: /usr/lib64/libreoffice/program/libsfxlo.so(_ZN7SfxHelp13CreateHelpURLERKN3rtl8OUStringES3_+0x28) [0x7feb416340c8]
    19139[19139] #42: /usr/lib64/libreoffice/program/libsfxlo.so(+0x1a711b) [0x7feb4163411b]
    19139[19139] #43: /usr/lib64/libreoffice/program/libsfxlo.so(_ZN7SfxHelp11GetHelpTextERKN3rtl8OUStringEPK6Window+0x4b) [0x7feb4163426b]
    19139[19139] #44: /usr/lib64/libreoffice/program/libvcllo.so(_ZNK6Window11GetHelpTextEv+0xcc) [0x7feb3f76340c]
    19139[19139] #45: /usr/lib64/libreoffice/program/libvcllo.so(_ZNK6Window24GetAccessibleDescriptionEv+0x69) [0x7feb3f6c8ac9]
    19139[19139] #46: /usr/lib64/libreoffice/program/libtklo.so(_ZN23VCLXAccessibleComponent24getAccessibleDescriptionEv+0x9c) [0x7feb406efa3c]
    19139[19139] #47: /usr/lib64/libreoffice/program/libvclplug_gtklo.so(+0x3525d) [0x7feb2de7a25d]
    19139[19139] #48: /lib64/libatk-bridge-2.0.so.0(+0x138f6) [0x7feb243628f6]
    19139[19139] #49: /lib64/libglib-2.0.so.0(g_hash_table_foreach+0x60) [0x7feb3e3db3d0]
    19139[19139] #50: /lib64/libatk-bridge-2.0.so.0(+0x13d90) [0x7feb24362d90]
    19139[19139] #51: /lib64/libatk-bridge-2.0.so.0(+0x10f84) [0x7feb2435ff84]
    19139[19139] #52: /lib64/libdbus-1.so.3(+0x1dc76) [0x7feb3a50ac76]
    19139[19139] #53: /lib64/libdbus-1.so.3(dbus_connection_dispatch+0x399) [0x7feb3a4fce49]
    19139[19139] #54: /lib64/libatspi.so.0(+0xc005) [0x7feb24134005]
    19139[19139] #55: /lib64/libglib-2.0.so.0(g_main_context_dispatch+0x15a) [0x7feb3e3eb7aa]
    19139[19139] #56: /lib64/libglib-2.0.so.0(+0x49af8) [0x7feb3e3ebaf8]
    19139[19139] #57: /lib64/libglib-2.0.so.0(g_main_context_iteration+0x2c) [0x7feb3e3ebbac]
    19139[19139] #58: /usr/lib64/libreoffice/program/libvclplug_gtklo.so(+0x38da5) [0x7feb2de7dda5]
    19139[19139] #59: /usr/lib64/libreoffice/program/libvcllo.so(_ZN11Application5YieldEv+0x5e) [0x7feb3f95b28e]
    19139[19139] #60: /usr/lib64/libreoffice/program/libvcllo.so(_ZN11Application7ExecuteEv+0x25) [0x7feb3f95b325]
    19139[19139] #61: /usr/lib64/libreoffice/program/libsofficeapp.so(+0x24063) [0x7feb440df063]
    19139[19139] #62: /usr/lib64/libreoffice/program/libvcllo.so(+0x3cdd31) [0x7feb3f95fd31]
    19139[19139] #63: /usr/lib64/libreoffice/program/libvcllo.so(_Z6SVMainv+0x22) [0x7feb3f95fd62]
    19139[19139] #64: /usr/lib64/libreoffice/program/libsofficeapp.so(soffice_main+0x135) [0x7feb44103675]
    19139[19139] #65: /usr/lib64/libreoffice/program/soffice.bin() [0x40071b]
    19139[19139] #66: /lib64/libc.so.6(__libc_start_main+0xf5) [0x7feb43d1bb15]
    19139[19139] #67: /usr/lib64/libreoffice/program/soffice.bin() [0x400751]
    19139[19139] ------------------------------------------------------------
    19139[19139] Origin of Dump: __cxa_begin_catch
    19139[19139] Exception Time: 22:25:09.986490
    19139[19139] Stack Frames
    19139[19139] #1: ./libexray.so(__cxa_begin_catch+0x55) [0x7feb4459283a]
    19139[19139] #2: /usr/lib64/libreoffice/program/libdeploymentmisclo.so(_ZN7dp_misc18create_ucb_contentEPN9ucbhelper7ContentERKN3rtl8OUStringERKN3com3sun4star3uno9ReferenceINS9_3ucb19XCommandEnvironmentEEEb+0xef) [0x7feb423f18ef]
    19139[19139] #3: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x6ea52) [0x7feaf9ea2a52]
    19139[19139] #4: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x72e03) [0x7feaf9ea6e03]
    19139[19139] #5: /usr/lib64/libreoffice/program/libcomphelper.so(_ZN10comphelper12service_decl11ServiceDecl7Factory37createInstanceWithArgumentsAndContextERKN3com3sun4star3uno8SequenceINS6_3AnyEEERKNS6_9ReferenceINS6_17XComponentContextEEE+0x3d) [0x7feb42c1147d]
    19139[19139] #6: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x88b6e) [0x7feaf9ebcb6e]
    19139[19139] #7: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x8b1bc) [0x7feaf9ebf1bc]
    19139[19139] #8: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x40898) [0x7feaf9e74898]
    19139[19139] #9: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x4abb1) [0x7feaf9e7ebb1]
    19139[19139] #10: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x4f7cf) [0x7feaf9e837cf]
    19139[19139] #11: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x2fd63) [0x7feaf9e63d63]
    19139[19139] #12: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x30252) [0x7feaf9e64252]
    19139[19139] #13: /usr/lib64/libreoffice/program/../program/libdeployment.so(+0x30439) [0x7feaf9e64439]
    19139[19139] #14: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x24940) [0x7feafaf11940]
    19139[19139] #15: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x27169) [0x7feafaf14169]
    19139[19139] #16: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x49dfc) [0x7feafaf36dfc]
    19139[19139] #17: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x4a50d) [0x7feafaf3750d]
    19139[19139] #18: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x4a555) [0x7feafaf37555]
    19139[19139] #19: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x13a26) [0x7feafaf00a26]
    19139[19139] #20: /usr/lib64/libreoffice/program/../program/libucpchelp1.so(+0x14551) [0x7feafaf01551]
    19139[19139] #21: /usr/lib64/libreoffice/program/libucbhelper.so(+0x2a525) [0x7feb400bd525]
    19139[19139] #22: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content26getPropertyValuesInterfaceERKN3com3sun4star3uno8SequenceIN3rtl8OUStringEEE+0x17f) [0x7feb400be2ef]
    19139[19139] #23: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content17getPropertyValuesERKN3com3sun4star3uno8SequenceIN3rtl8OUStringEEE+0x2c) [0x7feb400beb6c]
    19139[19139] #24: /usr/lib64/libreoffice/program/libucbhelper.so(_ZN9ucbhelper7Content16getPropertyValueERKN3rtl8OUStringE+0xa6) [0x7feb400bedf6]
    19139[19139] #25: /usr/lib64/libreoffice/program/libsfxlo.so(+0x1a293b) [0x7feb4162f93b]
    19139[19139] #26: /usr/lib64/libreoffice/program/libsfxlo.so(+0x1a7026) [0x7feb41634026]
    19139[19139] #27: /usr/lib64/libreoffice/program/libsfxlo.so(_ZN7SfxHelp13CreateHelpURLERKN3rtl8OUStringES3_+0x28) [0x7feb416340c8]
    19139[19139] #28: /usr/lib64/libreoffice/program/libsfxlo.so(+0x1a711b) [0x7feb4163411b]
    19139[19139] #29: /usr/lib64/libreoffice/program/libsfxlo.so(_ZN7SfxHelp11GetHelpTextERKN3rtl8OUStringEPK6Window+0x4b) [0x7feb4163426b]
    19139[19139] #30: /usr/lib64/libreoffice/program/libvcllo.so(_ZNK6Window11GetHelpTextEv+0xcc) [0x7feb3f76340c]
    19139[19139] #31: /usr/lib64/libreoffice/program/libvcllo.so(_ZNK6Window24GetAccessibleDescriptionEv+0x69) [0x7feb3f6c8ac9]
    19139[19139] #32: /usr/lib64/libreoffice/program/libtklo.so(_ZN23VCLXAccessibleComponent24getAccessibleDescriptionEv+0x9c) [0x7feb406efa3c]
    19139[19139] #33: /usr/lib64/libreoffice/program/libvclplug_gtklo.so(+0x3525d) [0x7feb2de7a25d]
    19139[19139] #34: /lib64/libatk-bridge-2.0.so.0(+0x138f6) [0x7feb243628f6]
    19139[19139] #35: /lib64/libglib-2.0.so.0(g_hash_table_foreach+0x60) [0x7feb3e3db3d0]
    19139[19139] #36: /lib64/libatk-bridge-2.0.so.0(+0x13d90) [0x7feb24362d90]
    19139[19139] #37: /lib64/libatk-bridge-2.0.so.0(+0x10f84) [0x7feb2435ff84]
    19139[19139] #38: /lib64/libdbus-1.so.3(+0x1dc76) [0x7feb3a50ac76]
    19139[19139] #39: /lib64/libdbus-1.so.3(dbus_connection_dispatch+0x399) [0x7feb3a4fce49]
    19139[19139] #40: /lib64/libatspi.so.0(+0xc005) [0x7feb24134005]
    19139[19139] #41: /lib64/libglib-2.0.so.0(g_main_context_dispatch+0x15a) [0x7feb3e3eb7aa]
    19139[19139] #42: /lib64/libglib-2.0.so.0(+0x49af8) [0x7feb3e3ebaf8]
    19139[19139] #43: /lib64/libglib-2.0.so.0(g_main_context_iteration+0x2c) [0x7feb3e3ebbac]
    19139[19139] #44: /usr/lib64/libreoffice/program/libvclplug_gtklo.so(+0x38da5) [0x7feb2de7dda5]
    19139[19139] #45: /usr/lib64/libreoffice/program/libvcllo.so(_ZN11Application5YieldEv+0x5e) [0x7feb3f95b28e]
    19139[19139] #46: /usr/lib64/libreoffice/program/libvcllo.so(_ZN11Application7ExecuteEv+0x25) [0x7feb3f95b325]
    19139[19139] #47: /usr/lib64/libreoffice/program/libsofficeapp.so(+0x24063) [0x7feb440df063]
    19139[19139] #48: /usr/lib64/libreoffice/program/libvcllo.so(+0x3cdd31) [0x7feb3f95fd31]
    19139[19139] #49: /usr/lib64/libreoffice/program/libvcllo.so(_Z6SVMainv+0x22) [0x7feb3f95fd62]
    19139[19139] #50: /usr/lib64/libreoffice/program/libsofficeapp.so(soffice_main+0x135) [0x7feb44103675]
    19139[19139] #51: /usr/lib64/libreoffice/program/soffice.bin() [0x40071b]
    19139[19139] #52: /lib64/libc.so.6(__libc_start_main+0xf5) [0x7feb43d1bb15]
    19139[19139] #53: /usr/lib64/libreoffice/program/soffice.bin() [0x400751]
    ...

Real-world application examples of libexray are in the [[Wiki|wiki]].

#Options

You can control the behaviour of the library by configuring options as an environment variable like

    $ export LIBEXRAY=exitonly

before invoking your target application with LD_PRELOAD.

### Option Values

- exitonly : Only write the latest call stack when exit() is called in the target application, and all exceptions after exit() is called.
- logfile=filename : The library writes information to stderr by default. This option redirects the output to specified file.
- maxframes=n : Limit the number of frames written to this value in each dump.
- outputfilter=exception-name,... : Comma-separated list of name of exceptions that will be excluded from output. Useful when you see numerous number of insignificant exceptions. you can write partial name of exceptions. For example, "InteractiveAugmentedIOException" will match the exception name in the above example.
- pthread : Enable stack frame dumps from pthread functions. This only takes effect when you include src/interpose/PThead.cpp into the build. See the .cpp file for the target pthread functions.

# Tips
- If your target application throws tons of exceptions and stacks are deep, this library can slow down its execution significantly due to the amount of output it generates. (Frame dumps are serialized by a global mutex.) In such cases, filter out unnecessary exceptions by the "outputfilter" option and limit the number of frames by the "maxframes" option.

- In case you want to enhance this library, it is not possible to interpose functions within the ELF executable itself. Only functions defined in shared libraries can be interposed and analyzed.

# Mechanism
Fundamentally this is a collection of wrappers around OS library functions including 'throw' and 'catch' handlers. Underneath the wrappers are building blocks for capturing and writing stack frames at arbitrary execution points.

LD\_PRELOAD and dlsym() are OS facilities that allow you to add functionality to or even completely replace functions in shared libraries. With the help of these facilities, this library grabs calls to C++ throw and catch statements in libstdc++.so.

G++ translates throw and catch statements into these function calls, which reside in libstc++.so.

    __cxa_throw()
    __cxa_begin_catch()

libexray.so adds back trace functionality to these functions by interposing calls to them and writing back traces from there.
