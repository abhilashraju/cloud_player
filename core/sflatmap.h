#ifndef SFLATMAP_H
#define SFLATMAP_H
#include<vector>
#include<utility>
#include<algorithm>
#include<assert.h>
template <typename Key, typename Value>
struct SFlatMap
{
  auto find(const Key &s) {
    return std::find_if(std::begin(data), std::end(data), [&s](const auto &d) { return d.first == s; });
  }

  auto find(const Key &s) const {
    return std::find_if(std::begin(data), std::end(data), [&s](const auto &d) { return d.first == s; });
  }

  auto size() const {
    return data.size();
  }

  auto begin() const {
    return data.begin();
  }

  auto end() const {
    return data.end();
  }


  auto begin() {
    return data.begin();
  }

  auto end() {
    return data.end();
  }


  Value &operator[](const Key &s) {
    const auto itr = find(s);
    if (itr != data.end()) {
      return itr->second;
    } else {
      data.emplace_back(s, Value());
      return data.back().second;
    }
  }

  Value &at(const Key &s) {
    const auto itr = find(s);
    if (itr != data.end()) {
      return itr->second;
    } else {
      assert(!"Unknown key: ");
    }
  }

  const Value &at(const Key &s) const {
    const auto itr = find(s);
    if (itr != data.end()) {
      return itr->second;
    } else {
      assert(!"Unknown key: ");
    }
  }

  size_t count(const Key &s) const {
    return (find(s) != data.end())?1:0;
  }
  template<typename Predicate>
  void erase_all_if(Predicate pred)
  {
      auto iter = std::find_if(begin(),end(),pred);
      while (iter != end()) {
          data.erase(iter);
          iter =  std::find_if(begin(),end(),pred);
      }
  }
  std::vector<std::pair<Key, Value>> data;

  using iterator = typename decltype(data)::iterator;
  using const_iterator =typename decltype(data)::const_iterator;


};

#endif // SFLATMAP_H
