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
    std::string addUri("http://localhost:26762/v1/hidguardian/whitelist/add/");
    addUri += std::to_string(GetCurrentProcessId());

    return SendRequest(addUri);
}

HIDCERBERUSLIB_API BOOL HidGuardianClose()
{
    std::string addUri("http://localhost:26762/v1/hidguardian/whitelist/remove/");
    addUri += std::to_string(GetCurrentProcessId());

    return SendRequest(addUri);
}
