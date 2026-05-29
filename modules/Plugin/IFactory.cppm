export module Lattice.Plugin.IFactory;

import std;

export namespace Lattice::Plugin {
    /**
     * @brief An abstract factory class
     *
     * This abstract class can be inherited from in order to
     * create a factory for any other class. The factory pattern
     * is used significantly in this project to add functionality.
     */
    template <typename T>
    class IFactory {
        public:
            virtual ~IFactory<T>() = default;
            /**
             * @brief Creates an object from the factory
             *
             * @param Identifier for the factory creation
             *
             * @return Shared pointer to the factoried object
             */
            virtual auto Create(const std::string &identifier) -> std::shared_ptr<T> = 0;
    };

    template <typename Factory, typename T>
    class ISingletonFactory : public IFactory<T> {
        protected:
            struct Constructable {};
        public:
            using FactoryType = IFactory<T>;
            /**
             * @brief Gets the factory instance
             *
             * @return Shared pointer to the factory instance
             */
            inline static auto GetInstance() -> std::shared_ptr<IFactory<T>> requires std::is_constructible_v<Factory, Constructable> {
                static std::shared_ptr<Factory> instance = std::make_shared<Factory>(Constructable());

                return instance;
            }
    };
}  // export namespace Lattice::Plugin
