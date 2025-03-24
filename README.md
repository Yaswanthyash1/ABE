# Alternative Backoff with ECN (ABE) for TCP

## *Course: Advanced Computer Networks (CS359)* 
## *Assignment #11*

---

## Project Resources
- **[Project Wiki](https://github.com/Yaswanthyash1/Alternative-Backoff-with-ECN-for-TCP/wiki)**
- **[Report](https://docs.google.com/document/d/1IERWv0bjkAechFp44KMFB_lOYOndRyhEqN7XR7rDeCw/edit?tab=t.0)**

---

## Overview
**Alternative Backoff with ECN (ABE)** is a TCP congestion control mechanism proposed in **[RFC 8511](https://datatracker.ietf.org/doc/html/rfc8511)**. It optimizes congestion window reduction when **Explicit Congestion Notification (ECN)** is used, improving throughput compared to traditional loss-based TCP congestion control. This project implements and evaluates ABE using the **NS-3 network simulator**.

---

## Why ABE when ECN already exists?
While **ECN** helps reduce packet loss by signaling congestion before queue overflows, **standard TCP still reduces its congestion window aggressively (by 50%)**. 

**ABE improves upon this by reducing the window less aggressively when ECN marks are received, leading to better performance in high-bandwidth networks.**

---

## Key Features of ABE
- **Less Aggressive Backoff**: ABE reduces the congestion window by a smaller factor than traditional TCP (e.g., 15% for **CUBIC** and 30% for **Reno**).

- **Improved Throughput**: By reducing the window less aggressively, ABE maintains higher throughput in ECN-enabled networks.

- **Compatibility**: ABE is **backward-compatible** with existing TCP congestion control algorithms like **CUBIC** and **Reno**.

---

## Implementation in NS-3
### **Modified NS-3 Modules**
1. **tcp-socket-state**:
   - Added `m_enableAbe` boolean flag to enable or disable ABE.
   - Configured ABE via NS-3 attribute system.

2. **tcp-cubic**:
   - Added `BetaEcn` attribute for ECN-marked packet handling.
   - Modified `GetSsThresh` method to use `BetaEcn` when ABE is enabled.

3. **tcp-linux-reno**:
   - Added `Beta` and `BetaEcn` attributes.
   - Modified `GetSsThresh` method to apply `BetaEcn` when ABE is enabled.

---

## System Requirements
### **Machine Type**
- **OS**: Linux (Ubuntu 20.04 or later recommended)
- **Processor**: Multi-core CPU (Intel i5 or equivalent)
- **RAM**: 8 GB or higher
- **Storage**: 20 GB free disk space

### **Boot Disk**
- **Type**: SSD (recommended for faster simulation runs)
- **Size**: At least 20 GB for NS-3 installation and simulation data

---

##  Firewall Configuration
To allow NS-3 simulations, configure your firewall as follows:

### **Allow Inbound Traffic**
```bash
sudo ufw allow 50001/tcp
```

### **Allow Outbound Traffic**
```bash
sudo ufw allow out 50001/tcp
```

### **Enable Logging (Optional)**
```bash
sudo ufw logging on
```

---

## Usage
### **Enable ABE in NS-3**
```cpp
Config::SetDefault("ns3::TcpSocketState::EnableAbe", BooleanValue(true));
```

### **Configure BetaEcn**
#### **For CUBIC**
```cpp
Config::SetDefault("ns3::TcpCubic::BetaEcn", DoubleValue(0.85));
```
#### **For Linux Reno**
```cpp
Config::SetDefault("ns3::TcpLinuxReno::BetaEcn", DoubleValue(0.7));
```

### **Run Simulation**
- Use the modified TCP congestion control algorithms (**CUBIC** or **Linux Reno**) in your simulation script.

---

## Testing and Validation
The changes were tested using the following steps:
1. *Unit Tests*:
   - Verified the behavior of ABE in both CUBIC and Linux Reno.
   - Ensured that BetaEcn is used when ABE is enabled and ECN marks are received.

2. *Simulation Tests*:
   - Ran simulations with ECN-enabled traffic to observe the impact of ABE on throughput and queue size.
   - Compared the results with and without ABE to validate the expected behavior.

3. *Integration Tests*:
   - Verified that the changes integrate seamlessly with the existing NS-3 TCP stack.
   - Ensured backward compatibility with existing congestion control algorithms.

To set up and build NS-3 with ABE support, follow these steps:

- **Clean** any previous build files to ensure a fresh compilation *(optional)*.
```sh
./ns-3 clean
```
- **Configure** NS-3 with support for examples and tests.
```sh
./ns-3 configure --enable-examples --enable-tests
```
- **Build** the NS-3 simulator with the necessary configurations.
```sh
./ns-3 build
```

---

## References
1. **[RFC 3168: The Addition of Explicit Congestion Notification (ECN) to IP](https://datatracker.ietf.org/doc/html/rfc3168)**
2. **[RFC 8511: Alternative Backoff with ECN (ABE)](https://datatracker.ietf.org/doc/html/rfc8511)**
3. **[NS-3 TCP Documentation](https://www.nsnam.org/docs/release/3.36/doxygen/group__tcp.html)**
4. **[CUBIC Congestion Control](https://dl.acm.org/doi/10.1145/1400097.1400105)**

---
