# Assert: a cross platform drop-in + self-contained C++ assertion library
[![Build Status](https://travis-ci.org/gpakosz/Assert.png?branch=master)](https://travis-ci.org/gpakosz/Assert)

## TLDR

    #include <boost/assert.hpp>
    #include <sstream>

    int main()
    {
      float min = 0.0f;
      float max = 1.0f;
      float v = 2.0f;
      BOOST_ASSERT_MSG(v > min && v < max, static_cast<std::ostringstream&>(std::ostringstream().flush() << \
                      "invalid value: " << v << ", must be between " << min << " and " << max).str().c_str());
      return 0;
    }

**vs**

    #include <pempek_assert.h>

    int main()
    {
      float min = 0.0f;
      float max = 1.0f;
      float v = 2.0f;
      PPK_ASSERT(v > min && v < max, "invalid value: %f, must be between %f and %f", v, min, max);

      return 0;
    }

Now which do you prefer? I know which I prefer.

Just drop `pempek_assert.h` and `pempek_assert.cpp` into your build and get
started. (see also [customizing compilation])

[customizing compilation]: #customizing-compilation

## Why?

It all started with the need to provide a meaningful message when assertions
fire. There is a well-known hack with standard `assert` to inject a message next
to the expression being tested:

    assert(expression && "message");

But it's limited to string literals. I wanted improve on `assert()` by
providing the following features:

- being able to format a message that would also contain the values for
  different variables around the point of failure
- having different levels of severity
- being able to selectively ignore assertions while debugging
- being able to break into the debugger at the exact point an assertion fires
  (that is in your own source code, instead of somewhere deep inside `assert`
  implementation)
- no memory allocation
- no unused variables warning when assertions are disabled

--------------------------------------------------------------------------------

## What?

The library is designed to be lightweight would you decide to keep assertions
enabled even in release builds (`#define PPK_ASSERT_ENABLED 1`).

Each assertion eats up `sizeof(bool)` of stack, used to keep track whether the
assertion should be ignored for the remaining lifetime of the program.

### Message Formatting

The library provides `printf` like formatting:

    PPK_ASSERT(expression);
    PPK_ASSERT(expression, message, ...);

E.g:

    PPK_ASSERT(validate(v, min, max), "invalid value: %f, must be between %f and %f", v, min, max);

### Levels Of Severity

This library defines different levels of severity:

- `PPK_ASSERT_WARNING`
- `PPK_ASSERT_DEBUG`
- `PPK_ASSERT_ERROR`
- `PPK_ASSERT_FATAL`

When you use `PPK_ASSERT`, the severity level is determined by the
`PPK_ASSERT_DEFAULT_LEVEL` preprocessor token.

You can also add your own additional severity levels by using:

    PPK_ASSERT_CUSTOM(level, expression);
    PPK_ASSERT_CUSTOM(level, expression, message, ...);

### Default Assertion Handler

The default handler associates a predefined behavior to each of the different
levels:

- `WARNING <= level < DEBUG`: print the assertion message to `stderr`
- `DEBUG <= level < ERROR`: print the assertion message to `stderr` and prompt
   the user for action (disabled by default on iOS and Android)
- `ERROR <= level < FATAL`: throw an `AssertionException`
- `FATAL < level`: abort the program

If you know you're going to launch your program from within a login shell
session on iOS or Android (e.g. through SSH), define the
`PPK_ASSERT_DEFAULT_HANDLER_STDIN` preprocessor token.

When prompting for user action, the default handler prints the following
message on `stderr`:

    `Press (I)gnore / Ignore (F)orever / Ignore (A)ll / (D)ebug / A(b)ort:`

And waits for input on `stdin`:

- Ignore: ignore the current assertion
- Ignore Forever: remember the file and line where the assertion fired and
  ignore it for the remaining execution of the program
- Ignore All: ignore all remaining assertions (all files and lines)
- Debug: break into the debugger if attached, otherwise `abort()` (on Windows,
  the system will prompt the user to attach a debugger)
- Abort: call `abort()` immediately

Under the Windows platform, the default handler also uses `OutputDebugString`
and in the case of a GUI application allocates a console upon encountering the
first failed assertion.

Under the Android platform, the default handler also sends log messages to the
in-kernel log buffer, which can later be accessed through the `logcat` utility.

The default handler supports optional logging to a file (suggested by
[@nothings]):

- `#define PPK_ASSERT_LOG_FILE "/tmp/assert.txt"`
- to truncate the log file upon each program invocation, `#define
  PPK_ASSERT_LOG_FILE_TRUNCATE`

[@nothings]: https://twitter.com/nothings

### Providing Your Own Handler

If you want to change the default behavior, e.g. by opening a dialog box or
logging assertions to a database, you can provide a custom handler with the
following signature:

    typedef AssertAction::AssertAction (*AssertHandler)(const char* file,
                                                        int line,
                                                        const char* function,
                                                        const char* expression,
                                                        int level,
                                                        const char* message);

Your handler will be called with the proper information filled and needs to
return the action to be performed:

    PPK_ASSERT_ACTION_NONE,
    PPK_ASSERT_ACTION_ABORT,
    PPK_ASSERT_ACTION_BREAK,
    PPK_ASSERT_ACTION_IGNORE,
    PPK_ASSERT_ACTION_IGNORE_LINE,
    PPK_ASSERT_ACTION_IGNORE_ALL,
    PPK_ASSERT_ACTION_THROW

To install your custom handler, call:

    pempek::assert::implementation::setAssertHandler(customHandler);

### Unused Return Values

The library provides `PPK_ASSERT_USED` that fires an assertion when an unused
return value reaches end of scope:

    PPK_ASSERT_USED(int) foo();

When calling `foo()`,

    {
      foo();

      // ...

      bar();

      // ...

      baz();
    } <- assertion fires, caused by unused `foo()` return value reaching end of scope

Just like `PPK_ASSERT`, `PPK_ASSERT_USED` uses
`PPK_ASSERT_DEFAULT_LEVEL`. If you want more control on the severity, use one
of:

    PPK_ASSERT_USED_WARNING(type)
    PPK_ASSERT_USED_DEBUG(type)
    PPK_ASSERT_USED_ERROR(type)
    PPK_ASSERT_USED_FATAL(type)
    PPK_ASSERT_USED_CUSTOM(level, type)

Arguably, unused return values are better of detected by the compiler. For
instance GCC and Clang allow you to mark function with attributes:

    __attribute__((warn_unused_result)) int foo();

Which will emit the following warning in case the return value is not used:

    warning: ignoring return value of function declared with warn_unused_result attribute [-Wunused-result]

However there is no MSVC++ equivalent. Well there is `__checkReturn` but it
supposedly only have effect when running static code analysis and I failed to
make it work with Visual Studio 2013 Express. Wrapping `PPK_ASSERT_USED`
around a return type is a cheap way to debug a program where you suspect a
function return value is being ignored and shouldn't have been.

### Compile-time assertions

    PPK_STATIC_ASSERT(expression)
    PPK_STATIC_ASSERT(expression, message)

In case of compile-time assertions, the message must be a string literal and
can't be formated like with run-time assertions, e.g:

    PPK_STATIC_ASSERT(sizeof(foo) > sizeof(bar), "size mismatch");

When compiled with a C++11 capable compiler, `PPK_STATIC_ASSERT` defers to
`static_assert`. Contrary to `static_assert`, it's possible to use
`PPK_STATIC_ASSERT` without a message.

## Customizing compilation

In order to use `PPK_ASSERT` in your own project, you just have to bring in
the two `pempek_assert.h` and `pempek_assert.cpp` files. **It's that simple**.

You can customize the library's behavior by defining the following macros:

- `#define PPK_ASSERT_ENABLED 1` or `#define PPK_ASSERT_ENABLED 0`: enable
  or disable assertions, otherwise enabled state is based on `NDEBUG`
  preprocessor token being defined
- `PPK_ASSERT_DEFAULT_LEVEL`: default level to use when using the
  `PPK_ASSERT` macro
- `PPK_ASSERT_DISABLE_STL`: `AssertionException` won't inherit from
  `std::exception`
- `PPK_ASSERT_DISABLE_EXCEPTIONS`: the library won't throw exceptions on
  `ERROR` level but instead rely on a user provided `throwException` function
  that will likely `abort()` the program
- `PPK_ASSERT_MESSAGE_BUFFER_SIZE`

If you want to use a different prefix, provide your own header that includes
`pempek_assert.h` and define the following:

    // custom prefix
    #define ASSERT                PPK_ASSERT
    #define ASSERT_WARNING        PPK_ASSERT_WARNING
    #define ASSERT_DEBUG          PPK_ASSERT_DEBUG
    #define ASSERT_ERROR          PPK_ASSERT_ERROR
    #define ASSERT_FATAL          PPK_ASSERT_FATAL
    #define ASSERT_CUSTOM         PPK_ASSERT_CUSTOM
    #define ASSERT_USED           PPK_ASSERT_USED
    #define ASSERT_USED_WARNING   PPK_ASSERT_USED_WARNING
    #define ASSERT_USED_DEBUG     PPK_ASSERT_USED_DEBUG
    #define ASSERT_USED_ERROR     PPK_ASSERT_USED_ERROR
    #define ASSERT_USED_FATAL     PPK_ASSERT_USED_FATAL
    #define ASSERT_USED_CUSTOM    PPK_ASSERT_USED_CUSTOM


### Compiling for Windows

There is a Visual Studio 2012 solution in the `_win-vs11/` folder.

### Compiling for Linux or Mac

There is a GNU Make 3.81 `MakeFile` in the `_gnu-make/` folder:

    $ make -C _gnu-make/

### Compiling for Mac

See above if you want to compile from command line. Otherwise there is an Xcode
project located in the `_mac-xcode/` folder.

### Compiling for iOS

There is an Xcode project located in the `_ios-xcode/` folder.

If you prefer compiling from command line and deploying to a jailbroken device
through SSH, use:

    $ make -C _gnu-make/ binsubdir=ios CXX="$(xcrun --sdk iphoneos --find clang++) -isysroot $(xcrun --sdk iphoneos --show-sdk-path) -arch armv7 -arch armv7s -arch arm64" CPPFLAGS=-DPPK_ASSERT_DEFAULT_HANDLER_STDIN postbuild="codesign -s 'iPhone Developer'"

### Compiling for Android

You will have to install the Android NDK, and point the `$NDK_ROOT` environment
variable to the NDK path: e.g. `export NDK_ROOT=/opt/android-ndk` (without a
trailing `/` character).

Next, the easy way is to make a standalone Android toolchain with the following
command:

    $ $NDK_ROOT/build/tools/make-standalone-toolchain.sh --system=$(uname -s | tr [A-Z] [a-z])-$(uname -m) --platform=android-3 --toolchain=arm-linux-androideabi-clang3.4 --install-dir=/tmp/android-clang

Now you can compile the self test and self benchmark programs by running:

    $ make -C _gnu-make/ binsubdir=android CXX=/tmp/android-clang/bin/clang++ CXXFLAGS='-march=armv7-a -mfloat-abi=softfp -O2' LDFLAGS='-llog' CPPFLAGS=-DPPK_ASSERT_DEFAULT_HANDLER_STDIN

--------------------------------------------------------------------------------

## Credits Where It's Due:

This assertion library has been lingering in my pet codebase for years. It has
greatly been inspired by [Andrei Alexandrescu][@incomputable]'s CUJ articles:

- [Assertions][assertions]
- [Enhancing Assertions][enhancing-assertions]

[assertions]: http://www.drdobbs.com/assertions/184403861
[enhancing-assertions]: http://www.drdobbs.com/cpp/enhancing-assertions/184403745
[@incomputable]: https://twitter.com/incomputable

I learnt the `PPK_UNUSED` trick from [Branimir Karadžić][@bkaradzic].

Finally, [`__VA_NARG__` has been invented by Laurent Deniau][__VA_NARG__].

[@bkaradzic]: https://twitter.com/bkaradzic
[__VA_NARG__]: https://groups.google.com/d/msg/comp.std.c/d-6Mj5Lko_s/5R6bMWTEbzQJ


--------------------------------------------------------------------------------

If you find this library useful and decide to use it in your own projects please
drop me a line [@gpakosz].

If you use it in a commercial project, consider using [Gittip].

[@gpakosz]: https://twitter.com/gpakosz
[Gittip]: https://www.gittip.com/gpakosz/
