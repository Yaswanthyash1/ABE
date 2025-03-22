/*
* NS-3 Simulation for ECN with Flow Monitoring and Queue Size Tracking
*
* This simulation sets up a network with TCP Cubic, RED queue management, and ECN.
* It monitors throughput, queue size, and congestion window (CWND) changes over time.
*
* Features:
* - Configurable TCP variant (Cubic, NewReno).
* - RED queue management with ECN support.
* - Flow monitoring for throughput and queue size.
* - Congestion window tracing.
* - PCAP file generation for debugging.
* 

-----------------------* Place this file in the scratch directory of ns-3, then configure and build it.*
*/

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/red-queue-disc.h"
#include "ns3/callback.h"
#include <filesystem>

using namespace ns3;

// Global variables for output files and directory
std::string dir;               
std::ofstream throughputFile; 
std::ofstream queueSizeFile;   

// Previous values for throughput calculation
uint32_t prevTxBytes = 0;
Time prevTime = Seconds(0);

/**
* @brief Trace throughput and log it to a file.
* @param monitor The FlowMonitor instance.
*/
static void
TraceThroughput(Ptr<FlowMonitor> monitor)
{
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    if (!stats.empty())
    {
        auto itr = stats.begin();
        Time curTime = Now();
        throughputFile << curTime << " "
                    << 8 * (itr->second.txBytes - prevTxBytes) / ((curTime - prevTime).ToDouble(Time::US))
                    << std::endl;
        prevTime = curTime;
        prevTxBytes = itr->second.txBytes;
    }
    Simulator::Schedule(Seconds(0.2), &TraceThroughput, monitor);
}

/**
* @brief Trace queue size and log it to a file.
* @param qd The QueueDisc instance.
*/
void
CheckQueueSize(Ptr<QueueDisc> qd)
{
    uint32_t qsize = qd->GetCurrentSize().GetValue();
    queueSizeFile << Simulator::Now().GetSeconds() << " " << qsize << std::endl;
    Simulator::Schedule(Seconds(0.2), &CheckQueueSize, qd);
}

/**
* @brief Trace congestion window and log it to a file.
* @param stream The output stream.
* @param oldval Old CWND value.
* @param newval New CWND value.
*/
static void
CwndTracer(Ptr<OutputStreamWrapper> stream, uint32_t oldval, uint32_t newval)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << " " << newval / 1448.0 << std::endl;
}

/**
* @brief Set up CWND tracing for a specific node and socket.
* @param nodeId The node ID.
* @param socketId The socket ID.
*/
void
TraceCwnd(uint32_t nodeId, uint32_t socketId)
{
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(dir + "/cwnd.dat");
    Config::ConnectWithoutContext("/NodeList/" + std::to_string(nodeId) +
                                    "/$ns3::TcpL4Protocol/SocketList/" +
                                    std::to_string(socketId) + "/CongestionWindow",
                                MakeBoundCallback(&CwndTracer, stream));
}

/**
* @brief Create a directory if it doesn't exist.
* @param path The directory path.
*/
void
MakeDirectories(const std::string& path)
{
    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directories(path);
    }
}

