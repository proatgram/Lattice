# Lattice

Another build system written in C++. This one is built to be:
 - Simple: Easy YAML configuration files and basic by default.
 - Fast: Heiarchical object system and caching
 - Extendable: Native plugins

### But, why?
It's simple: I wanted a project to build, and I got fed up with having to jump over to the CMake documentation every time I wanted to do a very simple thing. I though: It would be a lot simpler to just have a build system that I can just configure, instead of needing to learn a new language to use.

## Usage/Examples
### Plugins
#### Overview
By default, Lattice is just an engine and cannot do much. In order to use Lattice, Plugins can be made and installed that add capabilities to lattice.

A plugin can:
 - Add a new language toolchain.
 - Add a new provider.
 - Add a new object type.
 - Add new system tooling.

The list might not seem big, but the things Lattice allows you to do with those is.

#### Using them
Using them is simple: You just install them, and Lattice handles the rest. The Plugin, if it behaves, should register all of it's types with Lattice, new toolchains, providers, language types, system tooling, should be made available and usable.

#### Making them
Making them is simple too: You simply inherit from types defined by Lattice in C++, implement your plugin specific stuff (e.g. the specifics of how a language library is build using a toolchain by calling tools), and register them with with Lattice.

A very barebones example plugin that can get you started is located at [examples/plugin](https://github.com/proatgram/Lattice/tree/main/examples/plugin).

Additionally, you can reference an official C/C++ Plugin [here](https://github.com/proatgram/LatticeCxxPlugin).

### Configuration
Here's an example configuration file containing a lot of stuff you may use. Don't worry, it may seem like a lot, but it's actually really simple!

```YAML
include:
  - "someDir"
  - someFile

project:
  Lattice:
    description: "Another build engine written in C++ based solely on configuration files."
    homepage: "https://github.com/proatgram/Lattice"
    version: 0.1
    default_toolchain: g++
    requires:
      - libcurl:
          version: ">=8"
          provider: "pkg-config"
          optional: true

Lattice:
  binary:
    lattice:
      output_name: "lattice"
      sources: ["src/main.cpp"]
      dependencies:
        private: [Utils@Lattice, Plugins@Lattice]
    SomeOtherBinary:
      sources: ["src/other/main.cpp"]
  library:
    Utils:
      type: static
      sources: ["src/Utils.cpp"]
      headers:
        public:
          - "include/Utils.hpp"
    Plugins:
      export:
        - modules
      sources:
        - "src/modules/Plugins.cpp"
      modules:
        - "modules/Plugins.cppm"
      dependencies:
        public: [Utils@Lattice]
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
 - `lattice build aBuildableObject@aProjectName`
    - Builds just one target, unless it depends on others.

#### Caches
Lattice mostly works with caches to build things. This enables Lattice to easily do incremental builds by checking if a cache for an object is dirty or not. Additionally it speeds up parsing for consecutive runs. When there is no cache present, the first time you run build, it will generate one. If you wish to preload the cache, you can run `lattice cache init`.

### Running an executable
Running is simple too! If you've only defined one project and one executable object, you can just do `lattice run`. If you've defined multiple projects, you can do `lattice run projectName`. If you define multiple executable objects in a project, you must specifiy which one you want to run: `lattice run objectName@projectName`.

## Authors

- [@thetimbrick](https://www.github.com/proatgram)
