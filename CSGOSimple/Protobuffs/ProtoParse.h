//https://developers.google.com/protocol-buffers/docs/encoding
#pragma once
#include <cstdint>
#include <string>
#include <vector>

#define MAKE_TAG(FIELD_NUMBER, TYPE) static_cast<uint32_t>(((FIELD_NUMBER) << kTagTypeBits) | (TYPE))

#define make_struct(_name_, _size_) \
	struct _name_ : ProtoWriter { \
		constexpr static size_t MAX_FIELD = _size_; \
		_name_()						: ProtoWriter(MAX_FIELD) {} \
		_name_(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD) {} \
		_name_(std::string data)        : ProtoWriter(data, MAX_FIELD) {}

#define make_field(_name_, _id_, _type_) \
	constexpr static Tag _name_ = { _id_, _type_ }; \
	void clear_##_name_() { this->clear(_name_); } \
	bool has_##_name_() { return this->has(_name_); } \
	Field get_##_name_() { return this->get(_name_); } \
	std::vector<Field> getAll_##_name_() { return this->getAll(_name_); } \
	\
	void add_##_name_(std::string v) { this->add(_name_, v); } \
	template<typename T> void add_##_name_(T v) { this->add(_name_, v); } \
	void replace_##_name_(std::string v) { this->replace(_name_, v); } \
	void replace_##_name_(std::string v, uint32_t index) { this->replace(_name_, v, index); } \
	template<typename T> void replace_##_name_(T v) { this->replace(_name_, v); } \
	template<typename T> void replace_##_name_(T v, uint32_t index) { this->replace(_name_, v, index); } \
	template<class T> T get_##_name_() { return std::move( T(this->get(_name_).String()) ); }

/* 
 * make_struct and make_field make inherited struct from ProtoWriter and add methods for work with fields
 * 
 * Example generated struct with one field:
 * 
 * make_struct(CMsgClientHello, 8)
 *    make_field(client_session_need, 3, TYPE_UINT32)
 * };
 * 
 * struct CMsgClientHello : ProtoWriter {
 *    constexpr static size_t MAX_FIELD = 8;
 *    CMsgClientHello() : ProtoWriter(MAX_FIELD) {}
 *    CMsgClientHello(void* data, size_t size) : ProtoWriter(data, size, MAX_FIELD) {}
 *    CMsgClientHello(std::string data) : ProtoWriter(data, MAX_FIELD) {}
 *    
 *    constexpr static Tag client_session_need = { 3, TYPE_UINT32 };
 *    
 *    void clear_client_session_need() {
 *		this->clear(client_session_need);
 *	  }
 *	  
 *    bool has_client_session_need() {
 *       return this->has(client_session_need);
 *    }
 *    
 *    Field get_client_session_need() {
 *       return this->get(client_session_need);
 *    }
 *    
 *    std::vector<Field> getAll_client_session_need() {
 *       return this->getAll(client_session_need);
 *    }
 *    
 *    void add_client_session_need(std::string v) {
 *       this->add(client_session_need, v);
 *    }
 *    
 *    template<typename T>
 *    void add_client_session_need(T v) {
 *       this->add(client_session_need, v);
 *    }
 *    
 *    void replace_client_session_need(std::string v) {
 *       this->replace(client_session_need, v);
 *    }
 *    
 *    void replace_client_session_need(std::string v, uint32_t index) {
 *       this->replace(client_session_need, v, index);
 *    }
 *    
 *    template<typename T>
 *    void replace_client_session_need(T v) {
 *       this->replace(client_session_need, v);
 *    }
 *    
 *    template<typename T>
 *    void replace_client_session_need(T v, uint32_t index) {
 *       this->replace(client_session_need, v, index);
 *    }
 *    
 *    template<class T>
 *    T get_client_session_need() {
 *       return std::move( T(this->get(client_session_need).String()) );
 *    }
 * };
 */
	

struct Tag
{
	unsigned field;
	unsigned type;
};

enum FieldType {
	TYPE_DOUBLE = 1,
	TYPE_FLOAT = 2,
	TYPE_INT64 = 3,
	TYPE_UINT64 = 4,
	TYPE_INT32 = 5,
	TYPE_FIXED64 = 6,
	TYPE_FIXED32 = 7,
	TYPE_BOOL = 8,
	TYPE_STRING = 9,
	TYPE_GROUP = 10,
	TYPE_MESSAGE = 11,
	TYPE_BYTES = 12,
	TYPE_UINT32 = 13,
	TYPE_ENUM = 14,
	TYPE_SFIXED32 = 15,
	TYPE_SFIXED64 = 16,
	TYPE_SINT32 = 17,
	TYPE_SINT64 = 18,
	MAX_FIELD_TYPE = 18,
};

struct field
{
	friend class ProtoWriter;
	field& operator=(const field& f);

	field() : tag({ 0,0 }) { }
	field(unsigned field, unsigned type, std::string value, std::string full);
	
	field(Tag tag, const std::string& value);
	field(unsigned field, unsigned type, const std::string& value);


	template<typename T>
	field(Tag tag, T value) {
		const auto wireType = kWireTypeForFieldType[tag.type];
		full = getBytesVariant32(MAKE_TAG(tag.field, wireType));

		switch (wireType) {
		case WIRETYPE_VARINT:
			full += getBytesVariant64(static_cast<uint64_t>(value));
			break;
		case WIRETYPE_FIXED32:
			full += std::string{ reinterpret_cast<const char*>(&value), 4 };
			break;
		case WIRETYPE_FIXED64:
			full += std::string{ reinterpret_cast<const char*>(&value), 8 };
			break;
		default: ;
		}
	}

	template<typename T>
	field(unsigned field, unsigned type, T value) {
		const auto wireType = kWireTypeForFieldType[type];
		tag = { field, (unsigned)wireType };
		full = getBytesVariant32(MAKE_TAG(field, wireType));

		switch (wireType) {
		case WIRETYPE_VARINT:
			full += getBytesVariant64(static_cast<uint64_t>(value));
			break;
		case WIRETYPE_FIXED32:
			full += std::string{ reinterpret_cast<const char*>(&value), 4 };
			break;
		case WIRETYPE_FIXED64:
			full += std::string{ reinterpret_cast<const char*>(&value), 8 };
			break;
		default: ;
		}
	}

	static field ReadField(void* data, size_t& bytesRead);

	[[nodiscard]] float Float() const;
	[[nodiscard]] double Double() const;
	[[nodiscard]] int32_t Int32() const;
	[[nodiscard]] int64_t Int64() const;
	[[nodiscard]] uint32_t UInt32() const;
	[[nodiscard]] uint64_t UInt64() const;
	[[nodiscard]] uint32_t Fixed32() const;
	[[nodiscard]] uint64_t Fixed64() const;
	[[nodiscard]] int32_t SFixed32() const;
	[[nodiscard]] int64_t SFixed64() const;
	[[nodiscard]] bool Bool() const;
	[[nodiscard]] std::string String() const;

private:
	Tag tag{};
	std::string value;
	std::string full;

	static std::string getBytesVariant32(uint32_t value);
	static std::string getBytesVariant64(uint64_t value);
	static uint32_t readVarUint32(void* data, size_t& bytesRead);
	static uint64_t readVarUint64(void* data, size_t& bytesRead);

	enum WireType {
		WIRETYPE_VARINT = 0,
		WIRETYPE_FIXED64 = 1,
		WIRETYPE_LENGTH_DELIMITED = 2,
		WIRETYPE_START_GROUP = 3,
		WIRETYPE_END_GROUP = 4,
		WIRETYPE_FIXED32 = 5,
	};

	constexpr static WireType kWireTypeForFieldType[MAX_FIELD_TYPE + 1] = {
		static_cast<WireType>(-1),  // invalid
		WIRETYPE_FIXED64,           // TYPE_DOUBLE
		WIRETYPE_FIXED32,           // TYPE_FLOAT
		WIRETYPE_VARINT,            // TYPE_INT64
		WIRETYPE_VARINT,            // TYPE_UINT64
		WIRETYPE_VARINT,            // TYPE_INT32
		WIRETYPE_FIXED64,           // TYPE_FIXED64
		WIRETYPE_FIXED32,           // TYPE_FIXED32
		WIRETYPE_VARINT,            // TYPE_BOOL
		WIRETYPE_LENGTH_DELIMITED,  // TYPE_STRING
		WIRETYPE_START_GROUP,       // TYPE_GROUP
		WIRETYPE_LENGTH_DELIMITED,  // TYPE_MESSAGE
		WIRETYPE_LENGTH_DELIMITED,  // TYPE_BYTES
		WIRETYPE_VARINT,            // TYPE_UINT32
		WIRETYPE_VARINT,            // TYPE_ENUM
		WIRETYPE_FIXED32,           // TYPE_SFIXED32
		WIRETYPE_FIXED64,           // TYPE_SFIXED64
		WIRETYPE_VARINT,            // TYPE_SINT32
		WIRETYPE_VARINT,            // TYPE_SINT64
	};
	constexpr static int kTagTypeBits = 3;
	constexpr static uint32_t kTagTypeMask = (1 << kTagTypeBits) - 1;
	constexpr static int kMaxVariantBytes = 10;
	constexpr static int kMaxVariant32Bytes = 5;
};

class ProtoWriter
{
public:
	ProtoWriter();
	ProtoWriter(size_t maxFields);
	ProtoWriter(void* data, size_t size, size_t maxFields);
	ProtoWriter(const std::string& dataStr, size_t maxFields);

	/* Old method */
	void add(const field& field);
	void replace(const field& field);
	void replace(const field& field, uint32_t index);
	void clear(unsigned fieldId);
	bool has(unsigned fieldId);
	field get(unsigned fieldId);
	std::vector<field> getAll(unsigned fieldId);

	void clear(Tag tag);
	bool has(Tag tag);
	field get(Tag tag);
	std::vector<field> getAll(Tag tag);

	template<typename T>
	void add(Tag tag, T value)
	{
		fields[tag.field].emplace_back(tag, value);
	}

	template<typename T>
	void replace(Tag tag, T value)
	{
		fields[tag.field].clear();
		fields[tag.field].emplace_back(tag, value);
	}

	template<typename T>
	void replace(Tag tag, T value, uint32_t index)
	{
		fields[tag.field][index] = field(tag, value);
	}

	/* New method */
	void add(Tag tag, const std::string& value)
	{
		fields[tag.field].emplace_back(tag, value);
	}

	void replace(Tag tag, const std::string& value)
	{
		fields[tag.field].clear();
		fields[tag.field].emplace_back(tag, value);
	}

	void replace(Tag tag, const std::string& value, uint32_t index)
	{
		fields[tag.field][index] = field(tag, value);
	}

	std::string serialize();
	void print();

private:
	std::vector<std::vector<field>> fields;
};
