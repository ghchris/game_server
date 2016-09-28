// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: membercommongame.proto

#ifndef PROTOBUF_membercommongame_2eproto__INCLUDED
#define PROTOBUF_membercommongame_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_membercommongame_2eproto();
void protobuf_AssignDesc_membercommongame_2eproto();
void protobuf_ShutdownFile_membercommongame_2eproto();

class MemberCommonGame;

// ===================================================================

class MemberCommonGame : public ::google::protobuf::Message {
 public:
  MemberCommonGame();
  virtual ~MemberCommonGame();

  MemberCommonGame(const MemberCommonGame& from);

  inline MemberCommonGame& operator=(const MemberCommonGame& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MemberCommonGame& default_instance();

  void Swap(MemberCommonGame* other);

  // implements Message ----------------------------------------------

  MemberCommonGame* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MemberCommonGame& from);
  void MergeFrom(const MemberCommonGame& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 mid = 1;
  inline bool has_mid() const;
  inline void clear_mid();
  static const int kMidFieldNumber = 1;
  inline ::google::protobuf::int32 mid() const;
  inline void set_mid(::google::protobuf::int32 value);

  // required int64 gold = 2;
  inline bool has_gold() const;
  inline void clear_gold();
  static const int kGoldFieldNumber = 2;
  inline ::google::protobuf::int64 gold() const;
  inline void set_gold(::google::protobuf::int64 value);

  // required int32 vip = 3;
  inline bool has_vip() const;
  inline void clear_vip();
  static const int kVipFieldNumber = 3;
  inline ::google::protobuf::int32 vip() const;
  inline void set_vip(::google::protobuf::int32 value);

  // required string bankpasswd = 4;
  inline bool has_bankpasswd() const;
  inline void clear_bankpasswd();
  static const int kBankpasswdFieldNumber = 4;
  inline const ::std::string& bankpasswd() const;
  inline void set_bankpasswd(const ::std::string& value);
  inline void set_bankpasswd(const char* value);
  inline void set_bankpasswd(const char* value, size_t size);
  inline ::std::string* mutable_bankpasswd();
  inline ::std::string* release_bankpasswd();
  inline void set_allocated_bankpasswd(::std::string* bankpasswd);

  // required int64 bankAssets = 5;
  inline bool has_bankassets() const;
  inline void clear_bankassets();
  static const int kBankAssetsFieldNumber = 5;
  inline ::google::protobuf::int64 bankassets() const;
  inline void set_bankassets(::google::protobuf::int64 value);

  // @@protoc_insertion_point(class_scope:MemberCommonGame)
 private:
  inline void set_has_mid();
  inline void clear_has_mid();
  inline void set_has_gold();
  inline void clear_has_gold();
  inline void set_has_vip();
  inline void clear_has_vip();
  inline void set_has_bankpasswd();
  inline void clear_has_bankpasswd();
  inline void set_has_bankassets();
  inline void clear_has_bankassets();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int64 gold_;
  ::google::protobuf::int32 mid_;
  ::google::protobuf::int32 vip_;
  ::std::string* bankpasswd_;
  ::google::protobuf::int64 bankassets_;
  friend void  protobuf_AddDesc_membercommongame_2eproto();
  friend void protobuf_AssignDesc_membercommongame_2eproto();
  friend void protobuf_ShutdownFile_membercommongame_2eproto();

  void InitAsDefaultInstance();
  static MemberCommonGame* default_instance_;
};
// ===================================================================


// ===================================================================

// MemberCommonGame

// required int32 mid = 1;
inline bool MemberCommonGame::has_mid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MemberCommonGame::set_has_mid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MemberCommonGame::clear_has_mid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MemberCommonGame::clear_mid() {
  mid_ = 0;
  clear_has_mid();
}
inline ::google::protobuf::int32 MemberCommonGame::mid() const {
  // @@protoc_insertion_point(field_get:MemberCommonGame.mid)
  return mid_;
}
inline void MemberCommonGame::set_mid(::google::protobuf::int32 value) {
  set_has_mid();
  mid_ = value;
  // @@protoc_insertion_point(field_set:MemberCommonGame.mid)
}

// required int64 gold = 2;
inline bool MemberCommonGame::has_gold() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MemberCommonGame::set_has_gold() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MemberCommonGame::clear_has_gold() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MemberCommonGame::clear_gold() {
  gold_ = GOOGLE_LONGLONG(0);
  clear_has_gold();
}
inline ::google::protobuf::int64 MemberCommonGame::gold() const {
  // @@protoc_insertion_point(field_get:MemberCommonGame.gold)
  return gold_;
}
inline void MemberCommonGame::set_gold(::google::protobuf::int64 value) {
  set_has_gold();
  gold_ = value;
  // @@protoc_insertion_point(field_set:MemberCommonGame.gold)
}

// required int32 vip = 3;
inline bool MemberCommonGame::has_vip() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MemberCommonGame::set_has_vip() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MemberCommonGame::clear_has_vip() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MemberCommonGame::clear_vip() {
  vip_ = 0;
  clear_has_vip();
}
inline ::google::protobuf::int32 MemberCommonGame::vip() const {
  // @@protoc_insertion_point(field_get:MemberCommonGame.vip)
  return vip_;
}
inline void MemberCommonGame::set_vip(::google::protobuf::int32 value) {
  set_has_vip();
  vip_ = value;
  // @@protoc_insertion_point(field_set:MemberCommonGame.vip)
}

// required string bankpasswd = 4;
inline bool MemberCommonGame::has_bankpasswd() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void MemberCommonGame::set_has_bankpasswd() {
  _has_bits_[0] |= 0x00000008u;
}
inline void MemberCommonGame::clear_has_bankpasswd() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void MemberCommonGame::clear_bankpasswd() {
  if (bankpasswd_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    bankpasswd_->clear();
  }
  clear_has_bankpasswd();
}
inline const ::std::string& MemberCommonGame::bankpasswd() const {
  // @@protoc_insertion_point(field_get:MemberCommonGame.bankpasswd)
  return *bankpasswd_;
}
inline void MemberCommonGame::set_bankpasswd(const ::std::string& value) {
  set_has_bankpasswd();
  if (bankpasswd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    bankpasswd_ = new ::std::string;
  }
  bankpasswd_->assign(value);
  // @@protoc_insertion_point(field_set:MemberCommonGame.bankpasswd)
}
inline void MemberCommonGame::set_bankpasswd(const char* value) {
  set_has_bankpasswd();
  if (bankpasswd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    bankpasswd_ = new ::std::string;
  }
  bankpasswd_->assign(value);
  // @@protoc_insertion_point(field_set_char:MemberCommonGame.bankpasswd)
}
inline void MemberCommonGame::set_bankpasswd(const char* value, size_t size) {
  set_has_bankpasswd();
  if (bankpasswd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    bankpasswd_ = new ::std::string;
  }
  bankpasswd_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MemberCommonGame.bankpasswd)
}
inline ::std::string* MemberCommonGame::mutable_bankpasswd() {
  set_has_bankpasswd();
  if (bankpasswd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    bankpasswd_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MemberCommonGame.bankpasswd)
  return bankpasswd_;
}
inline ::std::string* MemberCommonGame::release_bankpasswd() {
  clear_has_bankpasswd();
  if (bankpasswd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = bankpasswd_;
    bankpasswd_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MemberCommonGame::set_allocated_bankpasswd(::std::string* bankpasswd) {
  if (bankpasswd_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete bankpasswd_;
  }
  if (bankpasswd) {
    set_has_bankpasswd();
    bankpasswd_ = bankpasswd;
  } else {
    clear_has_bankpasswd();
    bankpasswd_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MemberCommonGame.bankpasswd)
}

// required int64 bankAssets = 5;
inline bool MemberCommonGame::has_bankassets() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void MemberCommonGame::set_has_bankassets() {
  _has_bits_[0] |= 0x00000010u;
}
inline void MemberCommonGame::clear_has_bankassets() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void MemberCommonGame::clear_bankassets() {
  bankassets_ = GOOGLE_LONGLONG(0);
  clear_has_bankassets();
}
inline ::google::protobuf::int64 MemberCommonGame::bankassets() const {
  // @@protoc_insertion_point(field_get:MemberCommonGame.bankAssets)
  return bankassets_;
}
inline void MemberCommonGame::set_bankassets(::google::protobuf::int64 value) {
  set_has_bankassets();
  bankassets_ = value;
  // @@protoc_insertion_point(field_set:MemberCommonGame.bankAssets)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_membercommongame_2eproto__INCLUDED
