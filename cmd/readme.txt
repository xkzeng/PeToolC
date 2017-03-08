
PostBuild_Cmds=cmd\RenameByTime Debug\PeToolC.exe D
PostBuild_Cmds=cmd\RenameByTime Release\PeToolC.exe R

PostBuild_Cmds=cmd\CopyByTime Debug\PeToolC.exe D
PostBuild_Cmds=cmd\CopyByTime Release\PeToolC.exe R

PostBuild_Cmds=cscript cmd\CopyByTime.vbs Debug\PeToolC.exe D
PostBuild_Cmds=cscript cmd\CopyByTime.js Release\PeToolC.exe R

PostBuild_Cmds=copy /Y Debug\PeToolC.exe Debug\PeTool%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%_D.exe
PostBuild_Cmds=copy /Y Release\PeToolC.exe Release\PeTool%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%_R.exe

PostBuild_Cmds=echo F|xcopy /Y Debug\PeToolC.exe Debug\PeTool%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%_D.exe
PostBuild_Cmds=echo F|xcopy /Y Release\PeToolC.exe Release\PeTool%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%_R.exe