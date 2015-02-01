import QtQuick 2.2
import Flow 1.0 as Flow
import App 1.0 as App

Flow.Graph{
    name:"G"
    Flow.TimerSourceNode{
        id: sourceNode
        debug:true
        outputPort.next: n1.inputPort
        onResponseReceived: console.log("N1 Response Received:" + port)

        running: true
        repeat: false
        interval: 1000
        property int seq:0
        onTriggered: {
            var newPacket = {"seq": seq, "data": "Start Content" }
            sourceNode.sendRequestToPort(outputPort,newPacket)
            seq ++
        }
        onRunningChanged: console.log("total:" + seq)
    }

    Flow.InOutNode{
        id: n1
        name : "n1"
        debug: true
        outputPort.next: n2.inputPort
        onRequestReceived: {
            sendRequestToPort(outputPort,flowPacket)
        }
        onResponseReceived: {
            sendResponseToPort(inputPort,flowPacket)
        }
    }

    Flow.InOutNode{
        id: n2
        name : "n2"
        debug: true
        outputPort.next: n3.inputPort
        onRequestReceived: {
            sendRequestToPort(outputPort,flowPacket)
        }
        onResponseReceived: {
            sendResponseToPort(inputPort,flowPacket)
        }
    }

    Flow.InOutNode{
        id: n3
        name : "n3"
        debug: true
        outputPort.next: sinkNode.inputPort
        onRequestReceived: {
            sendRequestToPort(outputPort,flowPacket)
        }
        onResponseReceived: {
            sendResponseToPort(inputPort,flowPacket)
        }
    }

    Flow.SinkNode{
        id: sinkNode
        debug: true
        onRequestReceived: {
            console.log("N1 Request Received:" + port)
            sendResponseToPort(port,flowPacket)
        }
    }

    App.CppNode{
        id: cppNode
    }

}
