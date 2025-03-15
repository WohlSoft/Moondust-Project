#pragma once
#include <stdio.h>
#include "types.h"

class FileClass
{
	FILE* f;
	bool LittleEndian, own;
	int filePos;

	size_t _RawRead(void* buffer, size_t size)
	{
		size_t x = fread(buffer, 1, size, f);
		filePos += x;
		return x;
	}

	size_t _RawWrite(const void* buffer, size_t size)
	{
		size_t x = fwrite(buffer, 1, size, f);
		filePos += x;
		return x;
	}

public:
	FileClass(const char* file, const char* mode) : LittleEndian(true), own(true), filePos(0)
	{
		f = fopen(file, mode);
	}
	FileClass(FILE* inf) : f(inf), LittleEndian(true), own(false), filePos(0) { }
	~FileClass()
	{
		if (f && own) fclose(f);
	}

	void SetLittleEndian() { LittleEndian = true; }
	void SetBigEndian() { LittleEndian = false; }

	FILE* get_ptr() { return f; }
	bool openerror() { return f == NULL; }

	dword_t ReadDword()
	{
		dword_t value;
		_RawRead(&value, sizeof(dword_t));
		return LittleEndian ? le_dword(value) : be_dword(value);
	}

	void WriteDword(dword_t value)
	{
		value = LittleEndian ? le_dword(value) : be_dword(value);
		_RawWrite(&value, sizeof(dword_t));
	}

	word_t ReadWord()
	{
		word_t value;
		_RawRead(&value, sizeof(word_t));
		return LittleEndian ? le_word(value) : be_word(value);
	}

	void WriteWord(word_t value)
	{
		value = LittleEndian ? le_word(value) : be_word(value);
		_RawWrite(&value, sizeof(word_t));
	}

	hword_t ReadHword()
	{
		hword_t value;
		_RawRead(&value, sizeof(hword_t));
		return LittleEndian ? le_hword(value) : be_hword(value);
	}

	void WriteHword(hword_t value)
	{
		value = LittleEndian ? le_hword(value) : be_hword(value);
		_RawWrite(&value, sizeof(hword_t));
	}

	byte_t ReadByte()
	{
		byte_t value;
		_RawRead(&value, sizeof(byte_t));
		return value;
	}

	void WriteByte(byte_t value)
	{
		_RawWrite(&value, sizeof(byte_t));
	}
	
	bool ReadRaw(void* buffer, size_t size) { return _RawRead(buffer, size) == size; }
	bool WriteRaw(const void* buffer, size_t size) { return _RawWrite(buffer, size) == size; }

	void Seek(int pos, int mode) { fseek(f, pos, mode); }
	int Tell() { return filePos /*ftell(f)*/; }
	void Flush() { fflush(f); }
};
