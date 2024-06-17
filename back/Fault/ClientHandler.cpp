#include "ClientHandler.h"

ClientHandler::ClientHandler(const wstring& url) : myClient(url)
{

}

pplx::task<web::http::http_response> ClientHandler::post(web::json::value postData, const std::wstring path)
{
    web::http::http_request request(web::http::methods::POST);
    request.set_request_uri(path);
    request.headers().set_content_type(U("application/json"));

    request.set_body(postData);
    ucout << postData << endl;
    return myClient.request(request);
}

http_response AlgoClient::MyClient(web::json::value postData, wstring uri)
{
    // Ç°¶Ë·þÎñ¶Ëip
    ClientHandler client(U("http://127.0.0.1:8888"));
    auto postResponse = client.post(postData, uri).get();
    return postResponse;
}
