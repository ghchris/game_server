// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: membergame.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "membergame.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* MemberGame_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  MemberGame_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_membergame_2eproto() {
  protobuf_AddDesc_membergame_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "membergame.proto");
  GOOGLE_CHECK(file != NULL);
  MemberGame_descriptor_ = file->message_type(0);
  static const int MemberGame_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberGame, mid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberGame, jifen_),
  };
  MemberGame_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      MemberGame_descriptor_,
      MemberGame::default_instance_,
      MemberGame_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberGame, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberGame, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(MemberGame));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_membergame_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    MemberGame_descriptor_, &MemberGame::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_membergame_2eproto() {
  delete MemberGame::default_instance_;
  delete MemberGame_reflection_;
}

void protobuf_AddDesc_membergame_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\020membergame.proto\"(\n\nMemberGame\022\013\n\003mid\030"
    "\001 \002(\005\022\r\n\005jifen\030\002 \002(\003", 60);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "membergame.proto", &protobuf_RegisterTypes);
  MemberGame::default_instance_ = new MemberGame();
  MemberGame::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_membergame_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_membergame_2eproto {
  StaticDescriptorInitializer_membergame_2eproto() {
    protobuf_AddDesc_membergame_2eproto();
  }
} static_descriptor_initializer_membergame_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int MemberGame::kMidFieldNumber;
const int MemberGame::kJifenFieldNumber;
#endif  // !_MSC_VER

MemberGame::MemberGame()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:MemberGame)
}

void MemberGame::InitAsDefaultInstance() {
}

MemberGame::MemberGame(const MemberGame& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:MemberGame)
}

void MemberGame::SharedCtor() {
  _cached_size_ = 0;
  mid_ = 0;
  jifen_ = GOOGLE_LONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

MemberGame::~MemberGame() {
  // @@protoc_insertion_point(destructor:MemberGame)
  SharedDtor();
}

void MemberGame::SharedDtor() {
  if (this != default_instance_) {
  }
}

void MemberGame::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* MemberGame::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MemberGame_descriptor_;
}

const MemberGame& MemberGame::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_membergame_2eproto();
  return *default_instance_;
}

MemberGame* MemberGame::default_instance_ = NULL;

MemberGame* MemberGame::New() const {
  return new MemberGame;
}

void MemberGame::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<MemberGame*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  ZR_(jifen_, mid_);

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool MemberGame::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:MemberGame)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 mid = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &mid_)));
          set_has_mid();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_jifen;
        break;
      }

      // required int64 jifen = 2;
      case 2: {
        if (tag == 16) {
         parse_jifen:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &jifen_)));
          set_has_jifen();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:MemberGame)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:MemberGame)
  return false;
#undef DO_
}

void MemberGame::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:MemberGame)
  // required int32 mid = 1;
  if (has_mid()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->mid(), output);
  }

  // required int64 jifen = 2;
  if (has_jifen()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(2, this->jifen(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:MemberGame)
}

::google::protobuf::uint8* MemberGame::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:MemberGame)
  // required int32 mid = 1;
  if (has_mid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->mid(), target);
  }

  // required int64 jifen = 2;
  if (has_jifen()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(2, this->jifen(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:MemberGame)
  return target;
}

int MemberGame::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 mid = 1;
    if (has_mid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->mid());
    }

    // required int64 jifen = 2;
    if (has_jifen()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->jifen());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void MemberGame::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const MemberGame* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const MemberGame*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void MemberGame::MergeFrom(const MemberGame& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_mid()) {
      set_mid(from.mid());
    }
    if (from.has_jifen()) {
      set_jifen(from.jifen());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void MemberGame::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MemberGame::CopyFrom(const MemberGame& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MemberGame::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void MemberGame::Swap(MemberGame* other) {
  if (other != this) {
    std::swap(mid_, other->mid_);
    std::swap(jifen_, other->jifen_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata MemberGame::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = MemberGame_descriptor_;
  metadata.reflection = MemberGame_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
