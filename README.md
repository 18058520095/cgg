# linux_cpp_lab
C++17 Linux 系统编程实验项目

## 构建
在项目根目录执行：

```bash
cmake -S . -B build
cmake --build build -j
```

## 运行
所有命令均从项目根目录 `linux_cpp_lab` 执行。文件复制示例：

```bash
echo "这是测试内容" > src.txt
./build/file_copy src.txt dst.txt
cat dst.txt
```

服务器和客户端需要在**两个终端**中运行。以下示例使用本机回环地址；服务器启动后，在另一个终端启动客户端并输入要回显的文本。

### TCP echo

终端 1（服务器）：

```bash
./build/tcp_echo_server 127.0.0.1 8888
```

终端 2（客户端）：

```bash
./build/tcp_echo_client 127.0.0.1 8888
```

### UDP echo

终端 1（服务器）：

./build/udp_echo_server 127.0.0.1 9000

终端 2（客户端）：

./build/udp_echo_client 127.0.0.1 9000

### 进程池 TCP 服务器

终端 1（4 个 worker 进程）：

./build/process_pool_server 127.0.0.1 9100 4

终端 2（复用 TCP echo 客户端）：

./build/tcp_echo_client 127.0.0.1 9100

### 线程池 TCP 服务器

终端 1（4 个 worker 线程）：

./build/tcp_pool_server 127.0.0.1 9200 4

终端 2（复用 TCP echo 客户端）：

./build/tcp_echo_client 127.0.0.1 9200