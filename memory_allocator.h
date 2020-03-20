#pragma once

#undef new
#undef delete

class CMemoryAllocator
{
public:
	CMemoryAllocator()
	{
	}
	virtual ~CMemoryAllocator()
	{

	}

	void *operator new(const size_t allocation_length)
	{
		return std::malloc(allocation_length);
	}
	void *operator new[](const size_t allocation_length)
	{
		return std::malloc(allocation_length);
	}

	void operator delete(void *pointer)
	{
		std::free(pointer);
	}
	void operator delete[](void *pointer)
	{
		std::free(pointer);
	}

protected:

};

class CMemoryTest : public CMemoryAllocator
{
public:
};