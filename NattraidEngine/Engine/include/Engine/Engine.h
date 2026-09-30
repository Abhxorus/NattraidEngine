#pragma once

#include "Prerequisites.h"

class ENGINE_API
	Engine final {
public:
	Engine() noexcept;
	~Engine() noexcept;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;

	bool Initialize(
		void* nativeWindow,
		std::uint32_t width,
		std::uint32_t height
	) noexcept;

	void Render() noexcept;
	void Shutdown() noexcept;

private:
	struct implementation;
	implementation* m_implementation = nullptr;
};