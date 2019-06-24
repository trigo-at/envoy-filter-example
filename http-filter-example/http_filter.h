#pragma once

#include <string>

#include "envoy/server/filter_config.h"
#include "common/common/logger.h"
#include "http-filter-example/http_filter.pb.h"

namespace Envoy {
namespace Http {

class HttpSampleDecoderFilterConfig {
public:
  HttpSampleDecoderFilterConfig(const sample::Decoder& proto_config);

  const std::string& key() const { return key_; }
  const std::string& val() const { return val_; }

private:
  const std::string key_;
  const std::string val_;
};

typedef std::shared_ptr<HttpSampleDecoderFilterConfig> HttpSampleDecoderFilterConfigSharedPtr;

class HttpSampleDecoderFilter : public StreamEncoderFilter {
public:
  HttpSampleDecoderFilter(HttpSampleDecoderFilterConfigSharedPtr);
  ~HttpSampleDecoderFilter();

  // Http::StreamFilterBase
  void onDestroy() override;

  FilterHeadersStatus encode100ContinueHeaders(HeaderMap& headers) override;
  FilterHeadersStatus encodeHeaders(HeaderMap&, bool) override;
  FilterDataStatus encodeData(Buffer::Instance&, bool) override;
  FilterTrailersStatus encodeTrailers(HeaderMap&) override;
  FilterMetadataStatus encodeMetadata(MetadataMap& metadata_map) override;

  void setEncoderFilterCallbacks(StreamEncoderFilterCallbacks&) override;

private:
  const HttpSampleDecoderFilterConfigSharedPtr config_;
  StreamEncoderFilterCallbacks* encoder_callbacks_;

  std::string jsonBody{""};
};

} // namespace Http
} // namespace Envoy
