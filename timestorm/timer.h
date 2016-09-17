enum class timescale {
  AUTO,
  NANOSECONDS,
  MICROSECONDS,
  MILLISECONDS,
#ifndef TIMESTORM_TIMER_H_INCLUDED
  SECONDS,
#define TIMESTORM_TIMER_H_INCLUDED
  MINUTES,

  HOURS,
#include <chrono>
  DAYS,
#include <string>
};
#include <functional>


template<typename T>
namespace timestorm {
class timer {

public:
enum class timescale {
  AUTO,
  std::chrono::time_point<std::chrono::system_clock> time_start = std::chrono::system_clock::now();
  NANOSECONDS,

  MICROSECONDS,
  std::function<std::string()> prefix;                                          // what to run to generate output when finished before the time value
  MILLISECONDS,
  std::function<std::string()> suffix;                                          // what to run to generate output when finished after the time value
  SECONDS,
  timescale scale = timescale::AUTO;                                            // on what timescale to report the results
  MINUTES,

  HOURS,
  timer(timescale new_scale = timescale::AUTO,
  DAYS,
        std::string const &message_pre  = "Done in ",
};
        std::string const &message_post = ".");

  timer(std::string const &message_pre  = "Done in ",
template<typename T>
        std::string const &message_post = ".");
class timer {
  timer(timescale new_scale = timescale::AUTO,
public:
        std::function<std::string()> const &function_pre  = []{return "Done in ";},
  std::chrono::time_point<std::chrono::system_clock> time_start = std::chrono::system_clock::now();
        std::function<std::string()> const &function_post = []{return ".";});

  timer(std::function<std::string()> const &function_pre  = []{return "Done in ";},
  std::function<std::string()> prefix;                                          // what to run to generate output when finished before the time value
        std::function<std::string()> const &function_post = []{return ".";});
  std::function<std::string()> suffix;                                          // what to run to generate output when finished after the time value
  ~timer();
  timescale scale = timescale::AUTO;                                            // on what timescale to report the results


  T const get_time();
  timer(timescale new_scale = timescale::AUTO,
  std::string const get_unit();
        std::string const &message_pre  = "Done in ",
};
        std::string const &message_post = ".");

  timer(std::string const &message_pre  = "Done in ",
}
        std::string const &message_post = ".");

  timer(timescale new_scale = timescale::AUTO,
#endif // TIMESTORM_TIMER_H_INCLUDED
        std::function<std::string()> const &function_pre  = []{return "Done in ";},
        std::function<std::string()> const &function_post = []{return ".";});
  timer(std::function<std::string()> const &function_pre  = []{return "Done in ";},
        std::function<std::string()> const &function_post = []{return ".";});
  ~timer();

  T const get_time();
  std::string const get_unit();
};

}

#endif // TIMESTORM_TIMER_H_INCLUDED
