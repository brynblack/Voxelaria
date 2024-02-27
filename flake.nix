{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system}; in
      {
        devShells.default = with pkgs; stdenv.mkDerivation {
          name = "env";
          nativeBuildInputs = [
            gcc
            meson
            ninja
            pkg-config
          ];
          buildInputs = [
            glew-egl
            glfw-wayland
            glm
          ];
        };
      }
    );
}
