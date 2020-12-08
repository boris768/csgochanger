#include <utility>
#include "Protobuffs/ProtoParse.h"
#include <cassert>

std::string field::getBytesVariant32(uint32_t value)
{
	uint8_t bytes[kMaxVariant32Bytes];
	int size = 0;
	while (value > 0x7F) {
		bytes[size++] = (static_cast<uint8_t>(value) & 0x7F) | 0x80;
		value >>= 7;
	}
	bytes[size++] = static_cast<uint8_t>(value) & 0x7F;
	return std::string{ reinterpret_cast<const char*>(&bytes[0]), static_cast<size_t>(size) };
}

std::string field::getBytesVariant64(uint64_t value)
{
	uint8_t bytes[kMaxVariantBytes];
	int size = 0;
	while (value > 0x7F) {
		bytes[size++] = (static_cast<uint8_t>(value) & 0x7F) | 0x80;
		value >>= 7;
	}
	bytes[size++] = static_cast<uint8_t>(value) & 0x7F;
	return std::string{ reinterpret_cast<const char*>(&bytes[0]), static_cast<size_t>(size) };
}

uint32_t field::readVarUint32(void* data, size_t& bytesRead)
{
	auto ptr = reinterpret_cast<const uint8_t*>(data);
	auto value = 0u;
	auto bytes = 0u;

	do {
		value |= static_cast<uint32_t>(*ptr & 0x7f) << (7 * bytes);
		bytes++;
	} while (*(ptr++) & 0x80 && bytes <= 5);

	bytesRead = bytes;
	return value;
}

uint64_t field::readVarUint64(void* data, size_t& bytesRead)
{
	auto ptr = reinterpret_cast<const uint8_t*>(data);
	auto value = 0ull;
	auto bytes = 0u;

	do
	{
		value |= static_cast<uint64_t>(*ptr & 0x7f) << (7 * bytes);
		bytes++;
	} while (*(ptr++) & 0x80 && bytes <= 10);

	bytesRead = bytes;

	return value;
}

field field::ReadField(void* data, size_t& bytesRead)
{
	unsigned field1 = *reinterpret_cast<uint16_t*>(data);
	const unsigned type = field1 & kTagTypeMask;

	if (field1 == 0xffff) {
		bytesRead = 0;
		//field::field 
		field ret{};
		return ret;
	}

	if (field1 & 0x80) {
		field1 = ((field1 & 0x7f) | ((field1 & 0xff00) >> 1)) >> kTagTypeBits;
		bytesRead = 2;
	}
	else {
		field1 = (field1 & 0xff) >> kTagTypeBits;
		bytesRead = 1;
	}

	size_t length, sizeDelimited;
	std::string value, full;
	switch (type)
	{
	case WIRETYPE_VARINT:
		readVarUint64(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + bytesRead), length);
		value = std::string{ reinterpret_cast<const char*>(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + bytesRead)), length };
		full = std::string{ reinterpret_cast<const char*>(data), bytesRead + length };
		bytesRead += length;
		break;
	case WIRETYPE_FIXED64:
		value = std::string{ reinterpret_cast<const char*>(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + bytesRead)), 8 };
		full = std::string{ reinterpret_cast<const char*>(data), bytesRead + 8 };
		bytesRead += 8;
		break;
	case WIRETYPE_LENGTH_DELIMITED:
		sizeDelimited = readVarUint32(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + bytesRead), length);
		value = std::string{ reinterpret_cast<const char*>(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + bytesRead)), length + sizeDelimited };
		full = std::string{ reinterpret_cast<const char*>(data), bytesRead + length + sizeDelimited };
		bytesRead += length + sizeDelimited;
		break;
	case WIRETYPE_START_GROUP:
		assert(0);
		break;
		//throw("WIRETYPE_START_GROUP unimplemented");
	case WIRETYPE_END_GROUP:
		assert(0);
		break;
		//throw("WIRETYPE_END_GROUP unimplemented");
	case WIRETYPE_FIXED32:
		value = std::string{ reinterpret_cast<const char*>(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + bytesRead)), 4 };
		full = std::string{ reinterpret_cast<const char*>(data), bytesRead + 4 };
		bytesRead += 4;
		break;
	default:
		break;
	}

	field ret(field1, type, value, full);
	return ret;
}

field& field::operator=(const field& f) = default;

field::field(unsigned field, unsigned type, std::string value, std::string full) {
	this->tag = { field, type };
	this->value = std::move(value);
	this->full = std::move(full);
}

field::field(Tag tag, const std::string& value) {
	const auto wireType = kWireTypeForFieldType[tag.type];
	full = getBytesVariant32(MAKE_TAG(tag.field, wireType));
	full += getBytesVariant32(value.size());
	full += value;
}

