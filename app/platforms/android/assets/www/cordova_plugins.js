cordova.define('cordova/plugin_list', function(require, exports, module) {
module.exports = [
  {
    "id": "cordova-plugin-bluetoothle.BluetoothLe",
    "file": "plugins/cordova-plugin-bluetoothle/www/bluetoothle.js",
    "pluginId": "cordova-plugin-bluetoothle",
    "clobbers": [
      "window.bluetoothle"
    ]
  }
];
module.exports.metadata = 
// TOP OF METADATA
{
  "cordova-plugin-bluetoothle": "4.4.2",
  "cordova-plugin-whitelist": "1.3.2"
};
// BOTTOM OF METADATA
});