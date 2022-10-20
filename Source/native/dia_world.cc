// runs the world

// c++ headers
#include <iostream>

// empirical headers
#include "emp/config/command_line.hpp"
#include "emp/config/ArgManager.hpp"

// our headers
#include "../config.h"
#include "../world.h"
#include "../org.h"

int main(int argc, char* argv[])
{
  DiaConfig config;
  config.Read("Dia.cfg", false);
  auto args = emp::cl::ArgManager(argc, argv);
  if (args.ProcessConfigOptions(config, std::cout, "Dia.cfg", "Dia-macros.h") == false) exit(0);
  if (args.TestUnknown() == false) exit(0);  // If there are leftover args, throw an error.

  std::cout << "==============================" << std::endl;
  std::cout << "|    How am I configured?    |" << std::endl;
  std::cout << "==============================" << std::endl;
  config.Write(std::cout);
  std::cout << "==============================\n"
            << std::endl;

  DiagWorld world(config);

  for (size_t ud = 0; ud <= config.MAX_GENS(); ud++)
  {
    world.Update();
  }

  return 0;
}