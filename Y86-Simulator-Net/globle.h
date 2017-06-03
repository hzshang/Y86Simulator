#ifndef GLOBLE_H
#define GLOBLE_H


class globle
{
public:
    globle();

    static int eax,ecx,ebx,edx,esp,ebp,esi,edi;//寄存器,按顺序从0～7标记
    static int AOK,HTL,ADZ,INS;//四种状态指令：AOK(正常),HTL(执行halt指令),ADR(非法地址)和INS(非法指令)
};

#endif // GLOBLE_H
