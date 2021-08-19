# Maho_Shojo
Maho_Shojo compiler 欢迎来到魔法少女的世界~

- [Maho_Shojo](#maho_shojo)
  - [References](#references)
  - [魔法少女调教日志](#魔法少女调教日志)
  - [魔法少女育成指南](#魔法少女育成指南)
  - [魔法少女使用指南](#魔法少女使用指南)
    - [flag说明:](#flag说明)
    - [优化说明](#优化说明)

## References
竞赛所需资料
https://gitlab.eduxiji.net/nscscc/compiler2021/-/tree/master


## 魔法少女调教日志
> 06.06 完成词法、语法分析，语法树构建、TreePrinter,简单的语义检查checker。

> 07.18 完成MIR构建。准备进行目标平台代码生成。

> 07.21 完成MIR测试，通过2020，2021全部公开的功能测试。

> 08.08 基本完成Codegen功能测试

> 08.18 完成答辩，开源

## 魔法少女育成指南
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make -j6
```

## 魔法少女使用指南
```shell
$ cd build
$ ./compiler [ -h | --help ] [ -p | --trace_parsing ] [ -s | --trace_scanning ] [ -emit-mir ] [ -emit-lir ] [ -emit-ast ] [-check] [-o <output-file> ] [ -O2 ] [ -S ] [ -no-const-prop ] [ -no-ava-expr ] [ -no-cfg-simply ] [ -no-dead-code-eli ] [ -no-func-inline ] [ -no-loop-expand ] [ -no-loop-invar ] [ -no-sccp ] <input-file>
```

### flag说明:

`-h`或`--help`:帮助信息

`-p`或`--trace_parsing`追踪语法分析详情

`-s`或`--trace_scanning`追踪词法分析详情

`-emit-mir`生成MIR。

`-emit-lir`生成LIR

`-o <output-file>`指定输出文件。未指定的情况下默认输出文件是`testcase.ll`或`testcase.s`

`-emit-ast`通过ast复原代码，直接打印出来

`-check`进行静态检查

`-O2`开启优化

`-S`生成armv7ve汇编

`-no-x`不开启某项优化（只有`-O2`时这些flag才有作用）

### 优化说明

默认不开启优化。开启`-O2`后会执行的优化有

1. 公共子表达式删除
2. 控制流简化
3. 常量传播
4. 死代码删除
5. 函数内联
6. 全局变量局部化
7. 循环展开
8. 循环不变外提
9. 稀疏有条件常量传播（SCCP）