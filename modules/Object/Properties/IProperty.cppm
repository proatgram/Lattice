export module Lattice.Object.Properties.IProperty;

export import std;

export namespace Lattice::Object::Properties {
    /**
     * @brief Represents a property that can be propogated up from
     * different objects.
     */
    class IProperty {
        public:
            enum class Visibility {
                Public,
                Private
            };

            IProperty(Visibility visibility);
            virtual ~IProperty() = default;

            auto GetVisibility() const -> Visibility;

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

        private:
            Visibility m_visibility;
    };
}  // export namespace Lattice::Object::Properties
