#!/usr/bin/env bash
set -euo pipefail

./scripts/ci/install_deps.sh

mkdir -p build && cd build
cmake .. -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$HOME/.local" \
  -DBUILD_TESTING=ON
cmake --build . -- -j"$(nproc || sysctl -n hw.ncpu)"
ctest --output-on-failure
cmake --install .

echo "✅ Built, tested & installed daedalux to \$HOME/.local/bin"

