#pragma once
#include <utility>
#include "ProtoParse.h"

constexpr auto k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve = 9107;
constexpr auto k_EMsgGCClientWelcome = 4004;
constexpr auto k_EMsgGCClientHello = 4006;
constexpr auto k_EMsgGCAdjustItemEquippedState = 1059;
constexpr auto k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello = 9109;
constexpr auto k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello = 9110;

//FORMAT: constexpr static Tag field_name = {field_id, field_type};

struct CMsgClientHello : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 8;

	CMsgClientHello() : ProtoWriter(MAX_FIELD)
	{
	}

	CMsgClientHello(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	CMsgClientHello(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag client_session_need = {3, TYPE_UINT32};
	void clear_client_session_need() { this->clear(client_session_need); }
	bool has_client_session_need() { return this->has(client_session_need); }
	field get_client_session_need() { return this->get(client_session_need); }
	std::vector<field> getAll_client_session_need() { return this->getAll(client_session_need); }
	void add_client_session_need(const std::string& v) { this->add(client_session_need, v); }

	template <typename T>
	void add_client_session_need(T v) { this->add(client_session_need, v); }

	void replace_client_session_need(const std::string& v) { this->replace(client_session_need, v); }

	void replace_client_session_need(const std::string& v, uint32_t index)
	{
		this->replace(client_session_need, v, index);
	}

	template <typename T>
	void replace_client_session_need(T v) { this->replace(client_session_need, v); }

	template <typename T>
	void replace_client_session_need(T v, uint32_t index) { this->replace(client_session_need, v, index); }

	template <class T>
	T get_client_session_need() { return std::move(T(this->get(client_session_need).String())); }
};


// ProfileChanger
struct MatchmakingGC2ClientHello : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 20;

	MatchmakingGC2ClientHello() : ProtoWriter(MAX_FIELD)
	{
	}

	MatchmakingGC2ClientHello(void* data, const size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	MatchmakingGC2ClientHello(const std::string& data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag ranking = {7, TYPE_STRING};
	void clear_ranking() { this->clear(ranking); }
	bool has_ranking() { return this->has(ranking); }
	field get_ranking() { return this->get(ranking); }
	std::vector<field> getAll_ranking() { return this->getAll(ranking); }
	void add_ranking(const std::string& v) { this->add(ranking, v); }

	template <typename T>
	void add_ranking(T v) { this->add(ranking, v); }

	void replace_ranking(const std::string& v) { this->replace(ranking, v); }
	void replace_ranking(const std::string& v, uint32_t index) { this->replace(ranking, v, index); }

	template <typename T>
	void replace_ranking(T v) { this->replace(ranking, v); }

	template <typename T>
	void replace_ranking(T v, uint32_t index) { this->replace(ranking, v, index); }

	template <class T>
	T get_ranking() { return std::move(T(this->get(ranking).String())); }

	constexpr static Tag commendation = {8, TYPE_STRING};
	void clear_commendation() { this->clear(commendation); }
	bool has_commendation() { return this->has(commendation); }
	field get_commendation() { return this->get(commendation); }
	std::vector<field> getAll_commendation() { return this->getAll(commendation); }
	void add_commendation(const std::string& v) { this->add(commendation, v); }

	template <typename T>
	void add_commendation(T v) { this->add(commendation, v); }

	void replace_commendation(const std::string& v) { this->replace(commendation, v); }
	void replace_commendation(const std::string& v, uint32_t index) { this->replace(commendation, v, index); }

	template <typename T>
	void replace_commendation(T v) { this->replace(commendation, v); }

	template <typename T>
	void replace_commendation(T v, uint32_t index) { this->replace(commendation, v, index); }

	template <class T>
	T get_commendation() { return std::move(T(this->get(commendation).String())); }

	constexpr static Tag player_level = {17, TYPE_INT32};
	void clear_player_level() { this->clear(player_level); }
	bool has_player_level() { return this->has(player_level); }
	field get_player_level() { return this->get(player_level); }
	std::vector<field> getAll_player_level() { return this->getAll(player_level); }
	void add_player_level(const std::string& v) { this->add(player_level, v); }

	template <typename T>
	void add_player_level(T v) { this->add(player_level, v); }

	void replace_player_level(const std::string& v) { this->replace(player_level, v); }
	void replace_player_level(const std::string& v, uint32_t index) { this->replace(player_level, v, index); }

	template <typename T>
	void replace_player_level(T v) { this->replace(player_level, v); }

	template <typename T>
	void replace_player_level(T v, uint32_t index) { this->replace(player_level, v, index); }

	template <class T>
	T get_player_level() { return std::move(T(this->get(player_level).String())); }
};

struct PlayerCommendationInfo : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 4;

	PlayerCommendationInfo() : ProtoWriter(MAX_FIELD)
	{
	}

	PlayerCommendationInfo(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	PlayerCommendationInfo(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag cmd_friendly = {1, TYPE_UINT32};
	void clear_cmd_friendly() { this->clear(cmd_friendly); }
	bool has_cmd_friendly() { return this->has(cmd_friendly); }
	field get_cmd_friendly() { return this->get(cmd_friendly); }
	std::vector<field> getAll_cmd_friendly() { return this->getAll(cmd_friendly); }
	void add_cmd_friendly(const std::string& v) { this->add(cmd_friendly, v); }

	template <typename T>
	void add_cmd_friendly(T v) { this->add(cmd_friendly, v); }

	void replace_cmd_friendly(const std::string& v) { this->replace(cmd_friendly, v); }
	void replace_cmd_friendly(const std::string& v, uint32_t index) { this->replace(cmd_friendly, v, index); }

	template <typename T>
	void replace_cmd_friendly(T v) { this->replace(cmd_friendly, v); }

	template <typename T>
	void replace_cmd_friendly(T v, uint32_t index) { this->replace(cmd_friendly, v, index); }

	template <class T>
	T get_cmd_friendly() { return std::move(T(this->get(cmd_friendly).String())); }

	constexpr static Tag cmd_teaching = {2, TYPE_UINT32};
	void clear_cmd_teaching() { this->clear(cmd_teaching); }
	bool has_cmd_teaching() { return this->has(cmd_teaching); }
	field get_cmd_teaching() { return this->get(cmd_teaching); }
	std::vector<field> getAll_cmd_teaching() { return this->getAll(cmd_teaching); }
	void add_cmd_teaching(const std::string& v) { this->add(cmd_teaching, v); }

	template <typename T>
	void add_cmd_teaching(T v) { this->add(cmd_teaching, v); }

	void replace_cmd_teaching(const std::string& v) { this->replace(cmd_teaching, v); }
	void replace_cmd_teaching(const std::string& v, uint32_t index) { this->replace(cmd_teaching, v, index); }

	template <typename T>
	void replace_cmd_teaching(T v) { this->replace(cmd_teaching, v); }

	template <typename T>
	void replace_cmd_teaching(T v, uint32_t index) { this->replace(cmd_teaching, v, index); }

	template <class T>
	T get_cmd_teaching() { return std::move(T(this->get(cmd_teaching).String())); }

	constexpr static Tag cmd_leader = {4, TYPE_UINT32};
	void clear_cmd_leader() { this->clear(cmd_leader); }
	bool has_cmd_leader() { return this->has(cmd_leader); }
	field get_cmd_leader() { return this->get(cmd_leader); }
	std::vector<field> getAll_cmd_leader() { return this->getAll(cmd_leader); }
	void add_cmd_leader(const std::string& v) { this->add(cmd_leader, v); }

	template <typename T>
	void add_cmd_leader(T v) { this->add(cmd_leader, v); }

	void replace_cmd_leader(const std::string& v) { this->replace(cmd_leader, v); }
	void replace_cmd_leader(const std::string& v, uint32_t index) { this->replace(cmd_leader, v, index); }

	template <typename T>
	void replace_cmd_leader(T v) { this->replace(cmd_leader, v); }

	template <typename T>
	void replace_cmd_leader(T v, uint32_t index) { this->replace(cmd_leader, v, index); }

	template <class T>
	T get_cmd_leader() { return std::move(T(this->get(cmd_leader).String())); }
};

struct PlayerRankingInfo : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 6;

	PlayerRankingInfo() : ProtoWriter(MAX_FIELD)
	{
	}

	PlayerRankingInfo(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	PlayerRankingInfo(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag rank_id = {2, TYPE_UINT32};
	void clear_rank_id() { this->clear(rank_id); }
	bool has_rank_id() { return this->has(rank_id); }
	field get_rank_id() { return this->get(rank_id); }
	std::vector<field> getAll_rank_id() { return this->getAll(rank_id); }
	void add_rank_id(const std::string& v) { this->add(rank_id, v); }

	template <typename T>
	void add_rank_id(T v) { this->add(rank_id, v); }

	void replace_rank_id(const std::string& v) { this->replace(rank_id, v); }
	void replace_rank_id(const std::string& v, uint32_t index) { this->replace(rank_id, v, index); }

	template <typename T>
	void replace_rank_id(T v) { this->replace(rank_id, v); }

	template <typename T>
	void replace_rank_id(T v, uint32_t index) { this->replace(rank_id, v, index); }

	template <class T>
	T get_rank_id() { return std::move(T(this->get(rank_id).String())); }

	constexpr static Tag wins = {4, TYPE_UINT32};
	void clear_wins() { this->clear(wins); }
	bool has_wins() { return this->has(wins); }
	field get_wins() { return this->get(wins); }
	std::vector<field> getAll_wins() { return this->getAll(wins); }
	void add_wins(const std::string& v) { this->add(wins, v); }

	template <typename T>
	void add_wins(T v) { this->add(wins, v); }

	void replace_wins(const std::string& v) { this->replace(wins, v); }
	void replace_wins(const std::string& v, uint32_t index) { this->replace(wins, v, index); }

	template <typename T>
	void replace_wins(T v) { this->replace(wins, v); }

	template <typename T>
	void replace_wins(T v, uint32_t index) { this->replace(wins, v, index); }

	template <class T>
	T get_wins() { return std::move(T(this->get(wins).String())); }
};


// InvChanger
struct SubscribedType : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 2;

	SubscribedType() : ProtoWriter(MAX_FIELD)
	{
	}

	SubscribedType(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	SubscribedType(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag type_id = {1, TYPE_INT32};
	void clear_type_id() { this->clear(type_id); }
	bool has_type_id() { return this->has(type_id); }
	field get_type_id() { return this->get(type_id); }
	std::vector<field> getAll_type_id() { return this->getAll(type_id); }
	void add_type_id(const std::string& v) { this->add(type_id, v); }

	template <typename T>
	void add_type_id(T v) { this->add(type_id, v); }

	void replace_type_id(const std::string& v) { this->replace(type_id, v); }
	void replace_type_id(const std::string& v, uint32_t index) { this->replace(type_id, v, index); }

	template <typename T>
	void replace_type_id(T v) { this->replace(type_id, v); }

	template <typename T>
	void replace_type_id(T v, uint32_t index) { this->replace(type_id, v, index); }

	template <class T>
	T get_type_id() { return std::move(T(this->get(type_id).String())); }

	constexpr static Tag object_data = {2, TYPE_STRING};
	void clear_object_data() { this->clear(object_data); }
	bool has_object_data() { return this->has(object_data); }
	field get_object_data() { return this->get(object_data); }
	std::vector<field> getAll_object_data() { return this->getAll(object_data); }
	void add_object_data(const std::string& v) { this->add(object_data, v); }

	template <typename T>
	void add_object_data(T v) { this->add(object_data, v); }

	void replace_object_data(const std::string& v) { this->replace(object_data, v); }
	void replace_object_data(const std::string& v, uint32_t index) { this->replace(object_data, v, index); }

	template <typename T>
	void replace_object_data(T v) { this->replace(object_data, v); }

	template <typename T>
	void replace_object_data(T v, uint32_t index) { this->replace(object_data, v, index); }

	template <class T>
	T get_object_data() { return std::move(T(this->get(object_data).String())); }
};

struct CMsgSOCacheSubscribed : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 4;

	CMsgSOCacheSubscribed() : ProtoWriter(MAX_FIELD)
	{
	}

	CMsgSOCacheSubscribed(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	CMsgSOCacheSubscribed(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag objects = {2, TYPE_STRING};
	void clear_objects() { this->clear(objects); }
	bool has_objects() { return this->has(objects); }
	field get_objects() { return this->get(objects); }
	std::vector<field> getAll_objects() { return this->getAll(objects); }
	void add_objects(const std::string& v) { this->add(objects, v); }

	template <typename T>
	void add_objects(T v) { this->add(objects, v); }

	void replace_objects(const std::string& v) { this->replace(objects, v); }
	void replace_objects(const std::string& v, uint32_t index) { this->replace(objects, v, index); }

	template <typename T>
	void replace_objects(T v) { this->replace(objects, v); }

	template <typename T>
	void replace_objects(T v, uint32_t index) { this->replace(objects, v, index); }

	template <class T>
	T get_objects() { return std::move(T(this->get(objects).String())); }
};

struct CMsgClientWelcome : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 11;

	CMsgClientWelcome() : ProtoWriter(MAX_FIELD)
	{
	}

	CMsgClientWelcome(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	CMsgClientWelcome(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag outofdate_subscribed_caches = {3, TYPE_STRING};
	void clear_outofdate_subscribed_caches() { this->clear(outofdate_subscribed_caches); }
	bool has_outofdate_subscribed_caches() { return this->has(outofdate_subscribed_caches); }
	field get_outofdate_subscribed_caches() { return this->get(outofdate_subscribed_caches); }
	std::vector<field> getAll_outofdate_subscribed_caches() { return this->getAll(outofdate_subscribed_caches); }
	void add_outofdate_subscribed_caches(const std::string& v) { this->add(outofdate_subscribed_caches, v); }

	template <typename T>
	void add_outofdate_subscribed_caches(T v) { this->add(outofdate_subscribed_caches, v); }

	void replace_outofdate_subscribed_caches(const std::string& v) { this->replace(outofdate_subscribed_caches, v); }

	void replace_outofdate_subscribed_caches(const std::string& v, uint32_t index)
	{
		this->replace(outofdate_subscribed_caches, v, index);
	}

	template <typename T>
	void replace_outofdate_subscribed_caches(T v) { this->replace(outofdate_subscribed_caches, v); }

	template <typename T>
	void replace_outofdate_subscribed_caches(T v, uint32_t index)
	{
		this->replace(outofdate_subscribed_caches, v, index);
	}

	template <class T>
	T get_outofdate_subscribed_caches() { return std::move(T(this->get(outofdate_subscribed_caches).String())); }
};

struct CSOEconItem : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 19;

	CSOEconItem() : ProtoWriter(MAX_FIELD)
	{
	}

	CSOEconItem(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	CSOEconItem(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag id = {1, TYPE_UINT64};
	void clear_id() { this->clear(id); }
	bool has_id() { return this->has(id); }
	field get_id() { return this->get(id); }
	std::vector<field> getAll_id() { return this->getAll(id); }
	void add_id(const std::string& v) { this->add(id, v); }

	template <typename T>
	void add_id(T v) { this->add(id, v); }

	void replace_id(const std::string& v) { this->replace(id, v); }
	void replace_id(const std::string& v, uint32_t index) { this->replace(id, v, index); }

	template <typename T>
	void replace_id(T v) { this->replace(id, v); }

	template <typename T>
	void replace_id(T v, uint32_t index) { this->replace(id, v, index); }

	template <class T>
	T get_id() { return std::move(T(this->get(id).String())); }

	constexpr static Tag account_id = {2, TYPE_UINT32};
	void clear_account_id() { this->clear(account_id); }
	bool has_account_id() { return this->has(account_id); }
	field get_account_id() { return this->get(account_id); }
	std::vector<field> getAll_account_id() { return this->getAll(account_id); }
	void add_account_id(const std::string& v) { this->add(account_id, v); }

	template <typename T>
	void add_account_id(T v) { this->add(account_id, v); }

	void replace_account_id(const std::string& v) { this->replace(account_id, v); }
	void replace_account_id(const std::string& v, uint32_t index) { this->replace(account_id, v, index); }

	template <typename T>
	void replace_account_id(T v) { this->replace(account_id, v); }

	template <typename T>
	void replace_account_id(T v, uint32_t index) { this->replace(account_id, v, index); }

	template <class T>
	T get_account_id() { return std::move(T(this->get(account_id).String())); }

	constexpr static Tag inventory = {3, TYPE_UINT32};
	void clear_inventory() { this->clear(inventory); }
	bool has_inventory() { return this->has(inventory); }
	field get_inventory() { return this->get(inventory); }
	std::vector<field> getAll_inventory() { return this->getAll(inventory); }
	void add_inventory(const std::string& v) { this->add(inventory, v); }

	template <typename T>
	void add_inventory(T v) { this->add(inventory, v); }

	void replace_inventory(const std::string& v) { this->replace(inventory, v); }
	void replace_inventory(const std::string& v, uint32_t index) { this->replace(inventory, v, index); }

	template <typename T>
	void replace_inventory(T v) { this->replace(inventory, v); }

	template <typename T>
	void replace_inventory(T v, uint32_t index) { this->replace(inventory, v, index); }

	template <class T>
	T get_inventory() { return std::move(T(this->get(inventory).String())); }

	constexpr static Tag def_index = {4, TYPE_UINT32};
	void clear_def_index() { this->clear(def_index); }
	bool has_def_index() { return this->has(def_index); }
	field get_def_index() { return this->get(def_index); }
	std::vector<field> getAll_def_index() { return this->getAll(def_index); }
	void add_def_index(const std::string& v) { this->add(def_index, v); }

	template <typename T>
	void add_def_index(T v) { this->add(def_index, v); }

	void replace_def_index(const std::string& v) { this->replace(def_index, v); }
	void replace_def_index(const std::string& v, uint32_t index) { this->replace(def_index, v, index); }

	template <typename T>
	void replace_def_index(T v) { this->replace(def_index, v); }

	template <typename T>
	void replace_def_index(T v, uint32_t index) { this->replace(def_index, v, index); }

	template <class T>
	T get_def_index() { return std::move(T(this->get(def_index).String())); }

	constexpr static Tag quantity = {5, TYPE_UINT32};
	void clear_quantity() { this->clear(quantity); }
	bool has_quantity() { return this->has(quantity); }
	field get_quantity() { return this->get(quantity); }
	std::vector<field> getAll_quantity() { return this->getAll(quantity); }
	void add_quantity(const std::string& v) { this->add(quantity, v); }

	template <typename T>
	void add_quantity(T v) { this->add(quantity, v); }

	void replace_quantity(const std::string& v) { this->replace(quantity, v); }
	void replace_quantity(const std::string& v, uint32_t index) { this->replace(quantity, v, index); }

	template <typename T>
	void replace_quantity(T v) { this->replace(quantity, v); }

	template <typename T>
	void replace_quantity(T v, uint32_t index) { this->replace(quantity, v, index); }

	template <class T>
	T get_quantity() { return std::move(T(this->get(quantity).String())); }

	constexpr static Tag level = {6, TYPE_UINT32};
	void clear_level() { this->clear(level); }
	bool has_level() { return this->has(level); }
	field get_level() { return this->get(level); }
	std::vector<field> getAll_level() { return this->getAll(level); }
	void add_level(const std::string& v) { this->add(level, v); }

	template <typename T>
	void add_level(T v) { this->add(level, v); }

	void replace_level(const std::string& v) { this->replace(level, v); }
	void replace_level(const std::string& v, uint32_t index) { this->replace(level, v, index); }

	template <typename T>
	void replace_level(T v) { this->replace(level, v); }

	template <typename T>
	void replace_level(T v, uint32_t index) { this->replace(level, v, index); }

	template <class T>
	T get_level() { return std::move(T(this->get(level).String())); }

	constexpr static Tag quality = {7, TYPE_UINT32};
	void clear_quality() { this->clear(quality); }
	bool has_quality() { return this->has(quality); }
	field get_quality() { return this->get(quality); }
	std::vector<field> getAll_quality() { return this->getAll(quality); }
	void add_quality(const std::string& v) { this->add(quality, v); }

	template <typename T>
	void add_quality(T v) { this->add(quality, v); }

	void replace_quality(const std::string& v) { this->replace(quality, v); }
	void replace_quality(const std::string& v, uint32_t index) { this->replace(quality, v, index); }

	template <typename T>
	void replace_quality(T v) { this->replace(quality, v); }

	template <typename T>
	void replace_quality(T v, uint32_t index) { this->replace(quality, v, index); }

	template <class T>
	T get_quality() { return std::move(T(this->get(quality).String())); }

	constexpr static Tag flags = {8, TYPE_UINT32};
	void clear_flags() { this->clear(flags); }
	bool has_flags() { return this->has(flags); }
	field get_flags() { return this->get(flags); }
	std::vector<field> getAll_flags() { return this->getAll(flags); }
	void add_flags(const std::string& v) { this->add(flags, v); }

	template <typename T>
	void add_flags(T v) { this->add(flags, v); }

	void replace_flags(const std::string& v) { this->replace(flags, v); }
	void replace_flags(const std::string& v, uint32_t index) { this->replace(flags, v, index); }

	template <typename T>
	void replace_flags(T v) { this->replace(flags, v); }

	template <typename T>
	void replace_flags(T v, uint32_t index) { this->replace(flags, v, index); }

	template <class T>
	T get_flags() { return std::move(T(this->get(flags).String())); }

	constexpr static Tag origin = {9, TYPE_UINT32};
	void clear_origin() { this->clear(origin); }
	bool has_origin() { return this->has(origin); }
	field get_origin() { return this->get(origin); }
	std::vector<field> getAll_origin() { return this->getAll(origin); }
	void add_origin(const std::string& v) { this->add(origin, v); }

	template <typename T>
	void add_origin(T v) { this->add(origin, v); }

	void replace_origin(const std::string& v) { this->replace(origin, v); }
	void replace_origin(const std::string& v, uint32_t index) { this->replace(origin, v, index); }

	template <typename T>
	void replace_origin(T v) { this->replace(origin, v); }

	template <typename T>
	void replace_origin(T v, uint32_t index) { this->replace(origin, v, index); }

	template <class T>
	T get_origin() { return std::move(T(this->get(origin).String())); }

	constexpr static Tag custom_name = {10, TYPE_STRING};
	void clear_custom_name() { this->clear(custom_name); }
	bool has_custom_name() { return this->has(custom_name); }
	field get_custom_name() { return this->get(custom_name); }
	std::vector<field> getAll_custom_name() { return this->getAll(custom_name); }
	void add_custom_name(const std::string& v) { this->add(custom_name, v); }

	template <typename T>
	void add_custom_name(T v) { this->add(custom_name, v); }

	void replace_custom_name(const std::string& v) { this->replace(custom_name, v); }
	void replace_custom_name(const std::string& v, uint32_t index) { this->replace(custom_name, v, index); }

	template <typename T>
	void replace_custom_name(T v) { this->replace(custom_name, v); }

	template <typename T>
	void replace_custom_name(T v, uint32_t index) { this->replace(custom_name, v, index); }

	template <class T>
	T get_custom_name() { return std::move(T(this->get(custom_name).String())); }

	constexpr static Tag attribute = {12, TYPE_STRING};
	void clear_attribute() { this->clear(attribute); }
	bool has_attribute() { return this->has(attribute); }
	field get_attribute() { return this->get(attribute); }
	std::vector<field> getAll_attribute() { return this->getAll(attribute); }
	void add_attribute(const std::string& v) { this->add(attribute, v); }

	template <typename T>
	void add_attribute(T v) { this->add(attribute, v); }

	void replace_attribute(const std::string& v) { this->replace(attribute, v); }
	void replace_attribute(const std::string& v, uint32_t index) { this->replace(attribute, v, index); }

	template <typename T>
	void replace_attribute(T v) { this->replace(attribute, v); }

	template <typename T>
	void replace_attribute(T v, uint32_t index) { this->replace(attribute, v, index); }

	template <class T>
	T get_attribute() { return std::move(T(this->get(attribute).String())); }

	constexpr static Tag in_use = {14, TYPE_BOOL};
	void clear_in_use() { this->clear(in_use); }
	bool has_in_use() { return this->has(in_use); }
	field get_in_use() { return this->get(in_use); }
	std::vector<field> getAll_in_use() { return this->getAll(in_use); }
	void add_in_use(const std::string& v) { this->add(in_use, v); }

	template <typename T>
	void add_in_use(T v) { this->add(in_use, v); }

	void replace_in_use(const std::string& v) { this->replace(in_use, v); }
	void replace_in_use(const std::string& v, uint32_t index) { this->replace(in_use, v, index); }

	template <typename T>
	void replace_in_use(T v) { this->replace(in_use, v); }

	template <typename T>
	void replace_in_use(T v, uint32_t index) { this->replace(in_use, v, index); }

	template <class T>
	T get_in_use() { return std::move(T(this->get(in_use).String())); }

	constexpr static Tag style = {15, TYPE_UINT32};
	void clear_style() { this->clear(style); }
	bool has_style() { return this->has(style); }
	field get_style() { return this->get(style); }
	std::vector<field> getAll_style() { return this->getAll(style); }
	void add_style(const std::string& v) { this->add(style, v); }

	template <typename T>
	void add_style(T v) { this->add(style, v); }

	void replace_style(const std::string& v) { this->replace(style, v); }
	void replace_style(const std::string& v, uint32_t index) { this->replace(style, v, index); }

	template <typename T>
	void replace_style(T v) { this->replace(style, v); }

	template <typename T>
	void replace_style(T v, uint32_t index) { this->replace(style, v, index); }

	template <class T>
	T get_style() { return std::move(T(this->get(style).String())); }

	constexpr static Tag original_id = {16, TYPE_UINT64};
	void clear_original_id() { this->clear(original_id); }
	bool has_original_id() { return this->has(original_id); }
	field get_original_id() { return this->get(original_id); }
	std::vector<field> getAll_original_id() { return this->getAll(original_id); }
	void add_original_id(const std::string& v) { this->add(original_id, v); }

	template <typename T>
	void add_original_id(T v) { this->add(original_id, v); }

	void replace_original_id(const std::string& v) { this->replace(original_id, v); }
	void replace_original_id(const std::string& v, uint32_t index) { this->replace(original_id, v, index); }

	template <typename T>
	void replace_original_id(T v) { this->replace(original_id, v); }

	template <typename T>
	void replace_original_id(T v, uint32_t index) { this->replace(original_id, v, index); }

	template <class T>
	T get_original_id() { return std::move(T(this->get(original_id).String())); }

	constexpr static Tag equipped_state = {18, TYPE_STRING};
	void clear_equipped_state() { this->clear(equipped_state); }
	bool has_equipped_state() { return this->has(equipped_state); }
	field get_equipped_state() { return this->get(equipped_state); }
	std::vector<field> getAll_equipped_state() { return this->getAll(equipped_state); }
	void add_equipped_state(const std::string& v) { this->add(equipped_state, v); }

	template <typename T>
	void add_equipped_state(T v) { this->add(equipped_state, v); }

	void replace_equipped_state(const std::string& v) { this->replace(equipped_state, v); }
	void replace_equipped_state(const std::string& v, uint32_t index) { this->replace(equipped_state, v, index); }

	template <typename T>
	void replace_equipped_state(T v) { this->replace(equipped_state, v); }

	template <typename T>
	void replace_equipped_state(T v, uint32_t index) { this->replace(equipped_state, v, index); }

	template <class T>
	T get_equipped_state() { return std::move(T(this->get(equipped_state).String())); }

	constexpr static Tag rarity = {19, TYPE_UINT32};
	void clear_rarity() { this->clear(rarity); }
	bool has_rarity() { return this->has(rarity); }
	field get_rarity() { return this->get(rarity); }
	std::vector<field> getAll_rarity() { return this->getAll(rarity); }
	void add_rarity(const std::string& v) { this->add(rarity, v); }

	template <typename T>
	void add_rarity(T v) { this->add(rarity, v); }

	void replace_rarity(const std::string& v) { this->replace(rarity, v); }
	void replace_rarity(const std::string& v, uint32_t index) { this->replace(rarity, v, index); }

	template <typename T>
	void replace_rarity(T v) { this->replace(rarity, v); }

	template <typename T>
	void replace_rarity(T v, uint32_t index) { this->replace(rarity, v, index); }

	template <class T>
	T get_rarity() { return std::move(T(this->get(rarity).String())); }
};

struct CMsgAdjustItemEquippedState : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 4;

	CMsgAdjustItemEquippedState() : ProtoWriter(MAX_FIELD)
	{
	}

	CMsgAdjustItemEquippedState(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	CMsgAdjustItemEquippedState(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag item_id = {1, TYPE_UINT64};
	void clear_item_id() { this->clear(item_id); }
	bool has_item_id() { return this->has(item_id); }
	field get_item_id() { return this->get(item_id); }
	std::vector<field> getAll_item_id() { return this->getAll(item_id); }
	void add_item_id(const std::string& v) { this->add(item_id, v); }

	template <typename T>
	void add_item_id(T v) { this->add(item_id, v); }

	void replace_item_id(const std::string& v) { this->replace(item_id, v); }
	void replace_item_id(const std::string& v, uint32_t index) { this->replace(item_id, v, index); }

	template <typename T>
	void replace_item_id(T v) { this->replace(item_id, v); }

	template <typename T>
	void replace_item_id(T v, uint32_t index) { this->replace(item_id, v, index); }

	template <class T>
	T get_item_id() { return std::move(T(this->get(item_id).String())); }

	constexpr static Tag new_class = {2, TYPE_UINT32};
	void clear_new_class() { this->clear(new_class); }
	bool has_new_class() { return this->has(new_class); }
	field get_new_class() { return this->get(new_class); }
	std::vector<field> getAll_new_class() { return this->getAll(new_class); }
	void add_new_class(const std::string& v) { this->add(new_class, v); }

	template <typename T>
	void add_new_class(T v) { this->add(new_class, v); }

	void replace_new_class(const std::string& v) { this->replace(new_class, v); }
	void replace_new_class(const std::string& v, uint32_t index) { this->replace(new_class, v, index); }

	template <typename T>
	void replace_new_class(T v) { this->replace(new_class, v); }

	template <typename T>
	void replace_new_class(T v, uint32_t index) { this->replace(new_class, v, index); }

	template <class T>
	T get_new_class() { return std::move(T(this->get(new_class).String())); }

	constexpr static Tag new_slot = {3, TYPE_UINT32};
	void clear_new_slot() { this->clear(new_slot); }
	bool has_new_slot() { return this->has(new_slot); }
	field get_new_slot() { return this->get(new_slot); }
	std::vector<field> getAll_new_slot() { return this->getAll(new_slot); }
	void add_new_slot(const std::string& v) { this->add(new_slot, v); }

	template <typename T>
	void add_new_slot(T v) { this->add(new_slot, v); }

	void replace_new_slot(const std::string& v) { this->replace(new_slot, v); }
	void replace_new_slot(const std::string& v, uint32_t index) { this->replace(new_slot, v, index); }

	template <typename T>
	void replace_new_slot(T v) { this->replace(new_slot, v); }

	template <typename T>
	void replace_new_slot(T v, uint32_t index) { this->replace(new_slot, v, index); }

	template <class T>
	T get_new_slot() { return std::move(T(this->get(new_slot).String())); }

	constexpr static Tag swap = {4, TYPE_BOOL};
	void clear_swap() { this->clear(swap); }
	bool has_swap() { return this->has(swap); }
	field get_swap() { return this->get(swap); }
	std::vector<field> getAll_swap() { return this->getAll(swap); }
	void add_swap(const std::string& v) { this->add(swap, v); }

	template <typename T>
	void add_swap(T v) { this->add(swap, v); }

	void replace_swap(const std::string& v) { this->replace(swap, v); }
	void replace_swap(const std::string& v, uint32_t index) { this->replace(swap, v, index); }

	template <typename T>
	void replace_swap(T v) { this->replace(swap, v); }

	template <typename T>
	void replace_swap(T v, uint32_t index) { this->replace(swap, v, index); }

	template <class T>
	T get_swap() { return std::move(T(this->get(swap).String())); }
};

struct CSOEconItemEquipped : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 2;

	CSOEconItemEquipped() : ProtoWriter(MAX_FIELD)
	{
	}

	CSOEconItemEquipped(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	CSOEconItemEquipped(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag new_class = {1, TYPE_UINT32};
	void clear_new_class() { this->clear(new_class); }
	bool has_new_class() { return this->has(new_class); }
	field get_new_class() { return this->get(new_class); }
	std::vector<field> getAll_new_class() { return this->getAll(new_class); }
	void add_new_class(const std::string& v) { this->add(new_class, v); }

	template <typename T>
	void add_new_class(T v) { this->add(new_class, v); }

	void replace_new_class(const std::string& v) { this->replace(new_class, v); }
	void replace_new_class(const std::string& v, uint32_t index) { this->replace(new_class, v, index); }

	template <typename T>
	void replace_new_class(T v) { this->replace(new_class, v); }

	template <typename T>
	void replace_new_class(T v, uint32_t index) { this->replace(new_class, v, index); }

	template <class T>
	T get_new_class() { return std::move(T(this->get(new_class).String())); }

	constexpr static Tag new_slot = {2, TYPE_UINT32};
	void clear_new_slot() { this->clear(new_slot); }
	bool has_new_slot() { return this->has(new_slot); }
	field get_new_slot() { return this->get(new_slot); }
	std::vector<field> getAll_new_slot() { return this->getAll(new_slot); }
	void add_new_slot(const std::string& v) { this->add(new_slot, v); }

	template <typename T>
	void add_new_slot(T v) { this->add(new_slot, v); }

	void replace_new_slot(const std::string& v) { this->replace(new_slot, v); }
	void replace_new_slot(const std::string& v, uint32_t index) { this->replace(new_slot, v, index); }

	template <typename T>
	void replace_new_slot(T v) { this->replace(new_slot, v); }

	template <typename T>
	void replace_new_slot(T v, uint32_t index) { this->replace(new_slot, v, index); }

	template <class T>
	T get_new_slot() { return std::move(T(this->get(new_slot).String())); }
};

struct CSOEconItemAttribute : ProtoWriter
{
	constexpr static size_t MAX_FIELD = 3;

	CSOEconItemAttribute() : ProtoWriter(MAX_FIELD)
	{
	}

	CSOEconItemAttribute(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD)
	{
	}

	CSOEconItemAttribute(const std::string data) : ProtoWriter(data, MAX_FIELD)
	{
	}

	constexpr static Tag def_index = {1, TYPE_UINT32};
	void clear_def_index() { this->clear(def_index); }
	bool has_def_index() { return this->has(def_index); }
	field get_def_index() { return this->get(def_index); }
	std::vector<field> getAll_def_index() { return this->getAll(def_index); }
	void add_def_index(const std::string& v) { this->add(def_index, v); }

	template <typename T>
	void add_def_index(T v) { this->add(def_index, v); }

	void replace_def_index(const std::string& v) { this->replace(def_index, v); }
	void replace_def_index(const std::string& v, uint32_t index) { this->replace(def_index, v, index); }

	template <typename T>
	void replace_def_index(T v) { this->replace(def_index, v); }

	template <typename T>
	void replace_def_index(T v, uint32_t index) { this->replace(def_index, v, index); }

	template <class T>
	T get_def_index() { return std::move(T(this->get(def_index).String())); }

	constexpr static Tag value = {2, TYPE_UINT32};
	void clear_value() { this->clear(value); }
	bool has_value() { return this->has(value); }
	field get_value() { return this->get(value); }
	std::vector<field> getAll_value() { return this->getAll(value); }
	void add_value(const std::string& v) { this->add(value, v); }

	template <typename T>
	void add_value(T v) { this->add(value, v); }

	void replace_value(const std::string& v) { this->replace(value, v); }
	void replace_value(const std::string& v, uint32_t index) { this->replace(value, v, index); }

	template <typename T>
	void replace_value(T v) { this->replace(value, v); }

	template <typename T>
	void replace_value(T v, uint32_t index) { this->replace(value, v, index); }

	template <class T>
	T get_value() { return std::move(T(this->get(value).String())); }

	constexpr static Tag value_bytes = {3, TYPE_STRING};
	void clear_value_bytes() { this->clear(value_bytes); }
	bool has_value_bytes() { return this->has(value_bytes); }
	field get_value_bytes() { return this->get(value_bytes); }
	std::vector<field> getAll_value_bytes() { return this->getAll(value_bytes); }
	void add_value_bytes(const std::string& v) { this->add(value_bytes, v); }

	template <typename T>
	void add_value_bytes(T v) { this->add(value_bytes, v); }

	void replace_value_bytes(const std::string& v) { this->replace(value_bytes, v); }
	void replace_value_bytes(const std::string& v, uint32_t index) { this->replace(value_bytes, v, index); }

	template <typename T>
	void replace_value_bytes(T v) { this->replace(value_bytes, v); }

	template <typename T>
	void replace_value_bytes(T v, uint32_t index) { this->replace(value_bytes, v, index); }

	template <class T>
	T get_value_bytes() { return std::move(T(this->get(value_bytes).String())); }
};
