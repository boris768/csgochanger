#pragma once

namespace SourceEngine
{
	class KeyValues;

	class IGameTypes
	{
	public:
		virtual ~IGameTypes() {}
		virtual bool Initialize(bool force) = 0;
		virtual bool IsInitialized() const = 0;

		virtual bool SetGameTypeAndMode(const char* gameType, const char* gameMode) = 0;
		virtual bool GetGameTypeAndModeFromAlias(const char* alias, int& gameType, int& gameMode) = 0;

		virtual bool SetGameTypeAndMode(int gameType, int gameMode) = 0;

		virtual void SetAndParseExtendedServerInfo(void* pExtendedServerInfo) = 0;

		virtual bool CheckShouldSetDefaultGameModeAndType(const char* mapName) = 0;

		virtual int GetCurrentGameType() const = 0;
		virtual int GetCurrentGameMode() const = 0;

		virtual const char* GetCurrentMapName() = 0;

		virtual const char* GetCurrentGameTypeNameID() = 0;
		virtual const char* GetCurrentGameModeNameID() = 0;

		virtual void ApplyConvarsForCurrentMode(bool) = 0;
		virtual void DisplayConvarsForCurrentMode() = 0;
		virtual void GetWeaponProgressionForCurrentModeCT() = 0;
		virtual void GetWeaponProgressionForCurrentModeT() = 0;
		virtual void GetNoResetVoteThresholdForCurrentModeCT() = 0;
		virtual void GetNoResetVoteThresholdForCurrentModeT() = 0;
		virtual void GetGameTypeFromInt(int) = 0;
		virtual void GetGameModeFromInt(int, int) = 0;
		virtual void GetGameModeAndTypeIntsFromStrings(char const*, char const*, int&, int&) = 0;
		virtual void GetGameModeAndTypeNameIdsFromStrings(char const*, char const*, char const*&, char const*&) = 0;
		virtual void CreateOrUpdateWorkshopMapGroup(char const*, /*CUtlStringList& */void*) = 0;
		virtual void IsWorkshopMapGroup(char const*) = 0;
		virtual void GetRandomMapGroup(char const*, char const*) = 0;
		virtual void GetFirstMap(char const*) = 0;
		virtual void GetRandomMap(char const*) = 0;
		virtual void GetNextMap(char const*, char const*) = 0;
		virtual void GetMaxPlayersForTypeAndMode(int, int) = 0;
		virtual void IsValidMapGroupName(char const*) = 0;
		virtual void IsValidMapInMapGroup(char const*, char const*) = 0;
		virtual void IsValidMapGroupForTypeAndMode(char const*, char const*, char const*) = 0;
		virtual void ApplyConvarsForMap(char const*, bool) = 0;
		virtual void GetMapInfo(char const*, unsigned int&) = 0;
		virtual void* GetTModelsForMap(char const*) = 0;
		virtual void* GetCTModelsForMap(char const*) = 0;
		virtual void GetHostageModelsForMap(char const*) = 0;
		virtual void GetDefaultGameTypeForMap(char const*) = 0;
		virtual void GetDefaultGameModeForMap(char const*) = 0;
		virtual void GetTViewModelArmsForMap(char const*) = 0;
		virtual void GetCTViewModelArmsForMap(char const*) = 0;
		virtual void GetRequiredAttrForMap(char const*) = 0;
		virtual void GetRequiredAttrValueForMap(char const*) = 0;
		virtual void GetRequiredAttrRewardForMap(char const*) = 0;
		virtual void GetRewardDropListForMap(char const*) = 0;
		virtual void GetMapGroupMapList(char const*) = 0;
		virtual void GetRunMapWithDefaultGametype(void) = 0;
		virtual void SetRunMapWithDefaultGametype(bool) = 0;
		virtual void GetLoadingScreenDataIsCorrect(void) = 0;
		virtual void SetLoadingScreenDataIsCorrect(bool) = 0;
		virtual void SetCustomBotDifficulty(int) = 0;
		virtual void GetCustomBotDifficulty(void) = 0;
		virtual void GetCurrentServerNumSlots(void) = 0;
		virtual void GetCurrentServerSettingInt(char const*, int) = 0;
		virtual void GetGameTypeFromMode(char const*, char const*&) = 0;
		virtual void LoadMapEntry(KeyValues*) = 0;
	};
}