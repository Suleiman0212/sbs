#pragma once

#include "../include/lexer.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Const {
  std::string name;
  std::variant<std::string, std::vector<std::string>> value;

  std::string tos();
};

class Consts {
public:
  Consts(std::vector<Const> init_data) : data(std::move(init_data)) {}
  std::optional<Const> get(std::string name);
  void set(Const value);

private:
  std::vector<Const> data;
};

class Interpreter {
public:
  Interpreter();
  ~Interpreter();

  Consts interpret(std::vector<Token> input);
  std::optional<Const> get_const(std::string name);
  std::optional<std::string> get_value(std::string name);
  std::optional<std::vector<std::string>> get_vector(std::string name);
};
