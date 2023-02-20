#include <bits/stdc++.h>
/*
#include <Classes.h>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
*/
#include <windows.h>
using namespace std;

const int MAXSIZE = 1e5;

typedef unsigned __int32 tjs_uint32;
typedef unsigned __int16 tjs_uint16;
typedef unsigned __int8 tjs_uint8;
typedef int tjs_int32;
typedef short tjs_int16;
typedef unsigned int uint;
typedef unsigned int tjs_uint;
typedef wchar_t tjs_char;
typedef int tjs_int;

struct tTVPPrerenderedCharacterItem
{
	tjs_uint32 Offset;
	tjs_uint16 Width;
	tjs_uint16 Height;
	tjs_int16 OriginX;
	tjs_int16 OriginY;
	tjs_int16 IncX;
	tjs_int16 IncY;
	tjs_int16 Inc;
	tjs_uint16 Reserved;
}DC[(int)5e4];

char * Image;
int filelen;
uint IndexCount;
tjs_char * chindex;
tTVPPrerenderedCharacterItem * Index;
string FILETYP;
bool Antialiased;
/*
TCanvas *TargetCanvas;
TCanvas *PrivTargetCanvas;

DWORD WINAPI (*procGetFontUnicodeRanges)(HDC, LPGLYPHSET);
	// GetFontUnicodeRanges function ( only supporter on Win2000 or later )

DWORD WINAPI (*procGetGlyphIndicesW)(HDC, LPCWSTR, int, LPWORD, DWORD);
	// GetGlyphIndicesW

DWORD WINAPI (*procGetGlyphOutlineW)(HDC, UINT, UINT, LPGLYPHMETRICS, DWORD, LPVOID, CONST MAT2 *);
	// GetGlyphOutlineW function ( WinNT or Win9x ( but 95 is not supported ) )

BOOL  WINAPI (*procTextOutW)(HDC, int, int, LPCWSTR, int);
	// TextOutW

BOOL  APIENTRY (*procGetTextExtentPoint32W)(HDC, LPCWSTR, int, LPSIZE);
	// GetTextExtentPoint32W
*/
tTVPPrerenderedCharacterItem * Find(tjs_char ch)
{
	// search through chindex
	tjs_uint s = 0;
	tjs_uint e = IndexCount;
	tjs_char *chindex = chindex;
	while(true)
	{
		tjs_int d = e-s;
		if(d <= 1)
		{
			if(chindex[s] == ch)
				return Index + s;
			else
				return NULL;
		}
		tjs_uint m = s + (d>>1);
		cout << (int)chindex[m] << ' ' << (int)ch << endl;
		if(chindex[m] > ch) e = m; else s = m;
	}
}

void Retrieve(tTVPPrerenderedCharacterItem * item,
	tjs_uint8 *buffer, tjs_int bufferpitch)
{
	// retrieve font data and store to buffer
	// bufferpitch must be larger then or equal to item->Width
	if(item->Width == 0 || item->Height == 0) return;

	tjs_uint8 *ptr = item->Offset + (tjs_uint8 *)Image;
	tjs_uint8 *dest = buffer;
	tjs_uint8 *destlim = dest + item->Width * item->Height;

	// expand compressed character bitmap data
	// version 1+ decompressor
	while(dest < destlim)
	{
		if(*ptr >= 0x41) // running
		{
			tjs_int len = *ptr - 0x40;
			ptr++;
			tjs_uint8 last = dest[-1];
			while(len--) *(dest++) = (tjs_uint8)last;
		}
		else
		{
			*(dest++) = *(ptr++);
		}
		}

	// expand to each pitch
	ptr = destlim - item->Width;
	dest = buffer + bufferpitch * item->Height - bufferpitch;
	while(buffer <= dest)
	{
		if(dest != ptr)
			memmove(dest, ptr, item->Width);
		dest -= bufferpitch;
		ptr -= item->Width;
	}
}

int main()
{
	Image = NULL;
	//cin >> FILETYP;
	FILETYP = "1001";
	Antialiased = FILETYP[0] - '0';
	ifstream File("in.tft", /*std::ifstream::binary*/ios::in);
	File.seekg(0, File.end);
	filelen = File.tellg();
	File.seekg (0, File.beg);
	//cout << filelen << endl;
	/*
	char ch[10];
	File.read(ch, sizeof(ch));
	for (int i = 0; i <= 9; i++)
		cout << ch[i];
	cout << endl;*/
	//HMODULE gdi32dll = GetModuleHandle("gdi32.dll");
	/*
	*(void**)&procGetFontUnicodeRanges =
		GetProcAddress(gdi32dll, "GetFontUnicodeRanges");
	*(void**)&procGetGlyphIndicesW =
		GetProcAddress(gdi32dll, "GetGlyphIndicesW");
	*(void**)&procGetGlyphOutlineW =
		(void**)GetProcAddress(gdi32dll, "GetGlyphOutlineW");
	*(void**)&procTextOutW =
		GetProcAddress(gdi32dll, "TextOutW");
	*(void**)&procGetTextExtentPoint32W =
		GetProcAddress(gdi32dll, "GetTextExtentPoint32W");
	*/
	Image = new char[filelen];
	File.read(Image, filelen);
	//cout << File.gcount() << endl;
	//cout << (int)*(Image + 22) << endl;
	IndexCount = *(unsigned __int32 *)(Image + 24);
	chindex = (tjs_char*)(Image + *(const tjs_uint32*)(Image + 28));
	Index = (tTVPPrerenderedCharacterItem*)(Image + *(const tjs_uint32*)(Image + 32));
	cout << IndexCount << endl;
	//wchar_t ch;
	//scanf("%C", &ch);
	//tTVPPrerenderedCharacterItem * test = Find(59016);
	//cout << test->Width << ' ' << test->Height << endl;
	
	for (int j = 0; j < 10; j++)
	{
		ofstream OUTFILE;
		string fileadd = "D:\\fontfill\\extract\\in\\" + to_string((int)((short)(chindex[j])));
		OUTFILE.open(fileadd, ios::out);
		tTVPPrerenderedCharacterItem * item = Index + j * sizeof(tTVPPrerenderedCharacterItem);
		if(item->Width == 0 || item->Height == 0) continue;
		int size = item->Width * item->Height;
		tjs_uint8 *ptr = item->Offset + (tjs_uint8 *)Image;
		tjs_uint8 * buffer;
		buffer = new tjs_uint8[size];
		tjs_uint8 *dest = buffer;
		tjs_uint8 *destlim = dest + size;
		while(dest < destlim)
		{
			if(*ptr >= 0x41) // running
			{
				tjs_int len = *ptr - 0x40;
				ptr++;
				tjs_uint8 last = dest[-1];
				while(len--) *(dest++) = (tjs_uint8)last;
			}
			else
			{
				*(dest++) = *(ptr++);
			}
		}
		OUTFILE.write((char*)dest, size);
		OUTFILE.close();
	}
	int cnt = 0;
	File.close();
	delete[] Image;
	//while (cnt < 1000) cout << (getchar()) << endl, cnt++;
	
	return 0;
}
