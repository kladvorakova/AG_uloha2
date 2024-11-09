#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>

#endif

template < typename T >
struct Queue {

    struct Ref {
        typename std::list<T>::iterator it;
        explicit Ref(typename std::list<T>::iterator iter) : it(iter) {}
    };

    bool empty() const {
        return elements.empty();
    }

    size_t size() const {
        return elements.size();
    }

    Ref push_last(T x) {
        elements.push_back(x);
        auto it = std::prev(elements.end());
        return Ref(it);
    }

    T pop_first() {
        if (elements.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        T value = elements.front();
        elements.pop_front();
        return value;
    }

    size_t position(const Ref& ref) const {
        return std::distance(elements.begin(), static_cast<typename std::list<T>::const_iterator>(ref.it));
    }

    void jump_ahead(const Ref& ref, size_t positions) {
        auto it = ref.it;
        size_t current_pos = position(ref);

        if (positions >= current_pos) {
            elements.splice(elements.begin(), elements, it);
        } else {
            auto target = std::next(elements.begin(), current_pos - positions);
            elements.splice(target, elements, it);
        }
    }

private:
    std::list<T> elements;
};

#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
  std::string ret = "\"";
  for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
  return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)
 
////////////////// End of dark magic ////////////////////////


void test1(int& ok, int& fail) {
  Queue<int> Q;
  CHECK(Q.empty(), true);
  CHECK(Q.size(), 0);

  constexpr int RUN = 10, TOT = 105;

  for (int i = 0; i < TOT; i++) {
    Q.push_last(i % RUN);
    CHECK(Q.empty(), false);
    CHECK(Q.size(), i + 1);
  }

  for (int i = 0; i < TOT; i++) {
    CHECK(Q.pop_first(), i % RUN);
    CHECK(Q.size(), TOT - 1 - i);
  }

  CHECK(Q.empty(), true);
}

void test2(int& ok, int& fail) {
  Queue<int> Q;
  CHECK(Q.empty(), true);
  CHECK(Q.size(), 0);
  std::vector<decltype(Q.push_last(0))> refs;

  constexpr int RUN = 10, TOT = 105;

  for (int i = 0; i < TOT; i++) {
    refs.push_back(Q.push_last(i % RUN));
    CHECK(Q.size(), i + 1);
  }
  
  for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), i);

  Q.jump_ahead(refs[0], 15);
  Q.jump_ahead(refs[3], 0);
  
  CHECK(Q.size(), TOT);
  for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), i);

  Q.jump_ahead(refs[8], 100);
  Q.jump_ahead(refs[9], 100);
  Q.jump_ahead(refs[7], 1);

  static_assert(RUN == 10 && TOT >= 30);
  for (int i : { 9, 8, 0, 1, 2, 3, 4, 5, 7, 6 })
    CHECK(Q.pop_first(), i);

  for (int i = 0; i < TOT*2 / 3; i++) {
    CHECK(Q.pop_first(), i % RUN);
    CHECK(Q.size(), TOT - 11 - i);
  }

  CHECK(Q.empty(), false);
}

template < int TOT >
void test_speed(int& ok, int& fail) {
  Queue<int> Q;
  CHECK(Q.empty(), true);
  CHECK(Q.size(), 0);
  std::vector<decltype(Q.push_last(0))> refs;

  for (int i = 0; i < TOT; i++) {
    refs.push_back(Q.push_last(i));
    CHECK(Q.size(), i + 1);
  }
  
  for (int i = 0; i < TOT; i++) {
    CHECK(Q.position(refs[i]), i);
    Q.jump_ahead(refs[i], i);
  }

  for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), TOT - 1 - i);

  for (int i = 0; i < TOT; i++) {
    CHECK(Q.pop_first(), TOT - 1 - i);
    CHECK(Q.size(), TOT - 1 - i);
  }

  CHECK(Q.empty(), true);
}

int main() {
  int ok = 0, fail = 0;
  if (!fail) test1(ok, fail);
  if (!fail) test2(ok, fail);
  if (!fail) test_speed<1'000>(ok, fail);
 
  if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
  else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif


