// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: membercommongame.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "membercommongame.pb.h"

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

const ::google::protobuf::Descriptor* MemberCommonGame_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  MemberCommonGame_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_membercommongame_2eproto() {
  protobuf_AddDesc_membercommongame_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "membercommongame.proto");
  GOOGLE_CHECK(file != NULL);
  MemberCommonGame_descriptor_ = file->message_type(0);
  static const int MemberCommonGame_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberCommonGame, mid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberCommonGame, gold_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberCommonGame, vip_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberCommonGame, bankpasswd_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberCommonGame, bankassets_),
  };
  MemberCommonGame_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      MemberCommonGame_descriptor_,
      MemberCommonGame::default_instance_,
      MemberCommonGame_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberCommonGame, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MemberCommonGame, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(MemberCommonGame));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_membercommongame_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    MemberCommonGame_descriptor_, &MemberCommonGame::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_membercommongame_2eproto() {
  delete MemberCommonGame::default_instance_;
  delete MemberCommonGame_reflection_;
}

void protobuf_AddDesc_membercommongame_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\026membercommongame.proto\"b\n\020MemberCommon"
    "Game\022\013\n\003mid\030\001 \002(\005\022\014\n\004gold\030\002 \002(\003\022\013\n\003vip\030\003"
    " \002(\005\022\022\n\nbankpasswd\030\004 \002(\t\022\022\n\nbankAssets\030\005"
    " \002(\003", 124);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "membercommongame.proto", &protobuf_RegisterTypes);
  MemberCommonGame::default_instance_ = new MemberCommonGame();
  MemberCommonGame::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_membercommongame_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_membercommongame_2eproto {
  StaticDescriptorInitializer_membercommongame_2eproto() {
    protobuf_AddDesc_membercommongame_2eproto();
  }
} static_descriptor_initializer_membercommongame_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int MemberCommonGame::kMidFieldNumber;
const int MemberCommonGame::kGoldFieldNumber;
const int MemberCommonGame::kVipFieldNumber;
const int MemberCommonGame::kBankpasswdFieldNumber;
const int MemberCommonGame::kBankAssetsFieldNumber;
#endif  // !_MSC_VER

MemberCommonGame::MemberCommonGame()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:MemberCommonGame)
}

void MemberCommonGame::InitAsDefaultInstance() {
}

MemberCommonGame::MemberCommonGame(const MemberCommonGame& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:MemberCommonGame)
}

void MemberCommonGame::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  mid_ = 0;
  gold_ = GOOGLE_LONGLONG(0);
  vip_ = 0;
  bankpasswd_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  bankassets_ = GOOGLE_LONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

MemberCommonGame::~MemberCommonGame() {
  // @@protoc_insertion_point(destructor:MemberCommonGame)
  SharedDtor();
}

void MemberCommonGame::SharedDtor() {
  if (bankpasswd_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete bankpasswd_;
  }
  if (this != default_instance_) {
  }
}

void MemberCommonGame::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* MemberCommonGame::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MemberCommonGame_descriptor_;
}

const MemberCommonGame& MemberCommonGame::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_membercommongame_2eproto();
  return *default_instance_;
}

MemberCommonGame* MemberCommonGame::default_instance_ = NULL;

MemberCommonGame* MemberCommonGame::New() const {
  return new MemberCommonGame;
}

void MemberCommonGame::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<MemberCommonGame*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  if (_has_bits_[0 / 32] & 31) {
    ZR_(gold_, vip_);
    if (has_bankpasswd()) {
      if (bankpasswd_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        bankpasswd_->clear();
      }
    }
    bankassets_ = GOOGLE_LONGLONG(0);
  }

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool MemberCommonGame::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:MemberCommonGame)
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
        if (input->ExpectTag(16)) goto parse_gold;
        break;
      }

      // required int64 gold = 2;
      case 2: {
        if (tag == 16) {
         parse_gold:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &gold_)));
          set_has_gold();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_vip;
        break;
      }

      // required int32 vip = 3;
      case 3: {
        if (tag == 24) {
         parse_vip:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &vip_)));
          set_has_vip();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(34)) goto parse_bankpasswd;
        break;
      }

      // required string bankpasswd = 4;
      case 4: {
        if (tag == 34) {
         parse_bankpasswd:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_bankpasswd()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->bankpasswd().data(), this->bankpasswd().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "bankpasswd");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(40)) goto parse_bankAssets;
        break;
      }

      // required int64 bankAssets = 5;
      case 5: {
        if (tag == 40) {
         parse_bankAssets:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &bankassets_)));
          set_has_bankassets();
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
  // @@protoc_insertion_point(parse_success:MemberCommonGame)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:MemberCommonGame)
  return false;
#undef DO_
}

void MemberCommonGame::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:MemberCommonGame)
  // required int32 mid = 1;
  if (has_mid()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->mid(), output);
  }

  // required int64 gold = 2;
  if (has_gold()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(2, this->gold(), output);
  }

  // required int32 vip = 3;
  if (has_vip()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->vip(), output);
  }

  // required string bankpasswd = 4;
  if (has_bankpasswd()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->bankpasswd().data(), this->bankpasswd().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "bankpasswd");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      4, this->bankpasswd(), output);
  }

  // required int64 bankAssets = 5;
  if (has_bankassets()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(5, this->bankassets(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:MemberCommonGame)
}

::google::protobuf::uint8* MemberCommonGame::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:MemberCommonGame)
  // required int32 mid = 1;
  if (has_mid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->mid(), target);
  }

  // required int64 gold = 2;
  if (has_gold()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(2, this->gold(), target);
  }

  // required int32 vip = 3;
  if (has_vip()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->vip(), target);
  }

  // required string bankpasswd = 4;
  if (has_bankpasswd()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->bankpasswd().data(), this->bankpasswd().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "bankpasswd");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->bankpasswd(), target);
  }

  // required int64 bankAssets = 5;
  if (has_bankassets()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(5, this->bankassets(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:MemberCommonGame)
  return target;
}

int MemberCommonGame::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 mid = 1;
    if (has_mid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->mid());
    }

    // required int64 gold = 2;
    if (has_gold()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->gold());
    }

    // required int32 vip = 3;
    if (has_vip()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->vip());
    }

    // required string bankpasswd = 4;
    if (has_bankpasswd()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->bankpasswd());
    }

    // required int64 bankAssets = 5;
    if (has_bankassets()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->bankassets());
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

void MemberCommonGame::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const MemberCommonGame* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const MemberCommonGame*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void MemberCommonGame::MergeFrom(const MemberCommonGame& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_mid()) {
      set_mid(from.mid());
    }
    if (from.has_gold()) {
      set_gold(from.gold());
    }
    if (from.has_vip()) {
      set_vip(from.vip());
    }
    if (from.has_bankpasswd()) {
      set_bankpasswd(from.bankpasswd());
    }
    if (from.has_bankassets()) {
      set_bankassets(from.bankassets());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void MemberCommonGame::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MemberCommonGame::CopyFrom(const MemberCommonGame& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MemberCommonGame::IsInitialized() const {
  if ((_has_bits_[0] & 0x0000001f) != 0x0000001f) return false;

  return true;
}

void MemberCommonGame::Swap(MemberCommonGame* other) {
  if (other != this) {
    std::swap(mid_, other->mid_);
    std::swap(gold_, other->gold_);
    std::swap(vip_, other->vip_);
    std::swap(bankpasswd_, other->bankpasswd_);
    std::swap(bankassets_, other->bankassets_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata MemberCommonGame::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = MemberCommonGame_descriptor_;
  metadata.reflection = MemberCommonGame_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)