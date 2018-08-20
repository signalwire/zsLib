/*

 Copyright (c) 2014, Robin Raymond
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.
 
 */

#include <zsLib/helpers.h>
#include <zsLib/IHelper.h>
#include <zsLib/Log.h>
#include <zsLib/internal/platform.h>

#ifndef ZSLIB_EVENTING_NOOP
#include <zsLib/internal/zsLib.events.h>
#include <zsLib/internal/zsLib.events.jman.h>
#else
#include <zsLib/eventing/noop.h>
#endif //ndef ZSLIB_EVENTING_NOOP

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif //HAVE_PTHREAD_H

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif //HAVE_WINDOWS_H

#include <zsLib/internal/zsLib_MessageQueueThreadUsingCurrentGUIMessageQueueForWinUWP.h>
#include <zsLib/internal/zsLib_MessageQueueThreadUsingCurrentGUIMessageQueueForCppWinrt.h>

#ifdef _WIN32

namespace zsLib {
  namespace compatibility {

#ifdef HAVE_RAISEEXCEPTION
    const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
      DWORD dwType; // Must be 0x1000.
      LPCSTR szName; // Pointer to name (in user addr space).
      DWORD dwThreadID; // Thread ID (-1=caller thread).
      DWORD dwFlags; // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    //-------------------------------------------------------------------------
    void SetThreadName(DWORD dwThreadID, const char* threadName)
    {
      THREADNAME_INFO info;
      info.dwType = 0x1000;
      info.szName = threadName;
      info.dwThreadID = dwThreadID;
      info.dwFlags = 0;

      __try
      {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
      }
    }
#endif //HAVE_RAISEEXCEPTION

    //-------------------------------------------------------------------------
    void uuid_generate_random(zsLib::internal::uuid_wrapper::raw_uuid_type &uuid) {
      ZS_MAYBE_USED() auto result = CoCreateGuid(&uuid);
      ZS_MAYBE_USED(result);
      ZS_ASSERT(S_OK == result);
    }
  }

  using namespace zsLib::compatibility;
}

#endif //_WIN32


//namespace zsLib { ZS_DECLARE_SUBSYSTEM(zslib) }

namespace zsLib
{

  namespace internal
  {
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //
    // Setup
    //

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //
    // (helpers)
    //

    // forward declarations
    void initSubsystems() noexcept;
#ifdef _WIN32
    void initWindowsEventProviderLogger() noexcept;
#endif //__WIN32
    void installTimerMonitorSettingsDefaults() noexcept;
    void installSocketMonitorSettingsDefaults() noexcept;
    void installMessageQueueManagerSettingsDefaults() noexcept;

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //
    // Setup
    //

    class Setup
    {
    protected:
      //-----------------------------------------------------------------------
      Setup() noexcept
      {
      }

    public:
      //-----------------------------------------------------------------------
      ~Setup() noexcept
      {
        ZS_EVENTING_UNREGISTER(zsLib);
      }

      //-----------------------------------------------------------------------
      static Setup &singleton() noexcept
      {
        static Setup setup;
        return setup;
      }

      //-----------------------------------------------------------------------
      void setup() noexcept
      {
        {
          AutoRecursiveLock lock(*IHelper::getGlobalLock());
        }
        initSubsystems();
        Log::initSingleton();
#ifdef _WIN32
        initWindowsEventProviderLogger();
#endif //__WIN32
        ZS_EVENTING_REGISTER(zsLib);
        installTimerMonitorSettingsDefaults();
        installSocketMonitorSettingsDefaults();
        installMessageQueueManagerSettingsDefaults();
      }

      //-----------------------------------------------------------------------
      PUID createPUID() noexcept
      {
        return ++mID;
      }

    protected:
      std::atomic_ulong mID {};
    };

    //---------------------------------------------------------------------------
    void setup() noexcept
    {
      internal::Setup::singleton().setup();
    }

#ifdef WINUWP
#ifdef __cplusplus_winrt
    void setup(Windows::UI::Core::CoreDispatcher ^dispatcher)
    {
      setup();
      internal::MessageQueueThreadUsingCurrentGUIMessageQueueForWinUWP::setupDispatcher(dispatcher);
      internal::MessageQueueThreadUsingCurrentGUIMessageQueueForWinUWP::hasDispatcher(true);
    }
#endif //__cplusplus_winrt
#ifdef CPPWINRT_VERSION
    void setup(winrt::Windows::UI::Core::CoreDispatcher dispatcher)
    {
      setup();
      internal::MessageQueueThreadUsingCurrentGUIMessageQueueForCppWinrt::setupDispatcher(dispatcher);
      internal::MessageQueueThreadUsingCurrentGUIMessageQueueForCppWinrt::hasDispatcher(true);
    }
#endif //CPPWINRT_VERSION
#endif //WINUWP

  }

  //---------------------------------------------------------------------------
  PUID createPUID() noexcept
  {
    return internal::Setup::singleton().createPUID();
  }

  //---------------------------------------------------------------------------
  UUID createUUID() noexcept
  {
    UUID gen;
    uuid_generate_random(gen.mUUID);
    return gen;
  }

  //---------------------------------------------------------------------------
  void debugSetCurrentThreadName(const char *name) noexcept
  {
    if (!name) name = "";

#ifdef HAVE_RAISEEXCEPTION
    SetThreadName(GetCurrentThreadId(), name);
#endif //HAVE_RAISEEXCEPTION

#ifdef HAVE_PTHREAD_SETNAME_WITH_2

    pthread_setname_np(pthread_self(), name);

#else //HAVE_PTHREAD_SETNAME_WITH_2

#ifdef HAVE_PTHREAD_SETNAME_WITH_1
    pthread_setname_np(name);
#endif //HAVE_PTHREAD_SETNAME_WITH_1

#endif //HAVE_PTHREAD_SETNAME_WITH_2
  }

  //---------------------------------------------------------------------------
  Time now() noexcept
  {
    return std::chrono::system_clock::now();
  }
}
