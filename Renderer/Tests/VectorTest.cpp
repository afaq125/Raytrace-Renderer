#include "Tests.h"

TEST_F(VectorUnitTests, Test_Good) 
{
	Renderer::Math::Vector3 a = Renderer::Math::Vector3(10);
	a.Normalize();
	EXPECT_NEAR(a.Length(), 1.0f, 0.1f) << "Length: " << a.Length();
}