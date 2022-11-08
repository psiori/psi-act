#!/usr/bin/env bash

set -e

ROOT=$(git rev-parse --show-toplevel 2> /dev/null || pwd)
INCLUDE_LIST="src/ tools/ tests/"
# Some systems have multiple clang-format versions installed, therefore use
# "clang-format-XX" explicitly if available.
if command -v clang-format-14; then
    CLANGFORMAT="clang-format-14"
elif command -v clang-format-13; then
    CLANGFORMAT="clang-format-13"
elif command -v clang-format-12; then
    CLANGFORMAT="clang-format-12"
elif command -v clang-format-11; then
    CLANGFORMAT="clang-format-11"
elif command -v clang-format-10; then
    CLANGFORMAT="clang-format-10"
else
    CLANGFORMAT="clang-format"
fi

# @brief    Get list of files in git index with names relative to root
function sources()
{
    (
        # we could maybe use `git diff --cached --name-only` for this,  which
        # outputs relative paths, may be simpler

        # note: I would like the pattern do be a variable, but for the life of me I cannot
        # figure out how to expand it in this process substitution
        FILE_STRING=$(git ls-tree -r $(git rev-parse --abbrev-ref HEAD) --full-name --name-only $INCLUDE_LIST | grep '.*\.\(cpp\|hpp\|tpp\|c\|h\|ino\|proto\)$' | sed -e "s:^:$ROOT/:g")
        echo "$FILE_STRING"
    )
}

function format()
{
    echo "Using" `which $CLANGFORMAT` `$CLANGFORMAT --version`
    FILES=$(sources)
    if [ -n "$FILES" ]; then
        if command -v parallel; then
            parallel $CLANGFORMAT -verbose -style=file -i {} ::: $FILES
        else
            $CLANGFORMAT -verbose -style=file -i $FILES
        fi
    else
        echo "Nothing to format."
        exit 0
    fi
}
