# libco
`libco` 是一个最简单实现的协程库，支持64位和32位下的环境。😵目前只有C语言版本，将来会考虑提供C++版本。
## 编译环境
- Ubuntu 20.04
- GCC 9.4.0
## 项目构建
- Makefile
## 项目结构
```BASH
.
├── co.c
├── co.h
├── libco-32.so
├── libco-64.so
├── Makefile
├── README.md
└── tests
    ├── co-test.h
    ├── libco-test-32
    ├── libco-test-64
    ├── main.c
    └── Makefile
```
## 部署
如果你想要检验或运行本项目，建议在项目根目录和 `./test` 下执行 `make clean` 。然后再在 `./test` 下执行 `make test`。

如果您想直接运行 `./libco-test-64` 或 `./libco-test-32`，请在当前终端执行 `export LD_LIBRARY_PATH=..`，设置好环境变量，否则你将会遇到 ***“error while loading shared libraries: libco-xx.so: cannot open shared object file: No such file or directory”*** 的错误。

## 计划
- C   version ✔️
- C++ version ❌