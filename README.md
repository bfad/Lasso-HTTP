# Lasso HTTP

This is meant to be an Object-Oriented replacement for include_url, especially
for accessing Web APIs. It is essentially a nice wrapper for the built-in curl
type to do HTTP requests. It comes with two types: http_request and
http_response. This allows for creating requests that can be later inspected
and modified as well as a nice response object for parsing raw HTTP responses.

Example Usage:

```lasso
    local(req) = http_request(
        "http://example.com/foo",
        -postParams = (:'name'='Rhino'),
        -reqMethod  = `PUT`
    )
    local(resp) = #req->response
    fail_if(#resp->getStatus != 200, #resp->statusCode, #resp->statusMsg)
    #resp->bodyString
```


## Basic Installation

The easiest way to install these files is to place the http_request.lasso and
http_response.lasso files into either your instance's LassoStartup folder or
your instances' LASSO9_MASTER_HOME LassoStartup folder.

If you need further help for this, or a different installation method (such as
compiling them into a dynamic library), please post your question to
[StackOverflow](http://stackoverflow.com/questions/ask) or the
[LassoTalk](http://www.lassotalk.com) list.


## Building Requests

The http_request type allows you to easily build a curl request through many
steps. This means you don't have to have everything ready for the request and
can even delay in making the request:

```lasso
    local(req) = http_request("http://example.com/foo")

    #req->headers    = (:'Content-Type'='application/json')
    #req->postParams = json_serialize(map('moose'='hair'))
    #req->timeout    = 300
```

Alternatively, if you have everything ready, you can pass all the data to the
creator method:

```lasso
    local(req) = http_request(
        "http://example.com/foo",
        -postParams = (:'name'='Rhino'),
        -reqMethod  = `PUT`
    )
```


## Inspecting Responses

Once you have a request built, you can get the result back in an http_response
type and easily inspect the various parts:

```lasso
    // If the request doesn't return with a 200 status code,
    // error with the status and message.
    // Otherwise show the body of the HTTP response
    local(resp) = #req->response
    fail_if(#resp->getStatus != 200, #resp->statusCode, #resp->statusMsg)
    #resp->bodyString
```


## Full Example

More often then not, this is probably the form your code will take:

```lasso
    local(resp) = http_request(
        "http://example.com/foo",
        -postParams = (:'name'='Rhino'),
        -reqMethod  = `PUT`
    )->response

    match(#resp->statusCode) => {
    case(400)
        content_body = 'Malformed Request'
    case(401)
        content_body = 'Credentials Expired'
    case(200)
        content_body = #resp->bodyString
    case
        fail(#resp->statusCode, #resp->statusMsg)
    }
```


## Feedback

Please use Github Issues area for bug reports and suggestions.


## License

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