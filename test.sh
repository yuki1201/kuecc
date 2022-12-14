#!/bin/bash
assert() {
 expected="$1"
 input="$2"
 ./ycc "$input" > tmp.s
 cc -o tmp tmp.s
 ./tmp
 actual="$?"
 if [ "$actual" = "$expected" ]; then
 echo "$input => $actual"
 else
 echo "$input => $expected expected, but got $actual"
 exit 1
 fi
}
assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 "-10 + 20;"
assert 1 "10==10;"
assert 1 "10!=11;"
assert 1 "5+2>5-2;"
assert 1 "5>3;"
assert 7 "a=2*3;b=4+2-5;c=a+b;"
assert 10 "return 10;"
assert 6 "a=1;b=2;c=3;return a+b+c;"
assert 3 'if (0){ return 2;} return 3;'
echo OK
