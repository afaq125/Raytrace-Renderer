#include "Tests.h"

class VectorUnitTests : public ::testing::Test
{
public:
	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(VectorUnitTests, Test_Good) 
{
	Renderer::Math::Vector3 a = Renderer::Math::Vector3(10);
	a.Normalize();
	EXPECT_NEAR(a.Length(), 1.0f, 0.1f) << "Length: " << a.Length();
}

using namespace Renderer;

TEST_F(VectorUnitTests, Test_Logger)
{
    {
        //AsyncQueue<int> q([](int i) { 
        //    std::cout << "Working: " << i << std::endl; 
        //});
        //
        //for (int i = 0; i < 12; ++i)
        //{
        //    //q.Push(i);
        //}

        //std::thread t([&]() {
        //    for (int i = 0; i < 240; ++i)
        //    {
        //        std::cout << "t1: " << i << std::endl;
        //        q.Push(i);
        //    }});



        //std::thread t2([&]() {
        //    for (int i = 0; i < 240; ++i)
        //    {
        //        std::cout << "t2: " << i << std::endl;
        //        q.Push(i);
        //    }});

        //t.join();
        //t2.join();
    }


	FileLogger::Settings settings = {};
	settings.RetentionDays = 1;
	settings.MaxLogFiles = 3;
	settings.MaxLogFileSize = 64;

	Logger::GetInstance().GetFileLogger().SetSettings(settings);

	const std::string thirty_two_byte_text = "__test__";
	LOG_INFO(thirty_two_byte_text);
	LOG_INFO(thirty_two_byte_text);

	LOG_WARNING(thirty_two_byte_text);
	LOG_WARNING(thirty_two_byte_text);

	LOG_ERROR(thirty_two_byte_text);
	LOG_ERROR(thirty_two_byte_text);

	LOG_FAILURE(thirty_two_byte_text);
	LOG_FAILURE(thirty_two_byte_text);

	LOG_DEBUG(thirty_two_byte_text);
	LOG_DEBUG(thirty_two_byte_text);

	LOG_INFO(thirty_two_byte_text);
	LOG_INFO(thirty_two_byte_text);

	LOG_INFO(thirty_two_byte_text);
	LOG_INFO(thirty_two_byte_text);

	LOG_INFO(thirty_two_byte_text);
	LOG_INFO(thirty_two_byte_text);

	LOG_INFO(thirty_two_byte_text);
	LOG_INFO(thirty_two_byte_text);

	LOG_INFO(thirty_two_byte_text);
	LOG_INFO(thirty_two_byte_text);

	LOG_INFO(thirty_two_byte_text);
	LOG_INFO(thirty_two_byte_text);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);

    //while (!Logger::GetInstance().GetQueue().Empty())
    {
    }

    auto& qu = Logger::GetInstance().GetQueue();

    int a = 0;
}

TEST_F(VectorUnitTests, GaussEliminationGood)
{

	Renderer::Math::Matrix<float> aa = { { 1, 0, 0 },{ -0, 0, -1 },{ 0, -1, -0 } };
	auto rr = aa.Inversed();
	auto r = aa.GaussElimination();

	auto a = Renderer::Math::Matrix<float>::Arrange(3, 3);
	a += 1.0f;

	a = { {1, 1, 1}, {2, 4, 0}, {2, 8, 1} };
	auto b = a;

	Renderer::Math::Matrix<float> ia;
	Renderer::Math::Matrix<float> ib;

	double da = 0;
	double db = 0;

	typedef std::chrono::steady_clock clock;
	
	{
		auto start = clock::now();
		ia = a.GaussElimination();
		auto end = clock::now();
		auto duration = std::chrono::duration<double, std::micro>(end - start).count();
		da = duration;
	}

	{
		auto start = clock::now();
		ib = b.Inversed();
		auto end = clock::now();
		auto duration = std::chrono::duration<double, std::micro>(end - start).count();
		db = duration;
	}

	int aaa = 0;

}