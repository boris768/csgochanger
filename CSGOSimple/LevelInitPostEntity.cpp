#include "Hooks.hpp"
#include "Utils.hpp"
#include "Menu.h"
#include "XorStr.hpp"


namespace Hooks
{
	void __fastcall Hooked_LevelInitPostEntity(void* ecx, void* /*edx*/)
	{
		g_fnOriginalLevelInitPostEntity(ecx);
		g_ctx.local_player_index = SourceEngine::interfaces.Engine()->GetLocalPlayer();
		g_ctx.local_player = static_cast<C_CSPlayer*>(SourceEngine::interfaces.EntityList()->GetClientEntity(g_ctx.local_player_index));
		SourceEngine::interfaces.Engine()->GetPlayerInfo(g_ctx.local_player_index, &g_ctx.player_info);
		const auto* const default_sky = static_cast<SourceEngine::ConVar*>(Hooks::g_allCvars->at(FNV("sv_skyname")))->GetString();
		if (default_sky)
		{
			strcpy_s(currentloadedsky, 128, default_sky);
		}
		else
		{
			strcpy_s(currentloadedsky, 128, XorStr("sky_urb01"));
		}
		original_sequences[0] = SourceEngine::ACT_VM_DRAW;
		original_sequences[1] = SourceEngine::ACT_VM_IDLE;
		original_sequences[2] = SourceEngine::ACT_VM_IDLE;
		original_sequences[3] = SourceEngine::ACT_VM_MISSCENTER;
		original_sequences[4] = SourceEngine::ACT_VM_MISSCENTER;
		original_sequences[5] = SourceEngine::ACT_VM_HITCENTER;
		original_sequences[6] = SourceEngine::ACT_VM_HITCENTER;
		original_sequences[7] = SourceEngine::ACT_VM_SWINGHIT;
		original_sequences[8] = SourceEngine::ACT_VM_SWINGHIT;
		original_sequences[9] = SourceEngine::ACT_VM_MISSCENTER2;
		original_sequences[10] = SourceEngine::ACT_VM_HITCENTER2;
		original_sequences[11] = SourceEngine::ACT_VM_SWINGHARD;
		original_sequences[12] = SourceEngine::ACT_VM_IDLE_LOWERED;
				
		//using GetSequenceActivity_hdr_t = int(__fastcall*)(SourceEngine::studiohdr_t*, int, int*);
		//auto gsa = reinterpret_cast<GetSequenceActivity_hdr_t>(Utils::dwGetCallAddress(Utils::FindSignature(SourceEngine::memory.client(), XorStr("E8 ? ? ? ? 83 C4 04 3D ? ? ? ? 75 15"))));

		//auto default_knife_index = SourceEngine::interfaces.ModelInfo()->GetModelIndex("models/weapons/v_t_knife_anim.mdl");
		//auto default_knife = SourceEngine::interfaces.ModelInfo()->GetModel(default_knife_index);
		
		//auto handle = SourceEngine::interfaces.FileSystem()->Open("models/weapons/v_t_knife_anim.mdl", "r");
		//auto file_size = SourceEngine::interfaces.FileSystem()->Size(handle);

		//auto data = new char[file_size + 1];
		//SourceEngine::interfaces.FileSystem()->Read(data, file_size, handle);
		//
		//SourceEngine::studiohdr_t* default_knife_hdr = (SourceEngine::studiohdr_t*)data;//SourceEngine::interfaces.ModelInfo()->hack_GetStudioModel(default_knife);
		//                                               

		//struct bad_studiohdr_t
		//{
		//	char pad1[0xc];
		//	char name[64];
		//	char pad2[0x68];
		//	int32_t numlocalanim;
		//	int32_t localanimindex;
		//	int32_t numlocalseq; // sequences
		//	int32_t localseqindex;
		//};
		//
		//bad_studiohdr_t* tt =(bad_studiohdr_t*)default_knife_hdr;

		//SourceEngine::mstudioanimdesc_t* anim_desc = (SourceEngine::mstudioanimdesc_t*)((char*)tt + tt->localanimindex);
		//SourceEngine::mstudioseqdesc_t* seq_desc = (SourceEngine::mstudioseqdesc_t*)((char*)tt + tt->localseqindex);

		//for (int i = 0; i < tt->numlocalseq; ++i)
		//{
		//	SourceEngine::mstudioseqdesc_t* current = &seq_desc[i];
		//	const char* name = current->pszActivityName();
		//	original_sequences[i] = current->activity;
		//	changer_log->msg(std::to_string(i));
		//	changer_log->msg(" : ");
		//	changer_log->msg(std::to_string(original_sequences[i]));
		//	changer_log->msg(" : ");
		//	changer_log->msg(name);
		//	changer_log->msg("\n");
		//}

		//for (int i = 0; i < tt->numlocalanim; ++i)
		//{
		//	SourceEngine::mstudioanimdesc_t* current = &anim_desc[i];
		//	const char* name = current->pszName();
		//	changer_log->msg(std::to_string(i));
		//	changer_log->msg(" : ");
		//	changer_log->msg(name);
		//	changer_log->msg("\n");
		//}
		//delete[] data;
	}
}
