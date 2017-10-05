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

BOOLEAN KmExtsPsCreateProcessNotifyRoutineRegistered;
BOOLEAN KmExtsPsCreateProcessExNotifyRoutineRegistered;
#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
BOOLEAN KmExtsPsCreateProcessEx2NotifyRoutineRegistered;
#endif
BOOLEAN KmExtsPsCreateThreadNotifyRoutineRegistered;
#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
BOOLEAN KmExtsPsCreateThreadNotifyRoutineExRegistered;
#endif
BOOLEAN KmExtsPsLoadImageNotifyRoutineRegistered;

_Use_decl_annotations_
VOID
KmExtsPsCreateProcessNotifyRoutine(
    HANDLE ParentId,
    HANDLE ProcessId,
    BOOLEAN Create) 
{
    // 
    // This routine called after creation of first thread in
    // process!
    // 
    DbgPrint("KmExtsPsCreateProcessNotifyRoutine: PID 0x%p %s. "\
             "Parent PID 0x%p\n", 
             ProcessId, 
             Create ? " being created" : "exiting",
             ParentId);
    return;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
_Use_decl_annotations_
VOID
KmExtsPsCreateProcessNotifyRoutineEx(
    PEPROCESS Process,
    HANDLE ProcessId,
    PPS_CREATE_NOTIFY_INFO CreateInfo) 
{

    if (CreateInfo == NULL) {
        DbgPrint("KmExtsPsCreateProcessNotifyRoutine: PID 0x%p exiting. "\
                 "Process PID 0x%p\n", 
                 Process, 
                 ProcessId);
        return;
    }
    // 
    // This routine called after creation of first thread in
    // process!
    // 
    DbgPrint("KmExtsPsCreateProcessNotifyRoutine: Process 0x%p (PID 0x%p). "\
             "Parent PID 0x%p. Creating Thread 0x%p.%p\n",
             Process,
             ProcessId,
             CreateInfo->ParentProcessId,
             CreateInfo->CreatingThreadId.UniqueProcess,
             CreateInfo->CreatingThreadId.UniqueThread);
    if (CreateInfo->CommandLine != NULL) {
        DbgPrint("\t(0x%p) Command Line %wZ\n", 
                 ProcessId, 
                 CreateInfo->CommandLine);
    }
    DbgPrint("\t(0x%p) Backing File Object 0x%p\n", 
             ProcessId, 
             CreateInfo->FileObject);

    if (CreateInfo->FileOpenNameAvailable) {
        DbgPrint("\t(0x%p) Full Image Name %wZ\n", 
                 ProcessId, 
                 CreateInfo->ImageFileName);
    } else {
        DbgPrint("\t(0x%p) Partial Image Name %wZ\n", 
                 ProcessId, 
                 CreateInfo->ImageFileName);
    }

    return;
}
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
_Use_decl_annotations_
VOID
KmExtsPsCreateProcessNotifyRoutineEx2(
    PEPROCESS Process,
    HANDLE ProcessId,
    PPS_CREATE_NOTIFY_INFO CreateInfo) 
{
    if (CreateInfo == NULL) {
        DbgPrint("KmExtsPsCreateProcessNotifyRoutineEx2: PID 0x%p exiting. "\
                 "Process PID 0x%p\n", 
                 Process, 
                 ProcessId);
        return;
    }
    // 
    // This routine called after creation of first thread in
    // process!
    // 
    DbgPrint("KmExtsPsCreateProcessNotifyRoutineEx2: Process 0x%p (PID 0x%p). "\
             "Parent PID 0x%p. Creating Thread 0x%p.%p\n",
             Process,
             ProcessId,
             CreateInfo->ParentProcessId,
             CreateInfo->CreatingThreadId.UniqueProcess,
             CreateInfo->CreatingThreadId.UniqueThread);
    if (CreateInfo->CommandLine != NULL) {
        DbgPrint("\t(0x%p) Command Line %wZ\n", 
                 ProcessId, 
                 CreateInfo->CommandLine);
    }
    DbgPrint("\t(0x%p) Backing File Object 0x%p\n", 
             ProcessId, 
             CreateInfo->FileObject);

    if (CreateInfo->FileOpenNameAvailable) {
        DbgPrint("\t(0x%p) Full Image Name %wZ\n", 
                 ProcessId, 
                 CreateInfo->ImageFileName);
    } else {
        DbgPrint("\t(0x%p) Partial Image Name %wZ\n", 
                 ProcessId, 
                 CreateInfo->ImageFileName);
    }

    return;
}
#endif

_Use_decl_annotations_
VOID
KmExtsPsCreateThreadNotifyRoutine(
    HANDLE ProcessId,
    HANDLE ThreadId,
    BOOLEAN Create) 
{

    // 
    // When in this routine, target thread has been created and is
    // in the INITIALIZED state. It will not transition to READY
    // until this routine exits.
    // 

    DbgPrint("KmExtsPsCreateThreadNotifyRoutine: Thread 0x%p %s in "\
             "Process 0x%p\n", 
             ThreadId, 
             Create ? "being created" : "exiting",
             ProcessId);

    return;
}

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
_Use_decl_annotations_
VOID
KmExtsPsCreateThreadNotifyRoutineEx(
    HANDLE ProcessId,
    HANDLE ThreadId,
    BOOLEAN Create) 
{

    // 
    // When in this routine, target thread has been created and is
    // in the INITIALIZED state. It will not transition to READY
    // until this routine exits.
    // 

    DbgPrint("KmExtsPsCreateThreadNotifyRoutineEx: Thread 0x%p %s in "\
             "Process 0x%p\n", 
             ThreadId, 
             Create ? "being created" : "exiting",
             ProcessId);

    return;
}
#endif

_Use_decl_annotations_
VOID
KmExtsPsLoadImageNotifyRoutine(
    PUNICODE_STRING  FullImageName,
    HANDLE ProcessId,
    PIMAGE_INFO  ImageInfo) 
{

    PIMAGE_INFO_EX imageInfoEx = NULL;
    PFILE_OBJECT   backingFileObject;

    // 
    // IMAGE_INFO_EX available on Vista, which will give us the
    // backing file object of the image section.
    // 
    if (ImageInfo->ExtendedInfoPresent) {

        imageInfoEx = CONTAINING_RECORD(ImageInfo, IMAGE_INFO_EX, ImageInfo);

        backingFileObject = imageInfoEx->FileObject;

    } else {

        backingFileObject = NULL;

    }   


    DbgPrint("KmExtsPsLoadImageNotifyRoutine: %wZ being loaded into "\
             "Process 0x%p. Backing File Object %s (0x%p)\n", 
             FullImageName,
             ProcessId,
             backingFileObject != NULL ? "Available" : "Unavailable",
             backingFileObject);
    return;
}

