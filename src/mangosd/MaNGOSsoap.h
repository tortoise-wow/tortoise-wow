/*
 * SOAP remote-command interface (re-added).
 *
 * Exposes a single ns1__executeCommand method (urn:MaNGOS namespace) so an
 * external manager can run one server command and get that command's output
 * back as the response - a clean request/response channel, unlike the shared
 * stdin console stream.
 *
 * Off unless SOAP.Enabled = 1. Binds 127.0.0.1:7878 by default and requires a
 * SEC_ADMINISTRATOR account that is not banned; both are checked before the
 * request body is read (401 / 403). A declared Content-Length over 64 KiB is
 * refused with 413; a chunked or undeclared body over that is cut off and the
 * connection closed. Commands are queued to the world thread via
 * sWorld.QueueCliCommand and their captured output is returned to the caller.
 */

#ifndef MANGOSSERVER_MANGOSSOAP_H
#define MANGOSSERVER_MANGOSSOAP_H

#include "Common.h"
#include "World.h"
#include "AccountMgr.h"
#include "Log.h"

#include "soap/soapH.h"
#include "soap/soapStub.h"

#include <string>
#include <thread>

class SOAPThread
{
    private:
        static const int AcceptTimeout = 3;                 // seconds; lets the loop poll World::IsStopped()
        static const int DataTimeout   = 5;                 // seconds
        static const int BackLogSize   = 100;
        static const int MaxRequestBytes = 64 * 1024;       // whole HTTP message; no command comes near it

        const std::string m_host;
        const int m_port;

        std::thread m_workerThread;

        void Work();
        static int ParseAndAuthenticate(struct soap* soap);

    public:
        static const AccountTypes MinLevel = SEC_ADMINISTRATOR;

        SOAPThread(const std::string& host, int port);
        ~SOAPThread();
};

#endif
