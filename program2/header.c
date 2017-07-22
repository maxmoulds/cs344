#include "header.h"

int main(int argc, char **args) {
  int i = 10;

  _test_debug(argc, args);

  trace("Debugging is enabled.");
  trace("Debug level: %d", i);

}
