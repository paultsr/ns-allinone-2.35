#Simulation of TCP
set ns [new Simulator]

set f0 [open reno.tr w]
set f1 [open reno1.tr w]
set f2 [open reno2.tr w]

set nr [open cong.tr w]
$ns trace-all $nr

set nf [open cong.nam w]
$ns namtrace-all $nf

        proc finish { } {
        global ns f0 f2 f1 nr nf
        $ns flush-trace
	close $f0
	close $f1
	close $f2
        close $nf

        exec nam cong.nam &
	exec xgraph reno.tr reno1.tr reno2.tr  -geometry 800 x 400 &
	#exec xgraph reno1.tr -geometry 800 x 400 &
	#exec xgraph reno2.tr -geometry 800 x 400 &
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
$ns duplex-link $n0 $n2 1Mb 10ms DropTail
$ns duplex-link $n1 $n2 1Mb 10ms DropTail
$ns duplex-link $n2 $n3 3Mb 10ms DropTail
$ns duplex-link $n3 $n4 2Mb 10ms DropTail
$ns duplex-link $n3 $n5 2Mb 10ms DropTail
$ns duplex-link $n6 $n2 1Mb 10ms DropTail
$ns duplex-link $n3 $n7 2Mb 10ms DropTail

#Orientation of nodes:
$ns duplex-link-op $n0 $n2 orient right-up
$ns duplex-link-op $n2 $n3 orient middle
$ns duplex-link-op $n1 $n2 orient right-down
$ns duplex-link-op $n3 $n4 orient right-up
$ns duplex-link-op $n3 $n5 orient right-down
$ns duplex-link-op $n6 $n2 orient right
$ns duplex-link-op $n7 $n3 orient left

#Connection of node1 with node5
set tcp0 [new Agent/TCP/Reno]
$ns attach-agent $n1 $tcp0
set ftp0 [new Application/FTP]
$ftp0 set packetSize_ 500
$ftp0 set  interval_ 0.005
$ftp0 attach-agent $tcp0
set sink0 [new Agent/TCPSink]
$ns attach-agent $n4 $sink0
$ns connect $tcp0 $sink0

#Connection of node1 with node4
set tcp1 [new Agent/TCP/Reno]
$ns attach-agent $n0 $tcp1
set ftp [new Application/FTP]
$ftp set packetSize_ 500
$ftp set  interval_ 0.005
$ftp attach-agent $tcp1
set sink [new Agent/TCPSink]
$ns attach-agent $n5 $sink
$ns connect $tcp1 $sink

set tcp2 [new Agent/TCP/Reno]
$ns attach-agent $n6 $tcp2
set ftp1 [new Application/FTP]
$ftp1 set packetSize_ 500
$ftp1 set  interval_ 0.005
$ftp1 attach-agent $tcp2
set sink1 [new Agent/TCPSink]
$ns attach-agent $n7 $sink1
$ns connect $tcp2 $sink1
#Setting color for the nodes
$tcp0 set fid_ 1
$tcp1 set fid_ 2
$tcp2 set fid_ 3

proc record {} {

        global tcp2 tcp0 tcp1 f0 f1 f2

        set ns [Simulator instance]

        set time 0.01

        set wnd0 [$tcp0 set cwnd_]
        set wnd1 [$tcp1 set cwnd_]
        set wnd2 [$tcp2 set cwnd_]
        set now [$ns now]

        puts $f0 "$now $wnd0"
        puts $f1 "$now $wnd1"
        puts $f2 "$now $wnd2"

        $tcp0 set bytes_ 0
        $tcp1 set bytes_ 0
        $tcp2 set bytes_ 0

        $ns at [expr $now+$time] "record"
}

$ns color 1 Red
$ns color 2 Green
$ns color 3 Brown

#Schedule events for the CBR and FTP agents

$ns at 0.1 "record"

$ns at 0.5 "$ftp0 start"
$ns at 1.4 "$ftp start"
$ns at 1.9 "$ftp1 start"
$ns at 4.1 "$ftp stop"
$ns at 3.5 "$ftp0 stop"
$ns at 4.4 "$ftp1 stop"
$ns at 5.0 "finish"
$ns run

