#!env bash
set -euo pipefail

if ! which markdown-toc >/dev/null 2>&1; then
    echo "error: markdown-toc not found in PATH" >&2
    echo "you can install it using 'npm install --save markdown-toc'" >&2
fi

cd "$(dirname "$0")"/..

cmd="markdown-toc -i README.md"
echo "running $cmd"
$cmd
echo "done"
