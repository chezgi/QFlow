import QtQuick 2.2
import Flow 1.0 as Flow
import App 1.0 as App

Flow.Graph{
    name:"G"
    Flow.Node{
        id: sourceNode
        debug:true
        Flow.OutPort{
            id: sourceOut1
            next:n1InPort
        }
        onResponseReceived: console.log("N1 Response Received:" + port)
        Timer{
            property int seq:0
            running: true
            repeat: false
            interval: 1000
            onTriggered: {
                var newPacket = {"seq": seq, "data": "Start Content" }
                sourceNode.sendRequestToPort(sourceOut1,newPacket)
                seq ++
            }
            onRunningChanged: console.log("total:" + seq)
        }
    }

    Flow.Node{
        id: n1
        name : "n1"
        debug: true
        Flow.InPort{
            id: n1InPort
        }

        Flow.OutPort{
            id: n1OutPort
            next: n2InPort
        }

        onRequestReceived: {
            sendRequestToPort(n1OutPort,flowPacket)
        }
        onResponseReceived: {
            sendResponseToPort(n1InPort,flowPacket)
        }

    }

    Flow.Node{
        id: n2
        name : "n2"
        debug: true
        Flow.InPort{
            id: n2InPort
        }

        Flow.OutPort{
            id: n2OutPort
            next: n3InPort
        }

        onRequestReceived: {
            sendRequestToPort(n2OutPort,flowPacket)
        }
        onResponseReceived: {
            sendResponseToPort(n2InPort,flowPacket)
        }
    }

    Flow.Node{
        id: n3
        name : "n3"
        debug: true
        Flow.InPort{
            id: n3InPort
        }

        Flow.OutPort{
            id: n3OutPort
            next: sinkInPort
        }
        onRequestReceived: {
            sendRequestToPort(n3OutPort,flowPacket)
        }
        onResponseReceived: {
            sendResponseToPort(n3InPort,flowPacket)
        }
    }

    Flow.Node{
        id: sinkNode
        debug: true
        Flow.InPort{
            id:sinkInPort
        }

        onRequestReceived: {
            console.log("N1 Request Received:" + port)
            sendResponseToPort(port,flowPacket)
        }
    }

    App.CppNode{
        id: cppNode
    }

}
