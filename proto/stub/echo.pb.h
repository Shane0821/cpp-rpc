// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: echo.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_echo_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_echo_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3020000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3020003 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/service.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_echo_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_echo_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_echo_2eproto;
namespace echo {
class EchoRequest;
struct EchoRequestDefaultTypeInternal;
extern EchoRequestDefaultTypeInternal _EchoRequest_default_instance_;
class EchoResponse;
struct EchoResponseDefaultTypeInternal;
extern EchoResponseDefaultTypeInternal _EchoResponse_default_instance_;
class GetDataReq;
struct GetDataReqDefaultTypeInternal;
extern GetDataReqDefaultTypeInternal _GetDataReq_default_instance_;
class GetDataRsp;
struct GetDataRspDefaultTypeInternal;
extern GetDataRspDefaultTypeInternal _GetDataRsp_default_instance_;
}  // namespace echo
PROTOBUF_NAMESPACE_OPEN
template<> ::echo::EchoRequest* Arena::CreateMaybeMessage<::echo::EchoRequest>(Arena*);
template<> ::echo::EchoResponse* Arena::CreateMaybeMessage<::echo::EchoResponse>(Arena*);
template<> ::echo::GetDataReq* Arena::CreateMaybeMessage<::echo::GetDataReq>(Arena*);
template<> ::echo::GetDataRsp* Arena::CreateMaybeMessage<::echo::GetDataRsp>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace echo {

enum ErrorCode : int {
  SUCCESS = 0,
  UNKNOWN = 9999,
  RPC_TIMEOUT = 10000,
  PARSE_PKG_FAILED = 10001,
  RECVED_TIMEOUT_PKG = 10002,
  ErrorCode_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ErrorCode_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ErrorCode_IsValid(int value);
constexpr ErrorCode ErrorCode_MIN = SUCCESS;
constexpr ErrorCode ErrorCode_MAX = RECVED_TIMEOUT_PKG;
constexpr int ErrorCode_ARRAYSIZE = ErrorCode_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ErrorCode_descriptor();
template<typename T>
inline const std::string& ErrorCode_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ErrorCode>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ErrorCode_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ErrorCode_descriptor(), enum_t_value);
}
inline bool ErrorCode_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ErrorCode* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ErrorCode>(
    ErrorCode_descriptor(), name, value);
}
// ===================================================================

