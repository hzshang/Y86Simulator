## Y86 Pipeline 
## �ص�
- ����׶��������  
- ʹudp��tcpͨ��  

## ע�� 
ÿ��ʱ�����ڣ�ÿ����ˮ�ߴӻ�������ȡ��ִ��ָ�� �������д����һ����ˮ�ߵĻ�����

## Fetch  
�Ƚ���pcUpdate  
��ָ��洢����ȡ������ֵ������д��D��  

## Decode  
���8���Ĵ���
��ȡ�Ĵ���������ð�գ�����д��E��  

## Execute  
ALU���㣬����д��M��  
## Memory  
���memory
��д�ڴ棬д��WriteBack��  

## Writeback  
д�ؼĴ���

## ʱ���߳�
��master��Ӧ�ó�����
�ȴ�����׶η���������ź�
Ȼ���ٵȴ��̶�ʱ�䣬����ʱ���źš�

## ���е�tcp����
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
## ��ʱ�ӵ�����  

client|server
------|------
fetch|clock
decode|clock
execute|clock
memory|clock
writeback|clock


## Y86��TCP����
��pool��32λ����ָ��λ��Ϊ1����master��⣬���ǰ8λȫΪ1�������пͻ��˷��Ϳ�ʼ����

## Y86 ��������
�����ʼ���Ӻ�
ÿ��clientÿ3����ָ�����϶˿ڹ㲥,�����Լ����ӵ�״̬���Լ��ܵĽ׶�  
��ʽ
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
master������������Ƿ���ɣ�����Ժ��͹㲥��ʼ
```json
{
    "id":3   
}
```
��ʼ֮�����ÿ���׶ε��Ƿ���ɣ������ɹ㲥��һ�����ڽ���
```json
{
    "id":4,
    "step":23
}
```