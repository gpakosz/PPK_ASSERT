// see README.md for usage instructions.
// (‑●‑●)> released under the WTFPL v2 license, by Gregory Pakosz (@gpakosz)

// -- usage --------------------------------------------------------------------
/*

  PEMPEK_ASSERT(expression);
  PEMPEK_ASSERT(expression, message, ...);

  PEMPEK_ASSERT_WARNING(expression);
  PEMPEK_ASSERT_WARNING(expression, message, ...);

  PEMPEK_ASSERT_DEBUG(expression);
  PEMPEK_ASSERT_DEBUG(expression, message, ...);

  PEMPEK_ASSERT_ERROR(expression);
  PEMPEK_ASSERT_ERROR(expression, message);

  PEMPEK_ASSERT_FATAL(expression);
  PEMPEK_ASSERT_FATAL(expression, message, ...);

  PEMPEK_ASSERT_CUSTOM(level, expression);
  PEMPEK_ASSERT_CUSTOM(level, expression, message, ...);

  PEMPEK_ASSERT_USED(type)
  PEMPEK_ASSERT_USED_WARNING(type)
  PEMPEK_ASSERT_USED_DEBUG(type)
  PEMPEK_ASSERT_USED_ERROR(type)
  PEMPEK_ASSERT_USED_FATAL(type)
  PEMPEK_ASSERT_USED_CUSTOM(level, type)

  PEMPEK_ASSERT_USED(bool) foo()
  {
    return true;
  }

*/

#if !defined(PEMPEK_ASSERT_ENABLED)
#  if !defined(NDEBUG) // if we are in debug mode
#    define PEMPEK_ASSERT_ENABLED 1 // enable them
#  endif
#endif

#if !defined(PEMPEK_ASSERT_DEFAULT_LEVEL)
#  define PEMPEK_ASSERT_DEFAULT_LEVEL PEMPEK_ASSERT_LEVEL_DEBUG
#endif

// -- implementation -----------------------------------------------------------

#if !defined(PEMPEK_ASSERT_H)
#define PEMPEK_ASSERT_H

#define PEMPEK_ASSERT(...)                    PEMPEK_ASSERT_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_DEFAULT_LEVEL, __VA_ARGS__)
#define PEMPEK_ASSERT_WARNING(...)            PEMPEK_ASSERT_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_LEVEL_WARNING, __VA_ARGS__)
#define PEMPEK_ASSERT_DEBUG(...)              PEMPEK_ASSERT_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_LEVEL_DEBUG, __VA_ARGS__)
#define PEMPEK_ASSERT_ERROR(...)              PEMPEK_ASSERT_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_LEVEL_ERROR, __VA_ARGS__)
#define PEMPEK_ASSERT_FATAL(...)              PEMPEK_ASSERT_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_LEVEL_FATAL, __VA_ARGS__)
#define PEMPEK_ASSERT_CUSTOM(level, ...)      PEMPEK_ASSERT_(level, __VA_ARGS__)

#define PEMPEK_ASSERT_USED(...)               PEMPEK_ASSERT_USED_(__VA_ARGS__)
#define PEMPEK_ASSERT_USED_WARNING(...)       PEMPEK_ASSERT_USED_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_LEVEL_WARNING, __VA_ARGS__)
#define PEMPEK_ASSERT_USED_DEBUG(...)         PEMPEK_ASSERT_USED_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_LEVEL_DEBUG, __VA_ARGS__)
#define PEMPEK_ASSERT_USED_ERROR(...)         PEMPEK_ASSERT_USED_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_LEVEL_ERROR, __VA_ARGS__)
#define PEMPEK_ASSERT_USED_FATAL(...)         PEMPEK_ASSERT_USED_(pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_LEVEL_FATAL, __VA_ARGS__)
#define PEMPEK_ASSERT_USED_CUSTOM(level, ...) PEMPEK_ASSERT_USED_(level, __VA_ARGS__)


#define PEMPEK_ASSERT_CONCATENATE(lhs, rhs)   PEMPEK_ASSERT_CONCATENATE_(lhs, rhs)
#define PEMPEK_ASSERT_CONCATENATE_(lhs, rhs)  PEMPEK_ASSERT_CONCATENATE__(lhs, rhs)
#define PEMPEK_ASSERT_CONCATENATE__(lhs, rhs) lhs##rhs

