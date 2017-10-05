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

_Use_decl_annotations_
extern "C"
NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath) 
{

    NTSTATUS       status;
    UNICODE_STRING callbackName;
    UNICODE_STRING altitude;

    UNREFERENCED_PARAMETER(RegistryPath);

#if (NTDDI_VERSION >= NTDDI_VISTA)
    OB_CALLBACK_REGISTRATION   obCbRegistration;
    POB_OPERATION_REGISTRATION obOpRegistration = NULL;
    ULONG                      obOpSize;
#endif

    // 
    // In this driver we're going to register for every callback
    // possible. This will include:
    // 
    // Configuration Manager (Cm) registry callbacks
    // 
    // Built in Executive (Ex) callbacks
    // 
    // Object Manager (Ob) object callbacks for every object type
    // 
    // Process Manager (Ps) callbacks for:
    // 
    //   - Process creation
    // 
    //   - Thread creation
    // 
    //   - Image loading
    // 
    // Support for each of these will vary by platform. Instead of
    // dynamically determining what's available, we'll do it at
    // compile time (only because it means less code for the
    // example).
    // 

    // 
    // CM CALLBACKS
    // 

    // 
    // We'll start with the Cm callbacks. Vista adds the Ex version,
    // so we'll use that if appropriate.
    // 

    // 
    // We use a global structure as our callback context for
    // illustrative purposes.
    // 
    KmExtsCmCallbackContext.MagicNumber = CM_CALLBACK_CONTEXT_MAGIC;

#if (NTDDI_VERSION < NTDDI_VISTA)
    
    // 
    // Legacy platform. Use the old DDI.
    // 
    status = CmRegisterCallback(
                           KmExtsCmRegistryCallback,
                           &KmExtsCmCallbackContext,
                           &KmExtsCmCallbackContext.CallbackRegistrationCookie);
#else

    // 
    // Pick a random altitude value
    // 
    RtlInitUnicodeString(&altitude,
                         L"12345");

    status = CmRegisterCallbackEx(
                           KmExtsCmRegistryCallback,
                           &altitude,
                           DriverObject,
                           &KmExtsCmCallbackContext,
                           &KmExtsCmCallbackContext.CallbackRegistrationCookie,
                           NULL); 
#endif
    
    if (!NT_SUCCESS(status)) {
        DbgPrint("CmRegisterCallback failed! Status 0x%x\n", 
                 status);
        goto Exit;
    }

    // 
    // Registered that callback
    // 
    KmExtsCmCallbackContext.CallbackRegistered = TRUE;

    // 
    // SYSTEM DEFINED EX CALLBACKS
    // 

    // 
    // Next we'll register for the system defined Ex callbacks. We'll use
    // a helper routine since there are a couple of steps and we
    // need to do this several times (once for each callback we'd
    // like to be notified of)
    // 

    // 
    // Register for the time change callback. 
    // 
    RtlInitUnicodeString(&callbackName,
                         L"\\Callback\\SetSystemTime");

    status = KmExtsCreateAndRegisterCallback(&callbackName,
                                             KmExtsExSetSystemTimeCallback,
                                             &KmExtsExSetTimeCbContext);
    if (!NT_SUCCESS(status)) {
        DbgPrint("CmRegisterCallback (%wZ) failed! Status 0x%x\n",
                 &callbackName, 
                 status);
        goto Exit;
    }

    // 
    // Now the power state callback.
    // 
    RtlInitUnicodeString(&callbackName,
                         L"\\Callback\\PowerState");

    status = KmExtsCreateAndRegisterCallback(&callbackName,
                                             KmExtsExPowerStateCallback,
                                             &KmExtsExPowerStateCbContext);
    if (!NT_SUCCESS(status)) {
        DbgPrint("CmRegisterCallback (%wZ) failed! Status 0x%x\n",
                 &callbackName, 
                 status);
        goto Exit;
    }

    // 
    // Now the processor add callback.
    // 
    RtlInitUnicodeString(&callbackName,
                         L"\\Callback\\ProcessorAdd");

    status = KmExtsCreateAndRegisterCallback(&callbackName,
                                             KmExtsExProcessorAddCallback,
                                             &KmExtsExProcAddCbContext);
    if (!NT_SUCCESS(status)) {

        // 
        // This is a fairly new callback. Let's not fail if it wasn't
        // found.
        // 
        if (status != STATUS_OBJECT_NAME_NOT_FOUND) {
            DbgPrint("CmRegisterCallback (%wZ) failed! Status 0x%x\n",
                     &callbackName, 
                     status);
            goto Exit;
        } else {
            DbgPrint("Processor add callback not available. Continuing.\n");
        }

    }

#if (NTDDI_VERSION >= NTDDI_VISTA)
    // 
    // OBJECT MANAGER CALLBACKS
    // 

    // 
    // These are only available in SP1 and later.
    // 

    // 
    // We'll register for pre and post operation callbacks for every
    // object type that is available.
    // 

    KmExtsObCbContext.MagicNumber = OB_CALLBACK_CONTEXT_MAGIC;

    // 
    // Fill in the header portion of the structure.
    // 
    obCbRegistration.Version = OB_FLT_REGISTRATION_VERSION;
    obCbRegistration.OperationRegistrationCount = KMEXTS_MAX_OBJECT_TYPE;
    obCbRegistration.RegistrationContext = &KmExtsObCbContext;

    // 
    // Again, a random altitude
    // 
    RtlInitUnicodeString(&obCbRegistration.Altitude,
                         L"123456");

    //
    // Now allocate and fill in the array of types that we want to
    // filter.
    // 
    obOpSize = (sizeof(OB_OPERATION_REGISTRATION) * KMEXTS_MAX_OBJECT_TYPE);
    obOpRegistration = (POB_OPERATION_REGISTRATION)
                            ExAllocatePoolWithTag(NonPagedPool,
                                                  obOpSize,
                                                  'POmK');

    if (obOpRegistration == NULL) {
        DbgPrint("No memory for operation registration array!\n");
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    obCbRegistration.OperationRegistration = obOpRegistration;

    // 
    // Fill in the common area of each structure.
    // 
    for (ULONG i = 0; i < KMEXTS_MAX_OBJECT_TYPE; i++) {
        obOpRegistration[i].Operations = OB_OPERATION_HANDLE_CREATE |
                                         OB_OPERATION_HANDLE_DUPLICATE;
        obOpRegistration[i].PreOperation  = KmExtsObPreCallback;
        obOpRegistration[i].PostOperation = KmExtsObPostCallback;
    }

    // 
    // Now fill in the object types.
    // 

    // 
    // Object callbacks are only supported for object types that are
    // created with the SupportsObjectCallbacks flag set (currently
    // only process and thread)
    // 
    obOpRegistration[KMEXTS_PROCESS_OBJECT_TYPE].ObjectType = PsProcessType;
    obOpRegistration[KMEXTS_THREAD_OBJECT_TYPE].ObjectType = PsThreadType;

    // MUST BE LINKED WITH /INTEGRITYCHECK

    // 
    // Now, yes, finally register for the callbacks...
    // 
    status = ObRegisterCallbacks(&obCbRegistration,
                                 &KmExtsObCbContext.RegistrationHandle);


    if (!NT_SUCCESS(status)) {
        DbgPrint("ObRegisterCallbacks failed! Status 0x%x\n", 
                 status);
        goto Exit;
    }

    KmExtsObCbContext.CallbackRegistered = TRUE;


#endif // #if (NTDDI_VERSION >= NTDDI_VISTA)


    // 
    // PS CALLBACKS 
    // 
#if (NTDDI_VERSION < NTDDI_VISTA)

    status = PsSetCreateProcessNotifyRoutine(KmExtsPsCreateProcessNotifyRoutine,
                                             FALSE);

    if (!NT_SUCCESS(status)) {
        DbgPrint("PsSetCreateProcessNotifyRoutine failed! Status 0x%x\n", 
                 status);
        goto Exit;
    }

    KmExtsPsCreateProcessNotifyRoutineRegistered = TRUE;

#else

    status = PsSetCreateProcessNotifyRoutineEx(
                                KmExtsPsCreateProcessNotifyRoutineEx,
                                FALSE);

    if (!NT_SUCCESS(status)) {
        DbgPrint("PsSetCreateProcessNotifyRoutineEx failed! Status 0x%x\n",
                 status);
        goto Exit;
    }

    KmExtsPsCreateProcessExNotifyRoutineRegistered = TRUE;

#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    status = PsSetCreateProcessNotifyRoutineEx2(PsCreateProcessNotifySubsystems,
                                                KmExtsPsCreateProcessNotifyRoutineEx2,
                                                FALSE);

    if (!NT_SUCCESS(status)) {
        DbgPrint("PsSetCreateProcessNotifyRoutineEx2 failed! Status 0x%x\n",
                 status);
        goto Exit;
    }

    KmExtsPsCreateProcessEx2NotifyRoutineRegistered = TRUE;
#endif



    status = PsSetCreateThreadNotifyRoutine(KmExtsPsCreateThreadNotifyRoutine);

    if (!NT_SUCCESS(status)) {
        DbgPrint("PsSetCreateThreadNotifyRoutine failed! Status 0x%x\n",
                 status);
        goto Exit;
    }

    KmExtsPsCreateThreadNotifyRoutineRegistered = TRUE;

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
    status = PsSetCreateThreadNotifyRoutineEx(PsCreateThreadNotifyNonSystem,
                                              KmExtsPsCreateThreadNotifyRoutineEx);

    if (!NT_SUCCESS(status)) {
        DbgPrint("PsSetCreateThreadNotifyRoutineEx failed! Status 0x%x\n",
                 status);
        goto Exit;
    }

    KmExtsPsCreateThreadNotifyRoutineExRegistered = TRUE;
#endif

    status = PsSetLoadImageNotifyRoutine(KmExtsPsLoadImageNotifyRoutine);

    if (!NT_SUCCESS(status)) {
        DbgPrint("PsSetLoadImageNotifyRoutine failed! Status 0x%x\n",
                 status);
        goto Exit;
    }

    KmExtsPsLoadImageNotifyRoutineRegistered = TRUE;
    
    DriverObject->DriverUnload = KmExtsUnload;    

    status = STATUS_SUCCESS;

Exit:

#if (NTDDI_VERSION >= NTDDI_VISTA)
    if (obOpRegistration != NULL) {
        ExFreePool(obOpRegistration);
    }
#endif

    if (!NT_SUCCESS(status)) {
        // 
        // Undo what we've done and fail.
        // 
        KmExtsUnregisterAllCallbacks();
    }

    return status;
}


