## Y86 Pipeline 
## 特点
- 五个阶段随意组合  
- 使udp和tcp通信  

## 注意 
每个时钟周期，每个流水线从缓冲区读取并执行指令 并将结果写到下一条流水线的缓冲区

## Fetch  
先进行pcUpdate  
从指令存储器中取出各个值，将其写到D层  

## Decode  
存放8个寄存器
读取寄存器，处理冒险，将其写到E层  

## Execute  
ALU计算，将其写到M层  
## Memory  
存放memory
读写内存，写到WriteBack层  

## Writeback  
写回寄存器

## 时钟线程
在master的应用程序上
等待五个阶段发送已完成信号
然后再等待固定时间，发送时钟信号。

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
## 和时钟的连接  

client|server
------|------
fetch|clock
decode|clock
execute|clock
memory|clock
writeback|clock


## Y86类TCP连接
将pool（32位）的指定位置为1，有master检测，如果前8位全为1，向所有客户端发送开始命令

## Y86 建立连接
点击开始连接后
每个client每3秒在指定的上端口广播,表明自己连接的状态和自己跑的阶段  
格式
```json
{
    "id":2,
    "FLevel":true,
    "DLevel":true,
    "ELevel":true,
    "MLevel":true,
    "WLevel":false,
    "pool":255
}
```
master检测所有连接是否完成，完成以后发送广播开始
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