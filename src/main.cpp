#include "builder.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

std::string read_file(std::string path) {
  std::fstream file(path);
  if (!file) {
    std::cerr << "ERROR!" << std::endl;
    std::cerr << "File(" << path << ") could not be opened" << std::endl;
    exit(1);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return buffer.str();
}

int main(int argc, char *argv[]) {
  std::string path = "./build.sbs";

  if (argc == 2) {
    path = argv[2];
  }

  std::string data = read_file(path);

  Lexer lx;
  auto tokens = lx.lex(data);

  Interpreter it;
  auto consts = it.interpret(tokens);

  Builder bl;
  bl.build(it);

  return 0;
}
