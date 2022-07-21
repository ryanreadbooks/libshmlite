# ShmLite - 在共享内存内的变量存储

[![Build Status](https://github.com/ryanreadbooks/libshmlite/actions/workflows/cmake.yml/badge.svg)](https://github.com/ryanreadbooks/libshmlite/actions)

### 需要支持的功能：支持像普通变量一样，用变量名－变量值的形式将一个变量放在共享内存中。

计划支持的数据类型：

* 基本数据类型
  * int
    * int32
    * int64
    * uint32
    * uint64
    * ...
  * char
  * float
  * double
* 字符串类型
* 容器
  * 顺序型容器
    * vector
    * queue
    * ...
  * 关系型容器
    * map
    * ...
