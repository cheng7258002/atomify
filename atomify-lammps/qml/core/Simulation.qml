import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2
import Atomify 1.0

QtObject {
    property string name: "Unnamed"
    property string description: "No description"
    property string longDescription: "No description"
    property string folder: ""
    property url scriptSource: (folder && identifier) ? folder + "/" + identifier + ".in" : ""
    property url screenshotSource: (folder && identifier) ? folder + "/" + identifier + ".png" : ""
    property real pan: -10
    property real tilt: -20
    property real zoom: -20
    property string identifier: {
        var names = folder.split("/")
        if(folder === "") {
            return ""
        }

        return names[names.length - 1]
    }

    property list<Component> controllers

    onDescriptionChanged: {
        var maximumCharacters = 210
        if(description.length > maximumCharacters) {
            console.warn("WARNING: Number of characters in description for " +
                         name + " is longer than the recommended maximum of " +
                         maximumCharacters + ".")
        }
    }
}
