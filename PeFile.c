
#include "PeFile.h"
#include <stdio.h>

struct SPeFile* g_lpPeFile = NULL;
static struct SPeFile* this = NULL;

static WORD SetFileName(LPCTSTR strFileName)
{
  WORD wLen = 0;

  if((NULL == strFileName) || (strlen(strFileName) == 0))
  {
    return 0;
  }
  
  wLen = _snprintf(this->strFileName, FILE_PATH_LEN, "%s", strFileName);
  return wLen;
}

static BOOL PE_Open(LPCTSTR strFileName, DWORD dwDesiredAccess/* = GENERIC_READ*/, DWORD dwShareMode/* = FILE_SHARE_READ*/)
{
  if(SetFileName(strFileName) == 0)
  {
    return FALSE;
  }
  
  //Open File
  this->hFile = CreateFile(this->strFileName, dwDesiredAccess, dwShareMode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
  if((INVALID_HANDLE_VALUE == this->hFile) || (NULL == this->hFile))
  {
    return FALSE;
  }
  return TRUE;
}

static BOOL PE_MapFile(DWORD dwMapProtect, LPCSTR lpObjectName/* = NULL*/)
{
  if((INVALID_HANDLE_VALUE == this->hFile) || (NULL == this->hFile))
  {
    return FALSE;
  }
  
  //Mapping the file into memory
  this->hMapFile = CreateFileMapping(this->hFile, NULL, dwMapProtect, 0, 0, NULL);
  if(NULL == this->hMapFile)
  {
    return FALSE;
  }
  return TRUE;
}

static BOOL PE_MapFileView(DWORD dwDesiredViewAccess, SIZE_T dwNumberOfBytesToMap/* = 0*/)
{
  if(NULL == this->hMapFile)
  {
    return FALSE;
  }

  //Mapping the view of the file into the address space of the calling process;
  this->lpvBase = MapViewOfFile(this->hMapFile, dwDesiredViewAccess, 0, 0, dwNumberOfBytesToMap);
  if(NULL == this->lpvBase)
  {
    return FALSE;
  }
  this->strBase = (LPBYTE)(this->lpvBase);
  return TRUE;
}

static BOOL PE_UnMapFileView(void)
{
  BOOL bResult = FALSE;

  if(NULL == this->lpvBase)
  {
    return FALSE;
  }

  bResult = UnmapViewOfFile(this->lpvBase);
  if(FALSE == bResult)
  {
    return FALSE;
  }

  this->lpvBase = NULL;
  this->strBase = NULL;
  return TRUE;
}

static BOOL PE_OpenEx(LPCTSTR strFileName, LPDWORD lpResult, DWORD dwDesiredAccess/* = GENERIC_READ*/, DWORD dwShareMode/* = FILE_SHARE_READ*/, DWORD dwMapProtect/* = PAGE_READONLY*/, DWORD dwDesiredViewAccess/* = FILE_MAP_READ*/)
{
  if(SetFileName(strFileName) == 0)
  {
    *lpResult = 1;
    return FALSE;
  }

  //Open File
  this->hFile = CreateFile(this->strFileName, dwDesiredAccess, dwShareMode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
  if((INVALID_HANDLE_VALUE == this->hFile) || (NULL == this->hFile))
  {
    *lpResult = 2;
    return FALSE;
  }

  //Mapping the file into memory
  this->hMapFile = CreateFileMapping(this->hFile, NULL, dwMapProtect, 0, 0, NULL);
  if(NULL == this->hMapFile)
  {
    CloseHandle(this->hFile);
    this->hFile = NULL;
    *lpResult = 3;
    return FALSE;
  }

  //Mapping the view of the file into the address space of the calling process;
  this->lpvBase = MapViewOfFile(this->hMapFile, dwDesiredViewAccess, 0, 0, 0);
  if(NULL == this->lpvBase)
  {
    CloseHandle(this->hMapFile);
    this->hMapFile = NULL;
    CloseHandle(this->hFile);
    this->hFile = NULL;
    *lpResult = 4;
    return FALSE;
  }
  
  this->strBase = (LPBYTE)(this->lpvBase);
  
  *lpResult = 0;
  return TRUE;
}

static VOID PE_Close(void)
{
  //Unmap all mapped views of the file-mapping object
  if(NULL != this->lpvBase)
  {
    UnmapViewOfFile(this->lpvBase);
    this->lpvBase = NULL;
    this->strBase = NULL;
  }

  //Close the file-mapping object handle
  if(NULL != this->hMapFile)
  {
    CloseHandle(this->hMapFile);
    this->hMapFile = NULL;
  }

  //Close the object handle of the file
  if((INVALID_HANDLE_VALUE != this->hFile) && (NULL != this->hFile))
  {
    CloseHandle(this->hFile);
    this->hFile = NULL;
  }
}

static LPSTR PE_GetMachineString(WORD machine)
{
  LPSTR name = NULL;
  switch(machine)
  {
    case IMAGE_FILE_MACHINE_I386:
    {
      name = "I386";
    }
    break;

    case IMAGE_FILE_MACHINE_IA64:
    {
      name = "IA64";
    }
    break;

    case IMAGE_FILE_MACHINE_ARM:
    {
      name = "ARM";
    }
    break;

    case IMAGE_FILE_MACHINE_THUMB:
    {
      name = "ARM Thumb";
    }
    break;

    case IMAGE_FILE_MACHINE_ALPHA:
    {
      name = "ALPHA";
    }
    break;

    case IMAGE_FILE_MACHINE_ALPHA64:
    {
      name = "ALPHA64";
    }
    break;

    case IMAGE_FILE_MACHINE_POWERPC:
    {
      name = "POWERPC";
    }
    break;

    default:
    {
      name = "Other";
    }
  }
  return name;
}

static LPSTR PE_GetSubsystemString(WORD subsystem, WORD magic)
{
  LPSTR name = NULL;
  switch(subsystem)
  {
    case IMAGE_SUBSYSTEM_UNKNOWN:
    {
      name = "Unknown";
    }
    break;

    case IMAGE_SUBSYSTEM_NATIVE:
    {
      name = "No Need Subsystem";
    }
    break;

    case IMAGE_SUBSYSTEM_WINDOWS_GUI:
    {
      if(IMAGE_NT_OPTIONAL_HDR32_MAGIC == magic)
      {
        name = "Win32 GUI";
      }
      else if(IMAGE_NT_OPTIONAL_HDR64_MAGIC == magic)
      {
        name = "Win64 GUI";
      }
      else if(IMAGE_ROM_OPTIONAL_HDR_MAGIC == magic)
      {
        name = "ROM GUI";
      }
      else
      {
        name = "Windows GUI";
      }
    }
    break;

    case IMAGE_SUBSYSTEM_WINDOWS_CUI:
    {
      if(IMAGE_NT_OPTIONAL_HDR32_MAGIC == magic)
      {
        name = "Win32 CUI";
      }
      else if(IMAGE_NT_OPTIONAL_HDR64_MAGIC == magic)
      {
        name = "Win64 CUI";
      }
      else if(IMAGE_ROM_OPTIONAL_HDR_MAGIC == magic)
      {
        name = "ROM CUI";
      }
      else
      {
        name = "Windows CUI";
      }
    }
    break;

    case IMAGE_SUBSYSTEM_OS2_CUI:
    {
      name = "OS2 CUI";
    }
    break;

    case IMAGE_SUBSYSTEM_POSIX_CUI:
    {
      name = "Posix CUI";
    }
    break;

    case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:
    {
      name = "Win9X Driver";
    }
    break;

    case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:
    {
      name = "Windows CE GUI";
    }
    break;

    default:
    {
      name = "Other";
    }
  }
  return name;
}

static DWORD PE_GetSectionProperty(LPSTR strBuf, WORD wBufSize, DWORD dwCharacteristic)
{
  DWORD dwLen = 0;

  ZeroMemory(strBuf, wBufSize);

  if(dwCharacteristic & IMAGE_SCN_MEM_READ)
  {
    strBuf[dwLen++] = 'R';
  }

  if(dwCharacteristic & IMAGE_SCN_MEM_WRITE)
  {
    strBuf[dwLen++] = 'W';
  }

  if(dwCharacteristic & IMAGE_SCN_MEM_EXECUTE)
  {
    strBuf[dwLen++] = 'E';
  }

  if(dwCharacteristic & IMAGE_SCN_MEM_SHARED)
  {
    strBuf[dwLen++] = 'S';
  }

  if(dwCharacteristic & IMAGE_SCN_MEM_DISCARDABLE)
  {
    strBuf[dwLen++] = 'D';
  }

  if(dwCharacteristic & IMAGE_SCN_CNT_CODE)
  {
    strBuf[dwLen++] = 'C';
  }

  if(dwCharacteristic & IMAGE_SCN_CNT_INITIALIZED_DATA)
  {
    strBuf[dwLen++] = 'I';
  }

  if(dwCharacteristic & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
  {
    strBuf[dwLen++] = 'U';
  }
  return dwLen;
}

static DWORD PE_Encrpty(LPBYTE lpStart, WORD wLength, CONST BYTE bKey)
{
  LPBYTE pos = NULL, posEnd = NULL;
  BYTE tmp = 0;
  posEnd = lpStart + wLength;
  for(pos = lpStart; pos != posEnd; pos++)
  {
    //(*pos) = (*pos) & bKey;
    tmp = *pos;
    tmp = (tmp >> 4) | (tmp << 4);
    *pos = tmp;
  }
  return 0;
}

static DWORD PE_Decrpty(LPBYTE lpStart, WORD wLength, CONST BYTE bKey)
{
  LPBYTE pos = NULL, posEnd = NULL;
  BYTE tmp = 0;
  posEnd = lpStart + wLength;
  for(pos = lpStart; pos != posEnd; pos++)
  {
    //(*pos) = (*pos) & bKey;
    tmp = *pos;
    tmp = (tmp >> 4) | (tmp << 4);
    *pos = tmp;
  }
  return 0;
}

static DWORD PE_Rva2Foa(DWORD dwRva)
{
  WORD i = 0, wCount = 0;
  DWORD dwFoa = 0, dwBegin = 0, dwEnd = 0;
  PIMAGE_SECTION_HEADER aSections = NULL;
  PIMAGE_SECTION_HEADER lpSection = NULL;

  if((NULL == this) || (NULL == this->stFmtPe.lpBlkTbl) || (NULL == this->stFmtPe.lpFilHdr))
  {
    return 0;
  }

  aSections = this->stFmtPe.lpBlkTbl;                 //节表基址
  wCount = this->stFmtPe.lpFilHdr->NumberOfSections;  //节的数量

  for(i = 0; i < wCount; i++)
  {
    lpSection = aSections + i;
    dwBegin = lpSection->VirtualAddress;
    dwEnd   = lpSection->VirtualAddress + lpSection->Misc.VirtualSize;
    if(dwRva >= dwBegin && dwRva <= dwEnd)
    {
      dwFoa = (dwRva - lpSection->VirtualAddress) + lpSection->PointerToRawData;
      break;
    }
  }
  return dwFoa;
}

static DWORD PE_Foa2Rva(DWORD dwFoa)
{
  WORD i = 0, wCount = 0;
  DWORD dwRva = 0, dwBegin = 0, dwEnd = 0;
  PIMAGE_SECTION_HEADER aSections = NULL;
  PIMAGE_SECTION_HEADER lpSection = NULL;

  if((NULL == this) || (NULL == this->stFmtPe.lpBlkTbl) || (NULL == this->stFmtPe.lpFilHdr))
  {
    return 0;
  }

  aSections = this->stFmtPe.lpBlkTbl;                 //节表基址
  wCount = this->stFmtPe.lpFilHdr->NumberOfSections;  //节的数量

  for(i = 0; i < wCount; i++)
  {
    lpSection = aSections + i;
    dwBegin = lpSection->PointerToRawData;
    dwEnd   = lpSection->PointerToRawData + lpSection->SizeOfRawData;
    if(dwFoa >= dwBegin && dwFoa <= dwEnd)
    {
      dwRva = (dwFoa - lpSection->PointerToRawData) + lpSection->VirtualAddress;
      break;
    }
  }
  return dwRva;
}

static BOOL PE_Parse(LPDWORD lpdwResult)
{
  PIMAGE_DOS_HEADER lpDosHdr = NULL;
  PIMAGE_NT_HEADERS lpNtHdr  = NULL;
  DWORD dwStart = 0;

  if(NULL == this)
  {
    *lpdwResult = 1;
    return FALSE;
  }

  //check DOS Header
  lpDosHdr = (PIMAGE_DOS_HEADER)(this->strBase);
  if(IMAGE_DOS_SIGNATURE != lpDosHdr->e_magic) //Invalid DOS header
  {
    *lpdwResult = 2;
    return FALSE;
  }

  //check NT Header
  lpNtHdr = (PIMAGE_NT_HEADERS)(this->strBase + lpDosHdr->e_lfanew);
  if(IMAGE_NT_SIGNATURE != lpNtHdr->Signature)
  {
    *lpdwResult = 3;
    return FALSE;
  }

  //DOS Header
  this->stFmtPe.lpDosHdr = lpDosHdr;

  //NT Header
  this->stFmtPe.lpNtHdr  = lpNtHdr;

  //File Header
  this->stFmtPe.lpFilHdr = &(this->stFmtPe.lpNtHdr->FileHeader);

  //Optional Header
  this->stFmtPe.lpOptHdr = &(this->stFmtPe.lpNtHdr->OptionalHeader);

  //Data Directory
  this->stFmtPe.lpDatDir = this->stFmtPe.lpOptHdr->DataDirectory;

  //Section Table
  this->stFmtPe.lpBlkTbl = (PIMAGE_SECTION_HEADER)(this->strBase + this->stFmtPe.lpDosHdr->e_lfanew + sizeof(IMAGE_NT_HEADERS));

  //Export Directory
  this->stFmtPe.lpExpBlk = this->stFmtPe.lpDatDir + IMAGE_DIRECTORY_ENTRY_EXPORT;
  dwStart = this->Rva2Foa(this->stFmtPe.lpExpBlk->VirtualAddress);
  this->stFmtPe.lpExpTbl = (PIMAGE_EXPORT_DIRECTORY)(this->strBase + dwStart);

  //Import Directory
  this->stFmtPe.lpImpBlk = this->stFmtPe.lpDatDir + IMAGE_DIRECTORY_ENTRY_IMPORT;
  dwStart = this->Rva2Foa(this->stFmtPe.lpImpBlk->VirtualAddress);
  this->stFmtPe.lpImpTbl = (PIMAGE_IMPORT_DESCRIPTOR)(this->strBase + dwStart);

  //Resource Directory
  this->stFmtPe.lpResBlk = this->stFmtPe.lpDatDir + IMAGE_DIRECTORY_ENTRY_RESOURCE;
  dwStart = this->Rva2Foa(this->stFmtPe.lpResBlk->VirtualAddress);
  this->stFmtPe.lpResTbl = (PIMAGE_RESOURCE_DIRECTORY)(this->strBase + dwStart);

  //Base Relocation Table
  this->stFmtPe.lpRlcBlk = this->stFmtPe.lpDatDir + IMAGE_DIRECTORY_ENTRY_BASERELOC;
  dwStart = this->Rva2Foa(this->stFmtPe.lpRlcBlk->VirtualAddress);
  this->stFmtPe.lpRlcTbl = (PIMAGE_BASE_RELOCATION)(this->strBase + dwStart);

  //Debug Directory
  this->stFmtPe.lpDbgBlk = this->stFmtPe.lpDatDir + IMAGE_DIRECTORY_ENTRY_DEBUG;
  dwStart = this->Rva2Foa(this->stFmtPe.lpDbgBlk->VirtualAddress);
  this->stFmtPe.lpDbgTbl = (PIMAGE_DEBUG_DIRECTORY)(this->strBase + dwStart);

  //TLS Directory
  this->stFmtPe.lpTlsBlk = this->stFmtPe.lpDatDir + IMAGE_DIRECTORY_ENTRY_TLS;
  dwStart = this->Rva2Foa(this->stFmtPe.lpTlsBlk->VirtualAddress);
  this->stFmtPe.lpTlsTbl = (PIMAGE_TLS_DIRECTORY)(this->strBase + dwStart);

  //Import Address Table (IAT)
  this->stFmtPe.lpIatBlk = this->stFmtPe.lpDatDir + IMAGE_DIRECTORY_ENTRY_IAT;
  dwStart = this->Rva2Foa(this->stFmtPe.lpIatBlk->VirtualAddress);
  this->stFmtPe.lpIatTbl = (PIMAGE_THUNK_DATA)(this->strBase + dwStart);

  //OK
  *lpdwResult = (DWORD)(&(this->stFmtPe));
  return TRUE;
}

struct SPeFile* AcquirePeFile(void)
{
  g_lpPeFile = NULL;
  this = NULL;
  this = (struct SPeFile*)malloc(sizeof(struct SPeFile));
  if(NULL == this)
  {
    return (struct SPeFile*)NULL;
  }

  ZeroMemory(this, sizeof(struct SPeFile));
  
  this->Open    = PE_Open;
  this->OpenEx  = PE_OpenEx;
  this->MapFile = PE_MapFile;
  this->MapFileView   = PE_MapFileView;
  this->UnMapFileView = PE_UnMapFileView;
  this->Close = PE_Close;
  
  this->Rva2Foa = PE_Rva2Foa;
  this->Foa2Rva = PE_Foa2Rva;
  this->Parse   = PE_Parse;
  
  this->GetMachineString   = PE_GetMachineString;
  this->GetSubsystemString = PE_GetSubsystemString;
  this->GetSectionProperty = PE_GetSectionProperty;
  this->Encrpty = PE_Encrpty;
  this->Decrpty = PE_Decrpty;
  
  g_lpPeFile = this;
  return this;
};

void ReleasePeFile(void)
{
  if(this)
  {
    free(this);
    this = NULL;
    g_lpPeFile = NULL;
  }
}
