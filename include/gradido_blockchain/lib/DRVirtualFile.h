#ifndef __GRADIDO_BLOCKCHAIN_LIB_VIRTUAL_FILE_H
#define __GRADIDO_BLOCKCHAIN_LIB_VIRTUAL_FILE_H

#include "gradido_blockchain/export.h"

#include <memory>
#include <string>
#include <list>

#define VIRTUAL_FILE_HEAD "DRVirtFile"

/*!
*
* \author: einhornimmond
*
* \date: 03.05.17
*
* \desc: A class for a virtual file, which handle adding various parts
* and saving everything to storage
*/
/*
namespace DRFilePart {
	enum Type {
		FILE_PART_BIN = 0,
		FILE_PART_INT = 1,
		FILE_PART_FLOAT = 2,
		FILE_PART_STRING = 3,
		FILE_PART_SHORT = 4,
		FILE_PART_LONG = 5,
		FILE_PART_BYTE = 6,
		FILE_PART_ARRAY_FLOAT = 7,
		FILE_PART_ARRAY_INT = 8,
		FILE_PART_UNKNOWN = 255

	};
	class FilePart {
	public:
		virtual ~FilePart() {};
		virtual size_t size() const = 0;
		virtual size_t count() const = 0;
		virtual const void* data() const = 0;
		virtual Type type() const = 0;

	};
	class Binary : public FilePart {
	public: 
		Binary(void* _data, size_t _size, bool copyData = false, bool freeMemory = true);
		Binary(bool freeMemory = true)
			: mData(NULL), mSize(0), mFreeMemory(freeMemory) {}
		virtual ~Binary() {
			if (mFreeMemory) {
				free(mData);
				mSize = 0;
				mData = NULL;
			}
		}

		void alloc(size_t size) {
			mData = (void*)malloc(size);
			mSize = size;
		}
		virtual size_t size() const { return mSize; }
		virtual size_t count() const { return 1; }
		virtual const void* data() const { return mData; }
		virtual void* data() { return mData; }
		virtual Type type() const { return FILE_PART_BIN;}
		inline void setFreeMemory(bool freeMemory) { mFreeMemory = freeMemory; }
	protected:
		void* mData;
		size_t mSize;
	private: 
		bool   mFreeMemory;
	};

	template <class T>
	class SimpleTypes : public FilePart
	{
	public:
		SimpleTypes(T value) : mValue(value) {}
		virtual size_t size() const { return sizeof(T); }
		virtual size_t count() const { return 1; }
		virtual const void* data() const { return &mValue; }

		inline operator T() { return mValue; }
		inline T value() { return mValue; }
		
		virtual Type type() const {
			if (std::is_same<T, int32_t>::value) return FILE_PART_INT;
			else if (std::is_same<T, int64_t>::value) return FILE_PART_LONG;
			else if (std::is_same<T, float>::value) return FILE_PART_FLOAT;
			else if (std::is_same<T, int16_t>::value) return FILE_PART_SHORT;
			else if (std::is_same<T, uint8_t>::value) return FILE_PART_BYTE;
			return FILE_PART_UNKNOWN;
		}
	protected:
		T mValue;
	};
	typedef SimpleTypes<int32_t> Integer;
	typedef SimpleTypes<int64_t> Long;
	typedef SimpleTypes<float> Float;
	typedef SimpleTypes<double> Double;
	typedef SimpleTypes<int16_t> Short;
	typedef SimpleTypes<uint8_t> Byte;

	template <class T>
	class ArrayTypes : public FilePart
	{
	public: 
		ArrayTypes(T* value, size_t _count, bool copyData = false, bool freeMemory = true) :
			mCount(_count), mFreeMemory(freeMemory) {
			if (copyData) {
				mValue = new T[_count];
				memcpy(mValue, value, _count * sizeof(T));
			}
			else {
				mValue = value;
			}
		}
		virtual ~ArrayTypes() {
			if (mFreeMemory) {
				DR_SAVE_DELETE_ARRAY(mValue);
				mCount = 0;
			}
			mCount = 0;
			mValue = NULL;
		}
		virtual size_t size() const { return sizeof(T); }
		virtual size_t count() const { return mCount; }
		virtual const void* data() const { return mValue; }
		virtual Type type() const {
			if (std::is_same<T, int32_t>::value) return FILE_PART_ARRAY_INT;
			else if (std::is_same<T, float>::value) return FILE_PART_ARRAY_FLOAT;
			return FILE_PART_UNKNOWN;
		}
	protected:
		T* mValue;
		size_t mCount;
	private:
		bool mFreeMemory;
	};

	typedef ArrayTypes<int32_t> IntegerArray;
	typedef ArrayTypes<float> FloatArray;

	class String : public FilePart {
	public:
		String(std::string& zeichenkette) : mString(zeichenkette) {}
		String(const char* zeichen) : mString(zeichen) {}
		String(const char* charBuffer, size_t count);
		virtual size_t size() const { return sizeof(char); }
		virtual size_t count() const { return mString.size(); }
		virtual const void* data() const { return mString.data(); }
		virtual Type type() const { return FILE_PART_STRING; }
	protected:
		std::string mString;
	};

}

class GRADIDOBLOCKCHAIN_EXPORT DRVirtualFile
{
public:
	virtual bool saveToFile(const char* filename) = 0;
	virtual bool readFromFile(const char* filename) = 0;

};

class GRADIDOBLOCKCHAIN_EXPORT DRVirtualBinaryFile : public DRVirtualFile
{
public:
	DRVirtualBinaryFile(bool freeMemory = true) :
		mBinSize(0), mBinData(NULL), mFreeMemory(freeMemory) {}
	DRVirtualBinaryFile(void* data, size_t size, bool copyData = true, bool freeMemory = true);
	~DRVirtualBinaryFile() {
		if (mFreeMemory) free(mBinData);
	}

	virtual bool saveToFile(const char* filename);
	virtual bool readFromFile(const char* filename);

	const size_t getSize() const { return mBinSize; }
	void* getData() { return mBinData;  }

protected:
	size_t		mBinSize;
	void*		mBinData;
	bool		mFreeMemory;
};

/*!
 * target is getting a easy to use solution, to put different kinds of datatypes into binary file
 * 
 *
 */
