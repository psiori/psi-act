// Tests for psi_act::Logger helpers.
//
// Focus: Logger::parseByteSize, the parser behind the `logger.max_size` config
// that bounds a log file's on-disk size. The size-rotation mechanics reuse the
// already-exercised rotateLogFile/clearRolledLogfiles paths; the parser is the
// new logic and the place a bad input would silently disable the disk guard.

#include "gtest/gtest.h"

#include <cstddef>

#include <psiact/utils/Logger.hpp>

namespace {

constexpr size_t KB = 1024ULL;
constexpr size_t MB = 1024ULL * 1024ULL;
constexpr size_t GB = 1024ULL * 1024ULL * 1024ULL;

using psi_act::Logger;

TEST(LoggerParseByteSize, PlainBytes) {
  EXPECT_EQ(Logger::parseByteSize("0"), 0u);
  EXPECT_EQ(Logger::parseByteSize("1"), 1u);
  EXPECT_EQ(Logger::parseByteSize("1048576"), 1048576u);
  EXPECT_EQ(Logger::parseByteSize("512B"), 512u);
}

TEST(LoggerParseByteSize, BinaryUnits) {
  EXPECT_EQ(Logger::parseByteSize("1KB"), KB);
  EXPECT_EQ(Logger::parseByteSize("500MB"), 500u * MB);
  EXPECT_EQ(Logger::parseByteSize("2GB"), 2u * GB);
  // Single-letter and *iB spellings are accepted.
  EXPECT_EQ(Logger::parseByteSize("1K"), KB);
  EXPECT_EQ(Logger::parseByteSize("1M"), MB);
  EXPECT_EQ(Logger::parseByteSize("1G"), GB);
  EXPECT_EQ(Logger::parseByteSize("1KiB"), KB);
  EXPECT_EQ(Logger::parseByteSize("1MiB"), MB);
  EXPECT_EQ(Logger::parseByteSize("1GiB"), GB);
}

TEST(LoggerParseByteSize, CaseInsensitiveAndWhitespace) {
  EXPECT_EQ(Logger::parseByteSize("500mb"), 500u * MB);
  EXPECT_EQ(Logger::parseByteSize("500Mb"), 500u * MB);
  EXPECT_EQ(Logger::parseByteSize("  500 MB  "), 500u * MB);
  EXPECT_EQ(Logger::parseByteSize("500\tMB"), 500u * MB);
}

TEST(LoggerParseByteSize, UnparseableIsUnbounded) {
  // 0 means "unbounded" — the historical behaviour. Anything we cannot make
  // sense of must fall back to that rather than to some arbitrary small cap
  // that would start silently rotating a production log.
  EXPECT_EQ(Logger::parseByteSize(""), 0u);
  EXPECT_EQ(Logger::parseByteSize("   "), 0u);
  EXPECT_EQ(Logger::parseByteSize("MB"), 0u);
  EXPECT_EQ(Logger::parseByteSize("abc"), 0u);
  EXPECT_EQ(Logger::parseByteSize("-5"), 0u);
}

TEST(LoggerParseByteSize, UnknownUnitTreatedAsBytes) {
  // A number with a nonsense unit still yields the number (as bytes) rather
  // than collapsing to unbounded — the intent to cap is clear.
  EXPECT_EQ(Logger::parseByteSize("100XY"), 100u);
}

} // namespace
