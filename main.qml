import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
    id: root
    width: 800
    height: 580
    visible: true
    title: gameController && gameController.questTitle.length > 0 ? "Game Quests — " + gameController.questTitle : "Game Quests"
    color: "#181a1f"

    readonly property color clrSurface: "#21252b"
    readonly property color clrBorder: "#3b4048"
    readonly property color clrTextPrimary: "#abb2bf"
    readonly property color clrTextTitle: "#e5c07b"
    readonly property color clrAccent: "#61afef"
    readonly property color clrItemBg: "#2c313a"
    readonly property color clrMuted: "#5c6370"

    FileDialog {
        id: openDialog
        title: "Choose a quest file"
        nameFilters: ["Forth/Quest scripts (*.forth *.fth *.txt)", "All files (*.*)"]
        onAccepted: {
            if (gameController) {
                gameController.loadQuestFile(selectedFile);
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 1. Top menu
        MenuBar {
            Layout.fillWidth: true
            background: Rectangle {
                color: root.clrSurface
                border.color: root.clrBorder
                border.width: 1
            }

            Menu {
                title: qsTr("File")
                background: Rectangle {
                    implicitWidth: 180
                    color: root.clrSurface
                    border.color: root.clrBorder
                    radius: 4
                }

                Action {
                    text: qsTr("Load quest...")
                    shortcut: StandardKey.Open
                    onTriggered: openDialog.open()
                }

                Action {
                    text: qsTr("Quest menu")
                    enabled: gameController && gameController.hasCatalog
                    onTriggered: {
                        if (gameController)
                            gameController.returnToMainMenu();
                    }
                }

                MenuSeparator {
                    contentItem: Rectangle {
                        implicitHeight: 1
                        color: root.clrBorder
                    }
                }

                Action {
                    text: qsTr("Quit")
                    shortcut: StandardKey.Quit
                    onTriggered: Qt.quit()
                }

                delegate: MenuItem {
                    id: menuItem
                    contentItem: Text {
                        text: menuItem.text
                        color: menuItem.highlighted ? "#ffffff" : root.clrTextPrimary
                        font.pointSize: 10
                        leftPadding: 10
                    }
                    background: Rectangle {
                        color: menuItem.highlighted ? root.clrItemBg : "transparent"
                    }
                }
            }
        }

        // 2. Workspace (log + inventory)
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 16
            spacing: 16

            // Left column
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                Label {
                    text: "EVENT LOG"
                    color: root.clrTextTitle
                    font.bold: true
                    font.letterSpacing: 1.2
                    font.pointSize: 10
                }
                Item {
                    Layout.fillWidth: true
                } // Spacer

                // Current location badge
                Rectangle {
                    implicitHeight: 24
                    implicitWidth: locRow.implicitWidth + 16
                    color: root.clrItemBg
                    border.color: root.clrAccent
                    border.width: 1
                    radius: 12
                    visible: gameController && gameController.currentLocation.length > 0

                    RowLayout {
                        id: locRow
                        anchors.centerIn: parent
                        spacing: 6

                        Text {
                            text: "📍"
                            font.pointSize: 9
                        }

                        Text {
                            text: gameController ? gameController.currentLocation : ""
                            color: root.clrAccent
                            font.bold: true
                            font.pointSize: 9
                        }
                    }
                }

                ScrollView {
                    id: logScrollView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: gameController && gameController.canvasVisible ? parent.height * 0.45 : -1
                    clip: true

                    TextArea {
                        id: logArea
                        text: gameController ? gameController.logText : ""
                        readOnly: true
                        wrapMode: TextArea.Wrap
                        color: root.clrTextPrimary
                        font.family: "JetBrains Mono, Fira Code, Monospace"
                        font.pointSize: 10
                        selectByMouse: true
                        leftPadding: 12
                        rightPadding: 12
                        topPadding: 10
                        bottomPadding: 10

                        onTextChanged: {
                            cursorPosition = length;
                        }

                        background: Rectangle {
                            color: root.clrSurface
                            border.color: root.clrBorder
                            border.width: 1
                            radius: 6
                        }
                    }
                }

                // Drawing canvas (CANVAS.*)
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 180
                    Layout.fillHeight: gameController && gameController.canvasVisible
                    visible: gameController && gameController.canvasVisible
                    color: "#0d0f12"
                    border.color: root.clrBorder
                    border.width: 1
                    radius: 6
                    clip: true

                    Canvas {
                        id: questCanvas
                        anchors.fill: parent
                        anchors.margins: 4

                        property var drawColor: "#61afef"
                        property var ops: gameController ? gameController.canvasOps : []

                        onOpsChanged: requestPaint()
                        onWidthChanged: requestPaint()
                        onHeightChanged: requestPaint()

                        onPaint: {
                            var ctx = getContext("2d");
                            ctx.reset();
                            ctx.fillStyle = "#0d0f12";
                            ctx.fillRect(0, 0, width, height);
                            drawColor = "#61afef";
                            ctx.strokeStyle = drawColor;
                            ctx.fillStyle = drawColor;
                            ctx.lineWidth = 2;
                            ctx.font = "12px sans-serif";

                            for (var i = 0; i < ops.length; ++i) {
                                var line = ops[i];
                                var parts = line.split(" ");
                                if (parts[0] === "CLEAR") {
                                    ctx.fillStyle = "#0d0f12";
                                    ctx.fillRect(0, 0, width, height);
                                    ctx.fillStyle = drawColor;
                                } else if (parts[0] === "COLOR" && parts.length >= 4) {
                                    drawColor = "rgb(" + parts[1] + "," + parts[2] + "," + parts[3] + ")";
                                    ctx.strokeStyle = drawColor;
                                    ctx.fillStyle = drawColor;
                                } else if (parts[0] === "RECT" && parts.length >= 5) {
                                    ctx.fillRect(+parts[1], +parts[2], +parts[3], +parts[4]);
                                } else if (parts[0] === "LINE" && parts.length >= 5) {
                                    ctx.beginPath();
                                    ctx.moveTo(+parts[1], +parts[2]);
                                    ctx.lineTo(+parts[3], +parts[4]);
                                    ctx.stroke();
                                } else if (parts[0] === "TEXT" && parts.length >= 4) {
                                    var tx = +parts[1];
                                    var ty = +parts[2];
                                    var tlen = +parts[3];
                                    var prefix = "TEXT " + parts[1] + " " + parts[2] + " " + parts[3] + " ";
                                    var msg = line.substring(prefix.length, prefix.length + tlen);
                                    ctx.fillText(msg, tx, ty);
                                }
                            }
                        }
                    }
                }

                // Choice buttons
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    visible: gameController && gameController.choices.length > 0

                    Label {
                        text: "AVAILABLE ACTIONS:"
                        color: root.clrTextTitle
                        font.bold: true
                        font.pointSize: 9
                        font.letterSpacing: 1.1
                    }

                    Repeater {
                        model: gameController ? gameController.choices : []

                        Button {
                            id: choiceBtn
                            required property string modelData
                            required property int index

                            Layout.fillWidth: true
                            Layout.preferredHeight: 38
                            font.pointSize: 10

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                acceptedButtons: Qt.NoButton
                            }

                            onClicked: {
                                gameController.makeChoice(index);
                            }

                            contentItem: RowLayout {
                                spacing: 8
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12

                                Text {
                                    text: (choiceBtn.index + 1) + "."
                                    color: root.clrAccent
                                    font.bold: true
                                    font.pointSize: 10
                                }

                                Text {
                                    text: choiceBtn.modelData
                                    color: choiceBtn.down ? "#ffffff" : (choiceBtn.hovered ? "#ffffff" : "#dcdfe4")
                                    font.pointSize: 10
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                            }

                            background: Rectangle {
                                color: choiceBtn.down ? "#3a3f4b" : (choiceBtn.hovered ? root.clrItemBg : root.clrSurface)
                                border.color: choiceBtn.hovered ? root.clrAccent : root.clrBorder
                                border.width: 1
                                radius: 6
                            }
                        }
                    }
                }
            }

            // Right column
            ColumnLayout {
                Layout.preferredWidth: 230
                Layout.fillHeight: true
                spacing: 10

                Label {
                    text: "INVENTORY"
                    color: root.clrTextTitle
                    font.bold: true
                    font.letterSpacing: 1.2
                    font.pointSize: 10
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: root.clrSurface
                    border.color: root.clrBorder
                    border.width: 1
                    radius: 6

                    ListView {
                        id: invList
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 6
                        clip: true
                        model: gameController ? gameController.inventory : []

                        delegate: Rectangle {
                            width: invList.width
                            height: 38
                            color: root.clrItemBg
                            radius: 4
                            border.color: root.clrBorder
                            border.width: 1

                            required property string modelData

                            // Split "item - N" into name and count
                            readonly property var parts: modelData.split(" - ")
                            readonly property string itemName: parts[0] ? parts[0] : modelData
                            readonly property string itemCount: parts[1] ? parts[1] : ""

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                spacing: 8

                                Text {
                                    text: "✦"
                                    color: root.clrAccent
                                    font.pointSize: 9
                                }

                                // Item name
                                Text {
                                    text: itemName
                                    color: "#dcdfe4"
                                    font.pointSize: 10
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }

                                // Count badge
                                Rectangle {
                                    visible: itemCount.length > 0
                                    implicitWidth: countText.implicitWidth + 12
                                    implicitHeight: 20
                                    radius: 10
                                    color: "#21252b"
                                    border.color: root.clrBorder

                                    Text {
                                        id: countText
                                        anchors.centerIn: parent
                                        text: "×" + itemCount
                                        color: root.clrAccent
                                        font.bold: true
                                        font.pointSize: 9
                                    }
                                }
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "Inventory empty"
                            color: root.clrMuted
                            font.italic: true
                            font.pointSize: 10
                            visible: invList.count === 0
                        }
                    }
                }
            }
        }

        // 3. Status bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            color: root.clrSurface
            border.color: root.clrBorder
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                spacing: 12

                Text {
                    text: gameController && gameController.questAuthor.length > 0 ? "Author: " + gameController.questAuthor : "Author: unknown"
                    color: root.clrMuted
                    font.pointSize: 9
                }

                Rectangle {
                    width: 1
                    height: 12
                    color: root.clrBorder
                }

                Text {
                    text: gameController && gameController.questVersion.length > 0 ? "Version: " + gameController.questVersion : "Version: 1.0"
                    color: root.clrMuted
                    font.pointSize: 9
                }

                Item {
                    Layout.fillWidth: true
                } // Spacer (Spacer)

                Text {
                    text: "Forth Quest Engine"
                    color: root.clrBorder
                    font.pointSize: 9
                }
            }
        }
    }

    Rectangle {
        id: endOverlay
        anchors.fill: parent
        color: "#d9101216"
        z: 100

        visible: opacity > 0
        opacity: gameController && gameController.isGameOver ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }

        MouseArea {
            anchors.fill: parent
        }

        // Outcome helpers
        readonly property bool isWin: gameController && gameController.gameOutcome === "victory"
        readonly property color themeColor: isWin ? "#98c379" : "#e06c75" // Green win / red lose

        Rectangle {
            anchors.centerIn: parent
            z: 1
            width: Math.min(parent.width - 40, 430)
            implicitHeight: contentCol.implicitHeight + 44
            color: root.clrSurface
            border.color: endOverlay.themeColor
            border.width: 1
            radius: 10

            ColumnLayout {
                id: contentCol
                anchors.fill: parent
                anchors.margins: 24
                spacing: 14

                // Outcome icon
                Text {
                    text: endOverlay.isWin ? "🏆" : "💀"
                    font.pointSize: 40
                    Layout.alignment: Qt.AlignHCenter
                }

                // Title
                Text {
                    text: endOverlay.isWin ? "QUEST COMPLETE!" : "QUEST FAILED!"
                    color: endOverlay.themeColor
                    font.bold: true
                    font.pointSize: 13
                    font.letterSpacing: 1.5
                    Layout.alignment: Qt.AlignHCenter
                }

                // Outcome message
                Text {
                    text: gameController ? gameController.endMessage : ""
                    color: root.clrTextPrimary
                    font.pointSize: 10
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                    lineHeight: 1.3
                }

                Item {
                    Layout.preferredHeight: 8
                }

                // Restart button
                Button {
                    id: restartBtn
                    text: endOverlay.isWin ? "↻  Play again" : "↻  Try again"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 42
                    font.bold: true
                    font.pointSize: 10

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        acceptedButtons: Qt.NoButton
                    }

                    onClicked: {
                        if (gameController) {
                            gameController.restartQuest();
                        }
                    }

                    contentItem: Text {
                        text: restartBtn.text
                        font: restartBtn.font
                        color: restartBtn.down ? "#ffffff" : (restartBtn.hovered ? "#ffffff" : endOverlay.themeColor)
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: restartBtn.down ? "#3a3f4b" : (restartBtn.hovered ? root.clrItemBg : root.clrSurface)
                        border.color: restartBtn.hovered ? endOverlay.themeColor : root.clrBorder
                        border.width: 1
                        radius: 6
                    }
                }

                Button {
                    id: loadOtherBtn
                    text: gameController && gameController.hasCatalog ? "☰  Quest menu" : "📂  Load another quest…"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 42
                    font.pointSize: 10

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        acceptedButtons: Qt.NoButton
                    }

                    onClicked: {
                        if (gameController && gameController.hasCatalog)
                            gameController.returnToMainMenu();
                        else
                            openDialog.open();
                    }

                    contentItem: Text {
                        text: loadOtherBtn.text
                        font: loadOtherBtn.font
                        color: loadOtherBtn.down ? "#ffffff" : (loadOtherBtn.hovered ? "#ffffff" : root.clrAccent)
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: loadOtherBtn.down ? "#3a3f4b" : (loadOtherBtn.hovered ? root.clrItemBg : root.clrSurface)
                        border.color: loadOtherBtn.hovered ? root.clrAccent : root.clrBorder
                        border.width: 1
                        radius: 6
                    }
                }
            }
        }
    }

    // Main quest catalog menu
    Rectangle {
        id: mainMenu
        anchors.fill: parent
        color: "#e0101216"
        z: 90

        visible: opacity > 0
        opacity: gameController && gameController.mainMenuVisible ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                duration: 220
            }
        }

        MouseArea {
            anchors.fill: parent
        }

        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width - 48, 520)
            height: Math.min(parent.height - 48, 520)
            color: root.clrSurface
            border.color: root.clrBorder
            border.width: 1
            radius: 10

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 22
                spacing: 12

                Text {
                    text: "GAME QUESTS"
                    color: root.clrTextTitle
                    font.bold: true
                    font.pointSize: 16
                    font.letterSpacing: 2
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: "Pick a scenario from the catalog"
                    color: root.clrMuted
                    font.pointSize: 10
                    Layout.alignment: Qt.AlignHCenter
                }

                ListView {
                    id: catalogList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 6
                    model: gameController ? gameController.catalogQuests : []

                    delegate: Button {
                        id: questBtn
                        required property var modelData
                        width: catalogList.width
                        height: 52
                        font.pointSize: 10

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            acceptedButtons: Qt.NoButton
                        }

                        onClicked: {
                            if (gameController)
                                gameController.loadQuestPath(modelData.path);
                        }

                        contentItem: ColumnLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            spacing: 2

                            Text {
                                text: questBtn.modelData.category
                                color: root.clrAccent
                                font.pointSize: 8
                                font.bold: true
                                visible: questBtn.modelData.category.length > 0
                            }
                            Text {
                                text: questBtn.modelData.title
                                color: questBtn.hovered ? "#ffffff" : "#dcdfe4"
                                font.pointSize: 11
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }

                        background: Rectangle {
                            color: questBtn.down ? "#3a3f4b" : (questBtn.hovered ? root.clrItemBg : "#1b1e24")
                            border.color: questBtn.hovered ? root.clrAccent : root.clrBorder
                            border.width: 1
                            radius: 6
                        }
                    }
                }

                Button {
                    id: browseBtn
                    text: "Open file from disk…"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    font.pointSize: 10

                    onClicked: openDialog.open()

                    contentItem: Text {
                        text: browseBtn.text
                        font: browseBtn.font
                        color: browseBtn.hovered ? "#ffffff" : root.clrTextPrimary
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: browseBtn.hovered ? root.clrItemBg : "transparent"
                        border.color: root.clrBorder
                        border.width: 1
                        radius: 6
                    }
                }
            }
        }
    }
}
