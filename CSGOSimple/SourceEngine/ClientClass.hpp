#pragma once

#include "Recv.hpp"
#include "IClientNetworkable.hpp"

namespace SourceEngine
{
    class ClientClass;

    typedef IClientNetworkable*   (_cdecl *CreateClientClassFn)(int entnum, int serialNum);
    typedef IClientNetworkable*   (_cdecl *CreateEventFn)();

    class ClientClass
    {
    public:
        CreateClientClassFn      m_pCreateFn;
        CreateEventFn            m_pCreateEventFn;
        char*                    m_pNetworkName;
        RecvTable*               m_pRecvTable;
        ClientClass*             m_pNext;
        EClassIds                m_ClassID;
    };
}