if(not var_defined('_prep4tests')) => {
    local(here) = currentCapture->callSite_file->stripLastComponent + '/'
    not #here->beginsWith('/') or #here->size == 1
        ? #here = io_file_getcwd + '/' + #here

    sourcefile(file(#here + '../http_response.inc'), -autoCollect=false)->invoke
    sourcefile(file(#here + '../http_request.inc' ), -autoCollect=false)->invoke
    var(_prep4tests) = true


    // Start the HTTP Echo Server
    local(_) = split_thread => {
        var(listener) = net_tcp
        handle => { $listener->close }
        $listener->bind(9000)&listen

        $listener->forEachAccept => {
            local(con) = #1
            local(req) = ''
            handle => { #con->close }
            
            // Read in the request in chunks
            {
                #req->append(#con->readSomeBytes(8096))
                not #req->contains('\r\n\r\n')? currentCapture->restart
            }()

            // Echo the request
            #con->writeBytes(bytes('HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n' + #req))
        }
    }
}