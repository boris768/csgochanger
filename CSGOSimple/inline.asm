.686
.MODEL FLAT, C
.STACK
.CODE

handler proc
handler endp
.safeseh handler

GetPEB PROC
ASSUME FS:NOTHING
mov eax, fs:[48]
ASSUME FS:ERROR
ret
GetPEB ENDP

GetTEB PROC
ASSUME FS:NOTHING
mov eax, fs:[24]
ASSUME FS:ERROR
ret
GetTEB ENDP
END 