// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_SWBUFF_SOULMETERFBS_HISTORY_H_
#define FLATBUFFERS_GENERATED_SWBUFF_SOULMETERFBS_HISTORY_H_

#include "flatbuffers/flatbuffers.h"

namespace SoulMeterFBS {
namespace History {

struct _tBuff;
struct _tBuffBuilder;

struct _tBuffInfo;
struct _tBuffInfoBuilder;

struct _tBuff FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef _tBuffBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT__ID = 4,
    VT__BUFF_INFO = 6
  };
  uint32_t _id() const {
    return GetField<uint32_t>(VT__ID, 0);
  }
  const flatbuffers::Vector<flatbuffers::Offset<SoulMeterFBS::History::_tBuffInfo>> *_buff_info() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<SoulMeterFBS::History::_tBuffInfo>> *>(VT__BUFF_INFO);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint32_t>(verifier, VT__ID) &&
           VerifyOffset(verifier, VT__BUFF_INFO) &&
           verifier.VerifyVector(_buff_info()) &&
           verifier.VerifyVectorOfTables(_buff_info()) &&
           verifier.EndTable();
  }
};

struct _tBuffBuilder {
  typedef _tBuff Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add__id(uint32_t _id) {
    fbb_.AddElement<uint32_t>(_tBuff::VT__ID, _id, 0);
  }
  void add__buff_info(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SoulMeterFBS::History::_tBuffInfo>>> _buff_info) {
    fbb_.AddOffset(_tBuff::VT__BUFF_INFO, _buff_info);
  }
  explicit _tBuffBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<_tBuff> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<_tBuff>(end);
    return o;
  }
};

inline flatbuffers::Offset<_tBuff> Create_tBuff(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t _id = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<SoulMeterFBS::History::_tBuffInfo>>> _buff_info = 0) {
  _tBuffBuilder builder_(_fbb);
  builder_.add__buff_info(_buff_info);
  builder_.add__id(_id);
  return builder_.Finish();
}

inline flatbuffers::Offset<_tBuff> Create_tBuffDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint32_t _id = 0,
    const std::vector<flatbuffers::Offset<SoulMeterFBS::History::_tBuffInfo>> *_buff_info = nullptr) {
  auto _buff_info__ = _buff_info ? _fbb.CreateVector<flatbuffers::Offset<SoulMeterFBS::History::_tBuffInfo>>(*_buff_info) : 0;
  return SoulMeterFBS::History::Create_tBuff(
      _fbb,
      _id,
      _buff_info__);
}

struct _tBuffInfo FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef _tBuffInfoBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT__BUFF_ID = 4,
    VT__STACK = 6,
    VT__TIME = 8
  };
  uint16_t _buff_id() const {
    return GetField<uint16_t>(VT__BUFF_ID, 0);
  }
  int8_t _stack() const {
    return GetField<int8_t>(VT__STACK, 0);
  }
  float _time() const {
    return GetField<float>(VT__TIME, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint16_t>(verifier, VT__BUFF_ID) &&
           VerifyField<int8_t>(verifier, VT__STACK) &&
           VerifyField<float>(verifier, VT__TIME) &&
           verifier.EndTable();
  }
};

struct _tBuffInfoBuilder {
  typedef _tBuffInfo Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add__buff_id(uint16_t _buff_id) {
    fbb_.AddElement<uint16_t>(_tBuffInfo::VT__BUFF_ID, _buff_id, 0);
  }
  void add__stack(int8_t _stack) {
    fbb_.AddElement<int8_t>(_tBuffInfo::VT__STACK, _stack, 0);
  }
  void add__time(float _time) {
    fbb_.AddElement<float>(_tBuffInfo::VT__TIME, _time, 0.0f);
  }
  explicit _tBuffInfoBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<_tBuffInfo> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<_tBuffInfo>(end);
    return o;
  }
};

inline flatbuffers::Offset<_tBuffInfo> Create_tBuffInfo(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint16_t _buff_id = 0,
    int8_t _stack = 0,
    float _time = 0.0f) {
  _tBuffInfoBuilder builder_(_fbb);
  builder_.add__time(_time);
  builder_.add__buff_id(_buff_id);
  builder_.add__stack(_stack);
  return builder_.Finish();
}

inline const SoulMeterFBS::History::_tBuff *Get_tBuff(const void *buf) {
  return flatbuffers::GetRoot<SoulMeterFBS::History::_tBuff>(buf);
}

inline const SoulMeterFBS::History::_tBuff *GetSizePrefixed_tBuff(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<SoulMeterFBS::History::_tBuff>(buf);
}

inline bool Verify_tBuffBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<SoulMeterFBS::History::_tBuff>(nullptr);
}

inline bool VerifySizePrefixed_tBuffBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<SoulMeterFBS::History::_tBuff>(nullptr);
}

inline void Finish_tBuffBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<SoulMeterFBS::History::_tBuff> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixed_tBuffBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<SoulMeterFBS::History::_tBuff> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace History
}  // namespace SoulMeterFBS

#endif  // FLATBUFFERS_GENERATED_SWBUFF_SOULMETERFBS_HISTORY_H_