/*
class GRADIDOBLOCKCHAIN_EXPORT DRVirtualCustomFile : public DRVirtualFile
{
public:
	DRVirtualCustomFile(bool freeMemory = true) :
		mFreeMemory(freeMemory) {}
	~DRVirtualCustomFile();

	void write(DRFilePart::FilePart* part);
	// write easy access operators
	inline DRVirtualCustomFile& operator << (int32_t value) { write(new DRFilePart::Integer(value)); return *this; }
	inline DRVirtualCustomFile& operator << (uint32_t value) { write(new DRFilePart::Integer(value)); return *this; }
	inline DRVirtualCustomFile& operator << (float value) {write(new DRFilePart::Float(value)); return *this; }
	inline DRVirtualCustomFile& operator << (int64_t value) {write(new DRFilePart::Long(value)); return *this; }
	inline DRVirtualCustomFile& operator << (uint64_t value) {write(new DRFilePart::Long(value)); return *this; }
	inline DRVirtualCustomFile& operator << (int16_t value) {write(new DRFilePart::Short(value)); return *this; }
	inline DRVirtualCustomFile& operator << (uint16_t value) {write(new DRFilePart::Short(value)); return *this; }
	inline DRVirtualCustomFile& operator << (uint8_t value) {write(new DRFilePart::Byte(value)); return *this; }
	inline DRVirtualCustomFile& operator << (double value) {write(new DRFilePart::Double(value)); return *this; }
	inline DRVirtualCustomFile& operator << (std::string str) { write(new DRFilePart::String(str)); return *this; }
	inline DRVirtualCustomFile& operator << (const char* str) { write(new DRFilePart::String(str)); return *this; }
	// 
	DRFilePart::FilePart* read();
	// read operator for fast read access
	inline operator int32_t() { return dynamic_cast<DRFilePart::Integer*>(read())->value(); };
	inline operator uint32_t() { return dynamic_cast<DRFilePart::Integer*>(read())->value(); };
	inline operator float() { return dynamic_cast<DRFilePart::Float*>(read())->value(); };
	inline operator int64_t() { return dynamic_cast<DRFilePart::Long*>(read())->value(); };
	inline operator uint64_t() { return dynamic_cast<DRFilePart::Long*>(read())->value(); };
	inline operator int16_t() { return dynamic_cast<DRFilePart::Short*>(read())->value(); };
	inline operator uint16_t() { return dynamic_cast<DRFilePart::Short*>(read())->value(); };
	inline operator uint8_t() { return dynamic_cast<DRFilePart::Byte*>(read())->value(); };
	inline operator double() { return dynamic_cast<DRFilePart::Double*>(read())->value(); };
	inline operator std::string() { return std::string((const char*)dynamic_cast<DRFilePart::String*>(read())->data()); };
	//inline operator const char*() { return (const char*)dynamic_cast<DRFilePart::Integer*>(read())->value(); };

	virtual bool saveToFile(const char* filename);
	virtual bool readFromFile(const char* filename);

protected:
	bool							mFreeMemory;
	typedef std::list<DRFilePart::FilePart*> FilePartList;
	typedef FilePartList::iterator FilePartListIterator;
	FilePartList mFileParts;
	FilePartList mFilePartsForDeleting;
	
};

*/

#endif // __GRADIDO_BLOCKCHAIN_LIB_VIRTUAL_FILE_H
