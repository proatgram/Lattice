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
    template <class Factory, typename T>
    class IFactory {
        public:
            /**
             * @brief Gets the factory instance
             *
             * @return Shared pointer to the factory instance
             */
            inline static auto GetInstance() -> std::shared_ptr<Factory> {
                static std::shared_ptr<Factory> instance = std::make_shared<Factory>(Constructable());
                return instance;
            }

            /**
             * @brief Creates an object from the factory
             *
             * @param Identifier for the factory creation
             *
             * @return Shared pointer to the factoried object
             */
            virtual auto Create(const std::string &identifier) -> std::shared_ptr<T> = 0;

        protected:
            struct Constructable {};
    };
}  // export namespace Lattice::Plugin
