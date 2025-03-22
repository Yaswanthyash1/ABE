#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/tcp-socket-state.h"
#include "ns3/tcp-cubic.h"
#include "ns3/tcp-linux-reno.h"


namespace ns3
{
NS_LOG_COMPONENT_DEFINE("TcpAbeTestSuite");

/**
 * Test case to check enabling/disabling ABE
 */
class TcpAbeToggleTest : public TestCase
{

    public:
        TcpAbeToggleTest() : TestCase("Test enabling and disabling ABE") {}
        void DoRun() override
        {
            Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
            state->m_enableAbe=true;
            NS_TEST_EXPECT_MSG_EQ(state->m_enableAbe, true, "ABE should be enabled");

            state->m_enableAbe=false;
            NS_TEST_EXPECT_MSG_EQ(state->m_enableAbe, false, "ABE should be disabled");
        }
};

/**
 * Test case for TCP CUBIC with ABE
 */
class TcpCubicAbeTest : public TestCase
{
    private:
    uint32_t testCase;
    uint32_t m_segmentSize;
    uint32_t m_initialCwnd;
    uint32_t m_expectedCwnd;
    uint32_t m_bytesInFlight;
    
    public:
    TcpCubicAbeTest(
        uint32_t testCase,
        uint32_t segmentSize,
        uint32_t initialCwnd,
        uint32_t expectedCwnd,
        uint32_t bytesInFlight,
        const std::string& desc) : 
            TestCase(desc),
            testCase(testCase),
            m_segmentSize(segmentSize),
            m_initialCwnd(initialCwnd),
            m_expectedCwnd(expectedCwnd),
            m_bytesInFlight(bytesInFlight)
        {}

    void DoRun() override
        {
            if(testCase==1)
            {
                Ptr<TcpCubic> cubic = CreateObject<TcpCubic>();
                Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
                state->m_enableAbe=true;
                state->m_ecnState=TcpSocketState::ECN_ECE_RCVD;
                state->m_segmentSize=m_segmentSize;
                state->m_cWnd=m_initialCwnd;
                state->m_bytesInFlight=m_bytesInFlight;
                //update cwnd
                state->m_cWnd=cubic->GetSsThresh(state, m_bytesInFlight);
                NS_TEST_EXPECT_MSG_EQ(state->m_cWnd, m_expectedCwnd, "CUBIC congestion control with ABE should apply BetaEcn correctly");
            }
            else if(testCase==0){
                Ptr<TcpCubic> cubic = CreateObject<TcpCubic>();
                Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
                state->m_ecnState=TcpSocketState::ECN_ECE_RCVD;
                state->m_segmentSize=m_segmentSize;
                state->m_cWnd=m_initialCwnd;
                state->m_bytesInFlight=m_bytesInFlight;
                //update cwnd
                state->m_cWnd=cubic->GetSsThresh(state, m_bytesInFlight);
                NS_TEST_EXPECT_MSG_EQ(state->m_cWnd, m_expectedCwnd, "CUBIC congestion control should apply Beta correctly");
            }
            else{
                NS_TEST_EXPECT_MSG_EQ(0, 1, "Invalid test case");
            }
        }
};

/**
 * Test suite for ABE
 */
class TcpAbeTestSuite : public TestSuite
{
public:
    TcpAbeTestSuite() : TestSuite("tcp-abe-test", Type::UNIT)
    {
        AddTestCase(new TcpAbeToggleTest,  TestCase::Duration::QUICK);
        AddTestCase(new TcpCubicAbeTest(
            0,
            1,
            1000,
            700,
            100,
            "Test CUBIC without ABE"),
            TestCase::Duration::QUICK);
        AddTestCase(new TcpCubicAbeTest(
            1,
            1,
            1000,
            850,
            100,
            "Test CUBIC ABE with BetaEcn"),
            TestCase::Duration::QUICK);
    }
};

// Register test suite
static TcpAbeTestSuite g_tcpAbeTestSuite;

}