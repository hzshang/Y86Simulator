## Y86 Pipeline 
#####写之前应检查指令是否被读  
#####每个时钟周期，每个流水线从缓冲区读取并执行指令 并将结果写到下一条流水线的缓冲区

## Fetch  
先进行pcUpdate  
从指令存储器中取出各个值，将其写到D层
- server 端口49936  
- client 连接Decode_server  

变量  
- PC
- PrePC

## Decode  
存放8个寄存器
读取寄存器，处理冒险，将其写到E层  
- 连接Memory
- 连接writeback

## Execute  
ALU计算，将其写到M层  
- server 端口49940
- client 连接Memory_server  
变量  
ifun,rA,rB,valC,valP

## Memory  
存放memory
读写内存，写到WriteBack层
- server 端口49942
- client 连接WriteBack_server  
变量  
ifun,valC,valA,valB,desE,desM,srcA,srcB

## Writeback  
写回寄存器
- server 端口49944
- client 连接

### 时钟线程
## 所有的tcp连接
client|server|cid
------|------|---
Fetch|WriteBack|1
Fetch|Memory|2
Fetch|Decode|3
Decode|Execute|4
Decode|Memory|5
Decode|Writeback|6
Execute|Memory|7
Memory|Writeback|0


## 建立连接
广播地址：37877
点击开始连接后
每个client都在指定的上端口广播  
格式
```json
{
    "id":1,
    "level":"F",
    "ip":"192.168.1.1",
    "port":43396
}
```
在指定的端口监听需要的广播，解析并连接，再次广播自己建立了连接
```json
{
    "id":2,
    "cid":3
}
```
时钟线程监听（和PCUpdate在一起）,并检测所有连接是否完成，完成以后发送广播开始
```json
{
    "id":3   
}
```
开始之后监听每个阶段的是否完成，如果完成广播这一个周期结束
```json
{
    "id":4,
    "step":23
}
```
等待特定时间后再次发送时钟信号，继续监听阶段状况。
结束时，Write_back广播结束
```json
{
    "id":5
}
```


## y86广播格式
取消连接
```json
{
    "id":0
}
```
广播自己的level,点击连接按钮后，每个客户端每一秒发送一次该链接
```json
{
    "id":1,
    "FLevel":True,
    "DLevel":True,
    "ELevel":True,
    "MLevel":True,
    "WLevel":False
}
```
给master广播自己已建立连接
```json
{
    "id":2,
    "cid":6
}
```
master广播开始
```json
{
    "id":3
}
```