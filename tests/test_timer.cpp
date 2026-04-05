#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <chrono>
#include <functional>
#include <sstream>
#include <string>
#include <thread>

#include "timestorm/timestorm.h"

using namespace timestorm;
using Catch::Approx;

// Capture timer output by using an ostringstream as the sink.
// The timer is destroyed inside the braces, triggering its destructor output.
template<typename T = float>
static std::string capture_output(timescale scale,
                                  std::string const &pre,
                                  std::string const &post) {
  std::ostringstream oss;
  {
    timer<T> t(static_cast<std::ostream &>(oss), scale, pre, post);
  }
  return oss.str();
}

// ─────────────────────────────────────────────────────────────────────────────
// timescale enum
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("timescale enum values are all distinct", "[timescale]") {
  // Verify each enumerator exists and is distinct from the others
  CHECK(timescale::AUTO        != timescale::NANOSECONDS);
  CHECK(timescale::NANOSECONDS != timescale::MICROSECONDS);
  CHECK(timescale::MICROSECONDS!= timescale::MILLISECONDS);
  CHECK(timescale::MILLISECONDS!= timescale::SECONDS);
  CHECK(timescale::SECONDS     != timescale::MINUTES);
  CHECK(timescale::MINUTES     != timescale::HOURS);
  CHECK(timescale::HOURS       != timescale::DAYS);
}

// ─────────────────────────────────────────────────────────────────────────────
// Construction and output format
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("default prefix and suffix appear in output", "[timer][output]") {
  std::ostringstream oss;
  {
    // Pass the default strings explicitly to the sink-based constructor
    timer<float> t(static_cast<std::ostream &>(oss), "Done in ", ".\n");
  }
  std::string const out = oss.str();
  CHECK(out.find("Done in ") != std::string::npos);
  CHECK(out.find(".\n")      != std::string::npos);
}

TEST_CASE("custom string prefix and suffix appear in output", "[timer][output]") {
  std::string const out = capture_output(timescale::NANOSECONDS, "Elapsed: ", " ns-suffix");
  CHECK(out.find("Elapsed: ")  != std::string::npos);
  CHECK(out.find(" ns-suffix") != std::string::npos);
}

TEST_CASE("output contains a numeric value between prefix and suffix", "[timer][output]") {
  std::string const out = capture_output(timescale::NANOSECONDS, "T:", "U");
  // The string should be "T:<number>nsU"
  auto pre_pos  = out.find("T:");
  auto suf_pos  = out.find("U", pre_pos);
  REQUIRE(pre_pos != std::string::npos);
  REQUIRE(suf_pos != std::string::npos);
  CHECK(suf_pos > pre_pos + 2); // there is content between prefix and suffix
}

TEST_CASE("function-based prefix and suffix appear in output", "[timer][output]") {
  std::ostringstream oss;
  {
    timer<float> t(
      static_cast<std::ostream &>(oss),
      []{ return std::string("FuncPre:"); },
      []{ return std::string(":FuncPost"); });
  }
  std::string const out = oss.str();
  CHECK(out.find("FuncPre:") != std::string::npos);
  CHECK(out.find(":FuncPost") != std::string::npos);
}

TEST_CASE("function-based prefix/suffix with explicit scale", "[timer][output]") {
  std::ostringstream oss;
  {
    timer<float> t(
      static_cast<std::ostream &>(oss),
      timescale::MILLISECONDS,
      []{ return std::string("Pre>"); },
      []{ return std::string("<Post"); });
  }
  std::string const out = oss.str();
  CHECK(out.find("Pre>") != std::string::npos);
  CHECK(out.find("ms")   != std::string::npos);
  CHECK(out.find("<Post") != std::string::npos);
}

TEST_CASE("output() can be called explicitly and multiple times", "[timer][output]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS, "X", "Y");
  t.output();
  t.output();
  std::string const out = oss.str();
  // Two calls → "X" appears at least twice
  auto first  = out.find("X");
  REQUIRE(first != std::string::npos);
  auto second = out.find("X", first + 1);
  CHECK(second != std::string::npos);
  // Destructor will add a third; that is fine
}

// ─────────────────────────────────────────────────────────────────────────────
// get_unit() – explicit scales
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("get_unit returns correct string for explicit timescales", "[timer][get_unit]") {
  std::ostringstream oss;

  SECTION("NANOSECONDS → ns") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS, "", "");
    CHECK(t.get_unit() == "ns");
  }

  SECTION("MICROSECONDS → μs") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::MICROSECONDS, "", "");
#ifdef TIMESTORM_NO_UNICODE
    CHECK(t.get_unit() == "us");
#else
    CHECK(t.get_unit() == "μs");
