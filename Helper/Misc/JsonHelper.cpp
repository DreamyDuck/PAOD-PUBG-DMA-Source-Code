#include "Include/WinHeaders.hpp"
#include "JsonHelper.h"
#include <cassert>
#include <stack>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

struct JsonReaderStackItem {
	enum State {
		BeforeStart,    //!< An object/array is in the stack but it is not yet called by StartObject()/StartArray().
		Started,        //!< An object/array is called by StartObject()/StartArray().
		Closed          //!< An array is closed after read all element, but before EndArray().
	};

	JsonReaderStackItem(const Value* value, State state) : value(value), state(state), index() {}

	const Value* value;
	State state;
	SizeType index;   // For array iteration
};

typedef std::stack<JsonReaderStackItem> JsonReaderStack;

#define DOCUMENT reinterpret_cast<Document*>(mDocument)
#define STACK (reinterpret_cast<JsonReaderStack*>(mStack))
#define TOP (STACK->top())
#define CURRENT (*TOP.value)

JsonReader::JsonReader(const char* json, size_t length) : mDocument(), mStack(), mError(false) {
	mDocument = new Document;
	if (json)
	{
		if (length)
			DOCUMENT->Parse(json, length);
		else
			DOCUMENT->Parse(json);

		if (DOCUMENT->HasParseError())
			mError = true;
		else {
			mStack = new JsonReaderStack;
			STACK->push(JsonReaderStackItem(DOCUMENT, JsonReaderStackItem::BeforeStart));
		}
	}
}

JsonReader::~JsonReader() {
	delete DOCUMENT;
	delete STACK;
}

