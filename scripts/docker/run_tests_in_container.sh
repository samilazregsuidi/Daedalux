#!/usr/bin/env bash
set -euo pipefail

echo "Running daedalux tests in Docker…"
# Stop & remove any old container
docker rm -f daedalux || true

# Build (just in case)
./scripts/docker/build_image.sh

# Run and stream output
docker run --name daedalux daedalux:latest
echo "✅ Tests completed."

