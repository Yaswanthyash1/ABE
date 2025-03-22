# **NS-3 TCP ABE Test Suite Documentation**

## **Overview**
The **TCP ABE Test Suite** is designed to verify the implementation and behavior of the **Alternative Backoff with ECN (ABE)** mechanism in NS-3. ABE is a congestion control enhancement that modifies the way TCP reduces its congestion window (`cwnd`) when Explicit Congestion Notification (ECN) marks are received. This test suite ensures that ABE works correctly with TCP congestion control algorithms like **CUBIC** and **Linux Reno**.

The test suite includes the following test cases:
1. **TcpAbeToggleTest**: Verifies that ABE can be enabled and disabled correctly.
2. **TcpCubicAbeTest**: Tests the behavior of the CUBIC congestion control algorithm with and without ABE.
3. **TcpLinuxRenoAbeTest**: Tests the behavior of the Linux Reno congestion control algorithm with and without ABE.

---

## **Test Cases**

### **1. TcpAbeToggleTest**
- **Purpose**: Verifies that the ABE feature can be enabled and disabled correctly in the `TcpSocketState` class.
- **Steps**:
  1. Enable ABE and check if the `m_enableAbe` flag is set to `true`.
  2. Disable ABE and check if the `m_enableAbe` flag is set to `false`.
- **Expected Outcome**: The `m_enableAbe` flag should correctly reflect the enabled/disabled state of ABE.

### **2. TcpCubicAbeTest**
- **Purpose**: Tests the behavior of the CUBIC congestion control algorithm with and without ABE.
- **Parameters**:
  - `enableAbe`: Whether ABE is enabled (`true` or `false`).
  - `segmentSize`: Size of TCP segments in bytes.
  - `initialCwnd`: Initial congestion window size.
  - `expectedCwnd`: Expected congestion window size after applying the backoff factor.
  - `bytesInFlight`: Number of bytes in flight during the test.
- **Steps**:
  1. Configure the TCP state with the given parameters.
  2. Simulate an ECN event by setting the ECN state to `ECN_ECE_RCVD`.
  3. Update the congestion window using the CUBIC `GetSsThresh` method.
  4. Verify that the updated congestion window matches the expected value.
- **Expected Outcome**:
  - With ABE enabled, the congestion window should be reduced by the ABE-specific backoff factor (e.g., 15% for CUBIC).
  - With ABE disabled, the congestion window should be reduced by the standard backoff factor (e.g., 30% for CUBIC).

### **3. TcpLinuxRenoAbeTest**
- **Purpose**: Tests the behavior of the Linux Reno congestion control algorithm with and without ABE.
- **Parameters**:
  - `enableAbe`: Whether ABE is enabled (`true` or `false`).
  - `initialCwnd`: Initial congestion window size.
  - `expectedCwnd`: Expected congestion window size after applying the backoff factor.
  - `bytesInFlight`: Number of bytes in flight during the test.
- **Steps**:
  1. Configure the TCP state with the given parameters.
  2. Simulate an ECN event by setting the ECN state to `ECN_ECE_RCVD`.
  3. Update the congestion window using the Linux Reno `GetSsThresh` method.
  4. Verify that the updated congestion window matches the expected value.
- **Expected Outcome**:
  - With ABE enabled, the congestion window should be reduced by the ABE-specific backoff factor (e.g., 30% for Linux Reno).
  - With ABE disabled, the congestion window should be reduced by the standard backoff factor (e.g., 50% for Linux Reno).

---

## **Test Suite Integration**

The **TcpAbeTestSuite** registers all the test cases and integrates them into the NS-3 testing framework. To include this test suite in your NS-3 project, follow these steps:

### **1. Add the Test File**
Place the test file (e.g., `tcp-abe-test-suite.cc`) in the `src/internet/test/` directory of your NS-3 installation.

### **2. Update `CMakeLists.txt`**
Add the test file to the `CMakeLists.txt` file in the `src/internet/test/` directory:
```cmake
set(SOURCES
    tcp-abe-test-suite.cc
    # Other test files...
)
```

### **3. Build NS-3**
Rebuild NS-3 to include the new test suite:
```bash
./ns3 configure --enable-tests --enable-examples
```

### **4. Build NS-3**
Rebuild NS-3 to include the new test suite:
```bash
./ns3 build
```

### **5. Run the Test Suite**
Execute the test suite using the NS-3 test runner:
```bash
./test.py --suite=tcp-abe-test
```

---

## **Example Test Output**
When you run the test suite, you should see output similar to the following:
```
Consolidate compiler generated dependencies of target internet-test
Finished executing the following commands:
/usr/bin/cmake --build /home/vishruth/ns-3-dev-abe/cmake-cache -j 19
[0/1] PASS: TestSuite tcp-abe-test
1 of 1 tests passed (1 passed, 0 skipped, 0 failed, 0 crashed, 0 valgrind errors)
```

---

## **Dependencies**

### **NS-3 Modules**:
- `internet` module (for TCP implementation).
- `core` module (for logging and testing framework).

### **External Dependencies**:
- None.

---

## References
1. **[RFC 3168: The Addition of Explicit Congestion Notification (ECN) to IP](https://datatracker.ietf.org/doc/html/rfc3168)**
2. **[RFC 8511: Alternative Backoff with ECN (ABE)](https://datatracker.ietf.org/doc/html/rfc8511)**
3. **[NS-3 TCP Documentation](https://www.nsnam.org/docs/release/3.36/doxygen/group__tcp.html)**
4. **[CUBIC Congestion Control](https://dl.acm.org/doi/10.1145/1400097.1400105)**

---

## **Notes**
- Ensure that the NS-3 version you are using supports the ABE feature.
- Modify the test parameters (e.g., `initialCwnd`, `expectedCwnd`) as needed to test different scenarios.
- Use the NS-3 logging system (`NS_LOG`) to debug and trace the behavior of the tests.

---

