define http_response => type {
    data
        options::map = map,
        private curl,
        private headerBytes,

        public protocol::string,
        public statusCode::integer,
        public statusMsg::string,
        public headers::map,
        public body::bytes

    // Meant for raw HTTP responses - in this case, now request info
    public onCreate(response::bytes) => {
        local(break)   = #response->find('\r\n\r\n')

        .'headerBytes' = #response->sub(1, #break + 3)
        .'body'        = #response->sub(#break + 4)

        .populateHeaderInfo
    }

    // Code adapted from include_url
    public onCreate(
        url::string,
        -username::string='', 
        -password::string='',
        -basicAuthOnly::boolean=false,
        -getParams::trait_forEach=(:),
        -postParams=void,
        -reqheaders::trait_forEach=(:),
        -noVerify::boolean=false,
        -sslCert=void,
        -sslCertType=void,
        -sslKey=void,
        -sslKeyType=void,
        -sslKeyPasswd=void,
        -timeout=void,
        -connectTimeout=void,
        -reqMethod::string='',
        -options=(:)
    ) => {
        .'options'->insert(`url`            = #url           )
        .'options'->insert(`getParams`      = #getParams     )
        .'options'->insert(`postParams`     = #postParams    )
        .'options'->insert(`username`       = #username      )
        .'options'->insert(`password`       = #password      )
        .'options'->insert(`basicAuthOnly`  = #basicAuthOnly )
        .'options'->insert(`reqheaders`     = #reqheaders    )
        .'options'->insert(`noVerify`       = #noVerify      )
        .'options'->insert(`sslCert`        = #sslCert       )
        .'options'->insert(`sslCertType`    = #sslCertType   )
        .'options'->insert(`sslKey`         = #sslKey        )
        .'options'->insert(`sslKeyType`     = #sslKeyType    )
        .'options'->insert(`sslKeyPasswd`   = #sslKeyPasswd  )
        .'options'->insert(`timeout`        = #timeout       )
        .'options'->insert(`connectTimeout` = #connectTimeout)
        .'options'->insert(`options`        = #options       )

        // Prepare URL with Get Parameters
        #url = #url->asCopy
        local(delimiter) = (#url >> `?` ? `&` | `?`)
        with param in #getParams do {
            #url->append(#delimiter + #param->first->asString->asBytes->encodeUrl + '=' + #param->second->asString->asBytes->encodeUrl)

            #delimiter == `?`
                ? #delimiter = `&`
        }

        local(curl) = curl(#url)

        // Set cURL authentication options
        if(#username != '') => {
            #curl->set(CURLOPT_USERPWD, #username + ':' + #password)
            
            #basicAuthOnly
                ? #curl->set(CURLOPT_HTTPAUTH, CURLAUTH_BASIC)
                | #curl->set(CURLOPT_HTTPAUTH, CURLAUTH_ANY)
        }

        // Set cURL postParams
        if(#postParams->isA(::trait_forEach)) => {
            #curl->set(CURLOPT_POSTFIELDS,
                (
                    with param in #postParams
                    select #param->first->asString->asBytes->encodeUrl + '=' + #param->second->asString->asBytes->encodeUrl
                )->join('&')
            )
        else(#postParams->isA(::string) or #postParams->isA(::bytes))
            #curl->set(CURLOPT_POSTFIELDS, #postParams)
        }

        // Prepare headers
        #curl->set(CURLOPT_HTTPHEADER,
            (
                with item in #reqheaders
                let header = (#item->isA(::pair) ? #item->first + `: ` + #item->second | #item->asString)
                select #header
            )->join("\r\n")
        )

        // SSL Options
        #curl->set(CURLOPT_SSL_VERIFYPEER, not #noVerify)
        #sslCert?
            #curl->set(CURLOPT_SSLCERT, string(#sslCert))
        #sslCertType?
            #curl->set(CURLOPT_SSLCERTTYPE, string(#sslCertType))
        #sslKey?
            #curl->set(CURLOPT_SSLKEY, string(#sslKey))
        #sslKeyType?
            #curl->set(CURLOPT_SSLKEYTYPE, string(#sslKeyType))
        #sslKeyPasswd?
            #curl->set(CURLOPT_SSLKEYPASSWD, string(#sslKeyPasswd))

        // Timeout Options
        #timeout?
            #curl->set(CURLOPT_TIMEOUT, integer(#timeout))
        #connectTimeout?
            #curl->set(CURLOPT_CONNECTTIMEOUT, integer(#connectTimeout))

        // HTTP Request Method
        #reqMethod->size > 0
            ? #curl->set(CURLOPT_CUSTOMREQUEST, #reqMethod)

        // These options will override anything already set
        with option in #options
        where #option->isA(::pair)
        do #curl->set(#option->first, #option->second)

        .'body'        = #curl->result
        .'headerBytes' = #curl->header
        .'curl'        = #curl
        
        .populateHeaderInfo
    }

    // But of getters
    public
        url            => .'options'->find(`url`),
        username       => .'options'->find(`username`),
        password       => .'options'->find(`password`),
        basicAuthOnly  => .'options'->find(`basicAuthOnly`),
        getParams      => .'options'->find(`getParams`),
        postParams     => .'options'->find(`postParams`),
        reqheaders     => .'options'->find(`reqheaders`),
        noVerify       => .'options'->find(`noVerify`),
        sslCert        => .'options'->find(`sslCert`),
        sslCertType    => .'options'->find(`sslCertType`),
        sslKey         => .'options'->find(`sslKey`),
        sslKeyType     => .'options'->find(`sslKeyType`),
        sslKeyPasswd   => .'options'->find(`sslKeyPasswd`),
        timeout        => .'options'->find(`timeout`),
        connectTimeout => .'options'->find(`connectTimeout`),
        options        => .'options'->find(`options`)


    private populateHeaderInfo => {
        local(tmp_headers) = .headerAsString->asCopy
        local(end_status)  = #tmp_headers->find('\r\n')
        local(message)     = #tmp_headers->sub(1, #end_status - 1)->split(' ')

        #tmp_headers
            ->remove(1, #end_status + 1)
            & replace('\r\n ' , ' ')
            & replace('\r\n\t', '\t')
            & removeTrailing('\r\n')

        .'headers' = map
        with header in #tmp_headers->split('\r\n')
        let key = #header->sub(1, #header->find(':') - 1)
        let val = #header->sub(#header->find(':') + 1)
        let cur = .'headers'->find(#key)
        do {

            #val->trim
            // Taking advantage of #cur being a references in the else clause
            #cur == void
                ? .'headers'->insert(#key=#val)
                | #cur->append(',' + #val)
        }

        .'protocol'   =  string(#message->get(1))
        .'statusCode' = integer(#message->get(2))
        .'statusMsg'  =  string(#message->get(3))
    }


    public headerAsString => .'headerBytes'->exportAs(`ISO-8859-1`)
    public bodyAsString(charset::string) => .'body'->exportAs(#charset)
    public bodyAsString => {
    // Code adapted from include_url
        // auto-discover charset
        local(charset) = string_findregexp(.headerAsString, -find='(?i)charset\\s*=\\s*([\\w\\-]+)')
        if(#charset->size >= 2) => {
            #charset = #charset->get(2)
        else
        // charset not found in headers, try meta on page
            #charset = string_findregexp(.'body', -find='(?i)charset\\s*=\\s*([\\w\\-]+)')
            #charset->size >= 2
                ? #charset = #charset->get(2)
                | #charset = 'utf-8'
        }
        #charset == 'ISO-8859-1'
            ? #charset = 'Windows-1252'

        return .bodyAsString(#charset)
    }

}