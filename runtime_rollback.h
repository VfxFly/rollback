#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <type_traits>
#include <vector>

typedef void* rollback_state;

/// <summary>
/// Rollback data identifier for detemine available value in buffer
/// </summary>
enum class rollback_type : uint32_t
{
	max
};

template <rollback_type id>
struct rollback_identifier;

/// <summary>
/// Defination of traits for rollable back value, data source/target and on-restore event decl.
/// </summary>
#define ENABLE_ROLLBACK(id, ty, d, e, ...) \
	template<> \
	struct rollback_identifier<id> { \
		using decl = ty(*)(__VA_ARGS__); \
		using type = ty; \
		\
		inline static void* data = d; \
		inline static void* event = e; \
	}

namespace utils 
{
	inline size_t rollback_states_capacity = 128, total_rollback_states = 0;
	inline std::vector<std::pair<rollback_type, rollback_state>> rollback_states;
	
	auto can_rollback() -> bool
	{
		return !rollback_states.empty();
	}
	
	/// <summary>
	/// Capture current value to buffer.
	/// </summary>
	/// <param name="id"> - rollback_type:: enum entry</param>
	/// <param name="operation"> - value modification lambda</param>
	/// <returns></returns>
	template <rollback_type id>
	__forceinline auto capture_rollback_state(std::function<void(void)> operation) -> void
	{
		if (id >= rollback_type::max)
			return;
	
		std::vector<std::pair<rollback_type, rollback_state>>& states = rollback_states;
	
		if (rollback_states.size() + 1 > rollback_states_capacity) {
			free(states.begin()->second);
			states.erase(states.begin());
		}
	
		rollback_state data = static_cast<rollback_state>(rollback_identifier<id>::data);
	
		states.push_back({ id, (rollback_state)::calloc(1, sizeof(typename rollback_identifier<id>::type)) });
		memcpy(states.back().second, data, sizeof(typename rollback_identifier<id>::type));
	
		operation();
	}
	
	/// <summary>
	/// Rollback value by id.
	/// </summary>
	/// <param name="a"> - arguments list for on-restore event</param>
	/// <returns></returns>
	template <rollback_type id, typename... args>
	__forceinline auto rollback(args&&... a) -> bool
	{
		if (!can_rollback())
			return false;
	
		std::vector<std::pair<rollback_type, rollback_state>>::iterator remove;
		for (int i = rollback_states.size() - 1; i >= 0; i--) {
			const auto& it = rollback_states.begin() + i;
			if (it->first != id)
				continue;
	
			memcpy(rollback_identifier<id>::data, it->second, sizeof(typename rollback_identifier<id>::type));
			if (rollback_identifier<id>::event != nullptr)
				reinterpret_cast<typename rollback_identifier<id>::decl>(rollback_identifier<id>::event)(std::forward<args>(a)...);
			remove = it;
			break;
		}
	
		if (remove._Ptr != nullptr)
			rollback_states.erase(remove);
	
		return true;
	}
}