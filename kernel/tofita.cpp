// The Tofita Kernel
// Copyright (C) 2020-2023 Oleh Petrenko
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

typedef void *EFI_HANDLE;
typedef void VOID;
typedef void function;
#define nullptr ((void*)0)
#define null nullptr

struct UefiPayload_;
struct UefiPayload_ *kernelParams; // Used by Hexa

// Generated by Hexa compiler
#define HEXA_NO_DEFAULT_INCLUDES
#undef char
#undef int
static void printf(const void/*chat*/ *c, ...) {
};
void fflush(void *pipe) {
};
static void* HeapAllocAt(size_t lineNumber, char const* filename, char const* functionName, int8_t x,int8_t u, uint64_t size);
void *malloc(uint64_t bytes) {
	return (void *)HeapAllocAt(1, "unsafe", "unsafe", 1, 1, bytes);
	// TODO return (void *)PhysicalAllocator_$allocateBytes_(((bytes - 1) | 15) + 1);
};
function memcpy(void *dest, const void *src, uint64_t n);
int32_t wcslen(const wchar_t *string_) {
	int32_t i = 0;
	while (string_[i] != '\0')
		i++;
	return i;
};
void free(void *ptr) {
	// TODO
};
typedef void FILE;
#define HEXA_MAIN mainHexa
#define HEXA_NEW(z) malloc(z)

void *memset(void *dest, int32_t e, uint64_t len) {
	uint8_t *d = (uint8_t *)dest;
	for (uint64_t i = 0; i < len; i++, d++) {
		*d = e;
	}
	return dest;
}

function memzero(void *dest, uint64_t len) {
	memset(dest, 0, len);
}

void ___chkstk_ms(){};

void vmemcpy(volatile void *dest, const volatile void *src, volatile uint64_t count) {
	uint8_t *dst8 = (uint8_t *)dest;
	uint8_t *src8 = (uint8_t *)src;

	while (count--) {
		*dst8++ = *src8++;
	}
}

function mouseHandler();
function keyboardHandler();
#define PACKED __attribute__((packed))
#pragma pack(1)
typedef struct {
	uint16_t limit;
	uint64_t offset;
} PACKED Idtr;
#pragma pack()
_Static_assert(sizeof(Idtr) == 10, "IDTR register has to be 80 bits long");
#pragma pack(1)
struct TablePtr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));
typedef struct TablePtr TablePtr;
#define char8_t uint8_t
#pragma pack()
_Static_assert(sizeof(TablePtr) == 10, "sizeof is incorrect");

// STB library
#define STBI_NO_SIMD
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "formats/stb_image/libc.cpp"
#include "formats/stb_image/stb_image.hpp"

#undef free
#undef malloc
#undef realloc
#undef memset
#undef abs
#undef pow
#undef memcpy

#define HEAP_ZERO_MEMORY 0
#define stdout 0
#define HANDLE void*

static void ExitProcess(int32_t x) {
	// Stub
}

static void wprintf(const wchar_t* x, const void* y) {
}

uint64_t kstrlen_(const uint8_t*);
static uint32_t strlen(const char *x) {
	return kstrlen_((const uint8_t *)x);
}

#define macro_serialPrintf(print_, ...) serialPrintf((const wchar_t *)print_->_->utf16_(print_), ## __VA_ARGS__)

#define HEAP_C 2097152
static uint8_t heapInitial[HEAP_C] = {0};
static uint8_t* heap = nullptr;
static uint64_t heapOffset = 0;
static void* HeapAllocAt(size_t lineNumber, char const* filename, char const* functionName, int8_t x,int8_t u, uint64_t size) {
	// serialPrintf_(L"HeapAlloc [%s:%d] %d", /*filename,*/ functionName, lineNumber, size);
	// size = ((size - 1) | 7) + 1; // Align by 8
	size = ((size - 1) | 15) + 1; // Align by 16
	if (size < 16) size = 16;
	heapOffset += 16;
	heapOffset += size;
	if (heapOffset >= HEAP_C) {
		//serialPrint_(L"Heap overflow\n");
		// TODO heap zero fill?
		heap = (uint8_t*)PhysicalAllocator_$allocateBytes_(HEAP_C);
		heapOffset = 16 + size;
		while (heap == nullptr) {
			//serialPrint_(L"Heap total overflow\n");
		};
	}
	// TODO FILL WITH ZEROS RIGHT HERE!
	return &heap[heapOffset - size];
}
#define HeapAlloc(a, b, c) HeapAllocAt(__LINE__, __FILE__, __func__, a, b, c)
#define TRACER() serialPrintf_(L"[%s:%d]\n", __func__, __LINE__)

#define HEXA_UNREACHABLE(where) {}

#define WriteFile(x,y,z,t,r) {}
#define FlushFileBuffers(x) {}
#define GetStdHandle(x) 0

#define GetProcessHeap() 0
#define DWORD uint32_t
#define HEXA_NO_DEFAULT_INCLUDES

#define ArrayByValue_$uint8_t$Any_ void
#define ArrayByValue_$uint32_t$Any_ void
#define ArrayByValue_$IdtEntry_$Any_ void
#define Virtual_$Any_ Any_
#define Physical_$Any_ Any_

#include "../kernel-diff/kernel.c"

function kernelMain(/*const TODO*/void *params) {
	kernelParams = (UefiPayload_ *)params;
	heap = &heapInitial[0];
	HEXA_MAIN(0, nullptr);
}
