#pragma once

#include "ofMain.h"
#include "ofLog.h"
#include "lsl_cpp.h"

using namespace lsl;

class ofxLSLSender {

public:
  ofxLSLSender(){}
  ~ofxLSLSender(){;}

  void addStream(const std::string &name, const std::string &type, int32_t channel_count=1, double nominal_srate=IRREGULAR_RATE, channel_format_t channel_format=cf_float32, const std::string &source_id=std::string()) {
    stream_info info(name, type, channel_count, nominal_srate, channel_format, source_id);
    addStream(info);
  }

  void addStream(const stream_info &_info) {
    // resolve and check if stream exists
    std::shared_ptr<stream_outlet> outlet = getOutlet(_info.name(), _info.type(), _info.source_id());
    if (outlet){
      ofLogWarning("ofxLSLSender::addStream") << "stream '" << _info.name() << "' with type '" << _info.type() << " and source ID '" << _info.source_id() << "' already exists";
      return;
    }

    ofLogNotice("ofxLSLSender::addStream") << "created stream '" << _info.name() << "' with type '" << _info.type() << " and source ID '" << _info.source_id() << "'";
    outlet = std::make_shared<stream_outlet>(_info);
    outlets.push_back((outlet));
  }


  template<typename T>
  void addSample(const vector<T> &_values, const std::string &name, const std::string &type, const std::string &source_id=std::string()) {

    std::shared_ptr<stream_outlet> outlet = getOutlet(name, type, source_id);
    if (!outlet){
        ofLogWarning("ofxLSLSender::addSample") << "stream '" << name << "' with type '" << type << " and source ID '" << source_id << "' does not exist";
        return;
    }

    if (outlet->info().channel_count() != _values.size()) {
      ofLogWarning("ofxLSLSender::addSample") << "value size '" << _values.size() << "' has to be equal to channel count '" << outlet->info().channel_count() << "'";
      return;
    }

    bool hasConsumers = outlet->have_consumers();

    outlet->push_sample(_values);
  }
    
    bool isConsumed(const std::string &name, const std::string &type, const std::string &source_id=std::string()) {
        return getOutlet(name, type, source_id)->have_consumers();
    }
    
private:
  std::vector<std::shared_ptr<stream_outlet>> outlets;

  std::shared_ptr<stream_outlet> getOutlet(const std::string &name, const std::string &type, const std::string &source_id=std::string()){
    for(auto outlet: outlets) {
      const auto& info = outlet->info();

      if(source_id.size()) {
        if (info.name() == name && info.type() == type && info.source_id() == source_id){
          return outlet;
        }
      } else if (info.name() == name && info.type() == type){
        return outlet;
      }
    }
    return nullptr;
  }

};
