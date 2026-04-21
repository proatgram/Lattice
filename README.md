
# Lattice

Another build system for C++. This one is built to be:
 - Simple: Easy YAML configuration files.
 - Fast: Heiarchical object system and caching
 - Extendable: Native plugins



## Usage/Examples
### Configuration
Here's an example configuration file containing a lot of stuff you may use. Don't worry, it may seem like a lot, but it's actually really simple!

```YAML
include:
  - "someDir"
  - "someFile"

project:
  lattice:
    description: "Another build system for C++ based solely on configuration files."
    homepage: "https://github.com/proatgram/lattice"
    version: 0.1
    requires:
      - libcurl:
          version: ">=8"
          provider: "pkg-config"
          optional: true

lattice:
  binary:
    Lattice:
      output_name: "lattice"
      sources: ["src/main.cpp"]
      dependencies:
        private: [lattice::libcurl, lattice::Plugins]
    SomeOtherBinary:
      sources: ["src/other/main.cpp"]
  module:
    Plugins:
      units:
        export:
          - "modules/Plugins.cppm"
      sources:
        - "src/modules/Plugins.cpp"
      dependencies:
        public: [lattice::Utils]
  library:
    Utils:
      type: static
      sources: ["src/Utils.cpp"]
      headers:
        public:
          - "include/Utils.hpp"


```

For the basics, you first start off by defining a project by putting an entry in the `project:` section. You can define multiple projects in one configuration set. You can then define your project properties in that section.
```YAML
project:
    yourProject:
        # Any properties here?
```

Second, you define objects that make up that project. An object is tied to a project by defining a section with an identifier the same name as the project. You can then define different objects, whether it be a binary (something you execute), a module (a modern C++ feature, a type of library), a library (something you link to), or something else provided by a plugin, creating a section under that project section with the type of object, and any objects you want to define under that.

```YAML
yourProject:
    binary:
        binaryOne:
            # Any properties here?
        binaryTwo:
            # Any properties here?
    library:
        libraryOne:
            # ...
        libraryTwo:
            # ...
```
### Building
Now that you have a basic configuration, you can build your project(s), which you can do by running one of these:
 - `lattice build`
    - Builds everything, every project and all targets in each project.
 - `lattice build aProjectName`
    - Builds just the specified project and all of it's targets unless it depends on others.
 - `lattice build aProjectName::aBuildableObject`
    - Builds just one target, unless it depends on others.

#### Caches
Lattice mostly works with caches to build things. When there is no cache present, the first time you run build, it will generate one, and use that cache for the build. If you wish to preload the cache, you can run `lattice cache init`.

### Running
Running is simple too! If you've only defined one project and one binary, you can just do `lattice run`. If you've defined multiple projects, you can do `lattice run projectName`. If you define multiple binaries in a project, you must specifiy which one you want to run: `lattice run projectName:objectName`.

## Authors

- [@thetimbrick](https://www.github.com/proatgram)
