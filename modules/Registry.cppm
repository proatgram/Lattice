module;
#include <ranges>
export module Lattice.Registry;

export import std;

export namespace Lattice {
    /**
     * @brief A simple registry.
     *
     * A registry is a singleton, obtained by calling `GetInstance()` on the registry.
     * The registry can have contain elements of any type, but the key is always a string.
     *
     * A Registry element is unique and forceful. If you try to register something that
     * already exists, it will not overwrite it, and will give an error.
     */
    class Registry {
        struct Constructable {};
        public:
            Registry(Constructable);

            /**
             * @brief Gets the instance for the registry.
             *
             * @return A shared_ptr to the Registry instance.
             */
            static auto GetInstance() -> std::shared_ptr<Registry>;


            /**
             * @brief Registers a value with a specific key.
             *
             * This method will fail if the given key already exists and
             * return an unexpected value of an error string.
             *
             * @param[in] key The key for the addition.
             * @param[in] value The value to add.
             *
             * @tparam Val Value type
             *
             * @return A `std::expected` containing an expected value of `void`, or unexpected value of `std::string` telling what happened.
             */
            template <typename Val>
            inline auto Register(const std::string &key, const Val &value) -> std::expected<std::reference_wrapper<Val>, std::string> {
                std::type_index typeKey = typeid(std::map<std::string, Val>);
                
                if (m_registryStore.contains(typeKey)) {
                    auto &map = std::any_cast<std::map<std::string, Val>&>(m_registryStore.at(typeKey));
                    
                    if (map.contains(key))
                        return std::unexpected("Key already exists in Registry.");
                    
                    map[key] = value;
                } else {
                    m_registryStore[typeKey] = std::map<std::string, Val>{{key, value}};
                }
                
                auto &map = std::any_cast<std::map<std::string, Val>&>(m_registryStore.at(typeKey));
                auto it = map.find(key);
                
                if (it == map.end())
                    return std::unexpected("Failed to register: key not found after insertion.");
                
                return std::reference_wrapper<Val>(it->second);
            }

            /**
             * @brief Unregisters a specific key.
             *
             * This method will fail if the given key doesn't exist
             * in the registry and will return an unexpected value
             * giving an error string.
             *
             * @param[in] key The key to remove.
             *
             * @tparam Val Value type
             *
             * @return A `std::expected` containing an expected value of `void`, or unexpected value of `std::string` telling what happened.
             */
            template <typename Val>
            inline auto Unregister(const std::string &key) -> std::expected<void, std::string> {
                if (!m_registryStore.contains(typeid(std::map<std::string, Val>)))
                    return std::unexpected("std::string doesn't exist in Registry,");

                std::any_cast<std::map<std::string, Val>>(m_registryStore[typeid(std::map<std::string, Val>)]).erase(key);
                if (std::any_cast<std::map<std::string, Val>>(m_registryStore[typeid(std::map<std::string, Val>)]).empty())
                    m_registryStore.erase(typeid(std::map<std::string, Val>));

                return {};
            }

            /**
             * @brief Queries for a specific key.
             *
             * @param[in] key The key to query for.
             *
             * @tparam Val Value type
             *
             * @return A `std::optional` containing the value for the key, or nothing if it wasn't found.
             */
            template <typename Val>
            inline auto Query(const std::string &key) const -> std::optional<Val> {
                if (!m_registryStore.contains(typeid(std::map<std::string, Val>)))
                    return {};

                std::map<std::string, Val> registry = std::any_cast<std::map<std::string, Val>>(m_registryStore.at(typeid(std::map<std::string, Val>)));

                typename std::map<std::string, Val>::const_iterator it = registry.find(key);

                return (it != std::cend(registry) ? std::optional<Val>{it->second} : std::optional<Val>{std::nullopt});
            }

            /**
             * @brief Checks if a key exists.
             *
             * @param[in] key The key to check for.
             *
             * @tparam Val Value type
             *
             * @return `true` if found, `false` if not.
             */
            template <typename Val>
            inline auto Contains(const std::string &key) const -> bool {
                if (!m_registryStore.contains(typeid(std::map<std::string, Val>)))
                    return false;

                return std::any_cast<std::map<std::string, Val>>(m_registryStore.at(typeid(std::map<std::string, Val>))).contains(key);
            }

            /**
             * @brief Gets all of the values.
             *
             * @tparam Val Value type
             *
             * @return A `std::list<Val>` of all the values in the registry.
             */
            template <typename Val>
            inline auto All() const -> auto {
                if (!m_registryStore.contains(typeid(std::map<std::string, Val>)))
                    return std::map<std::string, Val>{} | std::views::values;

                return std::any_cast<std::map<std::string, Val>>(m_registryStore.at(typeid(std::map<std::string, Val>))) | std::views::values;
            }

        private:
            std::map<std::type_index, std::any> m_registryStore;
    };
}  // export namespace Lattice
