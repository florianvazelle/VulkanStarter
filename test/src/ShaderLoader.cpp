#include <doctest/doctest.h>

#include <base_frag.h>

#include <iostream>

// Obvious test, you can remove
TEST_CASE("Load Shader") {
  try {
    auto vertShaderCode = BASE_FRAG;
    CHECK(vertShaderCode.size() > 0);
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    CHECK(false);
  }
}
