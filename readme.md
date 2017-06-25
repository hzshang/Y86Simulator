Y86 Simulator Net
===============
 
开发环境：QT 5.0  


## 特点
- 五个阶段随意组合,在一个局域网里面多个客户端通过UDP协议广播,自动连接     
- 每个阶段均在独立的线程里，线程之间的通信使用TCP协议传送数据  

## 主体架构 
- 程序分为clock、fetch、decode、execute、memory、writeback 六个线程  
- 每个时钟周期开始时，clock向每个阶段发送TCP数据，代表时钟信号上升沿  
- 收到clock信号后，每个阶段并发执行所在阶段的工作  
- 各阶段执行完毕后向clock发送TCP数据，等待clock的下一个信号


## Fetch  
先进行pcUpdate  
从指令存储器中取出各个值，将其写到D层  

## Decode  
存放8个寄存器
读取寄存器，处理冒险，将其写到E层  
需要等待Execute,Memory,Writeback阶段发送数据

## Execute  
ALU计算，将其写到M层  

## Memory  
存放memory
读写内存，写到WriteBack层  

## Writeback  
写回寄存器

## 时钟线程
和fetch线程在同一个客户端  
向五个阶段发送时钟信号  
等待五个阶段发送已完成信号
然后再等待固定时间，再次发送时钟信号。

## Pipeline各阶段的TCP连接
client|server|cid
------|------|---
Fetch|WriteBack|0
Fetch|Memory|1
Fetch|Decode|2
Decode|Execute|3
Decode|Memory|4
Decode|Writeback|5
Execute|Memory|6
Memory|Writeback|7
Fetch|Execute|8


## Pipeline和时钟的TCP连接  
client|server|cid
------|------|---
fetch|clock|9
decode|clock|10
execute|clock|11
memory|clock|12
writeback|clock|13

