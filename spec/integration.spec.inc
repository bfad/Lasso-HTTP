local(here) = currentCapture->callSite_file->stripLastComponent + '/'
not #here->beginsWith('/') or #here->size == 1
    ? #here = io_file_getcwd + '/' + #here
if(not var_defined('_prep4tests')) => {
    sourcefile(file(#here + './spec_helper.inc'), -autoCollect=false)->invoke
}


local(myHost) = `localhost:9000`

describe(`basic HTTP requests`) => {
    it(`successfully connects and requests the specified URL`) => {
        local(resp) = http_request(#myHost)->response

        expect(200 , #resp->statusCode)
        expect(`OK`, #resp->statusMsg)
        expect(#resp->body >> "GET / HTTP/1.1\r\n")
        expect(#resp->body >> "\r\nHost: " + #myHost + "\r\n")
    }

    it(`requests the correct URL path`) => {
        local(resp) = http_request(#myHost + `/rhino`)->response

        expect(#resp->body->beginsWith("GET /rhino HTTP/1.1\r\n"))
    }
}


describe(`Requests with GET parameters`) => {
     it(`requests the correct URL when GET parameters in URL string`) => {
        local(resp) = http_request(#myHost + `/rhino?run=1`)->response

        expect(#resp->body->beginsWith("GET /rhino?run=1 HTTP/1.1\r\n"))
    }

    it(`requests the correct URL when GET parameters are passed as params`) => {
        local(resp) = http_request(
            #myHost + `/rhino`,
            -getParams = (:`run`=1, `id`=`silver`)
        )->response

        expect(#resp->body->beginsWith("GET /rhino?run=1&id=silver HTTP/1.1\r\n"))
    }

    it(`requests the correct URL when GET parameters in both URL string and as params`) => {
        local(resp) = http_request(
            #myHost + `/rhino?run=1`,
            -getParams = (:`id`=`silver`)
        )->response

        expect(#resp->body->beginsWith("GET /rhino?run=1&id=silver HTTP/1.1\r\n"))
    }
}

describe(`Requests with POST params passed in`) => {
    it(`Sets the HTTP method to POST and the request body to the passed string data`) => {
        local(resp) = http_request(
            #myHost,
            -postParams='data'
        )->response

        expect(#resp->body->beginsWith("POST"))
        expect(`data`, #resp->body->split("\r\n\r\n")->second)
    }

    it(`Sets the request body to URL encoded data from array of pairs`) => {
        local(resp) = http_request(
            #myHost,
            -postParams=(:`run`=`for it`, `marty`=`ball`)
        )->response

        expect(`run=for%20it&marty=ball`, #resp->body->split("\r\n\r\n")->second)
    }
}


describe(`Creating PUT requests`) => {
    it(`Allows for setting up A PUT request`) => {
        local(resp) = http_request(
            #myHost,
            -method=`PUT`
        )->response

        expect(#resp->body->beginsWith("PUT / HTTP/1.1\r\n"))
    }

    it(`Issues a PUT request with POST data`) => {
        local(resp) = http_request(
            #myHost,
            -postParams='data',
            -method=`PUT`
        )->response

        expect(#resp->body->beginsWith("PUT / HTTP/1.1\r\n"))
        expect(`data`, #resp->body->split("\r\n\r\n")->second)
    }

    it(`Issues a PUT request with array POST param data`) => {
        local(resp) = http_request(
            #myHost,
            -postParams=(:`run`=`for it`, `marty`=`ball`),
            -method=`PUT`
        )->response

        expect(#resp->body->beginsWith("PUT / HTTP/1.1\r\n"))
        expect(`run=for%20it&marty=ball`, #resp->body->split("\r\n\r\n")->second)
    }
}