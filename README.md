# Lasso HTTP

This is meant to be an Object-Oriented replacement for include_url, especially
for accessing Web APIs. It is essentially a nice wrapper for the built-in curl
type to do HTTP requests. It comes with two types: http_request and
http_response. This allows for creating requests that can be later inspected
and modified as well as a nice response opject for parsing raw HTTP responses.

Example Usage:

    local(req) = http_request(
        "http://example.com/foo",
        -postParams = (:'name'='Rhino'),
        -reqMethod  = `PUT`
    )
    local(resp) = #req->response
    fail_if(#resp->getStatus != 200, #resp->statusCode, #resp->statusMsg)
    #resp->bodyString

This is still under development, so bug reports and suggestions are welcome.


# License

Copyright 2013 Bradley Lindsay

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.