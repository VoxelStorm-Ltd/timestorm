#ifndef TIMESTORM_TIMER_H_INCLUDED
#define TIMESTORM_TIMER_H_INCLUDED

#include <chrono>
#include <string>

namespace timestorm {

enum class timescale {
  AUTO,
  NANOSECONDS,
  MICROSECONDS,
  MILLISECONDS,
  SECONDS,
  MINUTES,
  HOURS,
  DAYS,
};

template<typename T>
class timer {
public:
  std::chrono::time_point<std::chrono::system_clock> time_start = std::chrono::system_clock::now();

  std::string prefix;                                                           // what to output when finished before the time value
  std::string suffix;                                                           // what to output when finished after the time value
  timescale scale = timescale::AUTO;                                            // on what timescale to report the results

  timer(timescale new_scale = timescale::AUTO,
        std::string const &message_pre  = "Done in ",
        std::string const &message_post = ".");
  timer(std::string const &message_pre  = "Done in ",
        std::string const &message_post = ".");
  ~timer();

  T const get_time();
  std::string const get_unit();
};

}

#endif // TIMESTORM_TIMER_H_INCLUDED
