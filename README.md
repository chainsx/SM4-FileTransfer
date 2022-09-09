Filetransfer using SM4 decrypt

```
Send a file  : filetransfer -send -key <key> -iv <iv> -mode <cbc/crt> -files <filepath>
Get a file   : filetransfer -get -ip <ipaddr> -key <key> -iv <iv> -mode <cbc/crt>
Help         : filetransfer -help\n\n", VERSION)
```

```
IV=11223344556677881122334455667788
KEY=1122334455667788
filetransfer -send -key $KEY -iv $IV -mode cbc -files text.txt
```
