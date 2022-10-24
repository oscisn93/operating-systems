//
//  vector_.h
//  bankers
//
//  Created by William McCarthy on 21 Mar 2022.
//

#ifndef __vector_h__
#define __vector_h__

#include <iostream>
#include <iomanip>
#include <sstream>

#include <vector>


template <typename T>
class vector_ {
public:
  vector_() : v(std::vector<T>()) { }
  vector_(size_t size, const T& k) : vector_() {
    for (size_t i = 0; i < size; ++i) { v.push_back(k); }
  }
  vector_(const std::vector<T>& other) : v(other) { }
  vector_(const std::string& name, const std::initializer_list<T>& li) : vector_<T>() {
    for (const T& el : li) {
      v.push_back(el);
    }
  }
  vector_(const std::initializer_list<T>& li) : vector_<T>() {
    for (const T& el : li) {
      v.push_back(el);
    }
  }
//------------------------------------------------------------------
  bool empty()  const { return v.empty(); }
  size_t size() const { return v.size(); }
//------------------------------------------------------------------
  void add(T& val) { v.push_back(val); }
  void push_back(const T& val) { v.push_back(val); }
  void clear() {
    while (!empty()) { v.pop_back(); }
  }
//------------------------------------------------------------------
  T  operator[](size_t i) const { return v[i]; }
  T& operator[](size_t i)       { return v[i]; }
  
  typename std::vector<T>::const_iterator begin() const { return v.begin(); }
  typename std::vector<T>::const_iterator end()   const { return v.end();   }
//------------------------------------------------------------------
  vector_& operator+=(const vector_<T>& other) {    // alloc += req;
    for (int i = 0; i < other.v.size(); ++i) {
      v[i] += other.v[i];
    }
    return *this;
  }
  vector_& operator-=(const vector_<T>& other) {    // avail -= req;
    for (int i = 0; i < other.v.size(); ++i) {
      v[i] -= other.v[i];
    }
    return *this;
  }
//------------------------------------------------------------------
  vector_ operator+(const vector_<T>& other) const {   // max + need == alloc;
    vector_<T> temp(v);
    temp += other;
    return temp;
  }
  vector_ operator-(const vector_<T>& other) const {   // need = max - alloc;
    vector_<T> temp(v);
    temp -= other;
    return temp;
  }
  vector_ operator*(const T& k) const {
    vector_<T> temp(v);
    for (int i = 0; i < v.size(); ++i) { temp.v[i] *= k; }            // returns v * k
    return temp;
  }
  friend vector_ operator*(const T& k, const vector_<T>& v) {
    vector_<T> temp(v);
    for (int i = 0; i < v.size(); ++i) { temp.v[i] *= k; }            // returns v * k
    return temp;
  }
//------------------------------------------------------------------
  bool operator< (const vector_<T>& other) const { return v < other.v;  }     // alloc < max;
  bool operator<=(const vector_<T>& other) const { return v <= other.v; }     // alloc + req <= max;   // should occur
  bool operator> (const vector_<T>& other) const { return v > other.v;  }     // alloc + req > max;    // should not occur
  bool operator>=(const vector_<T>& other) const { return v >= other.v; }

  bool operator==(const vector_<T>& other) const { return v == other.v; }     // alloc == max;
  bool operator!=(const vector_<T>& other)       { return !operator==(other); }
  bool is_zero() const {
    for (size_t i = 0; i < v.size(); ++i) {
      if (v[i] != 0) { return false; }
    }
    return true;
  }
  bool is_k(const T& constant) {
    for (size_t i = 0; i < v.size(); ++i) {
      if (v[i] != constant) { return false; }
    }
    return true;
  }

  std::string as_string() const { 
    std::stringstream ss;
    if (empty()) { ss << "vector_<T> is empty\n";  return ss.str(); }
    size_t i = 0;
    for (const T& el : v) { 
        ss << std::setw(2) << el;
        if (i++ < v.size() - 1) { ss << " "; } 
    }
    return ss.str();
  }
//------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const vector_& vect) {
    return os << vect.as_string();
    // if (vect.empty()) { return os << "vector_<T> is empty]\n"; }
    // size_t i = 0;
    // for (const T& el : vect.v) {
    //   os << std::setw(2) << el;
    //   if (i++ < vect.v.size() - 1) { os << " "; }
    // }
    // return os;
  }
//------------------------------------------------------------------
  static void run_tests() {
    vector_ v = { 1, 2, 3, 4, 5 };
    vector_ v2 = { 1, 0, 2, 0, 3 };
    std::cout << "v  is: " << v << "\n";
    std::cout << "v2 is: " << v2 << "\n";

    v += v2;
    std::cout << "v += v2 is: " << v << "\n";
    std::cout << "v +  v2 is: " << v + v2 << "\n";
    std::cout << "v -  v2 is: " << v - v2 << "\n";
    std::cout << "v -  v  is: " << v - v << "\n";

    std::cout << "3 * v is: " << 3 * v << "\n";
    std::cout << "v * 2 is: " << v * 2 << "\n";
  }

private:
  std::vector<T> v;
};  


#endif /* __vector_h__ */
