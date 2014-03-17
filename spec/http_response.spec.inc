local(here) = currentCapture->callSite_file->stripLastComponent + '/'
not #here->beginsWith('/') or #here->size == 1
    ? #here = io_file_getcwd + '/' + #here
if(not var_defined('_prep4tests')) => {
    sourcefile(file(#here + './spec_helper.inc'), -autoCollect=false)->invoke
}


describe(::http_response) => {
    describe('Parsing the HTTP Response Headers') => {
        it("properly parses the HTTP message") => {
            local(resp) = http_response(bytes('HTTP/1.1 200 OK\r\n\r\n'))
            
            expect('HTTP/1.1', #resp->protocol)
            expect(200       , #resp->statusCode)
            expect('OK'      , #resp->statusMsg)
        }
        
        it("properly parses basic headers") => {
            local(resp) = http_response(bytes('\
                HTTP/1.1 200 OK\r\n\
                Content-Type: text/html\r\n\
                Pragma: no-cache\r\n\r\n\
            '))

            expect( 2         , #resp->headers->size)
            expect(`text/html`, #resp->headers->find(`Content-Type`))
            expect(`no-cache` , #resp->headers->find(`Pragma`))
        }
        it("properly parses headers that wrap with a space") => {
            local(resp) = http_response(bytes('\
                HTTP/1.1 200 OK\r\n\
                Server: localhost:8000\r\n\
                Content-Type: text/html;\r\n utf-8\r\n\r\n\
            '))
            
            expect( 2                , #resp->headers->size)
            expect(`text/html; utf-8`, #resp->headers->find(`Content-Type`))
            expect(`localhost:8000`  , #resp->headers->find(`Server`))
        }
        it("properly parses headers that wrap with a tab") => {
            local(resp) = http_response(bytes('\
                HTTP/1.1 200 OK\r\n\
                Server: localhost:8000\r\n\
                Content-Type: text/html;\r\n\tutf-8\r\n\r\n\
            '))
            
            expect( 2                 , #resp->headers->size)
            expect('text/html;\tutf-8', #resp->headers->find(`Content-Type`))
            expect(`localhost:8000`   , #resp->headers->find(`Server`))
        }
        it("properly parses repeated headers as csv values") => {
            local(resp) = http_response(bytes('\
                HTTP/1.1 401 Unauthorized\r\n\
                Server: localhost:8000\r\n\
                WWW-Authenticate: Basic\r\n\
                WWW-Authenticate: Digest\r\n\r\n\
            '))
            
            expect( 2              , #resp->headers->size)
            expect(`Basic,Digest`  , #resp->headers->find(`WWW-Authenticate`))
            expect(`localhost:8000`, #resp->headers->find(`Server`))
        }
        it("sticks the message body into body") => {
            local(resp) = http_response(bytes('\
                HTTP/1.1 200 OK\r\n\
                Content-Type: text/html\r\n\
                Pragma: no-cache\r\n\r\n\
                Message Body\
            '))
            
            expect('Message Body', #resp->body)
        }
    }


    describe(`Parsing data from format returned by curl->raw`) => {
        it(`Properly populates header and body data`) => {
             local(data) = (:
                true,
                bytes("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"),
                bytes("<!DOCTYPE html><html><head><title></title></head><body><h1>Hello, World!</h1></body></html>")
            )
            local(resp) = http_response(#data)

            expect(#data->get(2), #resp->headerBytes)
            expect(#data->get(3), #resp->body)

            expect('HTTP/1.1'                , #resp->protocol)
            expect(200                       , #resp->statusCode)
            expect('OK'                      , #resp->statusMsg)
            expect( 1                        , #resp->headers->size)
            expect(`text/html; charset=UTF-8`, #resp->headers->find(`Content-Type`))
        }

        it(`Removes any intermediate headers`) => {
            local(data) = (:
                true,
                bytes(
                    "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"Site\"\r\n\r\n" +
                    "HTTP/1.1 100 Continue\r\n\r\n" +
                    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                ),
                bytes("<!DOCTYPE html><html><head><title></title></head><body><h1>MooseHair</h1></body></html>")
            )
            local(resp) = http_response(#data)
            
            expect("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n", #resp->headerBytes)
            expect(#data->get(3), #resp->body)
            
            expect('HTTP/1.1' , #resp->protocol)
            expect(200        , #resp->statusCode)
            expect('OK'       , #resp->statusMsg)
            expect( 1         , #resp->headers->size)
            expect(`text/html`, #resp->headers->find(`Content-Type`))
        }
    }
}