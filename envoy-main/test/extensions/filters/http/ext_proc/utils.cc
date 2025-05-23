#include "test/extensions/filters/http/ext_proc/utils.h"

#include <string>
#include <utility>

#include "envoy/http/header_map.h"
#include "envoy/stream_info/stream_info.h"

#include "source/common/common/assert.h"
#include "source/common/common/macros.h"
#include "source/common/protobuf/protobuf.h"

#include "test/test_common/utility.h"

#include "absl/container/flat_hash_set.h"
#include "absl/status/status.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace ExternalProcessing {

const absl::flat_hash_set<std::string> ExtProcTestUtility::ignoredHeaders() {
  CONSTRUCT_ON_FIRST_USE(absl::flat_hash_set<std::string>, "x-request-id",
                         "x-envoy-upstream-service-time", "x-envoy-expected-rq-timeout-ms");
}

bool ExtProcTestUtility::headerProtosEqualIgnoreOrder(
    const Http::HeaderMap& expected, const envoy::config::core::v3::HeaderMap& actual) {
  // Comparing header maps is hard because they have duplicates in them.
  // So we're going to turn them into a HeaderMap and let Envoy do the work.
  Http::TestRequestHeaderMapImpl actual_headers;
  for (const auto& header : actual.headers()) {
    if (!ignoredHeaders().contains(header.key())) {
      actual_headers.addCopy(header.key(), header.raw_value());
    }
  }
  return TestUtility::headerMapEqualIgnoreOrder(expected, actual_headers);
}

envoy::config::core::v3::HeaderValue makeHeaderValue(const std::string& key,
                                                     const std::string& value) {
  envoy::config::core::v3::HeaderValue v;
  v.set_key(key);
  v.set_value(value);
  return v;
}

void TestOnProcessingResponse::afterProcessingRequestHeaders(
    const envoy::service::ext_proc::v3::HeadersResponse& response, absl::Status,
    Envoy::StreamInfo::StreamInfo& stream_info) {
  if (response.has_response() && response.response().has_header_mutation()) {
    stream_info.setDynamicMetadata("envoy-test-ext_proc-request_headers_response",
                                   getHeaderMutations(response.response().header_mutation()));
  }
}

void TestOnProcessingResponse::afterProcessingResponseHeaders(
    const envoy::service::ext_proc::v3::HeadersResponse& response, absl::Status,
    Envoy::StreamInfo::StreamInfo& stream_info) {
  if (response.has_response() && response.response().has_header_mutation()) {
    stream_info.setDynamicMetadata("envoy-test-ext_proc-response_headers_response",
                                   getHeaderMutations(response.response().header_mutation()));
  }
}

void TestOnProcessingResponse::afterProcessingRequestBody(
    const envoy::service::ext_proc::v3::BodyResponse& response, absl::Status,
    Envoy::StreamInfo::StreamInfo& stream_info) {
  if (response.has_response() && response.response().has_body_mutation()) {
    stream_info.setDynamicMetadata("envoy-test-ext_proc-request_body_response",
                                   getBodyMutation(response.response().body_mutation()));
  }
}

void TestOnProcessingResponse::afterProcessingResponseBody(
    const envoy::service::ext_proc::v3::BodyResponse& response, absl::Status,
    Envoy::StreamInfo::StreamInfo& stream_info) {
  if (response.has_response() && response.response().has_body_mutation()) {
    stream_info.setDynamicMetadata("envoy-test-ext_proc-response_body_response",
                                   getBodyMutation(response.response().body_mutation()));
  }
}

void TestOnProcessingResponse::afterProcessingRequestTrailers(
    const envoy::service::ext_proc::v3::TrailersResponse& response, absl::Status,
    Envoy::StreamInfo::StreamInfo& stream_info) {
  if (response.has_header_mutation()) {
    stream_info.setDynamicMetadata("envoy-test-ext_proc-request_trailers_response",
                                   getHeaderMutations(response.header_mutation()));
  }
}

void TestOnProcessingResponse::afterProcessingResponseTrailers(
    const envoy::service::ext_proc::v3::TrailersResponse& response, absl::Status,
    Envoy::StreamInfo::StreamInfo& stream_info) {
  if (response.has_header_mutation()) {
    stream_info.setDynamicMetadata("envoy-test-ext_proc-response_trailers_response",
                                   getHeaderMutations(response.header_mutation()));
  }
}

void TestOnProcessingResponse::afterReceivingImmediateResponse(
    const envoy::service::ext_proc::v3::ImmediateResponse& response, absl::Status,
    Envoy::StreamInfo::StreamInfo& stream_info) {
  if (response.has_headers()) {
    stream_info.setDynamicMetadata("envoy-test-ext_proc-response_immediate_response",
                                   getHeaderMutations(response.headers()));
  }
}
} // namespace ExternalProcessing
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