_Use_decl_annotations_
VOID
KmExtsUnload(
    PDRIVER_OBJECT DriverObject) 
{

    UNREFERENCED_PARAMETER(DriverObject);

    KmExtsUnregisterAllCallbacks();
    return;
}

VOID
KmExtsUnregisterAllCallbacks(
    VOID) 
{

    NTSTATUS status;

    // 
    // Undo anything we may have done.
    // 

    if (KmExtsCmCallbackContext.CallbackRegistered == TRUE) {
        status = CmUnRegisterCallback(
                        KmExtsCmCallbackContext.CallbackRegistrationCookie);

        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }

    if (KmExtsExSetTimeCbContext.CallbackRegistered == TRUE){
        ExUnregisterCallback(KmExtsExSetTimeCbContext.RegistrationHandle);
    }

    if (KmExtsExPowerStateCbContext.CallbackRegistered == TRUE) {
        ExUnregisterCallback(KmExtsExPowerStateCbContext.RegistrationHandle);
    }
    if (KmExtsExProcAddCbContext.CallbackRegistered == TRUE){
        ExUnregisterCallback(KmExtsExProcAddCbContext.RegistrationHandle);
    }
    if (KmExtsObCbContext.CallbackRegistered) {
        ObUnRegisterCallbacks(KmExtsObCbContext.RegistrationHandle);
    }

#if (NTDDI_VERSION < NTDDI_VISTA)
    if (KmExtsPsCreateProcessNotifyRoutineRegistered == TRUE) {
        status = PsSetCreateProcessNotifyRoutine(
                                           KmExtsPsCreateProcessNotifyRoutine,
                                           TRUE);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#else
    if (KmExtsPsCreateProcessExNotifyRoutineRegistered == TRUE) {
        status = PsSetCreateProcessNotifyRoutineEx(
                                           KmExtsPsCreateProcessNotifyRoutineEx,
                                           TRUE);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2) 
    if (KmExtsPsCreateProcessExNotifyRoutineRegistered == TRUE) {
        status = PsSetCreateProcessNotifyRoutineEx2(
                                        PsCreateProcessNotifySubsystems,
                                        KmExtsPsCreateProcessNotifyRoutineEx2,
                                        TRUE);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#endif

    if (KmExtsPsCreateThreadNotifyRoutineRegistered == TRUE) {
        status = PsRemoveCreateThreadNotifyRoutine(
                                            KmExtsPsCreateThreadNotifyRoutine);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
    if (KmExtsPsCreateThreadNotifyRoutineExRegistered == TRUE) {
        status = PsRemoveCreateThreadNotifyRoutine(
                                            KmExtsPsCreateThreadNotifyRoutineEx);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#endif

    if (KmExtsPsLoadImageNotifyRoutineRegistered == TRUE) {
        status = PsRemoveLoadImageNotifyRoutine(KmExtsPsLoadImageNotifyRoutine);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }

}
