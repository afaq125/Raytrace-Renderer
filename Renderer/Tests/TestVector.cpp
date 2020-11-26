#include "TestVector.h"
#include "Vector.h"

TEST_F(VectorUnitTests, Test_Good) 
{
	Renderer::Math::Vector3 a = Renderer::Math::Vector3(10);
	a.Normalize();
	EXPECT_NEAR(a.Length(), 1.0f, 0.1f) << "Length: " << a.Length();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}