#pragma once
#include "ProtoParse.h"
#include "ProtobuffMessages.h"

#define _gc2ch MatchmakingGC2ClientHello
#define _pci PlayerCommendationInfo
#define _pri PlayerRankingInfo

std::string profile_info_changer(void* pubDest, const uint32_t* pcubMsgSize);