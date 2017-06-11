/*
MIT License

Copyright (c) 2017 Benjamin "Nefarius" Höglinger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


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

static auto const apiAddUrl = "http://localhost:26762/api/v1/hidguardian/whitelist/add/";
static auto const apiRemoveUrl = "http://localhost:26762/api/v1/hidguardian/whitelist/remove/";


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
    auto& logger = Logger::get("HidCerberus.Lib:HidGuardianOpen");

    try
    {
        std::string addUri(apiAddUrl);
        addUri += std::to_string(GetCurrentProcessId());
        logger.information("Sending add request: %s", addUri);

        return SendRequest(addUri);
    }
    catch (std::exception const & ex)
    {
        logger.error("Couldn't send request: %s", std::string(ex.what()));
        return FALSE;
    }
}

HIDCERBERUSLIB_API BOOL HidGuardianClose()
{
    auto& logger = Logger::get("HidCerberus.Lib:HidGuardianClose");

    try
    {
        std::string removeUri(apiRemoveUrl);
        removeUri += std::to_string(GetCurrentProcessId());
        logger.information("Sending remove request: %s", removeUri);

        return SendRequest(removeUri);
    }
    catch (std::exception const & ex)
    {
        logger.error("Couldn't send request: %s", std::string(ex.what()));
        return FALSE;
    }
}
