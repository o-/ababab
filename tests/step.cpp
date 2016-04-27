#include "tests.h"
#include "../step.h"

TEST(Step, other) {
  I i({'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'});
  std::vector<BC> code = {
    {BC_t::push, 49},
    BC_t::dec,
    BC_t::dup,
    BC_t::left,
    BC_t::dup,
    BC_t::pop,
    BC_t::nop,
    BC_t::write,
    BC_t::write,
    BC_t::inc,
    BC_t::dup,
    BC_t::write,
    {BC_t::testne, -11}};

  auto res = i.test(code);
  ASSERT_EQ(res, 0);

  S s;
  s.remove(6, code);

  res = i.test(code);
  ASSERT_EQ(res, 0);

  s.insert(6, BC_t::nop, code);

  res = i.test(code);
  ASSERT_EQ(res, 0);
}

TEST(Step, another_1) {
  I i({1,2,3,4,5,6,7});
  std::vector<BC> code = {
    BC_t::left,
    BC_t::inc,
    BC_t::write,
    {BC_t::testne, -2}};

  auto res = i.test(code);
  ASSERT_EQ(res, 0);

  S s;
  s.remove(0, code);

  res = i.test(code);
  ASSERT_EQ(res, 0);
}

TEST(Step, another_2) {
  I i({1,2,3,4,5,6,7});
  std::vector<BC> code = {
    {BC_t::writen, 0},
    BC_t::inc,
    BC_t::write,
    {BC_t::testne, -2}};

  auto res = i.test(code);
  ASSERT_EQ(res, 0);

  S s;
  s.remove(0, code);

  res = i.test(code);
  ASSERT_EQ(res, 0);
}
