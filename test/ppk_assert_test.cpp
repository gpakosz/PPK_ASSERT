#include <gtest/gtest.h>

#define PPK_ASSERT_ENABLED 1
#include <ppk_assert.h>

#include <cstdlib>
#include <cstring>


using namespace ppk::assert;

namespace {

  namespace implementation = ppk::assert::implementation;
  namespace AssertLevel = implementation::AssertLevel;
  namespace AssertAction = implementation::AssertAction;

  const char* _file;
  int _line;
  const char* _function;
  const char* _expression;
  int _level;
  char* _message;

  AssertAction::AssertAction _action = AssertAction::None;

  AssertAction::AssertAction _testHandler(const char* file,
                                          int line,
                                          const char* function,
                                          const char* expression,
                                          int level,
                                          const char* message)
  {
    _file = file;
    _line = line;
    _function = function;
    _expression = expression;
    _level = level;

    if (_message)
      free(_message);

    if (message)
      _message = strdup(message);

    if (level == AssertLevel::Error)
      return AssertAction::Throw;

    return _action;
  }
}

#if defined(PPK_ASSERT_DISABLE_EXCEPTIONS)
namespace ppk {
namespace assert {
namespace implementation {

  void throwException(const ppk::assert::AssertionException& e)
  {
    _file = e.file();
    _line = e.line();
    _function = e.function();
    _expression = e.expression();
    _level = AssertLevel::Error;
  }

} // namespace implementation
} // namespace assert
} // namespace ppk
#endif

namespace {

  class AssertTest : public ::testing::Test
  {
    protected:
    explicit AssertTest()
    {
      implementation::setAssertHandler(_testHandler);
      _action = AssertAction::None;

      _message = 0;
    }

    ~AssertTest()
    {
      implementation::setAssertHandler(PPK_ASSERT_NULLPTR);

      if (_message)
        free(_message);

      _message = 0;
    }

  };

  TEST_F(AssertTest, ASSERT_WARNING)
  {
    PPK_ASSERT_WARNING(true);
    PPK_ASSERT_WARNING(true, "always true, never fails");

    PPK_ASSERT_WARNING(false);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Warning, _level);
    EXPECT_STREQ(static_cast<const char*>(PPK_ASSERT_NULLPTR), _message);

    PPK_ASSERT_WARNING(false, "always false, always fails");
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Warning, _level);
    EXPECT_STREQ("always false, always fails", _message);

    const char* s = "foo";
    int i = 123;
    float f = 123.456f;
    PPK_ASSERT_WARNING(false, "always false, always fails -- s: %s, i: %d, f: %3.3f", s, i, f);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Warning, _level);
    EXPECT_STREQ("always false, always fails -- s: foo, i: 123, f: 123.456", _message);
  }

  TEST_F(AssertTest, ASSERT)
  {
    PPK_ASSERT(true);
    PPK_ASSERT(true, "always true, never fails");

    PPK_ASSERT(false);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Debug, _level);
    EXPECT_STREQ(static_cast<const char*>(PPK_ASSERT_NULLPTR), _message);

    PPK_ASSERT(false, "always false, always fails");
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Debug, _level);
    EXPECT_STREQ("always false, always fails", _message);

    const char* s = "foo";
    int i = 123;
    float f = 123.456f;
    PPK_ASSERT_DEBUG(false, "always false, always fails -- s: %s, i: %d, f: %3.3f", s, i, f);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Debug, _level);
    EXPECT_STREQ("always false, always fails -- s: foo, i: 123, f: 123.456", _message);
  }

  TEST_F(AssertTest, ASSERT_DEBUG)
  {
    PPK_ASSERT_DEBUG(true);
    PPK_ASSERT_DEBUG(true, "always true, never fails");

    PPK_ASSERT_DEBUG(false);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Debug, _level);
    EXPECT_STREQ(static_cast<const char*>(PPK_ASSERT_NULLPTR), _message);

    PPK_ASSERT_DEBUG(false, "always false, always fails");
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Debug, _level);
    EXPECT_STREQ("always false, always fails", _message);

    const char* s = "foo";
    int i = 123;
    float f = 123.456f;
    PPK_ASSERT_DEBUG(false, "always false, always fails -- s: %s, i: %d, f: %3.3f", s, i, f);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Debug, _level);
    EXPECT_STREQ("always false, always fails -- s: foo, i: 123, f: 123.456", _message);
  }

  TEST_F(AssertTest, ASSERT_ERROR)
  {
    PPK_ASSERT_ERROR(true);
    PPK_ASSERT_ERROR(true, "always true, never fails");

#if defined(PPK_ASSERT_DISABLE_EXCEPTIONS)
    PPK_ASSERT_ERROR(false);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Error, _level);
    EXPECT_STREQ(static_cast<const char*>(PPK_ASSERT_NULLPTR), _message);
