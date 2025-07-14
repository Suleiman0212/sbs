#include "interpreter.hpp"

#include <cstdlib>
#include <iostream>

void interpret_const(std::vector<Token>::iterator &tok,
                     std::vector<Token>::iterator end, Consts &buf) {
  Token prev = *tok;
  ++tok;

  if (tok == end) {
    std::cerr << "ERROR!\n" << std::endl;
    std::cerr << "Expected `Value` or `Vector` after `Const`" << std::endl;
    exit(1);
  }

  if (tok->type != TokenType::Value && tok->type != TokenType::Vector) {
  }

  switch (tok->type) {
  case TokenType::Value:
  case TokenType::Vector:
    buf.set(Const{std::get<std::string>(prev.data), tok->data});
    break;
  default:
    std::cerr << "ERROR!\n" << std::endl;
    std::cerr << "Unexpected token after `Const` {\n"
              << tok->tos() << "\n}\n"
              << std::endl;
    exit(1);
  }

  ++tok;
}

// Main parsing process
Consts Interpreter::interpret(std::vector<Token> input) {
  Consts buf = DSL_DEFAULT_CONSTS;
  auto tok = input.begin();
  auto end = input.end();

  while (tok != end) {
    switch (tok->type) {
    case TokenType::Unknown:
      std::cerr << "ERROR:\n" << std::endl;
      std::cerr << "Unknown token {\n" << tok->tos() << "\n}\n" << std::endl;
      exit(1);
    case TokenType::Const:
      interpret_const(tok, end, buf);
      break;
    default:
      std::cerr << "ERROR:\n" << std::endl;
      std::cerr << "Expected `Const` found {\n"
                << tok->tos() << "\n}\n"
                << std::endl;
      exit(1);
    }
  }

  return buf;
}
