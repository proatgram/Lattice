export module Lattice.Object.Capabilities.HasProperties;

export import std;

export import Lattice.Object.Capabilities.ICapability;
export import Lattice.Object.Properties.IProperty;

export namespace Lattice::Object::Capabilities {
    /**
     * @brief Provides the capability for inheriting Objects to propogate
     * properties.
     */
    class HasProperties : public ICapability {
        public:
            virtual ~HasProperties() = default;

            /**
             * @brief Gets a Public Property if it exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return A std::shared_ptr to the Property if it exists, std::nullopt if it doesn't.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto GetPublicProperty() const -> std::optional<const std::shared_ptr<const T>> {
                try {
                    std::shared_ptr<Properties::IProperty> property = m_propertyStore.at(typeid(T));
                    if (property->GetVisibility() == Properties::IProperty::Visibility::Public)
                        return property;

                    return {};
                } catch ([[ maybe_unused ]] const std::out_of_range &err) {
                    return {};
                }
            }

            /**
             * @brief Gets a Public Property if it exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return A std::shared_ptr to the Property if it exists, std::nullopt if it doesn't.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto GetPublicProperty() -> std::optional<std::shared_ptr<T>> {
                try {
                    std::shared_ptr<Properties::IProperty> property = m_propertyStore.at(typeid(T));
                    if (property->GetVisibility() == Properties::IProperty::Visibility::Public)
                        return property;

                    return {};
                } catch ([[ maybe_unused ]] const std::out_of_range &err) {
                    return {};
                }
            }

            /**
             * @brief Checks if a Public Property exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return true if the Property exists, false otherwise.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto HasPublicProperty() const -> bool {
                return m_propertyStore.contains(typeid(T)) && (m_propertyStore.at(typeid(T))->GetVisibility() == Properties::IProperty::Visibility::Public);
            }

        protected:
            /**
             * @brief Adds a Property to the internal Property store.
             *
             * If a property that is being added already exists in the store,
             * it will be added to that existing property.
             *
             * @param[in] property The property to add
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto AddProperty(const std::shared_ptr<T> &property) -> void {
                try {
                    *(m_propertyStore.at(typeid(T))) += *(property);
                } catch (const std::out_of_range &err) {
                    m_propertyStore[typeid(T)] = property;
                }

                return;
            }

            /**
             * @brief Gets a Private Property if it exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return A std::shared_ptr to the Property if it exists, std::nullopt if it doesn't.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto GetPrivateProperty() const -> std::optional<const std::shared_ptr<const T>> {
                try {
                    std::shared_ptr<Properties::IProperty> property = m_propertyStore.at(typeid(T));
                    if (property->GetVisibility() == Properties::IProperty::Visibility::Private)
                        return property;

                    return {};
                } catch ([[ maybe_unused ]] const std::out_of_range &err) {
                    return {};
                }
            }

            /**
             * @brief Gets a Private Property if it exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return A std::shared_ptr to the Property if it exists, std::nullopt if it doesn't.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto GetPrivateProperty() -> std::optional<std::shared_ptr<T>> {
                try {
                    std::shared_ptr<Properties::IProperty> property = m_propertyStore.at(typeid(T));
                    if (property->GetVisibility() == Properties::IProperty::Visibility::Private)
                        return property;

                    return {};
                } catch ([[ maybe_unused ]] const std::out_of_range &err) {
                    return {};
                }
            }

            /**
             * @brief Checks if a Private Property exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return true if the Property exists, false otherwise.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto HasPrivateProperty() const -> bool {
                return m_propertyStore.contains(typeid(T)) && (m_propertyStore.at(typeid(T))->GetVisibility() == Properties::IProperty::Visibility::Private);
            }

            /**
             * @brief Gets a Private Property if it exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return A std::shared_ptr to the Property if it exists, std::nullopt if it doesn't.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto GetProperty() const -> std::optional<const std::shared_ptr<const T>> {
                try {
                    return m_propertyStore.at(typeid(T));
                } catch ([[ maybe_unused ]] const std::out_of_range &err) {
                    return {};
                }
            }

            /**
             * @brief Gets a Private Property if it exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return A std::shared_ptr to the Property if it exists, std::nullopt if it doesn't.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto GetProperty() -> std::optional<std::shared_ptr<T>> {
                try {
                    return m_propertyStore.at(typeid(T));
                } catch ([[ maybe_unused ]] const std::out_of_range &err) {
                    return {};
                }
            }

            /**
             * @brief Checks if a Property exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return true if the Property exists, false otherwise.
             */
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto HasProperty() const -> bool {
                return m_propertyStore.contains(typeid(T));
            }

        private:
            std::map<std::type_index, std::shared_ptr<Properties::IProperty>> m_propertyStore;
    };
}  // export namespace Lattice::Object::Capabilities
