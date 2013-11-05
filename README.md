# HTTP_Response

This is meant to be an Object-Oriented replacement for include_url, especially
for accessing Web APIs. It is essentially a nice wrapper for the built-in curl
type to do HTTP requests.

Example Usage:

    local(resp) = http_response(
        "http://example.com/foo",
        -postParams = (:'name'='Rhino'),
        -reqMethod  = `PUT`
    )
    fail_if(#resp->getStatus != 200, #resp->statusCode, #resp->statusMsg)
    #resp->bodyAsString



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