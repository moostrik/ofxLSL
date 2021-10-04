#include "ofxLSLReceiver.h"

ofxLSLReceiver::ofxLSLReceiver() : active(false), containerCapacity(100) {}

bool ofxLSLReceiver::start() {
  if (active) return false;
  active = true;

  resolver = make_unique<continuous_resolver>();
  connectThread = std::make_unique<std::thread>(&ofxLSLReceiver::connect, this);
  pullThread = std::make_unique<std::thread>(&ofxLSLReceiver::pull, this);

  return true;
}

bool ofxLSLReceiver::stop() {
  if (!active) return false;
  active = false;
  disconnect();


  connectThread->join();
  connectThread = nullptr;
  pullThread->join();
  pullThread = nullptr;
  resolver = nullptr;

  return true;
}

void ofxLSLReceiver::connect() {
  while (active) {
    std::vector<stream_info> resolved_infos = resolver->results();
    // add new streams
    for (auto& info : resolved_infos) {
      bool inletFound = false;
      for (auto& inlet : inlets) {
        if (isEqual(info, inlet->info())) {
          inletFound = true;
          ;
        }
      }

      if (!inletFound) {
        ofLogNotice("ofxLSLReceiver::connect")
            << "open stream'" << info.name() << "' with type '" << info.type()
            << " and source ID '" << info.source_id() << "'";
        auto inlet  = std::make_unique<stream_inlet>(info);
        inlet->open_stream();

        std::lock_guard<std::mutex> lock(connectMutex);
        inlets.emplace_back(std::move(inlet));

      }
    }

    // remove disconnected streams
    for (int i = inlets.size() - 1; i >= 0; i--) {
      auto& inlet = inlets[i];
      bool found = false;
      const auto& iI = inlet->info();
      for (auto& rI : resolved_infos) {
        if (isEqual(iI, rI)) {
          found = true;
        }
      }
      if (!found) {
        ofLogNotice("ofxLSLReceiver::connect")
            << "lost stream '" << iI.name() << "' with type '" << iI.type()
            << " and source ID '" << iI.source_id() << "'";
        std::lock_guard<std::mutex> lock(connectMutex);
        inlets.erase(inlets.begin() + i);
      }
    }

    sleep(500);
  }
}

void ofxLSLReceiver::disconnect() {
  {
    std::lock_guard<std::mutex> lock(connectMutex);
    for (auto& inlet : inlets) {
      inlet->close_stream();
    }
    inlets.clear();
  }
  {
    std::lock_guard<std::mutex> lock(pullMutex);
    containers.clear();
  }
}

bool ofxLSLReceiver::isConnected() {
  std::lock_guard<std::mutex> lock(connectMutex);
  return inlets.size() > 0;
}

std::vector<ofxLSLSample> ofxLSLReceiver::flush(stream_info info) {
  std::vector<ofxLSLSample> currentBuffer;
  auto container = getContainer(info);
  if (container) {
    std::lock_guard<std::mutex> lock(pullMutex);
    currentBuffer = std::vector<ofxLSLSample>(container->samples.begin(),
                                              container->samples.end());
    container->samples.clear();
  }
  return currentBuffer;
};

void ofxLSLReceiver::pull() {
  std::unique_lock<std::mutex> pullLock(pullMutex);
  std::chrono::microseconds timeout(100);

  while (active) {

    std::lock_guard<std::mutex> lock(connectMutex);

    for (auto& inlet : inlets) {
      std::vector<float> sampleBuffer;
      double ts = inlet->pull_sample(sampleBuffer, 0.0);
      const auto& info = inlet->info();

      ofxLSLSample sample;
      sample.timestamp = ts;

      if (ts > 0) {
        sample.sample =
            std::vector<float>(sampleBuffer.begin(), sampleBuffer.end());

//        std::lock_guard<std::mutex> lock(pullMutex);

        auto container = getContainer(info);
        if (!container) {
          containers.emplace_back(std::make_shared<ofxLSLContainer>());
          container = containers.back();
          container->info = info;
        }
        auto& samples = container->samples;
        samples.push_back(sample);

        while (samples.size() && samples.size() > containerCapacity) {
          //        ofLogWarning() << "Buffer capacity reached, erasing
          //        samples";
          samples.erase(samples.begin());
        }
      }
    }

    pullSignal.wait_for(pullLock, timeout, [&] { return !active.load(); });
  }
}

vector<stream_info> ofxLSLReceiver::getStreamNames() {
  std::lock_guard<std::mutex> lock(pullMutex);
  vector<stream_info> infos;
  for (const auto& c : containers) {
    infos.push_back(c->info);
  }
  return infos;
}

std::shared_ptr<ofxLSLContainer> ofxLSLReceiver::getContainer(stream_info _info) {
  for (auto container : containers) {
    if (isEqual(_info, container->info)) {
      return container;
    }
  }
  return nullptr;
}

bool ofxLSLReceiver::isEqual(stream_info _infoA, stream_info _infoB) {
  if (_infoA.name() == _infoB.name() && _infoA.type() == _infoB.type() &&
      _infoA.source_id() == _infoB.source_id()) {
    return true;
  }
  return false;
}
