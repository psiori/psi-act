// ///////////////////////////////////////////////////////////////////////////
//
//   AUTOCRANE
//
//   PSIORI GmbH -- all rights reserved.
//
// ///////////////////////////////////////////////////////////////////////////

#ifndef YAMLREADER_HPP_NHYXU2C0
#define YAMLREADER_HPP_NHYXU2C0

/**
 * @file YAMLReader.hpp
 * @brief Parse yaml configs including a form of "subfile include" to allow
 * for file-separation, which is not part of the YAML standard.
 */

#include <filesystem>
#include <psiact/TimePoint.h>
#include <string>
#include <yaml-cpp/yaml.h>

#include "psiact/fundamentals/ConfigNode.hpp"

namespace psi_act {

/**
 * @brief Load a yaml file and traverse it in search for `include:` keys.
 * Their referenced files will be loaded and inserted, replacing the include
 * node.
 * @param file Path to file
 * @return YAML node
 */
YAML::Node recursiveLoad(const std::filesystem::path &file);

class YAMLReader : public ConfigNode {
private:
  /**
   * @brief The main section.
   * By default toplevel section `main`. May be altered by setting it.
   */
  ConfigNode main_section;

  /** Whether currently running as a sensornode. */
  bool sensornode = false;

  /** Whether currently running as a mastercontrol. */
  bool mastercontrol = false;

public:
  /**
   * @brief Path to config file this YAMLReader was created from.
   * Might be empty-string for YAMLReaders initialized from a config directly
   * (e.g. on sensornodes).
   */
  const std::string filepath;

  /** Construct from parsed yaml.
   * @param config  A parsed yaml object.
   */
  explicit YAMLReader(const YAML::Node &config,
                      const std::string &filepath = "");

  /**
   * @brief Construct from a filepath, recursively loading it.
   * @param filepath  Path to a yaml file.
   */
  explicit YAMLReader(const std::string &filepath);

  ~YAMLReader();

  /**
   * @return The singleton YAMLReader instance.
   */
  static YAMLReader &getYAMLReader();

  /**
   * @brief Initialise the YAMLReader. This function must be called before
   * using the global yaml reader.
   * @param filepath  Path to the config yaml file.
   * @param reinitialize  Whether to reload the yaml reader once again from the
   * config path. This is used to reload the config after editing it on the fly.
   */
  static void initYAMLReader(const std::string &filepath,
                             const bool reinitialize = false);

  /**
   * @brief Initialise the YAMLReader from another YAMLReader's dump.
   * @param dump          Yaml dump which this method will try to parse.
   * @param reinitialize  Whether its okay to reinitialize (replace) the yaml
   *                      reader. Doing this is necessary on sensornodes where
   *                      the yaml config is replaced when the core restarts.
   */
  static void initYAMLReaderFromDump(const std::string &dump,
                                     bool reinitialize = false);

  /** Set the config's main section, should be part of \ref node. */
  void setMainSection(const ConfigNode &section);

  /** Get the config's main section, node within \ref node. */
  const ConfigNode &main() const;

  /** Whether currently running on a SensorNode or on core. */
  bool isSensorNode() const;

  /** Update state whether currently running on a SensorNode. */
  void setSensorNode(bool sensornode);

  /** Update state whether currently running on a SensorNode. */
  void setMastercontrol(bool mastercontrol);
};

} // namespace psi_act

namespace YAML {

void operator>>(const YAML::Node &node, psi_act::TimePoint &t);

void operator>>(const YAML::Node &node, psi_act::Duration &d);

template <> struct convert<psi_act::Duration> {
  static bool decode(const Node &node, psi_act::Duration &rhs) {
    if (!node.IsScalar()) {
      return false;
    }
    node >> rhs;
    return true;
  }
};

template <> struct convert<psi_act::TimePoint> {
  static bool decode(const Node &node, psi_act::TimePoint &rhs) {
    if (!node.IsScalar()) {
      return false;
    }
    node >> rhs;
    return true;
  }
};

} // namespace YAML

#define AC_CR YAMLReader::getYAMLReader()
#endif
