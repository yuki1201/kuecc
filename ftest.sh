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
assert 0 "-f" "testfiles/code.yc"
assert 5 "-f" "testfiles/code2.yc"
assert 10 "-f" "testfiles/code3.yc"
assert 56 "-f" "testfiles/code4.yc"
assert 10 "-f" "testfiles/code5.yc"
assert 34 "-f" "testfiles/code6.yc"

echo OK