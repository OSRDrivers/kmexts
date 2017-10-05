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

EX_CALLBACK_CONTEXT KmExtsExSetTimeCbContext;
EX_CALLBACK_CONTEXT KmExtsExPowerStateCbContext;
EX_CALLBACK_CONTEXT KmExtsExProcAddCbContext;

_Use_decl_annotations_
NTSTATUS
KmExtsCreateAndRegisterCallback(
    PUNICODE_STRING CallbackName,
    PCALLBACK_FUNCTION CallbackFunction,
    PEX_CALLBACK_CONTEXT CallbackContext) 
{

    OBJECT_ATTRIBUTES oa;
    NTSTATUS          status;

    // 
    // Set up our context structure.
    // 
    CallbackContext->MagicNumber = EX_CALLBACK_CONTEXT_MAGIC;

    // 
    // Set up the object attibutes for the call.
    // 
    InitializeObjectAttributes(&oa, 
                               CallbackName,
                               OBJ_PERMANENT | OBJ_KERNEL_HANDLE,
                               NULL,
                               NULL);

    // 
    // Now open the executive callback.
    // 
    status = ExCreateCallback(&CallbackContext->ExCallbackObject,
                              &oa,
                              FALSE, // Do NOT create the object!
                              TRUE);

    if (!NT_SUCCESS(status)) {
        DbgPrint("ExCreateCallback for %wZ failed! Status 0x%x\n", 
                 CallbackName, status);
        goto Exit;
    }

    // 
    // Now that we've opened the callback, we must register for
    // notification of it.
    // 
    CallbackContext->RegistrationHandle = 
                    ExRegisterCallback(CallbackContext->ExCallbackObject,
                                       CallbackFunction,
                                       CallbackContext);

    if (CallbackContext->RegistrationHandle == NULL) {
        DbgPrint("ExRegisterCallback for %wZ failed!\n", CallbackName);
        status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    CallbackContext->CallbackRegistered = TRUE;
    status = STATUS_SUCCESS;

Exit:
    return status;
}


_Use_decl_annotations_
VOID
KmExtsExSetSystemTimeCallback(
    PVOID CallbackContext,
    PVOID Argument1,
    PVOID Argument2) 
{

    LARGE_INTEGER systemTime;
    LARGE_INTEGER localTime;
    TIME_FIELDS   timeFields;

    UNREFERENCED_PARAMETER(CallbackContext);
    UNREFERENCED_PARAMETER(Argument1);
    UNREFERENCED_PARAMETER(Argument2);

    DbgPrint("KmExtsExSetSystemTimeCallback: System time has changed!\n");

    KeQuerySystemTime(&systemTime);
    ExSystemTimeToLocalTime(&systemTime, &localTime);

    RtlTimeToTimeFields(&localTime, &timeFields);

    DbgPrint("Current time is %2.2d:%2.2d:%2.2d %d AD\n",
             timeFields.Hour, timeFields.Minute,
             timeFields.Second, timeFields.Year);


    return;
}

_Use_decl_annotations_
VOID
KmExtsExPowerStateCallback(
    PVOID CallbackContext,
    PVOID Argument1,
    PVOID Argument2) 
{
    UNREFERENCED_PARAMETER(CallbackContext);
    UNREFERENCED_PARAMETER(Argument1);
    UNREFERENCED_PARAMETER(Argument2);

    DbgPrint("KmExtsExPowerStateCallback: Power state changing or "\
             "has changed!\n"); 
    return;
}

_Use_decl_annotations_
VOID
KmExtsExProcessorAddCallback(
    PVOID CallbackContext,
    PVOID Argument1,
    PVOID Argument2) 
{
    UNREFERENCED_PARAMETER(CallbackContext);
    UNREFERENCED_PARAMETER(Argument1);
    UNREFERENCED_PARAMETER(Argument2);

    DbgPrint("KmExtsExProcessorAddCallback: Processor added!\n"); 
    return;
}

