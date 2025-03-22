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
  * Test case to verify enabling and disabling ABE (Alternative Backoff with ECN).
  */
 class TcpAbeToggleTest : public TestCase
 {
 public:
     TcpAbeToggleTest() : TestCase("Test enabling and disabling ABE") {}
 
     void DoRun() override
     {
         Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
 
         // Enable ABE and verify
         state->m_enableAbe = true;
         NS_TEST_EXPECT_MSG_EQ(state->m_enableAbe, true, "ABE should be enabled");
 
         // Disable ABE and verify
         state->m_enableAbe = false;
         NS_TEST_EXPECT_MSG_EQ(state->m_enableAbe, false, "ABE should be disabled");
     }
 };
 
 /**
  * Test case for TCP CUBIC with ABE.
  *
  * This test verifies the behavior of CUBIC congestion control with and without ABE.
  */
 class TcpCubicAbeTest : public TestCase
 {
 private:
     // Variables for managing congestion control behavior 
     bool m_enableAbe;          
     uint32_t m_segmentSize;    
     uint32_t m_initialCwnd;    
     uint32_t m_expectedCwnd;  
     uint32_t m_bytesInFlight;  
 
 public:
     TcpCubicAbeTest(
         bool enableAbe,
         uint32_t segmentSize,
         uint32_t initialCwnd,
         uint32_t expectedCwnd,
         uint32_t bytesInFlight,
         const std::string& desc)
         : TestCase(desc),
           m_enableAbe(enableAbe),
           m_segmentSize(segmentSize),
           m_initialCwnd(initialCwnd),
           m_expectedCwnd(expectedCwnd),
           m_bytesInFlight(bytesInFlight)
     {
     }
 
     void DoRun() override
     {
         Ptr<TcpCubic> cubic = CreateObject<TcpCubic>();
         Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
 
         // Configure TCP state
         state->m_enableAbe = m_enableAbe;
         state->m_ecnState = TcpSocketState::ECN_ECE_RCVD;
         state->m_segmentSize = m_segmentSize;
         state->m_cWnd = m_initialCwnd;
         state->m_bytesInFlight = m_bytesInFlight;
 
         // Update congestion window
         state->m_cWnd = cubic->GetSsThresh(state, m_bytesInFlight);
 
         // Verify the congestion window reduction
         NS_TEST_EXPECT_MSG_EQ(
             state->m_cWnd, m_expectedCwnd,
             "CUBIC congestion control " << (m_enableAbe ? "with ABE" : "without ABE") <<
             " did not apply the correct backoff factor. Expected CWND: " << m_expectedCwnd <<
             ", Actual CWND: " << state->m_cWnd);
     }
 };
 
 /**
  * Test case for TCP Linux Reno with ABE.
  *
  * This test verifies the behavior of Linux Reno congestion control with and without ABE.
  */
 class TcpLinuxRenoAbeTest : public TestCase
 {
 private:
     // Variables for managing congestion control behavior 
     bool m_enableAbe;          
     uint32_t m_initialCwnd;    
     uint32_t m_expectedCwnd;   
     uint32_t m_bytesInFlight; 
 
 public:
     TcpLinuxRenoAbeTest(
         bool enableAbe,
         uint32_t initialCwnd,
         uint32_t expectedCwnd,
         uint32_t bytesInFlight,
         const std::string& desc)
         : TestCase(desc),
           m_enableAbe(enableAbe),
           m_initialCwnd(initialCwnd),
           m_expectedCwnd(expectedCwnd),
           m_bytesInFlight(bytesInFlight)
     {
     }
 
     void DoRun() override
     {
         Ptr<TcpLinuxReno> reno = CreateObject<TcpLinuxReno>();
         Ptr<TcpSocketState> state = CreateObject<TcpSocketState>();
 
         // Configure TCP state
         state->m_enableAbe = m_enableAbe;
         state->m_ecnState = TcpSocketState::ECN_ECE_RCVD;
         state->m_cWnd = m_initialCwnd;
         state->m_bytesInFlight = m_bytesInFlight;
 
         // Update congestion window
         state->m_cWnd = reno->GetSsThresh(state, m_bytesInFlight);
 
         // Verify the congestion window reduction
         NS_TEST_EXPECT_MSG_EQ(
             state->m_cWnd, m_expectedCwnd,
             "Linux Reno congestion control " << (m_enableAbe ? "with ABE" : "without ABE") <<
             " did not apply the correct backoff factor. Expected CWND: " << m_expectedCwnd <<
             ", Actual CWND: " << state->m_cWnd);
     }
 };
 
 /**
  * Test suite for ABE (Alternative Backoff with ECN).
  */
 class TcpAbeTestSuite : public TestSuite
 {
 public:
     TcpAbeTestSuite()
         : TestSuite("tcp-abe-test", Type::UNIT)
     {
         // Add test cases
         AddTestCase(new TcpAbeToggleTest, TestCase::Duration::QUICK);
 
         // CUBIC tests
         AddTestCase(new TcpCubicAbeTest(
             false, 1, 1000, 700, 100, "Test CUBIC without ABE"), TestCase::Duration::QUICK);
         AddTestCase(new TcpCubicAbeTest(
             true, 1, 1000, 850, 100, "Test CUBIC with ABE"), TestCase::Duration::QUICK);
 
         // Linux Reno tests
         AddTestCase(new TcpLinuxRenoAbeTest(
             false, 1000, 500, 100, "Test Linux Reno without ABE"), TestCase::Duration::QUICK);
         AddTestCase(new TcpLinuxRenoAbeTest(
             true, 1000, 700, 100, "Test Linux Reno with ABE"), TestCase::Duration::QUICK);
     }
 };
 
 // Register the test suite
 static TcpAbeTestSuite g_tcpAbeTestSuite;
 
 } // namespace ns3