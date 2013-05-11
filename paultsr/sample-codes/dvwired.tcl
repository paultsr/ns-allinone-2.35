set ns [new Simulator]

$ns color 1 Blue
$ns color 2 Red

$ns rtproto DV

set nr [open rout.tr w]
$ns trace-all $nr
set nf [open out.nam w]
$ns namtrace-all $nf

	proc finish { } {
	global ns nr nf
	$ns flush-trace
	close $nf
	exec nam out.nam &
exec awk {

         BEGIN { print "Routing Overhead for DV"

                recv=1
                sent=1

              }
             {
                if(($1=="+" || $3=="1" || $3=="0") )
                {
                        sent++
                }
                if( $1=="r" )
                {
                        recv++
                 }
                if( $5=="rtProtoDV")
                {
                        route++
                }

        }

	END {
		print "No. of Packets Sent	:" sent
		print "No. of Packets Received	:" recv
		print "No. of Routing Packets	:" route
		noh=route/recv
		print "Normalized Routing Overhead:" noh
		pdf=recv/sent
		print "Packet Delivery Fraction	:" pdf
	}
	
  } rout.tr > rpdv.data  &

	exit 0
	}
#Creation of nodes:
for { set i  0 } { $i < 21 } { incr i 1 } {
set n($i) [$ns node]
}

#Connection between nodes:
for {set i 0} {$i < 15} {incr i} {
$ns duplex-link $n($i) $n([expr $i+1]) 1Mb 10ms DropTail
}
$ns duplex-link $n(0) $n(4) 1Mb 10ms DropTail
$ns duplex-link $n(4) $n(7) 1Mb 10ms DropTail
$ns duplex-link $n(3) $n(5) 1Mb 10ms DropTail
$ns duplex-link $n(5) $n(6) 1Mb 10ms DropTail
$ns duplex-link $n(7) $n(14) 1Mb 10ms DropTail
$ns duplex-link $n(14) $n(16) 1Mb 10ms DropTail
$ns duplex-link $n(16) $n(19) 1Mb 10ms DropTail
$ns duplex-link $n(17) $n(20) 1Mb 10ms DropTail
$ns duplex-link $n(11) $n(17) 1Mb 10ms DropTail
$ns duplex-link $n(13) $n(18) 1Mb 10ms DropTail
$ns duplex-link $n(18) $n(19) 1Mb 10ms DropTail
$ns duplex-link $n(19) $n(20) 1Mb 10ms DropTail
$ns duplex-link $n(15) $n(18) 1Mb 10ms DropTail
$ns duplex-link $n(6) $n(8) 1Mb 10ms DropTail 
#Packet from node0 to node 5:
set udp0 [new Agent/UDP]
$ns attach-agent $n(0) $udp0
set cbr0 [new Application/Traffic/Exponential]
$cbr0 set packetSize_ 500
$cbr0 set interval_ 0.005 
$cbr0 attach-agent $udp0
set null0 [new Agent/Null]
$ns attach-agent $n(20) $null0
$ns connect $udp0 $null0

#Packet from node0 to node 5;
set udp1 [new Agent/UDP]
$ns attach-agent $n(1) $udp1
set cbr1 [new Application/Traffic/Exponential]
$cbr1 set packetSize_ 500
$cbr1 set interval_ 0.005
$cbr1 attach-agent $udp1
set null1 [new Agent/Null]
$ns attach-agent $n(20) $null1
$ns connect $udp1 $null1

$udp0 set class_ 1
$udp1 set class_ 2

#Downing the Link for 1 Min
$ns rtmodel-at 10.0 down $n(4) $n(7)
$ns rtmodel-at 17.0 up $n(4) $n(7)
$ns rtmodel-at 15.0 down $n(14) $n(16)
$ns rtmodel-at 27.0 up $n(14) $n(16)
$ns rtmodel-at 25.0 down $n(19) $n(20)
$ns rtmodel-at 30.0 up $n(19) $n(20)

$ns at 5 "$cbr0 start"
$ns at 5 "$cbr1 start"

$ns at 45 "$cbr0 stop"
$ns at 45 "$cbr1 stop"
$ns at 50.0 "finish"
$ns run

