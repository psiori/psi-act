// ///////////////////////////////////////////////////////////////////////////
//
//   psi_act
//
//   PSIORI GmbH -- all rights reserved.
//
// ///////////////////////////////////////////////////////////////////////////

/**
 * @file YAMLReader.cpp
 * @brief Parse yaml configs including a form of "subfile include" to allow
 * for file-separation, which is not part of the YAML standard.
 */

#include "YAMLReader.hpp"

#include <filesystem>
#include <iostream>

#include "psiact/fundamentals/Exceptions.hpp"
// #include "Fundamental/TimeUtils.hpp"

namespace sfs = std::filesystem;
using namespace psi_act::exceptions;

/**
 * @brief Helper for recursive yaml loading
 * @param begin Node to start with
 * @param root_path Path relative to which any relative paths inside the yaml
 *                  node should be interpreted
 */
static void expandNode(YAML::Node &begin, const sfs::path &root_path = "");

/**
 * @brief Merge two nodes, key by key deep, src taking precedence over dst.
 * @param dst Destination node to merge into.
 * @param src Source node to merge into destination node.
 */
static void mergeNodesOverwrite(YAML::Node &dst, const YAML::Node &src) {
  if (!src.IsMap()) {
    // src is not a map, cannot merge, overwrite if there is a value in src
    dst = src.IsNull() ? dst : src;
  } else if (!dst.IsMap()) {
    // dst is not a map, cannot merge, overwrite
    dst = src;
  } else if (dst.size() == 0) {
    // dst is empty, overwrite
    dst = src;
  } else {
    // merge key by key, deep
    for (auto node : src) {
      auto dst_node = dst[node.first.as<std::string>()];
      mergeNodesOverwrite(dst_node, node.second);
    }
  }
}

static void expandNode(YAML::Node &begin, const sfs::path &root_path) {
  using namespace YAML;
  if (begin.IsNull()) {
    // it's valid YAML to have a key without value. The value will be a Null
    // node which we ignore
    return;
  } else if (begin.IsScalar()) {
    // Single value, no key->value map, simply abort, nothing to expand.
    return;
  } else if (begin.IsSequence()) {
    // complex values list, iterate over all items ...
    // new sequence with expanded items
    Node new_sequence;
    for (size_t idx = 0; idx < begin.size(); ++idx) {
      Node item = begin[idx];
      // If item is a map and has only a single subitem with key "<<", expand
      // that subitem's contents into the list itself.
      if (item.IsMap() && item.size() == 1 &&
          item.begin()->first.as<std::string>() == "<<") {
        if (!item.begin()->second.IsSequence()) {
          throw ConfigError(
              "Can only expand lists into lists (aka '- <<:') in " +
              YAML::Dump(begin));
        }
        /* It used to be
         *
         *   for (Node subitem : item.begin()->second)
         *
         * which triggered this problem
         * https://github.com/jbeder/yaml-cpp/issues/977 */
        for (auto iter = item.begin()->second.begin();
             iter != item.begin()->second.end(); ++iter) {
          Node subitem = *iter;
          // Expand each subitem and add flat.
          expandNode(subitem, root_path);
          new_sequence.push_back(subitem);
        }
      }
      // If item is a map and has only a single subitem with key "include",
      // expand that subitem's contents into the list itself.
      else if (item.IsMap() && item.size() == 1 &&
               item.begin()->first.as<std::string>() == "include") {
        // Load included file contents, applying the same logic as for
        // 'include'. Then do the same for each member, putting each member into
        // the top level hierarchy after expansion.
        expandNode(item, root_path);
        if (item.IsSequence()) {
          for (Node subitem : item) {
            expandNode(subitem, root_path);
            new_sequence.push_back(subitem);
          }
        } else {
          expandNode(item, root_path);
          new_sequence.push_back(item);
        }
      } else {
        expandNode(item, root_path);
        new_sequence.push_back(item);
      }
    }
    // replace original sequence with new sequence
    begin = new_sequence;
  } else if (begin.IsMap()) {
    // complex keys->values map, iterate over all key-value pairs ...
    Node clone; // maybe can be done in place, not sure if iterators get
                // invalidated when mutating iteratee
    for (YAML::iterator it = begin.begin(); it != begin.end(); ++it) {
      // grab key and value
      const std::string key = it->first.as<std::string>();
      // We currently do not enforce "no dotted keys" as there are some maps
      // with decimals-as-strings as keys. Instead, ConfigNode[] tries accessing
      // the full path (including dots) first, then the sub-paths step by
      // step.
      // if (key.find(".") != std::string::npos) {
      //   throw psi_act::exceptions::ConfigError(
      //       "YAML config may not contain dots (\".\") in map keys (key: "
      //       + key + ").");
      // }
      auto value = it->second;
      // if key is magic word ..
      if (key == "include") {
        std::vector<std::string> include_path_names;
        if (value.IsSequence()) {
          for (size_t idx = 0; idx < value.size(); ++idx) {
            Node item = value[idx];
            include_path_names.emplace_back(item.as<std::string>());
          }
        } else {
          include_path_names.emplace_back(value.as<std::string>());
        }
        for (const std::string &path_name : include_path_names) {
          // ... load the file. if path relative, prepend given root path
          sfs::path include_path = sfs::path(path_name);
          if (!include_path.is_absolute()) {
            include_path = sfs::path(root_path) / include_path;
          }
          Node expanded;
          try {
            expanded = YAML::LoadFile(include_path.string());
            // recursively expand the loaded file
            expandNode(expanded, include_path.parent_path());
          } catch (const YAML::ParserException &ex) {
            throw psi_act::exceptions::BadFile(
                "Could not parse '" + include_path.string() +
                "' (paths are relative to file). Error: " + ex.what());
          } catch (const YAML::BadFile &ex) {
            throw psi_act::exceptions::BadFile(
                "Could not read '" + include_path.string() +
                "' (paths are relative to file). Error: " + ex.what());
          }

          // now insert result into original
          if (expanded.IsScalar()) {
            // if nothing further to expand, simply replace include: node with
            // this value
            clone = expanded;
          } else if (expanded.IsSequence()) {
            for (Node subitem : expanded) {
              // Expand each subitem and add flat.
              expandNode(subitem, include_path.parent_path());
              clone.push_back(subitem);
            }
          } else {
            // otherwise, bring all key->value pairs from the included file into
            // the original
            for (YAML::iterator it2 = expanded.begin(); it2 != expanded.end();
                 ++it2) {
              clone[it2->first.as<std::string>()] = it2->second;
            }
          }
        }
      } else if (key == "_merge_overwrite") {
        if (value.IsMap()) {
          expandNode(value, root_path);
          mergeNodesOverwrite(clone, value);
        } else {
          throw ConfigError(
              std::string("Can only merge a mapping, not this:. ") +
              YAML::Dump(value));
        }
      } else {
        // key is not magic itself, but the value may be arbitrarily nested
        // and contain further includes
        expandNode(value, root_path);
        // add the key and possibly expanded values to the return node
        clone[key] = value;
      }
    }
    // we are done, so replace original with new
    begin = clone;
  } else {
    throw std::invalid_argument(
        std::string("Can't handle node with content '") + YAML::Dump(begin) +
        "' of type " + std::to_string(begin.Type()));
  }
}

