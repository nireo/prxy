#ifndef PR_LOGGER_H
#define PR_LOGGER_H

#include <iostream>
#include <string>

enum class LogType {
  Debug,
  Info,
  Warning,
  Error,
};

class LOG {
public:
  LOG() {}
  LOG(LogType type) { operator<<("[" + getLabel(type) + "]"); }
  ~LOG() { std::cout << '\n'; }

  template <class T> LOG &operator<<(const T &msg) {
    std::cout << msg;
    return *this;
  }

private:
  inline std::string getLabel(LogType type) {
    switch (type) {
    case LogType::Debug:
      return "DEBUG";
    case LogType::Info:
      return "INFO";
    case LogType::Warning:
      return "WARNING";
    case LogType::Error:
      return "ERROR";
    }
  }
};

#endif
