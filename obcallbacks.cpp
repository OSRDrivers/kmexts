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

#if (NTDDI_VERSION >= NTDDI_VISTA)

OB_CALLBACK_CONTEXT KmExtsObCbContext;

_Ret_z_
PWCHAR 
ObjectTypeToString(
    _In_ POBJECT_TYPE ObjectType
    );

_Use_decl_annotations_
OB_PREOP_CALLBACK_STATUS 
KmExtsObPreCallback(
    PVOID RegistrationContext,
    POB_PRE_OPERATION_INFORMATION OperationInformation) 
{

    POB_PRE_OPERATION_PARAMETERS         operationParams;
    POB_PRE_CREATE_HANDLE_INFORMATION    createParams;
    POB_PRE_DUPLICATE_HANDLE_INFORMATION dupParams;

    UNREFERENCED_PARAMETER(RegistrationContext);

    operationParams = OperationInformation->Parameters;

    switch (OperationInformation->Operation) {
        case OB_OPERATION_HANDLE_CREATE: {

            createParams = &operationParams->CreateHandleInformation;

            DbgPrint("KmExtsObPreCallback: %s Handle being created to object "\
                     "0x%p, which is a %ls. Desired access is 0x%x\n",
                     OperationInformation->KernelHandle ? "Kernel" : "User",
                     OperationInformation->Object,
                     ObjectTypeToString(OperationInformation->ObjectType),
                     createParams->OriginalDesiredAccess);
            break;
        }
        case OB_OPERATION_HANDLE_DUPLICATE: {
            dupParams = &operationParams->DuplicateHandleInformation;
            DbgPrint("KmExtsObPreCallback: %s Handle being dup'd to object "\
                     "0x%p, which is a %ls. Desired access is 0x%x. "\
                     "Source Process 0x%p, Target Process 0x%p\n",
                     OperationInformation->KernelHandle ? "Kernel" : "User",
                     OperationInformation->Object,
                     ObjectTypeToString(OperationInformation->ObjectType),
                     dupParams->OriginalDesiredAccess,
                     dupParams->SourceProcess,
                     dupParams->TargetProcess);
            break;
        } 
        default: {
            DbgPrint("KmExtsObPreCallback: Unknown operation 0x%x!\n",
                     OperationInformation->Operation);
            break;
        }
    }
    return OB_PREOP_SUCCESS;
}

_Use_decl_annotations_
VOID
KmExtsObPostCallback(
    PVOID RegistrationContext,
    POB_POST_OPERATION_INFORMATION OperationInformation) 
{
    POB_POST_OPERATION_PARAMETERS         operationParams;
    POB_POST_CREATE_HANDLE_INFORMATION    createParams;
    POB_POST_DUPLICATE_HANDLE_INFORMATION dupParams;

    UNREFERENCED_PARAMETER(RegistrationContext);

    operationParams = OperationInformation->Parameters;

    switch (OperationInformation->Operation) {
        case OB_OPERATION_HANDLE_CREATE: {

            createParams = &operationParams->CreateHandleInformation;

            DbgPrint("KmExtsObPostCallback: %s Handle create to object "\
                     "0x%p, which is a %ls. Result was 0x%x, "\
                     "Granted Access 0x%x\n",
                     OperationInformation->KernelHandle ? "Kernel" : "User",
                     OperationInformation->Object,
                     ObjectTypeToString(OperationInformation->ObjectType),
                     OperationInformation->ReturnStatus,
                     createParams->GrantedAccess);
            break;
        }
        case OB_OPERATION_HANDLE_DUPLICATE: {
            dupParams = &operationParams->DuplicateHandleInformation;
            DbgPrint("KmExtsObPostCallback: %s Handle dup to object "\
                     "0x%p, which is a %ls. Result was 0x%x, "\
                     "Granted Access 0x%x\n",
                     OperationInformation->KernelHandle ? "Kernel" : "User",
                     OperationInformation->Object,
                     ObjectTypeToString(OperationInformation->ObjectType),
                     OperationInformation->ReturnStatus,
                     dupParams->GrantedAccess);
            break;
        } 
        default: {
            DbgPrint("KmExtsObPreCallback: Unknown operation 0x%x!\n",
                     OperationInformation->Operation);
            break;
        }
    }
    return;
}

_Use_decl_annotations_
PWCHAR 
ObjectTypeToString(
    POBJECT_TYPE ObjectType) 
{
    if (ObjectType == *IoFileObjectType) {
        return L"IoFileObjectType";
    }
    if (ObjectType == *ExEventObjectType) {
        return L"ExEventObjectType";
    }
    if (ObjectType == *ExSemaphoreObjectType) {
        return L"ExSemaphoreObjectType";
    }
    if (ObjectType == *TmTransactionManagerObjectType) {
        return L"TmTransactionManagerObjectType";
    }
    if (ObjectType == *TmResourceManagerObjectType) {
        return L"TmResourceManagerObjectType";
    }
    if (ObjectType == *TmEnlistmentObjectType) {
        return L"TmEnlistmentObjectType";
    }
    if (ObjectType == *TmTransactionObjectType) {
        return L"TmTransactionObjectType";
    }
    if (ObjectType == *PsProcessType) {
        return L"PsProcessType";
    }
    if (ObjectType == *PsThreadType) {
        return L"PsThreadType";
    }

    return L"UNKNOWN";
}

#endif
