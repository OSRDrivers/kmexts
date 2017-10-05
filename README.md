# Kernel Mode Extensions Driver #
This is a very simple driver that registers the callbacks using the following routines:

- CmRegisterCallback
- CmRegisterCallbackEx
- ExRegisterCallback
- ObRegisterCallbacks
- PsSetCreateProcessNotifyRoutine
- PsSetCreateProcessNotifyRoutineEx
- PsSetCreateProcessNotifyRoutineEx2
- PsSetCreateThreadNotifyRoutine
- PsSetCreateThreadNotifyRoutineEx
- PsSetLoadImageNotifyRoutine

In each callback registered the driver DbgPrints some minimal amount of information describing the operation. It is meant as a convenient way to play with the behavior and invocation of these callbacks on different versions of Windows.

# Building #
The provided solution builds in Visual Studio 2015 with the Windows 10 1607 WDK

# Installation #

Create a service entry for the driver:

    sc create kmexts binpath=c:\users\osr\desktop\kmexts.sys type=kernel start=demand

Then start the driver:

	net start kmexts

Be sure to have DbgPrint messages enabled for your target machine either via the Registry or the debugger. [See Getting DbgPrint Output to Appear in Vista and Later](https://www.osr.com/getting-dbgprint-output-appear-windows-vista-later/).


