#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./jcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expetced, but got $actual"
    fi
}

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 21 "5 + 20 - 4"

echo OK
