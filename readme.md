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
Fetch|Execute|8
## 和时钟的连接  

client|server
------|------
fetch|clock
decode|clock
execute|clock
memory|clock
writeback|clock



