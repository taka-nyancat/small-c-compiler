#!/bin/bash

# assert関数の作成
assert() {
    # 引数の設定
    expected="$1"
    input="$2"

    # アセンブリ
    ./main "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    # 判定
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 21 "5+20-4"
assert 21 "5 + 20 - 4"


echo OK