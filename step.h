#ifndef STEP_H
#define STEP_H

#include "bc.h"
#include <random>
#include <functional>
#include <list>

class S {
 public:
  //typedef std::uniform_int_distribution<std::ranlux48_base::result_type> Dist;
  //std::ranlux48_base rng;

  typedef std::uniform_int_distribution<std::minstd_rand::result_type> Dist;
  std::minstd_rand rng;

  Dist d_bc;

  Dist d_search_accept_longer;
  Dist d_search_accept_wrong;
  Dist d_search_accept_reset;

  Dist d_narrow_accept_longer;
  Dist d_narrow_accept_wrong;
  Dist d_narrow_accept_reset;

  Dist d_accept_write;
  Dist d_accept_write_i;

  Dist d_coin;
  Dist d_byte;

  float m_cutoff_center;

  S() : d_bc(0, (int)BC_t::num_of - 1),
        d_search_accept_longer(0, 20),
        d_search_accept_wrong(0, 300),
        d_search_accept_reset(0, 20000000),
        d_narrow_accept_longer(0, 300),
        d_narrow_accept_wrong(0, 10000),
        d_narrow_accept_reset(0, 1000000),
        d_accept_write(0,8),
        d_accept_write_i(0,20),
        d_coin(0,1),
        d_byte(0, 0xff),
        m_cutoff_center(1.8) {
    rng.seed(std::random_device()());
    //rng.seed(12);
  }

  void reset(std::vector<BC>& code) {
    if (d_search_accept_reset(rng) == 0) {
      std::cout << "<- shuffle ->\n";
      code = {BC_t::nop};
    }
  }

  void reset(std::vector<BC>& code, std::list<std::vector<BC>>& good_code) {
    if (d_narrow_accept_reset(rng) == 0) {
      Dist dc(0, good_code.size() - 1);
      auto it = good_code.begin();
      std::advance(it, dc(rng));
      code = *it;
    }
  }

  bool shouldAccept(unsigned old_diff, unsigned new_diff,
                    unsigned old_len, unsigned new_len) {
    if (new_diff < old_diff)
      return true;

    if (new_diff < old_diff+5 && new_diff < new_len*10) {
      if (new_len < old_len)
        return true;
      else
        return d_search_accept_longer(rng) == 0;
    }

    return d_search_accept_wrong(rng) == 0;
  }

  bool shouldAccept2(unsigned old_diff, unsigned new_diff,
                     unsigned old_len, unsigned new_len) {
    if (new_diff < old_diff)
      return true;
    if (new_diff == old_diff) {
      if (new_len < old_len)
        return true;
      else
        return d_narrow_accept_longer(rng) == 0;
    }
    return d_narrow_accept_wrong(rng) == 0;
  }


  BC_t rand_bc() {
    BC_t bc = (BC_t)d_bc(rng);
    while ((bc == BC_t::write || bc == BC_t::writen) && d_accept_write(rng) != 0)
      bc = (BC_t)d_bc(rng);
    while (bc == BC_t::writei && d_accept_write_i(rng) != 0)
      bc = (BC_t)d_bc(rng);
    return bc;
  }

  // Modify random bytecode
  void m1(Dist& d, std::vector<BC>& code) {
    unsigned pos = d(rng);
    BC_t bc = rand_bc();
    code[pos].bc = bc;
  }

  // Append random bytecode + random immediate
  void m2(Dist& d, std::vector<BC>& code) {
    code.push_back(BC({rand_bc(), (B)d_byte(rng)}));
  }

  // Modify random immediate
  void m3(Dist& d, std::vector<BC>& code) {
    code[d(rng)].immediate = d_byte(rng);
  }

  // Modify random normal dist immediate
  void m32(Dist& d, std::vector<BC>& code) {
    std::normal_distribution<double> dn(
        0,
        code.size() / 4.0);
    code[d(rng)].immediate = dn(rng);
  }

  // Modify random bytecode + immediate
  void m4(Dist& d, std::vector<BC>& code) {
    unsigned pos = d(rng);
    BC_t bc = rand_bc();
    code[pos] = bc;
  }

  // Insert random bytecode
  void m5(Dist& d, std::vector<BC>& code) {
    unsigned pos = d(rng);
    BC_t bc = rand_bc();
    insert(pos, bc, code);
  }

  void insert(unsigned pos, BC_t bc, std::vector<BC>& code) {
    code.push_back(BC_t::nop);
    for (unsigned i = code.size()-1; i > pos; i--)
      code[i] = code[i-1];
    code[pos] = bc;

    fixup(pos, code, 1);
  }

  // Remove random bytecode
  void m6(Dist& d, std::vector<BC>& code) {
    if (code.size() <= 1)
      return;
    unsigned pos = d(rng);
    remove(pos, code);
  }

  void remove(unsigned pos, std::vector<BC>& code) {
    for (unsigned i = pos; i < code.size()-1; i++)
      code[i] = code[i+1];
    code.pop_back();
    fixup(pos, code, -1);
  }

  void fixup(unsigned pos, std::vector<BC>& code, int insert) {
    //fixup jump labels
    unsigned p = (pos > 2) ? pos-2 : 0;
    int d = 0;
    while (p > 0) {
      d++;
      p--;
      BC& b = code[p];
      if (b.bc >= BC_t::jmp && b.bc <= BC_t::testnz && b.immediate > d)
        b.immediate += insert;
    }

    p = pos;
    d = 0;
    while (p < code.size()-1) {
      d--;
      p++;
      BC& b = code[p];
      if (b.bc >= BC_t::jmp && b.bc <= BC_t::testnz && b.immediate < d)
        b.immediate -= insert;
    }
  }

  // Remove from random offset
  void m7(Dist& d, std::vector<BC>& code) {
    std::poisson_distribution<int> dp((float)code.size() * m_cutoff_center);
    unsigned pos = dp(rng);
    if (pos >= code.size())
      return;
    code.resize(pos);
    if (code.size() == 0)
      code.push_back(BC_t::nop);
  }

  void m(std::vector<BC>& code) {
    Dist dc(0, code.size() - 1);
    Dist dm(0, 8);

    switch(dm(rng)) {
      case 0:
        m1(dc, code);
        break;
      case 1:
        m2(dc, code);
        break;
      case 2:
        m3(dc, code);
        break;
      case 3:
        m4(dc, code);
        break;
      case 4:
        if (d_coin(rng) == 0)
          m5(dc, code);
        m5(dc, code);
        break;
      case 5:
        m6(dc, code);
        break;
      case 6:
        m7(dc, code);
        break;
      case 7:
        if (d_coin(rng) == 0)
          m6(dc, code);
        m6(dc, code);
        m5(dc, code);
        break;
      case 8:
        m32(dc, code);
        break;
    }
  };
};

#endif
