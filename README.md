# Alternative-Backoff-with-ECN-for-TCP
## Course Code: CS359
## Assignment #11

### Webpage  
[Project Wiki](https://github.com/Yaswanthyash1/Alternative-Backoff-with-ECN-for-TCP/wiki)  

### Overview  
Alternative Backoff with ECN (ABE) is a TCP congestion control mechanism proposed in [RFC 8511](https://datatracker.ietf.org/doc/html/rfc8511). It optimizes congestion window reduction when Explicit Congestion Notification (ECN) is used, improving throughput compared to traditional loss-based TCP congestion control. This project implements and evaluates ABE in the NS-3 network simulator.  

### Why ABE when ECN already exists?  
ECN helps reduce packet loss by signaling congestion before queue overflows, but standard TCP still reduces its congestion window aggressively (by 50%). ABE improves upon this by reducing the window less aggressively when ECN marks are received, leading to better performance in high-bandwidth networks.  

### To run this code, set up a Google Cloud Instance with the following specs:  

#### Machine Type  
* 2 vCPUs with 8 GB memory  

#### Boot Disk  
* Ubuntu 20.04 LTS  

#### Check the following under Firewall  
* Allow HTTP Traffic  
* Allow HTTPS Traffic  

Click ‘Create’.  

Run the following commands after logging in through SSH:  
```sh
sudo apt-get update
sudo apt-get -y install git
```

### Building and Setting Up NS-3

To set up and build NS-3 with ABE support, follow these steps:

```sh
./ns-3 clean
./ns-3 configure --enable-examples --enable-tests
./ns-3 build
```

These commands will:

- **Clean** any previous build files to ensure a fresh compilation.
- **Configure** NS-3 with support for examples and tests.
- **Build** the NS-3 simulator with the necessary configurations.
