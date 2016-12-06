/*
*  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef THIRD_PARTY_H264_WINRT_UTILS_SAMPLEATTRIBUTEQUEUE_H_
#define THIRD_PARTY_H264_WINRT_UTILS_SAMPLEATTRIBUTEQUEUE_H_

#include <stdint.h>
#include <queue>
#include <utility>
#include "webrtc/system_wrappers/include/critical_section_wrapper.h"
#include "webrtc/base/scoped_ptr.h"

// A sorted queue with certain properties which makes it
// good for mapping attributes to frames and samples.
// The ids have to be in increasing order.
template <typename T>
class SampleAttributeQueue {
 public:
  SampleAttributeQueue()
    : _lock(webrtc::CriticalSectionWrapper::CreateCriticalSection())
  {
  }
  ~SampleAttributeQueue() {}

  void push(uint64_t id, const T& t) {
    webrtc::CriticalSectionScoped csLock(_lock.get());
    _attributes.push(std::make_pair(id, t));
  }

  bool pop(uint64_t id, T& outT) {
    webrtc::CriticalSectionScoped csLock(_lock.get());
    while (!_attributes.empty()) {
      auto entry = _attributes.front();
      if (entry.first > id) {
        outT = entry.second;
        return true;
      } else if (entry.first == id) {
        outT = entry.second;
        _attributes.pop();
        return true;
      } else {
        _attributes.pop();
      }
    }
    return false;
  }

  void clear() {
    webrtc::CriticalSectionScoped csLock(_lock.get());
    while (!_attributes.empty()) {
      _attributes.pop();
    }
  }

  uint32_t size() {
    webrtc::CriticalSectionScoped csLock(_lock.get());
    return static_cast<uint32_t>(_attributes.size());
  }

private:
   rtc::scoped_ptr<webrtc::CriticalSectionWrapper> _lock;
   std::queue<std::pair<uint64_t, const T>> _attributes;
};

#endif  // THIRD_PARTY_H264_WINRT_UTILS_SAMPLEATTRIBUTEQUEUE_H_