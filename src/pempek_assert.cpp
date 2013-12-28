// see README.md for usage instructions.
// (‑●‑●)> released under the WTFPL v2 license, by Gregory Pakosz (@gpakosz)

#include <pempek_assert.h>

#include <cstdio>  // fprintf() and vsnprintf()
#include <cstring>
#include <cstdarg> // va_start() and va_end()
#include <cstdlib> // abort()

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#if !defined(PEMPEK_ASSERT_MESSAGE_BUFFER_SIZE)
#  define PEMPEK_ASSERT_MESSAGE_BUFFER_SIZE 1024
#endif

namespace {

  namespace AssertLevel = pempek::assert::implementation::AssertLevel;
  namespace AssertAction = pempek::assert::implementation::AssertAction;

  typedef int (*printHandler)(void*, const char* format, ...);

  int formatLevel(int level, const char* expression, void* out, printHandler print)
  {
    int count = 0;

    count += print(out, "Assertion '%s' failed", expression);

    switch (level)
    {
      case AssertLevel::PEMPEK_ASSERT_LEVEL_WARNING:
        count += print(out, " (WARNING)");
        break;

      case AssertLevel::PEMPEK_ASSERT_LEVEL_DEBUG:
        count += print(out, " (DEBUG)");
        break;

      case AssertLevel::PEMPEK_ASSERT_LEVEL_ERROR:
        count += print(out, " (ERROR)");
        break;


      case AssertLevel::PEMPEK_ASSERT_LEVEL_FATAL:
        count += print(out, " (FATAL)");
        break;

      default:
        count += print(out, " (level = %d)", level);
    }

    return count;
  }

  AssertAction::AssertAction _defaultHandler( const char* file,
                                              int line,
                                              const char* function,
                                              const char* expression,
                                              int level,
                                              const char* message)
  {
    formatLevel(level, expression, stderr, reinterpret_cast<printHandler>(::fprintf));
    fprintf(stderr, "\n  in file %s, line %d\n  function: %s\n", file, line, function);
    fflush(stderr);

    if (message)
    {
      fprintf(stderr, "  with message: %s\n\n", message);
      fflush(stderr);
    }

    if (level < AssertLevel::PEMPEK_ASSERT_LEVEL_DEBUG)
    {
      return AssertAction::PEMPEK_ASSERT_ACTION_NONE;
    }
    else if (AssertLevel::PEMPEK_ASSERT_LEVEL_DEBUG <= level && level < AssertLevel::PEMPEK_ASSERT_LEVEL_ERROR)
    {
#if (!TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR) && (!defined(__ANDROID__) && !defined(ANDROID)) || defined(PEMPEK_ASSERT_DEFAULT_HANDLER_STDIN)
      for (;;)
      {
        fprintf(stderr, "Press (I)gnore / Ignore (F)orever / Ignore (A)ll / (D)ebug / A(b)ort: ");
        fflush(stderr);

        char line[256];
        fgets(line, sizeof(line), stdin);

        // we eventually skip the leading spaces but that's it
        char input[2] = {'b', 0};
        if (sscanf(line, " %1[a-zA-Z] ", input) != 1)
          continue;

        switch (*input)
        {
          case 'b':
          case 'B':
            return AssertAction::PEMPEK_ASSERT_ACTION_ABORT;

          case 'd':
          case 'D':
            return AssertAction::PEMPEK_ASSERT_ACTION_BREAK;

          case 'i':
          case 'I':
            return AssertAction::PEMPEK_ASSERT_ACTION_IGNORE;

          case 'f':
          case 'F':
            return AssertAction::PEMPEK_ASSERT_ACTION_IGNORE_LINE;

          case 'a':
          case 'A':
            return AssertAction::PEMPEK_ASSERT_ACTION_IGNORE_ALL;

          default:
            break;
        }
      }
#else
      return AssertAction::PEMPEK_ASSERT_ACTION_BREAK;
#endif
    }
    else if (AssertLevel::PEMPEK_ASSERT_LEVEL_ERROR <= level && level < AssertLevel::PEMPEK_ASSERT_LEVEL_FATAL)
    {
      return AssertAction::PEMPEK_ASSERT_ACTION_THROW;
    }

    return AssertAction::PEMPEK_ASSERT_ACTION_ABORT;
  }

  void _throw(const char* file,
              int line,
              const char* function,
              const char* expression,
              const char* message)
  {
    using pempek::assert::implementation::throwException;
    throwException(pempek::assert::AssertionException(file, line, function, expression, message));
  }
}

namespace pempek {
namespace assert {

  AssertionException::AssertionException(const char* file,
                                         int line,
                                         const char* function,
                                         const char* expression,
                                         const char* message)
  : _file(file), _line(line), _function(function), _expression(expression), _heap(PEMPEK_ASSERT_NULLPTR)
  {
    if (!message)
    {
      strncpy(_stack, "", size);
      return;
    }

    size_t length = strlen(message);

    if (length < size) // message is short enough for the stack buffer
    {
      strncpy(_stack, message, length);
      strncpy(_stack + length, "", size - length); // pad with 0
    }
    else // allocate storage on the heap
    {
      _heap = static_cast<char*>(malloc(sizeof(char) * (length + 1)));

      if (!_heap) // allocation failed
      {
        strncpy(_stack, message, size - 1); // stack fallback, truncate :/
        _stack[size - 1] = 0;
      }
      else
      {
        strncpy(_heap, message, length); // copy the message
        _heap[length] = 0;
        _stack[size - 1] = 1; // mark the stack
      }
    }
  }

