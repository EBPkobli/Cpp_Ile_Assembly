//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop

//---
#define PAGE_EXECUTE_READWRITE 0x40
#define DLL_BAGLANDI 0x1
#define TUS_SPACE 0x20
//---

//--------

bool Kanca(void *InjectBolge,void *Fonksiyon,int byteBoyutu)
{
        if(byteBoyutu<5)return false; // JMP + BOLGE 5 BYTE en az 5 byte
        DWORD vProtect;
        VirtualProtect(InjectBolge,byteBoyutu,PAGE_EXECUTE_READWRITE,&vProtect);
        memset(InjectBolge,0x90,byteBoyutu/*(NOP)*/);
        DWORD bagliAdres = ((DWORD)Fonksiyon-(DWORD)InjectBolge)-5/*(jmp boyutu)*/;
        *(BYTE*) InjectBolge = 0xE9; /*(jmp)*/
        *(DWORD*)((DWORD)InjectBolge+1)=bagliAdres;
        DWORD temp;
        VirtualProtect(InjectBolge,byteBoyutu,vProtect,&temp);
        return true;
}
//--------

//      Kanca Fonksiyonumuz
DWORD jmpDonus;
void __declspec(naked) Fonksiyon()
{
        __asm
        {
                add [ebx+04],edi //normalde sub [ebx+04],edi 'idi
                mov eax,edi
                jmp dword ptr[jmpDonus]
        }
}
//

DWORD WINAPI AnaThread(LPVOID PARAM)
{
        int K_Uzunluk = 5;
        DWORD K_Adres = 0x429D1F;   //AssaultCube'da can azaltan yer
        jmpDonus = K_Adres + K_Uzunluk;
        bool KancaKontrol= Kanca((void *)K_Adres,Fonksiyon,K_Uzunluk);

        while(true)
        {
            if(GetAsyncKeyState(TUS_SPACE))break;
            Sleep(50);
        }

        FreeLibraryAndExitThread(PARAM,0); 

        return 0;
}

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
        switch(reason)
        {
                case DLL_BAGLANDI:
                 CreateThread(NULL,0,AnaThread,hinst,0,0); //Thread olu�turduk
                break;
        }
        return 1;
}
//---------------------------------------------------------------------------
 