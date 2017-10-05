//
// Copyright 2017 OSR Open Systems Resources, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE
// 
#include "kmexts.h"

CM_CALLBACK_CONTEXT KmExtsCmCallbackContext;

_Ret_z_
PWCHAR
RegNotifyClassToString(
    _In_ REG_NOTIFY_CLASS RegNotifyClass
    );

_Use_decl_annotations_
NTSTATUS 
KmExtsCmRegistryCallback(
    PVOID CallbackContext,
    PVOID Argument1,
    PVOID Argument2) 
{

    REG_NOTIFY_CLASS regNotifyClass;

    UNREFERENCED_PARAMETER(CallbackContext);
    UNREFERENCED_PARAMETER(Argument2);

    // 
    // Argument1 is a REG_NOTIFY_CLASS, which will indicate the
    // operation being performed. For each REG_NOTIFY_CLASS there is
    // an associated data structure passed as Argument2.
    // 
    // See the documentation CmRegisterCallback(Ex) documentation
    // for more info.
    // 
    regNotifyClass = (REG_NOTIFY_CLASS)PtrToUlong(Argument1);
     
    DbgPrint("KmExtsCmRegistryCallback: Called for %ls (0x%x)\n",
             RegNotifyClassToString(regNotifyClass), regNotifyClass);

    return STATUS_SUCCESS;
}


_Use_decl_annotations_
PWCHAR
RegNotifyClassToString(
    REG_NOTIFY_CLASS RegNotifyClass) 
{
    switch (RegNotifyClass) {
        case RegNtPreDeleteKey:
            return L"RegNtPreDeleteKey";
        case RegNtPreSetValueKey:
            return L"RegNtPreSetValueKey";
        case RegNtPreDeleteValueKey:
            return L"RegNtPreDeleteValueKey";
        case RegNtPreSetInformationKey:
            return L"RegNtPreSetInformationKey";
        case RegNtPreRenameKey:
            return L"RegNtPreRenameKey";
        case RegNtPreEnumerateKey:
            return L"RegNtPreEnumerateKey";
        case RegNtPreEnumerateValueKey:
            return L"RegNtPreEnumerateValueKey";
        case RegNtPreQueryKey:
            return L"RegNtPreQueryKey";
        case RegNtPreQueryValueKey:
            return L"RegNtPreQueryValueKey";
        case RegNtPreQueryMultipleValueKey:
            return L"RegNtPreQueryMultipleValueKey";
        case RegNtPreCreateKey:
            return L"RegNtPreCreateKey";
        case RegNtPostCreateKey:
            return L"RegNtPostCreateKey";
        case RegNtPreOpenKey:
            return L"RegNtPreOpenKey";
        case RegNtPostOpenKey:
            return L"RegNtPostOpenKey";
        case RegNtPreKeyHandleClose:
            return L"RegNtPreKeyHandleClose";
        case RegNtPostDeleteKey:
            return L"RegNtPostDeleteKey";
        case RegNtPostSetValueKey:
            return L"RegNtPostSetValueKey";
        case RegNtPostDeleteValueKey:
            return L"RegNtPostDeleteValueKey";
        case RegNtPostSetInformationKey:
            return L"RegNtPostSetInformationKey";
        case RegNtPostRenameKey:
            return L"RegNtPostRenameKey";
        case RegNtPostEnumerateKey:
            return L"RegNtPostEnumerateKey";
        case RegNtPostEnumerateValueKey:
            return L"RegNtPostEnumerateValueKey";
        case RegNtPostQueryKey:
            return L"RegNtPostQueryKey";
        case RegNtPostQueryValueKey:
            return L"RegNtPostQueryValueKey";
        case RegNtPostQueryMultipleValueKey:
            return L"RegNtPostQueryMultipleValueKey";
        case RegNtPostKeyHandleClose:
            return L"RegNtPostKeyHandleClose";
        case RegNtPreCreateKeyEx:
            return L"RegNtPreCreateKeyEx";
        case RegNtPostCreateKeyEx:
            return L"RegNtPostCreateKeyEx";
        case RegNtPreOpenKeyEx:
            return L"RegNtPreOpenKeyEx";
        case RegNtPostOpenKeyEx:
            return L"RegNtPostOpenKeyEx";
        case RegNtPreFlushKey:
            return L"RegNtPreFlushKey";
        case RegNtPostFlushKey:
            return L"RegNtPostFlushKey";
        case RegNtPreLoadKey:
            return L"RegNtPreLoadKey";
        case RegNtPostLoadKey:
            return L"RegNtPostLoadKey";
        case RegNtPreUnLoadKey:
            return L"RegNtPreUnLoadKey";
        case RegNtPostUnLoadKey:
            return L"RegNtPostUnLoadKey";
        case RegNtPreQueryKeySecurity:
            return L"RegNtPreQueryKeySecurity";
        case RegNtPostQueryKeySecurity:
            return L"RegNtPostQueryKeySecurity";
        case RegNtPreSetKeySecurity:
            return L"RegNtPreSetKeySecurity";
        case RegNtPostSetKeySecurity:
            return L"RegNtPostSetKeySecurity";
        case RegNtCallbackObjectContextCleanup:
            return L"RegNtCallbackObjectContextCleanup";
        default:
            return L"UNKNOWN";
    }
}
