#include <drogon/drogon.h>
#include <iostream>

int main() {
  std::cout << "welcome to prxy!\n";

  drogon::app().loadConfigFile("../config.json");
  drogon::app().run();

  return 0;
}
