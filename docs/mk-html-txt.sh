#!/bin/sh

set -eu

# generate HTML and plain-text versions of the manual pages

mandoc -T html -O style=man-style.css cutechess-cli.6 > cutechess-cli.6.html
mandoc -T html -O style=man-style.css cutechess-engines.json.5 > cutechess-engines.json.5.html

mandoc -T ascii cutechess-cli.6 | col -b > cutechess-cli.6.txt
mandoc -T ascii cutechess-engines.json.5 | col -b > cutechess-engines.json.5.txt