namespace psi_act {

YAML::Node recursiveLoad(const sfs::path &file) {
  YAML::Node root = YAML::LoadFile(file.string());
  expandNode(root, sfs::absolute(file).parent_path());
  return root;
}

/** The singleton config reader */
static YAMLReader *yaml_config = nullptr;

YAMLReader::YAMLReader(const YAML::Node &config, const std::string &filepath)
    : ConfigNode(config, "root"),
      main_section(
          ConfigNode(YAML::Clone(this->get("main").getNode()), "root.main")),
      filepath{filepath} {}

YAMLReader::YAMLReader(const std::string &filepath)
    : YAMLReader(recursiveLoad(filepath), filepath) {}

YAMLReader::~YAMLReader() {}

void YAMLReader::setMainSection(const ConfigNode &section) {
  this->main_section =
      ConfigNode(YAML::Clone(section.getNode()), section.getTag());
}

const ConfigNode &YAMLReader::main() const { return this->main_section; }

bool YAMLReader::isSensorNode() const { return this->sensornode; }

void YAMLReader::setSensorNode(bool sensornode) { this->sensornode = true; }

void YAMLReader::setMastercontrol(bool mastercontrol) {
  this->mastercontrol = true;
}

void YAMLReader::initYAMLReader(const std::string &filepath,
                                const bool reinitialize) {
  if (yaml_config != nullptr && !reinitialize) {
    throw exceptions::ConfigError(
        "YAMLReader already initialized, init call had no effect.");
  }
  yaml_config = new YAMLReader(filepath);
}

void YAMLReader::initYAMLReaderFromDump(const std::string &dump,
                                        const bool reinitialize) {
  if (yaml_config != nullptr) {
    if (reinitialize) {
      delete yaml_config;
    } else {
      throw exceptions::ConfigError(
          "YAMLReader already initialized, init call had no effect.");
    }
  }
  yaml_config = new YAMLReader(YAML::Load(dump));
}

YAMLReader &YAMLReader::getYAMLReader() {
  if (yaml_config == nullptr) {
    throw exceptions::ConfigError(
        "YAMLReader::initYAMLReader must be called before getting the "
        "singleton.");
  }
  return *yaml_config;
}

} // namespace psi_act

// namespace YAML {

// void operator>>(const YAML::Node& node, psi_act::TimePoint& t) {
//   t = psi_act::utils::parse_timepoint_iso8601(
//       node.as<std::string>(), /* sanitized */ false);
// }

// void operator>>(const YAML::Node& node, psi_act::Duration& d) {
//   d = psi_act::utils::durationFromHHMMSS(node.as<std::string>());
// }

// }  // namespace YAML
