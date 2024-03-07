#pragma once

#include <iostream>
#include <string>

namespace logger {
template <typename T>
inline void error(T text) {
  std::cerr << "error: " << text << std::endl;
}
template <typename T>
inline void warn(T text) {
  std::cout << "warn: " << text << std::endl;
}
template <typename T>
inline void info(T text) {
  std::cout << "info: " << text << std::endl;
}
}  // namespace logger
