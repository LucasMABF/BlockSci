{
  description = "BlockSci analysis tool for blockchains";

  inputs = {
    nixpkgs = {
      url = "github:NixOS/nixpkgs/nixos-19.03";
      flake = false;
    };
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachSystem [ "x86_64-linux" "aarch64-linux" ] (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
        pythonRuntimeDeps =
          ps: with ps; [
            dateparser
            multiprocess
            pandas
            psutil
            pycrypto
            requests
          ];
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "blocksci";
          version = "0.7.0";
          src = self;

          nativeBuildInputs = with pkgs; [
            cmake
          ];

          buildInputs = with pkgs; [
            curl
            gtest
            jsoncpp
            libjson-rpc-cpp
            lz4
            openssl_1_1
            python37
            secp256k1
            sparsehash
          ];

          propagatedBuildInputs = with pkgs; [
            boost
          ];

          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
            "-DUSE_SYSTEM_DEPS=ON"
          ];
        };

        packages.blockscipy = pkgs.python37Packages.buildPythonPackage {
          pname = "blockscipy";
          version = "0.7.0";
          format = "setuptools";

          dontUseCmakeConfigure = true;

          src = "${self}/blockscipy";

          nativeBuildInputs = with pkgs; [
            cmake
          ];

          buildInputs = [
            self.packages.${system}.default
          ];

          propagatedBuildInputs = pythonRuntimeDeps pkgs.python37Packages;
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ self.packages.${system}.default ];

          nativeBuildInputs =
            with pkgs;
            [
              autoconf
              automake
              git
              libtool
              python37Packages.pip
            ]
            ++ pythonRuntimeDeps pkgs.python37Packages;

          shellHook = ''
            export LOCAL_PIP="$PWD/.nix-pip"
            mkdir -p "$LOCAL_PIP/${pkgs.python37.sitePackages}"
            export PYTHONPATH="$LOCAL_PIP/${pkgs.python37.sitePackages}:$PYTHONPATH"
            export PATH="$LOCAL_PIP/bin:$PATH"

            export CMAKE_PREFIX_PATH="$PWD/.nix-install:$CMAKE_PREFIX_PATH"
          '';
        };
      }
    );
}
