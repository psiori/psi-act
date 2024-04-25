#pragma once

#include <boost/circular_buffer.hpp>
#include <iostream>
#include <optional>
#include <unordered_map>

// Weirdness: Unless this definition is done inside the autocrane namespace, or
// in std namespace, it is not found during compilation of explicit.hpp #watman
namespace std {

template <typename TK, typename TV> static std::ostream&
operator<<(std::ostream& stream, const std::pair<TK, TV>& pr) {
  return stream << "(" << pr.first << ", " << pr.second << ")";
}

// Print any enum
template <typename T> std::ostream& operator<<(
    typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream,
    const T& e) {
  return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

template <typename TK, typename TV> static std::ostream&
operator<<(std::ostream& stream, const std::unordered_map<TK, TV>& mp) {
  if (mp.empty()) {
    stream << "{empty unordered map}";
    return stream;
  }
  size_t ix = 0;
  stream << "{";
  for (const auto& entry : mp) {
    stream << entry.first << ": " << entry.second;
    if (++ix < mp.size()) {
      stream << ", ";
    }
  }
  stream << "}";
  return stream;
}

template <typename TK, typename TV> static std::ostream&
operator<<(std::ostream& stream, const std::map<TK, TV>& mp) {
  if (mp.empty()) {
    stream << "{empty map}";
    return stream;
  }
  size_t ix = 0;
  stream << "{";
  for (const auto& entry : mp) {
    stream << entry.first << ": " << entry.second;
    if (++ix < mp.size()) {
      stream << ", ";
    }
  }
  stream << "}";
  return stream;
}

template <typename T> static std::ostream&
operator<<(std::ostream& stream, const std::vector<T>& vec) {
  if (vec.empty()) {
    stream << "{empty vec}";
    return stream;
  }
  for (size_t i = 0; i < vec.size() - 1; ++i) {
    stream << vec[i] << ", ";
  }
  stream << vec[vec.size() - 1];
  return stream;
}

template <typename T> static std::ostream&
operator<<(std::ostream& stream, const std::set<T>& st) {
  stream << "{";
  typename std::set<T>::iterator it;
  for (it = st.begin(); it != st.end(); ++it) {
    stream << (*it);
    if (std::next(it) != st.end()) {
      stream << ", ";
    }
  }
  stream << "}";
  return stream;
}

template <typename T> static std::ostream&
operator<<(std::ostream& stream, const boost::circular_buffer<T>& vec) {
  if (vec.empty()) {
    stream << "{empty vec}";
    return stream;
  }
  for (size_t i = 0; i < vec.size() - 1; ++i) {
    stream << vec[i] << ", ";
  }
  stream << vec[vec.size() - 1];
  return stream;
}

template <class CharType, class CharTrait>
inline std::basic_ostream<CharType, CharTrait>&
operator<<(std::basic_ostream<CharType, CharTrait>& out, nullopt_t) {
  if (out.good()) {
    out << "--";
  }

  return out;
}

template <class CharType, class CharTrait, class T>
inline std::basic_ostream<CharType, CharTrait>&
operator<<(basic_ostream<CharType, CharTrait>& out, const optional<T>& v) {
  if (out.good()) {
    if (!v) {
      out << "--";
    } else {
      out << ' ' << *v;
    }
  }

  return out;
}

}  // namespace std
