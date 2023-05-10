#include "../test.h"

bool isPassingGrade(unsigned int grade)
{
  if (grade < 60)
  {
    return false;
  }

  return true;
}

TEST("Test will pass without any confirms")
{}

TEST("Test passing grades")
{
  bool result = isPassingGrade(0);
  CONFIRM_FALSE(result);

  result = isPassingGrade(100);
  CONFIRM_TRUE(result);

  result = isPassingGrade(59);
  CONFIRM_FALSE(result);

  result = isPassingGrade(60);
  CONFIRM_TRUE(result);

  result = isPassingGrade(101);
  CONFIRM_TRUE(result);

  result = isPassingGrade(1000000);
  CONFIRM_TRUE(result);
}