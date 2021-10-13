#pragma once

namespace Renderer
{
	template <typename T>
	class Singleton
	{
	public:
		inline static T& GetInstance()
		{
			static T singleton;
			return singleton;
		}

	protected:
		Singleton() = default;
		virtual ~Singleton() {};
		Singleton(const Singleton &rhs) = delete;
		Singleton(Singleton &&rhs) = delete;
		Singleton& operator=(const Singleton& rhs) = delete;
		Singleton& operator=(Singleton&& rhs) = default;
	};
}
