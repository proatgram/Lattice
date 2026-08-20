export module Lattice.Object.Capabilities.Propagatable;

export import std;

export import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    /**
     * @brief Provides the capability for inheriting Objects to propogate
     * properties.
     */
    class Propagatable : public ICapability {
        public:
            virtual ~Propagatable() = default;

            /**
             * @brief Represents a property that can be propogated up from
             * different objects.
             */
            class IProperty {
                public:
                    virtual ~IProperty() = default;
                    
                    /**
                     * @brief Additive propogation.
                     *
                     * The use of this operator overload allows properties to truly
                     * be propogated.
                     *
                     * Valid implementations of this method should add all components
                     * of the Property rhs to itself, and return a reference to `this`.
                     *
                     * @param[in] rhs Another property to inherit from.
                     *
                     * @return IProperty&
                     */
                    virtual auto operator+(const IProperty &rhs) -> IProperty& = 0;
            };

            /**
             * @brief Gets a property if it exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return A std::shared_ptr to the Property if it exists, std::nullopt if it doesn't.
             */
            template <typename T> requires std::is_base_of_v<IProperty, T>
            inline auto GetProperty() const -> std::optional<const std::shared_ptr<const T>> {
                try {
                    return m_propertyStore.at(typeid(T));
                } catch ([[ maybe_unused ]] const std::out_of_range &err) {
                    return {};
                }
            }

            /**
             * @brief Gets a property if it exists within the Property store.
             *
             * @tparam `T` A sub-class of IProperty.
             *
             * @return A std::shared_ptr to the Property if it exists, std::nullopt if it doesn't.
             */
            template <typename T> requires std::is_base_of_v<IProperty, T>
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
            template <typename T> requires std::is_base_of_v<IProperty, T>
            inline auto HasProperty() const -> bool {
                return m_propertyStore.contains(typeid(T));
            }

        protected:
            /**
             * @brief Adds a property to the internal Property store.
             *
             * If a property that is being added already exists in the store,
             * it will be added to that existing property.
             *
             * @param[in] property The property to add
             */
            template <typename T> requires std::is_base_of_v<IProperty, T>
            inline auto AddProperty(const std::shared_ptr<T> &property) -> void {
                try {
                    *(m_propertyStore.at(typeid(T))) += *(property);
                } catch (const std::out_of_range &err) {
                    m_propertyStore[typeid(T)] = property;
                }

                return;
            }

        private:
            std::map<std::type_index, std::shared_ptr<IProperty>> m_propertyStore;
    };
}  // export namespace Lattice::Object::Capabilities
