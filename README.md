# The Game

Simulation playground for drone fights.

## Installation

Download and install UE5.
Download and install matching Visual Studio compiler.
In PowerShell terminal run:
```
winget install Python.Python.3.14
```
Run .uproject file to build project.
In PowerShell terminal run:
```
.\Tools\SetupProject.ps1
```

## Run

1. In UE5 editor click play button.

2. In PowerShell terminal run:
```
.\Tools\RunServer.ps1
```

## Troubleshooting

1. Error running command: .\Tools\ProjSetup.ps1

```
.\Tools\ProjSetup.ps1 : File ...\TheGame\Tools\ProjSetup.ps1 cannot be loaded because running scripts is disabled on this system. For more information, see about_Execution_Policies at https:/go.microso
ft.com/fwlink/?LinkID=135170.
At line:1 char:1
+ .\Tools\ProjSetup.ps1
+ ~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : SecurityError: (:) [], PSSecurityException
    + FullyQualifiedErrorId : UnauthorizedAccess
```

Solution:

In PowerShell Terminal run:
```
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
```
