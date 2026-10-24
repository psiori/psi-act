#pragma once

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "ClockProvider.hpp"
#include "SystemClock.hpp"

namespace psi_act {

/** Clock factory mechanics with system time as the built-in default. */
template <typename Options> class ClockProviderRegistry {
public:
  /** Construct a provider from application-defined settings. */
  using Factory = std::function<std::unique_ptr<ClockProvider>(const Options&)>;

  /** Get the shared registry for this application's configuration type. */
  static ClockProviderRegistry& getRegistry() {
    static ClockProviderRegistry registry;
    return registry;
  }

  /** Register a factory, retaining the first registration for a name. */
  void add(const std::string& name, Factory factory) {
    factories_.emplace(name, std::move(factory));
  }

  /** Find a factory, reporting an unknown provider type as an error. */
  const Factory& lookup(const std::string& name = "SystemClock") const {
    const auto entry = factories_.find(name);
    if (entry == factories_.end()) {
      throw std::runtime_error("Could not find clock provider type " + name);
    }
    return entry->second;
  }

  /** Check whether an application registered a provider. */
  bool contains(const std::string& name) const {
    return factories_.find(name) != factories_.end();
  }

private:
  /** Register the default clock without application-specific configuration. */
  ClockProviderRegistry() {
    add("SystemClock",
        [](const Options&) { return std::make_unique<SystemClock>(); });
  }

  /** Factories registered by the application before worker threads start. */
  std::map<std::string, Factory> factories_;
};

}  // namespace psi_act

/** Register a config-constructed clock; use once per provider class. */
#define REGISTER_CLOCK_PROVIDER(name, Options)                                 \
  class name##Factory {                                                        \
  private:                                                                     \
    static struct _static_initialization_helper {                              \
      /** Register the factory before application setup. */                    \
      _static_initialization_helper() {                                        \
        psi_act::ClockProviderRegistry<Options>::getRegistry().add(            \
            #name, &name##Factory::getInstance);                               \
      }                                                                        \
    } _static_initializer;                                                     \
                                                                               \
  public:                                                                      \
    /** Construct the registered provider from application options. */         \
    static std::unique_ptr<psi_act::ClockProvider>                             \
    getInstance(const Options& options) {                                      \
      return std::make_unique<name>(options);                                  \
    }                                                                          \
  };                                                                           \
  name##Factory::_static_initialization_helper                                 \
      name##Factory::_static_initializer;
