export module Lattice.Project;

export import Lattice.Object;
export import Lattice.Object.Buildable;
export import Lattice.Object.Parsable;
import std;

export namespace Lattice {
    class Project final : public Object, public Buildable, public Parsable, public std::enable_shared_from_this<Project> {
        public:
            Project(Constructable, const std::string &identifier);

            static auto Create(const std::string &identifier) -> std::shared_ptr<Project>;

            auto GetIdentifier() const -> std::string;

            auto SetDescription(const std::string &description) -> std::shared_ptr<Project>;
            auto GetDescription() const -> std::string;

            auto SetVersion(const std::string &version) -> std::shared_ptr<Project>;
            auto GetVersion() const -> std::string;

            auto SetHomepageUrl(const std::string &homepageUrl) -> std::shared_ptr<Project>;
            auto GetHomepageUrl() const -> std::optional<std::string>;

            auto GetProperties() const -> std::bitset<8> final;

            auto Build() -> void final;
            auto Parse(const std::string &parsableString) -> void final;

        private:
            std::string m_description;
            std::string m_version;

            std::optional<std::string> m_homepageUrl;
    };
}  // export namespace Lattice
