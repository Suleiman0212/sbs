#include "lexer.hpp"

#include <cctype>
#include <string>
#include <vector>

// Token To String
std::string Token::tos() {
  std::string buffer;
  buffer.append("\tTokenType: ");
  switch (type) {
  case TokenType::Unknown:
    buffer.append("Unknown");
    break;
  case TokenType::Const:
    buffer.append("Const");
    break;
  case TokenType::Value:
    buffer.append("Value");
    break;
  case TokenType::Vector:
    buffer.append("Vector");
    break;
  default:
    buffer.append("Unknown");
    break;
  }

  buffer.append("\n\tTokenData: ");
  std::visit(
      [&buffer](const auto &value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::string>) {
          buffer.append(value);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
          for (auto v : value) {
            buffer.append(v);
            buffer.append(" ");
          }
        }
      },
      data);
  return buffer;
}

// Lex const, $something
Token lex_const(std::string::iterator &ch, std::string::iterator end) {
  std::string data;

  bool is_const = *ch == '$';
  ++ch;

  while (ch < end && (std::isalpha(*ch) || *ch == '_' || *ch == '-')) {
    data.push_back(*ch);
    ++ch;
  }

  if (is_const)
    return Token{TokenType::Const, data};
  else
    return Token{TokenType::Unknown, data};
}

// Lex string, 'something'
Token lex_value(std::string::iterator &ch, std::string::iterator end) {
  std::string data;
  ++ch;

  while (ch < end && *ch != '\'') {
    data.push_back(*ch);
    ++ch;
  }

  if (ch < end && *ch == '\'') {
    ++ch;
  }

  return Token{TokenType::Value, data};
}

// Lex vector, [ 'something0' 'something1' ]
Token lex_vector(std::string::iterator &ch, std::string::iterator end) {
  std::vector<std::string> data;
  ++ch;

  while (ch < end) {
    while (ch < end && std::isspace(*ch))
      ++ch;

    if (*ch == ']') {
      ++ch;
      break;
    }

    if (*ch == '\'') {
      auto tk = lex_value(ch, end);
      if (std::holds_alternative<std::string>(tk.data)) {
        data.emplace_back(std::get<std::string>(tk.data));
      } else {
        data.emplace_back("!bad_token");
      }
    } else {
      ++ch;
    }
  }

  return Token{TokenType::Vector, data};
}

std::vector<Token> Lexer::lex(std::string input) {
  std::vector<Token> tokens;
  auto ch = input.begin();

  while (ch != input.end()) {
    // Skip spaces, tabs and newlines
    if (std::isspace(*ch) || *ch == '_' || *ch == '-') {
      ++ch;
      continue;
    }

    // Check if char is special symbol
    switch (*ch) {
    case '$':
      tokens.push_back(lex_const(ch, input.end()));
      continue;

    // String start symbol
    case '\'':
      tokens.push_back(lex_value(ch, input.end()));
      continue;

    case '[':
      tokens.push_back(lex_vector(ch, input.end()));
      continue;

    // Unknown character
    default:
      tokens.push_back(Token{TokenType::Unknown, std::string{*ch}});
      ++ch;
    }
  }

  return tokens;
}
