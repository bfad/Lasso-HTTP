define http_request => type {
    data
        public curl,

        
        public urlProtocol   ::string = '',
        public urlHostname   ::string = '',
        public urlPath       ::string = '',
        public username      ::string = '',
        public password      ::string = '',
        public basicAuthOnly ::boolean = false,
        public getParams     ::staticarray = (:),
        public postParams,
        public headers       ::staticarray = (:),
        public sslNoVerify   ::boolean = false,
        public sslCert,
        public sslCertType,
        public sslKey,
        public sslKeyType,
        public sslKeyPasswd,
        public timeout,
        public connectTimeout,
        public method,
        public options


    public onCreate() => { .onCreate('') }
    public onCreate(
        url::string,
        -username::string='', 
        -password::string='',
        -basicAuthOnly::boolean=false,
        -getParams::trait_forEach=(:),
        -postParams=void,
        -headers::trait_forEach=(:),
        -sslNoVerify::boolean=false,
        -sslCert=void,
        -sslCertType=void,
        -sslKey=void,
        -sslKeyType=void,
        -sslKeyPasswd=void,
        -timeout=void,
        -connectTimeout=void,
        -method::string='',
        -options::trait_forEach=(:)
    ) => {
        .getParams      = #getParams->asStaticArray
        .postParams     = #postParams
        .username       = #username
        .password       = #password
        .basicAuthOnly  = #basicAuthOnly
        .headers        = #headers->asStaticArray
        .sslNoVerify    = #sslNoVerify
        .sslCert        = #sslCert
        .sslCertType    = #sslCertType
        .sslKey         = #sslKey
        .sslKeyType     = #sslKeyType
        .sslKeyPasswd   = #sslKeyPasswd
        .timeout        = #timeout
        .connectTimeout = #connectTimeout
        .method         = #method
        .options        = #options->asStaticArray
        // URL must go last (or at least after getParams)
        .url            = #url
    }

    public url => .urlProtocol + .urlHostname + .urlPath + .getParamsString
    public url=(value::string) => {
        // split up the different parts

        local(copy) = #value->asCopy

        if(#copy->beginsWith(`https://`)) => {
            .urlProtocol = `https://`
            #copy->remove(1,8)
        else
            .urlProtocol = `http://`
            #copy->beginsWith(`http://`)
                ? #copy->remove(1,7)
        }

        local(path_start)  = #copy->find(`/`)
        local(param_start) = #copy->find(`?`)

        if(#path_start == 0) => {
            .urlHostname = #copy
            .urlPath     = ``
            return #value
        else
            .urlHostname = #copy->sub(1, #path_start-1)
        }

        if(#param_start == 0) => {
            .urlPath = #copy->sub(#path_start)
            return #value
        else
            .urlPath = #copy->sub(#path_start, #param_start-#path_start)
        }

        // Prepend any GET parameters
        local(params) = array
        with item in #copy->sub(#param_start+1)->split(`&`)
        let param = #item->split(`=`)
        do #params->insert(#param->first=#param->second)

        .getParams = #params->asStaticArray + .getParams

        return #value
    }

    public getParamsString::string => {
        .getParams->size == 0
            ? return ``
        
        return '?' + (
            with elm in .getParams
            select #elm->first->asString->asBytes->encodeUrl + '=' + #elm->second->asString->asBytes->encodeUrl
        )->join(`&`)
    }

    public response => {
        .curl->isNotA(::curl)? .makeRequest

        return http_response(.curl->raw)
    }

    // Code adapted from include_url
    public makeRequest => {
        fail_if(.urlHostname == '', `No URL specified`)

        local(curl) = curl(.url)

        // Set cURL authentication options
        if(.username != '') => {
            #curl->set(CURLOPT_USERPWD, .username + ':' + .password)
            
            .basicAuthOnly
                ? #curl->set(CURLOPT_HTTPAUTH, CURLAUTH_BASIC)
                | #curl->set(CURLOPT_HTTPAUTH, CURLAUTH_ANY)
        }

        // Set cURL postParams
        if(.postParams->isA(::trait_forEach)) => {
            #curl->set(CURLOPT_POSTFIELDS,
                (
                    with param in .postParams
                    select #param->first->asString->asBytes->encodeUrl + '=' + #param->second->asString->asBytes->encodeUrl
                )->join('&')
            )
        else(.postParams->isA(::string) or .postParams->isA(::bytes))
            #curl->set(CURLOPT_POSTFIELDS, .postParams)
        }

        // Prepare headers
        #curl->set(CURLOPT_HTTPHEADER,
            (
                with item in .headers
                let header = (#item->isA(::pair) ? #item->first + `: ` + #item->second | #item->asString)
                select #header
            )->asStaticArray
        )

        // SSL Options
        #curl->set(CURLOPT_SSL_VERIFYPEER, not .sslNoVerify)
        .sslCert?
            #curl->set(CURLOPT_SSLCERT, string(.sslCert))
        .sslCertType?
            #curl->set(CURLOPT_SSLCERTTYPE, string(.sslCertType))
        .sslKey?
            #curl->set(CURLOPT_SSLKEY, string(.sslKey))
        .sslKeyType?
            #curl->set(CURLOPT_SSLKEYTYPE, string(.sslKeyType))
        .sslKeyPasswd?
            #curl->set(CURLOPT_SSLKEYPASSWD, string(.sslKeyPasswd))

        // Timeout Options
        .timeout?
            #curl->set(CURLOPT_TIMEOUT, integer(.timeout))
        .connectTimeout?
            #curl->set(CURLOPT_CONNECTTIMEOUT, integer(.connectTimeout))

        // HTTP Request Method
        .method->size > 0
            ? #curl->set(CURLOPT_CUSTOMREQUEST, .method)

        // These options will override anything already set
        with option in .options
        where #option->isA(::pair)
        do #curl->set(#option->first, #option->second)

        .`curl` = #curl
    }
}
