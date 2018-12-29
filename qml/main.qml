import QtQuick 2.9
import QtQuick.Controls 2.2

ApplicationWindow {

    id: appWindow
    title: qsTr("Subsystem for Linux x86")

    // running on mobiles you don't need width and height
    // ApplicationWindow will always fill entire screen
    // testing also on desktop it makes sense to set values
    width: 1024
    height: 768

    visible: true
}
