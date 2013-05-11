set ns [new Simulator]
set nr [open thro.tr w]
$ns trace-all $nr
set nf [open thro.nam w]
$ns namtrace-all $nf

        proc finish { } {
        global ns nr nf
        $ns flush-trace
        close $nf
        close $nr

        exec awk {
                        {
                        if($1="r" && $4=="5" && $5=="tcp")
                                {
                                data+=$6;
                                print $2,data*8.0/$2/1000000;
                                }
                        }



                } thro.tr > out.dat
        exec awk {
                        {
                        if($1="r" && $4=="4" && $5=="cbr")
                                {
                                data+=$6;
                                print $2,data*8.0/$2/1000000;
                                }
                        }
                } thro.tr > out1.dat
 exec awk {
                        {
                        if($1="r" && $4=="7" && $5=="tcp")
                                {
                                data+=$6;
                                print $2,data*8.0/$2/1000000;
                                }
                        }
                } thro.tr > out2.dat




        exec xgraph out.dat out1.dat out2.dat &
        exec nam thro.nam &

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
set udp0 [new Agent/UDP]
$ns attach-agent $n1 $udp0
set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ 500
$cbr0 set  interval_ 0.005
$cbr0 attach-agent $udp0
set null0 [new Agent/Null]
$ns attach-agent $n4 $null0
$ns connect $udp0 $null0

#Connection of node1 with node4
set tcp0 [new Agent/TCP]
$ns attach-agent $n0 $tcp0
set ftp [new Application/FTP]
$ftp set packetSize_ 500
$ftp set  interval_ 0.005
$ftp attach-agent $tcp0
set sink [new Agent/TCPSink]
$ns attach-agent $n5 $sink
$ns connect $tcp0 $sink

#Connection of node6 with node7
set tcp1 [new Agent/TCP]
$ns attach-agent $n6 $tcp1
set ftp1 [new Application/FTP]
$ftp1 set packetSize_ 500
$ftp1 set  interval_ 0.005
$ftp1 attach-agent $tcp1
set sink [new Agent/TCPSink]
$ns attach-agent $n7 $sink
$ns connect $tcp1 $sink

#Setting color for the nodes
$udp0 set fid_ 1
$tcp0 set fid_ 2
$tcp1 set fid_ 3
$ns color 1 Red
$ns color 2 Green
$ns color 3 Brown


#Schedule events for the CBR and FTP agents
$ns at 0.1 "$cbr0 start"
$ns at 1.0 "$ftp start"
$ns at 1.5 "$ftp1 start"
$ns at 4.0 "$ftp stop"
$ns at 4.2 "$ftp1 stop"
$ns at 4.5 "$cbr0 stop"
$ns at 5.0 "finish"
$ns run









