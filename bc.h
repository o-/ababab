#ifndef BC_H
#define BC_H

#include <iomanip>
#include <cassert>
#include <vector>

typedef int8_t B;

enum class BC_t : uint8_t {
  nop,
  stop,
  read,
  left,
  pop,
  right,

  jmp,
  testz,
  teste,
  testne,
  testnz,

  push,
  add,
  mul,
  div,
  sub,
  dec,
  inc,
  dup,
  swap,
  stope,

  write,
  writen,
  writei,

  num_of,
};

struct BC {
  BC_t bc;
  B immediate;
  BC(BC_t bc, B immediate) : bc(bc), immediate(immediate) {}
  BC(BC_t bc) : bc(bc), immediate(0) {}
  BC() : bc(BC_t::nop), immediate(0) {}
};

class P {
 public:
  void p(std::vector<BC>& code) {
    for (unsigned i = 0; i < code.size(); i++) {
      auto pc = code[i];
      print_bc(pc);
      if (i < code.size()-1)
        std::cout << " ";
    }
  }

  static void print_bc(BC pc, bool aligned=false) {
    if (aligned)
      std::cout << std::setw(9);

    switch(pc.bc) {
      case BC_t::nop:
        std::cout << "nop";
        break;

      case BC_t::read:
        std::cout << "read";
        break;

      case BC_t::write:
        std::cout << "write";
        break;

      case BC_t::writen:
        if (aligned)
          std::cout << std::setw(6);
        std::cout << "writen";
        if (aligned)
          std::cout << std::setw(3);
        std::cout << (int)pc.immediate;
        break;

      case BC_t::writei:
        if (aligned)
          std::cout << std::setw(6);
        std::cout << "writei";
        if (aligned)
          std::cout << std::setw(3);
        std::cout << (int)pc.immediate;
        break;

      case BC_t::left:
        std::cout << "left";
        break;

      case BC_t::right:
        std::cout << "right";
        break;

      case BC_t::swap:
        std::cout << "swap";
        break;

      case BC_t::dup:
        std::cout << "dup";
        break;

      case BC_t::dec:
        std::cout << "dec";
        break;

      case BC_t::inc:
        std::cout << "inc";
        break;

      case BC_t::stope:
        std::cout << "stope";
        break;

      case BC_t::testne:
        if (aligned)
          std::cout << std::setw(6);
        std::cout << "testne";
        if (aligned)
          std::cout << std::setw(3);
        std::cout << (int)pc.immediate;
        break;

      case BC_t::teste:
        if (aligned)
          std::cout << std::setw(6);
        std::cout << "teste";
        if (aligned)
          std::cout << std::setw(3);
        std::cout << (int)pc.immediate;
        break;

      case BC_t::testz:
        if (aligned)
          std::cout << std::setw(6);
        std::cout << "testz";
        if (aligned)
          std::cout << std::setw(3);
        std::cout << (int)pc.immediate;
        break;

      case BC_t::testnz:
        if (aligned)
          std::cout << std::setw(6);
        std::cout << "testnz";
        if (aligned)
          std::cout << std::setw(3);
        std::cout << (int)pc.immediate;
        break;

      case BC_t::push:
        if (aligned)
          std::cout << std::setw(6);
        std::cout << "push";
        if (aligned)
          std::cout << std::setw(3);
        std::cout << (int)pc.immediate;
        break;

      case BC_t::pop:
        std::cout << "pop";
        break;

      case BC_t::add:
        std::cout << "add";
        break;

      case BC_t::mul:
        std::cout << "mul";
        break;

      case BC_t::div:
        std::cout << "div";
        break;

      case BC_t::sub:
        std::cout << "sub";
        break;

      case BC_t::stop:
        std::cout << ".";
        return;

      case BC_t::jmp:
        if (aligned)
          std::cout << std::setw(6);
        std::cout << "jmp";
        if (aligned)
          std::cout << std::setw(3);
        std::cout << (int)pc.immediate;
        return;

      default:
        assert(false);
    }
  }
};

#endif