#define PEMPEK_ASSERT_FILE __FILE__
#define PEMPEK_ASSERT_LINE __LINE__
#if defined(__GNUC__)
#  define PEMPEK_ASSERT_FUNCTION __PRETTY_FUNCTION__
#else
#  define PEMPEK_ASSERT_FUNCTION __FUNCTION__
#endif

#if defined(_MSC_VER)
  #define PEMPEK_ASSERT_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
  #define PEMPEK_ASSERT_ALWAYS_INLINE inline __attribute__((always_inline))
#else
  #define PEMPEK_ASSERT_ALWAYS_INLINE inline
#endif

#define PEMPEK_ASSERT_NO_MACRO

#define PEMPEK_ASSERT_APPLY_VA_ARGS(M, ...) PEMPEK_ASSERT_APPLY_VA_ARGS_(M, (__VA_ARGS__))
#define PEMPEK_ASSERT_APPLY_VA_ARGS_(M, args) M args

#define PEMPEK_ASSERT_NARG(...) PEMPEK_ASSERT_APPLY_VA_ARGS(PEMPEK_ASSERT_NARG_, PEMPEK_ASSERT_NO_MACRO,##__VA_ARGS__,\
  32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,\
  15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, PEMPEK_ASSERT_NO_MACRO)
#define PEMPEK_ASSERT_NARG_(_0, _1, _2, _3, _4, _5, _6, _7, _8,\
                            _9, _10, _11, _12, _13, _14, _15, _16,\
                            _17, _18, _19, _20, _21, _22, _23, _24,\
                            _25, _26, _27, _28, _29, _30, _31, _32, _33, ...) _33

#define PEMPEK_ASSERT_HAS_ONE_ARG(...) PEMPEK_ASSERT_APPLY_VA_ARGS(PEMPEK_ASSERT_NARG_, PEMPEK_ASSERT_NO_MACRO,##__VA_ARGS__,\
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, PEMPEK_ASSERT_NO_MACRO)

#if defined(__GNUC__)
#  define PEMPEK_ASSERT_LIKELY(arg) __builtin_expect(!!(arg), !0)
#  define PEMPEK_ASSERT_UNLIKELY(arg) __builtin_expect(!!(arg), 0)
#else
#  define PEMPEK_ASSERT_LIKELY(arg) arg
#  define PEMPEK_ASSERT_UNLIKELY(arg) !arg
#endif

#define PEMPEK_ASSERT_UNUSED(expression) (void)(true ? (void)0 : ((void)(expression)))

#if defined(_WIN32)
#  define PEMPEK_ASSERT_DEBUG_BREAK() __debugbreak()
#else
#  if defined(__APPLE__)
#  include <TargetConditionals.h>
#  endif
#  if defined(__clang__) && !TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#    define PEMPEK_ASSERT_DEBUG_BREAK() __builtin_debugtrap()
#  elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#    include <signal.h>
#    define PEMPEK_ASSERT_DEBUG_BREAK() raise(SIGTRAP)
#  elif defined(__GNUC__)
#    define PEMPEK_ASSERT_DEBUG_BREAK() __builtin_trap()
#  else
#    define PEMPEK_ASSERT_DEBUG_BREAK() ((void)0)
#  endif
#endif

#if (defined (__cplusplus) && (__cplusplus > 199711L)) || (defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 150020706))
#  define PEMPEK_ASSERT_CXX11
#endif

#if defined(PEMPEK_ASSERT_CXX11)
#  define PEMPEK_ASSERT_NULLPTR nullptr
#else
#  define PEMPEK_ASSERT_NULLPTR 0
#endif

#define PEMPEK_ASSERT_(level, ...)          PEMPEK_ASSERT_CONCATENATE(PEMPEK_ASSERT_, PEMPEK_ASSERT_HAS_ONE_ARG(__VA_ARGS__))(level, __VA_ARGS__)
#define PEMPEK_ASSERT_0(level, ...)         PEMPEK_ASSERT_APPLY_VA_ARGS(PEMPEK_ASSERT_2, level, __VA_ARGS__)
#define PEMPEK_ASSERT_1(level, expression)  PEMPEK_ASSERT_2(level, expression, PEMPEK_ASSERT_NULLPTR)

