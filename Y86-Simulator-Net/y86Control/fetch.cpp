#include "fetch.h"
#include "const.h"
extern int broadcast;
Fetch::Fetch()
{
    init();
}

Fetch::~Fetch()
{

}

void Fetch::init()
{
    predPC=0;
    clientToDecode=NULL;
    clientToMemory=NULL;
    clientToWriteback=NULL;
}
void Fetch::dealDecodeData()
{

}

void Fetch::dealMemoryData()
{

}

void Fetch::dealWritebackData()
{

}
