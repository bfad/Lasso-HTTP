local(here) = currentCapture->callSite_file->stripLastComponent + '/'
not #here->beginsWith('/') or #here->size == 1
    ? #here = io_file_getcwd + '/' + #here
if(not var_defined('_prep4tests')) => {
    sourcefile(file(#here + '../spec_helper.inc'), -autoCollect=false)->invoke
}
local(myHost) = `localhost:9000`
describe(`basic HTTP requests`) => {
    it(`successfully connects and requests the specified URL`) => {
        local(result) = http_response(#myHost + ``)

        expect(200 , #result->statusCode)
        expect(`OK`, #result->statusMsg)
        expect(#result->body >> "GET / HTTP/1.1\r\n")
        expect(#result->body >> "\r\nHost: " + #myHost + "\r\n")
    }

    it(`requests the correct URL path`) => {
        local(result) = http_response(#myHost + `/rhino`)

        expect(#result->body->beginsWith("GET /rhino HTTP/1.1\r\n"))
    }

    it(`requests the correct URL when GET parameters in URL string`) => {
        local(result) = http_response(#myHost + `/rhino?run=1`)

        expect(#result->body->beginsWith("GET /rhino?run=1 HTTP/1.1\r\n"))
    }

    it(`requests the correct URL when GET parameters are passed as params`) => {
        local(result) = http_response(
            #myHost + `/rhino`,
            -getParams = (:`run`=1, `id`=`silver`)
        )

        expect(#result->body->beginsWith("GET /rhino?run=1&id=silver HTTP/1.1\r\n"))
    }

    it(`requests the correct URL when GET parameters in both URL string and as params`) => {
        local(result) = http_response(
            #myHost + `/rhino?run=1`,
            -getParams = (:`id`=`silver`)
        )

        expect(#result->body->beginsWith("GET /rhino?run=1&id=silver HTTP/1.1\r\n"))
    }
}