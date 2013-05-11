set ns [new Simulator]

set nr [open que1.tr w]
$ns trace-all $nr

set nf [open que.nam w]
$ns namtrace-all $nf

        proc finish { } {
        global ns nr nf
        $ns flush-trace
        close $nf
        close $nr
exec nam que.nam &
	exec awk {
BEGIN{print "output of droptail"
	
}	
                        {
                        if ($1=="d") {
                                data++
                                }
			if($1=="+")
			{
				send++
			}
			if($1=="r")
			{
			recv++
			}
                        }
END{
	print "data dropped:" data
	print "data send:" send
	print "data recieved:" recv
}
                } que1.tr > out12.dat
	
        exit 0
        }

#Creation of Nodes:
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]
set n6 [$ns node]
set n7 [$ns node]

#Creation of Links:
$ns duplex-link $n0 $n3 1Mb 10ms DropTail
$ns duplex-link $n1 $n3 1Mb 10ms DropTail
$ns duplex-link $n2 $n3 1Mb 10ms DropTail
$ns duplex-link $n3 $n4 1Mb 10ms DropTail
$ns duplex-link $n4 $n5 1Mb 10ms DropTail
$ns duplex-link $n4 $n6 1Mb 10ms DropTail
$ns duplex-link $n4 $n7 1Mb 10ms DropTail

#Orientation of nodes:
$ns duplex-link-op $n0 $n3 orient right-up
$ns duplex-link-op $n1 $n3 orient middle
$ns duplex-link-op $n3 $n4 orient middle
$ns duplex-link-op $n2 $n3 orient right-down
$ns duplex-link-op $n4 $n5 orient right-up
$ns duplex-link-op $n4 $n7 orient right-down
$ns duplex-link-op $n4 $n6 orient middle

#Connection of node0 with node5
set udp0 [new Agent/UDP]
$ns attach-agent $n0 $udp0
set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ 500
$cbr0 set  interval_ 0.005
$cbr0 attach-agent $udp0
set null0 [new Agent/LossMonitor]
$ns attach-agent $n5 $null0
$ns connect $udp0 $null0

#Connection of node1 with node6
set udp1 [new Agent/UDP]
$ns attach-agent $n1 $udp1
set cbr1 [new Application/Traffic/CBR]
$cbr1 set packetSize_ 500
$cbr1 set  interval_ 0.005
$cbr1 attach-agent $udp1
set null1 [new Agent/LossMonitor]
$ns attach-agent $n6 $null1
$ns connect $udp1 $null1

#Connection of node2 with node7
set udp2 [new Agent/UDP]
$ns attach-agent $n2 $udp2
set cbr2 [new Application/Traffic/CBR]
$cbr2 set packetSize_ 500
$cbr2 set  interval_ 0.005
$cbr2 attach-agent $udp2
set null2 [new Agent/LossMonitor]
$ns attach-agent $n7 $null2
$ns connect $udp2 $null2

#Setting color for the nodes
$udp0 set fid_ 1
$udp1 set fid_ 2
$udp2 set fid_ 3

$ns color 1 Red
$ns color 2 Green
$ns color 3 Blue

#Schedule events for the CBR and FTP agents
$ns at 0.3 "$cbr0 start"
$ns at 0.4 "$cbr1 start"
$ns at 0.5 "$cbr2 start"

$ns at 4.6 "$cbr0 stop"
$ns at 4.7 "$cbr1 stop"
$ns at 4.8 "$cbr2 stop"

$ns at 5.0 "finish"
$ns run


