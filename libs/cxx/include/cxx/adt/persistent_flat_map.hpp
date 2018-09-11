#ifndef LMADB_CXX_ADT_PERSISTENT_FLAT_MAP_HPP
#define LMADB_CXX_ADT_PERSISTENT_FLAT_MAP_HPP

#include "cxx/filesystem.hpp"

#include <boost/container/flat_map.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>

namespace lmadb::cxx {

template <typename Key, typename Value>
class persistent_flat_map {
  // until required...
  static_assert(std::is_trivially_copyable_v<Key>);
  static_assert(std::is_trivially_copyable_v<Value>);

  using allocator = boost::interprocess::allocator<
    std::pair<Key, Value>,
    boost::interprocess::managed_mapped_file::segment_manager>;
  using map_type = boost::container::flat_map<Key, Value, std::less<Key>, allocator>;

  // TODO: make this dynamic.
  constexpr static auto map_size = 65536;

public:
  using iterator = typename map_type::iterator;
  using const_iterator = typename map_type::const_iterator;

  using size_type = typename map_type::size_type;
  using key_type = typename map_type::key_type;
  using value_type = typename map_type::value_type;

  explicit persistent_flat_map(const cxx::filesystem::path &path);

  auto size() const -> std::size_t;

  auto insert(value_type value) -> std::pair<iterator, bool>;
  auto erase(const key_type &key) -> size_type;

  auto begin() const noexcept -> const_iterator;
  auto end() const noexcept -> const_iterator;

  auto find(const key_type &) const -> const_iterator;
  auto nth(size_type n) const -> const_iterator;

private:
  boost::interprocess::managed_mapped_file file_;
  allocator alloc_;
  map_type *map_;
};

template <typename Key, typename Value>
persistent_flat_map<Key, Value>::persistent_flat_map(const cxx::filesystem::path &path)
: file_{boost::interprocess::open_or_create, path.c_str(), map_size},
  alloc_{file_.get_segment_manager()},
  map_{file_.find_or_construct<map_type>("map")(alloc_)}
{
}

template <typename Key, typename Value>
auto persistent_flat_map<Key, Value>::size() const -> std::size_t
{
  return map_->size();
}

template <typename Key, typename Value>
auto persistent_flat_map<Key, Value>::insert(value_type value)
  -> std::pair<iterator, bool>
{
  return map_->insert(std::move(value));
}

template <typename Key, typename Value>
auto persistent_flat_map<Key, Value>::erase(const key_type &key) -> size_type
{
  return map_->erase(key);
}

template <typename Key, typename Value>
auto persistent_flat_map<Key, Value>::begin() const noexcept -> const_iterator
{
  return map_->begin();
}

template <typename Key, typename Value>
auto persistent_flat_map<Key, Value>::end() const noexcept -> const_iterator
{
  return map_->end();
}

template <typename Key, typename Value>
auto persistent_flat_map<Key, Value>::find(const key_type &key) const -> const_iterator
{
  return map_->find(key);
}

template <typename Key, typename Value>
auto persistent_flat_map<Key, Value>::nth(const size_type n) const -> const_iterator
{
  return map_->nth(n);
}

} // namespace lmadb::cxx

#endif // LMADB_CXX_ADT_PERSISTENT_FLAT_MAP_HPP
