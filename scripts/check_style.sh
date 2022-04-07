#!/usr/bin/env bash

source scripts/format_utils.sh

# @brief    Check style of files with clang-format. Copied mostly from
#           https://stackoverflow.com/a/22866610/2397253
# @param    $1  Newline-separated list of filenames
function check_style()
{
    echo "Using " `which $CLANGFORMAT` `$CLANGFORMAT --version`
    ALL_GOOD=1
    local FILES=$1
    if [[ -z "$FILES" ]]; then
        FILES=$(sources)
    fi
    if [[ "$FILES" ]]; then
        local _ALL_GOOD=0
        while read -r file; do
            if [[ "$file" =~ .*\.(cpp|hpp|tpp|c|h|ino|proto)$ ]]; then
                diff -u <(cat $file) <($CLANGFORMAT -style=file $file)
                if [ ! "$?" == "0" ]; then
                    printf "Bad: %s\n" $file
                    _ALL_GOOD=1
                fi
            else
                echo "Ignored $file"
            fi
        done <<< "$FILES"
        ALL_GOOD=$_ALL_GOOD
    fi
    if [[ "$ALL_GOOD" -eq 0 ]]; then
        echo "All good"
    fi
    return $ALL_GOOD
}

check_style $1
exit $?
