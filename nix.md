# Building BlockSci with Nix

This project ships a Nix flake that provides:

- Two package derivations (`packages.default` for libblocksci + tools,
  `packages.blockscipy` for the Python bindings).
- A development shell (`devShells.default`) that reproduces the upstream
  build manual with all native dependencies preinstalled, no system pollution.

The flake is pinned to `nixpkgs-19.03`, the last nixpkgs release on which
BlockSci builds unpatched. Supported systems: `x86_64-linux`, `aarch64-linux`.

BlockSci vendors several dependencies as git submodules, so `nix build`
invocations below require `?submodules=1`. If the repository was cloned
without `--recurse-submodules`, initialize them first:

```
git submodule update --init --recursive
```

## Build libblocksci and the command-line tools

```
nix build '.?submodules=1'
```

## Build the Python bindings

```
nix build '.?submodules=1#blockscipy'
```

## Building in the development shell

This workflow builds the project step by step, self-contained under
`.nix-install/`, `.nix-pip/` and `release`.

1. Enter the shell:

```
nix develop
```

2. Build and install libblocksci:

```bash
mkdir release && cd release
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=../.nix-install ..
make
make install
cd ..
```

3. Build the Python bindings:

```
pip install --prefix=$LOCAL_PIP -e blockscipy
```

`LOCAL_PIP` and `CMAKE_PREFIX_PATH` are exported by the shellHook so the
two halves find each other.
