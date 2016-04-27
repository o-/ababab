#include <iostream>
#include <list>

#include "vm.h"
#include "step.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

P p;

void stat(I& i, std::vector<BC>& code, bool print_res=true) {
   std::cout << "[";
   p.p(code);
   std::cout << "]" << code.size();
   if (!print_res)
     return;

   std::cout << " -> [";
   auto res = i.test(code);
   for (unsigned j = 0; j < i.result.size(); ++j) {
     auto a = i.result[j];
     std::cout << (int)a;
     if (j < i.result.size() - 1)
         std::cout << " ";
   }
   std::cout << "]" << res;
}


std::vector<B> parse(char* p) {
  std::vector<B> exp;

  while (*p) {
    std::string n;
    while (*p && *p != ',' && *p != ' ') {
      n += *p++;
    }
    if (*p) p++;
    exp.push_back(std::stoi(n));
  }
  return exp;
}

std::vector<BC> parseProg(char* p) {
  std::vector<BC> exp;

  while (*p) {
    std::string n;
    while (*p && *p != ',' && *p != ' ') {
      n += *p++;
    }
    if (*p) p++;
    if (!n.compare("nop")) {
      exp.push_back(BC_t::nop);
    } else if (!n.compare("read")) {
      exp.push_back(BC_t::read);
    } else if (!n.compare("write")) {
      exp.push_back(BC_t::write);
    } else if (!n.compare("left")) {
      exp.push_back(BC_t::left);
    } else if (!n.compare("right")) {
      exp.push_back(BC_t::right);
    } else if (!n.compare("swap")) {
      exp.push_back(BC_t::swap);
    } else if (!n.compare("dup")) {
      exp.push_back(BC_t::dup);
    } else if (!n.compare("dec")) {
      exp.push_back(BC_t::dec);
    } else if (!n.compare("inc")) {
      exp.push_back(BC_t::inc);
    } else if (!n.compare(0, 6, "writen")) {
      exp.push_back({BC_t::writen, (B)std::stoi(n.substr(6))});
    } else if (!n.compare(0, 6, "writei")) {
      exp.push_back({BC_t::writei, (B)std::stoi(n.substr(6))});
    } else if (!n.compare("stope")) {
      exp.push_back(BC_t::stope);
    } else if (!n.compare(0, 6, "testne")) {
      exp.push_back({BC_t::testne, (B)std::stoi(n.substr(6))});
    } else if (!n.compare(0, 5, "teste")) {
      exp.push_back({BC_t::teste, (B)std::stoi(n.substr(5))});
    } else if (!n.compare(0, 5, "testz")) {
      exp.push_back({BC_t::testz, (B)std::stoi(n.substr(5))});
    } else if (!n.compare(0, 6, "testnz")) {
      exp.push_back({BC_t::testnz, (B)std::stoi(n.substr(6))});
    } else if (!n.compare(0, 3, "jmp")) {
      exp.push_back({BC_t::jmp, (B)std::stoi(n.substr(3))});
    } else if (!n.compare(0, 4, "push")) {
      exp.push_back({BC_t::push, (B)std::stoi(n.substr(4))});
    } else if (!n.compare("pop")) {
      exp.push_back(BC_t::pop);
    } else if (!n.compare("add")) {
      exp.push_back(BC_t::add);
    } else if (!n.compare("mul")) {
      exp.push_back(BC_t::mul);
    } else if (!n.compare("div")) {
      exp.push_back(BC_t::div);
    } else if (!n.compare("sub")) {
      exp.push_back(BC_t::sub);
    } else if (!n.compare("stop")) {
      exp.push_back(BC_t::stop);
    } else {
      std::cout << "unknown bc " << n << "\n";
      assert(false);
    }
  }
  return exp;
}

bool in_search = false;
bool in_narrow = false;
bool abort_search = false;

void sigint_handler(int s){
  if (in_search && !abort_search) {
    std::cout << "---> abort search\n";
    abort_search = true;
  } else if (in_narrow && !abort_search) {
    std::cout << "---> abort narrowing\n";
    abort_search = true;
  } else {
    std::cout << "exit\n";
    exit(1);
  }
}