void JsonReader::Build(const char* json, size_t length)
{
	if (length)
		DOCUMENT->Parse(json, length);
	else
		DOCUMENT->Parse(json);

	if (DOCUMENT->HasParseError())
		mError = true;
	else {
		mStack = new JsonReaderStack;
		STACK->push(JsonReaderStackItem(DOCUMENT, JsonReaderStackItem::BeforeStart));
	}
}
// Archive concept
JsonReader& JsonReader::StartObject(size_t* size) {
	if (!mError) {
		if (CURRENT.IsObject() && TOP.state == JsonReaderStackItem::BeforeStart)
		{
			TOP.state = JsonReaderStackItem::Started;
			if (size)
				*size = CURRENT.MemberCount();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::EndObject() {
	if (!mError) {
		if (CURRENT.IsObject() && TOP.state == JsonReaderStackItem::Started)
			Next();
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::Member(const char* name) {
	if (!mError) {
		if (CURRENT.IsObject() && TOP.state == JsonReaderStackItem::Started) {

			Value::ConstMemberIterator memberItr = CURRENT.FindMember(name);
			if (memberItr != CURRENT.MemberEnd())
				STACK->push(JsonReaderStackItem(&memberItr->value, JsonReaderStackItem::BeforeStart));
			else
				mError = true;
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::Member(int index, std::string& memberName) {
	if (!mError) {
		if (CURRENT.IsObject() && TOP.state == JsonReaderStackItem::Started) {
			Value::ConstMemberIterator memberItr = CURRENT.MemberBegin() + index;
			if (memberItr < CURRENT.MemberEnd())
			{
				if (memberItr->name.IsString())
					memberName = memberItr->name.GetString();
				STACK->push(JsonReaderStackItem(&memberItr->value, JsonReaderStackItem::BeforeStart));
			}
			else
				mError = true;

		}
		else
			mError = true;
	}
	return *this;
}

bool JsonReader::HasMember(const char* name) {
	if (!mError && CURRENT.IsObject() && TOP.state == JsonReaderStackItem::Started)
	{
		if (CURRENT.HasMember(name))
		{
			Member(name);
			return true;
		}
	}
	return false;
}

JsonReader& JsonReader::StartArray(size_t* size) {
	if (!mError) {
		if (CURRENT.IsArray() && TOP.state == JsonReaderStackItem::BeforeStart) {
			TOP.state = JsonReaderStackItem::Started;
			if (size)
				*size = CURRENT.Size();

			if (!CURRENT.Empty()) {
				const Value* value = &CURRENT[TOP.index];
				STACK->push(JsonReaderStackItem(value, JsonReaderStackItem::BeforeStart));
			}
			else
				TOP.state = JsonReaderStackItem::Closed;
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::EndArray() {
	if (!mError) {
		if (CURRENT.IsArray() && TOP.state == JsonReaderStackItem::Closed)
			Next();
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::operator&(bool& b) {
	if (!mError) {
		if (CURRENT.IsBool()) {
			b = CURRENT.GetBool();
			Next();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::operator&(unsigned long long& ul)
{
	if (!mError) {
		if (CURRENT.IsUint64()) {
			ul = CURRENT.GetUint64();
			Next();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::operator&(long long& l)
{
	if (!mError) {
		if (CURRENT.IsInt64()) {
			l = CURRENT.GetInt64();
			Next();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::operator&(unsigned& u) {
	if (!mError) {
		if (CURRENT.IsUint()) {
			u = CURRENT.GetUint();
			Next();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::operator&(int& i) {
	if (!mError) {
		if (CURRENT.IsInt()) {
			i = CURRENT.GetInt();
			Next();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::operator&(double& d) {
	if (!mError) {
		if (CURRENT.IsNumber()) {
			d = CURRENT.GetDouble();
			Next();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::operator&(float& f) {
	if (!mError) {
		if (CURRENT.IsNumber()) {
			f = (float)CURRENT.GetDouble();
			Next();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::operator&(std::string& s) {
	if (!mError) {
		if (CURRENT.IsString()) {
			s = CURRENT.GetString();
			Next();
		}
		else
			mError = true;
	}
	return *this;
}

JsonReader& JsonReader::SetNull() {
	// This function is for JsonWriter only.
	mError = true;
	return *this;
}

void JsonReader::Next() {
	if (!mError) {
		assert(!STACK->empty());
		STACK->pop();

		if (!STACK->empty() && CURRENT.IsArray()) {
			if (TOP.state == JsonReaderStackItem::Started) { // Otherwise means reading array item pass end
				if (TOP.index < CURRENT.Size() - 1) {
					const Value* value = &CURRENT[++TOP.index];
					STACK->push(JsonReaderStackItem(value, JsonReaderStackItem::BeforeStart));
				}
				else
					TOP.state = JsonReaderStackItem::Closed;
			}
			else
				mError = true;
		}
	}
}

#undef DOCUMENT
#undef STACK
#undef TOP
#undef CURRENT

////////////////////////////////////////////////////////////////////////////////
// JsonWriter

#define WRITER reinterpret_cast<PrettyWriter<StringBuffer>*>(mWriter)
#define STREAM reinterpret_cast<StringBuffer*>(mStream)

JsonWriter::JsonWriter() : mWriter(), mStream() {
	mStream = new StringBuffer;
	mWriter = new PrettyWriter<StringBuffer>(*STREAM);
}

JsonWriter::~JsonWriter() {
	delete WRITER;
	delete STREAM;
}

const char* JsonWriter::GetString() const {
	return STREAM->GetString();
}
size_t JsonWriter::GetStringLength() const {
	return STREAM->GetLength();
}

JsonWriter& JsonWriter::StartObject(size_t* size) {
	WRITER->StartObject();
	return *this;
}

JsonWriter& JsonWriter::EndObject() {
	WRITER->EndObject();
	return *this;
}

JsonWriter& JsonWriter::Member(const char* name) {
	WRITER->String(name, static_cast<SizeType>(strlen(name)));
	return *this;
}

bool JsonWriter::HasMember(const char*) const {
	// This function is for JsonReader only.
	assert(false);
	return false;
}

JsonWriter& JsonWriter::StartArray(size_t*) {
	WRITER->StartArray();
	return *this;
}

JsonWriter& JsonWriter::EndArray() {
	WRITER->EndArray();
	return *this;
}

JsonWriter& JsonWriter::operator&(bool& b) {
	WRITER->Bool(b);
	return *this;
}

JsonWriter& JsonWriter::operator&(unsigned& u) {
	WRITER->Uint(u);
	return *this;
}

JsonWriter& JsonWriter::operator&(unsigned long long& ul)
{
	WRITER->Uint64(ul);
	return *this;
}

JsonWriter& JsonWriter::operator&(long long& l)
{
	WRITER->Int64(l);
	return *this;
}

JsonWriter& JsonWriter::operator&(int& i) {
	WRITER->Int(i);
	return *this;
}

JsonWriter& JsonWriter::operator&(double& d) {
	WRITER->Double(d);
	return *this;
}

JsonWriter& JsonWriter::operator&(float& f) {
	WRITER->Double(f);
	return *this;
}


JsonWriter& JsonWriter::operator&(std::string& s) {
	WRITER->String(s.c_str(), static_cast<SizeType>(s.size()));
	return *this;
}

JsonWriter& JsonWriter::SetNull() {
	WRITER->Null();
	return *this;
}

#undef STREAM
#undef WRITER
