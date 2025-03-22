# *NS-3 TCP ABE Implementation Documentation*

This document describes the changes made to the NS-3 TCP implementation to support *Alternative Backoff with ECN (ABE)* as defined in *RFC 8511*. The patches modify the TCP congestion control algorithms (CUBIC and Linux Reno) to incorporate ABE, which adjusts the congestion window (CWND) reduction factor when ECN (Explicit Congestion Notification) is received.

---

## *Table of Contents*
1. [Overview](#overview)
2. [Changes in tcp-socket-state](#changes-in-tcp-socket-state)
3. [Changes in tcp-cubic](#changes-in-tcp-cubic)
4. [Changes in tcp-linux-reno](#changes-in-tcp-linux-reno)
5. [Testing and Validation](#testing-and-validation)
6. [Usage](#usage)
7. [References](#references)

---

## *Overview*
The *Alternative Backoff with ECN (ABE)* mechanism modifies the TCP congestion control algorithm to reduce the congestion window (CWND) less aggressively when ECN marks are received, compared to packet drops. This is achieved by using a different multiplicative decrease factor (BetaEcn) for ECN-marked packets, as specified in *RFC 8511*.

The changes introduce the following:
1. *Enable ABE*: Added a boolean flag (m_enableAbe) to TcpSocketState to enable or disable ABE.
2. *CUBIC ABE Support*: Modified the CUBIC congestion control algorithm to use BetaEcn when ABE is enabled and ECN marks are received.
3. *Linux Reno ABE Support*: Modified the Linux Reno congestion control algorithm to use BetaEcn when ABE is enabled and ECN marks are received.

---

## **Changes in tcp-socket-state**

### **Added m_enableAbe to TcpSocketState**
- *Purpose*: Introduced a boolean flag (m_enableAbe) to enable or disable ABE in the TCP socket state.
- *Changes*:
  - Added m_enableAbe as a member variable in TcpSocketState.
  - Added a trace source and attribute for m_enableAbe to allow configuration via the NS-3 attribute system.
  - Updated the copy constructor to include m_enableAbe.
---

## **Changes in tcp-cubic**

### **Added BetaEcn to CUBIC**
- *Purpose*: Added support for ABE in the CUBIC congestion control algorithm.
- *Changes*:
  - Added a new attribute BetaEcn to configure the multiplicative decrease factor for ECN-marked packets.
  - Modified the GetSsThresh method to use BetaEcn when ABE is enabled and ECN marks are received.
  - Default value for BetaEcn is set to 0.85 as per RFC 8511.
---

## **Changes in tcp-linux-reno**

### *Added ABE to Linux Reno*
- *Purpose*: Added support for ABE in the Linux Reno congestion control algorithm.
- *Changes*:
  - Added Beta and BetaEcn attributes to configure the multiplicative decrease factors.
  - Modified the GetSsThresh method to use BetaEcn when ABE is enabled and ECN marks are received.
  - Default value for BetaEcn is set to 0.7 as it is recommended by RFC 8511.

---

## *Testing and Validation*
The changes were tested using the following steps:
1. *Unit Tests*:
   - Added unit tests to verify the behavior of ABE in both CUBIC and Linux Reno.
   - Verified that BetaEcn is used when ABE is enabled and ECN marks are received.
   - Verified that the default behavior (without ABE) remains unchanged.

2. *Simulation Tests*:
   - Ran simulations with ECN-enabled traffic to observe the impact of ABE on throughput and queue size.
   - Compared the results with and without ABE to validate the expected behavior.

3. *Integration Tests*:
   - Verified that the changes integrate seamlessly with the existing NS-3 TCP stack.
   - Ensured backward compatibility with existing congestion control algorithms.

---

## *Usage*
To enable ABE in your NS-3 simulations, follow these steps:

1. *Enable ABE*:
   cpp
   Config::SetDefault("ns3::TcpSocketState::EnableAbe", BooleanValue(true));
   

2. *Run Simulation*:
   - Use the modified TCP congestion control algorithms (CUBIC or Linux Reno) in your simulation script. An example script has been provided in the examples directory.

---

## *References*
1. *RFC 8511*: Alternative Backoff with ECN (ABE).
2. *NS-3 Documentations*: [TCP](https://www.nsnam.org/docs/release/3.36/doxygen/group__tcp.html), [TCP Socket Base](https://www.nsnam.org/docs/release/3.36/doxygen/classns3_1_1_tcp_socket_base.html), 
3. *CUBIC Congestion Control*: [CUBIC Paper](https://dl.acm.org/doi/10.1145/1400097.1400105).

---

This documentation provides a comprehensive overview of the changes made to support ABE in NS-3. Let me know if you need further details or assistance!