void search (std::vector<B> exp) {
  unsigned cost = -1;

  I i(exp);

  std::vector<BC> code({{BC_t::push, 1}, BC_t::write});

  S s;

  std::list<std::vector<BC>> good_code;

  unsigned long max_iter = 300000000L;
  unsigned long max_good = 5L;
  unsigned long print = 1600000L;
  unsigned long accept_cnt = 5000000L;
  unsigned threshold = i.target.size()*3;

  in_search = true;
  for (unsigned long iter = 0; iter < max_iter; ++iter) {
    std::vector<BC> old_code = code;

    s.m(code);
    unsigned new_cost = i.test(code);

    if (new_cost < threshold) {
      std::cout << "@" << iter << " ====>\nfound: ";
      stat(i, code);
      std::cout << "\n";

      good_code.push_back(code);
      code = {BC_t::nop};
    } else if (s.shouldAccept(cost, new_cost, old_code.size(), code.size())) {
      cost = new_cost;
    } else {
      code = old_code;
      s.reset(code);
    }

    if ((iter + 1) % print == 0) {
      std::cout << "@ " << iter << " ~~~~> good: " << good_code.size()
                << "/" << max_good << "\ncur: ";
      stat(i, code);
      std::cout << "\n";
      if (abort_search) break;
    }
    if (good_code.size() >= max_good) break;
  }
  in_search = false;

  if (good_code.empty()) {
    std::cout << "no candidates found\n";
    return;
  }

  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~ Found: ~~~~~~~~~~~~~~~~~~~~~~~~\n";

  std::vector<BC> best_code;
  unsigned best_size = -10;
  unsigned best_diff = -10;
  code = *good_code.begin();

  for (auto c : good_code) {
    unsigned new_cost = i.test(c);
    stat(i, c);
    std::cout << "\n";
    if (new_cost <= best_diff && c.size() < best_size) {
      best_diff = new_cost;
      best_size = c.size();
      best_code = c;
    }
  }

  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~ Opt ~~~~~~~~~~~~~~~~~~~~~~~~\n";

  unsigned long last_update = 0;

  abort_search = false;
  in_narrow = true;
  for (unsigned long iter = 0; iter < max_iter; ++iter) {
    std::vector<BC> old_code = code;

    s.m(code);
    unsigned new_cost = i.test(code);

    if (new_cost < best_diff ||
        (new_cost == best_diff && code.size() < best_size)) {
      best_code = code;
      good_code.push_back(best_code);
      if (good_code.size() == max_good)
        good_code.pop_front();
      best_diff = new_cost;
      best_size = code.size();
      last_update = iter;
    }

    if (s.shouldAccept2(cost, new_cost, old_code.size(), code.size())) {
      cost = new_cost;
    } else {
      code = old_code;
    }

    if ((iter + 1) % print == 0) {
      std::cout << "@ " << iter << " ~~~~>\nbest: ";
      stat(i, best_code);
      std::cout << "\ncur: ";
      stat(i, code, false);
      std::cout << "\n";

      if (best_diff == 0 && iter - last_update > accept_cnt)
        break;
      if (abort_search) break;
    }

    s.reset(code, good_code);
  }
  in_narrow = false;

  code = best_code;
  for (unsigned long iter = 0; iter < code.size()*10; ++iter) {
    std::vector<BC> old_code = code;
    s.remove(iter % (code.size()-1), code);
    unsigned new_cost = i.test(code);
    if (new_cost == 0) {
      best_code = code;
      best_diff = new_cost;
      best_size = code.size();
    } else {
      code = old_code;
    }
  }

  std::cout << "!! ~~~~>\nbest: ";
  stat(i, best_code);
  std::cout << "\n";

  i.test(best_code);
}

int main(int argc, char **argv) {
  assert(argc>2);

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = sigint_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  switch (argv[1][0]) {
    case 's': {
      std::vector<B> exp = parse(argv[2]);
      search(exp);
      break;
    }
    case 'r': {
      assert(argc>3);
      unsigned size = std::atoi(argv[2]);
      std::vector<BC> bc = parseProg(argv[3]);
      std::cout << "Running ";
      P p;
      p.p(bc);
      std::cout << "\n";
      I i(std::vector<B>(size, (B)0));
      i.test(bc, true);
      break;
    }
  }
  return 0;
}
