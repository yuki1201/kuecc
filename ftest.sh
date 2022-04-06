#!/bin/bash
assert() {
 expected="$1"
 input="$2"
 input2="$3"
 ./ycc "$input" "$input2"> tmp.s
 cc -o tmp tmp.s
 ./tmp
 actual="$?"
 if [ "$actual" = "$expected" ]; then
 echo "./ycc $input $input2 => $actual"
 else
 echo "./ycc $input $input2 => $expected expected, but got $actual"
 exit 1
 fi
}
assert 0 "-f" "code.yc"
assert 5 "-f" "code2.yc"
assert 10 "-f" "code3.yc"

echo OK