#else
    EXPECT_THROW(PPK_ASSERT_ERROR(false), AssertionException);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Error, _level);
    EXPECT_STREQ(static_cast<const char*>(PPK_ASSERT_NULLPTR), _message);
#endif

#if defined(PPK_ASSERT_DISABLE_EXCEPTIONS)
    PPK_ASSERT_ERROR(false, "always false, always fails");
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Error, _level);
    EXPECT_STREQ("always false, always fails", _message);
#else
    EXPECT_THROW(PPK_ASSERT_ERROR(false, "always false, always fails"), AssertionException);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Error, _level);
    EXPECT_STREQ("always false, always fails", _message);
#endif

    const char* s = "foo";
    int i = 123;
    float f = 123.456f;
#if defined(PPK_ASSERT_DISABLE_EXCEPTIONS)
    PPK_ASSERT_ERROR(false, "always false, always fails -- s: %s, i: %d, f: %3.3f", s, i, f);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Error, _level);
    EXPECT_STREQ("always false, always fails -- s: foo, i: 123, f: 123.456", _message);
#else
    EXPECT_THROW(PPK_ASSERT_ERROR(false, "always false, always fails -- s: %s, i: %d, f: %3.3f", s, i, f), AssertionException);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Error, _level);
    EXPECT_STREQ("always false, always fails -- s: foo, i: 123, f: 123.456", _message);