field::field(unsigned field, unsigned type, const std::string& value) {
	const auto wireType = kWireTypeForFieldType[type];
	tag = { field, static_cast<unsigned>(wireType) };
	full = getBytesVariant32(MAKE_TAG(field, wireType));
	full += getBytesVariant32(value.size());
	full += value;
}


float field::Float() const
{
	return *reinterpret_cast<float*>((void*)(value.data()));
}
double field::Double() const
{
	return *reinterpret_cast<double*>((void*)(value.data()));
}
int32_t field::Int32() const
{
	size_t bytesRead;
	return static_cast<int32_t>(readVarUint64((void*)(value.data()), bytesRead));
}
int64_t field::Int64() const
{
	size_t bytesRead;
	return readVarUint64((void*)(value.data()), bytesRead);
}
uint32_t field::UInt32() const
{
	size_t bytesRead;
	return readVarUint32((void*)(value.data()), bytesRead);
}
uint64_t field::UInt64() const
{
	size_t bytesRead;
	return readVarUint64((void*)(value.data()), bytesRead);
}
uint32_t field::Fixed32() const
{
	return *reinterpret_cast<uint32_t*>((void*)(value.data()));
}
uint64_t field::Fixed64() const
{
	return *reinterpret_cast<uint64_t*>((void*)(value.data()));
}
int32_t field::SFixed32() const
{
	return *reinterpret_cast<int32_t*>((void*)(value.data()));
}
int64_t field::SFixed64() const
{
	return *reinterpret_cast<int64_t*>((void*)(value.data()));
}
bool field::Bool() const
{
	size_t bytesRead;
	return !!readVarUint32((void*)(value.data()), bytesRead);
}

std::string field::String() const
{
	size_t bytesRead;
	void* data = (void*)(value.data());
	const auto length = readVarUint32((void*)(value.data()), bytesRead);
	auto value = std::string{ reinterpret_cast<const char*>(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + bytesRead)), length };
	return value;
}


ProtoWriter::ProtoWriter()
= default;

ProtoWriter::ProtoWriter(size_t maxFields)
{
	const size_t vector_size = maxFields + 1;
	fields.resize(vector_size);
	fields.reserve(vector_size);
}

ProtoWriter::ProtoWriter(void* data, size_t size, size_t maxFields) : ProtoWriter(maxFields)
{
	const size_t vector_size = maxFields + 1;
	size_t
		pos = 0,
		bytesRead;

	if (data == nullptr)
		return;
	// parse packet
	while (pos < size) {
		auto field = field::ReadField(reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(data) + pos), bytesRead);
		if (!bytesRead) break;

		const auto index = field.tag.field;

		assert(index <= vector_size);
		fields[index].emplace_back(field);
		pos += bytesRead;
	}
}

ProtoWriter::ProtoWriter(const std::string& dataStr, size_t maxFields) : ProtoWriter((void*)(dataStr.data()), dataStr.size(), maxFields)
{

}

std::string ProtoWriter::serialize()
{
	std::string result;
	for (auto& f0 : fields) {
		for (auto& f1 : f0) {
			result += f1.full;
		}
	}
	return result;
}

void ProtoWriter::print()
{
	const auto data = serialize();
	void* mem = (void*)(data.data());
	const size_t size = data.size();
	int j = 0;
	for (size_t i = 0; i <= size; ++i) {
		printf("%.2X ", *reinterpret_cast<unsigned char*>(reinterpret_cast<uintptr_t>(mem) + i));
		j++;
		if (j == 16)
		{
			j = 0;
			printf("\n");
		}
	}
	printf("\n");
}

void ProtoWriter::add(const field& field)
{
	fields[field.tag.field].push_back(field);
}

void ProtoWriter::replace(const field& field)
{
	fields[field.tag.field].clear();
	fields[field.tag.field].push_back(field);
}

void ProtoWriter::replace(const field& field, uint32_t index)
{
	fields[field.tag.field][index] = field;
}

void ProtoWriter::clear(unsigned fieldId)
{
	return fields[fieldId].clear();
}

bool ProtoWriter::has(unsigned fieldId)
{
	return !fields[fieldId].empty();
}

field ProtoWriter::get(unsigned fieldId)
{
	if (fields[fieldId].empty())
		return field();
	return fields[fieldId][0];
}

std::vector<field> ProtoWriter::getAll(unsigned fieldId)
{
	return fields[fieldId];
}

void ProtoWriter::clear(Tag tag)
{
	return fields[tag.field].clear();
}

bool ProtoWriter::has(Tag tag)
{
	return !fields[tag.field].empty();
}

field ProtoWriter::get(Tag tag)
{
	if (fields[tag.field].empty())
		return field();
	return fields[tag.field][0];
}

std::vector<field> ProtoWriter::getAll(Tag tag)
{
	return fields[tag.field];
}