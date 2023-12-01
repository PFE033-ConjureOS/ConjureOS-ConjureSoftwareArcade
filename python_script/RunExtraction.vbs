Set WshShell = WScript.CreateObject("WScript.Shell")
dim fso: set fso = CreateObject("Wscript.Shell")

strScriptFolder = fso.GetParentFolderName(WScript.ScriptFullName)

WshShell.Run Chr(34) & strScriptFolder & "Runner_Extraction.bat" & Chr(34) ,0,True