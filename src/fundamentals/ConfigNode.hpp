#ifndef CONFIGNODE_HPP_T6YBMYT4
#define CONFIGNODE_HPP_T6YBMYT4

#include <yaml-cpp/yaml.h>

#include "psiact/fundamentals/Exceptions.hpp"
#include "psiact/fundamentals/StreamUtils.hpp"

namespace psi_act {

/** Merge two yaml nodes. If arguments are un-mergable, the first is returned
 * as-is.
 * @param a
 * @param b
 * @param shallow Whether to merge shallowly or descend into nested structures.
 * @returns New YAML::Node, merger of the a and b.
 */
YAML::Node yaml_merge(const YAML::Node &a, const YAML::Node &b,
                      bool shallow = false);

class TaggedYAMLNode : public YAML::Node {
  std::string tag;

public:
  TaggedYAMLNode(const YAML::Node &node, const std::string &tag)
      : YAML::Node(node), tag(tag) {}
  const std::string &getTag() const { return tag; }
};

/**
 * @class ConfigNode
 * @brief Wrapper for YAML::Node which throws useful errors. This
 * class basically has a minimal interface similar to the Node. You can cast
 * it, query it by key, and it will raise better errors.
 */
class ConfigNode {
  /**
   * @brief   Small wrapper around a yaml node iterator to make ConfigNode
   * iterable. For reasons, this works only with for(auto e : config_node), so
   * no references and no consts. The range-based for will loop over all keys in
   *          the order of their appearance in the file
   */
  class iterator {
  private:
    YAML::Node node;
    YAML::Node::const_iterator it; ///< wrap an iterator doing most of the work
    std::string tag;               ///< name of config node (for display)
    size_t index;                  ///< current list index (for display)

  public:
    /**
     * @param   it  A Yaml iterator for the underlying node. Should be obtained
     * from the YAML::Node inside the ConfigNode
     * @param   tag Name of the root node to iterate
     * @param   index   Index at which this iterator starts (just for display,
     * no function)
     */
    iterator(const YAML::Node &node, const YAML::Node::const_iterator &it,
             const std::string &tag, size_t index = 0);

    ////////////////////////////////////////////////////////////////////////////////////
    //                     Functions mandated for range-based-for //
    ////////////////////////////////////////////////////////////////////////////////////
    /**
     * @brief   Get a new ConfigNode wrapping the current value behind the
     * iterator
     */
    ConfigNode operator*() const;

    /**
     * @brief   advance the iter
     */
    iterator &operator++();

    /**
     * @brief   Check if equal to other iter
     */
    bool operator!=(const iterator &rhs) const;
  };

  TaggedYAMLNode node;

public:
  /**
   * @brief Init node with empty map node and empty tag.
   *
   * Caveat: This produces an empty map as there is user code which expects it
   * to be an empty map, although that is not necessarily the most intuitive
   * "empty config node" default. User code (specifically the code which uses
   * ConfigNode() as a default value for ConfigNode::get) should probably be
   * adjusted to use ConfigNode::Map instead, making it explicit that a map and
   * not for example an empty string is expected.
   */
  ConfigNode();

  /**
   * @param node    yaml node wrapped by the object
   * @param tag    key belonging to the node
   */
  ConfigNode(const YAML::Node &node, const std::string &tag);

  /**
   * @param content YAML as string.
   * @param tag     key belonging to the node
   */
  ConfigNode(const std::string &content, const std::string &tag);

  /**
   * @brief Construct node as empty map with optional tag.
   * @param tag Optional tag, defaults to empty tag.
   */
  static ConfigNode Map(const std::string &tag = "");

  /**
   * @brief Construct node as empty sequence with optional tag.
   * @param tag Optional tag, defaults to empty tag.
   */
  static ConfigNode Seq(const std::string &tag = "");

  /** @brief    Get the name of this node
   * @note implemented in ConfigNode.cpp
   */
  const std::string &getTag() const;

  /**
   * @brief     Get the underlying YAML node. Only for debugging purposes
   * @note implemented in ConfigNode.cpp
   */
  const YAML::Node &getNode() const;

