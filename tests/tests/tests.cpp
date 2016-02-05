// tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;

enum TTileType : ui16
{
	Unknown, Graund, Water, Count
};
#pragma pack(push,1)
struct TParams
{
	i32 w = 0;
	i32 h = 0;
	TTileType DefTile = TTileType::Unknown;
};

struct TCoord2Type
{
	i16 x = 0;
	i16 y = 0;
	TTileType TileType = TTileType::Unknown;
	TCoord2Type() = default;
	TCoord2Type(i16 x, i16 y, TTileType TileType) : x(x), y(y), TileType(TileType) {}
};
#pragma pack(pop)

void func2();

int main()
{
	TParams p1, p2;
	p1.DefTile = TTileType::Graund;
	p1.h = 32;
	p1.w = 16;
	vector<TCoord2Type> c1 = { TCoord2Type{1,2, TTileType::Water},
		TCoord2Type{2,2, TTileType::Water},
		{2,3, TTileType::Graund}
	};

	FILE *f = 0;
	errno_t er = fopen_s(&f, "file", "wb");
	assert(er == 0);
	assert(f);
	char prefix[8];
	memset(prefix, 0, 8);
	//memcpy_s(prefix,8, "prefix", strlen("prefix"));
	strcpy_s(prefix, "prefix");
	// write prefix
	fwrite(prefix, 1, 8, f);
	// write header data (TParams)
	fwrite(&p1, sizeof(p1), 1, f);
	// write array count
	assert(c1.size() < _I16_MAX);
	i16 sz = i16(c1.size());
	fwrite(&sz, sizeof(sz), 1, f);
	// write array
	for (TCoord2Type& i : c1)
	{
		fwrite(&i, sizeof(i), 1, f);
	}
	fclose(f);
	f = 0;

	// test

	er = fopen_s(&f, "file", "r");
	assert(er == 0);
	// read prefix
	char prefix2[8];
	fread(prefix2, 1, 8, f);
	assert(0 == memcmp(prefix, prefix2, 8) );
	// read header
	fread(&p2, sizeof(p2), 1, f);
	assert(0 == memcmp(&p1, &p2, sizeof(TParams)));
	// read array size
	i16 sz2;
	fread(&sz2, sizeof(sz2), 1, f);
	vector<TCoord2Type> c2(sz2);
	for (int i = 0; i < sz2; ++i)
	{
		fread(&c2[i], sizeof(c2[0]), 1, f);
	}
	assert(0 == memcmp(c1.data(), c2.data(), sz2));
	fclose(f);
	f = 0;
	
	func2();
	printf("All tests passed.");
	//getchar();
    return 0;
}

#define PRINT(X) << #X << "=" << X << endl

#pragma pack(push,1)
const int pref_len = 8;
struct TData
{
	struct THeader
	{
		char prefix[pref_len] = { 0 };
		i32 w = 0;
		i32 h = 0;
		i32 DefTileType = 0;
		i32 CoordsCount = 0;
		string print()
		{
			stringstream ss;
			ss 
				PRINT(prefix)
				PRINT(w) PRINT(h) PRINT(DefTileType) PRINT(CoordsCount)
			;
			return ss.str();
		}
	}Header;
	struct TCoords
	{
		i16 x = 0;
		i16 y = 0;
		i16 TileType = 0;
	}Coords[0];
	string print();
};
#pragma pack(pop)

string TData::print()
{
	stringstream ss;
	ss << "TData{ ";
	ss << Header.print();

	for (int i = 0; i < Header.CoordsCount; ++i)
	{
		ss	<< "\tCoords[" << i << "].x="  << Coords[i].x << endl
			<< "\tCoords[" << i << "].y="   << Coords[i].y << endl
			<< "\tCoords[" << i << "].TileType=" << Coords[i].TileType << endl << endl;
	}
	ss << " }";
	return ss.str();
}

struct TFile
{
	FILE* f = 0;
	TFile() = default;
	TFile(const TFile&) = delete;
	TFile& operator=(const TFile&) = delete;
	TFile(FILE* f) : f(f) {}
	~TFile() { if (f) fclose(f); f = 0; }
	operator FILE*() { return f; }
	errno_t Open(const char *FileName, const char* Flags)
	{
		errno_t er = fopen_s(&f, FileName, Flags);
		return er;
	}
	void Close()
	{
		if (f)
		{
			fclose(f);
			f = 0;
		}
	}
	void Write(void* buf, int elem_size, int elem_count)
	{
		int ret = fwrite(buf, elem_size, elem_count, f);
		assert(1 == ret);
	}
};

void func2()
{
	{
		TFile f;
		assert(0 == f.Open("file2", "w"));
		
		const int CoordsCount = 4;
		int data1_len = sizeof(TData::THeader) + sizeof(TData::TCoords) * CoordsCount;
		TData *data1 = (TData*)malloc(data1_len);
		unique_ptr<void, void(*)(void*)> HeaderOwner( (void*)data1, free);
		
		strcpy_s(data1->Header.prefix, pref_len, "prefix");
		data1->Header.w = 24;
		data1->Header.h = 36;
		data1->Header.DefTileType = TTileType::Graund;
		data1->Header.CoordsCount = CoordsCount;
		for (int i = 0; i < CoordsCount; ++i)
		{
			data1->Coords[i].x = i + 1;
			data1->Coords[i].y = i + 2;
			data1->Coords[i].TileType = TTileType::Water;
		}

		f.Write(data1, data1_len, 1);
		f.Close();


		assert(0 == f.Open("file2", "r"));
		char buf[sizeof(TData::THeader)] = { 0 };
		int ret = fread_s(buf, sizeof(buf), sizeof(TData::THeader), 1, f.f);
		assert(1 == ret);

		int data2_len = sizeof(TData::Header) + sizeof(TData::TCoords) * ((TData::THeader*)buf)->CoordsCount;
		
		assert(data1_len == data2_len);
		
		TData *data2 = (TData*)malloc(data2_len);
		assert(data2);
		unique_ptr<void, void(*)(void*)> HeaderOwner2((void*)data2, free);

		assert(0 == memcpy_s(data2, data2_len, buf, sizeof(TData::THeader)) );

		int left = data2_len - sizeof(TData::THeader);
		assert(left >= 0);
		ret = fread_s(((char*)data2) + sizeof(TData::THeader), left, sizeof(TData::TCoords), data2->Header.CoordsCount, f.f);
		assert(data2->Header.CoordsCount == ret);
		
		assert(0 == memcmp(data1, data2, data1_len));

		f.Close();
	}	
}