#endif
  }

  TEST_F(AssertTest, ASSERT_FATAL)
  {
    PPK_ASSERT_FATAL(true);
    PPK_ASSERT_FATAL(true, "always true, never fails");

    PPK_ASSERT_FATAL(false);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Fatal, _level);
    EXPECT_STREQ(static_cast<const char*>(PPK_ASSERT_NULLPTR), _message);

    PPK_ASSERT_FATAL(false, "always false, always fails");
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Fatal, _level);
    EXPECT_STREQ("always false, always fails", _message);

    const char* s = "foo";
    int i = 123;
    float f = 123.456f;
    PPK_ASSERT_FATAL(false, "always false, always fails -- s: %s, i: %d, f: %3.3f", s, i, f);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(AssertLevel::Fatal, _level);
    EXPECT_STREQ("always false, always fails -- s: foo, i: 123, f: 123.456", _message);

    implementation::setAssertHandler(PPK_ASSERT_NULLPTR);

    // 3rd parameter of EXPECT_DEATH is a regex
    EXPECT_DEATH_IF_SUPPORTED(PPK_ASSERT_FATAL(false), "");
    EXPECT_DEATH_IF_SUPPORTED(PPK_ASSERT_FATAL(false, "always false, always fails"), "always false, always fails");
  }

  TEST_F(AssertTest, ASSERT_CustomLevel)
  {
    PPK_ASSERT_CUSTOM(1337, true);
    PPK_ASSERT_CUSTOM(1337, true, "always true, never fails");

    PPK_ASSERT_CUSTOM(1337, false);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(1337, _level);
    EXPECT_STREQ(static_cast<const char*>(PPK_ASSERT_NULLPTR), _message);

    PPK_ASSERT_CUSTOM(1337, false, "always false, always fails");
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(1337, _level);
    EXPECT_STREQ("always false, always fails", _message);

    const char* s = "foo";
    int i = 123;
    float f = 123.456f;
    PPK_ASSERT_CUSTOM(1337, false, "always false, always fails -- s: %s, i: %d, f: %3.3f", s, i, f);
    EXPECT_STREQ("ppk_assert_test.cpp", _file);
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
    EXPECT_EQ(1337, _level);
    EXPECT_STREQ("always false, always fails -- s: foo, i: 123, f: 123.456", _message);
  }

  TEST_F(AssertTest, ignoreLine)
  {
    struct Local
    {
      static void f()
      {
        PPK_ASSERT(false);
      }
    };

    _action = AssertAction::IgnoreLine;

    Local::f();
    EXPECT_EQ(PPK_ASSERT_LINE - 7, _line);

    PPK_ASSERT(false);
    Local::f(); // should be ignored the second time
    EXPECT_EQ(PPK_ASSERT_LINE - 2, _line);
  }

  TEST_F(AssertTest, ignoreAll)
  {
    struct Local
    {
      static void f()
      {
        PPK_ASSERT(false);
      }
    };

    _action = AssertAction::IgnoreAll;

    PPK_ASSERT(false);
    EXPECT_EQ(PPK_ASSERT_LINE - 1, _line);

    PPK_ASSERT(false);
    EXPECT_EQ(PPK_ASSERT_LINE - 4, _line);

    Local::f();
    EXPECT_EQ(PPK_ASSERT_LINE - 7, _line);

    implementation::ignoreAllAsserts(false);
#if defined(PPK_ASSERT_DISABLE_EXCEPTIONS)
    PPK_ASSERT_ERROR(false);
#else
    EXPECT_THROW(PPK_ASSERT_ERROR(false), AssertionException);
#endif
  }

  PPK_ASSERT_USED(bool) testBoolUsed()
  {
    return true;
  }

  PPK_ASSERT_USED_FATAL(bool) testBoolUsedFatal()
  {
    return true;
  }

  struct Struct
  {
    Struct()
    : count(0)
    {}

    Struct(const Struct& rhs)
    : count(rhs.count + 1)
    {}

    int count;
  };

  PPK_ASSERT_USED(Struct) testStructUsed()
  {
    return Struct();
  }

  PPK_ASSERT_USED_FATAL(Struct) testStructUsedFatal()
  {
    return Struct();
  }

  TEST_F(AssertTest, ASSERT_USED)
  {
    {
      bool b = testBoolUsed();
      EXPECT_TRUE(b);
    }

    {
      testBoolUsed();
    }
    EXPECT_EQ(_level, AssertLevel::Debug);
    EXPECT_STREQ(_message, "unused value");

    {
      bool b = testBoolUsedFatal();
      EXPECT_TRUE(b);
    }

    {
      testBoolUsedFatal();
    }
    EXPECT_EQ(_level, AssertLevel::Fatal);
    EXPECT_STREQ(_message, "unused value");

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4928)  // illegal copy-initialization; more than one
                                  // user-defined conversion has been implicitly
                                  // applied
#endif
    {
      Struct s = testStructUsed();
      EXPECT_LE(2, s.count);
    }

    {
      testStructUsed();
    }
    EXPECT_EQ(_level, AssertLevel::Debug);
    EXPECT_STREQ(_message, "unused value");

    {
      Struct s = testStructUsedFatal();
      EXPECT_LE(2, s.count);
    }

    {
      testStructUsedFatal();
    }
    EXPECT_EQ(_level, AssertLevel::Fatal);
    EXPECT_STREQ(_message, "unused value");

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
  }
}

#if defined(_WIN32)
#include <crtdbg.h>
#endif

GTEST_API_ int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);

#if defined(_WIN32)
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

  return RUN_ALL_TESTS();
}
