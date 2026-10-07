import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Window {
    id: root
    width: 960
    height: 680
    visible: true
    title: gameController && !gameController.mainMenuVisible && gameController.questTitle.length > 0
           ? "QuestForth — " + gameController.questTitle
           : "QuestForth"
    color: "#12141a"

    // Refined dark palette
    readonly property color clrBg: "#12141a"
    readonly property color clrSurface: "#1a1d26"
    readonly property color clrElevated: "#222632"
    readonly property color clrBorder: "#2e3440"
    readonly property color clrBorderSoft: "#252a35"
    readonly property color clrText: "#d8dce6"
    readonly property color clrTextDim: "#8b929e"
    readonly property color clrTextFaint: "#5c6370"
    readonly property color clrBrand: "#c9a227"
    readonly property color clrAccent: "#6cb2e3"
    readonly property color clrWin: "#7cb87c"
    readonly property color clrLoss: "#d16b6b"
    readonly property color clrStory: "#ece6d9"

    readonly property string fontUi: "IBM Plex Sans, Segoe UI, Ubuntu, Cantarell, sans-serif"
    readonly property string fontStory: "Literata, Source Serif 4, Georgia, 'Times New Roman', serif"
    readonly property string fontMono: "JetBrains Mono, Cascadia Code, Consolas, monospace"

    property string menuTab: "quests"

    function formatWhen(iso) {
        if (!iso)
            return "";
        var d = new Date(iso);
        if (isNaN(d.getTime()))
            return iso;
        return Qt.formatDateTime(d, "d MMM yyyy · HH:mm");
    }

    function formatDuration(sec) {
        sec = Number(sec) || 0;
        if (sec <= 0)
            return "";
        if (sec < 60)
            return sec + "s";
        var m = Math.floor(sec / 60);
        var s = sec % 60;
        return s > 0 ? (m + "m " + s + "s") : (m + "m");
    }

    component GhostButton: Button {
        id: gbtn
        property color accent: root.clrAccent
        Layout.preferredHeight: 32
        padding: 10
        contentItem: Text {
            text: gbtn.text
            font.family: root.fontUi
            font.pointSize: 10
            color: !gbtn.enabled ? root.clrTextFaint
                  : (gbtn.hovered || gbtn.down ? "#ffffff" : gbtn.accent)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 5
            color: gbtn.down ? root.clrElevated
                  : (gbtn.hovered && gbtn.enabled ? root.clrElevated : "transparent")
            border.color: gbtn.hovered && gbtn.enabled ? gbtn.accent : root.clrBorder
            border.width: 1
        }
    }

    FileDialog {
        id: openDialog
        title: "Open quest"
        nameFilters: ["QuestForth scripts (*.forth *.fth)", "All files (*.*)"]
        onAccepted: {
            if (gameController)
                gameController.loadQuestFile(selectedFile);
        }
    }

    Shortcut { sequence: StandardKey.Open; onActivated: openDialog.open() }
    Shortcut { sequence: StandardKey.Quit; onActivated: Qt.quit() }
    Shortcut {
        sequence: "Escape"
        enabled: gameController && gameController.mainMenuVisible && root.menuTab === "history"
        onActivated: root.menuTab = "quests"
    }

    // ── Play view ──────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        visible: gameController && !gameController.mainMenuVisible

        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 44
            color: root.clrSurface

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: root.clrBorderSoft
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                spacing: 12

                GhostButton {
                    text: "Quests"
                    onClicked: {
                        root.menuTab = "quests";
                        if (gameController)
                            gameController.returnToMainMenu();
                    }
                }

                Rectangle {
                    width: 1
                    Layout.preferredHeight: 18
                    color: root.clrBorder
                }

                Text {
                    text: gameController ? gameController.questTitle : ""
                    color: root.clrBrand
                    font.family: root.fontUi
                    font.bold: true
                    font.pointSize: 12
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    visible: gameController && gameController.currentLocation.length > 0
                    text: gameController ? gameController.currentLocation : ""
                    color: root.clrAccent
                    font.family: root.fontUi
                    font.pointSize: 10
                    elide: Text.ElideRight
                    Layout.maximumWidth: 220
                }

                GhostButton {
                    text: "Open…"
                    accent: root.clrTextDim
                    onClicked: openDialog.open()
                }
            }
        }

        // Workspace
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 18
            spacing: 18

            // Story column — must keep stretch so TextArea wrap cannot shrink it
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 360
                Layout.preferredWidth: 640
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "Story"
                        color: root.clrTextDim
                        font.family: root.fontUi
                        font.pointSize: 10
                        font.bold: true
                    }
                    Item { Layout.fillWidth: true }
                }

                // Event log — readable prose, not monospace
                Rectangle {
                    id: logFrame
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: gameController && gameController.canvasVisible
                                           ? parent.height * 0.42 : -1
                    Layout.minimumWidth: 0
                    color: root.clrSurface
                    radius: 8
                    border.color: root.clrBorderSoft
                    border.width: 1
                    clip: true

                    ScrollView {
                        id: logScrollView
                        anchors.fill: parent
                        anchors.margins: 2
                        clip: true
                        contentWidth: availableWidth
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                        TextArea {
                            id: logArea
                            // Pin width to viewport — wrap must not shrink the Story column
                            width: logScrollView.availableWidth
                            text: gameController ? gameController.logText : ""
                            readOnly: true
                            wrapMode: TextArea.Wrap
                            selectByMouse: true
                            color: root.clrStory
                            font.family: root.fontStory
                            font.pointSize: 13
                            font.preferShaping: true
                            leftPadding: 18
                            rightPadding: 18
                            topPadding: 16
                            bottomPadding: 16
                            background: Item {}

                            onTextChanged: cursorPosition = length
                        }
                    }
                }

                // Canvas
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 180
                    Layout.fillHeight: gameController && gameController.canvasVisible
                    visible: gameController && gameController.canvasVisible
                    color: "#0c0e13"
                    border.color: root.clrBorderSoft
                    border.width: 1
                    radius: 8
                    clip: true

                    Canvas {
                        id: questCanvas
                        anchors.fill: parent
                        anchors.margins: 6
                        property var drawColor: root.clrAccent
                        property var ops: gameController ? gameController.canvasOps : []
                        onOpsChanged: requestPaint()
                        onWidthChanged: requestPaint()
                        onHeightChanged: requestPaint()

                        onPaint: {
                            var ctx = getContext("2d");
                            ctx.reset();
                            ctx.fillStyle = "#0c0e13";
                            ctx.fillRect(0, 0, width, height);
                            drawColor = "#6cb2e3";
                            ctx.strokeStyle = drawColor;
                            ctx.fillStyle = drawColor;
                            ctx.lineWidth = 2;
                            ctx.font = "12px sans-serif";
                            for (var i = 0; i < ops.length; ++i) {
                                var line = ops[i];
                                var parts = line.split(" ");
                                if (parts[0] === "CLEAR") {
                                    ctx.fillStyle = "#0c0e13";
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
                                    ctx.fillText(line.substring(prefix.length, prefix.length + tlen), tx, ty);
                                }
                            }
                        }
                    }
                }

                // Choices
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    visible: gameController && gameController.choices.length > 0

                    Text {
                        text: "What do you do?"
                        color: root.clrTextDim
                        font.family: root.fontUi
                        font.pointSize: 10
                        font.bold: true
                    }

                    Repeater {
                        model: gameController ? gameController.choices : []

                        Button {
                            id: choiceBtn
                            required property string modelData
                            required property int index
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                acceptedButtons: Qt.NoButton
                            }
                            onClicked: gameController.makeChoice(index)

                            contentItem: RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 14
                                anchors.rightMargin: 14
                                spacing: 10
                                Text {
                                    text: String(choiceBtn.index + 1)
                                    color: root.clrAccent
                                    font.family: root.fontUi
                                    font.bold: true
                                    font.pointSize: 11
                                    Layout.preferredWidth: 16
                                }
                                Text {
                                    text: choiceBtn.modelData
                                    color: choiceBtn.hovered || choiceBtn.down ? "#ffffff" : root.clrText
                                    font.family: root.fontUi
                                    font.pointSize: 11
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                            }
                            background: Rectangle {
                                radius: 6
                                color: choiceBtn.down ? root.clrElevated
                                      : (choiceBtn.hovered ? "#2a3040" : root.clrSurface)
                                border.color: choiceBtn.hovered ? root.clrAccent : root.clrBorder
                                border.width: 1
                            }
                        }
                    }
                }
            }

            // Inventory — fixed side panel, never steals Story width
            ColumnLayout {
                Layout.preferredWidth: 300
                Layout.maximumWidth: 300
                Layout.minimumWidth: 200
                Layout.fillWidth: false
                Layout.fillHeight: true
                spacing: 10

                Text {
                    text: "Inventory"
                    color: root.clrTextDim
                    font.family: root.fontUi
                    font.pointSize: 10
                    font.bold: true
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: root.clrSurface
                    border.color: root.clrBorderSoft
                    border.width: 1
                    radius: 8

                    ListView {
                        id: invList
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 4
                        clip: true
                        model: gameController ? gameController.inventory : []

                        delegate: Rectangle {
                            width: invList.width
                            height: 36
                            radius: 5
                            color: root.clrElevated
                            required property string modelData
                            readonly property var parts: modelData.split(" - ")
                            readonly property string itemName: parts[0] ? parts[0] : modelData
                            readonly property string itemCount: parts[1] ? parts[1] : ""

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 10
                                spacing: 8
                                Text {
                                    text: itemName
                                    color: root.clrText
                                    font.family: root.fontUi
                                    font.pointSize: 10
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                                Text {
                                    visible: itemCount.length > 0
                                    text: "×" + itemCount
                                    color: root.clrAccent
                                    font.family: root.fontUi
                                    font.bold: true
                                    font.pointSize: 10
                                }
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "Empty"
                            color: root.clrTextFaint
                            font.family: root.fontUi
                            font.pointSize: 10
                            visible: invList.count === 0
                        }
                    }
                }
            }
        }

        // Status bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 26
            color: root.clrSurface

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: root.clrBorderSoft
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                spacing: 10

                Text {
                    text: {
                        if (!gameController)
                            return "";
                        var a = gameController.questAuthor.length > 0
                                ? gameController.questAuthor : "Unknown author";
                        var v = gameController.questVersion.length > 0
                                ? gameController.questVersion : "1.0";
                        return a + "  ·  v" + v;
                    }
                    color: root.clrTextFaint
                    font.family: root.fontUi
                    font.pointSize: 9
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: "QuestForth"
                    color: root.clrTextFaint
                    font.family: root.fontUi
                    font.pointSize: 9
                }
            }
        }
    }

    // ── End overlay ────────────────────────────────────────────
    Rectangle {
        id: endOverlay
        anchors.fill: parent
        color: "#cc0e1016"
        z: 100
        visible: opacity > 0
        opacity: gameController && gameController.isGameOver ? 1.0 : 0.0
        Behavior on opacity { NumberAnimation { duration: 280 } }

        readonly property bool isWin: gameController && gameController.gameOutcome === "victory"
        readonly property color themeColor: isWin ? root.clrWin : root.clrLoss

        MouseArea { anchors.fill: parent }

        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width - 48, 460)
            implicitHeight: endCol.implicitHeight + 48
            color: root.clrElevated
            border.color: endOverlay.themeColor
            border.width: 1
            radius: 10

            ColumnLayout {
                id: endCol
                anchors.fill: parent
                anchors.margins: 28
                spacing: 14

                Text {
                    text: endOverlay.isWin ? "Quest complete" : "Quest failed"
                    color: endOverlay.themeColor
                    font.family: root.fontUi
                    font.bold: true
                    font.pointSize: 16
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: gameController ? gameController.endMessage : ""
                    color: root.clrStory
                    font.family: root.fontStory
                    font.pointSize: 14
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                    lineHeight: 1.45
                }

                Item { Layout.preferredHeight: 6 }

                Button {
                    id: restartBtn
                    text: endOverlay.isWin ? "Play again" : "Try again"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    onClicked: if (gameController) gameController.restartQuest()
                    contentItem: Text {
                        text: restartBtn.text
                        font.family: root.fontUi
                        font.pointSize: 11
                        font.bold: true
                        color: restartBtn.hovered || restartBtn.down ? "#ffffff" : endOverlay.themeColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        radius: 6
                        color: restartBtn.hovered || restartBtn.down ? root.clrElevated : "transparent"
                        border.color: endOverlay.themeColor
                        border.width: 1
                    }
                }

                GhostButton {
                    text: gameController && gameController.hasCatalog ? "Back to quests" : "Open another…"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    onClicked: {
                        if (gameController && gameController.hasCatalog) {
                            root.menuTab = "quests";
                            gameController.returnToMainMenu();
                        } else {
                            openDialog.open();
                        }
                    }
                }
            }
        }
    }

    // ── Start screen ───────────────────────────────────────────
    Rectangle {
        id: mainMenu
        anchors.fill: parent
        color: root.clrBg
        visible: opacity > 0
        opacity: gameController && gameController.mainMenuVisible ? 1.0 : 0.0
        Behavior on opacity { NumberAnimation { duration: 160 } }

        // Soft top accent line
        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 2
            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 0.35; color: root.clrBrand }
                GradientStop { position: 0.65; color: root.clrAccent }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 32
            anchors.topMargin: 36
            spacing: 18

            RowLayout {
                Layout.fillWidth: true
                spacing: 16

                ColumnLayout {
                    spacing: 4
                    Layout.fillWidth: true
                    Text {
                        text: "QuestForth"
                        color: root.clrBrand
                        font.family: root.fontUi
                        font.bold: true
                        font.pointSize: 22
                    }
                    Text {
                        text: root.menuTab === "history"
                              ? "Past playthroughs and outcomes"
                              : "Select a quest from the catalog"
                        color: root.clrTextDim
                        font.family: root.fontUi
                        font.pointSize: 11
                    }
                }

                GhostButton {
                    text: "Open file…"
                    accent: root.clrText
                    Layout.preferredHeight: 34
                    onClicked: openDialog.open()
                }
            }

            // Stats
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                Layout.maximumHeight: 28
                spacing: 20
                visible: gameController && gameController.playStats.plays > 0

                Repeater {
                    model: gameController ? [
                        { label: "plays", value: gameController.playStats.plays, color: root.clrAccent },
                        { label: "wins", value: gameController.playStats.victories, color: root.clrWin },
                        { label: "losses", value: gameController.playStats.defeats, color: root.clrLoss },
                        { label: "win rate", value: gameController.playStats.winRate + "%", color: root.clrBrand }
                    ] : []

                    Row {
                        required property var modelData
                        spacing: 6
                        Text {
                            text: String(modelData.value)
                            color: modelData.color
                            font.family: root.fontUi
                            font.bold: true
                            font.pointSize: 13
                        }
                        Text {
                            text: modelData.label
                            color: root.clrTextFaint
                            font.family: root.fontUi
                            font.pointSize: 10
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
                Item { Layout.fillWidth: true }
                Text {
                    visible: gameController && gameController.playStats.uniqueQuests > 0
                    text: gameController.playStats.uniqueQuests
                          + (gameController.playStats.uniqueQuests === 1 ? " quest tried" : " quests tried")
                    color: root.clrTextFaint
                    font.family: root.fontUi
                    font.pointSize: 10
                }
            }

            // Tabs
            RowLayout {
                Layout.fillWidth: true
                spacing: 4

                Repeater {
                    model: [
                        { id: "quests", label: "Quests" },
                        { id: "history", label: "History" }
                    ]
                    Button {
                        id: tabBtn
                        required property var modelData
                        text: modelData.label
                        Layout.preferredHeight: 32
                        Layout.preferredWidth: 96
                        checkable: true
                        checked: root.menuTab === modelData.id
                        onClicked: root.menuTab = modelData.id
                        contentItem: Text {
                            text: tabBtn.text
                            font.family: root.fontUi
                            font.pointSize: 11
                            font.bold: tabBtn.checked
                            color: tabBtn.checked ? root.clrAccent
                                  : (tabBtn.hovered ? root.clrText : root.clrTextFaint)
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Item {
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 2
                                color: tabBtn.checked ? root.clrAccent : "transparent"
                            }
                        }
                    }
                }
                Item { Layout.fillWidth: true }
                Button {
                    text: "Clear history"
                    visible: root.menuTab === "history"
                    enabled: gameController && gameController.playHistory.length > 0
                    Layout.preferredHeight: 28
                    onClicked: if (gameController) gameController.clearPlayHistory()
                    contentItem: Text {
                        text: parent.text
                        font.family: root.fontUi
                        font.pointSize: 9
                        color: parent.enabled
                               ? (parent.hovered ? root.clrLoss : "#a86868")
                               : root.clrTextFaint
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Item {}
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: root.clrBorderSoft
            }

            ListView {
                id: catalogList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                spacing: 6
                visible: root.menuTab === "quests"
                model: gameController ? gameController.catalogQuests : []

                delegate: Button {
                    id: questBtn
                    required property var modelData
                    width: catalogList.width
                    height: 58
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        acceptedButtons: Qt.NoButton
                    }
                    onClicked: if (gameController) gameController.loadQuestPath(modelData.path)

                    contentItem: RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        spacing: 14
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 3
                            Text {
                                text: questBtn.modelData.category || "Quest"
                                color: root.clrAccent
                                font.family: root.fontUi
                                font.pointSize: 9
                                font.bold: true
                            }
                            Text {
                                text: questBtn.modelData.title
                                color: questBtn.hovered ? "#ffffff" : root.clrText
                                font.family: root.fontUi
                                font.pointSize: 13
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }
                        Text {
                            visible: questBtn.modelData.plays > 0
                            text: questBtn.modelData.victories + " / " + questBtn.modelData.plays + " won"
                            color: root.clrTextFaint
                            font.family: root.fontUi
                            font.pointSize: 10
                        }
                    }
                    background: Rectangle {
                        radius: 7
                        color: questBtn.down ? root.clrElevated
                              : (questBtn.hovered ? "#252a36" : root.clrSurface)
                        border.color: questBtn.hovered ? root.clrAccent : root.clrBorderSoft
                        border.width: 1
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: catalogList.count === 0
                    text: "No quests in the catalog.\nOpen a .forth file, or add scripts to the quests folder."
                    color: root.clrTextFaint
                    font.family: root.fontUi
                    font.pointSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    width: parent.width - 48
                }
            }

            ListView {
                id: historyList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                spacing: 6
                visible: root.menuTab === "history"
                model: gameController ? gameController.playHistory : []

                delegate: Rectangle {
                    id: histRow
                    required property var modelData
                    width: historyList.width
                    height: 54
                    radius: 7
                    color: root.clrSurface
                    border.color: root.clrBorderSoft
                    border.width: 1
                    readonly property bool isWin: modelData.outcome === "victory"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.rightMargin: 16
                        spacing: 12
                        Rectangle {
                            width: 3
                            Layout.fillHeight: true
                            Layout.topMargin: 12
                            Layout.bottomMargin: 12
                            radius: 1
                            color: histRow.isWin ? root.clrWin : root.clrLoss
                        }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 3
                            Text {
                                text: histRow.modelData.title
                                color: root.clrText
                                font.family: root.fontUi
                                font.pointSize: 12
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                            Text {
                                text: root.formatWhen(histRow.modelData.timestamp)
                                      + (histRow.modelData.durationSec > 0
                                         ? ("  ·  " + root.formatDuration(histRow.modelData.durationSec))
                                         : "")
                                color: root.clrTextFaint
                                font.family: root.fontUi
                                font.pointSize: 9
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }
                        Text {
                            text: histRow.isWin ? "Victory" : "Defeat"
                            color: histRow.isWin ? root.clrWin : root.clrLoss
                            font.family: root.fontUi
                            font.pointSize: 10
                            font.bold: true
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: historyList.count === 0
                    text: "No completed plays yet.\nFinish a quest to see it listed here."
                    color: root.clrTextFaint
                    font.family: root.fontUi
                    font.pointSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    width: parent.width - 48
                }
            }
        }
    }
}
