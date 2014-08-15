var recompressImage = {
    change: function(imagePath, compressionLvl, successCallback, errorCallback) {
        cordova.exec(
            successCallback, // success callback function
            errorCallback, // error callback function
            'RecompressImage', // mapped to our native Java class called "reconvertImagePlugin"
            'recompressImage', // with this action name
            [{                  // and this array of custom arguments
                "imagePath": imagePath,
                "compressionLvl": compressionLvl
            }]
        ); 
    }
}
module.exports = recompressImage;