/*

 Copyright (c) 2016, Robin Raymond
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

#pragma once

#ifdef _WIN32
#include <Evntprov.h>
#endif //_WIN32

namespace zsLib
{
  namespace eventing
  {
    namespace internal
    {
#ifdef _WIN32
      typedef EVENT_DATA_DESCRIPTOR USE_EVENT_DATA_DESCRIPTOR;
      typedef EVENT_DESCRIPTOR USE_EVENT_DESCRIPTOR;
#else
      struct EventDataDescriptor
      {
        uintptr_t Ptr;
        size_t Size;
        size_t Type;
      };
      
      struct EventDescriptor
      {
        uint16_t Id;
        uint8_t Version;
        uint8_t Channel;
        uint8_t Level;
        uint8_t Opcode;
        uint16_t Task;
        uint64_t Keyword;
      };
      
      typedef EventDataDescriptor USE_EVENT_DATA_DESCRIPTOR;
      typedef EventDescriptor USE_EVENT_DESCRIPTOR;
#endif //_WIN32
      
      struct EventParameterDescriptor
      {
        EventParameterTypes Type;
      };
      
      typedef EventParameterDescriptor USE_EVENT_PARAMETER_DESCRIPTOR;
    }
  }
}

#ifndef ZSLIB_EVENTING_NOOP

#define ZS_EVENTING_INTERNAL_GET_LOG_LEVEL()                                                                ((ZS_GET_SUBSYSTEM()).getEventingLevel())
#define ZS_EVENTING_INTERNAL_GET_SUBSYSTEM_LOG_LEVEL(xSubsystem)                                            ((xSubsystem).getEventingLevel())
#define ZS_EVENTING_INTERNAL_IS_LOGGING(xHandleReference, xKeywordBitmask, xLevel)                          ((::zsLib::Log::isEventingLogging((xHandleReference), (xKeywordBitmask))) && (((ZS_GET_SUBSYSTEM()).getEventingLevel()) >= ::zsLib::Log::xLevel))
#define ZS_EVENTING_INTERNAL_IS_LOGGING_VALUE(xHandleReference, xKeywordBitmask, xLevelValue)               ((::zsLib::Log::isEventingLogging((xHandleReference), (xKeywordBitmask))) && (((ZS_GET_SUBSYSTEM()).getEventingLevel()) >= (xLevelValue)))
#define ZS_EVENTING_INTERNAL_IS_SUBSYSTEM_LOGGING(xHandleReference, xKeywordBitmask, xSubsystem, xLevel)    ((::zsLib::Log::isEventingLogging((xHandleReference), (xKeywordBitmask))) && (((xSubsystem).getEventingLevel()) >= ::zsLib::Log::xLevel))

#define ZS_EVENTING_INTERNAL_REGISTER_EVENT_WRITER(xHandleReference, xProviderID, xProviderName, xUniqueProviderHash) 
#define ZS_EVENTING_INTERNAL_UNREGISTER_EVENT_WRITER(xHandleReference)

#define ZS_EVENTING_INTERNAL_REGISTER_SUBSYSTEM_DEFAULT_LEVEL(xSubsystemName, xLevel)                       {zsLib::Log::setEventingLevelByName(#xSubsystemName, zsLib::Log::Level);}


#define ZS_EVENTING_INTERNAL_WRITE_EVENT(xHandle, xSeverity, xLevel, xEventDescriptor, xEventParameterDescriptor, xEventDataDescriptor, xEventDataDescriptorCount)  \
  {                                                                                                                                                                 \
    ::zsLib::Log::writeEvent(                                                                                                                                       \
                             (xHandle),                                                                                                                             \
                             ::zsLib::Log::xSeverity,                                                                                                               \
                             ::zsLib::Log::xLevel,                                                                                                                  \
                             (::zsLib::Log::LOG_EVENT_DESCRIPTOR_HANDLE)(xEventDescriptor),                                                                         \
                             (::zsLib::Log::LOG_EVENT_PARAMETER_DESCRIPTOR_HANDLE)(xEventParameterDescriptor),                                                      \
                             (::zsLib::Log::LOG_EVENT_DATA_DESCRIPTOR_HANDLE)(xEventDataDescriptor),                                                                \
                             (xEventDataDescriptorCount)                                                                                                            \
                             );                                                                                                                                     \
  }

#ifdef _WIN32
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL(xInDescriptor, xPtrValue, xValueSize) \
  { EventDataDescCreate((xInDescriptor), (xPtrValue), (xValueSize)); }
#else
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL(xInDescriptor, xPtrValue, xValueSize) \
  { (xInDescriptor)->Ptr = (xPtrValue); (xInDescriptor)->Size = (xValueSize); }
#endif //_WIN32

#define ZS_EVENTING_INTERNAL_EVENT_DATA_ASTR_VALUE(xValue)                      (xValue)
#define ZS_EVENTING_INTERNAL_EVENT_DATA_WSTR_VALUE(xValue)                      (xValue)

#define ZS_EVENTING_INTERNAL_EVENT_DATA_ASTR_LEN(xValue)                        (((const char *)(xValue)) ? ((strlen(xValue)+1)*sizeof(char)) : 0)
#define ZS_EVENTING_INTERNAL_EVENT_DATA_WSTR_LEN(xValue)                        (((const wchar_t *)(xValue)) ? ((wcslen(xValue)+1)*sizeof(wchar_t)) : 0)

#ifdef _WIN32
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL_VALUE(xInDescriptor, xPtrValue, xValueSize) { EventDataDescCreate((xInDescriptor), (xPtrValue), (xValueSize)); }
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL_ASTR(xInDescriptor, xStr)                   { EventDataDescCreate((xInDescriptor), ZS_EVENTING_INTERNAL_EVENT_DATA_ASTR_VALUE(xStr), ZS_EVENTING_INTERNAL_EVENT_DATA_ASTR_LEN(xStr)); }
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL_WSTR(xInDescriptor, xStr)                   { EventDataDescCreate((xInDescriptor), ZS_EVENTING_INTERNAL_EVENT_DATA_WSTR_VALUE(xStr), ZS_EVENTING_INTERNAL_EVENT_DATA_WSTR_LEN(xStr)); }
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL_BUFFER(xInDescriptor, xPtr, xSize)          { EventDataDescCreate((xInDescriptor), (xPtr), (xSize)); }
#else
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL_VALUE(xInDescriptor, xPtrValue, xValueSize) { (xInDescriptor)->Ptr = (xPtrValue); (xInDescriptor)->Size = (xValueSize); }
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL_ASTR(xInDescriptor, xStr)                   { (xInDescriptor)->Ptr = ZS_EVENTING_INTERNAL_EVENT_DATA_ASTR_VALUE(xStr); (xInDescriptor)->Size = ZS_EVENTING_INTERNAL_EVENT_DATA_ASTR_LEN(xStr); }
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL_WSTR(xInDescriptor, xStr)                   { (xInDescriptor)->Ptr = ZS_EVENTING_INTERNAL_EVENT_DATA_WSTR_VALUE(xStr); (xInDescriptor)->Size = ZS_EVENTING_INTERNAL_EVENT_DATA_WSTR_LEN(xStr); }
#define ZS_EVENTING_INTERNAL_EVENT_DATA_DESCRIPTOR_FILL_BUFFER(xInDescriptor, xPtr, xSize)          { (xInDescriptor)->Ptr = (xPtr); (xInDescriptor)->Size = (xSize); }
#endif //_WIN32

#endif //ndef ZSLIB_EVENTING_NOOP