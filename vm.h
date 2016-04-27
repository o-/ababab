#ifndef VM_H
#define VM_H

#include "bc.h"

#include <cassert>

#include <deque>
#include <vector>

#define MAX(a, b) (((uintptr_t)(a)) < ((uintptr_t)(b)) ? (b) : (a))
#define MIN(a, b) (((uintptr_t)(a)) > ((uintptr_t)(b)) ? (b) : (a))
#define ABS(a) ((a) < 0 ? -(a) : (a))

class I {
 public:
  std::vector<B> target;
  std::vector<B> result;
  const unsigned size;
  const unsigned space;
  const unsigned init_steam;

  class Stack {
   public:
    std::vector<B> s;
    size_t pos;

    Stack(unsigned space) : s(space), pos(0) {}

    size_t size() {
      return pos;
    }

    void push(B b) {
      s[pos++] = b;
    }

    bool empty() {
      return pos == 0;
    }

    B top() {
      if (!pos) return 0;
      return s[pos-1];
    }

    B pop() {
      if (!pos) return 0;
      return s[--pos];
    }

    void clear() {
      pos = 0;
    }
  };

  Stack stack;

  I(std::vector<B> target) :
      target(target), result(target.size()), size(target.size()),
      space(size*size*size), init_steam(size*size*size), stack(space) {}

  unsigned test(std::vector<BC>& code, bool trace=false) {
    std::fill(result.begin(), result.end(), 0);

    unsigned steam = init_steam;

    register BC* pc = &code[0];
    B* finger = &result[0];

    B tmp1;
    B tmp2;

    const uintptr_t finger_start = (uintptr_t)finger;
    const uintptr_t finger_end = (uintptr_t)finger + size - 1;

    const uintptr_t bc_start = (uintptr_t)pc;
    const uintptr_t bc_end = (uintptr_t)(pc + code.size() - 1);

    P p;
    if (trace) std::cout << "Trace:\n";

    BC* cur;

    auto printstate = [&](){
      p.print_bc(*cur, true);
      std::cout << ":";

      std::cout << ((finger == &result[0]) ? "." : " ");
      for (unsigned i = 0; i < size; ++i) {
        std::cout << std::setw(2);
        std::cout << std::hex << (int)result[i] << std::dec;
        std::cout << ((finger == &result[i+1]) ? "." : " ");
      }
      std::cout << "\n";
    };

    auto bounds_check = [finger_start, finger_end](B* finger) -> bool {
      return (uintptr_t)finger >= finger_start && (uintptr_t)finger <= finger_end;
    };

    while(true) {
      cur = pc++;

      switch(cur->bc) {
        case BC_t::nop:
          break;

        case BC_t::read:
          if (!bounds_check(finger))
            goto test;
          stack.push(*finger);
          break;

        case BC_t::writen:
          for (auto i = 0; i < cur->immediate; i++) {
            if (!bounds_check(finger))
              goto test;
            *finger = stack.top();
            finger++;
          }
          break;

        case BC_t::write:
          if (!bounds_check(finger))
            goto test;
          *finger = stack.top();
          finger++;
          break;

        case BC_t::writei:
          if (!bounds_check(finger))
            goto test;
          *finger = cur->immediate;
          finger++;
          break;

        case BC_t::right:
          finger++;
          break;

        case BC_t::left:
          if ((uintptr_t)finger > finger_start)
            --finger;
          break;

        case BC_t::stope:
          if (!bounds_check(finger)) {
            goto test;
          }
          break;

        case BC_t::testne:
          if (bounds_check(finger)) {
            pc += cur->immediate - 1;
            if ((uintptr_t)pc < bc_start || (uintptr_t)pc > bc_end)
              goto bad;
          }
          break;

        case BC_t::teste:
          if (!bounds_check(finger)) {
            pc += cur->immediate - 1;
            if ((uintptr_t)pc < bc_start || (uintptr_t)pc > bc_end)
              goto bad;
          }
          break;

        case BC_t::testz:
          if (stack.top() == 0) {
            pc += cur->immediate - 1;
            if ((uintptr_t)pc < bc_start || (uintptr_t)pc > bc_end)
              goto bad;
          }
          break;

        case BC_t::testnz:
          if (stack.top() != 0) {
            pc += cur->immediate - 1;
            if ((uintptr_t)pc < bc_start || (uintptr_t)pc > bc_end)
              goto bad;
          }
          break;

        case BC_t::jmp:
          pc += cur->immediate - 1;
          if ((uintptr_t)pc < bc_start || (uintptr_t)pc > bc_end)
            goto bad;
          break;

        case BC_t::push:
          stack.push(cur->immediate);
          if (stack.size() > space)
            goto bad;
          break;

        case BC_t::pop:
          stack.pop();
          break;

        case BC_t::dec:
          stack.push(stack.pop() - 1);
          break;

        case BC_t::inc:
          stack.push(stack.pop() + 1);
          break;

        case BC_t::add:
          stack.push(stack.pop() + stack.pop());
          break;

        case BC_t::mul:
          stack.push(stack.pop() * stack.pop());
          break;

        case BC_t::swap:
          tmp1 = stack.pop();
          tmp2 = stack.pop();
          stack.push(tmp2);
          stack.push(tmp1);
          break;

        case BC_t::dup:
          stack.push(stack.top());
          break;

        case BC_t::div:
          tmp1 = stack.pop();
          tmp2 = stack.pop();
          if (tmp2 == 0)
            stack.push(0);
          else
            stack.push(tmp1 / tmp2);
          break;

        case BC_t::sub:
          tmp1 = stack.pop();
          tmp2 = stack.pop();
          stack.push(tmp1 - tmp2);
          break;

        case BC_t::stop:
          goto test;

        default:
          assert(false);
      }

      if ((uintptr_t)pc > bc_end) break;
      if (steam-- == 0) break;
      if (trace) printstate();
    }

test:
    {

    if (trace) printstate();
    int miss = 0;

    for (unsigned i = 0; i < size; ++i) {
      if (target[i] != result[i])
        miss += 5 + ABS(target[i] - result[i]);
    }

    stack.clear();
    return miss;
    }

bad:
    stack.clear();
    if (trace) printstate();
    return -1;
  }
};

#endif
