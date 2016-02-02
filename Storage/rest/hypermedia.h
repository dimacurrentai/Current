/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2016 Maxim Zhurovich <zhurovich@gmail.com>
          (c) 2016 Dmitry "Dima" Korolev <dmitry.korolev@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#ifndef CURRENT_STORAGE_REST_HYPERMEDIA_H
#define CURRENT_STORAGE_REST_HYPERMEDIA_H

#include "../storage.h"

#include "../../TypeSystem/struct.h"
#include "../../Blocks/HTTP/api.h"

CURRENT_STRUCT(HypermediaRESTTopLevel) {
  CURRENT_FIELD(url_healthz, std::string);
  CURRENT_FIELD(api, (std::map<std::string, std::string>));
  CURRENT_FIELD(build, std::string, __DATE__ " " __TIME__);
};

static const std::chrono::microseconds startup_time_us = current::time::Now();

CURRENT_STRUCT(HypermediaRESTHealthz) {
  CURRENT_FIELD(url, std::string);
  CURRENT_FIELD(up, bool, true);
  CURRENT_FIELD(server_time_current, std::string);
  CURRENT_FIELD(server_time_spawned, std::string);
  CURRENT_FIELD(uptime_us, std::chrono::microseconds);
  CURRENT_DEFAULT_CONSTRUCTOR(HypermediaRESTHealthz) {
    const std::chrono::microseconds now_us = current::time::Now();
    std::time_t t;
    using TP = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;
    t = std::chrono::system_clock::to_time_t(TP(now_us));
    server_time_current = std::ctime(&t);
    t = std::chrono::system_clock::to_time_t(TP(startup_time_us));
    server_time_spawned = std::ctime(&t);
    uptime_us = now_us - startup_time_us;
  }
};

CURRENT_STRUCT(HypermediaRESTContainerResponse) {
  CURRENT_FIELD(url, std::string);
  CURRENT_FIELD(data, std::vector<std::string>);
};

CURRENT_STRUCT(HypermediaRESTError) {
  CURRENT_FIELD(error, std::string);
  CURRENT_CONSTRUCTOR(HypermediaRESTError)(const std::string& error) : error(error) {}
};

CURRENT_STRUCT(HypermediaRESTParseJSONError, HypermediaRESTError) {
  CURRENT_FIELD(json_details, std::string);
  CURRENT_CONSTRUCTOR(HypermediaRESTParseJSONError)(const std::string& error, const std::string& json_details)
      : SUPER(error), json_details(json_details) {}
};

namespace current {
namespace storage {
namespace rest {

struct Hypermedia {
  template <class HTTP_VERB, typename ALL_FIELDS, typename PARTICULAR_FIELD, typename ENTRY, typename KEY>
  struct RESTful;

  static void RegisterTopLevel(HTTPRoutesScope& scope,
                               const std::vector<std::string>& fields,
                               int port,
                               const std::string& path_prefix,
                               const std::string& restful_url_prefix) {
    scope += HTTP(port).Register(path_prefix,
                                 [fields, restful_url_prefix](Request request) {
                                   HypermediaRESTTopLevel response;
                                   response.url_healthz = restful_url_prefix + "/healthz";
                                   for (const auto& f : fields) {
                                     response.api[f] = restful_url_prefix + '/' + f;
                                   }
                                   request(response);
                                 });
    scope += HTTP(port).Register(path_prefix == "/" ? "/healthz" : path_prefix + "/healthz",
                                 [restful_url_prefix](Request request) {
                                   HypermediaRESTHealthz response;
                                   response.url = restful_url_prefix + "/healthz";
                                   request(response);
                                 });
  }

  template <typename F_WITH, typename F_WITHOUT>
  static void WithOrWithoutKeyFromURL(Request request, F_WITH&& with, F_WITHOUT&& without) {
    if (request.url.query.has("key")) {
      with(std::move(request), request.url.query["key"]);
    } else if (!request.url_path_args.empty()) {
      with(std::move(request), request.url_path_args[0]);
    } else {
      without(std::move(request));
    }
  }

  template <typename F>
  static void WithKeyFromURL(Request request, F&& next_with_key) {
    WithOrWithoutKeyFromURL(
        std::move(request),
        next_with_key,
        [](Request request) { request("Need resource key in the URL.\n", HTTPResponseCode.BadRequest); });
  }

  template <typename F>
  static void WithOptionalKeyFromURL(Request request, F&& next) {
    WithOrWithoutKeyFromURL(
        std::move(request), next, [&next](Request request) { next(std::move(request), ""); });
  }

