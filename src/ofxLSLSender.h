#pragma once

#include "lsl_cpp.h"
#include "ofLog.h"
#include "ofMain.h"

using namespace lsl;

namespace ofxLSL {
class Sender {
 public:
  Sender() : bUseAppTime(false) {}
  ~Sender() { ; }

  void useAppTime(const bool& value) { bUseAppTime = value;}

  void addStream(const std::string &name,
                 const std::string &type,
                 int32_t channel_count = 1,
                 double nominal_srate = IRREGULAR_RATE,
                 channel_format_t channel_format = cf_float32,
                 const std::string &source_id = std::string())
  {
    stream_info info(name, type, channel_count, nominal_srate, channel_format, source_id);
    addStream(info);
  }

  void addStream(const stream_info &_info) {
    // resolve and check if stream exists
    auto outlet = getOutlet(_info.name(), _info.type(), _info.source_id());
    if (outlet) {
      ofLogWarning("ofxLSLSender::addStream")
        << "stream '" << _info.name() << "' of type '" << _info.type()
        << "' and ID '" << _info.source_id() << "' already exists";
      return;
    }
    outlet = std::make_shared<stream_outlet>(_info);
    ofLogNotice("ofxLSLSender::addStream")
        << "created stream '" << _info.name() << "' of type '" << _info.type()
        << "' and ID '" << _info.source_id() << "'";
    outlets.push_back((outlet));
  }

  template <typename T>
  void addSample(const vector<T> &_values, const std::string &name,
                 const std::string &type,
                 const std::string &source_id = std::string()) {
    auto outlet = getOutlet(name, type, source_id);
    if (!outlet) {
      ofLogWarning("ofxLSLSender::addSample")
          << "stream '" << name << "' of type '" << type << " and ID '"
          << source_id << "' does not exist";
      return;
    }

    if (outlet->info().channel_count() != _values.size()) {
      ofLogWarning("ofxLSLSender::addSample")
          << "value size '" << _values.size()
          << "' has to be equal to channel count '"
          << outlet->info().channel_count() << "'";
      return;
    }

    bool hasConsumers = outlet->have_consumers();

    if (bUseAppTime) outlet->push_sample(_values, ofGetElapsedTimef());
    else outlet->push_sample(_values);
  }

  bool isConsumed(const std::string &name, const std::string &type,
                  const std::string &source_id = std::string()) {
    return getOutlet(name, type, source_id)->have_consumers();
  }

  int getNumStreams() { return outlets.size(); }

  int getNumStreamsConnected() {
    int numStreamsConnected = 0;
    for (const auto &outlet : outlets) {
      if (outlet->have_consumers()) {
        numStreamsConnected++;
      }
    }
    return numStreamsConnected;
  }

 private:
 bool bUseAppTime;
  std::vector<std::shared_ptr<stream_outlet>> outlets;

  std::shared_ptr<stream_outlet> getOutlet(
    const std::string &name, const std::string &type,
    const std::string &source_id = std::string())
  {
    for (auto outlet : outlets) {
      const auto &info = outlet->info();
      if (source_id.size()) {
        if (info.name() == name && info.type() == type && info.source_id() == source_id) {
          return outlet;
        }
      }
    }
    return nullptr;
  }
};
}