  AssertionException::AssertionException(const AssertionException& rhs)
  : _file(rhs._file), _line(rhs._line), _function(rhs._function), _expression(rhs._expression)
  {
    const char* message = rhs.what();
    size_t length = strlen(message);

    if (length < size) // message is short enough for the stack buffer
    {
      strncpy(_stack, message, size); // pad with 0
    }
    else // allocate storage on the heap
    {
      _heap = static_cast<char*>(malloc(sizeof(char) * (length + 1)));

      if (!_heap) // allocation failed
      {
        strncpy(_stack, message, size - 1); // stack fallback, truncate :/
        _stack[size - 1] = 0;
      }
      else
      {
        strncpy(_heap, message, length); // copy the message
        _heap[length] = 0;
        _stack[size - 1] = 1; // mark the stack
      }
    }
  }

  AssertionException::~AssertionException() PEMPEK_ASSERT_EXCEPTION_NO_THROW
  {
    if (_stack[size - 1])
      free(_heap);

    _heap = PEMPEK_ASSERT_NULLPTR; // in case the exception object is destroyed twice
    _stack[size - 1] = 0;
  }

  AssertionException& AssertionException::operator = (const AssertionException& rhs)
  {
    if (&rhs == this)
      return *this;

    const char* message = rhs.what();
    size_t length = strlen(message);

    if (length < size) // message is short enough for the stack buffer
    {
      if (_stack[size - 1])
        free(_heap);

      strncpy(_stack, message, size);
    }
    else // allocate storage on the heap
    {
      if (_stack[size - 1])
      {
        size_t _length = strlen(_heap);

        if (length <= _length)
        {
          strncpy(_heap, message, _length); // copy the message, pad with 0
          return *this;
        }
        else
        {
          free(_heap);
        }
      }

      _heap = static_cast<char*>(malloc(sizeof(char) * (length + 1)));

      if (!_heap) // allocation failed
      {
        strncpy(_stack, message, size - 1); // stack fallback, truncate :/
        _stack[size - 1] = 0;
      }
      else
      {
        strncpy(_heap, message, length); // copy the message
        _heap[length] = 0;
        _stack[size - 1] = 1; // mark the stack
      }
    }

    _file = rhs._file;
    _line = rhs._line;
    _function = rhs._function;
    _expression = rhs._expression;

    return *this;
  }

  const char* AssertionException::what() const PEMPEK_ASSERT_EXCEPTION_NO_THROW
  {
    return _stack[size - 1] ? _heap : _stack;
  }

namespace implementation {

  namespace {
    bool _ignoreAll = false;
  }

  void ignoreAllAsserts(bool value)
  {
    _ignoreAll = value;
  }

  bool ignoreAllAsserts()
  {
    return _ignoreAll;
  }

  namespace {
    AssertHandler _handler = _defaultHandler;
  }

  AssertHandler setAssertHandler(AssertHandler handler)
  {
    AssertHandler previous = _handler;

    _handler = handler ? handler : _defaultHandler;

    return previous;
  }

  AssertAction::AssertAction handleAssert(const char* file,
                                          int line,
                                          const char* function,
                                          const char* expression,
                                          int level,
                                          bool& ignoreLine,
                                          const char* message, ...)
  {
    char message_[PEMPEK_ASSERT_MESSAGE_BUFFER_SIZE] = {0};
    const char* file_;

    if (message)
    {
      va_list args;
      va_start(args, message);
      vsnprintf(message_, PEMPEK_ASSERT_MESSAGE_BUFFER_SIZE, message, args);
      va_end(args);

      message = message_;
    }

#if defined(_WIN32)
    file_ = strrchr(file, '\\');
#else
    file_ = strrchr(file, '/');
#endif // #if defined(_WIN32)

    file = file_ ? file_ + 1 : file;
    AssertAction::AssertAction action = _handler(file, line, function, expression, level, message);

    switch (action)
    {
      case AssertAction::PEMPEK_ASSERT_ACTION_ABORT:
        abort();

      case AssertAction::PEMPEK_ASSERT_ACTION_IGNORE_LINE:
        ignoreLine = true;
        break;

      case AssertAction::PEMPEK_ASSERT_ACTION_IGNORE_ALL:
        ignoreAllAsserts(true);
        break;

      case AssertAction::PEMPEK_ASSERT_ACTION_THROW:
        _throw(file, line, function, expression, message);
        break;

      case AssertAction::PEMPEK_ASSERT_ACTION_IGNORE:
      case AssertAction::PEMPEK_ASSERT_ACTION_BREAK:
      case AssertAction::PEMPEK_ASSERT_ACTION_NONE:
      default:
        return action;
    }

    return AssertAction::PEMPEK_ASSERT_ACTION_NONE;
  }

} // namespace implementation
} // namespace assert
} // namespace pempek
