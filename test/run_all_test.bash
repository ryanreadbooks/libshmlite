#!/bin/bash

# 运行所有生成的在bin目录下面的测试用例
for file in $(ls ../bin)
do 
  echo Testing ../bin/${file}
  ../bin/${file}
done