#pragma once
#include "SourceEngine/IClientEntity.hpp"
#include "SourceEngine/Vector.hpp"
#include "SourceEngine/UtlVector.hpp"

// Handy defines for EmitSound
#define SOUND_FROM_UI_PANEL			-2		// Sound being played inside a UI panel on the client
#define SOUND_FROM_LOCAL_PLAYER		-1
#define SOUND_FROM_WORLD			0



// These are used to feed a soundlevel to the sound system and have it use
// goldsrc-type attenuation. We should use this as little as possible and 
// phase it out as soon as possible.

// Take a regular sndlevel and convert it to compatibility mode.
#define SNDLEVEL_TO_COMPATIBILITY_MODE( x )		((soundlevel_t)(int)( (x) + 256 ))

// Take a compatibility-mode sndlevel and get the REAL sndlevel out of it.
#define SNDLEVEL_FROM_COMPATIBILITY_MODE( x )	((soundlevel_t)(int)( (x) - 256 ))

// Tells if the given sndlevel is marked as compatibility mode.
#define SNDLEVEL_IS_COMPATIBILITY_MODE( x ) ( (x) >= 256 )

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() {}

	virtual bool	IsReliable(void) const = 0;
	virtual bool	IsInitMessage(void) const = 0;

	virtual int		GetRecipientCount(void) const = 0;
	virtual int		GetRecipientIndex(int slot) const = 0;
};


namespace SourceEngine
{
	struct SndInfo_t
	{
		// Sound Guid
		int			m_nGuid;
		int m_filenameHandle;		// filesystem filename handle - call IFilesystem to conver this to a string
		int			m_nSoundSource;
		int			m_nChannel;
		// If a sound is being played through a speaker entity (e.g., on a monitor,), this is the
		//  entity upon which to show the lips moving, if the sound has sentence data
		int			m_nSpeakerEntity;
		float		m_flVolume;
		float		m_flLastSpatializedVolume;
		// Radius of this sound effect (spatialization is different within the radius)
		float		m_flRadius;
		int			m_nPitch;
		Vector		*m_pOrigin;
		Vector		*m_pDirection;

		// if true, assume sound source can move and update according to entity
		bool		m_bUpdatePositions;
		// true if playing linked sentence
		bool		m_bIsSentence;
		// if true, bypass all dsp processing for this sound (ie: music)	
		bool		m_bDryMix;
		// true if sound is playing through in-game speaker entity.
		bool		m_bSpeaker;
		// true if sound is playing with special DSP effect
		bool		m_bSpecialDSP;
		// for snd_show, networked sounds get colored differently than local sounds
		bool		m_bFromServer;
	};

	struct StartSoundParams_t
	{ //-V802
		bool			staticsound;
		int			userdata;
		int			soundsource;
		int			entchannel;
		void		*pSfx;
		Vector		origin;
		Vector		direction;
		bool			bUpdatePositions;
		float			fvol;
		soundlevel_t	soundlevel;
		int			flags;
		int			pitch;
		bool			fromserver;
		float			delay;
		int			speakerentity;
		bool			suppressrecording;
		int			initialStreamPosition;
	};

	class IEngineSound
	{
	public:
		// Precache a particular sample
		virtual bool PrecacheSound(const char *pSample, bool bPreload = false, bool bIsUISound = false) = 0;
		virtual bool IsSoundPrecached(const char *pSample) = 0;
		virtual void PrefetchSound(const char *pSample) = 0;
		virtual bool IsLoopingSound(char const*) = 0;
		// Just loads the file header and checks for duration (not hooked up for .mp3's yet)
		// Is accessible to server and client though
		virtual float GetSoundDuration(const char *pSample) = 0;

		// Pitch of 100 is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
		// down to 1 is a lower pitch.   150 to 70 is the realistic range.
		// EmitSound with pitch != 100 should be used sparingly, as it's not quite as
		// fast (the pitchshift mixer is not native coded).

		// NOTE: setting iEntIndex to -1 will cause the sound to be emitted from the local
		// player (client-side only)
		virtual void EmitSound(IRecipientFilter* filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
			const Vector *pOrigin, const Vector *pDirection, Vector * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, StartSoundParams_t& params) = 0;
		
			virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
			float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = 0, int iSpecialDSP = 0,
			const Vector *pOrigin = nullptr, const Vector *pDirection = nullptr, CUtlVector< Vector >* pUtlVecOrigins = nullptr, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1) = 0;

		virtual void EmitSentenceByIndex(IRecipientFilter& filter, int iEntIndex, int iChannel, int iSentenceIndex,
			float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = 0, int iSpecialDSP = 0,
			const Vector *pOrigin = nullptr, const Vector *pDirection = nullptr, CUtlVector< Vector >* pUtlVecOrigins = nullptr, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1) = 0;

		virtual void StopSound(int iEntIndex, int iChannel, const char *pSample) = 0;

		// stop all active sounds (client only)
		virtual void StopAllSounds(bool bClearBuffers) = 0;

		// Set the room type for a player (client only)
		virtual void SetRoomType(IRecipientFilter& filter, int roomType) = 0;

		// Set the dsp preset for a player (client only)
		virtual void SetPlayerDSP(IRecipientFilter& filter, int dspType, bool fastReset) = 0;

		// emit an "ambient" sound that isn't spatialized
		// only available on the client, assert on server
		virtual void EmitAmbientSound(const char *pSample, float flVolume, int iPitch = 0, int flags = 0, float soundtime = 0.0f) = 0;


		//	virtual EntChannel_t	CreateEntChannel() = 0;

		virtual float GetDistGainFromSoundLevel(soundlevel_t soundlevel, float dist) = 0;

		// Client .dll only functions
		virtual int		GetGuidForLastSoundEmitted() = 0;
		virtual bool	IsSoundStillPlaying(int guid) = 0;
		virtual void	StopSoundByGuid(int guid) = 0;
		// Set's master volume (0.0->1.0)
		virtual void	SetVolumeByGuid(int guid, float fvol) = 0;

		// Retrieves list of all active sounds
		virtual void	GetActiveSounds(CUtlVector< SndInfo_t >& sndlist) = 0;

		virtual void	PrecacheSentenceGroup(const char *pGroupName) = 0;
		virtual void	NotifyBeginMoviePlayback() = 0;
		virtual void	NotifyEndMoviePlayback() = 0;
	};
}