class EchoRequest final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:echo.EchoRequest) */ {
 public:
  inline EchoRequest() : EchoRequest(nullptr) {}
  ~EchoRequest() override;
  explicit PROTOBUF_CONSTEXPR EchoRequest(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  EchoRequest(const EchoRequest& from);
  EchoRequest(EchoRequest&& from) noexcept
    : EchoRequest() {
    *this = ::std::move(from);
  }

  inline EchoRequest& operator=(const EchoRequest& from) {
    CopyFrom(from);
    return *this;
  }
  inline EchoRequest& operator=(EchoRequest&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const EchoRequest& default_instance() {
    return *internal_default_instance();
  }
  static inline const EchoRequest* internal_default_instance() {
    return reinterpret_cast<const EchoRequest*>(
               &_EchoRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(EchoRequest& a, EchoRequest& b) {
    a.Swap(&b);
  }
  inline void Swap(EchoRequest* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(EchoRequest* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  EchoRequest* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<EchoRequest>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const EchoRequest& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const EchoRequest& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(EchoRequest* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "echo.EchoRequest";
  }
  protected:
  explicit EchoRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kMsgFieldNumber = 1,
  };
  // optional string msg = 1;
  bool has_msg() const;
  private:
  bool _internal_has_msg() const;
  public:
  void clear_msg();
  const std::string& msg() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_msg(ArgT0&& arg0, ArgT... args);
  std::string* mutable_msg();
  PROTOBUF_NODISCARD std::string* release_msg();
  void set_allocated_msg(std::string* msg);
  private:
  const std::string& _internal_msg() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_msg(const std::string& value);
  std::string* _internal_mutable_msg();
  public:

  // @@protoc_insertion_point(class_scope:echo.EchoRequest)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr msg_;
  friend struct ::TableStruct_echo_2eproto;
};
// -------------------------------------------------------------------

class EchoResponse final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:echo.EchoResponse) */ {
 public:
  inline EchoResponse() : EchoResponse(nullptr) {}
  ~EchoResponse() override;
  explicit PROTOBUF_CONSTEXPR EchoResponse(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  EchoResponse(const EchoResponse& from);
  EchoResponse(EchoResponse&& from) noexcept
    : EchoResponse() {
    *this = ::std::move(from);
  }

  inline EchoResponse& operator=(const EchoResponse& from) {
    CopyFrom(from);
    return *this;
  }
  inline EchoResponse& operator=(EchoResponse&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const EchoResponse& default_instance() {
    return *internal_default_instance();
  }
  static inline const EchoResponse* internal_default_instance() {
    return reinterpret_cast<const EchoResponse*>(
               &_EchoResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(EchoResponse& a, EchoResponse& b) {
    a.Swap(&b);
  }
  inline void Swap(EchoResponse* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(EchoResponse* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  EchoResponse* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<EchoResponse>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const EchoResponse& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const EchoResponse& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(EchoResponse* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "echo.EchoResponse";
  }
  protected:
  explicit EchoResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kMsgFieldNumber = 2,
  };
  // optional string msg = 2;
  bool has_msg() const;
  private:
  bool _internal_has_msg() const;
  public:
  void clear_msg();
  const std::string& msg() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_msg(ArgT0&& arg0, ArgT... args);
  std::string* mutable_msg();
  PROTOBUF_NODISCARD std::string* release_msg();
  void set_allocated_msg(std::string* msg);
  private:
  const std::string& _internal_msg() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_msg(const std::string& value);
  std::string* _internal_mutable_msg();
  public:

  // @@protoc_insertion_point(class_scope:echo.EchoResponse)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr msg_;
  friend struct ::TableStruct_echo_2eproto;
};
// -------------------------------------------------------------------

class GetDataReq final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:echo.GetDataReq) */ {
 public:
  inline GetDataReq() : GetDataReq(nullptr) {}
  ~GetDataReq() override;
  explicit PROTOBUF_CONSTEXPR GetDataReq(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  GetDataReq(const GetDataReq& from);
  GetDataReq(GetDataReq&& from) noexcept
    : GetDataReq() {
    *this = ::std::move(from);
  }

  inline GetDataReq& operator=(const GetDataReq& from) {
    CopyFrom(from);
    return *this;
  }
  inline GetDataReq& operator=(GetDataReq&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const GetDataReq& default_instance() {
    return *internal_default_instance();
  }
  static inline const GetDataReq* internal_default_instance() {
    return reinterpret_cast<const GetDataReq*>(
               &_GetDataReq_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(GetDataReq& a, GetDataReq& b) {
    a.Swap(&b);
  }
  inline void Swap(GetDataReq* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(GetDataReq* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  GetDataReq* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<GetDataReq>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const GetDataReq& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const GetDataReq& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(GetDataReq* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "echo.GetDataReq";
  }
  protected:
  explicit GetDataReq(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kMsgFieldNumber = 1,
  };
  // optional string msg = 1;
  bool has_msg() const;
  private:
  bool _internal_has_msg() const;
  public:
  void clear_msg();
  const std::string& msg() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_msg(ArgT0&& arg0, ArgT... args);
  std::string* mutable_msg();
  PROTOBUF_NODISCARD std::string* release_msg();
  void set_allocated_msg(std::string* msg);
  private:
  const std::string& _internal_msg() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_msg(const std::string& value);
  std::string* _internal_mutable_msg();
  public:

  // @@protoc_insertion_point(class_scope:echo.GetDataReq)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr msg_;
  friend struct ::TableStruct_echo_2eproto;
};
// -------------------------------------------------------------------

class GetDataRsp final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:echo.GetDataRsp) */ {
 public:
  inline GetDataRsp() : GetDataRsp(nullptr) {}
  ~GetDataRsp() override;
  explicit PROTOBUF_CONSTEXPR GetDataRsp(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  GetDataRsp(const GetDataRsp& from);
  GetDataRsp(GetDataRsp&& from) noexcept
    : GetDataRsp() {
    *this = ::std::move(from);
  }

  inline GetDataRsp& operator=(const GetDataRsp& from) {
    CopyFrom(from);
    return *this;
  }
  inline GetDataRsp& operator=(GetDataRsp&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const GetDataRsp& default_instance() {
    return *internal_default_instance();
  }
  static inline const GetDataRsp* internal_default_instance() {
    return reinterpret_cast<const GetDataRsp*>(
               &_GetDataRsp_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    3;

  friend void swap(GetDataRsp& a, GetDataRsp& b) {
    a.Swap(&b);
  }
  inline void Swap(GetDataRsp* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(GetDataRsp* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  GetDataRsp* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<GetDataRsp>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const GetDataRsp& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const GetDataRsp& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(GetDataRsp* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "echo.GetDataRsp";
  }
  protected:
  explicit GetDataRsp(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kMsgFieldNumber = 1,
  };
  // optional string msg = 1;
  bool has_msg() const;
  private:
  bool _internal_has_msg() const;
  public:
  void clear_msg();
  const std::string& msg() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_msg(ArgT0&& arg0, ArgT... args);
  std::string* mutable_msg();
  PROTOBUF_NODISCARD std::string* release_msg();
  void set_allocated_msg(std::string* msg);
  private:
  const std::string& _internal_msg() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_msg(const std::string& value);
  std::string* _internal_mutable_msg();
  public:

  // @@protoc_insertion_point(class_scope:echo.GetDataRsp)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr msg_;
  friend struct ::TableStruct_echo_2eproto;
};
// ===================================================================

class EchoService_Stub;

class EchoService : public ::PROTOBUF_NAMESPACE_ID::Service {
 protected:
  // This class should be treated as an abstract interface.
  inline EchoService() {};
 public:
  virtual ~EchoService();

  typedef EchoService_Stub Stub;

  static const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* descriptor();

  virtual void Echo(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::echo::EchoRequest* request,
                       ::echo::EchoResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void RelayEcho(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::echo::EchoRequest* request,
                       ::echo::EchoResponse* response,
                       ::google::protobuf::Closure* done);

  // implements Service ----------------------------------------------

  const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method,
                  ::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                  const ::PROTOBUF_NAMESPACE_ID::Message* request,
                  ::PROTOBUF_NAMESPACE_ID::Message* response,
                  ::google::protobuf::Closure* done);
  const ::PROTOBUF_NAMESPACE_ID::Message& GetRequestPrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const;
  const ::PROTOBUF_NAMESPACE_ID::Message& GetResponsePrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(EchoService);
};

class EchoService_Stub : public EchoService {
 public:
  EchoService_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel);
  EchoService_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel,
                   ::PROTOBUF_NAMESPACE_ID::Service::ChannelOwnership ownership);
  ~EchoService_Stub();

  inline ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel() { return channel_; }

  // implements EchoService ------------------------------------------

  void Echo(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::echo::EchoRequest* request,
                       ::echo::EchoResponse* response,
                       ::google::protobuf::Closure* done);
  void RelayEcho(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::echo::EchoRequest* request,
                       ::echo::EchoResponse* response,
                       ::google::protobuf::Closure* done);
 private:
  ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(EchoService_Stub);
};


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// EchoRequest

// optional string msg = 1;
inline bool EchoRequest::_internal_has_msg() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool EchoRequest::has_msg() const {
  return _internal_has_msg();
}
inline void EchoRequest::clear_msg() {
  msg_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& EchoRequest::msg() const {
  // @@protoc_insertion_point(field_get:echo.EchoRequest.msg)
  return _internal_msg();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void EchoRequest::set_msg(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000001u;
 msg_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:echo.EchoRequest.msg)
}
inline std::string* EchoRequest::mutable_msg() {
  std::string* _s = _internal_mutable_msg();
  // @@protoc_insertion_point(field_mutable:echo.EchoRequest.msg)
  return _s;
}
inline const std::string& EchoRequest::_internal_msg() const {
  return msg_.Get();
}
inline void EchoRequest::_internal_set_msg(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  msg_.Set(value, GetArenaForAllocation());
}
inline std::string* EchoRequest::_internal_mutable_msg() {
  _has_bits_[0] |= 0x00000001u;
  return msg_.Mutable(GetArenaForAllocation());
}
inline std::string* EchoRequest::release_msg() {
  // @@protoc_insertion_point(field_release:echo.EchoRequest.msg)
  if (!_internal_has_msg()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  auto* p = msg_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (msg_.IsDefault()) {
    msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void EchoRequest::set_allocated_msg(std::string* msg) {
  if (msg != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  msg_.SetAllocated(msg, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (msg_.IsDefault()) {
    msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:echo.EchoRequest.msg)
}

// -------------------------------------------------------------------

// EchoResponse

// optional string msg = 2;
inline bool EchoResponse::_internal_has_msg() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool EchoResponse::has_msg() const {
  return _internal_has_msg();
}
inline void EchoResponse::clear_msg() {
  msg_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& EchoResponse::msg() const {
  // @@protoc_insertion_point(field_get:echo.EchoResponse.msg)
  return _internal_msg();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void EchoResponse::set_msg(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000001u;
 msg_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:echo.EchoResponse.msg)
}
inline std::string* EchoResponse::mutable_msg() {
  std::string* _s = _internal_mutable_msg();
  // @@protoc_insertion_point(field_mutable:echo.EchoResponse.msg)
  return _s;
}
inline const std::string& EchoResponse::_internal_msg() const {
  return msg_.Get();
}
inline void EchoResponse::_internal_set_msg(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  msg_.Set(value, GetArenaForAllocation());
}
inline std::string* EchoResponse::_internal_mutable_msg() {
  _has_bits_[0] |= 0x00000001u;
  return msg_.Mutable(GetArenaForAllocation());
}
inline std::string* EchoResponse::release_msg() {
  // @@protoc_insertion_point(field_release:echo.EchoResponse.msg)
  if (!_internal_has_msg()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  auto* p = msg_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (msg_.IsDefault()) {
    msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void EchoResponse::set_allocated_msg(std::string* msg) {
  if (msg != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  msg_.SetAllocated(msg, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (msg_.IsDefault()) {
    msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:echo.EchoResponse.msg)
}

// -------------------------------------------------------------------

// GetDataReq

// optional string msg = 1;
inline bool GetDataReq::_internal_has_msg() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool GetDataReq::has_msg() const {
  return _internal_has_msg();
}
inline void GetDataReq::clear_msg() {
  msg_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& GetDataReq::msg() const {
  // @@protoc_insertion_point(field_get:echo.GetDataReq.msg)
  return _internal_msg();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void GetDataReq::set_msg(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000001u;
 msg_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:echo.GetDataReq.msg)
}
inline std::string* GetDataReq::mutable_msg() {
  std::string* _s = _internal_mutable_msg();
  // @@protoc_insertion_point(field_mutable:echo.GetDataReq.msg)
  return _s;
}
inline const std::string& GetDataReq::_internal_msg() const {
  return msg_.Get();
}
inline void GetDataReq::_internal_set_msg(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  msg_.Set(value, GetArenaForAllocation());
}
inline std::string* GetDataReq::_internal_mutable_msg() {
  _has_bits_[0] |= 0x00000001u;
  return msg_.Mutable(GetArenaForAllocation());
}
inline std::string* GetDataReq::release_msg() {
  // @@protoc_insertion_point(field_release:echo.GetDataReq.msg)
  if (!_internal_has_msg()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  auto* p = msg_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (msg_.IsDefault()) {
    msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void GetDataReq::set_allocated_msg(std::string* msg) {
  if (msg != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  msg_.SetAllocated(msg, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (msg_.IsDefault()) {
    msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:echo.GetDataReq.msg)
}

// -------------------------------------------------------------------

// GetDataRsp

// optional string msg = 1;
inline bool GetDataRsp::_internal_has_msg() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool GetDataRsp::has_msg() const {
  return _internal_has_msg();
}
inline void GetDataRsp::clear_msg() {
  msg_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& GetDataRsp::msg() const {
  // @@protoc_insertion_point(field_get:echo.GetDataRsp.msg)
  return _internal_msg();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void GetDataRsp::set_msg(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000001u;
 msg_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:echo.GetDataRsp.msg)
}
inline std::string* GetDataRsp::mutable_msg() {
  std::string* _s = _internal_mutable_msg();
  // @@protoc_insertion_point(field_mutable:echo.GetDataRsp.msg)
  return _s;
}
inline const std::string& GetDataRsp::_internal_msg() const {
  return msg_.Get();
}
inline void GetDataRsp::_internal_set_msg(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  msg_.Set(value, GetArenaForAllocation());
}
inline std::string* GetDataRsp::_internal_mutable_msg() {
  _has_bits_[0] |= 0x00000001u;
  return msg_.Mutable(GetArenaForAllocation());
}
inline std::string* GetDataRsp::release_msg() {
  // @@protoc_insertion_point(field_release:echo.GetDataRsp.msg)
  if (!_internal_has_msg()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  auto* p = msg_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (msg_.IsDefault()) {
    msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void GetDataRsp::set_allocated_msg(std::string* msg) {
  if (msg != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  msg_.SetAllocated(msg, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (msg_.IsDefault()) {
    msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:echo.GetDataRsp.msg)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace echo

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::echo::ErrorCode> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::echo::ErrorCode>() {
  return ::echo::ErrorCode_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_echo_2eproto
