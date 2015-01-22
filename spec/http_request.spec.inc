local(here) = currentCapture->callSite_file->stripLastComponent + '/'
not #here->beginsWith('/') or #here->size == 1
    ? #here = io_file_getcwd + '/' + #here
if(not var_defined('_prep4tests')) => {
    sourcefile(file(#here + './spec_helper.inc'), -autoCollect=false)->invoke
}


describe(::http_request) => {
    it(`Takes in a URL string and parses out all the parts correctly`) => {
        local(req) = http_request(`http://example.com/foo/bar`)

        expect(`http://example.com/foo/bar`, #req->url)
        expect(`http://`       , #req->urlProtocol)
        expect(`example.com`   , #req->urlHostname)
        expect(`/foo/bar`      , #req->urlPath)
    }

    it(`Properly parses https:// URLs`) => {
        local(req) = http_request(`https://example.com/foo/bar`)

        expect(`https://example.com/foo/bar`, #req->url)
        expect(`https://`       , #req->urlProtocol)
        expect(`example.com`   , #req->urlHostname)
        expect(`/foo/bar`      , #req->urlPath)
    }

    it(`Allows for not specifying http:// for non-secure requests`) => {
        local(req) = http_request(`example.com/foo/bar`)

        expect(`http://example.com/foo/bar`, #req->url)
        expect(`http://`       , #req->urlProtocol)
        expect(`example.com`   , #req->urlHostname)
        expect(`/foo/bar`      , #req->urlPath)
    }

    it(`Takes in a URL string and parses out all the parts correctly including query params`) => {
        local(req) = http_request(`http://example.com/foo/bar?baz=2`)

        expect(`http://example.com/foo/bar?baz=2`, #req->url)
        expect(`http://`       , #req->urlProtocol)
        expect(`example.com`   , #req->urlHostname)
        expect(`/foo/bar`      , #req->urlPath)
        expect(pair(`baz`, `2`), #req->queryParams->first)
    }

    it(`Allows for creating an empty request object`) => {
        expect->errorCode(error_code_noError) => {
            local(req) = http_request
        }
    }

    it(`Throws an error if it tries to make a request with no URL`) => {
        expect->errorMsg(`No URL specified`) => {
            http_request->response
        }
    }

    it(`Properly handles GET parameters in both the URL and passed as an argument`) => {
        local(req) = http_request(
            `https://example.com/run?rhinos=1&bulls=0`,
            -queryParams = (:`hippos` = 2)
        )

        expect(
            (:`rhinos`=`1`,`bulls`=`0`, `hippos`=2),
            #req->queryParams
        )
    }

    context(`extending queryParams`) => {
        it(`Allows for adding new query parameters`) => {
            local(req) = http_request(
                `https://example.com/run?bulls=0`,
                -queryParams = (:`rhinos`=1)
            )
            #req->queryParams->insert(`hippos` = 2)

            expect(
                array(`bulls`='0', `rhinos`=1, `hippos`=2),
                #req->queryParams
            )
        }
    }

    context(`extending headers`) => {
        it(`Allows for adding new query parameters`) => {
            local(req) = http_request(
                `https://example.com/run`,
                -headers = (:`x-test`='rhino')
            )
            #req->headers->insert(`hippos` = `bulls`)
            
            expect(
                array(`x-test`=`rhino`, `hippos`='bulls'),
                #req->headers
            )
        }
    }
}