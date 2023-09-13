#pragma once
#define HAVE_IRCD_ALLOCATOR_STATE_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/*
类似环状数组，在其中找到连续的长度为n的区域，标记用来存数据（首尾不相接）
*/
namespace ircd::allocator
{
	struct state;
}

struct ircd::allocator::state
{
	using word_t                                 = unsigned long long;
	using size_type                              = std::size_t;

	size_t size                                  { 0                                               };
	word_t *avail                                { nullptr                                         };
	size_t last                                  { 0                                               };

	static uint byte(const uint &i)              { return i / (sizeof(word_t) * 8);                }
	static uint bit(const uint &i)               { return i % (sizeof(word_t) * 8);                }
	static word_t mask(const uint &pos)          { return word_t(1) << bit(pos);                   }

	bool test(const uint &pos) const             { return avail[byte(pos)] & mask(pos);            }
	void bts(const uint &pos)                    { avail[byte(pos)] |= mask(pos);                  }
	void btc(const uint &pos)                    { avail[byte(pos)] &= ~mask(pos);                 }
	uint next(const size_t &n) const;

  public:
	bool available(const size_t &n = 1) const;
	void deallocate(const uint &p, const size_t &n);
	uint allocate(std::nothrow_t, const size_t &n, const uint &hint = -1);
	uint allocate(const size_t &n, const uint &hint = -1);

	state(const size_t &size = 0,
	      word_t *const &avail = nullptr)
	:size{size}
	,avail{avail}
	,last{0}
	{}
};