#endif
  }

  SECTION("MILLISECONDS → ms") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::MILLISECONDS, "", "");
    CHECK(t.get_unit() == "ms");
  }

  SECTION("SECONDS → s") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::SECONDS, "", "");
    CHECK(t.get_unit() == "s");
  }

  SECTION("MINUTES → m") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::MINUTES, "", "");
    CHECK(t.get_unit() == "m");
  }

  SECTION("HOURS → h") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::HOURS, "", "");
    CHECK(t.get_unit() == "h");
  }
}

TEST_CASE("output contains the correct unit string for explicit scales", "[timer][output]") {
  SECTION("NANOSECONDS → ns in output") {
    CHECK(capture_output(timescale::NANOSECONDS,  "", "").find("ns") != std::string::npos);
  }
  SECTION("MICROSECONDS → μs in output") {
#ifdef TIMESTORM_NO_UNICODE
    CHECK(capture_output(timescale::MICROSECONDS, "", "").find("us") != std::string::npos);
#else
    CHECK(capture_output(timescale::MICROSECONDS, "", "").find("μs") != std::string::npos);
#endif
  }
  SECTION("MILLISECONDS → ms in output") {
    CHECK(capture_output(timescale::MILLISECONDS, "", "").find("ms") != std::string::npos);
  }
  SECTION("SECONDS → s in output") {
    std::string const out = capture_output(timescale::SECONDS, "", "");
    // With empty suffix the output ends with the unit; use back() to avoid
    // false matches against "ns", "μs", or "ms" which also contain 's'.
    REQUIRE(!out.empty());
    CHECK(out.back() == 's');
  }
  SECTION("MINUTES → m in output") {
    std::string const out = capture_output(timescale::MINUTES, "", "");
    // With empty suffix the output ends with the unit; use back() to avoid
    // false matches against "ms" which also contains 'm'.
    REQUIRE(!out.empty());
    CHECK(out.back() == 'm');
  }
  SECTION("HOURS → h in output") {
    CHECK(capture_output(timescale::HOURS,        "", "").find("h")  != std::string::npos);
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// get_time() – explicit scales
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("get_time returns a non-negative value for all explicit scales", "[timer][get_time]") {
  std::ostringstream oss;

  SECTION("NANOSECONDS") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS, "", "");
    CHECK(t.get_time() >= 0.0f);
  }
  SECTION("MICROSECONDS") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::MICROSECONDS, "", "");
    CHECK(t.get_time() >= 0.0f);
  }
  SECTION("MILLISECONDS") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::MILLISECONDS, "", "");
    CHECK(t.get_time() >= 0.0f);
  }
  SECTION("SECONDS") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::SECONDS, "", "");
    CHECK(t.get_time() >= 0.0f);
  }
  SECTION("MINUTES") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::MINUTES, "", "");
    CHECK(t.get_time() >= 0.0f);
  }
  SECTION("HOURS") {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::HOURS, "", "");
    CHECK(t.get_time() >= 0.0f);
  }
}

TEST_CASE("get_time in NANOSECONDS is positive shortly after construction", "[timer][get_time]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS, "", "");

  float elapsed = t.get_time();
  auto const deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(10);
  while(elapsed <= 0.0f && std::chrono::steady_clock::now() < deadline) {
    std::this_thread::yield();
    elapsed = t.get_time();
  }

  CHECK(elapsed > 0.0f);
}

TEST_CASE("get_time with MILLISECONDS scale after 30ms sleep is approximately correct", "[timer][get_time][accuracy]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::MILLISECONDS, "", "");
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  float const elapsed = t.get_time();
  // Allow generous bounds for CI scheduler jitter
  CHECK(elapsed >= 20.0f);
  CHECK(elapsed < 500.0f);
}

TEST_CASE("get_time with SECONDS scale after 50ms sleep returns a small fractional second", "[timer][get_time][accuracy]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::SECONDS, "", "");
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  float const elapsed = t.get_time();
  CHECK(elapsed >= 0.03f);
  CHECK(elapsed <  2.0f);
}

// ─────────────────────────────────────────────────────────────────────────────
// AUTO scale selection
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("AUTO scale selects NANOSECONDS or MICROSECONDS for an immediately-queried timer", "[timer][auto]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::AUTO, "", "");
  t.get_time(); // side-effect: sets scale based on elapsed time
  // Elapsed time since construction is tiny, but clock resolution and scheduling
  // may legitimately place it in either ns or μs.
  CHECK((t.scale == timescale::NANOSECONDS || t.scale == timescale::MICROSECONDS));
}

TEST_CASE("AUTO scale selects MILLISECONDS after a 20ms sleep", "[timer][auto]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::AUTO, "", "");
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  t.get_time();
  CHECK(t.scale == timescale::MILLISECONDS);
}

