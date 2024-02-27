# Voxelaria
An open world voxel playground.

## Building

### Nix
If you have the Nix package manager installed, you can follow the following steps to build the application.

```sh
nix develop
meson setup build
meson compile -C build
```

### Non-Nix
If you do not have the Nix package manager, you can still manually complile this application.

You need the following dependencies:
- meson
- ninja
- gcc
- pkg-config
- glew-egl
- glfw-wayland
- glm

```sh
meson setup build
meson compile -C build
```

## License
The source code for this project is licensed under the MIT license. You may find the conditions of the license [here](LICENSE).
