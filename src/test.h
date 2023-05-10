#ifndef MERETDD_TEST_H
#define MERETDD_TEST_H

#include <string>
#include <string_view>
#include <vector>
#include <ostream>

namespace MereTdd
{
  class ConfirmException
  {
  public:
    ConfirmException() = default;
    ~ConfirmException() = default;

    std::string_view reason() const
    {
      return m_reason;
    }

  protected:
    std::string m_reason;
  };

  class BoolConfirmException : public ConfirmException
  {
  public:
    BoolConfirmException(bool expected, int line)
    {
      m_reason = "Confirm failed on line " + std::to_string(line) + "\n";
      m_reason += "Expected: "; 
      m_reason += expected ? "true" : "false";
    }
  };

  class MissingException
  {
  public:
    MissingException(std::string_view exType) :
    m_exType(exType)
    {}
    std::string_view exType() const
    {
      return m_exType;
    }
  private:
    std::string m_exType;
  };

  class TestBase
  {
  public:
    TestBase(std::string_view name) :
      m_name(name),
      m_passed(true)
    {}
    ~TestBase() = default;
    virtual void run() = 0;
    virtual void runEx()
    {
      run();
    }

    std::string_view name() const
    {
      return m_name;
    }
    bool passed() const
    {
      return m_passed;
    }

    void setExpectedFailureReason(std::string_view reason)
    {
      m_expectedReason = reason;
    }

    std::string_view expecteFailureReason() const
    {
      return m_expectedReason;
    }

    std::string_view reason() const
    {
      return m_reason;
    }
    void setFailed(std::string_view reason)
    {
      m_passed = false;
      m_reason = reason;
    }

  private:
    std::string m_name;
    bool m_passed;
    std::string m_reason;
    std::string m_expectedReason;
  };

  inline std::vector<TestBase*>& getTests()
  {
    static std::vector<TestBase*> tests;
    return tests;
  }

  inline int runTests(std::ostream& output)
  {
    output << "Running " << getTests().size() << " tests\n";

    int passedCount = 0;
    int failedCount = 0;
    for (auto* test : getTests())
    {
      output << "-------------\n" << test->name() << std::endl;

      try
      {
        test->runEx();
      }
      catch (ConfirmException const& ex)
      {
        test->setFailed(ex.reason());
      }
      catch (MissingException const& ex)
      {
        std::string message = "Expected exception type ";
        message += ex.exType();
        message += " was not thrown";
        test->setFailed(message);
      }
      catch (...)
      {
        test->setFailed("Unexpected exception thrown");
      }

      if (test->passed())
      {
        passedCount++;
        output << "Passed" << std::endl;
      }
      else if (!test->expecteFailureReason().empty() && 
        test->expecteFailureReason() == test->reason())
      {
        passedCount++;
        output << "Expected failure\n" << test->reason() << std::endl;
      }
      else
      {
        failedCount++;
        output << "Failed\n" << test->reason() << std::endl;
      }
    }
    output << "-------------\n";
    if (failedCount == 0)
    {
      output << "All test passed." << std::endl;
    }
    else
    {
      output << "Passed tests: " << passedCount << "\nFailed tests: " << failedCount << std::endl;
    }
    return failedCount;
  }
} // namespace meretdd

#define MERETDD_CLASS_FINAL( line ) Test ## line
#define MERETDD_CLASS_RELAY( line ) MERETDD_CLASS_FINAL( line )
#define MERETDD_CLASS MERETDD_CLASS_RELAY( __LINE__ )

#define MERETDD_INSTANCE_FINAL( line ) test ## line
#define MERETDD_INSTANCE_RELAY( line ) MERETDD_INSTANCE_FINAL( line )
#define MERETDD_INSTANCE MERETDD_INSTANCE_RELAY( __LINE__ )

#define TEST(testName) \
namespace { \
  class MERETDD_CLASS : public MereTdd::TestBase \
  { \
  public: \
    MERETDD_CLASS(std::string_view name) : \
      TestBase(name) \
    { \
      MereTdd::getTests().push_back(this); \
    } \
    void run() override; \
  }; \
} /* end namespace */ \
  MERETDD_CLASS MERETDD_INSTANCE(testName); \
  void MERETDD_CLASS::run()

#define TEST_EX(testName, exceptionType) \
namespace { \
  class MERETDD_CLASS : public MereTdd::TestBase \
  { \
  public: \
    MERETDD_CLASS(std::string_view name) : \
      TestBase(name) \
    { \
      MereTdd::getTests().push_back(this); \
    } \
    void runEx() override \
    { \
      try \
      { \
      run(); \
      } \
      catch (exceptionType const&) \
      { \
        return; \
      } \
      throw MereTdd::MissingException(#exceptionType); \
    } \
    void run() override; \
  }; \
} /* end namespace */ \
  MERETDD_CLASS MERETDD_INSTANCE(testName); \
  void MERETDD_CLASS::run()

#define CONFIRM_FALSE(actual) \
if (actual) \
{ \
  throw MereTdd::BoolConfirmException(false, __LINE__); \
}

#define CONFIRM_TRUE(actual) \
if (!actual) \
{ \
  throw MereTdd::BoolConfirmException(true, __LINE__); \
}

#endif // MERETDD_TEST_H