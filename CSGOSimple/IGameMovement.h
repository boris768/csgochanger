#pragma once

namespace SourceEngine
{
	class IGameMovement {
	public:
		virtual			~IGameMovement() {}

		virtual void	ProcessMovement(void* pPlayer, SourceEngine::CMoveData *pMove) = 0;
		virtual void	Reset() = 0;
		virtual void	StartTrackPredictionErrors(void* pPlayer) = 0;
		virtual void	FinishTrackPredictionErrors(void* pPlayer) = 0;
		virtual void	DiffPrint(char const *fmt, ...) = 0;

		virtual SourceEngine::Vector const&	GetPlayerMins(bool ducked) const = 0;
		virtual SourceEngine::Vector const&	GetPlayerMaxs(bool ducked) const = 0;
		virtual SourceEngine::Vector const& GetPlayerViewOffset(bool ducked) const = 0;

		virtual bool			IsMovingPlayerStuck() const = 0;
		virtual void*			GetMovingPlayer() const = 0;
		virtual void			UnblockPusher(void* pPlayer, void* pPusher) = 0;

		virtual void SetupMovementBounds(SourceEngine::CMoveData *pMove) = 0;
	};
}