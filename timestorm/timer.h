#ifndef TIMESTORM_TIMER_H_INCLUDED
#define TIMESTORM_TIMER_H_INCLUDED

#include <chrono>
#include <string>
#include <functional>
#include "timescale.h"

namespace timestorm {

template<typename T>
class timer {
public:
  std::chrono::time_point<std::chrono::system_clock> time_start = std::chrono::system_clock::now();

  std::function<std::string()> prefix;                                          // what to run to generate output when finished before the time value
  std::function<std::string()> suffix;                                          // what to run to generate output when finished after the time value
  timescale scale = timescale::AUTO;                                            // on what timescale to report the results

  timer(timescale new_scale = timescale::AUTO,
        std::string const &message_pre  = "Done in ",
        std::string const &message_post = ".");
  timer(std::string const &message_pre  = "Done in ",
        std::string const &message_post = ".");
  timer(timescale new_scale = timescale::AUTO,
        std::function<std::string()> const &function_pre  = []{return "Done in ";},
        std::function<std::string()> const &function_post = []{return ".";});
  timer(std::function<std::string()> const &function_pre  = []{return "Done in ";},
        std::function<std::string()> const &function_post = []{return ".";});
  ~timer();

  void output();
  void reset();
  T const get_time();
  std::string const get_unit();

  void set_prefix(std::string const &new_prefix);
  void set_suffix(std::string const &new_suffix);
};

}

#endif // TIMESTORM_TIMER_H_INCLUDED
