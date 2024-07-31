#include "FaultClient.h"

#include "Log.h"

using namespace std;

pplx::task<web::http::http_response> FaultClient::post(web::json::value &postData, const wstring &path)
{
    web::http::http_request request(web::http::methods::POST);
    request.set_request_uri(path);
    request.headers().set_content_type(U("application/json"));

    request.set_body(postData);

    ucout << postData << endl;
    return myClient.request(request);
}

void FaultClient::push(web::json::value &postData, const wstring &uri)
{
    FaultClient client(U("http://127.0.0.1:8888"));
    auto postResponse = client.post(postData, uri).get();
    if (postResponse.status_code() != 200) {
        wstring log = postResponse.extract_string().get();
        LOG_ERR("Post response content: {}", string(log.begin(), log.end()));
    }
}