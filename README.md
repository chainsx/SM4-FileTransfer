# 使用 SM4 加密的文件传输工具

## 如何使用

```
发送文件   : filetransfer -send -key <key> -iv <iv> -mode <cbc/crt> -files <filepath>
接受文件   : filetransfer -get -ip <ipaddr> -key <key> -iv <iv> -mode <cbc/crt>
获取帮助   : filetransfer -help\n\n", VERSION)
```

### 例如：
```
IV=11223344556677881122334455667788
KEY=1122334455667788
filetransfer -send -key $KEY -iv $IV -mode cbc -files text.txt
```

# 使用到的库：

[guanzhi/GmSSL](https://github.com/guanzhi/GmSSL)

# 开放源代码许可：

[guanzhi/GmSSL](https://github.com/guanzhi/GmSSL/blob/develop/LICENSE)
