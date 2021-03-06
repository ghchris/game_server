// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: membergame.proto

#ifndef PROTOBUF_membergame_2eproto__INCLUDED
#define PROTOBUF_membergame_2eproto__INCLUDED

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
void  protobuf_AddDesc_membergame_2eproto();
void protobuf_AssignDesc_membergame_2eproto();
void protobuf_ShutdownFile_membergame_2eproto();

class MemberGame;

// ===================================================================

class MemberGame : public ::google::protobuf::Message {
 public:
  MemberGame();
  virtual ~MemberGame();

  MemberGame(const MemberGame& from);

  inline MemberGame& operator=(const MemberGame& from) {
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
  static const MemberGame& default_instance();

  void Swap(MemberGame* other);

  // implements Message ----------------------------------------------

  MemberGame* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MemberGame& from);
  void MergeFrom(const MemberGame& from);
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

  // required int64 jifen = 2;
  inline bool has_jifen() const;
  inline void clear_jifen();
  static const int kJifenFieldNumber = 2;
  inline ::google::protobuf::int64 jifen() const;
  inline void set_jifen(::google::protobuf::int64 value);

  // required int32 type = 3;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 3;
  inline ::google::protobuf::int32 type() const;
  inline void set_type(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:MemberGame)
 private:
  inline void set_has_mid();
  inline void clear_has_mid();
  inline void set_has_jifen();
  inline void clear_has_jifen();
  inline void set_has_type();
  inline void clear_has_type();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int64 jifen_;
  ::google::protobuf::int32 mid_;
  ::google::protobuf::int32 type_;
  friend void  protobuf_AddDesc_membergame_2eproto();
  friend void protobuf_AssignDesc_membergame_2eproto();
  friend void protobuf_ShutdownFile_membergame_2eproto();

  void InitAsDefaultInstance();
  static MemberGame* default_instance_;
};
// ===================================================================


// ===================================================================

// MemberGame

// required int32 mid = 1;
inline bool MemberGame::has_mid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MemberGame::set_has_mid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MemberGame::clear_has_mid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MemberGame::clear_mid() {
  mid_ = 0;
  clear_has_mid();
}
inline ::google::protobuf::int32 MemberGame::mid() const {
  // @@protoc_insertion_point(field_get:MemberGame.mid)
  return mid_;
}
inline void MemberGame::set_mid(::google::protobuf::int32 value) {
  set_has_mid();
  mid_ = value;
  // @@protoc_insertion_point(field_set:MemberGame.mid)
}

// required int64 jifen = 2;
inline bool MemberGame::has_jifen() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MemberGame::set_has_jifen() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MemberGame::clear_has_jifen() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MemberGame::clear_jifen() {
  jifen_ = GOOGLE_LONGLONG(0);
  clear_has_jifen();
}
inline ::google::protobuf::int64 MemberGame::jifen() const {
  // @@protoc_insertion_point(field_get:MemberGame.jifen)
  return jifen_;
}
inline void MemberGame::set_jifen(::google::protobuf::int64 value) {
  set_has_jifen();
  jifen_ = value;
  // @@protoc_insertion_point(field_set:MemberGame.jifen)
}

// required int32 type = 3;
inline bool MemberGame::has_type() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MemberGame::set_has_type() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MemberGame::clear_has_type() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MemberGame::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::google::protobuf::int32 MemberGame::type() const {
  // @@protoc_insertion_point(field_get:MemberGame.type)
  return type_;
}
inline void MemberGame::set_type(::google::protobuf::int32 value) {
  set_has_type();
  type_ = value;
  // @@protoc_insertion_point(field_set:MemberGame.type)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_membergame_2eproto__INCLUDED
