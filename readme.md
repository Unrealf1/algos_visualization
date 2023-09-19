#### What is this?

Repository of some graph traversal algorithms and their iterative visualization. Different 2d maze generation algorithms are represented here as well.

#### Build instructions

`cmake -B build -DCMAKE_BUILD_TYPE=Debug` // One may also use `Release` option here, but there are problems with MSVS builds on windows

`cmake --install build` // Libraries, executables and configuration files will appear in `package` subfolder of the project


#### Options

See `config.json` for configuration options

![Example](maze_traversal.GIF)
