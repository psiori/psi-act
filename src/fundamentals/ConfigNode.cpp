#include "psiact/fundamentals/ConfigNode.hpp"

#include "psiact/fundamentals/Exceptions.hpp"
#include "psiact/utils/Logger.hpp"

namespace psi_act {

YAML::Node yaml_merge(const YAML::Node &a, const YAML::Node &b, bool shallow) {
  // First is empty, keep second.
  if (a.IsNull()) {
    return b;
  }

  // Second is empty, keep first.
  if (b.IsNull()) {
    return a;
  }

  // Two maps, add keys from second to first if they do not exist.
  if (a.IsMap() && b.IsMap()) {
    YAML::Node c = YAML::Clone(a);
    for (const auto &node : b) {
      if (node.first.IsScalar()) {
        const std::string &key = node.first.Scalar();
        if (!c[key]) {
          c[key] = YAML::Clone(node.second);
        } else if (!shallow) {
          c[key] = yaml_merge(c[key], node.second);
        }
      } else {
        c[node.first] = node.second;
      }
    }
    return c;
  }

  // Two sequences, append values from second to first.
  if (a.IsSequence() && b.IsSequence()) {
    YAML::Node c = YAML::Clone(a);
    for (const YAML::Node &sub : b) {
      c.push_back(YAML::Clone(sub));
    }
    return c;
  }

  // Unable to merge two different types, keep the first object.
  return a;
}

/**
 * @brief Deep compare two yaml nodes.
 * @param a Node to compare to.
 * @param b Node to compare with.
 * @param a_blame Return argument, first found difference to b.
 * @param b_blame Return argument, first found difference to a.
 * @param key Key in which the difference was found, anywhere down the tree.
 * @return Whether a and b are equal.
 */
static bool compare(const YAML::Node &a, const YAML::Node &b,
                    YAML::Node &a_blame, YAML::Node &b_blame,
                    std::string &key) {
  if (a.is(b)) {
    return true;
  }
  if (a.Type() != b.Type() || a.Tag() != b.Tag()) {
    a_blame = a;
    b_blame = b;
    return false;
  }
  switch (a.Type()) {
  case YAML::NodeType::Scalar:
    if (a.Scalar() != b.Scalar()) {
      a_blame = a;
      b_blame = b;
      return false;
    }
    return true;
  case YAML::NodeType::Null:
    return true;
  case YAML::NodeType::Undefined:
    return true;
  case YAML::NodeType::Map:
  case YAML::NodeType::Sequence: {
    if (a.size() != b.size()) {
      a_blame = a;
      b_blame = b;
      return false;
    }
    if (a.IsMap()) {
      for (YAML::const_iterator it = a.begin(); it != a.end(); ++it) {
        key = it->first.as<std::string>();
        if (!compare(it->second, b[key], a_blame, b_blame, key)) {
          return false;
        }
      }
    } else {
      for (YAML::const_iterator it1 = a.begin(), it2 = b.begin();
           it1 != a.end(); ++it1, ++it2) {
        key = std::to_string(std::distance(a.begin(), it1));
        if (!compare(*it1, *it2, a_blame, b_blame, key)) {
          return false;
        }
      }
    }
    return true;
  }
  }
  return true;
}

/**
 * @brief Deep compare two yaml nodes.
 * @param a Node to compare to.
 * @param b Node to compare with.
 * @return Whether a and b are equal.
 */
static bool compare(const YAML::Node &a, const YAML::Node &b) {
  YAML::Node a_blame;
  YAML::Node b_blame;
  std::string key;
  return compare(a, b, a_blame, b_blame, key);
}

bool ConfigNode::contains(const std::string &key) const {
  try {
    this->operator[](key);
    return true;
  } catch (const exceptions::ConfigError &) {
    return false;
  }
}

bool ConfigNode::isScalar() const { return this->node.IsScalar(); }

bool ConfigNode::isSequence() const { return this->node.IsSequence(); }

// The following implementation may not be in the header as there it would
// produce a "multiple definitions" error on ancient ubuntu 16.04..
template <>
ConfigNode
ConfigNode::ConfigNode::get<ConfigNode>(const std::string &key) const {
  return this->operator[](key);
}

// The following implementation may not be in the class declaration in the
// header as it has to come after the very last specialization of the get<T>
// template method.
ConfigNode ConfigNode::get(const std::string &key) const {
  return this->get<ConfigNode>(key);
}

ConfigNode ConfigNode::operator[](const std::string &key) const {
  // When a dotted name is given, recursively call this operator.
  const size_t dot_index = key.find('.');
  if (dot_index != std::string::npos) {
    try {
      // Try full key first, including dots.
      return ConfigNode(this->node[key], this->node.getTag() + "." + key);
    } catch (const exceptions::ConfigError &) {
      // If key with dots does not exist, try it step by step.
      std::string subkey{key.begin(), key.begin() + dot_index};
      std::string restkey{key.begin() + dot_index + 1, key.end()};
      return (*this)[subkey][restkey];
    }
  }

  // No dots in key.
  try {
    return ConfigNode(this->node[key], this->node.getTag() + "." + key);
  } catch (const YAML::InvalidNode &) {
    throw exceptions::ConfigError("Key '" + key + "' does not exist in node '" +
                                  this->node.getTag() + "'");
  }
}

ConfigNode ConfigNode::operator[](unsigned int ix) const {
  try {
    return ConfigNode(this->node[ix],
                      this->node.getTag() + "[" + std::to_string(ix) + "]");
  } catch (const YAML::InvalidNode &) {
    throw exceptions::ConfigError("Index '" + std::to_string(ix) +
                                  "' does not exist in node '" +
                                  this->node.getTag() + "'");
  }
}

const YAML::Node &ConfigNode::getNode() const { return this->node; }

const std::string &ConfigNode::getTag() const { return this->node.getTag(); }

ConfigNode::ConfigNode(const YAML::Node &node, const std::string &tag)
    : node(node, tag) {
  if (!node.IsDefined()) {
    throw exceptions::ConfigError("Missing required node with tag " + tag);
  }
  // should be safe to remove, needed to remove it to allow initialization of
  // an empty ConfigNode
  // if (node.IsNull()) {
  //   throw exceptions::ConfigError("Got null node with tag " + tag);
  // }
}

ConfigNode::ConfigNode() : ConfigNode(YAML::Node(YAML::NodeType::Map), "") {}

ConfigNode::ConfigNode(const std::string &content, const std::string &tag)
    : ConfigNode(YAML::Load(content), tag) {}

ConfigNode ConfigNode::Map(const std::string &tag) {
  return ConfigNode(YAML::Node(YAML::NodeType::Map), tag);
}

ConfigNode ConfigNode::Seq(const std::string &tag) {
  return ConfigNode(YAML::Node(YAML::NodeType::Sequence), tag);
}

ConfigNode::iterator ConfigNode::begin() const {
  if (node.IsNull()) {
    return iterator(node, node.end(), node.getTag(), 0);
  }
  if (!node.IsSequence() && !node.IsMap()) {
    throw std::runtime_error(
        this->node.getTag() +
        " is neither a sequence nor a map; it might even not exist at all.");
  }
  return iterator(node, node.begin(), node.getTag(), 0);
}

ConfigNode::iterator ConfigNode::end() const {
  if (node.IsNull()) {
    return iterator(node, node.end(), node.getTag(), 0);
  }
  if (!node.IsSequence() && !node.IsMap()) {
    throw std::runtime_error(
        this->node.getTag() +
        " is neither a sequence nor a map; it might even not exist at all.");
  }
  return iterator(node, node.end(), node.getTag(), node.size());
}

ConfigNode::iterator::iterator(const YAML::Node &node,
                               const YAML::Node::const_iterator &it,
                               const std::string &tag, size_t index)
    : node(node), it(it), tag(tag), index(index) {}

ConfigNode ConfigNode::iterator::operator*() const {
  if (node.IsMap()) {
    return ConfigNode(it->second, it->first.as<std::string>());
  }
  return ConfigNode(*it, tag + "[" + std::to_string(index) + "]");
}

ConfigNode::iterator &ConfigNode::iterator::operator++() {
  ++index;
  ++it;
  return *this;
}

bool ConfigNode::iterator::operator!=(const iterator &rhs) const {
  return it != rhs.it;
}

unsigned int ConfigNode::size() const { return this->node.size(); }

bool ConfigNode::empty() const {
  return this->size() == 0 &&
         (!this->isScalar() || this->as<std::string>().empty());
}

ConfigNode ConfigNode::merge(const ConfigNode &other, bool shallow) const {
  return ConfigNode(yaml_merge(getNode(), other.getNode(), shallow), getTag());
}

std::string ConfigNode::dump() const { return YAML::Dump(node); }

std::string ConfigNode::firstDifference(const ConfigNode &other) const {
  YAML::Node a_blame;
  YAML::Node b_blame;
  std::string key;
  if (compare(this->getNode(), other.getNode(), a_blame, b_blame, key)) {
    return "";
  }
  if (!key.empty()) {
    return key + ": " + YAML::Dump(a_blame) + " != " + YAML::Dump(b_blame);
  }
  return YAML::Dump(a_blame) + " != " + YAML::Dump(b_blame);
}

bool ConfigNode::operator==(const ConfigNode &other) const {
  return compare(this->getNode(), other.getNode());
}

bool ConfigNode::operator!=(const ConfigNode &other) const {
  return !this->operator==(other);
}

void ConfigNode::log(const std::string &msg) const {
  LOG_INFO(Logger::CONFIGURATION, msg);
}

} // namespace psi_act
