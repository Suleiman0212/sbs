#include "builder.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string read_file(const std::string &path) {
  std::ifstream file(path);
  if (!file) {
    std::cerr << "ERROR: Could not open file: " << path << std::endl;
    std::exit(1);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main(int argc, char *argv[]) {
  std::string script_path = "./build.sbs";
  if (argc == 2) {
    script_path = argv[1];
  }

  const std::string source = read_file(script_path);

  Lexer lexer;
  const auto tokens = lexer.lex(source);

  Interpreter interpreter;
  Consts consts = interpreter.interpret(tokens);

  Builder builder;
  builder.build(consts);

  return 0;
}
