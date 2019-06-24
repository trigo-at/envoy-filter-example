#include <string>

#include "http_filter.h"

#include "envoy/server/filter_config.h"
#include "common/common/assert.h"
#include "common/common/logger.h"
#include "envoy/buffer/buffer.h"
#include "envoy/network/connection.h"
#include "envoy/json/json_object.h"
#include "envoy/buffer/buffer.h"
#include "common/buffer/buffer_impl.h"

// Do not let RapidJson leak outside of this file.
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/reader.h"
#include "rapidjson/schema.h"
#include "rapidjson/stream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


namespace Envoy {
namespace Http {

HttpSampleDecoderFilterConfig::HttpSampleDecoderFilterConfig(
    const sample::Decoder& proto_config)
    : key_(proto_config.key()), val_(proto_config.val()) {}

HttpSampleDecoderFilter::HttpSampleDecoderFilter(HttpSampleDecoderFilterConfigSharedPtr config)
    : config_(config) {}

HttpSampleDecoderFilter::~HttpSampleDecoderFilter() {}

void HttpSampleDecoderFilter::onDestroy() {}


FilterHeadersStatus HttpSampleDecoderFilter::encode100ContinueHeaders(HeaderMap&) {
  std::cout << "FilterHeadersStatus HttpSampleDecoderFilter::encode100ContinueHeaders(HeaderMap& headers, bool)" << std::endl; 
  
  return FilterHeadersStatus::Continue;
}


FilterMetadataStatus HttpSampleDecoderFilter::encodeMetadata(MetadataMap&) {
  std::cout << "FilterHeadersStatus HttpSampleDecoderFilter::encodeMetadata(MetadataMap&)" << std::endl; 

  return FilterMetadataStatus::Continue;
}

FilterHeadersStatus HttpSampleDecoderFilter::encodeHeaders(HeaderMap& headers, bool end_stream) {
  std::cout << "FilterHeadersStatus HttpSampleDecoderFilter::encodeHeaders(HeaderMap& headers, bool)" << std::endl; 
  if (end_stream) return FilterHeadersStatus::Continue;

  std::cout << "Remove original content length..." << headers.ContentLength()->value().getStringView() << std::endl;
  headers.removeContentLength();
  
  return FilterHeadersStatus::Continue;
}

FilterTrailersStatus HttpSampleDecoderFilter::encodeTrailers(HeaderMap& headers) {
  std::cout << "HttpSampleDecoderFilter::encodeTrailers(HeaderMap&)" << std::endl; 
  
  if (jsonBody.length()) {
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(jsonBody.c_str()); 
    jsonDoc.RemoveMember("type");

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    jsonDoc.Accept(writer);

    std::cout << "Filtered JSON: " << buffer.GetString() << "length:" << buffer.GetSize() << std::endl;

    Buffer::OwnedImpl empty_buffer;
    empty_buffer.add(buffer.GetString());
    
    headers.insertContentLength().value(buffer.GetSize());
    
    encoder_callbacks_->addEncodedData(empty_buffer, false);
  }
  
  return FilterTrailersStatus::Continue;
}

FilterDataStatus HttpSampleDecoderFilter::encodeData(Buffer::Instance& data, bool end_stream) {
  std::cout << "HttpSampleDecoderFilter::encodeData(Buffer::Instance& data, bool end_stream)" << std::endl; 
  if (end_stream) return FilterDataStatus::Continue;


  if (data.length()) {
    std::cout << "HAVE DATA!" << std::endl;

    jsonBody = data.toString();
    std::cout << "Original JSON" << jsonBody << jsonBody.c_str() <<  std::endl;
    
  }

  return FilterDataStatus::StopIterationAndBuffer;
}

void HttpSampleDecoderFilter::setEncoderFilterCallbacks(StreamEncoderFilterCallbacks& callbacks) {
    std::cout << "HttpSampleDecoderFilter::setEncoderFilterCallbacks(StreamEncoderFilterCallbacks& callbacks)" << std::endl; 
  encoder_callbacks_ = &callbacks;
}

} // namespace Http
} // namespace Envoy
