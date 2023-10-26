#!env bash
set -euo pipefail

run() {
    echo "Running: $*" 1>&2
    "$@" || exit 1
}

if ! which markdown-toc >/dev/null 2>&1; then
    echo "error: markdown-toc not found in PATH" >&2
    echo "you can install it using 'npm install --save markdown-toc'" >&2
fi

if ! which mdspell >/dev/null 2>&1; then
    echo "error: mdspell not found in PATH" >&2
    echo "you can install it using 'npm install --save markdown-spellcheck'" >&2
fi

cd "$(dirname "$0")"/..

run mdspell -n -a README.md
run sort .spelling -o .spelling

run markdown-toc -i README.md

cat >/tmp/signal-estimator.help <<'EOF'
<!-- help -->

```
$ signal-estimator --help
EOF

run bin/$(uname -m)-linux-gnu/signal-estimator --help |
    sed -e 's/[[:space:]]*$//' >>/tmp/signal-estimator.help

cat >>/tmp/signal-estimator.help <<'EOF'
```

<!-- helpstop -->
EOF

sed -e '/<!-- help -->/{r /tmp/signal-estimator.help' \
    -e 'N' \
    -e '}; /<!-- help -->/,/<!-- helpstop -->/d' \
    -i README.md

echo "Updated README.md"
