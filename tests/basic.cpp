#include "tests.h"

TEST(Basic, zero_ok) {
  I i({0,0});
  std::vector<BC> code = {{BC_t::push, (B)0}, {BC_t::write, 0}, {BC_t::stop, 0}};
  auto res = i.test(code);
  ASSERT_EQ(res, 0);
}


TEST(Basic, zero_nok) {
  I i({1});
  std::vector<BC> code = {{BC_t::push, 0}, BC_t::write, BC_t::stop};
  auto res = i.test(code);
  ASSERT_EQ(res, 6);
}

TEST(Basic, zero_nok2) {
  I i({1});
  std::vector<BC> code = {{BC_t::push, 0}, BC_t::write, BC_t::right, BC_t::write, BC_t::stop};
  auto res = i.test(code);
  ASSERT_EQ(res, 6);
}

TEST(Basic, loop) {
  I i({1,1,1,1});
  std::vector<BC> code = {{BC_t::push, 1}, BC_t::write, {BC_t::jmp, -1}, BC_t::stop};
  auto res = i.test(code);
  ASSERT_EQ(res, 0);
}

TEST(Basic, loop_1) {
  I i({1,1,1,0});
  std::vector<BC> code = {{BC_t::push, 1}, BC_t::write, {BC_t::jmp, -1}, BC_t::stop};
  auto res = i.test(code);
  ASSERT_EQ(res, 6);
}

TEST(Basic, loop_2) {
  I i({1,2,0,1});
  std::vector<BC> code = {{BC_t::push, 1}, BC_t::write, {BC_t::jmp, -1}, BC_t::stop};
  auto res = i.test(code);
  ASSERT_EQ(res, 12);
}

TEST(Basic, loop_3) {
  I i({0,1,1,0});
  std::vector<BC> code = {{BC_t::push, 1}, BC_t::write, {BC_t::jmp, -1}, BC_t::stop};
  auto res = i.test(code);
  ASSERT_EQ(res, 12);
}

TEST(Basic, other) {
  I i({33,0});
  std::vector<BC> code = {{BC_t::push, 10}, BC_t::write};
  auto res = i.test(code);
  ASSERT_EQ(res, 28);
}

TEST(Basic, other_2) {
  I i({0,0,0,0,0,1,1,1});
  std::vector<BC> code = {{BC_t::push, 5}, BC_t::right, BC_t::dec, {BC_t::testnz, -2}, BC_t::inc, {BC_t::writen, 3}};
  auto res = i.test(code);
  ASSERT_EQ(res, 0);
}
