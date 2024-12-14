#pragma once

#include <chrono>
#include <string>
#include <functional>
#include <iostream>
#include <iomanip>
#include "timescale.h"

//#define TIMESTORM_NO_UNICODE

namespace timestorm {

using default_timer_type = float;

template<typename T>
concept streamlike = std::derived_from<T, std::ostream> || requires(T sink, std::string const &str) {
  // constrain sink_t to stream-like types by requiring either derivation from ostream or a stream operator that accepts strings
  { sink << str };
};


template<typename T = default_timer_type, typename sink_t = std::ostream>
class timer {
  sink_t &sink;

public:
  std::chrono::time_point<std::chrono::system_clock> time_start = std::chrono::system_clock::now();

  std::function<std::string()> prefix;                                          // what to run to generate output when finished before the time value
  std::function<std::string()> suffix;                                          // what to run to generate output when finished after the time value
  timescale scale{timescale::AUTO};                                             // on what timescale to report the results

  timer(timescale new_scale = timescale::AUTO,
        std::string const &message_pre  = "Done in ",
        std::string const &message_post = ".\n");
  timer(std::string const &message_pre  = "Done in ",
        std::string const &message_post = ".\n");
  timer(timescale new_scale = timescale::AUTO,
        std::function<std::string()> const &function_pre  = []{return "Done in ";},
        std::function<std::string()> const &function_post = []{return ".\n";});
  timer(std::function<std::string()> const &function_pre  = []{return "Done in ";},
        std::function<std::string()> const &function_post = []{return ".\n";});
  timer(streamlike auto &sink,
        timescale new_scale = timescale::AUTO,
        std::string const &message_pre  = "Done in ",
        std::string const &message_post = ".\n");
  timer(streamlike auto &sink,
        std::string const &message_pre  = "Done in ",
        std::string const &message_post = ".\n");
  timer(streamlike auto &sink,
        timescale new_scale = timescale::AUTO,
        std::function<std::string()> const &function_pre  = []{return "Done in ";},
        std::function<std::string()> const &function_post = []{return ".\n";});
  timer(streamlike auto &sink,
        std::function<std::string()> const &function_pre  = []{return "Done in ";},
        std::function<std::string()> const &function_post = []{return ".\n";});
  ~timer();

  void output();
  void reset();
  T const get_time();
  std::string const get_unit();

