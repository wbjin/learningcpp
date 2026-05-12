#include <iostream>
#include <optional>
#include <unordered_map>

/*
 * An example of a complex object that is a headache to build all at once with a
 * constructor
 * */
struct HTTPRequest {
  std::unordered_map<std::string, std::string> headers{};
  std::string method{};
  std::string url{};
};

/*
 * A builder that is in charge of building said complex object. The builder is
 * also responsible for validating the object that is being built
 * */
class HTTPRequestBuilder {
public:
  HTTPRequestBuilder &SetMethod(const std::string &method) {
    std::cout << "HTTPRequestBuilder::SetMethod(" << method << ")\n";
    req_.method = method;
    return *this;
  }

  HTTPRequestBuilder &SetUrl(const std::string &url) {
    std::cout << "HTTPRequestBuilder::SetUrl(" << url << ")\n";
    req_.url = url;
    return *this;
  }

  HTTPRequestBuilder &
  SetHeaders(const std::unordered_map<std::string, std::string> &headers) {
    std::cout << "HTTPRequestBuilder::SetHeaders()\n";
    req_.headers = headers;
    return *this;
  }

  std::optional<HTTPRequest> Build() {
    // assume more complex validation logic
    if (req_.url.empty())
      return std::nullopt;
    if (req_.method.empty())
      return std::nullopt;
    if (req_.headers.empty())
      return std::nullopt;

    return req_;
  }

private:
  HTTPRequest req_{};
};

int main() {
  HTTPRequestBuilder builder{};
  auto request = builder.SetMethod("GET")
                     .SetUrl("https://google.com")
                     .SetHeaders({{"Authorization", "1234567"}})
                     .Build();

  if (!request)
    std::cout << "Invalid http header\n";
}
