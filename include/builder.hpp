#pragma once

#include "dsl.hpp"

enum class SourceType { C, CPP, ASM, Unknown };

class Builder {
public:
  Builder() {}
  ~Builder() {}

  void build(Consts data);
};