  iterator begin() const;

  iterator end() const;

  /*****************************************************************************
   * There's two overloads for as() here, one for vectors, one for scalar types.
   * the reason is that yaml will throw TypedBadConversion<T::value_type> for
   * vectors when some item cannot be cast as that type, which would not be
   * caught as a TypedBadConversion<T> in the catch. I found this problem when
   * trying this:
   *       main: camera_sections:
   *         - include: cam1.yml
   *         - include: cam2.yml
   * This gets cast to vector<string>, which fails, but with a YAML exception,
   * not with ours. We try to catch that exception, but it doesn't work when the
   * error is not casting to a vector type, but inside that casting the items to
   * the value type.
   ****************************************************************************/

  // type trait to determine if something is a vector. I haven't found a way to
  // enable_if based on whether T has a value_type member typedef which would
  // catch all containers at once
  template <typename T> struct is_vector {
    static constexpr bool value = false;
  };

  template <template <typename...> class C, typename U> struct is_vector<C<U>> {
    static constexpr bool value = std::is_same<C<U>, std::vector<U>>::value;
  };

  // implementations are identical, save for the catch type
  // Re ^: Can we not catch both with independent catch blocks in the same
  // method? try{}catch{}catch{}? Or use the YAML::BadConversion parent type?
  template <typename T, std::enable_if_t<is_vector<T>::value, bool> = false>
  T as() const {
    try {
      // TODO: probably, casting to YAML::Node is legal and would allow to
      // break out of this wrapper hierarchy
      return this->node.as<T>();
    } catch (const YAML::TypedBadConversion<typename T::value_type> &) {
      // new ex type to pass along a message
      throw exceptions::BadConversion<T>(
          std::string("Could not cast all items of node ") +
          this->node.getTag() + " with content '" + this->dump() +
          "' to desired type.");
    } catch (const YAML::BadConversion &) {
      if (this->node.IsNull()) {
        return T();
      }
      // Catch all, needed for deeply nested config nodes it seems?
      throw exceptions::BadConversion<T>(
          std::string("Could not cast node at path ") + this->node.getTag() +
          " with content '" + this->dump() + "' to desired type.");
    }
  }

  /**
   * @brief Cast node contents to desired type
   * @see YAML::Node::as()
   * @tparam T desired cast type
   * @return Node value as T
   * @throw exceptions::BadConversion<T> if the cast fails.
   * @throw YAML::TypedBadConversion<T> If T is a container type, the
   * yaml library will try to
   */
  template <typename T, std::enable_if_t<!is_vector<T>::value, bool> = true>
  T as() const {
    try {
      // TODO: probably, casting to YAML::Node is legal and would allow to
      // break out of this wrapper hierarchy
      return this->node.as<T>();
    } catch (const YAML::TypedBadConversion<T> &) {
      // new ex type to pass along a message
      throw exceptions::BadConversion<T>(
          std::string("Could not cast node at path ") + this->node.getTag() +
          " with content '" + this->dump() + "' to desired type.");
    }
  }

  /**
   * @brief Non-template get to retrieve a sub-ConfigNode.
   * @param key to get in node.
   * @returns ConfigNode
   * @throw exceptions::BadConversion if cast of actual content is unsuccessful.
   * @throw exception::ConfigError if key not found.
   */
  ConfigNode get(const std::string &key) const;
  // Implementation of this ^ method may not be in the header due to the
  // template specialization below the class definition.

  /**
   * @brief Retrieve a value cast to a type, or return default if not available.
   * @param key Key relative to this node
   * @param default_val Value to use if key is missing (not if value can not
   * be cast)
   * @param log_missing_value If default value is returned, log it. Should be
   * `false` for calls made by the Logger
   * @tparam T desired cast type
   * @returns Value of type T
   * @throw exceptions::BadConversion if cast of actual content is unsuccessful.
   */
  template <typename T>
  T get(const std::string &key, const T &default_val,
        bool log_missing_value = true) const {
    try {
      return this->get<T>(key);
    } catch (const exceptions::ConfigError &) {
      if (log_missing_value) {
        std::stringstream ss;
        ss << "Key '" << key << "' not found in section '"
           << this->node.getTag() << "'. Defaulting to '" << default_val
           << "'.";
        this->log(ss.str());
      }
      return default_val;
    }
  }

