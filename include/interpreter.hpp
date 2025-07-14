#pragma once

#include "dsl.hpp"
#include "lexer.hpp"

#include <optional>
#include <string>
#include <vector>

class Interpreter {
public:
  Interpreter() {}
  ~Interpreter() {}

  Consts interpret(std::vector<Token> input);
  std::optional<Const> get_const(std::string name);
  std::optional<std::string> get_value(std::string name);
  std::optional<std::vector<std::string>> get_vector(std::string name);
};