  void set_prefix(std::string const &new_prefix);
  void set_suffix(std::string const &new_suffix);
};

template<typename T, typename sink_t>
timer<T, sink_t>::timer(timescale new_scale,
                        std::string const &message_pre,
                        std::string const &message_post)
  : timer(std::cout,
          new_scale,
          message_pre,
          message_post) {
  /// Passthrough constructor: message, default sink
}

template<typename T, typename sink_t>
timer<T, sink_t>::timer(std::string const &message_pre,
                        std::string const &message_post)
  : timer(std::cout,
          message_pre,
          message_post) {
  /// Passthrough constructor: message, default sink and scale
}

template<typename T, typename sink_t>
timer<T, sink_t>::timer(timescale new_scale,
                        std::function<std::string()> const &function_pre,
                        std::function<std::string()> const &function_post)
  : timer(std::cout,
          new_scale,
          function_pre,
          function_post) {
  /// Passthrough constructor: function, default sink
}

template<typename T, typename sink_t>
timer<T, sink_t>::timer(std::function<std::string()> const &function_pre,
                        std::function<std::string()> const &function_post)
  : timer(std::cout,
          function_pre,
          function_post) {
  /// Passthrough constructor: function, default sink and scale
}

template<typename T, typename sink_t>
timer<T, sink_t>::timer(streamlike auto &this_sink,
                        timescale new_scale,
                        std::string const &message_pre,
                        std::string const &message_post)
  : sink{this_sink},
    prefix([message_pre]{return message_pre;}),
    suffix([message_post]{return message_post;}),
    scale(new_scale) {
  /// Specific constructor, message
}

template<typename T, typename sink_t>
timer<T, sink_t>::timer(streamlike auto &this_sink,
                        std::string const &message_pre,
                        std::string const &message_post)
  : timer(this_sink,
          timescale::AUTO,
          message_pre,
          message_post) {
  /// Passthrough constructor: message, default scale
}

template<typename T, typename sink_t>
timer<T, sink_t>::timer(streamlike auto &this_sink,
                        timescale new_scale,
                        std::function<std::string()> const &function_pre,
                        std::function<std::string()> const &function_post)
  : sink{this_sink},
    prefix(function_pre),
    suffix(function_post),
    scale(new_scale) {
  /// Specific constructor, function
}

template<typename T, typename sink_t>
timer<T, sink_t>::timer(streamlike auto &this_sink,
                        std::function<std::string()> const &function_pre,
                        std::function<std::string()> const &function_post)
  : timer(this_sink,
          timescale::AUTO,
          function_pre,
          function_post) {
  /// Passthrough constructor: function, default scale
}

template<typename T, typename sink_t>
timer<T, sink_t>::~timer() {
  /// Default destructor
  output();                                                                     // output the time on destruction
}

template<typename T, typename sink_t>
void timer<T, sink_t>::output() {
  /// Output the present time
  sink << prefix() << std::fixed << std::setprecision(1) << get_time() << get_unit() << suffix();
  if constexpr(std::is_same<sink_t, std::ostream>::value) {                     // flush std::ostream only
    sink << std::flush;
  }
}

template<typename T, typename sink_t>
void timer<T, sink_t>::reset() {
  /// Reset the timer to zero
  time_start = std::chrono::system_clock::now();
}

template<typename T, typename sink_t>
T const timer<T, sink_t>::get_time() {
  /// Return a value containing the type in whatever format is desired
  switch(scale) {
  case timescale::DAYS:
  case timescale::AUTO:
    {
      T const nanoseconds{static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - time_start).count())};
      int64_t constexpr trillion{int64_t{1'000'000} * int64_t{1'000'000}};      // this is a hack - see http://stackoverflow.com/questions/33644412/how-to-silence-long-long-integer-constant-warning-from-gcc
      if(nanoseconds < 1000) {
        scale = timescale::NANOSECONDS;
        return nanoseconds;
      } else if(nanoseconds < 1'000'000) {
        scale = timescale::MICROSECONDS;
        return nanoseconds / static_cast<T>(1'000);
      } else if(nanoseconds < 1'000'000'000) {
        scale = timescale::MILLISECONDS;
        return nanoseconds / static_cast<T>(1'000'000);
      } else if(static_cast<int64_t>(nanoseconds) < trillion) {
        scale = timescale::SECONDS;
        return nanoseconds / static_cast<T>(1'000'000'000);
      } else {
        T const seconds{static_cast<T>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - time_start).count())};
        if(seconds < 60) {
          scale = timescale::SECONDS;
          return seconds;
        } else if(seconds < 60 * 60) {
          scale = timescale::MINUTES;
          return seconds / static_cast<T>(60);
        } else if(seconds < 60 * 60 * 24) {
          scale = timescale::HOURS;
          return seconds / static_cast<T>(60 * 60);
        } else {
          scale = timescale::DAYS;
          return seconds / static_cast<T>(60 * 60 * 24);
        }
      }
    }
    [[fallthrough]];
  case timescale::NANOSECONDS:
    return static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::system_clock::now() - time_start).count());
  case timescale::MICROSECONDS:
    //return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - time_start).count();
    return static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - time_start).count()) / static_cast<T>(1'000);
  case timescale::MILLISECONDS:
    //return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_start).count();
    return static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::system_clock::now() - time_start).count()) / static_cast<T>(1'000'000);
  case timescale::SECONDS:
    //return std::chrono::duration_cast<std::chrono::seconds>(     std::chrono::system_clock::now() - time_start).count();
    //return std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::system_clock::now() - time_start).count() / 1'000'000;
    //return static_cast<T>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_start).count()) * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den;
    return static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - time_start).count()) / static_cast<T>(1'000'000'000);
  case timescale::MINUTES:
    //return std::chrono::duration_cast<std::chrono::minutes>(     std::chrono::system_clock::now() - time_start).count();
    //return std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::system_clock::now() - time_start).count() / (1'000'000 * 60);
    return static_cast<T>(std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now() - time_start).count()) / static_cast<T>(60);
  case timescale::HOURS:
    //return std::chrono::duration_cast<std::chrono::hours>(       std::chrono::system_clock::now() - time_start).count();
    //return std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::system_clock::now() - time_start).count() / (1'000'000 * 60 * 60);
    return static_cast<T>(std::chrono::duration_cast<std::chrono::minutes>( std::chrono::system_clock::now() - time_start).count()) / static_cast<T>(60);
  }
  return {};                                                                    // not actually reachable
}

template<typename T, typename sink_t>
std::string const timer<T, sink_t>::get_unit() {
  switch(scale) {
  case timescale::DAYS:
  case timescale::AUTO:
    {
      T const nanoseconds{static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - time_start).count())};
      int64_t constexpr trillion{int64_t{1'000'000} * int64_t{1'000'000}};      // this is a hack - see http://stackoverflow.com/questions/33644412/how-to-silence-long-long-integer-constant-warning-from-gcc
      if(nanoseconds < 1000) {
        scale = timescale::NANOSECONDS;
        return get_unit();
      } else if(nanoseconds < 1'000'000) {
        scale = timescale::MICROSECONDS;
        return get_unit();
      } else if(nanoseconds < 1'000'000'000) {
        scale = timescale::MILLISECONDS;
        return get_unit();
      } else if(static_cast<int64_t>(nanoseconds) < trillion) {
        scale = timescale::SECONDS;
        return get_unit();
      } else {
        T const seconds{static_cast<T>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - time_start).count())};
        if(seconds < 60) {
          scale = timescale::SECONDS;
          return get_unit();
        } else if(seconds < 60 * 60) {
          scale = timescale::MINUTES;
          return get_unit();
        } else if(seconds < 60 * 60 * 24) {
          scale = timescale::HOURS;
          return get_unit();
        } else {
          scale = timescale::DAYS;
          return get_unit();
        }
      }
    }
    [[fallthrough]];
  case timescale::NANOSECONDS:
    return "ns";
  case timescale::MICROSECONDS:
    #ifdef TIMESTORM_NO_UNICODE
      return "us";
    #else
      return "μs";
    #endif // TIMESTORM_NO_UNICODE
  case timescale::MILLISECONDS:
    return "ms";
  case timescale::SECONDS:
    return "s";
  case timescale::MINUTES:
    return "m";
  case timescale::HOURS:
    return "h";
  }
  return {};                                                                    // not actually reachable
}

template<typename T, typename sink_t>
void timer<T, sink_t>::set_prefix(std::string const &new_prefix) {
  prefix = [new_prefix]{return new_prefix;};
}
template<typename T, typename sink_t>
void timer<T, sink_t>::set_suffix(std::string const &new_suffix) {
  suffix = [new_suffix]{return new_suffix;};
}

}
