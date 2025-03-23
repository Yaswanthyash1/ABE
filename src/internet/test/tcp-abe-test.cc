/**
 * NS-3 TCP ABE (Alternative Backoff with ECN) Test Suite
 *
 * This file defines test cases for verifying the behavior of TCP congestion 
 * control algorithms (CUBIC and Linux Reno) with and without ABE. The tests 
 * evaluate the impact of ABE on congestion window adjustments when ECN is used.
 *
 * - TcpAbeToggleTest: Verifies enabling and disabling ABE.
 * - TcpCubicAbeTest: Tests CUBIC congestion control behavior with ABE.
 * - TcpLinuxRenoAbeTest: Tests Linux Reno congestion control behavior with ABE.
 * - TcpAbeTestSuite: Registers and runs all ABE-related test cases.
 *
 * This test suite ensures the correct implementation of ABE in NS-3 by 
 * checking if congestion control mechanisms apply the expected backoff factors.
 *
 * Note: Make sure to add this test file in CMakeLists.txt to integrate it into 
 * the NS-3 testing framework.
 */

#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/tcp-socket-state.h"
#include "ns3/tcp-cubic.h"
#include "ns3/tcp-linux-reno.h"


namespace ns3
{
NS_LOG_COMPONENT_DEFINE("TcpAbeTestSuite");

/**
 * @ingroup internet-test
 *
 * @brief checks if users are able to enable ABE or not .
 *        Also checks if the congestion window is getting reduced appropriately. 
 *
 **/

/** 
 * Test case to check enabling/disabling ABE
 */
class TcpAbeToggleTest : public TestCase
{

    public:
        /**
        * @brief Constructor
        *
        */
        TcpAbeToggleTest() : TestCase("Test enabling and disabling ABE") {}
       
        void DoRun() override
        {
            Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
         
            //Enable ABE     
            state->m_enableAbe=true;
            NS_TEST_EXPECT_MSG_EQ(state->m_enableAbe, true, "ABE should be enabled");
            
            //Disable ABE
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
        uint32_t testCase;//!<Test case number 
        uint32_t m_segmentSize;//!<Segment size
        uint32_t m_initialCwnd;//!<Initial congestion window
        uint32_t m_expectedCwnd;//!<Expected congestion window after applying Beta/BetaEcn
        uint32_t m_bytesInFlight;//!<Bytes in flight
    
    public:
        /**
        * @brief Constructor
        *
        * @param testcase test case number
        * @param desc Description about congestion window reduction usng CUBIC congestion control with/without ABE
        */
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
                
                //Configure TCP state 
                state->m_enableAbe=true;
                state->m_ecnState=TcpSocketState::ECN_ECE_RCVD;
                state->m_segmentSize=m_segmentSize;
                state->m_cWnd=m_initialCwnd;
                state->m_bytesInFlight=m_bytesInFlight;
                
                //update cwnd
                state->m_cWnd=cubic->GetSsThresh(state, m_bytesInFlight);
                
                NS_TEST_EXPECT_MSG_EQ(state->m_cWnd,
                    m_expectedCwnd,
                    "CUBIC congestion control with ABE should apply BetaEcn correctly");
            }
            else if(testCase==0){
                //Test CUBIC without ABE                 
                Ptr<TcpCubic> cubic = CreateObject<TcpCubic>();
                Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
               
                //Configure TCP state
                state->m_ecnState=TcpSocketState::ECN_ECE_RCVD;
                state->m_segmentSize=m_segmentSize;
                state->m_cWnd=m_initialCwnd;
                state->m_bytesInFlight=m_bytesInFlight;
                
                //update cwnd
                state->m_cWnd=cubic->GetSsThresh(state, m_bytesInFlight);
                
                NS_TEST_EXPECT_MSG_EQ(state->m_cWnd,
                    m_expectedCwnd, 
                    "CUBIC congestion control should apply Beta correctly");
            }
            else{

                NS_TEST_EXPECT_MSG_EQ(0,
                    1,
                    "Invalid test case");
            }
        }
};

/**
 * Test case for TCP Linux Reno with ABE
 */
class TcpLinuxRenoAbeTest : public TestCase
{
    private:
        uint32_t testCase;//!<Test case number
        uint32_t m_initialCwnd;//!<Initial congestion window
        uint32_t m_expectedCwnd;//!<Expected congestion window after applying Beta/BetaEcn
        uint32_t m_bytesInFlight;//!<Bytes in flight
        
    public:
        /**
        * @brief Constructor
        *
        * @param testcase test case number
        * @param desc Description about congestion window reduction while using Linux Reno congestion control with/without ABE
        */
        TcpLinuxRenoAbeTest(
            uint32_t testCase,
            uint32_t initialCwnd,
            uint32_t expectedCwnd,
            uint32_t bytesInFlight,
            const std::string& desc) : 
                TestCase(desc),
                testCase(testCase),
                m_initialCwnd(initialCwnd),
                m_expectedCwnd(expectedCwnd),
                m_bytesInFlight(bytesInFlight)
            {}

        void DoRun() override
        {
            if(testCase==1){

                Ptr<TcpLinuxReno> cubic = CreateObject<TcpLinuxReno>();
                Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
                
                //Configure TCP state
                state->m_enableAbe=true;
                state->m_ecnState=TcpSocketState::ECN_ECE_RCVD;
                state->m_cWnd=m_initialCwnd;
                state->m_bytesInFlight=m_bytesInFlight;
                //update cwnd
                state->m_cWnd=cubic->GetSsThresh(state, m_bytesInFlight);
                NS_TEST_EXPECT_MSG_EQ(state->m_cWnd, m_expectedCwnd, "RENO Congestion Control with ABE should apply BetaEcn correctly");
            }
            else if(testCase==0)
            {
                Ptr<TcpLinuxReno> cubic = CreateObject<TcpLinuxReno>();
                Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
                
                //Configure TCP state
                state->m_ecnState=TcpSocketState::ECN_ECE_RCVD;
                state->m_cWnd=m_initialCwnd;
                state->m_bytesInFlight=m_bytesInFlight;
                //update cwnd
                state->m_cWnd=cubic->GetSsThresh(state, m_bytesInFlight);
                
                NS_TEST_EXPECT_MSG_EQ(state->m_cWnd,
                    m_expectedCwnd,
                    "RENO congestion control should apply Beta correctly");
            }
            else{
                
                NS_TEST_EXPECT_MSG_EQ(0,
                    1,
                    "Invalid test case");
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
        AddTestCase(new TcpAbeToggleTest,
                    TestCase::Duration::QUICK);
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
        AddTestCase(new TcpLinuxRenoAbeTest(
                                            0,
                                            1000,
                                            500,
                                            100,
                                            "Test Linux Reno without ABE"),
                        TestCase::Duration::QUICK);
        AddTestCase(new TcpLinuxRenoAbeTest(
                                            1,
                                            1000,
                                            700,
                                            100,
                                            "Test Linux Reno ABE with BetaEcn"),
                        TestCase::Duration::QUICK);
    }
};

// Register test suite
static TcpAbeTestSuite g_tcpAbeTestSuite;

}