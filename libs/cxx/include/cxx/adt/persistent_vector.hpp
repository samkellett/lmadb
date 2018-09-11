#ifndef LMADB_CXX_ADT_PERSISTENT_VECTOR_HPP
#define LMADB_CXX_ADT_PERSISTENT_VECTOR_HPP

#include "cxx/filesystem.hpp"

#include <boost/container/vector.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>

namespace lmadb::cxx {

template <typename T>
class persistent_vector {
  // until required...
  static_assert(std::is_trivially_copyable_v<T>);

  using allocator = boost::interprocess::allocator<
    T,
    boost::interprocess::managed_mapped_file::segment_manager>;
  using vector_type = boost::container::vector<T, allocator>;

  // TODO: make this dynamic.
  constexpr static auto vector_size = 65536;

public:
  using iterator = typename vector_type::iterator;
  using const_iterator = typename vector_type::const_iterator;

  using reference = typename vector_type::reference;
  using const_reference = typename vector_type::const_reference;

  using size_type = typename vector_type::size_type;
  using value_type = typename vector_type::value_type;

  explicit persistent_vector(const cxx::filesystem::path &path);

  auto size() const -> std::size_t;

  auto push_back(value_type value) -> void;

  auto operator[](size_type index) const -> const_reference;

  auto begin() const noexcept -> const_iterator;
  auto end() const noexcept -> const_iterator;

private:
  boost::interprocess::managed_mapped_file file_;
  allocator alloc_;
  vector_type *vector_;
};

template <typename T>
persistent_vector<T>::persistent_vector(const cxx::filesystem::path &path)
: file_{boost::interprocess::open_or_create, path.c_str(), vector_size},
  alloc_{file_.get_segment_manager()},
  vector_{file_.find_or_construct<vector_type>("vector")(alloc_)}
{
}

template <typename T>
auto persistent_vector<T>::size() const -> std::size_t
{
  return vector_->size();
}

template <typename T>
auto persistent_vector<T>::push_back(value_type value)
  -> void
{
  return vector_->push_back(std::move(value));
}

template <typename T>
auto persistent_vector<T>::operator[](const size_type index) const -> const_reference
{
  return (*vector_)[index];
}

template <typename T>
auto persistent_vector<T>::begin() const noexcept -> const_iterator
{
  return vector_->begin();
}

template <typename T>
auto persistent_vector<T>::end() const noexcept -> const_iterator
{
  return vector_->end();
}

} // namespace lmadb::cxx

#endif // LMADB_CXX_ADT_PERSISTENT_VECTOR_HPP
