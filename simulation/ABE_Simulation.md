# NS-3 Simulation for ECN with Flow Monitoring and Queue Size Tracking

This NS-3 simulation demonstrates the behavior of Explicit Congestion Notification (ECN) in a network with flow monitoring, queue size tracking, and congestion window (CWND) analysis. The simulation uses TCP Cubic as the default transport protocol and RED (Random Early Detection) as the queue management algorithm. It generates outputs for throughput, queue size, and CWND changes over time.

---

## Table of Contents
1. [Overview](#overview)
2. [Simulation Setup](#simulation-setup)
3. [Dependencies](#dependencies)
4. [Building and Running the Simulation](#building-and-running-the-simulation)
5. [Configuration Options](#configuration-options)
6. [Output Files](#output-files)
7. [Key Components](#key-components)
8. [Troubleshooting](#troubleshooting)

---

## Overview
This simulation models a network with the following components:
- *Sender Node*: Generates TCP traffic using the BulkSend application.
- *Receiver Node*: Receives traffic using the PacketSink application.
- *Routers*: Two routers connected by a bottleneck link with RED queue management.
- *Flow Monitoring*: Tracks throughput and queue size over time.
- *CWND Tracing*: Logs congestion window changes for analysis.

The simulation outputs data for:
- Throughput (in bits per second).
- Queue size (in packets).
- Congestion window (in segments).

---

## Simulation Setup
The network topology consists of:
1. *Sender Node* → *Router 1* → *Router 2* → *Receiver Node*.
2. The link between Router 1 and Router 2 is the bottleneck (10 Mbps, 10 ms delay).
3. The sender and receiver links are high-speed (1000 Mbps, 5 ms delay).

---

## Dependencies
- *NS-3*: Ensure you have NS-3 installed. This code is compatible with NS-3.36 and later versions.
- *C++ Compiler*: A modern C++ compiler (e.g., g++ or clang).
- *Python*: Required for running NS-3 scripts (if applicable).
---

## Building and Running the Simulation

1. *Clone the Repository*:
   ```bash
   git clone git@github.com:Yaswanthyash1/Alternative-Backoff-with-ECN-for-TCP.git
   cd Alternative-Backoff-with-ECN-for-TCP
   ```

2. *Build the Simulation*:
   - Place the simulation code in the scratch directory of the NS-3 repo by checking out to tcp_linux_reno_update branch.
   - Build the simulation using the following command:
     ```bash
     ./ns3 configure --enable-examples --enable-tests ./ns3 build
     ```
     

4. *Run the Simulation*:
   - Execute the simulation with default parameters:
     ```bash
     ./ns3 run <simulation-file-name>
     ```
   - To customize parameters, use command-line arguments (see [Configuration Options](#configuration-options)).

---

## Configuration Options
The simulation can be configured using the following command-line arguments:

| Argument          | Description                                                                 | Default Value       |
|-------------------|-----------------------------------------------------------------------------|---------------------|
| --tcpTypeId     | TCP variant to use (e.g., TcpCubic, TcpNewReno).                        | TcpCubic          |
| --delAckCount   | Delayed ACK count.                                                          | 2                 |
| --enablePcap    | Enable/disable PCAP file generation.                                        | true              |
| --stopTime      | Simulation stop time (in seconds).                                          | 100               |
| --useEcn        | Enable/disable ECN.                                                         | true              |
| --minTh         | Minimum threshold for RED queue (in packets).                               | 5.0               |
| --maxTh         | Maximum threshold for RED queue (in packets).                               | 50.0              |
| --AdaptMaxP     | Enable adaptive maximum probability for RED.                                | true              |
| --QW            | Weight for queue size in RED.                                               | 0.5               |

Example:
```bash
./ns3 run <sim-name> --tcpTypeId=TcpNewReno --stopTime=200 --useEcn=false
```

---

## Output Files
The simulation generates the following output files in a timestamped directory under cubic-results/:

| File Name         | Description                                                                 |
|-------------------|-----------------------------------------------------------------------------|
| throughput.dat  | Throughput (in bits per second) over time.                                  |
| queueSize.dat   | Queue size (in packets) over time.                                          |
| cwnd.dat        | Congestion window (in segments) over time.                                  |
| queueStats.txt  | Statistics for the RED queue.                                               |
| config.txt      | Simulation configuration parameters.                                        |
| pcap/           | PCAP traces (if enabled).                                                   |

---

## Key Components

1. *Flow Monitor*:
   - Tracks throughput and flow statistics using the FlowMonitor module.
   - Logs throughput values every 0.2 seconds.

2. *Queue Size Monitoring*:
   - Tracks the size of the RED queue at Router 1.
   - Logs queue size every 0.2 seconds.

3. *CWND Tracing*:
   - Traces the congestion window of the TCP sender.
   - Logs CWND changes over time.

4. *RED Queue Configuration*:
   - Uses adaptive RED (ARED) with ECN support.
   - Configurable minimum and maximum thresholds.

---

## Troubleshooting

1. *Directory Creation Fails*:
   - Ensure you have write permissions in the working directory.
   - Check if the std::filesystem library is supported by your compiler.

2. *Queue Disc Installation Fails*:
   - Verify that the RED queue disc is properly configured in the NS-3 installation.

3. *No Output Files*:
   - Ensure the simulation runs to completion without errors.
   - Check if the output directory is created and accessible.

4. *NS-3 Errors*:
   - Ensure you are using a compatible version of NS-3.
   - Rebuild NS-3 if necessary.

---
