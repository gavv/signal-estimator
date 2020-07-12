#! /bin/bash
set -euo pipefail
cd "$(dirname "$0")"
markdown-toc -i ../README.md
