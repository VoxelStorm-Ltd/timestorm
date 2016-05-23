#include "timer.h"
#include <iostream>

namespace timestorm {

template<typename T>
timer<T>::timer(timescale new_scale,
                std::string const &message_pre,
                std::string const &message_post)
  : prefix([message_pre]{return message_pre;}),
    suffix([message_post]{return message_post;}),
    scale(new_scale) {
  /// Passthrough constructor
}

template<typename T>
timer<T>::timer(std::string const &message_pre,
                std::string const &message_post)
  : timer(timescale::AUTO,
          message_pre,
          message_post) {
  /// Passthrough constructor
}

template<typename T>
timer<T>::timer(timescale new_scale,
                std::function<std::string()> const &function_pre,
                std::function<std::string()> const &function_post)
  : prefix(function_pre),
    suffix(function_post),
    scale(new_scale) {
  /// Default constructor
}

template<typename T>
timer<T>::timer(std::function<std::string()> const &function_pre,
                std::function<std::string()> const &function_post)
  : timer(timescale::AUTO,
          function_pre,
          function_post) {
  /// Passthrough constructor
}

template<typename T>
timer<T>::~timer() {
  /// Default destructor
  std::cout << prefix() << get_time() << get_unit() << suffix() << std::endl;   // output the time on destruction
}

template<typename T>
T const timer<T>::get_time() {
  /// Return a value containing the type in whatever format is desired
  switch(scale) {
  case timescale::AUTO:
  default:
    {
      T const nanoseconds = static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - time_start).count());
      int64_t constexpr const trillion = int64_t(1'000'000) * int64_t(1'000'000); // this is a hack - see http://stackoverflow.com/questions/33644412/how-to-silence-long-long-integer-constant-warning-from-gcc
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
        T const seconds = static_cast<T>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - time_start).count());
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
}

template<typename T>
std::string const timer<T>::get_unit() {
  switch(scale) {
  case timescale::AUTO:
  default:
    {
      T const nanoseconds = static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - time_start).count());
      int64_t constexpr const trillion = int64_t(1'000'000) * int64_t(1'000'000); // this is a hack - see http://stackoverflow.com/questions/33644412/how-to-silence-long-long-integer-constant-warning-from-gcc
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
        T const seconds = static_cast<T>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - time_start).count());
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
  case timescale::NANOSECONDS:
    return "ns";
  case timescale::MICROSECONDS:
    return "us";
  case timescale::MILLISECONDS:
    return "ms";
  case timescale::SECONDS:
    return "s";
  case timescale::MINUTES:
    return "m";
  case timescale::HOURS:
    return "h";
  }
}

// explicit instantiations
template class timer<float>;
template class timer<double>;
template class timer<int>;
template class timer<unsigned int>;

}
