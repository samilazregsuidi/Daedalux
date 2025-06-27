#!/usr/bin/env bash
set -euo pipefail

echo "Building daedalux Docker image…"
docker build \
  --file Dockerfile \
  --platform linux/amd64 \
  --label daedalux \
  --tag daedalux:latest \
  .
echo "✅ Image daedalux:latest built."