  /**
   * @brief Retrieve value and cast to type.
   * @param key to get in node
   * @tparam T desired cast type
   * @returns Value of type T
   * @throw exceptions::BadConversion if cast of actual content is unsuccessful.
   * @throw exception::ConfigError if key not found
   */
  template <typename T> T get(const std::string &key) const {
    return this->get(key).as<T>();
  }

  /**
   * @brief Get value in node.
   * @param key
   * @return A ConfigNode wrapping the key-value
   * @throws exceptions::ConfigError If the key does not exist in node
   */
  ConfigNode operator[](const std::string &key) const;

  /**
   * @brief Set a value on the config node
   *
   * @tparam T  type of param
   * @param key parameter name
   * @param value   parameter value
   *
   * @return self
   */
  template <typename T>
  ConfigNode &set(const std::string &key, const T &value) {
    this->node[key] = value;
    return *this;
  }

  /**
   * @brief Get index in list node.
   * @param ix
   * @return A ConfigNode wrapping the value at index.
   * @throws exceptions::ConfigError If out of bounds or object not indexable
   */
  ConfigNode operator[](unsigned int ix) const;

  /** Whether the node is not empty. */
  explicit operator bool() const { return !this->empty(); }

  /**
   * @brief Number of child elements.
   * Note that scalar nodes have size 0, as they have no children: This does NOT
   * produce the size of characters in the string contained in a scalar node.
   */
  unsigned int size() const;

  /**
   * Whether this node has children or, if a scalar, is not the empty string.
   * Null and undefined are considered empty.
   */
  bool empty() const;

  /** Check whether the current node contains the given key */
  bool contains(const std::string &key) const;

  /**
   * Whether this node is a scalar, e.g. string or number, not a map or
   * sequence.
   */
  bool isScalar() const;

  /**
   * Whether this node is a sequence (aka list) and therefore iterable and its
   * members index-accessible.
   */
  bool isSequence() const;

  /** Merge this node with another node, creating a new node. */
  ConfigNode merge(const ConfigNode &other, bool shallow = false) const;

  /** @brief Dump the ConfigNode's contents as valid yaml. */
  std::string dump() const;

  /** Returns a string of format=key: old_value -> new_value when configs are
   * different */
  std::string firstDifference(const ConfigNode &other) const;

  friend std::ostream &operator<<(std::ostream &stream,
                                  const ConfigNode &node) {
    stream << node.getTag() << ": " << YAML::Dump(node.getNode());
    return stream;
  }

  bool operator==(const ConfigNode &other) const;

  bool operator!=(const ConfigNode &other) const;

private:
  /**
   * Wrapper around LOG_INFO to avoid including Logger in this header, as this
   * header is included in lots and lots of other files, propagating all its
   * includes downwards.
   */
  void log(const std::string &msg) const;
};

// The following had to be moved out of the class due to this error:
// explicit specialization in non-namespace scope 'class autocrane::ConfigNode'
// Note the error does not occur on all systems, but it does in the CI on
// Ubuntu 16.04. Read more at
// https://en.cppreference.com/w/cpp/language/template_specialization

/**
 * @brief Retrieve value and cast to type, specialization of \ref get<T>
 *    specifically to handle ConfigNodes.
 *
 * This allows getting optional ConfigNodes like
 *
 * \code{.cpp}
 * node.get<ConfigNode>("nonexistent.key", ConfigNode()) == ConfigNode()
 * node.get("nonexistent.key", ConfigNode()) == ConfigNode()
 * \endcode
 *
 * @param key to get in node
 * @returns ConfigNode
 * @throw exceptions::BadConversion if cast of actual content is unsuccessful.
 * @throw exception::ConfigError if key not found.
 */
template <>
ConfigNode ConfigNode::get<ConfigNode>(const std::string &key) const;

} // namespace psi_act
#endif /* end of include guard: CONFIGNODE_HPP_T6YBMYT4 */
