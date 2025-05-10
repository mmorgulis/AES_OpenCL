#ifndef SAFE_ALLOCATOR_HPP
#define SAFE_ALLOCATOR_HPP

#include <memory>
#include <iostream>
#include <cstring>
#include <limits>
#include <vector>

namespace crypto {

/**
* @brief Allocator Struct
* Uses global operator new and delete that allocates raw bytes,
* they don't construct object as new and delete
* 
* @param T object type that allocator deal with
*/
template <typename T>
struct safe_allocator {
	// Needed for compatibilty with stl
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

	safe_allocator() = default;

	// Empty copy constructor
	template<typename U>
	constexpr safe_allocator(const safe_allocator<U>&) noexcept {}

	T* allocate(std::size_t n) {
		// Check if n doesn't exceed the maximum type T object
		// allocable in a size_t n
		if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
			throw std::bad_alloc();
		}
		/*
		#ifndef NDEBUG
		std::cout << "Allocated " << n * sizeof(T) << " bytes\n";
		#endif
		*/
		return static_cast<T*>(::operator new(n * sizeof(T)));	
	}

	void deallocate(T* p, std::size_t n) noexcept {
		if (p) {
			// put to zero sensible data
			secure_clean_memory(p, n * sizeof(T)); 
			::operator delete(p);
		}
		/*
		#ifndef NDEBUG
		std::cout << "Deallocated memory\n";
		#endif 
		*/
	}

	// Useful to put object into allocated memory
	// It uses universal reference (&&...) and 
	// parameter pack (...Args)
	template<typename U, typename... Args>
	void construct(U* p, Args&&... args) {
		new(p) U(std::forward<Args>(args)...);
		/*
		#ifndef NDEBUG
		std::cout << "Construct element" << std::endl;
		#endif
		*/
	}

	// Cleaning up object without cleaning up memory
	template<typename U>
	void destroy(U* p) noexcept {
		p->~U();
		/*
		#ifndef NDEBUG
		std::cout << "Destroy element" << std::endl;
		#endif
		*/
	}

	void secure_clean_memory(void* p, size_t num_bytes) {
		// Mem clean, alernatives based on OS are possible
		volatile unsigned char* vp = static_cast<volatile unsigned char*>(p);
		for (size_t i = 0; i < num_bytes; ++i) {
			vp[i] = 0;
		}
		// No compiler optimization
		std::atomic_thread_fence(std::memory_order_seq_cst);
		/*
		#ifndef NDEBUG
		std::cout << "Clean memory" << std::endl;
		const unsigned char* check = reinterpret_cast<const unsigned char*>(p);
		for (size_t i = 0; i < num_bytes; ++i) {
			if (check[i] != 0) {
				break;
			}
		}
		#endif
		*/
	}

};

// Operators to maintain compatibilty with STL
template <class T, class U>
bool operator==(const safe_allocator<T>&, const safe_allocator<U>&) noexcept {
	return true;
}

template <class T, class U>
bool operator!=(const safe_allocator<T>&, const safe_allocator<U>&) noexcept {
	return false;
}

/**
* A safe_vector is consctructed with an std::vector<T>
* and a secure_allocator of type T
*/
template<typename T>
using safe_vector = std::vector<T, safe_allocator<T>>;

}

#endif //SAFE_ALLOCATOR_HPP
