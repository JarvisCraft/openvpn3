//    OpenVPN -- An application to securely tunnel IP networks
//               over a single port, with support for SSL/TLS-based
//               session authentication and key exchange,
//               packet encryption, packet authentication, and
//               packet compression.
//
//    Copyright (C) 2012-2022 OpenVPN Inc.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License Version 3
//    as published by the Free Software Foundation.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program in the COPYING file.
//    If not, see <http://www.gnu.org/licenses/>.

// NOTE: define USE_ASIO_THREADLOCAL if your C++ doesn't support the
// "thread_local" attribute.

#ifndef OPENVPN_LOG_LOGTHREAD_CLASS_H
#define OPENVPN_LOG_LOGTHREAD_CLASS_H

#include <string>
#include <sstream>
#include <thread>

#if defined(USE_ASIO) && defined(USE_ASIO_THREADLOCAL)
#include <asio/detail/tss_ptr.hpp>
#endif

#include <openvpn/common/size.hpp>

// Define this parameter before including this header:
// OPENVPN_LOG_CLASS -- client class that exposes a log() method

#ifndef OPENVPN_LOG_CLASS
#error OPENVPN_LOG_CLASS must be defined
#endif

namespace openvpn {
namespace Log {

#ifdef OPENVPN_LOG_GLOBAL
// OPENVPN_LOG uses global object pointer
inline OPENVPN_LOG_CLASS *global_log = nullptr; // GLOBAL
struct Context
{
    struct Wrapper
    {
    };

    Context(const Wrapper &wrap)
    {
    }

    Context(OPENVPN_LOG_CLASS *cli)
    {
        global_log = cli;
    }

    ~Context()
    {
        global_log = nullptr;
    }

    static bool defined()
    {
        return global_log != nullptr;
    }

    static OPENVPN_LOG_CLASS *obj()
    {
        return global_log;
    }
};
#else
// OPENVPN_LOG uses thread-local object pointer
#if defined(USE_ASIO) && defined(USE_ASIO_THREADLOCAL)
inline asio::detail::tss_ptr<OPENVPN_LOG_CLASS> global_log; // GLOBAL
#else
inline thread_local OPENVPN_LOG_CLASS *global_log = nullptr; // GLOBAL
#endif
struct Context
{
    // Mechanism for passing thread-local
    // global_log to another thread.
    class Wrapper
    {
      public:
        Wrapper()
            : log(obj())
        {
        }

      private:
        friend struct Context;
        OPENVPN_LOG_CLASS *log;
    };

    // While in scope, turns on global_log
    // for this thread.
    Context(const Wrapper &wrap)
    {
        global_log = wrap.log;
    }

    Context(OPENVPN_LOG_CLASS *cli)
    {
        global_log = cli;
    }

    ~Context()
    {
        global_log = nullptr;
    }

    static bool defined()
    {
        return global_log != nullptr;
    }

    static OPENVPN_LOG_CLASS *obj()
    {
        return global_log;
    }
};
#endif
} // namespace Log
} // namespace openvpn

#endif