  template <typename ALL_FIELDS, typename PARTICULAR_FIELD, typename ENTRY, typename KEY>
  struct RESTful<GET, ALL_FIELDS, PARTICULAR_FIELD, ENTRY, KEY> {
    template <typename F>
    void Enter(Request request, F&& next) {
      WithOptionalKeyFromURL(std::move(request), std::forward<F>(next));
    }
    template <class INPUT>
    Response Run(const INPUT& input) const {
      if (!input.url_key.empty()) {
        const ImmutableOptional<ENTRY> result = input.field[FromString<KEY>(input.url_key)];
        if (Exists(result)) {
          return Value(result);
        } else {
          return Response(HypermediaRESTError("Resource not found."), HTTPResponseCode.NotFound);
        }
      } else {
        HypermediaRESTContainerResponse response;
        response.url = input.restful_url_prefix + '/' + input.field_name;
        for (const auto& element : input.field) {
          response.data.emplace_back(response.url + '/' + ToString(sfinae::GetKey(element)));
        }
        return Response(response);
      }
    }
  };

  template <typename ALL_FIELDS, typename PARTICULAR_FIELD, typename ENTRY, typename KEY>
  struct RESTful<POST, ALL_FIELDS, PARTICULAR_FIELD, ENTRY, KEY> {
    template <typename F>
    void Enter(Request request, F&& next) {
      if (!request.url_path_args.empty()) {
        request(HypermediaRESTError("Should not have resource key in the URL."), HTTPResponseCode.BadRequest);
      } else {
        next(std::move(request));
      }
    }
    template <class INPUT>
    Response Run(const INPUT& input) const {
      input.entry.InitializeOwnKey();
      if (!Exists(input.field[sfinae::GetKey(input.entry)])) {
        input.field.Add(input.entry);
        // TODO(dkorolev): Return a JSON with a resource here.
        return Response(ToString(sfinae::GetKey(input.entry)), HTTPResponseCode.Created);
      } else {
        return Response(HypermediaRESTError("The key generated for entry already exists"),
                        HTTPResponseCode.Conflict);
      }
    }
    static Response ErrorBadJSON(const std::string& error_message) {
      return Response(HypermediaRESTParseJSONError("Invalid JSON in request body.", error_message),
                      HTTPResponseCode.BadRequest);
    }
  };

  template <typename ALL_FIELDS, typename PARTICULAR_FIELD, typename ENTRY, typename KEY>
  struct RESTful<PUT, ALL_FIELDS, PARTICULAR_FIELD, ENTRY, KEY> {
    template <typename F>
    void Enter(Request request, F&& next) {
      WithKeyFromURL(std::move(request), std::forward<F>(next));
    }
    template <class INPUT>
    Response Run(const INPUT& input) const {
      if (input.entry_key == input.url_key) {
        const bool exists = Exists(input.field[input.entry_key]);
        input.field.Add(input.entry);
        // TODO(dkorolev): Return a JSON with a resource here.
        if (exists) {
          return Response("Updated.\n", HTTPResponseCode.OK);
        } else {
          return Response("Created.\n", HTTPResponseCode.Created);
        }
      } else {
        return Response(HypermediaRESTError("Object key doesn't match URL key."), HTTPResponseCode.BadRequest);
      }
    }
    static Response ErrorBadJSON(const std::string& error_message) {
      return Response(HypermediaRESTParseJSONError("Invalid JSON in request body.", error_message),
                      HTTPResponseCode.BadRequest);
    }
  };
  template <typename ALL_FIELDS, typename PARTICULAR_FIELD, typename ENTRY, typename KEY>
  struct RESTful<DELETE, ALL_FIELDS, PARTICULAR_FIELD, ENTRY, KEY> {
    template <typename F>
    void Enter(Request request, F&& next) {
      WithKeyFromURL(std::move(request), std::forward<F>(next));
    }
    template <class INPUT>
    Response Run(const INPUT& input) const {
      input.field.Erase(input.key);
      // TODO(dkorolev): Return a JSON with something more useful here.
      return Response("Deleted.\n", HTTPResponseCode.OK);
    }
  };

  static Response ErrorMethodNotAllowed() {
    return Response(HypermediaRESTError("Method not allowed."), HTTPResponseCode.MethodNotAllowed);
  }
};

}  // namespace rest
}  // namespace storage
}  // namespace current

#endif  // CURRENT_STORAGE_REST_HYPERMEDIA_H