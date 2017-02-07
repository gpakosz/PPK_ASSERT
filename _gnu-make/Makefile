.PHONY: build test clean

# directories
ifeq ($(realpath .),)
  $(error your version of Make doesn't support $$(realpath names...) - please use GNU Make 3.81 or later)
endif

ifeq ($(platform),)
  __uname_s := $(shell sh -c 'uname -s 2>/dev/null | tr [A-Z] [a-z] || echo unknown-platform')

  platform := $(__uname_s)
  ifeq ($(__uname_s),darwin)
    override platform := mac
  endif
  ifeq ($(__uname_s),sunos)
    override platform := solaris
  endif
endif
ifeq ($(architecture),)
  __uname_m := $(shell sh -c 'uname -m 2>/dev/null | tr [A-Z] [a-z] || echo unknown-architecture')

  architecture := $(__uname_m)
endif

prefix := $(realpath ..)
srcdir := $(realpath ../src)
exampledir := $(realpath ../example)
testdir := $(realpath ../test)
buildir := $(realpath .)/build
binsubdir := $(platform)-$(architecture)
bindir := $(prefix)/bin/$(binsubdir)

CPPFLAGS := -DPPK_ASSERT_LOG_FILE=\"assert.txt\" -DPPK_ASSERT_LOG_FILE_TRUNCATE
CXXFLAGS := -O2 -g -Wall -Wextra

GTEST_CXXFLAGS := -std=c++03
ifeq ($(platform),linux)
  GTEST_CXXFLAGS += -pthread
endif

.PHONY: build-test
build: build-test
build-test: $(bindir)/test $(bindir)/test-no-stl $(bindir)/test-no-exceptions

$(bindir)/test: $(srcdir)/ppk_assert.cpp $(srcdir)/ppk_assert.h $(testdir)/ppk_assert_test.cpp $(testdir)/gtest/gtest-all.cc $(testdir)/gtest/gtest.h
	mkdir -p $(@D)
	$(CXX) -I $(srcdir) -I$(testdir) $(CPPFLAGS) $(CXXFLAGS) $(GTEST_CXXFLAGS) $(filter-out %.h,$^) $(LDFLAGS) -o $@
	$(if $(postbuild),$(postbuild) $@)

$(bindir)/test-no-stl: $(srcdir)/ppk_assert.cpp $(srcdir)/ppk_assert.h $(testdir)/ppk_assert_test.cpp $(testdir)/gtest/gtest-all.cc $(testdir)/gtest/gtest.h
	mkdir -p $(@D)
	$(CXX) -I $(srcdir) -I$(testdir) $(CPPFLAGS) -DPPK_ASSERT_DISABLE_STL $(CXXFLAGS) $(GTEST_CXXFLAGS) $(filter-out %.h,$^) $(LDFLAGS) -o $@
	$(if $(postbuild),$(postbuild) $@)

$(bindir)/test-no-exceptions: $(srcdir)/ppk_assert.cpp $(srcdir)/ppk_assert.h $(testdir)/ppk_assert_test.cpp $(testdir)/gtest/gtest-all.cc $(testdir)/gtest/gtest.h
	mkdir -p $(@D)
	$(CXX) -I $(srcdir) -I$(testdir) $(CPPFLAGS) -DPPK_ASSERT_DISABLE_EXCEPTIONS $(CXXFLAGS) $(GTEST_CXXFLAGS) $(filter-out %.h,$^) $(LDFLAGS) -o $@
	$(if $(postbuild),$(postbuild) $@)

.PHONY: build-example
build: build-example
build-example: $(bindir)/example

$(bindir)/example: $(srcdir)/ppk_assert.cpp $(srcdir)/ppk_assert.h $(exampledir)/main.cpp
	mkdir -p $(@D)
	$(CXX) -I $(srcdir) $(CPPFLAGS) $(CXXFLAGS) $(filter-out %.h,$^) $(LDFLAGS) -o $@
	$(if $(postbuild),$(postbuild) $@)

.PHONY: test
test : build-test
	$(bindir)/test
	$(bindir)/test-no-stl
	$(bindir)/test-no-exceptions

clean:
	rm -rf $(buildir)
	rm -rf $(bindir)
