#
# Copyright (C) 2001 by USC/ISI
# All rights reserved.                                            
#                                                                
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation, advertising
# materials, and other materials related to such distribution and use
# acknowledge that the software was developed by the University of
# Southern California, Information Sciences Institute.  The name of the
# University may not be used to endorse or promote products derived from
# this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
# 
# 



# editor-nsparser.tcl
#   - tcl parser for reading in nam editor files
#   - they are basically stripped down ns scripts in the future perhaps the 
#     actual ns parser could be used
#

Editor instproc loadNsScript {tracefile} {
  #filter the tcl file into a enam file first
  set stream [new NamStream $tracefile]

  while {[$stream eof] == 0} {
    set line [$stream gets]
    $self parseLine $line
  }
}


#-----------------------------------------------------------------------
# Editor instproc parseLine {line}
#   - This should be better integrated with setObjectProperties. 
#   - Basically this is a set of regular expressions for parsing
#     a ns script generated by the nam editor
#   - One piece of advice is to put the most complicated regular
#     expressions first so that they match before simpler ones
#-----------------------------------------------------------------------
Editor instproc parseLine {line} {
  $self instvar node_ edge_ agent_ traffic_source_ editor_view_ fid_ \
        loss_model_ netmodel_

  # ----- Nodes -----
  if {[regexp {## node\(([0-9]+)\) at (-*[0-9.]+),(-*[0-9.]+)} \
            $line matched node_id x y]} {
    # Set xy value for node       
    #set node_($node_id) [new Node $node_id circle 10.0]
#    $editor_view_ importNode $node_($node_id) $x $y

  } elseif {[regexp {^set node\(([0-9]+)\) \[\$ns node\]} $line matched node_id]} {
    # Create Node
    #  - 10.0 is the default node size from parser.cc:ParseTable::ParseTable()
    set node_($node_id) [new Node $node_id circle 10.0]
    $editor_view_ importNode $node_($node_id)

  } elseif {[regexp {^\$node\(([0-9]+)\) set ([XYZ]_) ([-.0-9]+)} \
                    $line matched node_id property_variable value]} {

    # Example: $node(id) set X_ 50.0
    $editor_view_ setNodeProperty $node_id $value $property_variable

  } elseif {[regexp {^\$node\(([0-9]+)\) ([a-zA-Z_]+) \"*([a-zA-Z.0-9#]+)\"*} \
                    $line matched node_id property_variable value]} {

    $editor_view_ setNodeProperty $node_id $value $property_variable

  } elseif {[regexp {^\$ns at [0-9.]+ \"\$node\(([0-9]+)\) label ([a-zA-Z_]+) \"} \
                    $line matched node_id value]} {
    #  Handle Node Labels
    $editor_view_ setNodeProperty $node_id $value dlabel_

  } elseif {[regexp {^\$ns at ([0-9.]+) \"\$node\(([0-9]+)\) setdest ([-.0-9]+) ([-.0-9]+) ([.0-9]+)\"} \
                    $line matched time node_id x y speed]} {
    $node_($node_id) addMovement $time $x $y $speed

  # ----- Links -----
  } elseif {[regexp {^\$ns simplex-link \$node\(([0-9]+)\) \$node\(([0-9]+)\) ([0-9.]+)Mb ([0-9.]+)ms ([A-Za-z]+)} \
                    $line matched source destination bandwidth delay queue_type]} {
    # Create Duplex Link
    set sd "$source - $destination"
    set edge_($sd) [new Edge $bandwidth $delay 1.0 0.0 25.0]
    $editor_view_ attachNodes $edge_($sd) $node_($source) $node_($destination) 
    $editor_view_ addQueueHandle $edge_($sd) $queue_type

#    set ds "$destination - $source"
#    set edge_($ds) [new Edge $bandwidth $delay 1.0 0.0 25.0]
#    $editor_view_ attachNodes $edge_($ds) $node_($destination) $node_($source)

  } elseif {[regexp {^\$ns simplex-link-op \$node\(([0-9]+)\) \$node\(([0-9]+)\) ([a-zA-Z_]+) ([0-9.a-zA-Z#]+)} $line matched source destination property_variable value]} {
    # Set Link Property
    $editor_view_ setLinkProperty $source $destination $value $property_variable
    
  # ----- Link Queue Handle Properties -----
  } elseif {[regexp {^\[\[\$ns link \$node\(([0-9]+)\) \$node\(([0-9]+)\)\] queue\] set ([a-zA-Z_]+) ([0-9.a-zA-Z]+)} \
                    $line matched source destination variable_name value]} {
    $editor_view_ setQueueHandleProperty $source $destination \
                                          $variable_name $value

  # ----- Link Loss Models -----
  } elseif {[regexp {^set loss_model\(([0-9]+)\) \[new ErrorModel/(Uniform) ([0-9.]+) ([a-zA-Z]+)} \
                    $line matched loss_model_id loss_model_type rate_ loss_unit_]} { 
    # Create new loss model
    set loss_model_($loss_model_id) [new LossModel $loss_model_type \
                    $loss_model_id 10.0]
    $loss_model_($loss_model_id) setRate $rate_ 
    $loss_model_($loss_model_id) setLossUnit $loss_unit_

  } elseif {[regexp {^set loss_model\(([0-9]+)\) \[new ErrorModel/([a-zA-Z]+)} \
                    $line matched loss_model_id loss_model_type]} { 
    # Create new loss model
    set loss_model_($loss_model_id) [new LossModel $loss_model_type \
                    $loss_model_id 10.0]

  } elseif {[regexp {^\$loss_model\(([0-9]+)\) set ([a-zA-Z_]+) ([0-9.a-zA-Z]+)} \
                    $line matched loss_model_id property_variable value]} { 
    $editor_view_ setLossModelProperty $loss_model_id $value $property_variable

  } elseif {[regexp {^\$ns lossmodel \$loss_model\(([0-9]+)\) \$node\(([0-9]+)\) \$node\(([0-9]+)\)} \
                    $line matched loss_model_id source destination]} { 
    $editor_view_ attachLossModel $loss_model_($loss_model_id) \
                                  $node_($source) $node_($destination) 

  # ----- Agents -----
  } elseif {[regexp {^set agent\(([0-9]+)\) \[new Agent/([a-zA-Z]+)} \
                    $line matched agent_id agent_type]} { 
    # Create new agent
    set agent_($agent_id) [new Agent $agent_type $agent_id 10.0]


  } elseif {[regexp {^\$ns attach-agent \$node\(([0-9]+)\) \$agent\(([0-9]+)\)} \
                    $line matched node_id agent_id]} {
    # Add agent to the editor's view and attach it to its node
    $editor_view_ importAgent $agent_($agent_id) $node_($node_id)


  } elseif {[regexp {^\$agent\(([0-9]+)\) set ([a-zA-Z_]+) ([0-9.a-zA-Z]+)} \
                    $line matched agent_id property_variable value]} {
    # Set Agent Property
    if {$property_variable == "fid_"} {
      $editor_view_ setAgentProperty $agent_id $fid_($value) flowcolor_
    } else {
        $editor_view_ setAgentProperty $agent_id $value $property_variable
    }

  } elseif {[regexp {^\$ns connect \$agent\(([0-9]+)\) \$agent\(([0-9]+)\)} \
                    $line matched source_id destination_id]} {
    # Connect agents to each other
    $editor_view_ linkAgents $agent_($source_id) $agent_($destination_id)



  # ----- NS flow id -----
  } elseif {[regexp {^\$ns color ([0-9]+) \"([a-zA-Z0-9#]+)\"} \
                    $line matched flow_id color]} {
    set fid_($flow_id) $color



  # ----- Traffic Sources -----
  } elseif {[regexp {^set traffic_source\(([0-9]+)\) \[new Application/Traffic/([a-zA-Z]+)} \
                    $line matched trafficsource_id trafficsource_type]} { 
    # Create Traffic Source
    set traffic_source_($trafficsource_id) \
        [new TrafficSource $trafficsource_type $trafficsource_id 10.0]

    # Matching FTP/Telnet Application type traffic sources 
  } elseif {[regexp {^set traffic_source\(([0-9]+)\) \[new Application/([a-zA-Z]+)} \
                    $line matched trafficsource_id trafficsource_type]} { 
    # Create Traffic Source
    set traffic_source_($trafficsource_id) \
        [new TrafficSource $trafficsource_type $trafficsource_id 10.0]

  } elseif {[regexp {^\$traffic_source\(([0-9]+)\) attach-agent \$agent\(([0-9]+)\)} \
                    $line matched trafficsource_id agent_id]} {
    $editor_view_ attachTrafficSource $traffic_source_($trafficsource_id) \
                                      $agent_($agent_id)

  } elseif {[regexp {^\$traffic_source\(([0-9]+)\) set ([a-zA-Z_]+) ([0-9.a-zA-Z]+)} \
                    $line matched trafficsource_id property_variable value]} {

    $editor_view_ setTrafficSourceProperty $trafficsource_id $value $property_variable


  } elseif {[regexp {^\$ns at ([0-9.]+) \"\$traffic_source\(([0-9]+)\) ([starop]+)\"} \
                    $line matched time trafficsource_id property_variable]} {
    switch "$property_variable" {
      start {
        $editor_view_ setTrafficSourceProperty $trafficsource_id $time start_
      }
      stop {
        $editor_view_ setTrafficSourceProperty $trafficsource_id $time stop_
      }
    }

  # ----- Simulator Properties
  } elseif {[regexp {^\$ns at ([0-9.]+) \"finish\"} $line matched finish_time]} {
    $netmodel_ instvar maximum_simulation_time_ 
    $self instvar slider_model_ slider_view_ editorwindow_
    set maximum_simulation_time_ $finish_time
    $slider_model_ destroy
    $slider_view_ destroy
    $self buildTimeSlider $editorwindow_.timeslider
    
  } elseif {[regexp {^#} $line]} {
    # Comment
  } elseif {[regexp {^\n} $line]} {
    # newline character
  } else {
    #puts "Skipping line: $line"
  }
}