int
main(int argc, char* argv[])
{
    // Default configuration values
    std::string tcpTypeId = "TcpCubic";
    std::string queueDisc = "RedQueueDisc";
    uint32_t dataSize = 1448;
    uint32_t delAckCount = 2;
    bool bql = true;
    bool enablePcap = true;
    Time stopTime = Seconds(100);
    double qW = 0.5;
    double minTh = 5.0;
    double maxTh = 50.0;
    bool AdaptMaxP = false;
    bool useEcn = true;

    // Parse command-line arguments
    CommandLine cmd(__FILE__);
    cmd.AddValue("tcpTypeId", "Transport protocol to use: TcpNewReno, TcpCubic", tcpTypeId);
    cmd.AddValue("delAckCount", "Delayed ACK count", delAckCount);
    cmd.AddValue("enablePcap", "Enable/Disable pcap file generation", enablePcap);
    cmd.AddValue("stopTime", "Stop time for applications / simulation time will be stopTime + 1", stopTime);
    cmd.AddValue("useEcn", "Use ECN", useEcn);
    cmd.AddValue("minTh", "Minimum threshold for RED", minTh);
    cmd.AddValue("maxTh", "Maximum threshold for RED", maxTh);
    cmd.AddValue("AdaptMaxP", "Enable adaptive max probability for RED", AdaptMaxP);
    cmd.AddValue("QW", "Weight for queue size for RED", qW);
    cmd.Parse(argc, argv);

    // Configure TCP and RED parameters
    queueDisc = "ns3::" + queueDisc;
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::" + tcpTypeId));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(4194304));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(6291456));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(10));
    Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(delAckCount));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(dataSize));
    Config::SetDefault("ns3::DropTailQueue<Packet>::MaxSize", QueueSizeValue(QueueSize("1p")));
    Config::SetDefault("ns3::RedQueueDisc::ARED", BooleanValue(true));
    Config::SetDefault("ns3::RedQueueDisc::Gentle", BooleanValue(true));
    Config::SetDefault("ns3::RedQueueDisc::MinTh", DoubleValue(minTh));
    Config::SetDefault("ns3::RedQueueDisc::MaxTh", DoubleValue(maxTh));
    Config::SetDefault("ns3::RedQueueDisc::AdaptMaxP", BooleanValue(AdaptMaxP));
    Config::SetDefault("ns3::RedQueueDisc::LinkBandwidth", StringValue("10Mbps"));
    Config::SetDefault("ns3::RedQueueDisc::LinkDelay", StringValue("10ms"));
    Config::SetDefault("ns3::RedQueueDisc::MeanPktSize", UintegerValue(500));
    Config::SetDefault("ns3::RedQueueDisc::QW", DoubleValue(qW));
    Config::SetDefault("ns3::RedQueueDisc::UseHardDrop", BooleanValue(false));
    Config::SetDefault(queueDisc + "::MaxSize", QueueSizeValue(QueueSize("666p")));
    Config::SetDefault("ns3::RedQueueDisc::UseEcn", BooleanValue(useEcn));
    Config::SetDefault("ns3::TcpSocketBase::UseEcn", EnumValue(TcpSocketState::On));
    Config::SetDefault("ns3::TcpSocketState::EnableAbe", BooleanValue(true));

    // Create nodes
    NodeContainer sender, receiver, routers;
    sender.Create(1);
    receiver.Create(1);
    routers.Create(2);

    // Create point-to-point links
    PointToPointHelper bottleneckLink, edgeLink;
    bottleneckLink.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    bottleneckLink.SetChannelAttribute("Delay", StringValue("10ms"));
    edgeLink.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    edgeLink.SetChannelAttribute("Delay", StringValue("5ms"));

    // Install network devices
    NetDeviceContainer senderEdge = edgeLink.Install(sender.Get(0), routers.Get(0));
    NetDeviceContainer r1r2 = bottleneckLink.Install(routers.Get(0), routers.Get(1));
    NetDeviceContainer receiverEdge = edgeLink.Install(routers.Get(1), receiver.Get(0));

    // Install internet stack
    InternetStackHelper internet;
    internet.Install(sender);
    internet.Install(receiver);
    internet.Install(routers);

    // Configure queue discipline
    TrafficControlHelper tch;
    tch.SetRootQueueDisc(queueDisc);
    if (bql)
    {
        tch.SetQueueLimits("ns3::DynamicQueueLimits", "HoldTime", StringValue("1000ms"));
    }
    tch.Install(senderEdge);
    tch.Install(receiverEdge);

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.0.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i2 = ipv4.Assign(r1r2);
    ipv4.NewNetwork();
    Ipv4InterfaceContainer is1 = ipv4.Assign(senderEdge);
    ipv4.NewNetwork();
    Ipv4InterfaceContainer ir1 = ipv4.Assign(receiverEdge);

    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Install applications
    uint16_t port = 50001;
    BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(ir1.GetAddress(1), port));
    source.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer sourceApps = source.Install(sender.Get(0));
    sourceApps.Start(Seconds(0.1));
    Simulator::Schedule(Seconds(0.1) + MilliSeconds(1), &TraceCwnd, 0, 0);
    sourceApps.Stop(stopTime);

    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sink.Install(receiver.Get(0));
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(stopTime);

    // Create output directory
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d-%m-%Y-%I-%M-%S", timeinfo);
    dir = "cubic-results/" + std::string(buffer) + "/";
    MakeDirectories(dir);

    // Trace queue occupancy
    tch.Uninstall(routers.Get(0)->GetDevice(1));
    QueueDiscContainer qd = tch.Install(routers.Get(0)->GetDevice(1));
    Simulator::ScheduleNow(&CheckQueueSize, qd.Get(0));

    // Generate PCAP traces if enabled
    if (enablePcap)
    {
        MakeDirectories(dir + "pcap/");
        bottleneckLink.EnablePcapAll(dir + "/pcap/cubic", true);
    }

    // Open output files
    throughputFile.open(dir + "/throughput.dat", std::ios::out);
    queueSizeFile.open(dir + "/queueSize.dat", std::ios::out);
    NS_ASSERT_MSG(throughputFile.is_open(), "Throughput file was not opened correctly");
    NS_ASSERT_MSG(queueSizeFile.is_open(), "Queue size file was not opened correctly");

    // Install FlowMonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    Simulator::Schedule(Seconds(0 + 0.000001), &TraceThroughput, monitor);

    // Run simulation
    Simulator::Stop(stopTime + TimeStep(1));
    Simulator::Run();

    // Log queue statistics and configuration
    std::ofstream configFile;
    configFile.open(dir + "config.txt", std::fstream::in | std::fstream::out | std::fstream::app);
    configFile << "useEcn " << useEcn << "\n";
    configFile << "queue disc type " << queueDisc << "\n";
    configFile << "transport_prot " << tcpTypeId << "\n";
    configFile << "dataSize " << dataSize << "\n";
    configFile << "delAckCount " << delAckCount << "\n";
    configFile << "stopTime " << stopTime << "\n";
    configFile.close();

    // Cleanup
    Simulator::Destroy();
    throughputFile.close();
    queueSizeFile.close();

    return 0;
}