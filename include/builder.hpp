#pragma once

#include "interpreter.hpp"

enum class SourceType { C, CPP, ASM, Unknown };

class Builder {
public:
  Builder();
  ~Builder();

  void build(Interpreter it);
};
