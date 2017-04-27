// HidCerberus.Lib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HidCerberus.Lib.h"
#include <string>
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPResponse.h>

using Poco::URI;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPResponse;

BOOL SendRequest(std::string addUri)
{
    URI uri(addUri);
    auto path(uri.getPathAndQuery());

    HTTPClientSession session(uri.getHost(), uri.getPort());
    HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
    HTTPResponse response;

    session.sendRequest(request);
    session.receiveResponse(response);

    return (response.getStatus() == HTTPResponse::HTTP_OK) ? TRUE : FALSE;
}

HIDCERBERUSLIB_API BOOL HidGuardianOpen()
{
    std::string addUri("http://localhost:26762/v1/hid/whitelist/add/");
    addUri += std::to_string(GetCurrentProcessId());

    return SendRequest(addUri);
}

HIDCERBERUSLIB_API BOOL HidGuardianClose()
{
    std::string addUri("http://localhost:26762/v1/hid/whitelist/remove/");
    addUri += std::to_string(GetCurrentProcessId());

    return SendRequest(addUri);
}
