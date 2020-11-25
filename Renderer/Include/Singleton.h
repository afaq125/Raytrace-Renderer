#pragma once

namespace Renderer
{
	template <typename T>
	class Singleton
	{
	public:
		inline static T* GetSingleton()
		{
			if (mSingleton == nullptr)
			{
				mSingleton = new T();
			}
			return mSingleton;
		}

		inline static void Destroy()
		{
			if (mSingleton != nullptr)
			{
				delete mSingleton;
			}
			mSingleton = nullptr;
		}

	protected:
		Singleton() = default;
		Singleton(const Singleton &rhs) = delete;
		Singleton(Singleton &&rhs) = delete;
		Singleton<T>& operator=(const Singleton<T>& rhs) = delete;
		virtual ~Singleton() {};

		static T* mSingleton;
	};
}
