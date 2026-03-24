#!/bin/bash

set -e

tracefs=/sys/kernel/tracing

# reset trace state
echo 0 > $tracefs/tracing_on
echo nop > $tracefs/current_tracer
echo > $tracefs/set_ftrace_pid
echo > $tracefs/trace
echo > $tracefs/set_graph_function
echo > $tracefs/set_ftrace_filter
echo > $tracefs/set_ftrace_notrace
echo > $tracefs/set_event
echo 0 > $tracefs/options/stacktrace

# trace only this process

mode="$1"
output=""

case "$mode" in
    send)
        echo "Tracing send path";
        echo function_graph > $tracefs/current_tracer
        echo __sys_sendto >> $tracefs/set_graph_function
        output="send_trace.txt"

        # turn trace on
        echo 1 > $tracefs/tracing_on
        # start target in background and get pid
        "${@:2}" &
        pid=$!
        kill -STOP $pid
        echo "Target pid: $pid"
        # trace the target process and wait
        echo $pid > $tracefs/set_ftrace_pid
        kill -CONT $pid
        wait $pid
        # turn tracing off
        echo 0 > $tracefs/tracing_on
        cat $tracefs/trace > "$output"
        ;;
    recv)
        echo "Tracing recv path";
        echo function_graph > $tracefs/current_tracer
        echo __sys_recvfrom > $tracefs/set_graph_function
        output="recv_trace.txt"

        echo 1 > $tracefs/tracing_on
        # start target in background and get pid
        "${@:2}" &
        pid=$!
        kill -STOP $pid
        echo "Target pid: $pid"
        # trace the target process and wait
        echo $pid > $tracefs/set_ftrace_pid
        kill -CONT $pid
        wait $pid
        # turn tracing off
        echo 0 > $tracefs/tracing_on
        cat $tracefs/trace > "$output"
        ;;
    irq)
        echo "Tracing irq path";

        echo function_graph > $tracefs/current_tracer
        echo handle_irq_event >> $tracefs/set_graph_function
        echo __do_softirq >> $tracefs/set_graph_function
        echo igb_msix_ring >> $tracefs/set_graph_function
        echo net_rx_action >> $tracefs/set_graph_function
        echo igb_poll >> $tracefs/set_graph_function

        output="irq_trace.txt"

        echo 1 > $tracefs/tracing_on

        "${@:2}" &
        pid=$!
        wait $pid

        echo 0 > $tracefs/tracing_on
        cat $tracefs/trace > "$output"
        ;;
    *)
        echo "Unknown mode"
        ;;
esac

