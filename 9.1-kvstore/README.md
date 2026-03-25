# High-Performance KV Store Engine (Linux C/C++)

这是一个探索 Linux 底层高性能网络 IO 与存储引擎极致性能的自研项目。系统实现了存储后端（数据结构）与网络调度引擎（IO 模型）的解耦，支持在 **Reactor (Epoll)**、**io_uring (Async IO)** 及 **NtyCo (Coroutine)** 之间灵活切换。

[Image of Linux IO infrastructure comparing Epoll and io_uring]

## 🌟 项目亮点

* **三维 IO 模型验证**：
    * **Reactor (Epoll-ET)**：基于非阻塞 IO 与边缘触发模式，实现经典的单线程高并发处理。
    * **io_uring (Proactor 范式)**：适配 Linux 5.10+ 内核，通过 SQ/CQ 双环缓冲区实现真正的异步 IO，规避了频繁的系统调用（Syscall）开销。
    * **NtyCo (协程)**：集成轻量级协程框架，将复杂的异步回调解耦为同步编程逻辑。
* **多态存储后端**：
    * **Hash Table**：基于 **DJB2 算法** 实现，点查询（Point Query）平均时间复杂度为 $O(1)$。
    * **RB-Tree**：自主实现红黑树平衡逻辑，支持有序检索及 $O(\log n)$ 范围查询。
* **工业级协议解析**：自研类 Redis 文本协议，支持 `SET/GET/DEL/MOD/EXIST` 等 15 种核心指令。
* **内存安全审计**：全局封装 `kvs_malloc/free`，结合 **Valgrind** 进行百万次级别压测，确保核心引擎零内存泄漏。

## 🚀 性能基准 (Benchmark)

测试环境：**Ryzen 7 5000H (Zen 3) @ Ubuntu 20.04 (Kernel 5.15)**

| 存储引擎 | IO 模型 | 操作总数 | 平均 QPS | 延迟 (Avg Latency) |
| :--- | :--- | :--- | :--- | :--- |
| **Hash Table** | io_uring | 1,000,000 | **120,000+** | < 10ms |
| **RB-Tree** | Epoll | 100,000 | **85,000+** | < 15ms |

## 🛠️ 构建与运行

### 依赖环境
* Linux Kernel 5.10+
* GCC/G++ 9.0+
* `liburing-dev`

### 编译与启动
make clean && make
./kvserver 2000

### 执行测试脚本，参数格式：[服务器IP] [服务器端口] [测试模式]
 模式定义：0-Array 引擎，1-RBTree 引擎，2-Hash 引擎
./testcase 127.0.0.1 2000 2 


## 📂 模块说明

* `kv_store.h`：核心抽象接口与宏定义配置。
* `kv_engine.c`：Hash Table、RB-Tree 等底层存储算法实现。
* `reactor.c` / `io_uring_start.c`：网络 IO 事件分发逻辑。
* `protocol.c`：协议解析与命令分发。
* `testcase.c`：多场景性能测试工具。

---

## 📝 写在最后

本项目是我在学习 **Linux C/C++ 高性能编程** 过程中的实践成果。通过本项目，我深入掌握了：

1.  **Linux 内核异步 IO (io_uring)** 的运作机制与 SQE/CQE 交互。
2.  **复杂数据结构** 在内存管理中的安全性控制。
3.  **网络模型对比** 及其在不同并发规模下的瓶颈分析。
