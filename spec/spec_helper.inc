if(not var_defined('_prep4tests')) => {
    local(here) = currentCapture->callSite_file->stripLastComponent + '/'
    not #here->beginsWith('/') or #here->size == 1
        ? #here = io_file_getcwd + '/' + #here

    sourcefile(file(#here + '../http_response.inc'), -autoCollect=false)->invoke
    var(_prep4tests) = true
}