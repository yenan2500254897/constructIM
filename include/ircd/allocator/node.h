#pragma once
#define HAVE_IRCD_ALLOCATOR_NODE_H

namespace ircd::allocator
{
	template<class T>
	struct node;
}

/// Allows elements of an STL container to be manually handled by the user.
///
/// C library containers usually allow the user to manually construct a node
/// and then insert it and remove it from the container. With STL containers
/// we can use devices like allocator::fixed, but what if we don't want to have
/// a bound on the allocator's size either at compile time or at runtime? What
/// if we simply want to manually handle the container's elements, like on the
/// stack, and in different frames, and then manipulate the container -- or
/// even better and safer: have the elements add and remove themselves while
/// storing the container's node data too?
///
/// This device helps the user achieve that by simply providing a variable
/// set by the user indicating where the 'next' block of memory is when the
/// container requests it. Whether the container is requesting memory which
/// should be fulfilled by that 'next' block must be ensured and asserted by
/// the user, but this is likely the case.
///
template<class T>
struct ircd::allocator::node
{
	struct allocator;
	struct scope;

	T *next {nullptr};

	node() = default;
};

/// The container will use provided node when it calls for an allocation during
/// the lifetime of this object. Construct this object ahead of the insertion.
template<class T>
struct ircd::allocator::node<T>::scope
{
	node<T> *state;

	template<class C,
	         class N>
	scope(C &container, N &node);
	scope(const scope &) = delete;
	~scope() noexcept;
};

/// The actual template passed to containers for using the allocator.
///
/// See the notes for ircd::allocator::fixed::allocator for details.
///
template<class T>
struct ircd::allocator::node<T>::allocator
{
	using value_type         = T;
	using pointer            = T *;
	using const_pointer      = const T *;
	using reference          = T &;
	using const_reference    = const T &;
	using size_type          = std::size_t;
	using difference_type    = std::ptrdiff_t;

	node *s;

  public:
	template<class U> struct rebind
	{
		using other = typename node<U>::allocator;
	};

	size_type max_size() const                   { return std::numeric_limits<size_t>::max();      }
	auto address(reference x) const              { return &x;                                      }
	auto address(const_reference x) const        { return &x;                                      }

	template<class U, class... args>
	void construct(U *p, args&&... a) noexcept
	{
		new (p) U(std::forward<args>(a)...);
	}

	void construct(pointer p, const_reference val)
	{
		new (p) T(val);
	}

	pointer
	__attribute__((returns_nonnull, warn_unused_result))
	allocate(const size_type &n, const const_pointer &hint = nullptr)
	{
		assert(n == 1);
		assert(hint == nullptr);
		assert(s->next != nullptr);
		return s->next;
	}

	void deallocate(const pointer &p, const size_type &n)
	{
		assert(n == 1);
	}

	template<class U>
	allocator(const typename node<U>::allocator &s) noexcept
	:s{reinterpret_cast<node *>(s.s)}
	{
	}

	template<class U>
	allocator(const U &s) noexcept
	:s{reinterpret_cast<node *>(s.s)}
	{
	}

	allocator(node &s) noexcept
	:s{&s}
	{
	}

	allocator() = default;
	allocator(allocator &&) noexcept = default;
	allocator(const allocator &) = default;

	friend bool operator==(const allocator &a, const allocator &b)
	{
		return &a == &b;
	}

	friend bool operator!=(const allocator &a, const allocator &b)
	{
		return &a == &b;
	}
};

template<class T>
template<class C,
         class N>
inline
ircd::allocator::node<T>::scope::scope(C &container,
                                       N &node)
:state
{
	container.get_allocator().s
}
{
	using value_type = typename C::value_type;

	assert(state);
	assert(!state->next);
	state->next = reinterpret_cast<value_type *>(&node);
}

template<class T>
inline
ircd::allocator::node<T>::scope::~scope()
noexcept
{
	assert(state);
	assert(state->next);
	state->next = nullptr;
}