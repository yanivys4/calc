#include "./calc/calc.hpp"
#include "./calc/calc_utils.hpp"
#include "TcpServer.hpp"
#include <iostream>
#include <string>

typedef sml::sm<Calc::states, sml::process_queue<std::queue>> SM;
void runCommand(const int& command, SM * p2sm);
using namespace sml;
