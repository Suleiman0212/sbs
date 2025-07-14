#pragma once

#include <string>
#include <variant>
#include <vector>

enum class TokenType {
  Unknown,
  Const,  // $SOMETHING
  Value,  // "Something"
  Vector, // [ "Something0" "Something1" ... ]
};

struct Token {
  TokenType type;
  std::variant<std::string, std::vector<std::string>> data;
  std::string tos(); // Convert token to std::string.
};

class Lexer {
public:
  Lexer() {}
  ~Lexer() {}

  std::vector<Token> lex(std::string input);
};