TEST_CASE("AUTO scale output contains a recognised time unit", "[timer][auto]") {
  // We just check that some recognised unit appears – the exact choice is time-dependent.
  std::string const out = capture_output(timescale::AUTO, "", "");
  bool const has_unit =
    out.find("ns") != std::string::npos ||
    out.find("μs") != std::string::npos ||
    out.find("ms") != std::string::npos ||
    out.find("s")  != std::string::npos;
  CHECK(has_unit);
}

// ─────────────────────────────────────────────────────────────────────────────
// reset()
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("reset() restarts the elapsed time counter", "[timer][reset]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::MILLISECONDS, "", "");
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  float const before = t.get_time();
  t.reset();
  float const after  = t.get_time();
  CHECK(before > after);
  CHECK(after  >= 0.0f);
}

TEST_CASE("reset() updates time_start to the current time", "[timer][reset]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), "", "");
  auto const before_reset = t.time_start;
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  t.reset();
  CHECK(t.time_start > before_reset);
}

// ─────────────────────────────────────────────────────────────────────────────
// set_prefix() / set_suffix()
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("set_prefix replaces the original prefix in output", "[timer][prefix]") {
  std::ostringstream oss;
  {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS,
                   "Original: ", " end");
    t.set_prefix("Updated: ");
  }
  std::string const out = oss.str();
  CHECK(out.find("Updated: ")  != std::string::npos);
  CHECK(out.find("Original: ") == std::string::npos);
}

TEST_CASE("set_suffix replaces the original suffix in output", "[timer][suffix]") {
  std::ostringstream oss;
  {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS,
                   "pre ", " original-suffix");
    t.set_suffix(" updated-suffix");
  }
  std::string const out = oss.str();
  CHECK(out.find(" updated-suffix")  != std::string::npos);
  CHECK(out.find(" original-suffix") == std::string::npos);
}

TEST_CASE("set_prefix and set_suffix can both be changed before output", "[timer][prefix][suffix]") {
  std::ostringstream oss;
  {
    timer<float> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS, "A:", ":B");
    t.set_prefix("C:");
    t.set_suffix(":D");
  }
  std::string const out = oss.str();
  CHECK(out.find("C:") != std::string::npos);
  CHECK(out.find(":D") != std::string::npos);
  CHECK(out.find("A:") == std::string::npos);
  CHECK(out.find(":B") == std::string::npos);
}

// ─────────────────────────────────────────────────────────────────────────────
// Template parameter T (precision type)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("timer<float> compiles, constructs, and outputs", "[timer][template]") {
  std::string const out = capture_output<float>(timescale::NANOSECONDS, "f:", "");
  CHECK(out.find("f:") != std::string::npos);
  CHECK(out.find("ns") != std::string::npos);
}

TEST_CASE("timer<double> compiles, constructs, and outputs", "[timer][template]") {
  std::ostringstream oss;
  {
    timer<double> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS, "d:", "");
  }
  std::string const out = oss.str();
  CHECK(out.find("d:") != std::string::npos);
  CHECK(out.find("ns") != std::string::npos);
}

TEST_CASE("timer<int> compiles, constructs, and outputs", "[timer][template]") {
  std::ostringstream oss;
  {
    timer<int> t(static_cast<std::ostream &>(oss), timescale::MILLISECONDS, "i:", "");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  std::string const out = oss.str();
  CHECK(out.find("i:") != std::string::npos);
  CHECK(out.find("ms") != std::string::npos);
}

TEST_CASE("timer<unsigned int> compiles, constructs, and outputs", "[timer][template]") {
  std::ostringstream oss;
  {
    timer<unsigned int> t(static_cast<std::ostream &>(oss), timescale::MILLISECONDS, "u:", "");
  }
  std::string const out = oss.str();
  CHECK(out.find("u:") != std::string::npos);
}

// ─────────────────────────────────────────────────────────────────────────────
// Public data members
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("time_start is set to approximately the construction time", "[timer][members]") {
  auto const before = std::chrono::system_clock::now();
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), "", "");
  auto const after = std::chrono::system_clock::now();

  CHECK(t.time_start >= before);
  CHECK(t.time_start <= after);
}

TEST_CASE("scale member reflects explicitly requested timescale", "[timer][members]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::SECONDS, "", "");
  CHECK(t.scale == timescale::SECONDS);
}

TEST_CASE("scale defaults to AUTO when not specified", "[timer][members]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), "", "");
  CHECK(t.scale == timescale::AUTO);
}

TEST_CASE("prefix and suffix functions are callable", "[timer][members]") {
  std::ostringstream oss;
  timer<float> t(static_cast<std::ostream &>(oss), timescale::NANOSECONDS, "p:", ":s");
  REQUIRE(t.prefix);
  REQUIRE(t.suffix);
  CHECK(t.prefix() == "p:");
  CHECK(t.suffix() == ":s");
}