#  if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140050215)

    #define PEMPEK_ASSERT_3(level, expression, ...)\
      __pragma(warning(push))\
      __pragma(warning(disable: 4127))\
      do\
      {\
        static bool _ignore = false;\
        if (PEMPEK_ASSERT_LIKELY(expression) || _ignore || pempek::assert::implementation::ignoreAllAsserts());\
        else\
        {\
          if (pempek::assert::implementation::handleAssert(PEMPEK_ASSERT_FILE, PEMPEK_ASSERT_LINE, PEMPEK_ASSERT_FUNCTION, #expression, level, _ignore, __VA_ARGS__) == pempek::assert::implementation::AssertAction::PEMPEK_ASSERT_ACTION_BREAK)\
            PEMPEK_ASSERT_DEBUG_BREAK();\
        }\
      }\
      while (false)\
      __pragma(warning(pop))

#  else

    #define PEMPEK_ASSERT_3(level, expression, ...)\
      do\
      {\
        static bool _ignore = false;\
        if (PEMPEK_ASSERT_LIKELY(expression) || _ignore || pempek::assert::implementation::ignoreAllAsserts());\
        else\
        {\
          if (pempek::assert::implementation::handleAssert(PEMPEK_ASSERT_FILE, PEMPEK_ASSERT_LINE, PEMPEK_ASSERT_FUNCTION, #expression, level, _ignore, __VA_ARGS__) == pempek::assert::implementation::AssertAction::PEMPEK_ASSERT_ACTION_BREAK)\
            PEMPEK_ASSERT_DEBUG_BREAK();\
        }\
      }\
      while (false)

#  endif

#define PEMPEK_ASSERT_USED_(...)            PEMPEK_ASSERT_USED_0(PEMPEK_ASSERT_NARG(__VA_ARGS__), __VA_ARGS__)
#define PEMPEK_ASSERT_USED_0(N, ...)        PEMPEK_ASSERT_CONCATENATE(PEMPEK_ASSERT_USED_, N)(__VA_ARGS__)

#define PEMPEK_ASSERT_STATIC_ASSERT(expression, message)\
  struct PEMPEK_ASSERT_CONCATENATE(__pempek_assert_static_assertion_at_line_, PEMPEK_ASSERT_LINE)\
  {\
    pempek::assert::implementation::StaticAssertion<static_cast<bool>((expression))> PEMPEK_ASSERT_CONCATENATE(PEMPEK_ASSERT_CONCATENATE(PEMPEK_ASSERT_CONCATENATE(STATIC_ASSERTION_FAILED_AT_LINE_, PEMPEK_ASSERT_LINE), _), message);\
  };\
  typedef pempek::assert::implementation::StaticAssertionTest<sizeof(PEMPEK_ASSERT_CONCATENATE(__pempek_assert_static_assertion_at_line_, PEMPEK_ASSERT_LINE))> PEMPEK_ASSERT_CONCATENATE(__pempek_assert_static_assertion_test_at_line_, PEMPEK_ASSERT_LINE)

// note that we wrap the non existing type inside a struct to avoid warning
// messages about unused variables when static assertions are used at function
// scope
// the use of sizeof makes sure the assertion error is not ignored by SFINAE

namespace pempek {
namespace assert {
namespace implementation {

  template <bool>
  struct StaticAssertion;

  template <>
  struct StaticAssertion<true>
  {
  }; // StaticAssertion<true>

  template<int i>
  struct StaticAssertionTest
  {
  }; // StaticAssertionTest<int>

} // namespace implementation
} // namespace assert
} // namespace pempek

#if !defined(PEMPEK_ASSERT_DISABLE_STL)
#  if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable: 4548)
#    pragma warning(disable: 4710)
#  endif
#  include <stdexcept>
#  if defined(_MSC_VER)
#    pragma warning(pop)
#  endif
#endif

#if !defined(PEMPEK_ASSERT_EXCEPTION_MESSAGE_BUFFER_SIZE)
#  define PEMPEK_ASSERT_EXCEPTION_MESSAGE_BUFFER_SIZE 1024
#endif

#if defined(PEMPEK_ASSERT_CXX11) && !defined(_MSC_VER)
#  define PEMPEK_ASSERT_EXCEPTION_NO_THROW noexcept(true)
#else
#  define PEMPEK_ASSERT_EXCEPTION_NO_THROW throw()
#endif

#if defined(PEMPEK_ASSERT_CXX11)
#  include <utility>
#endif

namespace pempek {
namespace assert {

#if !defined(PEMPEK_ASSERT_DISABLE_STL)
  class AssertionException: public std::exception
#else
  class AssertionException
#endif
  {
    public:
    explicit AssertionException(const char* file,
                                int line,
                                const char* function,
                                const char* expression,
                                const char* message);

    AssertionException(const AssertionException& rhs);

    virtual ~AssertionException() PEMPEK_ASSERT_EXCEPTION_NO_THROW;

    AssertionException& operator = (const AssertionException& rhs);

    virtual const char* what() const PEMPEK_ASSERT_EXCEPTION_NO_THROW;

    const char* file() const;
    const int line() const;
    const char* function() const;
    const char* expression() const;

    private:
    const char* _file;
    int _line;
    const char* _function;
    const char* _expression;

    enum
    {
      request = PEMPEK_ASSERT_EXCEPTION_MESSAGE_BUFFER_SIZE,
      size = request > sizeof(char*) ? request : sizeof(char*) + 1
    };

    union
    {
      char  _stack[size];
      char* _heap;
    };

    PEMPEK_ASSERT_STATIC_ASSERT(size > sizeof(char*), invalid_size);
  }; // AssertionException

  PEMPEK_ASSERT_ALWAYS_INLINE const char* AssertionException::file() const
  {
    return _file;
  }

  PEMPEK_ASSERT_ALWAYS_INLINE const int AssertionException::line() const
  {
    return _line;
  }

  PEMPEK_ASSERT_ALWAYS_INLINE const char* AssertionException::function() const
  {
    return _function;
  }

  PEMPEK_ASSERT_ALWAYS_INLINE const char* AssertionException::expression() const
  {
    return _expression;
  }

namespace implementation {

#if defined(_MSC_VER) && !defined(_CPPUNWIND)
#  if !defined(PEMPEK_ASSERT_DISABLE_EXCEPTIONS)
#    define PEMPEK_ASSERT_DISABLE_EXCEPTIONS
#  endif
#endif

#if !defined(PEMPEK_ASSERT_DISABLE_EXCEPTIONS)

  template<typename E>
  inline void throwException(const E& e)
  {
    throw e;
  }

#else

  // user defined, the behavior is undefined if the function returns
  void throwException(const pempek::assert::AssertionException& e);

#endif

  namespace AssertLevel {

    enum AssertLevel
    {
      PEMPEK_ASSERT_LEVEL_WARNING = 32,
      PEMPEK_ASSERT_LEVEL_DEBUG = 64,
      PEMPEK_ASSERT_LEVEL_ERROR = 128,
      PEMPEK_ASSERT_LEVEL_FATAL = 256

    }; // AssertLevel

  } // AssertLevel

  namespace AssertAction {

    enum AssertAction
    {
      PEMPEK_ASSERT_ACTION_NONE,
      PEMPEK_ASSERT_ACTION_ABORT,
      PEMPEK_ASSERT_ACTION_BREAK,
      PEMPEK_ASSERT_ACTION_IGNORE,
      PEMPEK_ASSERT_ACTION_IGNORE_LINE,
      PEMPEK_ASSERT_ACTION_IGNORE_ALL,
      PEMPEK_ASSERT_ACTION_THROW

    }; // AssertAction

  } // AssertAction

  typedef AssertAction::AssertAction (*AssertHandler)(const char* file,
                                                      int line,
                                                      const char* function,
                                                      const char* expression,
                                                      int level,
                                                      const char* message);


#if defined(__GNUC__)
#  define PEMPEK_ASSERT_HANDLE_ASSERT_FORMAT __attribute__((format (printf, 7, 8)))
#else
#  define PEMPEK_ASSERT_HANDLE_ASSERT_FORMAT
#endif

  AssertAction::AssertAction handleAssert(const char* file,
                                          int line,
                                          const char* function,
                                          const char* expression,
                                          int level,
                                          bool& ignoreLine,
                                          const char* message, ...) PEMPEK_ASSERT_HANDLE_ASSERT_FORMAT;

  AssertHandler setAssertHandler(AssertHandler handler);

  void ignoreAllAsserts(bool value);
  bool ignoreAllAsserts();

#if defined(PEMPEK_ASSERT_CXX11)

  template<int level, typename T>
  class AssertUsedWrapper
  {
    public:
    AssertUsedWrapper(T&& t);
    ~AssertUsedWrapper() PEMPEK_ASSERT_EXCEPTION_NO_THROW;

    operator T();

    private:
    const AssertUsedWrapper& operator = (const AssertUsedWrapper&); // not implemented on purpose (and only VS2013 supports deleted functions)

    T t;
    mutable bool used;

  }; // AssertUsedWrapper<int, T>

  template<int level, typename T>
  inline AssertUsedWrapper<level, T>::AssertUsedWrapper(T&& t)
    : t(std::forward<T>(t)), used(false)
  {}

  template<int level, typename T>
  inline AssertUsedWrapper<level, T>::operator T()
  {
    used = true;
    return std::move(t);
  }

  template<int level, typename T>
  inline AssertUsedWrapper<level, T>::~AssertUsedWrapper() PEMPEK_ASSERT_EXCEPTION_NO_THROW
  {
    PEMPEK_ASSERT_3(level, used, "unused value");
  }

#else

  template<int level, typename T>
  class AssertUsedWrapper
  {
    public:
    AssertUsedWrapper(const T& t);
    AssertUsedWrapper(const AssertUsedWrapper& rhs);
    ~AssertUsedWrapper() PEMPEK_ASSERT_EXCEPTION_NO_THROW;

    operator T() const;

    private:
    const AssertUsedWrapper& operator = (const AssertUsedWrapper&); // not implemented on purpose

    T t;
    mutable bool used;

  }; // AssertUsedWrapper<int, T>

  template<int level, typename T>
  PEMPEK_ASSERT_ALWAYS_INLINE AssertUsedWrapper<level, T>::AssertUsedWrapper(const T& t)
    : t(t), used(false)
  {}

  template<int level, typename T>
  PEMPEK_ASSERT_ALWAYS_INLINE AssertUsedWrapper<level, T>::AssertUsedWrapper(const AssertUsedWrapper& rhs)
    : t(rhs.t), used(rhs.used)
  {}

  // /!\ GCC is not so happy if we inline that destructor
  template<int level, typename T>
  AssertUsedWrapper<level, T>::~AssertUsedWrapper() PEMPEK_ASSERT_EXCEPTION_NO_THROW
  {
    PEMPEK_ASSERT_3(level, used, "unused value");
  }

  template<int level, typename T>
  PEMPEK_ASSERT_ALWAYS_INLINE AssertUsedWrapper<level, T>::operator T() const
  {
    used = true;
    return t;
  }

#endif

} // namespace implementation

} // namespace assert
} // namespace pempek

#endif

#undef PEMPEK_ASSERT_2
#undef PEMPEK_ASSERT_USED_1
#undef PEMPEK_ASSERT_USED_2

#if PEMPEK_ASSERT_ENABLED

  #define PEMPEK_ASSERT_2(level, expression, ...) PEMPEK_ASSERT_3(level, expression, __VA_ARGS__)
  #define PEMPEK_ASSERT_USED_1(type)              pempek::assert::implementation::AssertUsedWrapper<pempek::assert::implementation::AssertLevel::PEMPEK_ASSERT_DEFAULT_LEVEL, type>
  #define PEMPEK_ASSERT_USED_2(level, type)       pempek::assert::implementation::AssertUsedWrapper<level, type>

#else

  #define PEMPEK_ASSERT_2(level, expression, ...) PEMPEK_ASSERT_UNUSED(expression)
  #define PEMPEK_ASSERT_USED_1(type)              type
  #define PEMPEK_ASSERT_USED_2(level, type)       type

#endif
